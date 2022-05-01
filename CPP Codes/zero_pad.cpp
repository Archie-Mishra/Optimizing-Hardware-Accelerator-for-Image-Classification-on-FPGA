#include "zero_pad.h"

void zero_padding(hls::stream<in_type> &axi_input, hls::stream<in_type> &axi_output) 
{
#pragma HLS INTERFACE axis port=axi_output
#pragma HLS INTERFACE axis port=axi_input

	for(int r=0; r < IMG_ROW + 2*STRIDE; r++)
		for(int c=0; c < IMG_COL + 2*STRIDE; c++)
		{
                        #pragma HLS PIPELINE II=2
			if(r >= STRIDE and r < IMG_ROW + STRIDE and c >= STRIDE and c < IMG_COL + STRIDE)
			{
				in_type data = axi_input.read();
				axi_output.write(data);
			}
			else
			{
				axi_output.write(0);
			}

		}

}
