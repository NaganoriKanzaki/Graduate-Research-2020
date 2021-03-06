#include "declare.h"
#include "def_gtype.h"
#include "def_flt.h"
#include "math.h"
#include <stdio.h>
#include <stdlib.h>

#define PHASE 0
#define GENERATOR 0  /* =0:MT, =1:LFSR */
/* Period parameters */  
#define N 624
#define M 397
#define MATRIX_A 0x9908b0dfUL   /* constant vector a */
#define UPPER_MASK 0x80000000UL /* most significant w-r bits */
#define LOWER_MASK 0x7fffffffUL /* least significant r bits */
static unsigned long mt[N]; /* the array for the state vector  */
static int mti=N+1; /* mti==N+1 means mt[N] is not initialized */

/* initializes mt[N] with a seed */
void init_genrand(unsigned long s)
{
    mt[0]= s & 0xffffffffUL;
    for (mti=1; mti<N; mti++) {
        mt[mti] = 
	    (1812433253UL * (mt[mti-1] ^ (mt[mti-1] >> 30)) + mti); 
        /* See Knuth TAOCP Vol2. 3rd Ed. P.106 for multiplier. */
        /* In the previous versions, MSBs of the seed affect   */
        /* only MSBs of the array mt[].                        */
        /* 2002/01/09 modified by Makoto Matsumoto             */
        mt[mti] &= 0xffffffffUL;
        /* for >32 bit machines */
    }
}

/* initialize by an array with array-length */
/* init_key is the array for initializing keys */
/* key_length is its length *//* slight change for C++, 2004/2/26 */
void init_by_array(unsigned long init_key[], int key_length)
{
    int i, j, k;
    init_genrand(19650218UL);
    i=1; j=0;
    k = (N>key_length ? N : key_length);
    for (; k; k--) {
        mt[i] = (mt[i] ^ ((mt[i-1] ^ (mt[i-1] >> 30)) * 1664525UL))
          + init_key[j] + j; /* non linear */
        mt[i] &= 0xffffffffUL; /* for WORDSIZE > 32 machines */
        i++; j++;
        if (i>=N) { mt[0] = mt[N-1]; i=1; }
        if (j>=key_length) j=0;
    }
    for (k=N-1; k; k--) {
        mt[i] = (mt[i] ^ ((mt[i-1] ^ (mt[i-1] >> 30)) * 1566083941UL))
          - i; /* non linear */
        mt[i] &= 0xffffffffUL; /* for WORDSIZE > 32 machines */
        i++;
        if (i>=N) { mt[0] = mt[N-1]; i=1; }
    }

    mt[0] = 0x80000000UL; /* MSB is 1; assuring non-zero initial array */ 
}

/* generates a random number on [0,0xffffffff]-interval */
unsigned long genrand_int32(void)
{
    unsigned long y;
    static unsigned long mag01[2]={0x0UL, MATRIX_A};
    /* mag01[x] = x * MATRIX_A  for x=0,1 */

    if (mti >= N) { /* generate N words at one time */
        int kk;

        if (mti == N+1)   /* if init_genrand() has not been called, */
            init_genrand(5489UL); /* a default initial seed is used */

        for (kk=0;kk<N-M;kk++) {
            y = (mt[kk]&UPPER_MASK)|(mt[kk+1]&LOWER_MASK);
            mt[kk] = mt[kk+M] ^ (y >> 1) ^ mag01[y & 0x1UL];
        }
        for (;kk<N-1;kk++) {
            y = (mt[kk]&UPPER_MASK)|(mt[kk+1]&LOWER_MASK);
            mt[kk] = mt[kk+(M-N)] ^ (y >> 1) ^ mag01[y & 0x1UL];
        }
        y = (mt[N-1]&UPPER_MASK)|(mt[0]&LOWER_MASK);
        mt[N-1] = mt[M-1] ^ (y >> 1) ^ mag01[y & 0x1UL];

        mti = 0;
    }
  
    y = mt[mti++];

    /* Tempering */
    
    y ^= (y >> 11);
    y ^= (y << 7) & 0x9d2c5680UL;
    y ^= (y << 15) & 0xefc60000UL;
    y ^= (y >> 18);

    return y;
}

//#define DEBUG 1 //20140922
//#define ALPMODE 1 //0:AI-RTPG 1:Basic ALP-RTPG 2:Combination of AI&ALP
#define TAP_NUM 5
unsigned int TapGen_state[TAP_NUM];
void InternalLfsrGen(int *Xor);
void phase_shifter(int LFSR[], int nBit);

void phase_shifter(int LFSR[], int nBit)
{
  int ia;
  for (ia = 1; ia < nBit; ia++)
    LFSR[ia] ^= LFSR[ia - 1];
}

