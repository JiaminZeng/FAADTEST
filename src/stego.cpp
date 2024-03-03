#include "stego.h"

#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cstring>

#include "codebook/hcb_en.h"

// 可用的码字
// 修改后码字肯定为27x+9y+3v+w
uint8_t hcb1_enable_embed_index[] = {
        1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1,
        0, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1,
        0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1,
};

uint8_t hcb2_enable_embed_index[] = {
        0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0,
        0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 0, 1,
        1, 1, 1, 0, 1, 0, 1, 1, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0,
        0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0,
};
uint8_t hcb5_enable_embed_index[] = {
        1, 1, 0, 0, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1,
        1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 1,
        1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 0, 0, 1, 1

};
uint8_t hcb6_enable_embed_index[] = {
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 0, 0, 1, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};

double threshold[] = {
        40.29, 40.29, 40.29, 40.29, 35.29, 35.29, 35.29, 35.29, 35.29, 35.29, 32.29,
        32.29, 32.29, 32.29, 27.29, 27.29, 27.29, 27.29, 25.29, 25.29, 25.29, 25.29,
        25.29, 25.29, 25.29, 25.29, 25.29, 25.29, 25.29, 25.29, 25.29, 25.29, 25.29,
        25.29, 25.29, 25.29, 29.45, 29.45, 29.45, 27.05, 27.05, 27.05, 27.05, 27.05,
        27.05, 27.05, 27.05, 27.05, 27.05, 27.05, 27.05, 27.05, 27.05, 27.05, 27.05,
        27.05, 27.05, 27.05, 27.05, 27.05, 27.05, 27.05, 27.05, 28.3, 28.3, 28.3,
        28.3, 28.3, 28.3, 28.3, 28.3, 28.3, 28.3, 28.3, 28.3, 28.3, 28.3,
        28.3, 28.3, 28.3, 28.3, 28.3, 28.3, 29.27, 29.27, 29.27, 29.27, 29.27,
        29.27, 29.27, 29.27, 29.27, 29.27, 29.27, 29.27, 29.27, 29.27, 29.27, 30.06,
        30.06, 30.06, 30.06, 30.06, 30.06, 30.06, 30.06, 30.06, 30.06, 30.06, 30.06,
        30.73, 30.73, 30.73, 30.73, 30.73, 30.73, 30.73, 30.73, 30.73, 30.73, 30.73,
        30.73, 30.73, 30.73, 31.31, 31.31, 31.31, 31.31, 31.31, 31.31, 31.31, 31.31,
        31.31, 31.31, 31.31, 31.31, 31.31, 31.31, 31.31, 31.31, 31.82, 31.82, 31.82,
        31.82, 31.82, 31.82, 31.82, 31.82, 31.82, 31.82, 31.82, 31.82, 31.82, 31.82,
        31.82, 31.82, 31.82, 31.82, 32.28, 32.28, 32.28, 32.28, 32.28, 32.28, 32.28,
        32.28, 32.28, 32.28, 32.69, 32.69, 32.69, 32.69, 32.69, 32.69, 32.69, 32.69,
        32.69, 32.69, 32.69, 33.07, 33.07, 33.07, 33.07, 33.07, 33.07, 33.07, 33.07,
        33.07, 33.07, 33.07, 33.07, 33.42, 33.42, 33.42, 33.42, 33.42, 33.42, 33.42,
        33.42, 33.42, 33.42, 33.42, 33.42, 33.42, 33.74, 33.74, 33.74, 33.74, 33.74,
        33.74, 33.74, 33.74, 33.74, 33.74, 33.74, 33.74, 33.74, 33.74, 34.04, 34.04,
        34.04, 34.04, 34.04, 34.04, 34.04, 34.04, 34.04, 34.04, 34.04, 34.04, 34.04,
        34.04, 34.04, 34.32, 34.32, 34.32, 34.32, 34.32, 34.32, 34.32, 34.32, 34.32,
        34.32, 34.32, 34.32, 34.32, 34.32, 34.32, 34.32, 34.58, 34.58, 34.58, 34.58,
        34.58, 34.58, 34.58, 34.58, 34.58, 34.58, 34.58, 34.58, 34.58, 34.58, 34.58,
        34.58, 34.58, 34.83, 34.83, 34.83, 34.83, 34.83, 34.83, 34.83, 34.83, 34.83,
        34.83, 34.83, 34.83, 34.83, 34.83, 34.83, 34.83, 34.83, 34.83, 35.23, 35.23,
        35.23, 35.23, 35.23, 35.23, 35.23, 35.23, 35.23, 35.23, 35.23, 35.23, 35.23,
        35.23, 35.23, 35.23, 35.23, 35.23, 35.23, 35.23, 38.5, 38.5, 38.5, 38.5,
        38.5, 38.5, 38.5, 38.5, 38.5, 38.5, 38.5, 38.5, 38.5, 38.5, 38.5,
        38.5, 38.5, 38.5, 38.5, 38.5, 38.5, 38.89, 38.89, 38.89, 38.89, 38.89,
        38.89, 38.89, 38.89, 38.89, 38.89, 38.89, 38.89, 38.89, 38.89, 38.89, 38.89,
        38.89, 38.89, 38.89, 38.89, 38.89, 38.89, 38.89, 39.08, 39.08, 39.08, 39.08,
        39.08, 39.08, 39.08, 39.08, 39.08, 39.08, 39.08, 39.08, 39.08, 39.08, 39.08,
        39.08, 39.08, 39.08, 39.08, 39.08, 39.08, 39.08, 39.08, 39.08, 41.43, 41.43,
        41.43, 41.43, 41.43, 41.43, 41.43, 41.43, 41.43, 41.43, 41.43, 41.43, 41.43,
        41.43, 41.43, 41.43, 41.43, 41.43, 41.43, 41.43, 41.43, 41.43, 41.43, 41.43,
        41.43, 41.43, 41.75, 41.75, 41.75, 41.75, 41.75, 41.75, 41.75, 41.75, 41.75,
        41.75, 41.75, 41.75, 41.75, 41.75, 41.75, 41.75, 41.75, 41.75, 41.75, 41.75,
        41.75, 41.75, 41.75, 41.75, 41.75, 41.75, 41.75, 41.75, 42.05, 42.05, 42.05,
        42.05, 42.05, 42.05, 42.05, 42.05, 42.05, 42.05, 42.05, 42.05, 42.05, 42.05,
        42.05, 42.05, 42.05, 42.05, 42.05, 42.05, 42.05, 42.05, 42.05, 42.05, 42.05,
        42.05, 42.05, 42.05, 42.05, 42.05, 47.46, 47.46, 47.46, 47.46, 47.46, 47.46,
        47.46, 47.46, 47.46, 47.46, 47.46, 47.46, 47.46, 47.46, 47.46, 47.46, 47.46,
        47.46, 47.46, 47.46, 47.46, 47.46, 47.46, 47.46, 47.46, 47.46, 47.46, 47.46,
        47.46, 47.46, 47.46, 47.46, 47.46, 47.84, 47.84, 47.84, 47.84, 47.84, 47.84,
        47.84, 47.84, 47.84, 47.84, 47.84, 47.84, 47.84, 47.84, 47.84, 47.84, 47.84,
        47.84, 47.84, 47.84, 47.84, 47.84, 47.84, 47.84, 47.84, 47.84, 47.84, 47.84,
        47.84, 47.84, 47.84, 47.84, 47.84, 47.84, 47.84, 47.84, 48.19, 48.19, 48.19,
        48.19, 48.19, 48.19, 48.19, 48.19, 48.19, 48.19, 48.19, 48.19, 48.19, 48.19,
        48.19, 48.19, 48.19, 48.19, 48.19, 48.19, 48.19, 48.19, 48.19, 48.19, 48.19,
        48.19, 48.19, 48.19, 48.19, 48.19, 48.19, 48.19, 48.19, 48.19, 48.19, 48.19,
        48.19, 48.19, 48.19, 58.61, 58.61, 58.61, 58.61, 58.61, 58.61, 58.61, 58.61,
        58.61, 58.61, 58.61, 58.61, 58.61, 58.61, 58.61, 58.61, 58.61, 58.61, 58.61,
        58.61, 58.61, 58.61, 58.61, 58.61, 58.61, 58.61, 58.61, 58.61, 58.61, 58.61,
        58.61, 58.61, 58.61, 58.61, 58.61, 58.61, 58.61, 58.61, 58.61, 58.61, 58.61,
        58.61, 58.61, 59, 59, 59, 59, 59, 59, 59, 59, 59,
        59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59,
        59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59,
        59, 59, 59, 59, 59, 59, 59, 59, 59, 59, 59,
        59, 59, 59, 59, 59, 59.52, 59.52, 59.52, 59.52, 59.52, 59.52,
        59.52, 59.52, 59.52, 59.52, 59.52, 59.52, 59.52, 59.52, 59.52, 59.52, 59.52,
        59.52, 59.52, 59.52, 59.52, 59.52, 59.52, 59.52, 59.52, 59.52, 59.52, 59.52,
        59.52, 59.52, 59.52, 59.52, 59.52, 59.52, 59.52, 59.52, 59.52, 59.52, 59.52,
        59.52, 59.52, 59.52, 59.52, 59.52, 59.52, 59.52, 59.52, 59.52, 59.52, 59.52,
        59.52, 59.52, 59.52, 69.98, 69.98, 69.98, 69.98, 69.98, 69.98, 69.98, 69.98,
        69.98, 69.98, 69.98, 69.98, 69.98, 69.98, 69.98, 69.98, 69.98, 69.98, 69.98,
        69.98, 69.98, 69.98, 69.98, 69.98, 69.98, 69.98, 69.98, 69.98, 69.98, 69.98,
        69.98, 69.98, 69.98, 69.98, 69.98, 69.98, 69.98, 69.98, 69.98, 69.98, 69.98,
        69.98, 69.98, 69.98, 69.98, 69.98, 69.98, 69.98, 69.98, 69.98, 69.98, 69.98,
        69.98, 69.98, 69.98, 69.98, 69.98, 69.98, 69.98, 70.54, 70.54, 70.54, 70.54,
        70.54, 70.54, 70.54, 70.54, 70.54, 70.54, 70.54, 70.54, 70.54, 70.54, 70.54,
        70.54, 70.54, 70.54, 70.54, 70.54, 70.54, 70.54, 70.54, 70.54, 70.54, 70.54,
        70.54, 70.54, 70.54, 70.54, 70.54, 70.54, 70.54, 70.54, 70.54, 70.54, 70.54,
        70.54, 70.54, 70.54, 70.54, 70.54, 70.54, 70.54, 70.54, 70.54, 70.54, 70.54,
        70.54, 70.54, 70.54, 70.54, 70.54, 70.54, 70.54, 70.54, 70.54, 70.54, 70.54,
        70.54, 70.54, 70.54, 70.54, 70.54, 70.54, 70.54, 70.54, 71.08, 71.08, 71.08,
        71.08, 71.08, 71.08, 71.08, 71.08, 71.08, 71.08, 71.08, 71.08, 71.08, 71.08,
        71.08, 71.08, 71.08, 71.08, 71.08, 71.08, 71.08, 71.08, 71.08, 71.08, 71.08,
        71.08, 71.08, 71.08, 71.08, 71.08, 71.08, 71.08, 71.08, 71.08, 71.08, 71.08,
        71.08, 71.08, 71.08, 71.08, 71.08, 71.08, 71.08, 71.08, 71.08, 71.08, 71.08,
        71.08, 71.08, 71.08, 71.08, 71.08, 71.08, 71.08, 71.08, 71.08, 71.08, 71.08,
        71.08, 71.08, 71.08, 71.08, 71.08, 71.08, 71.08, 71.08, 71.08, 71.08, 71.08,
        71.08, 71.08, 71.08, 71.08, 71.08, 71.08, 71.08, 71.72, 71.72, 71.72, 71.72,
        71.72, 71.72, 71.72, 71.72, 71.72, 71.72, 71.72, 71.72, 71.72, 71.72, 71.72,
        71.72, 71.72, 71.72, 71.72, 71.72, 71.72, 71.72, 71.72, 71.72, 71.72, 71.72,
        71.72, 71.72, 71.72, 71.72, 71.72, 71.72, 71.72, 71.72, 71.72, 71.72, 71.72,
        71.72, 71.72, 71.72, 71.72, 71.72, 71.72, 71.72, 71.72, 71.72, 71.72, 71.72,
        71.72, 71.72, 71.72, 71.72, 71.72, 71.72, 71.72, 71.72, 71.72, 71.72, 71.72,
        71.72, 71.72, 71.72, 71.72, 71.72, 71.72, 71.72, 71.72, 71.72, 71.72, 71.72,
        71.72, 71.72, 71.72, 71.72, 71.72, 71.72, 71.72, 71.72, 71.72, 71.72, 71.72,
        71.72, 71.72, 71.72, 71.72, 71.72, 71.72, 71.72, 72.09, 72.09, 72.09, 72.09,
        72.09, 72.09, 72.09, 72.09, 72.09, 72.09, 72.09, 72.09, 72.09, 72.09, 72.09,
        72.09, 72.09, 72.09, 72.09, 72.09, 72.09, 72.09, 72.09, 72.09, 72.09, 72.09,
        72.09};

