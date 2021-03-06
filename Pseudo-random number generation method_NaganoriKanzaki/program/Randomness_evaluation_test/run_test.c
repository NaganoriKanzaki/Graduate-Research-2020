#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;

int main(int argc, char *argv[]){
  int i, j, bit_num, f[6], rand_num, run_length = 0, sum = 0; 
  double a_ary[6][6] = {
    {4529.35, 9044.9, 13568, 18091.3, 22614.7, 27892.2},
    {9044.9, 18097, 27139.5, 36186.7, 45233.8, 55788.8},
    {13568, 27139.5, 40721.3, 54281.3, 67852, 83684.6},
    {18091.3, 36186.7, 54281.3, 72413.6, 90470.1, 111580},
    {22614.7, 45233.8, 67852, 90470.1, 113262, 139476},
    {27892.2, 55788.8, 83684.6, 111580, 139476, 172860}
  };
  double b_ary[6] = {0.16667, 0.20833, 0.09167, 0.02639, 0.00575, 0.00119};
  double VN = 0;
  uint64_t *rand, tmp;
  char filename[60];
  FILE *fp;
  

  if(atoi(argv[1]) == 1){
    strcpy(filename,"lfsr_tbl_168.out");
  }
  else if(atoi(argv[1]) == 2){
  strcpy(filename,"mt19937_160.out");
  }
  if(atoi(argv[1]) == 11){
    strcpy(filename,"lfsr_tbl_168_vert.out");
  }
  else if(atoi(argv[1]) == 22){
    strcpy(filename,"mt19937_160_vert.out");
  }

  if((fp = fopen(filename,"r")) == NULL){
    printf("out File cannot open.\n");
    exit(-1);
  }

  if(fscanf(fp,"%d",&bit_num) != 1){
    printf("bit_num Error.\n");
    exit(-1);
  }
  //printf("%d\n", bit_num);


  rand_num = atoi(argv[2]);
  rand = malloc(rand_num * sizeof(unsigned long long)); 

  for(i = 0; i < rand_num; i++) { 
    if(fscanf(fp,"%llx",&rand[i]) != 1){
      printf("rand scan Error \n");
      exit(-1);
    }
  }

  fclose(fp);
  
  for(i = 0; i < 6 ; i++) {
    f[i]= 0;
  }

  for(i = 1; i < rand_num; i++) {
    if( rand[i] > rand[i-1] ){
      run_length++;
      if(run_length > 5){
	run_length = 5;
      }
    }
    else{
      f[run_length]++;
      run_length = 0;
    }
  }
  f[run_length]++;
  
  for(i = 0; i < 6; i++){
    for(j = 0; j < 6; j++){
      VN += (f[i]-rand_num*b_ary[i])*(f[j]-rand_num*b_ary[j])*a_ary[i][j];
    }
  }

  /*
  for(i = 0; i < 6; i++){
    printf("%.5lf ", b_ary[i]);
  }
  printf("\n");

  for(i = 0; i < 6; i++){
    for(j = 0; j < 6; j++){
      printf("%9.2lf ", a_ary[i][j]);
    }
    printf("\n");
  }

  */

  
  //printf result
  for(i = 0; i < 6; i++){
    printf("f[%d] = %d\n", i+1, f[i]);
  }
  

  printf("%.2lf\n", VN/rand_num);

  /*
  for(i = 0; i < 6; i++){
    sum += (i+1)*f[i];
  }
  printf(" %d\n", sum);
  */

  free(rand);

  return 0;
}