void InternalLfsrGen(int *Xor)
{
  int i, count = 0, tmp[LFSR_BIT + 1];
  unsigned int feedbackbit = 0, mask = 0, temple = 0;
  while (count < TAP_NUM / 2 + 1)
  {
    feedbackbit = TapGen_state[TAP_NUM / 2 + count] & 1;
    temple = (feedbackbit << (LFSR_BIT - 1)) | (TapGen_state[TAP_NUM / 2 + count] >> 1);
    mask = 0;
    for (i = 2; i <= Xor[0]; i++)
      mask |= (feedbackbit << (LFSR_BIT - Xor[i] - 1));
    count++;
    TapGen_state[TAP_NUM / 2 + count] = temple ^ mask;
#if PHASE
/*

    for (i = 0; i < LFSR_BIT; i++)
      tmp[i] = TapGen_state[TAP_NUM / 2 + count] >> (LFSR_BIT - i - 1);
    phase_shifter(tmp, LFSR_BIT);
*/
#endif
  }
}

void reseeding(lfsr_state, cnt) int lfsr_state[];
int cnt;
{
  int ia;
  unsigned int seed = 0;
  FILE *fin;

#if RANDOMSEED
  seed = (unsigned int)(rand() % 65535);
  printf("seedcount=%d seed=%d--", cnt, seed);
  for (ia = 16 - 1; ia >= 0; ia--)
    printf("%d", (seed >> ia) & 0x0001);
  printf("\n");
#elif SEEDMEMORY
  if ((fin = fopen("seed.dat", "r")) == NULL)
  {
    printf("error: 'seed.dat' is not found\n");
    exit(1);
  }
  for (ia = 0; ia < cnt; ia++)
    fscanf(fin, "%d", &seed);
  fclose(fin);
#endif
  TapGen_state[TAP_NUM / 2] = seed;
  printf("TapGen_state=%d\n", TapGen_state[TAP_NUM / 2]);
}

void initial_lfsr(EX_OR, lfsr_state, ff_state) int EX_OR[];
int lfsr_state[];
int ff_state[];
{
  int ia, max;
  char buf[20];
  unsigned long init[4]={0x123, 0x234, 0x345, 0x456}, length=4;
  init_by_array(init, length);

  FILE *fin;
  srand((unsigned)time(NULL));
  for (ia = 0; ia <= LFSR_BIT; ia++)
  { //initialize SI's LFSR
    if (ia % 4)
      lfsr_state[ia] = 0;
    else
      lfsr_state[ia] = 1;
  }

  for (ia = 0; ia <= ffnum; ia++) //initialize FF state
    ff_state[ia] = 0;
  CHAINLENGTH = 100;

  if (ffnum > LFSR_BIT * CHAINLENGTH)
  {
    printf("warning:ffnum is must smaller than LFSR_BIT*CHAINLENGTH\n");
    CHAINLENGTH = (1 + ffnum / (LFSR_BIT * CHAINLENGTH)) * 100;
  }
  chainnum = (ffnum - 1) / CHAINLENGTH + 1;

  for (ia = 0; ia < chainnum; ia++)
    ShiftPeak[ia] = 0.0;

  schain = (SCAN_CHAIN *)calloc(chainnum, sizeof(SCAN_CHAIN));
  if (schain == NULL)
    printf("memory error @initial_lfsr\n"), exit(1);

  fin = fopen("lfsr.dat", "r");
  if (fin == NULL)
  {
    printf("error: 'lfsr.dat' is not found\n");
    exit(1);
  }
  fscanf(fin, "%d", &max);

  if ((LFSR_BIT > max) || (chainnum > max))
  {
    printf("error: too many LFSR bits!\n");
    fclose(fin);
    exit(1);
  }

  for (ia = 0; ia < LFSR_BIT; ia++)
    if (fgets(buf, 20, fin) == NULL)
    {
      printf("error: too many fins!\n");
      fclose(fin);
      exit(1);
    }
  fscanf(fin, "%d", &EX_OR[0]);
  for (ia = 1; ia <= EX_OR[0]; ia++)
    fscanf(fin, "%d", &EX_OR[ia]);
  fclose(fin);

  for (ia = 0; ia < chainnum; ia++)
  {
    if (ia == 0)
      schain[ia].top = 0;
    else
      schain[ia].top = schain[ia - 1].top + schain[ia - 1].length;

    schain[ia].length = (ffnum - schain[ia].top) / (chainnum - ia);
    //schain[ia].lastval = ff_state[schain[ia].top + schain[ia].length - 1];
    schain[ia].lastval = ff_state[schain[ia].top];
  }
}

int PeakTogCount(ff_state, chainum) int ff_state[];
{
  int ia, bitval, togglecount = 0;
  bitval = ff_state[schain[chainum].top];
  //printf("bitval %d \n",bitval);

  for (ia = schain[chainum].top + 1; ia < schain[chainum].top + schain[chainum].length; ia++)
  {
    if (ff_state[ia] != bitval)
    {
      togglecount++;
      bitval = ff_state[ia];
    }
  }
  return togglecount;
  //printf("chain %d \n",togglecount);
}