uint8_t *hcb_enable_embed_index[] = {nullptr,
                                     hcb1_enable_embed_index,
                                     hcb2_enable_embed_index,
                                     nullptr,
                                     nullptr,
                                     hcb5_enable_embed_index,
                                     hcb6_enable_embed_index,
                                     nullptr,
                                     nullptr,
                                     nullptr,
                                     nullptr,
                                     nullptr,
                                     nullptr};


//stegoCxtData *initstegoData(int cap, int area, int threshold, uint8_t *msg,
//                            int *bitidx, int *bitsinfo, double *cost) {
//    // 全局部分
//    stegoCxtData *sp = (stegoCxtData *) malloc(sizeof(stegoCxtData));
//    sp->cap = cap;
//    sp->cur_inx = 0;
//
//    sp->area = area;
//    sp->threshold = threshold;
//
//    sp->msg = msg;
//
//    sp->bitidx = bitidx;
//    sp->bitsinfo = bitsinfo;
//    sp->cost = cost;
//
//    // 单帧部分，重复使用
//    sp->tnum = 0;
//
//    sp->ttype = (uint8_t *) malloc(sizeof(uint8_t) * 1024);
//    sp->tmsg = (uint8_t *) malloc(sizeof(uint8_t) * 1024);
//    sp->tbitidx = (int *) malloc(sizeof(int) * 1024);
//    sp->tbitsinfo = (int *) malloc(sizeof(int) * 1024);
//    sp->tsepcpos = (int *) malloc(sizeof(int) * 1024);
//    return sp;
//}

