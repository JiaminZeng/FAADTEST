#include "haac.h"

#include <fcntl.h>
#include <getopt.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdint>

#include "audio.h"
#include "codebook/hcb_en.h"
#include "faad.h"
#include "stego.h"

#define MAX_CHANNELS 6
static int quiet = 1;

/* FAAD file buffering routines */
typedef struct {
    long bytes_into_buffer;  // 在缓存中剩余的字节数目
    long bytes_consumed;     // 单次的字节消耗
    long file_offset;        // 文件的总字节消耗
    unsigned char *buffer;
    int at_eof;
    FILE *infile;
} aac_buffer;

static int fill_buffer(aac_buffer *b) {
    int bread;

    if (b->bytes_consumed > 0) {  // 如果字节的消耗大于0，
        if (b->bytes_into_buffer) {  // 如果buffer中还有字节剩余，将其移动到前面去
            memmove((void *) b->buffer, (void *) (b->buffer + b->bytes_consumed),
                    b->bytes_into_buffer * sizeof(unsigned char));
        }

        if (!b->at_eof) {  // 如果文件还有，读取消耗数量的字节进去
            bread = fread((void *) (b->buffer + b->bytes_into_buffer), 1,
                          b->bytes_consumed, b->infile);
            if (bread != b->bytes_consumed) b->at_eof = 1;

            b->bytes_into_buffer += bread;
        }

        b->bytes_consumed = 0;

        if (b->bytes_into_buffer > 3) {  // 特殊字段全部消耗
            if (memcmp(b->buffer, "TAG", 3) == 0) b->bytes_into_buffer = 0;
        }
        if (b->bytes_into_buffer > 11) {  // 特殊字段全部消耗
            if (memcmp(b->buffer, "LYRICSBEGIN", 11) == 0) b->bytes_into_buffer = 0;
        }
        if (b->bytes_into_buffer > 8) {  // 特殊字段全部消耗
            if (memcmp(b->buffer, "APETAGEX", 8) == 0) b->bytes_into_buffer = 0;
        }
    }

    return 1;
}

static void advance_buffer(aac_buffer *b, int bytes) {
    b->file_offset += bytes;
    b->bytes_consumed = bytes;
    b->bytes_into_buffer -= bytes;
    if (b->bytes_into_buffer < 0) b->bytes_into_buffer = 0;
}

static int adts_sample_rates[] = {96000, 88200, 64000, 48000, 44100, 32000,
                                  24000, 22050, 16000, 12000, 11025, 8000,
                                  7350, 0, 0, 0};

// 目的在于计算去除ID部分的总长度
static int adts_parse(aac_buffer *b, int *bitrate, float *length) {
    int frames, frame_length;
    int t_framelength = 0;
    int sample_rate;
    float frames_per_sec, bytes_per_frame;

    /* Read all frames to ensure correct time and bitrate */
    for (frames = 0; /* */; frames++) {
        fill_buffer(b);

        if (b->bytes_into_buffer > 7) {
            /* check syncword */
            // 检查同步字
            if (!((b->buffer[0] == 0xFF) && ((b->buffer[1] & 0xF6) == 0xF0))) break;

            // 在第一帧的时候确定采样率
            if (frames == 0)
                sample_rate = adts_sample_rates[(b->buffer[2] & 0x3c) >> 2];

            // 获取帧长度
            frame_length = ((((unsigned int) b->buffer[3] & 0x3)) << 11) |
                           (((unsigned int) b->buffer[4]) << 3) | (b->buffer[5] >> 5);

            // 总长度
            t_framelength += frame_length;

            // 如果帧长度比当前的长度还长，退出进行填充
            if (frame_length > b->bytes_into_buffer) break;

            advance_buffer(b, frame_length);
        } else {
            break;
        }
    }

    frames_per_sec = (float) sample_rate / 1024.0f;
    if (frames != 0)
        bytes_per_frame = (float) t_framelength / (float) (frames * 1000);
    else
        bytes_per_frame = 0;
    *bitrate = (int) (8. * bytes_per_frame * frames_per_sec + 0.5);
    if (frames_per_sec != 0)
        *length = (float) frames / frames_per_sec;
    else
        *length = 1;
    return 1;
}

uint32_t read_callback(void *user_data, void *buffer, uint32_t length) {
    return fread(buffer, 1, length, (FILE *) user_data);
}

uint32_t seek_callback(void *user_data, uint64_t position) {
    return fseek((FILE *) user_data, position, SEEK_SET);
}

