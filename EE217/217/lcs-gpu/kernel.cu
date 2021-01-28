#include <stdio.h>
#include <cmath>
#define BLOCK_SIZE 16

__global__ void LCS_kenel(int map_row, int map_col, const char *stringA, const char *stringB, int *map, int i) {

    int x = blockIdx.x * blockDim.x + threadIdx.x;
    int y = blockIdx.y * blockDim.y + threadIdx.y;
    int cur_element = x + y * map_col;

    // if(x < map_col && y < map_row){
    //     if(map[cur_element] < 0){
    //         map[cur_element]++;
    //     }
    //     else if(map[cur_element] == 0){
    //         if(x == 0 || y == 0) map[cur_element] = 0;
    //         else if(stringA[x - 1] == stringB[y - 1]){
    //             int pre_element = (y - 1) * map_col + (x - 1);
    //             map[cur_element] = map[pre_element] + 1;
    //         }
    //         else{
    //             int cur_left = y * map_col + (x - 1);
    //             int cur_up = (y - 1) * map_col + x;
    //             if(map[cur_up] > map[cur_left]) map[cur_element] = map[cur_up];
    //             else map[cur_element] = map[cur_left];
    //         }
    //     }
    // }

    if( 0 < x && x < map_col && 0 < y && y < map_row && x + y == i){
        if(stringA[x - 1] == stringB[y - 1]){
            int pre_element = (y - 1) * map_col + (x - 1);
            map[cur_element] = map[pre_element] + 1;
        }else{
            int cur_left = y * map_col + (x - 1);
            int cur_up = (y - 1) * map_col + x;
            if(map[cur_up] > map[cur_left]) map[cur_element] = map[cur_up];
            else map[cur_element] = map[cur_left];
        }
    }
}

void computeLCS(int map_row, int map_col, const char *stringA, const char *stringB, int *map)
{
    // Initialize thread block and kernel grid dimensions ---------------------

    dim3 DimGrid((map_col-1)/BLOCK_SIZE + 1, (map_row-1)/ BLOCK_SIZE + 1, 1);
    dim3 DimBlock(BLOCK_SIZE, BLOCK_SIZE, 1);

    // Invoke CUDA kernel -----------------------------------------------------

    for(int i = 2; i < map_row + map_col; i++){
        LCS_kenel<<<DimGrid, DimBlock>>>(map_row, map_col, stringA, stringB, map, i);
    }
}

__global__ void LCS_kernel_optimized(int row, int col, const char *X, const char *Y, int *dp, int i) {
    int offset = blockIdx.x * blockDim.x + threadIdx.x;
    int x = i - offset;
    int y = offset;
    int cur_element = x + y * col;

    if( 0 < x && x < col && 0 < y && y < row && x + y == i){
        if(X[x - 1] == Y[y - 1]){
            int pre_element = (y - 1) * col + (x - 1);
            dp[cur_element] = dp[pre_element] + 1;
        }else{
            int cur_left = y * col + (x - 1);
            int cur_up = (y - 1) * col + x;
            if(dp[cur_up] > dp[cur_left]) dp[cur_element] = dp[cur_up];
            else dp[cur_element] = dp[cur_left];
        }
    }
}

void computeLCS_optimized(int row, int col, const char *X, const char *Y, int *dp){
    const int block_size = 256;
    int max_n = sqrt((double)(row * row + col * col))+1;
    dim3 DimGrid((max_n-1)/block_size + 1, 1, 1);
    dim3 DimBlock(block_size, 1, 1);
    for(int i = 0; i < max_n; i++){
        LCS_kernel_optimized<<<DimGrid, DimBlock>>>(row, col, X, Y, dp, i);
    }
}

