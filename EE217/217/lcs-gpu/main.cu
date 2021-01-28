#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "kernel.cu"
#include "support.h"

int main (int argc, char *argv[])
{
    Timer timer;
    cudaError_t cuda_ret;
    srand(1);

    // Initialize host variables ----------------------------------------------

    printf("\nSetting up the problem..."); fflush(stdout);
    startTime(&timer);

    char *stringA_h, *stringB_h;
    char *stringA_d, *stringB_d;
    int *map_h, *map_d;
    size_t map_row, map_col, map_size;
    unsigned stringA_size, stringB_size;

    if (argc == 1) {
        stringA_size = stringB_size = 100;
    } else if (argc == 2) {
        stringA_size = stringB_size = atoi(argv[1]);
    } else if (argc == 3) {
        stringA_size = atoi(argv[1]);
        stringB_size = atoi(argv[2]);
    } else {
        printf("\n    Invalid input parameters!"
      "\n    Usage: ./lcs-gpu                    # Both String Sizes are 10"
      "\n    Usage: ./lcs-gpu  <m>               # Both String Sizes are m"
      "\n    Usage: ./lcs-gpu  <m> <n>           # String A Size = m, String B Size = n"
      "\n");
        exit(0);
    }

    map_col = stringA_size + 1;
    map_row = stringB_size + 1;
    map_size = map_row * map_col;

    stringA_h = (char*)malloc(sizeof(char) * stringA_size);
    for(unsigned int i = 0; i < stringA_size; i++) stringA_h[i] = 'A' + rand() % 26;

    stringB_h = (char*)malloc(sizeof(char) * stringB_size);
    for(unsigned int i = 0; i < stringB_size; i++) stringB_h[i] = 'A' + rand() % 26;

    map_h = (int*)malloc(sizeof(int) * map_size);
    for(int i = 0; i < map_row; i++){
        for(int j = 0; j < map_col; j++){
            map_h[i * map_col + j] = 0;
        }
    }

    stopTime(&timer); printf("%f s\n", elapsedTime(timer));

    // printf("    String A :");
    // for(int i = 0; i < stringA_size; i++) printf("%c", stringA_h[i]);
    // printf("\n    String B :");
    // for(int i = 0; i < stringB_size; i++) printf("%c", stringB_h[i]);
    printf("\n    String A Length: %u\n    String B Length: %u\n", stringA_size, stringB_size);

    // Allocate device variables ----------------------------------------------

    printf("Allocating device variables..."); fflush(stdout);
    startTime(&timer);

    cuda_ret = cudaMalloc((void**)&stringA_d, sizeof(char) * stringA_size);
    if(cuda_ret != cudaSuccess) FATAL("Unable to allocate device memory");

    cuda_ret = cudaMalloc((void**)&stringB_d, sizeof(char) * stringB_size);
    if(cuda_ret != cudaSuccess) FATAL("Unable to allocate device memory");

    cuda_ret = cudaMalloc((void**)&map_d, sizeof(int) * map_size);
    if(cuda_ret != cudaSuccess) FATAL("Unable to allocate device memory");

    cudaDeviceSynchronize();
    stopTime(&timer); printf("%f s\n", elapsedTime(timer));

    // Copy host variables to device ------------------------------------------

    printf("Copying data from host to device..."); fflush(stdout);
    startTime(&timer);

    cuda_ret = cudaMemcpy(stringA_d, stringA_h, sizeof(char) * stringA_size, cudaMemcpyHostToDevice);
    if(cuda_ret != cudaSuccess) FATAL("Unable to copy memory to the device");

    cuda_ret = cudaMemcpy(stringB_d, stringB_h, sizeof(char) * stringB_size, cudaMemcpyHostToDevice);
    if(cuda_ret != cudaSuccess) FATAL("Unable to copy memory to the device");

    cuda_ret = cudaMemcpy(map_d, map_h, sizeof(int) * map_size, cudaMemcpyHostToDevice);
    if(cuda_ret != cudaSuccess) FATAL("Unable to copy memory to the device");

    cudaDeviceSynchronize();
    stopTime(&timer); printf("%f s\n", elapsedTime(timer));

    // Launch kernel using standard sgemm interface ---------------------------
    printf("Launching kernel..."); fflush(stdout);
    startTime(&timer);

    computeLCS(map_row, map_col, stringA_d, stringB_d, map_d);

    cuda_ret = cudaDeviceSynchronize();
    if(cuda_ret != cudaSuccess) FATAL("Unable to launch kernel");
    stopTime(&timer); printf("%f s\n", elapsedTime(timer));

    // Copy device variables from host ----------------------------------------

    printf("Copying data from device to host..."); fflush(stdout);
    startTime(&timer);

    cuda_ret = cudaMemcpy(map_h, map_d, sizeof(int) * map_size, cudaMemcpyDeviceToHost);
    if(cuda_ret != cudaSuccess) FATAL("Unable to copy memory to the host");

    cudaDeviceSynchronize();
    stopTime(&timer); printf("%f s\n", elapsedTime(timer));

    // Verify correctness -----------------------------------------------------

    printf("Verifying results...\n"); fflush(stdout);
    startTime(&timer);

    // verify(stringA_h, stringB_h, map_h, map_col, map_row);
    // printRes(map_h, map_col, map_row);                 //Uncomment this line to print out the map of LCS

    // stopTime(&timer); printf("%f s\n", elapsedTime(timer));
    printf("The Longest Common Subsequence Length is: %d\n", map_h[map_row * map_col - 1]);


    // Free memory ------------------------------------------------------------

    free(stringA_h);
    free(stringB_h);
    free(map_h);

    cudaFree(stringA_d);
    cudaFree(stringB_d);
    cudaFree(map_d);

    return 0;

}