FAADstegoCxtData *faad_stego_init(FAADStegoMethod method) {
    auto *sp = new FAADstegoCxtData;
    memset(sp, 0, sizeof(FAADstegoCxtData));
    sp->METHOD = method;
    sp->ttype = new uint8_t[1024];
    sp->tmsg = new uint8_t[1024];
    sp->tsepcpos = new uint8_t[1024];
    sp->tbitidx = new unsigned int[1024];
    sp->tbitsinfo = new int[1024];
    return sp;
}

void faad_stego_close(FAADstegoCxtData *sp) {
    delete sp->ttype;
    delete sp->tmsg;
    delete sp->tbitidx;
    delete sp->tbitsinfo;
    delete sp->tsepcpos;
    delete sp;
}

static int get_1bit(FAADstegoCxtData *stegoCfg) {
    return (stegoCfg->cur_byte >> stegoCfg->cur_bit_idx) & 1;
}

static void put_1bit(FAADstegoCxtData *stegoCfg, unsigned char bit) {
    if (bit)
        stegoCfg->cur_byte |= (1 << stegoCfg->cur_bit_idx);
}

static void put_cost(FAADstegoCxtData *stegoCfg, double cost) {
    *(stegoCfg->cost) = cost;
    stegoCfg->cost += 1;
}