// ZJM
static int decodeAACfile(char *aacfile, StegaCxtData *sp) {
    int tagsize;
    unsigned long sample_rate;
    unsigned char channels;
    void *sample_buffer;

    NeAACDecHandle hDecoder;
    NeAACDecFrameInfo frameInfo;
    NeAACDecConfigurationPtr config;

    char percents[200];
    int percent, old_percent = -1;
    int bread, fileread;
    int header_type = 0;
    int bitrate = 0;
    int first_time = 1;
    float length = 0;

    aac_buffer b;

    memset(&b, 0, sizeof(aac_buffer));

    b.infile = fopen(aacfile, "rb");
    if (b.infile == NULL) {
        /* unable to open file */
        fprintf(stderr, "Error opening file: %s\n", aacfile);
        return 1;
    }

    fseek(b.infile, 0, SEEK_END);
    fileread = ftell(b.infile);
    fseek(b.infile, 0, SEEK_SET);

    if (!(b.buffer = (unsigned char *) malloc(FAAD_MIN_STREAMSIZE * MAX_CHANNELS))) {
        fprintf(stderr, "Memory allocation error\n");
        return 0;
    }
    memset(b.buffer, 0, FAAD_MIN_STREAMSIZE * MAX_CHANNELS);

    // 第一次read
    bread = fread(b.buffer, 1, FAAD_MIN_STREAMSIZE * MAX_CHANNELS, b.infile);

    b.bytes_into_buffer = bread;
    b.bytes_consumed = 0;
    b.file_offset = 0;

    if (bread != FAAD_MIN_STREAMSIZE * MAX_CHANNELS) b.at_eof = 1;

    tagsize = 0;
    if (!memcmp(b.buffer, "ID3", 3)) {
        /* high bit is not used */
        tagsize = (b.buffer[6] << 21) | (b.buffer[7] << 14) | (b.buffer[8] << 7) |
                  (b.buffer[9] << 0);

        // ID3+6789，共十个字节
        tagsize += 10;
        // tagsize 为tag部分的总长度，将tag部分读取掉
        advance_buffer(&b, tagsize);
        // 填充满buffer
        fill_buffer(&b);
    }

    hDecoder = NeAACDecOpen();

    /* Set the default object type and sample_rate */
    /* This is useful for RAW AAC files */
    config = NeAACDecGetCurrentConfiguration(hDecoder);
    config->defObjectType = LC;
    config->outputFormat = FAAD_FMT_16BIT;
    config->downMatrix = 0;
    config->useOldADTSFormat = 0;
    // config->dontUpSampleImplicitSBR = 1;
    NeAACDecSetConfiguration(hDecoder, config);

    /* get AAC infos for printing */
    header_type = 0;
    if ((b.buffer[0] == 0xFF) && ((b.buffer[1] & 0xF6) == 0xF0)) {
        // 第一次解析
        adts_parse(&b, &bitrate, &length);

        // 解析完后回到标签的开头
        fseek(b.infile, tagsize, SEEK_SET);

        // 新的开始
        bread = fread(b.buffer, 1, FAAD_MIN_STREAMSIZE * MAX_CHANNELS, b.infile);

        if (bread != FAAD_MIN_STREAMSIZE * MAX_CHANNELS)
            b.at_eof = 1;
        else
            b.at_eof = 0;

        b.bytes_into_buffer = bread;
        b.bytes_consumed = 0;

        // 已经解决的字节数目
        b.file_offset = tagsize;
        header_type = 1;
    } else if (memcmp(b.buffer, "ADIF", 4) == 0) {
        int skip_size = (b.buffer[4] & 0x80) ? 9 : 0;
        bitrate = ((unsigned int) (b.buffer[4 + skip_size] & 0x0F) << 19) |
                  ((unsigned int) b.buffer[5 + skip_size] << 11) |
                  ((unsigned int) b.buffer[6 + skip_size] << 3) |
                  ((unsigned int) b.buffer[7 + skip_size] & 0xE0);

        length = (float) fileread;
        if (length != 0) {
            length = ((float) length * 8.f) / ((float) bitrate) + 0.5f;
        }
        bitrate = (int) ((float) bitrate / 1000.0f + 0.5f);
        header_type = 2;
    }

    // 填充一下
    fill_buffer(&b);
    if ((bread = NeAACDecInit(hDecoder, b.buffer, b.bytes_into_buffer,
                              &sample_rate, &channels, (void *) sp)) < 0) {
        fprintf(stderr, "Error initializing decoder library.\n");
        if (b.buffer) free(b.buffer);
        NeAACDecClose(hDecoder);
        fclose(b.infile);
        return 1;
    }
    // 前进，初始化会进行一定的消耗
    advance_buffer(&b, bread);
    fill_buffer(&b);

    /* print AAC file info */
    fprintf(stderr, "%s file info:\n", aacfile);
    switch (header_type) {
        case 0:
            fprintf(stderr, "RAW\n\n");
            break;
        case 1:  // 都是ADTS，这个必须控制
            fprintf(stderr, "ADTS, %.3f sec, %d kbps, %d Hz\n\n", length,
                    bitrate, sample_rate);
            break;
        case 2:
            fprintf(stderr, "ADIF, %.3f sec, %d kbps, %d Hz\n\n", length,
                    bitrate, sample_rate);
            break;
    }
    do {
        // 每次都肯定大于一帧
        sample_buffer = NeAACDecDecode(hDecoder, &frameInfo, b.buffer,
                                       b.bytes_into_buffer, b.file_offset);
        /* update buffer indices */
        // 更新消耗的
        advance_buffer(&b, frameInfo.bytesconsumed);

        if (frameInfo.error > 0) {
            fprintf(stderr, "Error: %s\n",
                    NeAACDecGetErrorMessage(frameInfo.error));
        }
        /* open the sound file now that the number of channels are known */
        if (first_time && !frameInfo.error) {
            /* print some channel info */
            first_time = 0;
        }
        percent = (int) (b.file_offset * 100) / fileread;
        if (percent > old_percent) {
            old_percent = percent;
            fprintf(stderr, "%s\r", percents);
        }
        fill_buffer(&b);
        if (b.bytes_into_buffer == 0)
            sample_buffer = NULL; /* to make sure it stops now */
    } while (sample_buffer != NULL);
    NeAACDecClose(hDecoder);
    fclose(b.infile);
    if (b.buffer) free(b.buffer);
    return frameInfo.error;
}

