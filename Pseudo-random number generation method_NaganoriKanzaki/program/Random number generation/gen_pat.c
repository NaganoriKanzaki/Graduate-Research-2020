#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;

int main(int argc, char *argv[]){
  int i, j, k, bit_num, sig_num, cnt = 0;
  uint64_t rand[40000], tmp;
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


  for(i = 0; i < 40000; i++) { 
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
  strcpy(filename, "./iscas85/");
  strcat(filename, argv[3]);

  if(atoi(argv[1]) == 1){
    strcat(filename,"_lfsr.pat");
  }
  else if(atoi(argv[1]) == 2){
  strcat(filename,"_mt19937.pat");
  }

  if((fp = fopen(filename,"w")) == NULL){
    printf("out File cannot open.\n");
    exit(-1);
  }

  sig_num = atoi(argv[2]);
  fprintf(fp, " %d\n", (40000*bit_num)/sig_num);
  for(i = 0; i < 40000; i++) {
    rand[i] = rand[i] << (64-bit_num);
    for(k = 0; k < bit_num; k++){
      tmp = rand[i];
      rand[i] = tmp << 1;
      if(tmp > rand[i]){
	fprintf(fp, " 1");
      }
      else{
	fprintf(fp, " 0");
	
      }
      cnt++;
      if( (cnt%sig_num) == 0 ){
	fprintf(fp, "\n");
      }
    }
  }

  fclose(fp);

  return 0;
}