static void faad_flush_1bit(FAADstegoCxtData *stegoCfg) {
    stegoCfg->cur_bit_idx += 1;
    if (stegoCfg->cur_bit_idx == 8) {
        stegoCfg->cur_bit_idx = 0;
        if (stegoCfg->STEP == FAADStegoStep::GET_COST || stegoCfg->STEP == FAADStegoStep::EXTRACT) {
            // 提取过程
            stegoCfg->msg[stegoCfg->cur_byte_idx] = stegoCfg->cur_byte;
            stegoCfg->cur_byte = 0;
            stegoCfg->cur_byte_idx += 1;
        } else if (stegoCfg->STEP == FAADStegoStep::EMBED) {
            stegoCfg->cur_byte_idx += 1;
            stegoCfg->cur_byte = stegoCfg->msg[stegoCfg->cur_byte_idx];
        }
    }

    // 检查过程是否结束
    if (stegoCfg->cur_byte_idx == stegoCfg->cap_bytes) {
        stegoCfg->FINISH = true;
    }
}

static int faad_payload_check(FAADstegoCxtData *stegoCfg) {
    if (stegoCfg->payload > stegoCfg->logic_bit_idx++ % 10) {
        stegoCfg->useful_bit_idx += 1;
        return 1;
    } else {
        return 0;
    }
}

