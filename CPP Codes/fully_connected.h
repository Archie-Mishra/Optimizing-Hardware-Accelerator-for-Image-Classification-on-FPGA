#define IN_MAP 256
#define OUT_MAP 256
#define POUT 32

#include "hls_stream.h"
#include "ap_int.h"

#if IN_MAP == 1
#define ACCM 1
#elif IN_MAP == 2 and IN_MAP == 3
#define ACCM 2
#elif IN_MAP >= 4 and IN_MAP < 8
#define ACCM 3
#elif IN_MAP >= 8 and IN_MAP < 16
#define ACCM 4
#elif IN_MAP >= 16 and IN_MAP < 32
#define ACCM 5
#elif IN_MAP >= 32 and IN_MAP < 64
#define ACCM 6
#elif IN_MAP >= 64 and IN_MAP < 128
#define ACCM 7
#elif IN_MAP >= 128 and IN_MAP < 256
#define ACCM 8
#elif IN_MAP >= 256 and IN_MAP < 512
#define ACCM 9
#elif IN_MAP >= 512 and IN_MAP < 1024
#define ACCM 10
#elif IN_MAP >= 1024 and IN_MAP < 2048
#define ACCM 11
#elif IN_MAP >= 2048 and IN_MAP < 4096
#define ACCM 12
#elif IN_MAP >= 4096
#define ACCM 13
#endif

typedef ap_uint<1> bit;
typedef ap_uint<IN_MAP> IN_DATA;
typedef ap_uint<OUT_MAP> OUT_DATA;
typedef ap_uint<ACCM> ACCM_DATA;
typedef ap_uint<64> WT_DATA;

void fully_connected_layer(hls::stream<bit> &axi_input,hls::stream<bit> &axi_output,hls::stream<WT_DATA> &weight_input,bit interrupt_flag);

