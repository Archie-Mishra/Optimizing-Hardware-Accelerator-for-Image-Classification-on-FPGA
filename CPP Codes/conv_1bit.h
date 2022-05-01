#define PRAGMA_SUB(x) _Pragma (#x)
#define DO_PRAGMA(x) PRAGMA_SUB(x)

#include "ap_int.h"
#include "hls_stream.h"
#include <math.h>
#define BIT_ACCURATE

#define IMG_ROW 64
#define IMG_COL 64
#define ACCM 11
#define IN_MAP 64
#define OUT_MAP 64
#define KERNEL 3
#define PIN 64
#define POUT 1  

typedef ap_uint<IN_MAP> IN_DATA;
typedef ap_uint<OUT_MAP> OUT_DATA;
typedef ap_uint<64> WT_DATA;
typedef ap_uint<ACCM> ACCM_DATA;
typedef ap_uint<1> bit;

#define KERNEL_SQ (KERNEL*KERNEL)
#define KER_BITS 2
#define KERSQ_BITS 4

void convolution_1bit(hls::stream<IN_DATA> &axi_input, hls::stream<OUT_DATA> &axi_output, bit interrupt_flag, hls::stream<WT_DATA> &weight_input);