static int faad_base_check(int qmdct, int thr, int type, int area, int cb, int off) {
    // 1. 区域正确
    if (type & area) {
        if (type == 2) {    // 2. 溢出位
            return 1;
        } else if (type == 5) {  // 2. 符号位，单独比特编码
            return qmdct != 0 && abs(qmdct) <= thr;
        } else {  // 2. 符号位，联合编码
            return qmdct != 0 && abs(qmdct) <= thr && hcb_enable_embed_index[cb][off];
        }
    }
    // 1. 区域不正确
    return 0;
}

void haac_frame_flush(FAADstegoCxtData *sp, short *spectral_data) {
    sp->frame_idx += 1;
    for (int i = 0; i < sp->tnum && !sp->FINISH; i++) {
        // 信息读取
        int pos = sp->tsepcpos[i];
        int cb = sp->tbitsinfo[i] / 10;
        int idx = sp->tbitsinfo[i] % 10;
        int type = sp->ttype[i];
        int st_pos = pos - idx;
        int off = 40;
        if (type == 1) {
            if (cb <= 2) {
                off += 27 * spectral_data[st_pos] + 9 * spectral_data[st_pos + 1] +
                       3 * spectral_data[st_pos + 2] + spectral_data[st_pos + 3];
            } else {
                off += 9 * spectral_data[st_pos] + spectral_data[st_pos + 1];
            }
        }

        if (faad_base_check(spectral_data[pos], sp->haac_threshold, sp->ttype[i], sp->haac_part_mode, cb, off) &&
            faad_payload_check(sp)) {
            switch (sp->STEP) {
                case EMBED: {
                    sp->bitidx[sp->emb_bit_idx] = sp->tbitidx[i] + sp->file_offset * 8;
                    sp->bitsinfo[sp->emb_bit_idx] = sp->tbitsinfo[i] + off * 1000;
                    sp->type[sp->emb_bit_idx] = sp->ttype[i];
                    if (sp->tmsg[i])
                        sp->bitsinfo[sp->emb_bit_idx] *= -1;
                    sp->emb_bit_idx += 1;
                    if (sp->emb_bit_idx == sp->cap_bits)
                        sp->FINISH = 1;
                    break;
                }
                case EXTRACT:  // ok!
                    put_1bit(sp, sp->tmsg[i]);
                    faad_flush_1bit(sp);
                    break;
                case GET_CAP:  // check 已经完成了该过程
                    break;
                case GET_COST:  // ok!
                    put_1bit(sp, sp->tmsg[i]);
                    double cost = 0.2;  // 溢出位均为0.2
                    double alp = 1.3;  // 符号位需要单独计算
                    if (sp->ttype[i] & 1) {
                        if (pos - 1 >= 0) cost += abs(spectral_data[pos - 1]);
                        if (pos + 1 < 1024) cost += abs(spectral_data[pos + 1]);
                        cost /= log(threshold[pos] + alp);
                    }
                    put_cost(sp, cost);
                    faad_flush_1bit(sp);
                    break;
            }
        }
    }
    sp->tnum = 0;
}
//
//void closestegoData(stegoCxtData *sp) {
//    if (sp) {
//        if (sp->ttype) free(sp->ttype);
//        if (sp->tmsg) free(sp->tmsg);
//        if (sp->tbitidx) free(sp->tbitidx);
//        if (sp->tbitsinfo) free(sp->tbitsinfo);
//        if (sp->tsepcpos) free(sp->tsepcpos);
//        free(sp);
//    }
//}

