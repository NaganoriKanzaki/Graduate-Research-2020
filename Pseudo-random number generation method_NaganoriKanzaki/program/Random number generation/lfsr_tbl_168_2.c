#include <stdio.h>
#include <stdlib.h>

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;

int main(int argc, char *argv[]){
  int now_tap, i, k, tap_num, rand;
  int taps[5];
  int bit_num = atoi(argv[1]);
  FILE *fp;

  if(bit_num > 168 || bit_num < 3){
    printf("bit_num Error.\n");
    exit(-1);
  }

  for(i = 0; i < 5; i++){
    taps[i] = 0;
  }

  if((fp = fopen("taps.tbl","r")) == NULL){
    printf("taps.tbl File cannot open.\n");
    exit(-1);
  }
  
  fscanf(fp,"%d",&now_tap);
  while(now_tap != bit_num){
    fscanf(fp,"%d",&now_tap);
  }
  
  if(now_tap == 168){
    for(i = 0; i < 3;i++){
      fscanf(fp,"%d",&now_tap);
      taps[i] = bit_num-now_tap;
    }
  }
  else{
    fscanf(fp,"%d",&now_tap);
    i = 0;
    while(now_tap < bit_num){
      taps[i] = bit_num-now_tap;
      fscanf(fp,"%d",&now_tap);
      i++;
    }
  }

  tap_num = i;
  
  fclose(fp); 
 
  /*
  for(i = 0; i < 5; i++){
    printf("%d ", taps[i]);
  }
  printf("\n");
  exit(-1);
  */  

  //ファイルに乱数書き込む
  if((fp = fopen("lfsr_tbl_168.out", "w")) == NULL){
    printf("cannot open\n"); 
    exit(1);    
  }

  fprintf(fp, "%d\n", bit_num);
  if(bit_num <= 64){
    uint64_t reg = 0xACE1BF49 & (multi(bit_num+1)-1);
    uint64_t bit;
    for (k = 0; k < 100000; k++) {
      bit = (reg & 1);
      for(i = 0; i < tap_num; i++){
	bit ^=  ((reg & multi(taps[i])) >> taps[i]);
      }
      reg = (reg >> 1) | (bit << bit_num-1);
      //ふぇーずしふた
      rand = reg;
      if(reg & 1){
	for(i = 1; i < bit_num; i++){
	  rand ^= (1 << i); 
	}
      }
      fprintf(fp, "%llx\n", rand);
    }
  }
  else if(bit_num > 64 && bit_num <= 128){
    uint64_t reg = 0xACE1BF49;
    uint64_t reg2 = 0;
    uint64_t bit;
    for (k = 0; k < 100000; k++) {
      bit = (reg & 1);
      for(i = 0; i < tap_num; i++){
	if(taps[i] < 64){
	  bit ^=  ((reg & multi(taps[i])) >> taps[i]);
	}
	else if(taps[i] >= 64){
	  bit ^=  ((reg2 & multi(taps[i]-64)) >> (taps[i]-64));
	}
      }
      reg = (reg >> 1) | ((reg2 & 1) << 63);
      reg2 = (reg2 >> 1) | (bit << (bit_num-65));
      fprintf(fp, "%llu %llu\n", reg2,reg);
    }
  }
  else{
    uint64_t reg = 0xACE1BF49;
    uint64_t reg2 = 0;
    uint64_t reg3 = 0;
    uint64_t bit;
    for (k = 0; k < 100000; k++) {
      bit = (reg & 1);
      for(i = 0; i < tap_num; i++){
	if(taps[i] < 64){
	  bit ^=  ((reg & multi(taps[i])) >> taps[i]);
	}
	else if(taps[i] >= 64 && taps[i] < 128){
	  bit ^=  ((reg2 & multi(taps[i]-64)) >> (taps[i]-64));
	}
	else if(taps[i] >= 128){
	  bit ^=  ((reg3 & multi(taps[i]-128)) >> (taps[i]-128));
	}
      }
      reg = (reg >> 1) | ((reg2 & 1) << 63);
      reg2 = (reg2 >> 1) | ((reg3 & 1) << 63);
      reg3 = (reg3 >> 1) | (bit << (bit_num-129));
      fprintf(fp, "%llu %llu %llu\n", reg3, reg2 ,reg);
    }
  }

  fclose(fp);

  return 0;
}

int multi(int index){
  int i, x = 1;
  for(i = 0; i < index; i++){
    x*=2;
  }
  return x;
}