static void modify(char *file, int cap, uint8_t *msg, int *bitidx, int *bitsinfo) {
    FILE *sp = fopen(file, "rb+");
    uint8_t byte;
    for (int i = 0; i < cap; i++) {
        int bytes_pos = bitidx[i] >> 3;
        int left = bitidx[i] & 7;
        int mid_right = 7 - left;
        fseek(sp, bytes_pos, SEEK_SET);
        if (bitsinfo[i] == 1) {
            fread(&byte, 1UL, 1, sp);
            fseek(sp, -1, SEEK_CUR);
            // 2 5

            // 11100000 >> 111
            // 00000001
            if (((byte >> mid_right) & 1) != msg[i]) {
                byte ^= (1 << mid_right);
            }
            fwrite(&byte, 1UL, 1, sp);

        } else {
            int oldword = bitsinfo[i] / 100;
            int cb = bitsinfo[i] % 100 / 10;

            int z = oldword;
            int spe[4] = {0};

            if (cb <= 2) {
                for (int j = 3; j >= 0; j--) {
                    spe[j] = oldword % 3 - 1;
                    oldword /= 3;
                }
            } else {
                spe[0] = oldword / 9 - 4;
                spe[1] = oldword % 9 - 4;
            }

            while (1) {
                int idx = bitsinfo[i] % 10;  // 获取idx
                if (spe[idx] < 0 && msg[i] == 0 || spe[idx] > 0 && msg[i] == 1) {
                    spe[idx] = -spe[idx];  // 检查是否翻转
                }
                if ((i + 1 < cap && bitidx[i + 1] == bitidx[i]))
                    i++;
                else
                    break;
            }

            int newword = 0;
            if (cb <= 2) {  // 计算新的offset
                for (int j = 0; j < 4; j++) {
                    newword = newword * 3 + spe[j];
                }
            } else {
                newword = spe[0] * 9 + spe[1];
            }
            newword += 40;

            int totbits = 0;
            int val = 0;

            int tb = 0;
            // 码书选择
            switch (cb) {  // 获取bit的数目和对应的值
                case 1:
                    tb = huff1[z][0];
                    totbits = huff1[newword][0];
                    val = huff1[newword][1];
                    break;
                case 2:
                    tb = huff2[z][0];
                    totbits = huff2[newword][0];
                    val = huff2[newword][1];
                    break;
                case 5:
                    tb = huff5[z][0];
                    totbits = huff5[newword][0];
                    val = huff5[newword][1];
                    break;
                case 6:
                    tb = huff6[z][0];
                    totbits = huff6[newword][0];
                    val = huff6[newword][1];
                    break;
                default:
                    break;
            }

            // 写入流程
            while (totbits) {            // 如果还没有写完
                fread(&byte, 1UL, 1, sp);  // 读取一个byte
                fseek(sp, -1, SEEK_CUR);   // 退回一个byte
                int enable = 8 - left;
                if (totbits >= enable) {
                    // 保留左边left个                 保留左边enable
                    byte = ((byte >> enable) << enable) | (val >> (totbits - enable));
                    // 减去左边enable
                    val -= ((val >> (totbits - enable)) << (totbits - enable));
                    totbits -= enable;
                } else {
                    int mid = totbits;
                    int right = enable - mid;
                    // 保留左边left个                   // 保留右边right个
                    byte = ((byte >> enable) << enable) | (byte & ((1 << right) - 1)) |
                           (val << right);
                    totbits = 0;
                }
                fwrite(&byte, 1UL, 1, sp);
                left = 0;
            }
        }
    }
    fclose(sp);
}

