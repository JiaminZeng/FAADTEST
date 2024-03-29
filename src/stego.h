#ifndef FAAD_SRC_STEGO_H
#define FAAD_SRC_STEGO_H

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned char uint8_t;

enum FAADStegoMethod {
    HAAC,
};

enum FAADStegoStep {
    EMBED,
    EXTRACT,
    GET_CAP,
    GET_COST,
};

typedef struct {
    enum FAADStegoMethod METHOD;
    enum FAADStegoStep STEP;

    int FINISH;

    unsigned int file_offset;  // 当前的文件的偏移位置

    // 过程变量
    uint8_t cur_byte;  // 当前处理的字节
    unsigned int cur_bit_idx;  // 处理到当前字节的第几个bit
    unsigned int cur_byte_idx;  // 当前字节是第几个

    unsigned int frame_idx;  // 颗粒的编号
    unsigned int logic_bit_idx;  // 理论可用嵌入bit位置index
    unsigned int useful_bit_idx;  // 嵌入率控制后的嵌入位置index

    // 容量
    unsigned int cap_bits;  // 处理的比特数目上限
    unsigned int cap_bytes;  // 处理的字节数目上限
    unsigned int payload;  // 嵌入率，test版方案，预设是1-10, 每n个比特{x_1,x_2...x_n}嵌入一个信息，

    // 消息代价
    uint8_t *msg;  // 提取或者嵌入的msg，使用字节为单位
    double *cost;  // 提取出的代价，其长度大小应该为msg的8倍

    // 总信息
    unsigned int emb_bit_idx;  // 内容汇集时候当前索引
    unsigned int *bitidx;  // 在当前buffer的bit位置
    uint8_t *type; // 码书类型,1（1，2，5，6）,2（3，4，7，8，9，10）,5（11）
    int *bitsinfo;  // 比特信息 (word*1000 + cb*10 +idx)*(-1)^msg.

    // 帧信息采集，由于是半解码，所以需要将信息抽离出来再进行修改。
    // 该部分为每帧的信息，每帧完成后再拼接至总信息当中
    unsigned int tnum;        // 数目
    uint8_t *ttype;  // 比特类型，1.联合符号位，5.单独符号位；2. escape比特LSB
    uint8_t *tmsg;   // 0还是1，以bit形式存在
    unsigned int *tbitidx;    // 在当前buffer的bit位置
    int *tbitsinfo;  // cb*10 +idx
    int *tsepcpos;   // 所在帧的位置[0,1024)

    // HAAC隐写方法参数
    int haac_part_mode;          // 区域选择情况，输入时确定
    int haac_threshold;          // 符号翻转阈值，输入时确定
} FAADstegoCxtData;

FAADstegoCxtData *faad_stego_init(enum FAADStegoMethod method);
void faad_stego_close(FAADstegoCxtData *sp);
void haac_frame_flush(FAADstegoCxtData *sp, short *spectral_data);
int haac_embed_global(char *in_audio, char *out_audio, FAADstegoCxtData *stegoCxt);

#ifdef __cplusplus
}
#endif

#endif
