#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;

int main(int argc, char *argv[]){
  int i, j, k, bit_num, sig_num;
  uint64_t rand[100000], tmp, rand_vert;
  char filename[60];
  FILE *fp;


  if(atoi(argv[1]) == 1){
    strcpy(filename,"lfsr_tbl_168.out");
  }
  else if(atoi(argv[1]) == 2){
    strcpy(filename,"mt19937_160.out");
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


  for(i = 0; i < 100000; i++) { 
    if(fscanf(fp,"%llx",&rand[i]) != 1){
      printf("rand scan Error \n");
      exit(-1);
    }
  }

  fclose(fp);

  /*
    for(i = 0; i < 100; i++) { 
    for(j = 0; j < bit_num/64; j++){
    printf("%llx\n", rand[i][j]);
    }
    }
  */

  if(atoi(argv[1]) == 1){
    strcpy(filename,"lfsr_tbl_168_vert.out");
  }
  else if(atoi(argv[1]) == 2){
    strcpy(filename,"mt19937_160_vert.out");
  }

  if((fp = fopen(filename,"w")) == NULL){
    printf("out File cannot open.\n");
    exit(-1);
  }

  fprintf(fp, "%d\n", bit_num);
  
  sig_num = atoi(argv[2]);
  for(j = 0; j < 100000/sig_num; j++) {
    for(k = 0; k < bit_num; k++){
      rand_vert = 0;
      for(i = 0; i < sig_num; i++) {
	rand_vert = rand_vert << 1;
	if(k == 0){
	  rand[j*sig_num+i] = rand[j*sig_num+i] << (64-bit_num);
	}
	tmp = rand[j*sig_num+i];
	rand[j*sig_num+i] = tmp << 1;
	if(tmp > rand[j*sig_num+i]){
	  rand_vert = rand_vert | 1;
	}
      }
      fprintf(fp, "%llx\n", rand_vert);
    }
  }

  fclose(fp);

  return 0;
}
