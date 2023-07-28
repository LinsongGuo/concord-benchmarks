#line 270 "../../../../../splash2/codes//null_macros/c.m4.null"

#line 1 "decs.H"
/*************************************************************************/
/*                                                                       */
/*  Copyright (c) 1994 Stanford University                               */
/*                                                                       */
/*  All rights reserved.                                                 */
/*                                                                       */
/*  Permission is given to use, copy, and modify this software for any   */
/*  non-commercial purpose as long as this copyright notice is not       */
/*  removed.  All other uses, including redistribution in whole or in    */
/*  part, are forbidden without prior written permission.                */
/*                                                                       */
/*  This software is provided with absolutely no warranty and no         */
/*  support.                                                             */
/*                                                                       */
/*************************************************************************/

#define MASTER            0
#define RED_ITER          0
#define BLACK_ITER        1
#define UP                0
#define DOWN              1
#define LEFT              2
#define RIGHT             3
#define UPLEFT            4
#define UPRIGHT           5
#define DOWNLEFT          6
#define DOWNRIGHT         7
#define PAGE_SIZE      4096

struct multi_struct {
   double err_multi;
};

extern struct multi_struct *multi;

struct global_struct {
   long id;
   long starttime;
   long trackstart;
   double psiai;
   double psibi;
};

/**************************For Logical Clock******************************/
#ifndef LIBFIBER
struct DLC {
  long clock;
  char padding[128-sizeof(long)];
} __attribute__((aligned(128)));

extern struct DLC masterClocks[64] __attribute__((aligned(128)));
__thread extern long *tlMasterClock;
#else
#ifdef LC
/* required only when both LIBFIBER & LC flags are on */
extern __thread long LocalLC;
#endif
#endif
/*************************************************************************/

extern struct global_struct *global;

extern double eig2;
extern double ysca;
extern long jmm1;
extern double pi;
extern double t0;

extern double ****psi;
extern double ****psim;
extern double ***psium;
extern double ***psilm;
extern double ***psib;
extern double ***ga;
extern double ***gb;
extern double ****work1;
extern double ***work2;
extern double ***work3;
extern double ****work4;
extern double ****work5;
extern double ***work6;
extern double ****work7;
extern double ****temparray;
extern double ***tauz;
extern double ***oldga;
extern double ***oldgb;
extern double *f;
extern double ****q_multi;
extern double ****rhs_multi;

struct locks_struct {
   pthread_mutex_t (idlock);
   pthread_mutex_t (psiailock);
   pthread_mutex_t (psibilock);
   pthread_mutex_t (donelock);
   pthread_mutex_t (error_lock);
   pthread_mutex_t (bar_lock);
};

extern struct locks_struct *locks;

