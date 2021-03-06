#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define K0 4 
#define UINT_MAX  4294967295

typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;

int main(int argc, char *argv[]){
  int i, j, k, bit_num, f[K0], rand_num; 
  double interval = (UINT_MAX+1)/K0;
  double theory_val, xx;
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
  /*
    for(i = 0; i < 100; i++) { 
    for(j = 0; j < bit_num/64; j++){
    printf("%llu\n", rand[i][j]);
    }
    }
  */
  for(k = 0; k < K0 ; k++) {
    f[k]= 0;
  }

  for(i = 0; i < rand_num; i++) {
    for(k = 0; k < K0; k++) {
      if( (0+(interval*k) < rand[i]) && (rand[i] < (interval*(k+1)-1)) ){
	f[k]++;
      }
    }
  }

  theory_val = rand_num/K0;
  for(k = 0; k < K0; k++) {
    xx += ((f[k]-theory_val)*(f[k]-theory_val))/theory_val; 
  }

  /*
  for(k = 0; k < K; k++) {
    printf("%d\n", f[k]);
  }
  */

  printf("自由度 = %d\n", K0-1);
  printf("かい2乗値 = %.2lf\n", xx);


  /*
  printf("理論値 = %.1lf\n", theory_val);
  printf("範囲 = %.1lf\n", interval);
  printf("乱数列の数 = %d\n", rand_num);
  printf("%llu\n", UINT_MAX+1);
  */
  for(j = 0; j < K0; j++) {
    printf("%.0lf ", (theory_val-f[j]) );
  }
  printf("\n");
  

  free(rand);

  return 0;
}