void lfsr_next(EX_OR, lfsr_state, ff_state, init) int EX_OR[];
int lfsr_state[];
int ff_state[];
unsigned long init[];
{
  int ia, ib, i;

#if PEAK
  int OrigPatTog[chainnum], TogPerClk[chainnum];
  int Tem_state[ffnum];
  for (ia = 0; ia < ffnum; ia++)
    Tem_state[ia] = ff_state[ia];
  for (ia = 0; ia < chainnum; ia++)
  {
    OrigPatTog[ia] = 0;
    TogPerClk[ia] = 0;
    OrigPatTog[ia] = PeakTogCount(ff_state, ia);
    //printf("chain %d = %d \n",ia,OrigPatTog[ia]);
  }
#endif
  unsigned int rand, tmp; 

  for (ia = 0; ia <= (ffnum - 1) / chainnum; ia++)
  {
    if(GENERATOR == 0)
    {
      //kanzaki
      rand =  genrand_int32();
      //printf("%lx \n", rand);
      for (ib = 0; ib < LFSR_BIT; ib++)
      {
	tmp = rand;
	rand = tmp << 1;
	if(tmp > rand)
	  lfsr_state[ib] = 1;
	else
	  lfsr_state[ib] = 0;
      }
    }
    
    /*
    for (ib = 0; ib < LFSR_BIT; ib++)
      printf("%d ", lfsr_state[ib]);
    printf("%lx\n", rand);
    */

    for (ib = 0; ib < chainnum; ib++)
      {
	
      if (schain[ib].length == (ffnum - 1) / chainnum)
      {
        if (ia != 0)
          ff_state[schain[ib].top + schain[ib].length - ia] = lfsr_state[ib];
      }
      else
        ff_state[schain[ib].top + schain[ib].length - ia - 1] = lfsr_state[ib];
#if POWEREVA
      if (ff_state[schain[ib].top + schain[ib].length - ia - 1] != schain[ib].lastval)
      {
        toggle_scn += schain[ib].length - ia;
        toggle_scn_in += schain[ib].length - ia;
        toggle_shift_perpat += schain[ib].length - ia;
#if PEAK
        TogPerClk[ib] = OrigPatTog[ib] + 1;
        if (ia <= schain[ib].length - 2 && Tem_state[schain[ib].top + schain[ib].length - ia - 1] == Tem_state[schain[ib].top + schain[ib].length - ia - 2])
          OrigPatTog[ib]++;

#endif
      }
      else
      {
#if PEAK
        TogPerClk[ib] = OrigPatTog[ib];
        if (ia <= schain[ib].length - 2 && Tem_state[schain[ib].top + schain[ib].length - ia - 1] != Tem_state[schain[ib].top + schain[ib].length - ia - 2])
          OrigPatTog[ib]--;
#endif
      }
#endif

#if PEAK
      if (TogPerClk[ib] > ShiftPeak[ib])
        ShiftPeak[ib] = TogPerClk[ib];
#endif
      schain[ib].lastval = ff_state[schain[ib].top + schain[ib].length - ia - 1];
    }

#if DEBUG
    if (ia == 0)
    {
      printf("\nLFSR state:\n");

      printf("     ");
      for (ib = 0; ib <= LFSR_BIT; ib++)
        printf("%2d ", ib);
      printf("\n");

      printf("     ");
      for (ib = 0; ib <= LFSR_BIT; ib++)
        printf("===", ib);
      printf("\n");
    }

    printf("%3d |", ia);

    for (ib = 0; ib <= LFSR_BIT; ib++)
      printf(" %d ", lfsr_state[ib]);
    printf("\n");
#endif //LFSR pattern input
    
    if (clocktime == 1 && ia == 0)
    {

      for (i = 0; i < TAP_NUM; i++)
        TapGen_state[i] = 0;
       for (i = 0; i < LFSR_BIT; i++)
        TapGen_state[TAP_NUM / 2] |= (lfsr_state[i] << (LFSR_BIT - i - 1));
    }

    InternalLfsrGen(EX_OR);
#if GENERATOR
    for (ib = 0; ib < LFSR_BIT; ib++)
       lfsr_state[ib] = (TapGen_state[TAP_NUM / 2 + 1] >> (LFSR_BIT - ib - 1)) & 1;

#if PHASE
      phase_shifter(lfsr_state, LFSR_BIT);
#endif

#endif 
    TapGen_state[TAP_NUM / 2] = TapGen_state[TAP_NUM / 2 + 1];
    TapGen_state[TAP_NUM / 2 + 1] = 0;
  }

#if DEBUG
  printf("\n");

  for (ia = 0; ia < chainnum; ia++)
  {
    printf("sc%-2d:", ia);

    for (ib = 0; ib < schain[ia].length; ib++)
      printf("%d", ff_state[schain[ia].top + ib]);
    printf("\n");
  }

#endif
}
