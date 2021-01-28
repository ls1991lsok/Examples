#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "support.h"

void verify(const char *stringA, const char *stringB, int *map, unsigned int map_col, unsigned int map_row){

  const float relativeTolerance = 1e-6;
  unsigned int count = 0;
  int *dp = (int*)malloc(map_row * map_col * sizeof(int));
  int *print = (int*)malloc(map_row * map_col * sizeof(int));

  for(int i = 0; i < map_row; i++){
    for(int j = 0; j < map_col; j++){
      dp[i * map_col + j] = 0;
      print[i * map_col + j] = 0;
    }
  }

  for(int i = 1; i < map_row; i++) {
    for(int j = 1; j < map_col; j++) {
      if(stringB[i - 1] == stringA[j - 1]){
        dp[i * map_col + j] = 1 + dp[(i - 1) * map_col + (j - 1)];
        print[i * map_col + j] = 5;
      }
      else{
        dp[i * map_col + j] = max(dp[(i - 1) * map_col + j], dp[i * map_col + (j - 1)]);
        print[i * map_col + j] = dp[(i - 1) * map_col + j] >= dp[i * map_col + (j - 1)]? 7 : 9;
      }
      count++;
      float relativeError = dp[i * map_col + j] - map[i*(map_col) + j];
      if (relativeError > relativeTolerance
        || relativeError < -relativeTolerance) {
        printf("\nTEST FAILED %u\n\n",count);
        exit(1);
      }
    }
  }
  printf("TEST PASSED %u\n\n", count);
  printf("The Longest Common Subsequence is:  ");
  printLCS((int*)print, stringB, map_col, map_row, map_col);
  printf("\n");

  free(dp);
  free(print);
}

void printLCS(int *print, const char *stringB, int row_size, int map_row, int map_col){
  int stringB_size = map_row - 1;
  int stringA_size = map_col - 1;
  if(stringB_size == 0 || stringA_size == 0) return;
  if(print[stringB_size * row_size + stringA_size] == 5){
    printLCS(print, stringB, row_size, map_row - 1, map_col - 1);
    printf("%c", stringB[stringB_size - 1]);
  }else if(print[stringB_size * row_size + stringA_size] == 7){
    printLCS(print, stringB, row_size, map_row - 1, map_col);
  }else if(print[stringB_size * row_size + stringA_size] == 9){
    printLCS(print, stringB, row_size, map_row, map_col  - 1);
  }
}

void printRes(int *map, unsigned int map_col, unsigned int map_row){
    printf("\n");
    for(int i = 0; i < map_row; i++){
        printf("[");
        for(int j = 0; j < map_col; j++){
            printf(" %d ", map[i*map_col + j]);
        }
        printf("]\n");
    }
}

void startTime(Timer* timer) {
    gettimeofday(&(timer->startTime), NULL);
}

void stopTime(Timer* timer) {
    gettimeofday(&(timer->endTime), NULL);
}

float elapsedTime(Timer timer) {
    return ((float) ((timer.endTime.tv_sec - timer.startTime.tv_sec) \
                + (timer.endTime.tv_usec - timer.startTime.tv_usec)/1.0e6));
}

