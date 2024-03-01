#include "stego.h"

#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cstring>

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

uint8_t *hcb_enable_embed_index[] = {0,
                                     hcb1_enable_embed_index,
                                     hcb2_enable_embed_index,
                                     0,
                                     0,
                                     hcb5_enable_embed_index,
                                     hcb6_enable_embed_index,
                                     0,
                                     0,
                                     0,
                                     0,
                                     0,
                                     0};


StegaCxtData *initStegaData(int cap, int area, int threshold, uint8_t *msg,
                            int *bitidx, int *bitsinfo, double *cost) {
    // 全局部分
    StegaCxtData *sp = (StegaCxtData *) malloc(sizeof(StegaCxtData));
    sp->cap = cap;
    sp->cur_inx = 0;

    sp->area = area;
    sp->threshold = threshold;

    sp->msg = msg;

    sp->bitidx = bitidx;
    sp->bitsinfo = bitsinfo;
    sp->cost = cost;

    // 单帧部分，重复使用
    sp->tnum = 0;

    sp->ttype = (uint8_t *) malloc(sizeof(uint8_t) * 1024);
    sp->tmsg = (uint8_t *) malloc(sizeof(uint8_t) * 1024);
    sp->tbitidx = (int *) malloc(sizeof(int) * 1024);
    sp->tbitsinfo = (int *) malloc(sizeof(int) * 1024);
    sp->tsepcpos = (int *) malloc(sizeof(int) * 1024);
    return sp;
}

FAADStegaCxtData *faad_stega_init(FAADStegaMethod method) {
    auto *sp = new FAADStegaCxtData;
    memset(sp, 0, sizeof(FAADStegaCxtData));
    sp->METHOD = method;
    sp->ttype = new uint8_t[1024];
    sp->tmsg = new uint8_t[1024];
    sp->tbitidx = new int[1024];
    sp->tbitsinfo = new int[1024];
    sp->tsepcpos = new int[1024];
    return sp;
}

void faad_stega_close(FAADStegaCxtData *sp) {
    delete sp->ttype;
    delete sp->tmsg;
    delete sp->tbitidx;
    delete sp->tbitsinfo;
    delete sp->tsepcpos;
    delete sp;
}

static int get_1bit(FAADStegaCxtData *stegoCfg) {
    return (stegoCfg->cur_byte >> stegoCfg->cur_bit_idx) & 1;
}

static void put_1bit(FAADStegaCxtData *stegoCfg, unsigned char bit) {
    if (bit)
        stegoCfg->cur_byte |= (1 << stegoCfg->cur_bit_idx);
}

static void put_cost(FAADStegaCxtData *stegoCfg, double cost) {
    *(stegoCfg->cost) = cost;
    stegoCfg->cost += 1;
}

static void faad_flush_1bit(FAADStegaCxtData *stegoCfg) {
    stegoCfg->cur_bit_idx += 1;
    if (stegoCfg->cur_bit_idx == 8) {
        stegoCfg->cur_bit_idx = 0;
        if (stegoCfg->STEP == FAADStegaStep::GET_COST || stegoCfg->STEP == FAADStegaStep::EXTRACT) {
            // 提取过程
            stegoCfg->msg[stegoCfg->cur_byte_idx] = stegoCfg->cur_byte;
            stegoCfg->cur_byte = 0;
            stegoCfg->cur_byte_idx += 1;
        }
    }

    // 检查过程是否结束
    if (stegoCfg->cur_byte_idx == stegoCfg->cap_bytes) {
        stegoCfg->FINISH = true;
    }
}

static int faad_payload_check(FAADStegaCxtData *stegoCfg) {
    if (stegoCfg->payload > stegoCfg->logic_bit_idx++ % 10) {
        stegoCfg->useful_bit_idx += 1;
        return 1;
    } else {
        return 0;
    }
}

static int faad_base_check(int qmdct, int thr, int type, int area) {
    if (type & area) {
        if (area != 2 && (qmdct == 0 || abs(qmdct) > thr))
            return 0;
        else
            return 1;
    }
    return 0;
}

