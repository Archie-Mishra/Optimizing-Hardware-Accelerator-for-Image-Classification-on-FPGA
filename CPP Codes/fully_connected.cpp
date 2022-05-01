#include "fully_connected.h"

void fully_connected_layer(hls::stream<bit> &axi_input,hls::stream<bit> &axi_output,hls::stream<WT_DATA> &weight_input,bit interrupt_flag)
{

#pragma HLS INTERFACE axis port=weight_input
#pragma HLS INTERFACE s_axilite port=interrupt_flag
#pragma HLS INTERFACE axis port=axi_output
#pragma HLS INTERFACE axis port=axi_input

	static OUT_DATA FILTER_WEIGHT[IN_MAP];
	static ACCM_DATA LIMIT[OUT_MAP];

	if (interrupt_flag)
	{
		interrupt_flag = 0;
		OUT_DATA FILTER_WEIGHT_R;
		WT_DATA INPUT_WEIGHT;

		for(int i=0; i < IN_MAP; i++)
		{
			for (int j=0; j < OUT_MAP; j++)
			{
				int j64 = j % 64;
				if (j64 == 0)
					INPUT_WEIGHT = weight_input.read();

				FILTER_WEIGHT_R[j] = INPUT_WEIGHT[j64];
			}

			FILTER_WEIGHT[i] = FILTER_WEIGHT_R;
		}

                for(int n=0; n < OUT_MAP; n++)
		{
			LIMIT[n] = (ACCM_DATA) weight_input.read(); 
		}

	}

	bit input_data[IN_MAP];

	ACCM_DATA store[POUT];
	#pragma HLS ARRAY_PARTITION variable=store complete

	for(int t=0; t < OUT_MAP / POUT; t++)
	{
                int s=0;
                while(s < POUT)
		{
                        #pragma HLS UNROLL
			store[s] = 0;
                        s++;
		}

		for(int u=0; u < IN_MAP; u++)
		{
                        #pragma HLS PIPELINE II=1
			if (t == 0)
				input_data[u] = axi_input.read();

                        int v=0;
                        while(v < POUT)
			{
				if (input_data[u] == FILTER_WEIGHT[u].get_bit(t*POUT + v))
					store[v]=store[v] + 1;
                                v++;
			}

		  }

	        int w=0;
                while(w<POUT)
		{
	                #pragma HLS PIPELINE II=1
			if (store[w] > LIMIT[t*POUT + w])
				axi_output.write(1);
			else
				axi_output.write(0);
                        w++;
		}
	}

}
