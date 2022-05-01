#include "int_conv.h"
#include <stdio.h>


FIXED_DATA accumulate_sign(FIXED_DATA image[KERNEL][KERNEL], ap_uint<KERNEL_SQ> FILTER_WEIGHT)
{
#pragma HLS INLINE

	FIXED_DATA accumulate = 0;

	for(int i=0; i < KERNEL; i++)
	{
		FIXED_DATA R_ACC = 0;
		for(int j=0; j < KERNEL; j++)
                        R_ACC=(FILTER_WEIGHT[i*KERNEL +j] == 0)? R_ACC-image[i][j]:R_ACC+image[i][j];

		accumulate += R_ACC;
	}

	return accumulate;
}

void integer_convolution(hls::stream<FIXED_DATA> &axi_input, hls::stream<OUT_DATA> &axi_output, bit interrupt_flag, hls::stream<WT_DATA> &weight_input)
{

#pragma HLS INTERFACE axis port=weight_input
#pragma HLS INTERFACE s_axilite port=interrupt_flag
#pragma HLS INTERFACE axis port=axi_output
#pragma HLS INTERFACE axis port=axi_input

	static ap_uint<KERNEL_SQ> FILTER_WEIGHT[OUT_MAP];
	static FIXED_DATA LIMIT[OUT_MAP];


#if POUT > 1 && POUT < OUT_MAP
	#pragma HLS ARRAY_PARTITION variable=FILTER_WEIGHT cyclic factor=4 dim=1
	#pragma HLS ARRAY_PARTITION variable=LIMIT cyclic factor=4 dim=1
#else
	#pragma HLS ARRAY_PARTITION variable=FILTER_WEIGHT complete dim=1
	#pragma HLS ARRAY_PARTITION variable=LIMIT complete dim=1
#endif

#pragma HLS RESOURCE variable=FILTER_WEIGHT core=RAM_1P_BRAM


	//line buffer
	static FIXED_DATA LB[KERNEL - 1][IMG_COL];
	#pragma HLS ARRAY_PARTITION variable=LB complete dim=1

	//processing WIN
	static FIXED_DATA WIN[KERNEL][KERNEL];
	#pragma HLS ARRAY_PARTITION variable=WIN complete dim=0


	if(interrupt_flag)
	{
		interrupt_flag = 0;
                int f=0;
                
		while(f < OUT_MAP)
                {
			FILTER_WEIGHT[f] = (ap_uint<KERNEL_SQ>) weight_input.read(); 
			LIMIT[f] = (FIXED_DATA) weight_input.read();
                        f++;
                 }
	}

	OUT_DATA TEMP;


	for(int r = 0; r < IMG_ROW; r++) 
        {
		for(int c = 0; c < IMG_COL; c++)
		{

                 #if POUT < OUT_MAP
			for(int x=0; x < OUT_MAP / POUT; x++)
			{

	#pragma HLS PIPELINE II=2

				if(x == 0)
				{
					// shift columns of processing window
					for(int m = 0; m < KERNEL; m++)
						for(int n = 0; n < KERNEL-1; n++)
							WIN[m][n] = WIN[m][n+1];

                                        int m = 0;
			                while(m < KERNEL - 1)
                                        {
				          WIN[m][KERNEL - 1] = LB[m][c];
                                          m++;
                                        }


			                //shift row of line buffer
                                        m = 0;
			                while(m < KERNEL-2)
                                        {
				          LB[m][c] = LB[m+1][c];
                                          m++;
                                        }

				        FIXED_DATA buffer = axi_input.read();
				        WIN[KERNEL-1][KERNEL-1]  = buffer;
				        LB[KERNEL-2][c] = buffer;
				}

				for(int d = 0; d < POUT; d++)
				{
					int threshold = x * POUT + d;

					FIXED_DATA store = accumulate_sign(WIN, FILTER_WEIGHT[threshold]);

                                        TEMP[threshold]=(store >= LIMIT[threshold])?1:0;
				}

				if(x == OUT_MAP / POUT - 1)
				{
					if (r >= KERNEL - 1 && c >= KERNEL - 1)
						axi_output.write(TEMP);
				}

			}
#else

#pragma HLS PIPELINE II=1

			// shift columns of processing window
                        for(int m = 0;m < KERNEL; m++)
				for(int n = 0; n < KERNEL-1; n++)
					WIN[m][n] = WIN[m][n+1];


                        int m = 0;
			while(m < KERNEL - 1)
                        {
				WIN[m][KERNEL - 1] = LB[m][c];
                                m++;
                        }


			//shift row of line buffer
                        m = 0;
			while(m < KERNEL-2)
                        {
				LB[m][c] = LB[m+1][c];
                                m++;
                        }


			FIXED_DATA buffer = axi_input.read();
			WIN[KERNEL-1][KERNEL-1]  = buffer;
			LB[KERNEL-2][c] = buffer;

                        int f = 0
			while(f < OUT_MAP)
			{
				FIXED_DATA store = accumulate_sign(WIN, FILTER_WEIGHT[f]);
				TEMP[c]=(store>=LIMIT[c])?1:0;
                                f++;
			}


			if (r >= KERNEL - 1 && c >= KERNEL - 1)
			{
				axi_output.write(TEMP);
			}
#endif
		}
}