void haac_embed_single(int idx, FAADStegaCxtData *sp, short *spectral_data) {
    int pos = sp->tsepcpos[idx];
    int cb = sp->tbitsinfo[idx] % 1000 / 10;

    if (sp->ttype[idx] & 1) {
        // 判断部分：
        // 对于bitsinfo非1的值，考察word的第一个值，检测整个码字是否可选
        // 如果不可选则进行跳过，1和2号码书有4个值，5和6码书有2个值
        int off = 40;
        if (sp->ttype[idx] & 4) {
            off += 27 * spectral_data[pos] + 9 * spectral_data[pos + 1] +
                   3 * spectral_data[pos + 2] + spectral_data[pos + 3];

        } else {
            off += 9 * spectral_data[pos] + spectral_data[pos + 1];

        }
        for (int j = 0; j < 4; j++) {
            sp->tbitsinfo[idx + j] += 1000 * off;
        }
        if (!hcb_enable_embed_index[cb][off]) {
            return;
        }
        int ed = (sp->ttype[idx] & 4) ? 2 : 4;
        for (int j = 0; j < 2; j++) {
            sp->tbitsinfo[idx + j] += 1000 * off;
        }
        for (int j = 0; j < 4; j++) {
            sp->tbitsinfo[idx + j] += 1000 * off;
        }

        sp->bitidx[sp->cur_inx] = sp->file_offset * 8 + sp->tbitidx[idx];
        sp->bitsinfo[sp->cur_inx] = sp->file_offset * 8 + sp->tbitidx[idx];


        int spe = spectral_data[pos];
        if (spe < 0) spe = -spe;
        sp->msg[sp->cur_inx] = sp->tmsg[inx];
        if (sp->bitidx) {
            sp->bitidx[sp->cur_inx] = sp->file_offset * 8 + sp->tbitidx[i];
            sp->bitsinfo[sp->cur_inx] = sp->tbitsinfo[i];

            sp->cur_inx += 1;
        } else if (sp->ttype[inx] == 5) {
            int off = 40;
            off += 9 * spectral_data[pos] + spectral_data[pos + 1];
            for (int j = 0; j < 2; j++) {
                sp->tbitsinfo[idx + j] += 1000 * off;
            }

        } else if (sp->ttype[i] == 2) {  // 溢出位
            if (sp->msg) {
                sp->msg[sp->cur_inx] = sp->tmsg[i];
            }
            if (sp->bitidx) {
                sp->bitidx[sp->cur_inx] = sp->tbitidx[i] + sp->file_offset * 8;
                sp->bitsinfo[sp->cur_inx] = sp->tbitsinfo[i];
                sp->cost[sp->cur_inx] = 0.2;
            }
            sp->cur_inx += 1;
        }
    }
}

void haac_embed_flush(FAADStegaCxtData *sp, short *spectral_data) {
    sp->frame_idx += 1;
    if (sp->FINISH)return;
    for (int i = 0; i < sp->tnum && !sp->FINISH; i++) {
        if ((sp->ttype[i] & sp->haac_part_mode) == 0) {
            continue;
        }
        int cb = sp->tbitsinfo[i] / 10;
        int idx = sp->tbitsinfo[i] % 10;
        if (sp->ttype[i] == 1) {
            int pos = sp->tsepcpos[i];
            int off = 40;
            if (idx == 0) {
                if (cb <= 2) {
                    off += 27 * spectral_data[pos] + 9 * spectral_data[pos + 1] +
                           3 * spectral_data[pos + 2] + spectral_data[pos + 3];
                    for (int j = 0; j < 4; j++) {
                        sp->tbitsinfo[i + j] += 1000 * off;
                    }
                    if (!hcb_enable_embed_index[cb][off]) {
                        i += 3;
                        continue;
                    }
                } else {
                    off += 9 * spectral_data[pos] + spectral_data[pos + 1];
                    for (int j = 0; j < 2; j++) {
                        sp->tbitsinfo[i + j] += 1000 * off;
                    }
                    if (!hcb_enable_embed_index[cb][off]) {
                        i += 1;
                        continue;
                    }
                }
            } else if (idx == 5) {
                sp->msg[sp->cur_inx] = sp->tmsg[i];
                sp->bitidx[sp->cur_inx] = sp->file_offset * 8 + sp->tbitidx[i];
                sp->bitsinfo[sp->cur_inx] = sp->tbitsinfo[i];
                sp->cur_inx += 1;
            } else if (idx == 2) {

            }


            if (sp->tbitsinfo[i] > 1 && sp->tbitsinfo[i] % 10 == 0) {
                int cb = sp->tbitsinfo[i] % 1000 / 10;
                int off = 40;
                if (cb <= 2) {
                    off += 27 * spectral_data[pos] + 9 * spectral_data[pos + 1] +
                           3 * spectral_data[pos + 2] + spectral_data[pos + 3];
                    for (int j = 0; j < 4; j++) {
                        sp->tbitsinfo[i + j] += 1000 * off;
                    }
                } else {
                    off += 9 * spectral_data[pos] + spectral_data[pos + 1];
                    for (int j = 0; j < 2; j++) {
                        sp->tbitsinfo[i + j] += 1000 * off;
                    }
                }

                if (!hcb_enable_embed_index[cb][off]) {
                    if (cb <= 2)
                        i += 3;
                    else
                        i += 1;
                    continue;
                }
            }

            int spe = spectral_data[pos];
            if (spe < 0) spe = -spe;
            if (spe && spe <= sp->threshold) {  // 符号位
                if (sp->msg) {
                    sp->msg[sp->cur_inx] = sp->tmsg[i];
                }
                if (sp->bitidx) {
                    sp->bitidx[sp->cur_inx] = sp->file_offset * 8 + sp->tbitidx[i];
                    sp->bitsinfo[sp->cur_inx] = sp->tbitsinfo[i];

                    double alp = 1.3;
                    double cost = spe + alp;
                    if (pos - 1 >= 0) cost += abs(spectral_data[pos - 1]);
                    if (pos + 1 < 1024) cost += abs(spectral_data[pos + 1]);
                    sp->cost[sp->cur_inx] = cost / log(threshold[pos] + alp);
                }
                sp->cur_inx += 1;
            }
        } else if (sp->ttype[i] == 2) {  // 溢出位
            if (sp->msg) {
                sp->msg[sp->cur_inx] = sp->tmsg[i];
            }
            if (sp->bitidx) {
                sp->bitidx[sp->cur_inx] = sp->tbitidx[i] + sp->file_offset * 8;
                sp->bitsinfo[sp->cur_inx] = sp->tbitsinfo[i];
                sp->cost[sp->cur_inx] = 0.2;
            }
            sp->cur_inx += 1;
        }
    }
}