int haac_get_capacity(char *in_audio, int *logic_bit_cap, int threshold, int part_mode, int payload) {
    FAADStegaCxtData *sp = mp3_stega_init(FAADStegaMethod::HAAD);

}

int get_capacity(char *in_audio, int area, int threshold) {
    int cap = 50000000;  // MAX
    StegaCxtData *sp =
            initStegaData(cap, area, threshold, NULL, NULL, NULL, NULL);
    decodeAACfile(in_audio, sp);
    cap = sp->cur_inx;
    closeStegaData(sp);
    return cap;
}

int get_cost(char *in_audio, int cap, int area, int threshold, uint8_t *msg,
             int *bitidx, int *bitsinfo, double *cost) {
    StegaCxtData *sp =
            initStegaData(cap, area, threshold, msg, bitidx, bitsinfo, cost);
    decodeAACfile(in_audio, sp);
    cap = sp->cur_inx;
    closeStegaData(sp);
    return cap;
}

int embed(char *in_audio, char *out_audio, int cap, int area, int threshold,
          uint8_t *msg, int *bitidx, int *bitsinfo) {
    FILE *input = fopen(in_audio, "rb");
    FILE *output = fopen(out_audio, "wb+");

    if (input == NULL || output == NULL) {
        exit(-1);
    }

    int bread = 0;
    int bwrite = 0;
    int buffsize = 20480;
    uint8_t *buf = (uint8_t *) malloc(buffsize * sizeof(uint8_t));
    while ((bread = fread(buf, sizeof(uint8_t), buffsize, input)) != 0) {
        bwrite = fwrite(buf, sizeof(uint8_t), bread, output);
        if (bread != bwrite) exit(-1);
    }
    free(buf);
    fclose(input);
    fclose(output);

    FILE *sp = fopen(out_audio, "rb+");

    uint8_t byte;
    for (int i = 0; i < cap; i++) {
        int bytes_pos = bitidx[i] >> 3;
        int left = bitidx[i] & 7;
        int mid_right = 7 - left;
        fseek(sp, bytes_pos, SEEK_SET);
        if (bitsinfo[i] == 1) {
            fread(&byte, 1UL, 1, sp);
            fseek(sp, -1, SEEK_CUR);
            if (((byte >> mid_right) & 1) != msg[i]) {
                byte ^= (1 << mid_right);
            }
            fwrite(&byte, 1UL, 1, sp);
        } else {
            int oldword = bitsinfo[i] / 1000;
            int cb = bitsinfo[i] % 1000 / 10;
            int z = oldword;
            int spe[4] = {0};

            if (cb <= 2) {
                for (int j = 3; j >= 0; j--) {
                    spe[j] = oldword % 3 - 1;
                    oldword /= 3;
                }
            } else {
                spe[0] = oldword / 9 - 4;
                spe[1] = oldword % 9 - 4;
            }

            while (1) {
                int idx = bitsinfo[i] % 10;  // 获取idx
                if (spe[idx] < 0 && msg[i] == 0 || spe[idx] > 0 && msg[i] == 1) {
                    spe[idx] = -spe[idx];  // 检查是否翻转
                }
                if ((i + 1 < cap && bitidx[i + 1] == bitidx[i]))
                    i++;
                else
                    break;
            }

            int newword = 0;
            if (cb <= 2) {  // 计算新的offset
                for (int j = 0; j < 4; j++) {
                    newword = newword * 3 + spe[j];
                }
            } else {
                newword = spe[0] * 9 + spe[1];
            }
            newword += 40;

            int totbits = 0;
            int val = 0;

            // 码书选择
            switch (cb) {  // 获取bit的数目和对应的值
                case 1:
                    totbits = huff1[newword][0];
                    val = huff1[newword][1];
                    break;
                case 2:
                    totbits = huff2[newword][0];
                    val = huff2[newword][1];
                    break;
                case 5:
                    totbits = huff5[newword][0];
                    val = huff5[newword][1];
                    break;
                case 6:
                    totbits = huff6[newword][0];
                    val = huff6[newword][1];
                    break;
                default:
                    break;
            }

            // 写入流程
            while (totbits) {            // 如果还没有写完
                fread(&byte, 1UL, 1, sp);  // 读取一个byte
                fseek(sp, -1, SEEK_CUR);   // 退回一个byte
                int enable = 8 - left;
                if (totbits >= enable) {
                    // 保留左边left个                 保留左边enable
                    byte = ((byte >> enable) << enable) | (val >> (totbits - enable));
                    // 减去左边enable
                    val -= ((val >> (totbits - enable)) << (totbits - enable));
                    totbits -= enable;
                } else {
                    int mid = totbits;
                    int right = enable - mid;
                    // 保留左边left个                   // 保留右边right个
                    byte = ((byte >> enable) << enable) | (byte & ((1 << right) - 1)) |
                           (val << right);
                    totbits = 0;
                }
                fwrite(&byte, 1UL, 1, sp);
                left = 0;
            }
        }
    }
    fclose(sp);
    return 0;
}

