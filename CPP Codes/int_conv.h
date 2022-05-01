#include "ap_int.h"
#include "ap_fixed.h"
#include "hls_stream.h"

#define BIT_ACCURATE


#define IMG_ROW 32
#define IMG_COL 32

#define OUT_MAP 32
#define POUT 32

#define KERNEL 3

#define KERNEL_SQ (KERNEL*KERNEL)

typedef ap_fixed<13,5> FIXED_DATA;

typedef ap_uint<OUT_MAP> OUT_DATA;
typedef ap_uint<64> WT_DATA;
typedef ap_uint<1> bit;

void integer_convolution(hls::stream<FIXED_DATA> &axi_input, hls::stream<OUT_DATA> &axi_output, bit interrupt_flag, hls::stream<WT_DATA> &weight_input);