void haac_other_flush(FAADStegaCxtData *sp, short *spectral_data) {
    sp->frame_idx += 1;
    if (sp->FINISH) return;
    for (int i = 0; i < sp->tnum && !sp->FINISH; i++) {
        int pos = sp->tsepcpos[i];
        if (faad_base_check(spectral_data[pos], sp->haac_threshold, sp->ttype[i], sp->haac_part_mode) &&
            faad_payload_check(sp)) {
            switch (sp->STEP) {
                case GET_CAP:
                    break;
                case EXTRACT:
                    put_1bit(sp, sp->tmsg[i]);
                    faad_flush_1bit(sp);
                    break;
                case GET_COST:
                    put_1bit(sp, sp->tmsg[i]);
                    double cost = 0.2;
                    double alp = 1.3;
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


int checkHcbEncable(int cb, int offset) {
    return hcb_enable_embed_index[cb][offset];
}

void closeStegaData(StegaCxtData *sp) {
    if (sp) {
        if (sp->ttype) free(sp->ttype);
        if (sp->tmsg) free(sp->tmsg);
        if (sp->tbitidx) free(sp->tbitidx);
        if (sp->tbitsinfo) free(sp->tbitsinfo);
        if (sp->tsepcpos) free(sp->tsepcpos);
        free(sp);
    }
}

void flushStegaData(StegaCxtData *sp, short *spectral_data) {
    // static int id = 1;
    // printf("ID %d: %d %d\n", id, sp->tnum, sp->cur_inx);
    if (sp->cur_inx >= sp->cap) return;
    for (int i = 0; i < sp->tnum && sp->cur_inx < sp->cap; i++) {
        if ((sp->ttype[i] & sp->area) == 0) {
            continue;
        }
        if (sp->ttype[i] & 1) {
            int pos = sp->tsepcpos[i];
            // 判断部分：
            // 对于bitsinfo非1的值，考察word的第一个值，检测整个码字是否可选
            // 如果不可选则进行跳过，1和2号码书有4个值，5和6码书有2个值

            if (sp->tbitsinfo[i] > 1 && sp->tbitsinfo[i] % 10 == 0) {
                int cb = sp->tbitsinfo[i] % 100 / 10;
                int off = 40;
                if (cb <= 2) {
                    off += 27 * spectral_data[pos] + 9 * spectral_data[pos + 1] +
                           3 * spectral_data[pos + 2] + spectral_data[pos + 3];
                    for (int j = 0; j < 4; j++) {
                        sp->tbitsinfo[i + j] += 1000 * off;
                    }
                } else {
                    off += 9 * spectral_data[pos] + spectral_data[pos + 1];
                    for (int j = 0; j < 2; j++) {
                        sp->tbitsinfo[i + j] += 1000 * off;
                    }
                }

                if (!hcb_enable_embed_index[cb][off]) {
                    if (cb <= 2)
                        i += 3;
                    else
                        i += 1;
                    continue;
                }
            }

            int spe = spectral_data[pos];
            if (spe < 0) spe = -spe;
            if (spe && spe <= sp->threshold) {  // 符号位
                if (sp->msg) {
                    sp->msg[sp->cur_inx] = sp->tmsg[i];
                }
                if (sp->bitidx) {
                    sp->bitidx[sp->cur_inx] = sp->file_offset * 8 + sp->tbitidx[i];
                    sp->bitsinfo[sp->cur_inx] = sp->tbitsinfo[i];

                    double alp = 1.3;
                    double cost = spe + alp;
                    if (pos - 1 >= 0) cost += abs(spectral_data[pos - 1]);
                    if (pos + 1 < 1024) cost += abs(spectral_data[pos + 1]);
                    sp->cost[sp->cur_inx] = cost / log(threshold[pos] + alp);
                }
                sp->cur_inx += 1;
            }
        } else if (sp->ttype[i] == 2) {  // 溢出位
            if (sp->msg) {
                sp->msg[sp->cur_inx] = sp->tmsg[i];
            }
            if (sp->bitidx) {
                sp->bitidx[sp->cur_inx] = sp->tbitidx[i] + sp->file_offset * 8;
                sp->bitsinfo[sp->cur_inx] = sp->tbitsinfo[i];
                sp->cost[sp->cur_inx] = 0.2;
            }
            sp->cur_inx += 1;
        }
    }
    // printf("ID %d: %d %d\n", id++, sp->tnum, sp->cur_inx);
    sp->tnum = 0;
}


int haac_embed_global(char *in_audio, char *out_audio) {
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
                if ((i + 1 < cap &&  [i + 1] == bitidx[i]))
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