//void flushstegoData(stegoCxtData *sp, short *spectral_data) {
//    // static int id = 1;
//    // printf("ID %d: %d %d\n", id, sp->tnum, sp->cur_inx);
//    if (sp->cur_inx >= sp->cap) return;
//    for (int i = 0; i < sp->tnum && sp->cur_inx < sp->cap; i++) {
//        if ((sp->ttype[i] & sp->area) == 0) {
//            continue;
//        }
//        if (sp->ttype[i] & 1) {
//            int pos = sp->tsepcpos[i];
//            // 判断部分：
//            // 对于bitsinfo非1的值，考察word的第一个值，检测整个码字是否可选
//            // 如果不可选则进行跳过，1和2号码书有4个值，5和6码书有2个值
//
//            if (sp->tbitsinfo[i] > 1 && sp->tbitsinfo[i] % 10 == 0) {
//                int cb = sp->tbitsinfo[i] % 100 / 10;
//                int off = 40;
//                if (cb <= 2) {
//                    off += 27 * spectral_data[pos] + 9 * spectral_data[pos + 1] +
//                           3 * spectral_data[pos + 2] + spectral_data[pos + 3];
//                    for (int j = 0; j < 4; j++) {
//                        sp->tbitsinfo[i + j] += 1000 * off;
//                    }
//                } else {
//                    off += 9 * spectral_data[pos] + spectral_data[pos + 1];
//                    for (int j = 0; j < 2; j++) {
//                        sp->tbitsinfo[i + j] += 1000 * off;
//                    }
//                }
//
//                if (!hcb_enable_embed_index[cb][off]) {
//                    if (cb <= 2)
//                        i += 3;
//                    else
//                        i += 1;
//                    continue;
//                }
//            }
//
//            int spe = spectral_data[pos];
//            if (spe < 0) spe = -spe;
//            if (spe && spe <= sp->threshold) {  // 符号位
//                if (sp->msg) {
//                    sp->msg[sp->cur_inx] = sp->tmsg[i];
//                }
//                if (sp->bitidx) {
//                    sp->bitidx[sp->cur_inx] = sp->file_offset * 8 + sp->tbitidx[i];
//                    sp->bitsinfo[sp->cur_inx] = sp->tbitsinfo[i];
//
//                    double alp = 1.3;
//                    double cost = spe + alp;
//                    if (pos - 1 >= 0) cost += abs(spectral_data[pos - 1]);
//                    if (pos + 1 < 1024) cost += abs(spectral_data[pos + 1]);
//                    sp->cost[sp->cur_inx] = cost / log(threshold[pos] + alp);
//                }
//                sp->cur_inx += 1;
//            }
//        } else if (sp->ttype[i] == 2) {  // 溢出位
//            if (sp->msg) {
//                sp->msg[sp->cur_inx] = sp->tmsg[i];
//            }
//            if (sp->bitidx) {
//                sp->bitidx[sp->cur_inx] = sp->tbitidx[i] + sp->file_offset * 8;
//                sp->bitsinfo[sp->cur_inx] = sp->tbitsinfo[i];
//                sp->cost[sp->cur_inx] = 0.2;
//            }
//            sp->cur_inx += 1;
//        }
//    }
//    // printf("ID %d: %d %d\n", id++, sp->tnum, sp->cur_inx);
//    sp->tnum = 0;
//}


