#include "ap_int.h"
#include "hls_stream.h"

#define STRIDE 1
#define STRIDE_VALUE 0
#define IN_MAP 32

#define IMG_ROW 32
#define IMG_COL 32

typedef ap_uint<IN_MAP> in_type;

void zero_padding(hls::stream<in_type> &axi_input, hls::stream<in_type> &axi_output);
