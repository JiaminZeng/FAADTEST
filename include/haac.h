#ifndef FAAD_HAAC_H
#define FAAD_HAAC_H

#ifdef __cplusplus
extern "C" {
#endif

int haac_get_capacity(char *in_audio, int *logic_bit_cap, int threshold = 2, int part_mode = 1, int payload = 7);
int haac_get_cost(char *in_audio, unsigned char *msg, double *cost, int cap, int threshold = 2, int part_mode = 1,
                  int payload = 7);
int haac_embed(char *in_audio, char *out_audio, unsigned char *msg, int cap, int threshold = 2, int part_mode = 1,
               int payload = 7);
int haac_extract(char *in_audio, unsigned char *msg, int cap, int threshold = 2, int part_mode = 1, int payload = 7);

#ifdef __cplusplus
}
#endif
#endif