struct bars_struct {
#if defined(MULTIPLE_BARRIERS)
   
#line 104
struct {
#line 104
	pthread_mutex_t	mutex;
#line 104
	pthread_cond_t	cv;
#line 104
	unsigned long	counter;
#line 104
	unsigned long	cycle;
#line 104
} (iteration);
#line 104

   
#line 105
struct {
#line 105
	pthread_mutex_t	mutex;
#line 105
	pthread_cond_t	cv;
#line 105
	unsigned long	counter;
#line 105
	unsigned long	cycle;
#line 105
} (gsudn);
#line 105

   
#line 106
struct {
#line 106
	pthread_mutex_t	mutex;
#line 106
	pthread_cond_t	cv;
#line 106
	unsigned long	counter;
#line 106
	unsigned long	cycle;
#line 106
} (p_setup);
#line 106

   
#line 107
struct {
#line 107
	pthread_mutex_t	mutex;
#line 107
	pthread_cond_t	cv;
#line 107
	unsigned long	counter;
#line 107
	unsigned long	cycle;
#line 107
} (p_redph);
#line 107

   
#line 108
struct {
#line 108
	pthread_mutex_t	mutex;
#line 108
	pthread_cond_t	cv;
#line 108
	unsigned long	counter;
#line 108
	unsigned long	cycle;
#line 108
} (p_soln);
#line 108

   
#line 109
struct {
#line 109
	pthread_mutex_t	mutex;
#line 109
	pthread_cond_t	cv;
#line 109
	unsigned long	counter;
#line 109
	unsigned long	cycle;
#line 109
} (p_subph);
#line 109

   
#line 110
struct {
#line 110
	pthread_mutex_t	mutex;
#line 110
	pthread_cond_t	cv;
#line 110
	unsigned long	counter;
#line 110
	unsigned long	cycle;
#line 110
} (sl_prini);
#line 110

   
#line 111
struct {
#line 111
	pthread_mutex_t	mutex;
#line 111
	pthread_cond_t	cv;
#line 111
	unsigned long	counter;
#line 111
	unsigned long	cycle;
#line 111
} (sl_psini);
#line 111

   
#line 112
struct {
#line 112
	pthread_mutex_t	mutex;
#line 112
	pthread_cond_t	cv;
#line 112
	unsigned long	counter;
#line 112
	unsigned long	cycle;
#line 112
} (sl_onetime);
#line 112

   
#line 113
struct {
#line 113
	pthread_mutex_t	mutex;
#line 113
	pthread_cond_t	cv;
#line 113
	unsigned long	counter;
#line 113
	unsigned long	cycle;
#line 113
} (sl_phase_1);
#line 113

   
#line 114
struct {
#line 114
	pthread_mutex_t	mutex;
#line 114
	pthread_cond_t	cv;
#line 114
	unsigned long	counter;
#line 114
	unsigned long	cycle;
#line 114
} (sl_phase_2);
#line 114

   
#line 115
struct {
#line 115
	pthread_mutex_t	mutex;
#line 115
	pthread_cond_t	cv;
#line 115
	unsigned long	counter;
#line 115
	unsigned long	cycle;
#line 115
} (sl_phase_3);
#line 115

   
#line 116
struct {
#line 116
	pthread_mutex_t	mutex;
#line 116
	pthread_cond_t	cv;
#line 116
	unsigned long	counter;
#line 116
	unsigned long	cycle;
#line 116
} (sl_phase_4);
#line 116

   
#line 117
struct {
#line 117
	pthread_mutex_t	mutex;
#line 117
	pthread_cond_t	cv;
#line 117
	unsigned long	counter;
#line 117
	unsigned long	cycle;
#line 117
} (sl_phase_5);
#line 117

   
#line 118
struct {
#line 118
	pthread_mutex_t	mutex;
#line 118
	pthread_cond_t	cv;
#line 118
	unsigned long	counter;
#line 118
	unsigned long	cycle;
#line 118
} (sl_phase_6);
#line 118

   
#line 119
struct {
#line 119
	pthread_mutex_t	mutex;
#line 119
	pthread_cond_t	cv;
#line 119
	unsigned long	counter;
#line 119
	unsigned long	cycle;
#line 119
} (sl_phase_7);
#line 119

   
#line 120
struct {
#line 120
	pthread_mutex_t	mutex;
#line 120
	pthread_cond_t	cv;
#line 120
	unsigned long	counter;
#line 120
	unsigned long	cycle;
#line 120
} (sl_phase_8);
#line 120

   
#line 121
struct {
#line 121
	pthread_mutex_t	mutex;
#line 121
	pthread_cond_t	cv;
#line 121
	unsigned long	counter;
#line 121
	unsigned long	cycle;
#line 121
} (sl_phase_9);
#line 121

   
#line 122
struct {
#line 122
	pthread_mutex_t	mutex;
#line 122
	pthread_cond_t	cv;
#line 122
	unsigned long	counter;
#line 122
	unsigned long	cycle;
#line 122
} (sl_phase_10);
#line 122

   
#line 123
struct {
#line 123
	pthread_mutex_t	mutex;
#line 123
	pthread_cond_t	cv;
#line 123
	unsigned long	counter;
#line 123
	unsigned long	cycle;
#line 123
} (error_barrier);
#line 123

#else
   
#line 125
struct {
#line 125
	pthread_mutex_t	mutex;
#line 125
	pthread_cond_t	cv;
#line 125
	unsigned long	counter;
#line 125
	unsigned long	cycle;
#line 125
} (barrier);
#line 125

#endif
};

extern struct bars_struct *bars;

extern double factjacob;
extern double factlap;

struct Global_Private {
  char pad[PAGE_SIZE];
  long *rel_num_x;
  long *rel_num_y;
  long *eist;
  long *ejst;
  long *oist;
  long *ojst;
  long *rlist;
  long *rljst;
  long *rlien;
  long *rljen;
  long rownum;
  long colnum;
  long neighbors[8];
  double multi_time;
  double total_time;
};

extern struct Global_Private *gp;

extern double *i_int_coeff;
extern double *j_int_coeff;
extern long xprocs;
extern long yprocs;

extern long numlev;
extern long *imx;
extern long *jmx;
extern double *lev_res;
extern double *lev_tol;
extern double maxwork;
extern long *xpts_per_proc;
extern long *ypts_per_proc;
extern long minlevel;
extern double outday0;
extern double outday1;
extern double outday2;
extern double outday3;

extern long nprocs;
extern double h1;
extern double h3;
extern double h;
extern double lf;
extern double res;
extern double dtau;
extern double f0;
extern double beta;
extern double gpr;
extern long im;
extern long jm;
extern long do_stats;
extern long do_output;
extern long *multi_times;
extern long *total_times;

/*
 * jacobcalc.C
 */
void jacobcalc(double ***x, double ***y, double ***z, long pid, long firstrow, long lastrow, long firstcol, long lastcol);

/*
 * jacobcalc2.C
 */
void jacobcalc2(double ****x, double ****y, double ****z, long psiindex, long pid, long firstrow, long lastrow, long firstcol, long lastcol);

/*
 * laplacalc.C
 */
void laplacalc(long procid, double ****x, double ****z, long psiindex, long firstrow, long lastrow, long firstcol, long lastcol);

/*
 * linkup.C
 */
void link_all(void);
void linkup(double **row_ptr);
void link_multi(void);

/*
 * main.C
 */
long log_2(long number);
void printerr(char *s);

/*
 * multi.C
 */
void multig(long my_id);
void relax(long k, double *err, long color, long my_num);
void rescal(long kf, long my_num);
void intadd(long kc, long my_num);
void putz(long k, long my_num);
void copy_borders(long k, long pid);
void copy_rhs_borders(long k, long procid);
void copy_red(long k, long procid);
void copy_black(long k, long procid);

/*
 * slave1.C
 */
void slave(void *);

/*
 * slave2.C
 */
void slave2(long procid, long firstrow, long lastrow, long numrows, long firstcol, long lastcol, long numcols);

/*
 * subblock.C
 */
void subblock(void);
