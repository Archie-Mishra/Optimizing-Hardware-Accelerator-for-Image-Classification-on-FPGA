#include "last_fully_connected.h"
#include<cstring>

void last_fully_connected(hls::stream<in_type> &axi_input, uint* img_class)
{

#pragma HLS INTERFACE s_axilite port=return bundle=BUS
#pragma HLS INTERFACE m_axi depth=10 port=img_class bundle=BUS
#pragma HLS INTERFACE axis  port=axi_input

	static u32 store[IMG_TYPE];

	for(int i=0; i < IMG_TYPE; i++) 
	{
                #pragma HLS PIPELINE II=1
		in_type data= axi_input.read();
		store[i] = data;
	}

	memcpy((uint*) img_class, (uint*) store, IMG_TYPE * sizeof(uint));

}
