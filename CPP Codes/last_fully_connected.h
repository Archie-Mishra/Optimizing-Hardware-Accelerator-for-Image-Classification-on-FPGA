#include "ap_int.h"
#include "hls_stream.h"

#define INP_W 10
#define IMG_TYPE 10

typedef ap_uint<INP_W> in_type;
typedef ap_uint<32> u32;
typedef unsigned int uint;

void last_fully_connected(hls::stream<in_type> &axi_input, uint* img_class);
