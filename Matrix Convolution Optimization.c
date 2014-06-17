#include <smmintrin.h>
#include <stdio.h>
#include "xmmintrin.h"
/*Matrix convolution optimization
 *This function works with different size of kernel
 *Achieve 98 Gflops using openMP, loop unrolling, SSE instructions, matrix padding, register/cache blocking.
 */

int conv2D(const float* in, float* out, const int data_size_X, const int data_size_Y,
        const float* kernel, const int kernel_x, const int kernel_y)
{
    const int kern_cent_X = (kernel_x - 1)/2;
    const int kern_cent_Y = (kernel_y - 1)/2;
    const int size_X = data_size_X;
    const int size_Y = data_size_Y;
    const int max = kernel_x*kernel_y;
    const int plusX = size_X + kern_cent_X*2;
    const int plusY = size_Y + kern_cent_Y*2;
    float padding[(data_size_X + kern_cent_X*2)*(data_size_Y +kern_cent_Y*2)];
    const int unroll = 9;
    #pragma omp parallel for
    for (int i = 0; i < size_X + kern_cent_X*2; i++) {
      for(int j = 0; j < kern_cent_Y/3*3; j+=3){
        padding[i + j*(size_X + kern_cent_X*2)] = 0;
        padding[i + (j+1)*(size_X + kern_cent_X*2)] = 0;
        padding[i + (j+2)*(size_X + kern_cent_X*2)] = 0;
        padding[(size_Y + kern_cent_Y) * (size_X + kern_cent_X*2) + i + j*(size_X + kern_cent_X*2)] = 0;
        padding[(size_Y + kern_cent_Y) * (size_X + kern_cent_X*2) + i + (j+1)*(size_X + kern_cent_X*2)] = 0;
        padding[(size_Y + kern_cent_Y) * (size_X + kern_cent_X*2) + i + (j+2)*(size_X + kern_cent_X*2)] = 0;
      }
      for(int j = kern_cent_Y/3*3; j < kern_cent_Y; j++){
        padding[i + j*(size_X + kern_cent_X*2)] = 0;
        padding[(size_Y + kern_cent_Y) * (size_X + kern_cent_X*2) + i + j*(size_X + kern_cent_X*2)] = 0;
      }
    } 
    #pragma omp parallel for 
    for(int j = kern_cent_Y; j < size_Y + kern_cent_Y; j++){
        int i;
        for( i = 0; i < kern_cent_X; i++){
            padding[j*(size_X + kern_cent_X*2) + i] = 0;
            padding[j * (size_X + kern_cent_X*2) + i + size_X + kern_cent_X] = 0;
        }
        for(;i < size_X + kern_cent_X; i++) {
            padding[j * (size_X + kern_cent_X*2) + i] = in[(j - kern_cent_Y)*size_X + i - kern_cent_X];
        }
    }
   __m128 kernelArray[max];
    for(int i = 0; i < max; i++)
        kernelArray[i] = _mm_set_ps1(kernel[i]);
    // main convolution loop
   #pragma omp parallel for  
    for(int y = 0; y < size_Y; y++){ // the y coordinate of the output location we're focusing on
        for(int x = 0; x < size_X/20 * 20; x+=20){ // the x coordinate of the output location we're focusing on
            __m128 add1 = _mm_setzero_ps();
            __m128 add2 = _mm_setzero_ps();
            __m128 add3 = _mm_setzero_ps();
            __m128 add4 = _mm_setzero_ps();
            __m128 add5 = _mm_setzero_ps();
            float *address = out + x + y*size_X;
            for(int i = -kern_cent_X; i <= kern_cent_X; i++){ // kernel unflipped x coordinate  
                for(int j = -kern_cent_Y; j <= kern_cent_Y; j++){ // kernel unflipped y coordinate
                    int kernelPos = (kern_cent_X-i)+(kern_cent_Y-j)*kernel_x;
                    __m128 currentVector = kernelArray[kernelPos];
                    add1 = _mm_add_ps( add1, _mm_mul_ps(currentVector, _mm_loadu_ps(padding+(x+i+kern_cent_X)+(y+j+kern_cent_Y)*plusX)));
                    add2 = _mm_add_ps( add2, _mm_mul_ps(currentVector, _mm_loadu_ps(padding+(x+i+kern_cent_X)+(y+j+kern_cent_Y)*plusX+4)));
                    add3 = _mm_add_ps( add3, _mm_mul_ps(currentVector, _mm_loadu_ps(padding+(x+i+kern_cent_X)+(y+j+kern_cent_Y)*plusX+8)));
                    add4 = _mm_add_ps( add4, _mm_mul_ps(currentVector, _mm_loadu_ps(padding+(x+i+kern_cent_X)+(y+j+kern_cent_Y)*plusX+12)));
                    add5 = _mm_add_ps( add5, _mm_mul_ps(currentVector, _mm_loadu_ps(padding+(x+i+kern_cent_X)+(y+j+kern_cent_Y)*plusX+16)));
                }
            }
            _mm_storeu_ps(address, add1);
            _mm_storeu_ps(address + 4, add2);
            _mm_storeu_ps(address + 8, add3);
            _mm_storeu_ps(address + 12, add4);
            _mm_storeu_ps(address + 16, add5);
        }        
        // out of bound
        for(int x = size_X/20*20; x < data_size_X; x++){
            for(int i = -kern_cent_X; i <= kern_cent_X; i++){ // kernel unflipped x coordinate  
                for(int j = -kern_cent_Y; j <= kern_cent_Y; j++){ // kernel unflipped y coordinate
                    float kvalue = kernel[(kern_cent_X-i)+(kern_cent_Y-j)*kernel_x];
                    out[x + y*data_size_X] += kvalue * padding[(x+i+kern_cent_X) + (y+j+kern_cent_Y)*(data_size_X+kern_cent_X*2)];
                }       
            }
        }
    }
    return 1;
}
