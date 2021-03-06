#include "declare.h"
#include "def_gtype.h"
#include "def_flt.h"

#include <math.h>
int flt_det_cap[MAXCAP];
faultsim(argv) char *argv[13];
{
  int time, num_injgate, Tmp_num_injgate, tmp_time, remain_flt;
  FLT_NODE *injarray[32];
  FLT_NODE inj_flst, tmppointer;
  FIN_NODE *finnode;
  L_NODE *fnode;
  FILE *test_pat;
  FILE *tpi_pat;
  int i, ia, ib, ic, it;
  int n_inp, num_observe;
  int EX_OR[10], lfsr_state[LFSR_BIT + 1], ff_state[ffnum + 1];
  float TmpShiftPeek = 0.0, TmpSIPeek = 0.0, TmpSOPeek = 0.0, TmpCapPeek[MAXCAP];
  int sum;

  FILE *fout_flt_pat;

  char outpath[200];
  //char outpath_in[200];
  //char outpath_flt_in[200];
  char pt_file1[100];
  char pt_file2[100];

  sprintf(pt_file1, "%s_lfsr_pi.dat", argv[1]);
  sprintf(pt_file2, "%s_tgl_ff_tpi.dat", argv[1]);

  for (ia = 0; ia < MAXCAP; ia++)
    flt_cap[ia] = 0;
  printf("\n==Simulation Parameter Setting for==\n");
  switch (MODE_TOOL)
    {
    case 1:
      printf("\nNormal BIST Testing Mode\n");
      sprintf(outpath, "./OUTPUTS/BIST/%s.csv", argv[1]);
      fout_flt_pat = fopen(outpath, "w+");

      break;
    case 2:
      printf("\nBIST Multi-Cycle Testing Mode\n");
      if (SKIP_CAPTURE)
	printf("skip first %d capture\n", SKIP_CAPTURE);
      sprintf(outpath, "./OUTPUTS/MULTI_BIST/%s_%d.csv", argv[1], cap_freq);
      fout_flt_pat = fopen(outpath, "w+");
      //	printf("FCOV_PAT output file is not exist!\n"), exit(1);

      break;

    case 3:
      printf("\nBIST Multi-Cycle Testing With Sequential Observation Mode\n");
      sprintf(outpath, "./OUTPUTS/MULTI_BIST_OB/%s_%d.csv", argv[1], cap_freq);
      fout_flt_pat = fopen(outpath, "w+");
      //			printf("FCOV_PAT output file is not exist!\n"), exit(1);

#if PO_OBSERVE
      printf("PO observe mode\n");
      po_observe = (T_NODE **)calloc(numout, sizeof(T_NODE *));
      if (po_observe == NULL)
	printf("memory error @faultsim\n"), exit(1);

      for (ia = 0; ia < numout; ia++)
	{
	  po_observe[ia] = (T_NODE *)calloc(cap_freq, sizeof(T_NODE));
	  if (po_observe[ia] == NULL)
	    printf("memory error @faultsim\n"), exit(1);
	}
#endif

      ff_observe = (T_NODE **)calloc(ffnum, sizeof(T_NODE *));
      if (ff_observe == NULL)
	printf("memory error @faultsim \n"), exit(1);
      for (ia = 0; ia < ffnum; ia++)
	{
	  ff_observe[ia] = (T_NODE *)calloc(cap_freq, sizeof(T_NODE));
	  if (ff_observe[ia] == NULL)
	    printf("memory error @faultsim \n"), exit(1);
	}

#if SELECT_STATION
      num_observe = ffnum * OBSERVE_RATE;
      printf("Partial FF observation mode: %f \n", OBSERVE_RATE);
      printf("Station rate = %.0f[%]\n", (float)(OBSERVE_RATE * 100));
      ff_sta_src_read(FF_FILE, num_observe, argv);
#else
      ff_select = (char *)calloc(ffnum, sizeof(char));
      printf("Fully Observation Mode \n");
      printf("Station rate = 100[%]\n");
      for (ia = 0; ia < ffnum; ia++)
	{
	  ff_select[ia] = '1';
	}
#endif

      break;
    case 4:
      printf("\nBIST Multi-Cycle Testing under CPI Mode\n");
      /*=======Memorry Assign for Sequential Observation*/

#if PO_OBSERVE
      printf("PO observe mode\n");
      po_observe = (T_NODE **)calloc(numout, sizeof(T_NODE *));
      if (po_observe == NULL)
	printf("memory error @faultsim\n"), exit(1);

      for (ia = 0; ia < numout; ia++)
	{
	  po_observe[ia] = (T_NODE *)calloc(cap_freq, sizeof(T_NODE));
	  if (po_observe[ia] == NULL)
	    printf("memory error @faultsim\n"), exit(1);
	}
#endif

      ff_observe = (T_NODE **)calloc(ffnum, sizeof(T_NODE *));
      if (ff_observe == NULL)
	printf("memory error @faultsim \n"), exit(1);
      for (ia = 0; ia < ffnum; ia++)
	{
	  ff_observe[ia] = (T_NODE *)calloc(cap_freq, sizeof(T_NODE));
	  if (ff_observe[ia] == NULL)
	    printf("memory error @faultsim \n"), exit(1);
	}

#if SELECT_STATION
      num_observe = ffnum * OBSERVE_RATE;
      printf("Partial FF observation mode: %f \n", OBSERVE_RATE);
      printf("Station rate = %.0f[%]\n", (float)(OBSERVE_RATE * 100));
      ff_sta_src_read(FF_FILE, num_observe, argv);

#else
      ff_select = (char *)calloc(ffnum, sizeof(char));
      printf("Fully Observation Mode \n");
      printf("Station rate = 100[%]\n");
      for (ia = 0; ia < ffnum; ia++)
	{
	  ff_select[ia] = '1';
	}
#endif

      initial_node(argv);

#if FCOVPERPAT
      //if(MODE_TOOL==4){
      char cpi_sim_outpath[200];
      switch (TGL_GATE_MODE)		{
      case 0: //Non Toggle gate insertion
	sprintf(cpi_sim_outpath, "./OUTPUTS/CPI/%d_cycles/Non_%s.csv", cap_freq, argv[1]);
	//			sprintf(outpath_in, "./OUTPUTS/CPI/%dcycles/%dSKIP/input_pat/%s_NONTG_FF_TPI_INP_%.2f_%d_%.2f.csv", cap_freq, SKIP_CYCLE, argv[1], OBSERVE_RATE, FF_SEL_METHOD, ff_rate);
	break;
      case 1: //toggle gate insert by toggling

	sprintf(cpi_sim_outpath, "./OUTPUTS/CPI/%dcycles/%dSKIP/%s_LCPI_TGL_%d_%.2f.csv", cap_freq, SKIP_CYCLE, argv[1], INTERVAL_CYCLE, Tgl_rate);
	sprintf(cpi_sim_outpath, "./OUTPUTS/CPI/%d_cycles/%s_LCPI_TGL_%d_%d_%.2f.csv", cap_freq, argv[1], INTERVAL_CYCLE, SKIP_CYCLE,Tgl_rate);
	break;
      case 4:
	sprintf(cpi_sim_outpath, "./OUTPUTS/CPI/%d_cycles/%s_LCPI_RAN_%d_%d_%.2f.csv", cap_freq, argv[1], INTERVAL_CYCLE, SKIP_CYCLE, Tgl_rate);
	break;
      case 2: //toggle FF  insert by toggling
	sprintf(cpi_sim_outpath, "./OUTPUTS/CPI/%d_cycles/%s_FFCPI_TGL_%d_%d_%.2f.csv", cap_freq, argv[1],INTERVAL_CYCLE, SKIP_CYCLE,  ff_rate);
	break;
      case 3:
	sprintf(cpi_sim_outpath, "./OUTPUTS/CPI/%d_cycles/%s_FFCPI_RAN_%d_%d_%.2f.csv", cap_freq, argv[1], INTERVAL_CYCLE, SKIP_CYCLE,ff_rate);
	break;
      default: 		printf("No New Function is Supported by this Program!\n"), exit(1);
	break;
      }

      fout_flt_pat = fopen(cpi_sim_outpath, "w+");
      //		printf("FCOV_PAT output file is not exist!\n"), exit(1);
      //}
#endif
      break;
    }

  /*=======Initialization for Power Evaluation*/
#if POWEREVA
  toggle_scn = 0;
  toggle_scn_in = 0;
  toggle_scn_out = 0;

  for (ia = 0; ia < MAXCAP; ia++)
    {
      toggle_cap[ia] = 0;
      toggle_cap_perpat[ia] = 0;
#if WSA_REC
      WSA[ia] = 0;
      MaxWSA = 0.0;
      if (ia == 0)
	initial_node(argv);
#endif
    }
#endif


#if OUTPUT_FLIST
  flist_out = fopen("fault_list.dat", "w+");
  fprintf(flist_out, "%d %d\n", sum_flt, length);
#endif

  if (TPG_MODE == 1)
    { //=1:ATPG, =0:LFSR
      test_pat = fopen("ATPG.dat", "r");
      if (test_pat == NULL)
	printf("ATPG.dat is not found!\n"), exit(1);
      fscanf(test_pat, "%d", &length);
      printf("#of Vector loaded=%d \n", length);
      n_inp = inpnum;
    }
  else
    {
      test_pat = fopen(pt_file1, "r");
      if (test_pat == NULL)
	printf("%s is not found!\n", argv[2]), exit(1);
      fscanf(test_pat, "%d %d", &length, &n_inp);

      if (n_inp != inpnum)
	printf("test pattern format1 error!\n"), exit(1);
    }
  //printf("herer?\n");
  if (MODE_TOOL == 4)
    {
      if (TGL_GATE_MODE == 3)
	{ //FF_TPI by LFSR
	  tpi_pat = fopen(pt_file2, "r");
	  if (tpi_pat == NULL)
	    printf("tgl_ff_tpi.dat is not found!\n"), exit(1);
	  fscanf(tpi_pat, "%d %d", &n_tpi, &n_tpi);
	  printf("%d %d %f %d\n", n_tpi, ffnum, ff_rate, (int)(ffnum * ff_rate));
	  if (n_tpi != (int)(ffnum * ff_rate))
	    printf("test pattern format2 error!\n"), exit(1);
	  //tgl_tpi = (int **) malloc((n_tpi+1)*sizeof(int *));
	  tgl_tpi = (int **)calloc((n_tpi + 1), sizeof(int *));
	  for (ia = 0; ia <= n_tpi; ia++)
	    tgl_tpi[ia] = (int *)calloc((cap_freq + 1), sizeof(int));
	  //tgl_tpi[ia] = (int *)malloc((cap_freq+1)*sizeof(int));
	}
      else if (TGL_GATE_MODE == 4)
	{ //Toggle Gate TPI by LFSR
	  tpi_pat = fopen("tgl_gt_tpi.dat", "r");
	  if (tpi_pat == NULL)
	    printf("tgl_gt_tpi.dat is not found!\n"), exit(1);
	  fscanf(tpi_pat, "%d %d", &n_tpi, &n_tpi);
	  printf("%d %d %f %d\n", n_tpi, numgate, Tgl_rate, (int)(numgate * Tgl_rate));
	  if (n_tpi != (int)(numgate * Tgl_rate))
	    printf("test pattern format3 error?!\n"), exit(1);
	  tgl_tpi = (int **)malloc((n_tpi + 1) * sizeof(int *));
	  for (ia = 0; ia <= n_tpi; ia++)
	    tgl_tpi[ia] = (int *)malloc((cap_freq + 1) * sizeof(int));
	}
    }

  initial_lfsr(EX_OR, lfsr_state, ff_state);
  int pivalset[n_inp];
  int tmp_tpi[n_tpi];
  initialize_detect(fltlst.next, length);

  if (MODE_TOOL == 3 || MODE_TOOL == 4)
    flt_info(fltlst.next);

  printf("fault List initialization over \n");

  remain_flt = count_flt(fltlst.next);

  //2019/12
  // FILE *fp;
  // if ((fp = fopen("test_pat_s27.csv", "w")) == NULL)
  // {
  // 	printf("file open error!!\n");
  // 	exit(EXIT_FAILURE); /* (3)エラーの場合は通常、異常終了する */
  // }
  // prn_state_ao2(fp);
  /*Simulation Start*/
  //for (time = 1; time <= length && fltlst.next != NULL && ((float)flt_det_num[0] / (float)sum_flt) <= 0.90001; time++)
  for (time = 1; time <= length && fltlst.next != NULL; time++)
    //	for (time = 1; time <= length && fltlst.next != NULL && (1 - (float)remain_flt / (float)sum_flt) <= 0.90001; time++)
    {
      clocktime = time;
      //fprintf(fp, "-test pat %d-,", time); //2019/12

#if POWEREVA
      toggle_scn_max = (long double)ffnum * schain[chainnum - 1].length * time;
      toggle_cap_max = (long double)ffnum * time;
      toggle_shift_perpat = 0;
      for (ia = 0; ia <= cap_freq; ia++)
	toggle_cap_perpat[ia] = 0;
#endif


#if DEBUG1 || PRNT_PIVAL || PRNT_DET_FLT || PRNT_FF
      printf("\n ----- TIME %d -----\n", time);
#endif
      ///////PI Pattern Set////////////////
      if (TPG_MODE == 1)
	{ //=1:ATPG, =0:LFSR
	  for (ia = 1; ia <= n_inp; ia++)
	    fscanf(test_pat, "%d", &pivalset[ia]);
	  for (ia = 0; ia < ffnum; ia++)
	    fscanf(test_pat, "%d", &ff_state[ia]);
	}
      else
	{
	  for (ia = 1; ia <= n_inp; ia++)
	    {
	      fscanf(test_pat, "%d", &pivalset[ia]);
	      //printf("%d", pivalset[ia]);
	    }
	  //printf("\n");
	  lfsr_next(EX_OR, lfsr_state, ff_state);
	}
      pi_valset(pivalset);

      ///////TPI Pattern Set////////////////
      if (MODE_TOOL == 4)
	{
	  if (TGL_GATE_MODE == 3 || TGL_GATE_MODE == 4)
	    { //TPI by LFSR
	      //for (ib = 1; ib <= cap_freq - SKIP_CYCLE; ib++)
	      for (ib = SKIP_CYCLE - 1; ib <= cap_freq; ib++)
		{
		  for (ia = 0; ia < n_tpi; ia++)
		    {
		      fscanf(tpi_pat, " %d", &tmp_tpi[ia]);
		      //printf(" %d",tmp_tpi[ia]);
		    }
		  //printf("\n");
		  for (ia = 0; ia < n_tpi; ia++)
		    tgl_tpi[ia][ib] = tmp_tpi[ia];
		}

	      /* 	for (ib = 1; ib <= cap_freq; ib++)
			printf("%d ", tgl_tpi[1][ib]);
			printf("\n");
	      */
	    }
	}

      //////////////////////////////////////////////////
      initial_state(ff_state);

#if PRNT_FF
      prn_state(ffnode.next);
#endif

      /*=== multi-cycle logic simulation start ===*/

      //prn_state_ao3(fp, time); //2019/12

      for (ia = 1; ia <= cap_freq; ia++)
	{

	  //prn_state_ao(ffnode.next);

	  onetimesim(ia);

	  // if (cap_freq == ia)
	  // 	fprintf(fout_in, "\n");
	  // if (ia == 5)
	  // {
	  // 	exit(1);
	  // }
	  switch (MODE_TOOL)
	    {
	    case 3:
	    case 4:
#if SELECT_STATION
	      // ic=0;

	      finnode = ffnode.next;
	      for (ib = 0; finnode != NULL; finnode = finnode->next, ib++)
		{
		  //if(ff_select[ib] == '1'){
		  fnode = finnode->node;

		  // ff_observe[ic][ia-1].gdval1 = fnode->gdval1;
		  ff_observe[ib][ia - 1].gdval1 = fnode->finlst->node->gdval1;

		  //ff_observe[ic][ia-1].gdval1 = fnode->finlst->node->gdval1;
		  //ic++;
		  // }
		}

#else
	      finnode = ffnode.next;
	      for (ib = 0; ib < ffnum; finnode = finnode->next, ib++)
		{
		  fnode = finnode->node;
		  //ff_observe[ib][ia-1].gdval1 = fnode->gdval1;
		  ff_observe[ib][ia - 1].gdval1 = fnode->finlst->node->gdval1;
		}
#endif

#if PO_OBSERVE
	      finnode = ponode.next;
	      for (ib = 0; finnode != NULL; finnode = finnode->next, ib++)
		{
		  fnode = finnode->node;
		  po_observe[ib][ia - 1].gdval1 = fnode->gdval1;
		  //po_observe[ib][ia-1].gdval1 =  fnode->finlst->node->gdval1;
		}
#endif
	    }

	  update_nextstate(ia);

	  if (MODE_TOOL == 4 && TGL_GATE_MODE == 3 && ia >= SKIP_CYCLE - 1 && ia < cap_freq)
	    {
	      //printf("%d,%d,%d\n", MODE_TOOL, TGL_GATE_MODE, ia);
	      tpi_ff_state_load(ia);
	      //printf("+++++++++++++%d\n", fnode->line);
	    }

	  //	prn_state_ao(fp, ia); //2019/12

#if DEBUG1 || PRNT_FF
	  // prn_state(ffnode.next);
	  // prn_out_value(ponode.next, ia);
#endif
	  //printf(" %d: %d\n", fnode->line, ia);
	}
      scan_out(ffnode.next);

      /*=== multi-cycle logic simulation end ===*/
#if FAULTSIMULATION
      /*=== multi-cycle Original fault simulation start ===*/
#if TRANSITIONFAULT
      /**Transition delay fault Simulation**/
      //printf("###Transition delay fault Simulation Begin###\n");

      prn_detect(fltlst.next, length); /* print undetected fault list */

      inj_flst.next = fltlst.next;
      while (inj_flst.next != NULL)
	{
	  initial_state_ft(ff_state);
	  num_injgate = fault_inject(&inj_flst, injarray);
	  cop + typey_state(); //last slow capture value loading
	  for (ia = SLOW_CK + 1; ia <= cap_freq; ia++)
	    {
	      ftvalsim(ia);
	      if (MODE_TOOL == 3 || MODE_TOOL == 4)
		{
#if SELECT_STATION
		  //ic=0;
		  finnode = ffnode.next;
		  for (ib = 0; finnode != NULL; finnode = finnode->next, ib++)
		    {
		      //if(ff_select[ib] == '1'){
		      fnode = finnode->node;
		      //ff_observe[ic][ia-1].ftval1 = fnode->ftval1;
		      ff_observe[ib][ia - 1].ftval1 = fnode->finlst->node->ftval1;
		      //ff_observe[ic][ia-1].ftval1 =fnode->finlst->node->ftval1;
		      //ic++;
		      // }
		    }
#else
		  finnode = ffnode.next;
		  for (ib = 0; ib < ffnum; finnode = finnode->next, ib++)
		    {
		      fnode = finnode->node;
		      // ff_observe[ib][ia-1].ftval1 = fnode->ftval1;
		      ff_observe[ib][ia - 1].ftval1 = fnode->finlst->node->ftval1;
		    }
#endif

#if PO_OBSERVE
		  finnode = ponode.next;
		  for (ib = 0; finnode != NULL; finnode = finnode->next, ib++)
		    {
		      fnode = finnode->node;
		      po_observe[ib][ia - 1].ftval1 = fnode->ftval1;
		    }
#endif
		}
	      update_nextstate_ft(ia);
	    }

	  drop_flt(num_injgate, injarray, time);
	  clear_addgate(num_injgate, add_gate, add_pi);
		}

      prn_detect(fltlst.next, length); /* print undetected fault list */
#else
      /**Stuck at fault Simulation**/
      // printf("###Stuck at Fault Simulation Begin###\n");

      inj_flst.next = fltlst.next;
      while (inj_flst.next != NULL)
	{
	  //	prn_state_ao2(fp); //2019/12
	  initial_state_ft(ff_state);
	  num_injgate = fault_inject(&inj_flst, injarray);

	  //	fprintf(fp, "fault value -%d %d-\n", injarray[0]->line, injarray[0]->saval); //2019/12

	  for (ia = 1; ia <= cap_freq; ia++)
	    {

	      //	fprintf(fp, "cycle %d,", ia); //2019/12

	      ftvalsim(ia);
	      if (MODE_TOOL == 3 || MODE_TOOL == 4)
		{

#if SELECT_STATION
		  //ic=0;
		  finnode = ffnode.next;
		  for (ib = 0; finnode != NULL; finnode = finnode->next, ib++)
		    {
		      //if(ff_select[ib] == '1'){
		      fnode = finnode->node;
		      //ff_observe[ic][ia-1].ftval1 = fnode->ftval1;
		      ff_observe[ib][ia - 1].ftval1 = fnode->finlst->node->ftval1;
		      //ff_observe[ic][ia-1].ftval1 =fnode->finlst->node->ftval1;
		      //ic++;
		      // }
		    }
#else
		  finnode = ffnode.next;
		  for (ib = 0; ib < ffnum; finnode = finnode->next, ib++)
		    {
		      fnode = finnode->node;
		      // ff_observe[ib][ia-1].ftval1 = fnode->ftval1;
		      ff_observe[ib][ia - 1].ftval1 = fnode->finlst->node->ftval1;
		    }
#endif

#if PO_OBSERVE
		  finnode = ponode.next;
		  for (ib = 0; finnode != NULL; finnode = finnode->next, ib++)
		    {
		      fnode = finnode->node;
		      po_observe[ib][ia - 1].ftval1 = fnode->ftval1;
		    }
#endif
		}
	      update_nextstate_ft(ia);

	      //if (MODE_TOOL == 4 && TGL_GATE_MODE == 3 && ia >= SKIP_CYCLE - 1)
	      if (MODE_TOOL == 4 && TGL_GATE_MODE == 3 && ia >= SKIP_CYCLE - 1 && ia < cap_freq)
		{
		  tpi_ff_state_load_ft(ia);
		}
	      //prn_state_ao_ft(fp, ia); //2019/12
	    }

#if FAULTDROP

	  drop_flt(num_injgate, injarray, time);

#else
	  Full_flt_sim(num_injgate, injarray, time); //2014_10_21ここ//修正あり、2015710_王

#endif
	  clear_addgate(num_injgate, add_gate, add_pi);
	}
#endif
#endif

      /*=== multi-cycle fault simulation end ===*/

      if (time % PRN_FLT_INT == 0)
	{
	  //printf("is here?\n");
	  printf("CUT= %s,#pattern: %d -> \n", argv[1], time);

	  remain_flt = count_flt(fltlst.next);
#if TRANSITIONFAULT
	  printf("Fault Coverage: %4.6f \n", (1 - (float)remain_flt / (float)sum_Tran_flt) * 100.0);
#else

	  //printf("Fault Coverage: %6.4f \n", (float)flt_det_num[10] / (float)sum_flt * 100.0);
	  printf("#Ori FCov: %4.6f \n",(1 - (float)remain_flt/ (float)sum_flt) * 100.0);
#if FCOVPERPAT
	  //koko!!!fprintf(fout_flt_pat, "%d,", time);
#endif
#endif

#if POWEREVA
	  //printf("Test Power Evaluation \n#SHIFT_IN,  SHIFT_OUT, SHIFT");
#if PEAK
	  printf("SHIFT_TGL_RATE, PEAK TGL");
#endif
	  printf("\n");
	  printf("%.0lf,%.0lf,%.0lf,%.0lf", (float)toggle_scn_in, (float)toggle_scn_out, (float)toggle_scn, (float)toggle_scn_max);
#if PEAK
	  float Max = 0;
	  for (ia = 0; ia < chainnum; ia++)
	    {
	      if (ShiftPeak[ia] / (schain[ia].length - 1) > Max)
		Max = ShiftPeak[ia] / (schain[ia].length - 1);
	    }
	  printf("%4.3f, %4.3f", (float)toggle_scn / (float)toggle_scn_max * 100.0, Max * 100);
#endif
	  printf("\n");
#if WSA_REC
	  printf("Capture Power \n#Cap=%d\n", cap_freq);
	  for (ia = 1; ia <= cap_freq; ia++)
	    {
	      printf(",%.0f", WSA[ia] / clocktime); // exit(1);
#if FCOVPERPAT
	      fprintf(fout_flt_pat, ",%.0f", WSA[ia] / clocktime);
#endif
	    }
	  printf(",%.0f", MaxWSA);
#endif
#endif

//koko!!,%4.6f
	  fprintf(fout_flt_pat, "%4.6f", (1 - (float)remain_flt / (float)sum_flt) * 100.0);
	  if (MODE_TOOL == 4||MODE_TOOL == 3)
	    {
#if SELECT_STATION

	      for (ia = 0; ia < FF_FILE; ia++)
		{
		  printf("#DFT FCov: %4.6f\n", (float)flt_det_num[ia] / (float)sum_flt * 100.0);
		  fprintf(fout_flt_pat, ",%4.6f", (float)flt_det_num[ia] / (float)sum_flt * 100.0);
		}
	      fprintf(fout_flt_pat, ",%4.6f", (float)flt_det_num[10] / (float)sum_flt * 100.0);
	      printf("#Max FCov: %4.6f\n", (float)flt_det_num[10] / (float)sum_flt * 100.0);
#endif
	    }
	  fprintf(fout_flt_pat, "\n");
	}

#if MAKE_REC
      remain_flt = count_flt(fltlst.next);
      fout = fopen("log.dat", "a");
      fprintf(fout, "%6.3f\t%6.3f", 100.0 - (float)remain_flt / sum_flt * 100.0, toggle_scn / toggle_scn_max * 100.0);
#if TRANSITIONFAULT
      for (ia = 1 + SLOW_CK; ia <= cap_freq; ia++)
	fprintf(fout, "\t%6.3f", toggle_cap[ia] / toggle_cap_max * 100.0);
#else
      for (ia = 1; ia <= cap_freq; ia++)
	fprintf(fout, "\t%6.3f", toggle_cap[ia] / toggle_cap_max * 100.0);
#endif
      fprintf(fout, "\n");
      fclose(fout);
#endif
#if OUTPUT_FLIST
      fprintf(flist_out, "\n");
#endif
    }

#if DEBUG2 || PRNT_FF
  prn_detect(fltlst.next, length); //print undetected fault list
#endif

#if PO_OBSERVE
  for (ia = 0; ia < numout; ia++)
    free(po_observe[ia]);
  free(po_observe);
#endif

#if FCOVPERPAT
  fclose(fout_flt_pat);

  //fclose(fout_flt_in);
#endif

  if (MODE_TOOL == 3 || MODE_TOOL == 4)
    {
#if SELECT_STATION
      for (ia = 0; ia < num_observe; ia++)
	free(ff_observe[ia]);
      free(ff_observe);
#else
      for (ia = 0; ia < ffnum; ia++)
	free(ff_observe[ia]);
      free(ff_observe);
#endif
    }

  fclose(test_pat);
  if (MODE_TOOL == 4 && (TGL_GATE_MODE == 3 || TGL_GATE_MODE == 4))
    {
      fclose(tpi_pat);
      //printf("hehre1?\n");
      for (ia = 0; ia <= n_tpi; ia++)
	free(tgl_tpi[ia]);
      free(tgl_tpi);
      //printf("hehre2?\n");
    }
#if OUTPUT_FLIST
  fclose(flist_out);
#endif

}