int extract(char *in_audio, int cap, int area, int threshold, uint8_t *msg) {
    StegaCxtData *sp = initStegaData(cap, area, threshold, msg, NULL, NULL, NULL);
    decodeAACfile(in_audio, sp);
    cap = sp->cur_inx;
    closeStegaData(sp);
    return cap;
}


//
//int main() {
//  // char aacFileName[255] = "1.aac";
//
//  // // thoreshold = 2, 2766030
//  // int cap = 2766030;
//  // uint8_t *msg = (uint8_t *)malloc(sizeof(uint8_t) * cap);
//  // int *bitidx = (int *)malloc(sizeof(int) * cap);
//  // int *bitsinfo = (int *)malloc(sizeof(int) * cap);
//  // double *cost = (double *)malloc(sizeof(double) * cap);
//
//  // StegaCxtData *sp = initStegaData(cap, 3, 1, msg, bitidx, bitsinfo, cost);
//  // // StegaCxtData *sp = initStegaData(cap, 3, 2, NULL, NULL, NULL, NULL);
//  // decodeAACfile(aacFileName, sp);
//
//  // printf("#### %d\n", sp->cur_inx);
//
//  // for (int i = 0; i < sp->cur_inx; i++) {
//  //   msg[i] = i * (i + 3) % 2;
//  // }
//  // closeStegaData(sp);
//
//  // modify("2.aac", cap, msg, bitidx, bitsinfo);
//
//  // sp = initStegaData(cap, 3, 1, msg, bitidx, bitsinfo, cost);
//
//  // decodeAACfile("2.aac", sp);
//
//  // int num = 0;
//  // for (int i = 0; i < sp->cur_inx; i++) {
//  //   if (msg[i] != i * (i + 3) % 2) num++;
//  // }
//  // printf("#### %d\n", num);
//  // printf("#### %d\n", sp->cur_inx);
//  // printf("\n");
//
//  // closeStegaData(sp);
//
//  // free(bitidx);
//  // free(bitsinfo);
//  // free(cost);
//  // free(msg);
//
//  char in_audio[] = "1.aac";
//  char out_audio[] = "out.aac";
//  int area = 3;
//  int threshold = 1;
//  int cap = get_capacity(in_audio, area, threshold);
//
//  uint8_t *msg = (uint8_t *)malloc(sizeof(uint8_t) * cap);
//  uint8_t *base_msg = (uint8_t *)malloc(sizeof(uint8_t) * cap);
//
//  int *bitidx = (int *)malloc(sizeof(int) * cap);
//  int *bitsinfo = (int *)malloc(sizeof(int) * cap);
//  double *cost = (double *)malloc(sizeof(double) * cap);
//
//  int c1 =
//      get_cost(in_audio, cap, area, threshold, msg, bitidx, bitsinfo, cost);
//
//  for (int i = 0; i < cap; i++) msg[i] = i % 2;
//  int c2 =
//      embed(in_audio, out_audio, cap, area, threshold, msg, bitidx, bitsinfo);
//
//  int c3 = extract(out_audio, cap, area, threshold, base_msg);
//
//  int num = 0;
//  for (int i = 0; i < cap; i++) {
//    if (msg[i] != base_msg[i]) num++;
//  }
//  printf("%d %d %d\n", c1, c2, c3);
//
//  printf("%d\n", num);
//
//  return 0;
//}
