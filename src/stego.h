#ifndef FAAD_SRC_STEGO_H
#define FAAD_SRC_STEGO_H

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned char uint8_t;

enum FAADStegaMethod {
    HAAD,
};

enum FAADStegaStep {
    EMBED,
    EXTRACT,
    GET_CAP,
    GET_COST,
};

typedef struct {
    enum FAADStegaMethod METHOD;
    enum FAADStegaStep STEP;

    int FINISH;

    // 过程变量
    unsigned char cur_byte;  // 当前处理的字节
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
    unsigned char *msg;  // 提取或者嵌入的msg，使用字节为单位
    double *cost;  // 提取出的代价，其长度大小应该为msg的8倍

    // 帧信息采集，由于是半解码，所以需要将信息抽离出来再进行修改。
    // 该部分为每帧的信息，每帧完成后再拼接至总信息当中
    int tnum;        // 数目
    uint8_t *ttype;  // 比特类型，1.联合符号位，5.单独符号位；2. escape比特LSB
    uint8_t *tmsg;   // 0还是1，以bit形式存在
    int *tbitidx;    // 在当前buffer的bit位置
    int *tbitsinfo;  // WordValue*100 + cb*10 +idx.
    int *tsepcpos;   // 所在帧的位置[0,1024)

    // HAAC隐写方法参数
    int haac_part_mode;          // 区域选择情况，输入时确定
    int haac_threshold;          // 符号翻转阈值，输入时确定
} FAADStegaCxtData;


FAADStegaCxtData *faad_stega_init(FAADStegaMethod method);
typedef struct StegaCxtData {
    int file_offset;  // 当前的文件的偏移位置

    int cap;      // 总容量
    int cur_inx;  // 当前到第几个消息

    int area;       // 1.符号位翻转，2.逃脱位，3.全选
    int threshold;  // 符号位翻转阈值

    uint8_t *msg;  // 如果没有msg，则是获取容量模式

    int *bitidx;  // 可用的比特位置,如果没有bitidx,则仅提取内容
    int *bitsinfo;

    // 如果=1，则直接比特位翻转
    // 否则：
    // 1. 最高位表示该值所使用的码书，1,2,5,6 四种码书
    //    - 如果是1或者2号码书，（id = 40+27x+9y+3v+w）
    //    - 如果是5或者6号码书，（id = 40+27x+y）
    // 2. 次高位表示该值所处的位置，1,2,[3,4]

    double *cost;  // 比特对应系数的修改代价

    // 每个频带单独计算
    int tnum;        // 数目
    uint8_t *ttype;  // 比特类型，1.符号位，2. 逃脱位
    uint8_t *tmsg;   // 0还是1
    int *tbitidx;    // 在当前buffer的bit位置
    int *tbitsinfo;  // WordValue*100 + cb*10 +idx.
    int *tsepcpos;   // 所在帧的位置[0,1024)
} StegaCxtData;

StegaCxtData *initStegaData(int cap, int area, int threshold, uint8_t *msg,
                            int *bitidx, int *bitsinfo, double *cost);

int checkHcbEnable(int cb, int offset);

void closeStegaData(StegaCxtData *sp);

void flushStegaData(StegaCxtData *sp, short *spectral_data);


FAADStegaCxtData *faad_stega_init(enum FAADStegaMethod method);
void faad_frame_flush(FAADStegaCxtData *sp);

#ifdef __cplusplus
}
#endif

#endif
