#include "conv_1bit.h"
#include <stdio.h>

ap_uint<KERSQ_BITS> popcount_xnor(ap_uint<KERNEL_SQ> image, ap_uint<KERNEL_SQ> FILTER_WEIGHT )
{
    #pragma HLS INLINE
	ap_uint<KERSQ_BITS> accumulate = 0;
	ap_uint<KERSQ_BITS> xnor_res;

int q=0;
while(q < KERSQ_BITS)
{
	xnor_res[q] = (image[q] == FILTER_WEIGHT[q])? 1:0;
	q++;

}

int j=0;
while(j<KERNEL)
	{
		ap_uint<KER_BITS> R_ACC = 0;//2
                int z=0;
                      while (z < KERNEL)
                      {
			              if(xnor_res[j*KERNEL + z] == 1)
				          R_ACC++;
                          z++;
                      }

		accumulate += R_ACC;
                j++;
	}

	return accumulate;
}


void convolution_1bit(hls::stream<IN_DATA> &axi_input, hls::stream<OUT_DATA> &axi_output,
		bit interrupt_flag, hls::stream<WT_DATA> &weight_input)

{

#pragma HLS INTERFACE axis port=weight_input
#pragma HLS INTERFACE s_axilite port=interrupt_flag
#pragma HLS INTERFACE axis port=axi_output
#pragma HLS INTERFACE axis port=axi_input

#if POUT == 1 && PIN == IN_MAP
	static ap_uint<KERNEL_SQ> FILTER_WEIGHT[OUT_MAP / POUT][IN_MAP / PIN][POUT][PIN];
	#pragma HLS ARRAY_PARTITION variable=FILTER_WEIGHT complete dim=4

#else
	static ap_uint<KERNEL_SQ> FILTER_WEIGHT[OUT_MAP][IN_MAP];
#endif

#pragma HLS RESOURCE variable=FILTER_WEIGHT core=RAM_1P_BRAM
	static ACCM_DATA LIMIT[OUT_MAP];

	//line buffer
	static IN_DATA LB[KERNEL - 1][IMG_COL];

	//processing window
	static IN_DATA WIN[KERNEL][KERNEL];

	if(interrupt_flag)
	{
		interrupt_flag = 0;

		//Read weights
#if POUT == 1 && PIN == IN_MAP
		for(int a=0; a < OUT_MAP / POUT; a++)
			for(int b=0; b < IN_MAP / PIN; b++)
				for(int c=0; c < POUT; c++)
					for(int d=0; d < PIN; d++)
						FILTER_WEIGHT[a][b][c][d] = (ap_uint<KERNEL_SQ>) weight_input.read();

#else
		for(int a=0; a < OUT_MAP / POUT; a++)
				for(int b=0; b < IN_MAP; b++)
					FILTER_WEIGHT[a][b] = (ap_uint<KERNEL_SQ>) weight_input.read();
#endif

		//Read thresholds
                   int a=0;
                   while(a < OUT_MAP)
                   {
			            LIMIT[a] = (ACCM_DATA) weight_input.read();
                        a++;
                   }
	}


	OUT_DATA TEMP;

	 for(int r = 0; r < IMG_ROW; r++)
         {
		 for(int C = 0; C < IMG_COL; C++)
		{

#if POUT == 1 && PIN == IN_MAP

			 for(int x = 0; x < OUT_MAP / POUT; x++)
			{
#pragma HLS PIPELINE II=2
				 for(int y = 0; y < IN_MAP / PIN; y++)
				 {
					if (x == 0 && y == 0)
					{
						// shift columns of processing window
						loop_shift_window: for(int m = 0; m < KERNEL; m++)
							 for(int n = 0; n < KERNEL-1; n++)
								WIN[m][n] = WIN[m][n+1];                                                        


						//line_buffer_temp

                             int m=0;
                             while (m<KERNEL-1)
                             {
							    WIN[m][KERNEL - 1] = LB[m][C];
                                m++;
                             }

						//shift row of line buffer

                            m=0;
                            while (m < KERNEL-2)
                            {
								LB[m][C] = LB[m+1][C];
                                m++;
                            }
								

						IN_DATA buffer = axi_input.read();
						WIN[KERNEL-1][KERNEL-1]  = buffer;
						LB[KERNEL-2][C] = buffer;
					}


					ap_uint<KERNEL_SQ> pixel[PIN];
					ACCM_DATA store[POUT];

					if (y == 0)
					{
						int c=0;
						while(c<POUT)
						{
							store[c] = 0;
							c++;
						}
					}

					for(int g = 0; g < PIN; g++)
						for(int h=0; h < KERNEL; h++)
							for(int i=0; i < KERNEL; i++)
								pixel[g][h*KERNEL + i] = WIN[h][i][y * PIN + g];

					// Compute P_IN * P_OUT convolutions

					for(int c = 0; c < POUT; c++)
					{
#pragma HLS PIPELINE II=2
						for(int d=0; d < PIN; d++)
						{
							store[c] += popcount_xnor(pixel[d], FILTER_WEIGHT[x][y][c][d]);

						}
					}


					if (y == IN_MAP / PIN - 1)
					{
                        int l=0;
                        while(l<POUT)
						{
							int threshold = x*POUT + l;
							TEMP[threshold]=(store[l] >= LIMIT[threshold])?1:0;
							l++;
						}


					}

					if (y == IN_MAP / PIN - 1 && x == OUT_MAP / POUT - 1)
					{
						if (r >= KERNEL - 1 && C >= KERNEL - 1)
							axi_output.write(TEMP);
					}


				}//y loop

			}//x loop

#else

				 for(int c = 0; c < OUT_MAP; c++)
				{
					if (c == 0)
					{
						// shift columns of processing window
						 for(int m = 0; m < KERNEL; m++)
							for(int n = 0; n < KERNEL-1; n++)
								WIN[m][n] = WIN[m][n+1];


						//line_buffer_temp

                         int m=0;
                         while (m<KERNEL-1)
                         {
								WIN[m][KERNEL - 1] = LB[m][C];
                                m++;
                         }

						//shift row of line buffer
						 m=0;
                         while (m < KERNEL-2)
                         {
								LB[m][C] = LB[m+1][C];
                                m++;
                         }
						

						IN_DATA buffer = input_stream.read();
						WIN[KERNEL-1][KERNEL-1]  = buffer;
						LB[KERNEL-2][C] = buffer;
					}

					if (r >= KERNEL - 1 && C >= KERNEL - 1)
					{
						ap_uint<KERNEL_SQ> pixel[IN_MAP];
						ACCM_DATA store = 0;

						for(int u = 0; u < IN_MAP; u++)
							for(int v=0; v < KERNEL; v++)
								for(int w=0; w < KERNEL; w++)
									pixel[u][v*KERNEL + w] = WIN[v][w][u];

						//Compute P_IN * P_OUT convolutions


                        int u =0;
                        while (u < IN_MAP)
						{
							store += multiplier(pixel[u], FILTER_WEIGHT[c][u]);
                            u++;
						}

						TEMP[c]=(store>=LIMIT[c])?1:0;

						if (c == OUT_MAP - 1)
						{
							output_stream.write(TEMP);
						}
					}


				}

#endif

		} //loop columns
	}//loop rows
}