int haac_embed_global(char *in_audio, char *out_audio, FAADstegoCxtData *stegoCxt) {
    FILE *input = fopen(in_audio, "rb");
    FILE *output = fopen(out_audio, "wb+");

    unsigned int *bitidx = stegoCxt->bitidx;
    int *bitsinfo = stegoCxt->bitsinfo;
    uint8_t *type = stegoCxt->type;

    if (input == nullptr || output == nullptr) {
        exit(-1);
    }

    // 拷贝
    unsigned int bread;
    unsigned int bwrite;
    unsigned int buffsize = 20480;
    auto *buf = (uint8_t *) malloc(buffsize * sizeof(uint8_t));
    while ((bread = fread(buf, sizeof(uint8_t), buffsize, input)) != 0) {
        bwrite = fwrite(buf, sizeof(uint8_t), bread, output);
        if (bread != bwrite) exit(-1);
    }
    free(buf);
    fclose(input);
    fclose(output);

    FILE *sp = fopen(out_audio, "rb+");

    // 一种低效的实现方案，先这样吧，两种高效的方案如下
    // 1.实际上在上述复制的过程中即可以进行修改，但是只改前段，需要循环位移控制
    // 2.另外一种就是在编码过程，处理完每帧的内容直接进行修改即可
    uint8_t byte;
    for (int i = 0; i < stegoCxt->emb_bit_idx;) {
        if (i == 57) { ;
        }
        unsigned int bytes_pos = bitidx[i] >> 3;
        unsigned int left = bitidx[i] & 7;
        unsigned int mid_right = 7 - left;
        fseek(sp, bytes_pos, SEEK_SET);
        if (type[i] != 1) {
            fread(&byte, 1UL, 1, sp);
            fseek(sp, -1, SEEK_CUR);
            int msg = get_1bit(stegoCxt);
            if (((byte >> mid_right) & 1) != msg) {
                byte ^= (1 << mid_right);
            }
            fwrite(&byte, 1UL, 1, sp);
            faad_flush_1bit(stegoCxt);
            i += 1;
        } else {
            int old_word = abs(bitsinfo[i]) / 1000;
            int cb = abs(bitsinfo[i]) % 1000 / 10;
            int spe[4] = {0};
            if (cb <= 2) {
                for (int j = 3; j >= 0; j--) {
                    spe[j] = old_word % 3 - 1;
                    old_word /= 3;
                }
            } else {
                spe[0] = old_word / 9 - 4;
                spe[1] = old_word % 9 - 4;
            }

            int inc = 0;
            while (i + inc < stegoCxt->emb_bit_idx && bitidx[i] == bitidx[i + inc]) {
                int inx = abs(bitsinfo[i + inc]) % 10;
                int msg = bitsinfo[i + inc] < 0 ? 1 : 0;
                if (msg != get_1bit(stegoCxt)) {
                    spe[inx] *= -1;
                }
                faad_flush_1bit(stegoCxt);
                inc += 1;
            }
            i += inc;

            int new_word = 0;
            if (cb <= 2) {  // 计算新的offset
                for (int j: spe) {
                    new_word = new_word * 3 + j;
                }
            } else {
                new_word = spe[0] * 9 + spe[1];
            }
            new_word += 40;

            if (new_word == old_word)continue;
            unsigned int tot_bits = 0;
            int val = 0;
            // 码书选择
            switch (cb) {  // 获取bit的数目和对应的值
                case 1:
                    tot_bits = huff1[new_word][0];
                    val = huff1[new_word][1];
                    break;
                case 2:
                    tot_bits = huff2[new_word][0];
                    val = huff2[new_word][1];
                    break;
                case 5:
                    tot_bits = huff5[new_word][0];
                    val = huff5[new_word][1];
                    break;
                case 6:
                    tot_bits = huff6[new_word][0];
                    val = huff6[new_word][1];
                    break;
                default:
                    break;
            }

            // 写入流程
            while (tot_bits) {            // 如果还没有写完
                fread(&byte, 1UL, 1, sp);  // 读取一个byte
                fseek(sp, -1, SEEK_CUR);   // 退回一个byte
                unsigned int enable = 8 - left;
                if (tot_bits >= enable) {
                    // 保留左边left个                 保留左边enable
                    byte = ((byte >> enable) << enable) | (val >> (tot_bits - enable));
                    // 减去左边enable
                    val -= ((val >> (tot_bits - enable)) << (tot_bits - enable));
                    tot_bits -= enable;
                } else {
                    unsigned int mid = tot_bits;
                    unsigned int right = enable - mid;
                    // 保留左边left个                   // 保留右边right个
                    byte = ((byte >> enable) << enable) | (byte & ((1 << right) - 1)) |
                           (val << right);
                    tot_bits = 0;
                }
                fwrite(&byte, 1UL, 1, sp);
                left = 0;
            }
        }
    }
    fclose(sp);
    return 0;
}

