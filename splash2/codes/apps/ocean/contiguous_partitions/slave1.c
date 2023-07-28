#line 270 "../../../../../splash2/codes//null_macros/c.m4.null"

#line 1 "slave1.C"
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

/*    ****************
      subroutine slave
      ****************  */


#line 21
#include <pthread.h>
#line 21
#include <sys/time.h>
#line 21
#include <unistd.h>
#line 21
#include <stdlib.h>
#line 21
#include "TriggerActionDecl.h"
#line 21
extern pthread_t PThreadTable[];
#line 21


#include <stdio.h>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include "decs.h"

void slave(void* vpIndex)
{
   int index = (unsigned int)vpIndex;
   {
#line 32
    init_stats(index);
#line 32
    };

   long i;
   long j;
   long nstep;
   long iindex;
   long iday;
   double ysca1;
   double y;
   double factor;
   double sintemp;
   double curlt;
   double ressqr;
   long istart;
   long iend;
   long jstart;
   long jend;
   long ist;
   long ien;
   long jst;
   long jen;
   double fac;
   long dayflag=0;
   long dhourflag=0;
   long endflag=0;
   long firstrow;
   long lastrow;
   long numrows;
   long firstcol;
   long lastcol;
   long numcols;
   long psiindex;
   double psibipriv;
   double ttime;
   double dhour;
   double day;
   long procid;
   long psinum;
   long j_off = 0;
   unsigned long t1;
   double **t2a;
   double **t2b;
   double *t1a;
   double *t1b;
   double *t1c;
   double *t1d;

   ressqr = lev_res[numlev-1] * lev_res[numlev-1];

   {pthread_mutex_lock(&(locks->idlock));}
     procid = global->id;
     global->id = global->id+1;
   {pthread_mutex_unlock(&(locks->idlock));}

#if defined(MULTIPLE_BARRIERS)
   {
#line 87
	unsigned long	Error, Cycle;
#line 87
	long		Cancel, Temp;
#line 87

#line 87
	Error = pthread_mutex_lock(&(bars->sl_prini).mutex);
#line 87
	if (Error != 0) {
#line 87
		printf("Error while trying to get lock in barrier.\n");
#line 87
		exit(-1);
#line 87
	}
#line 87

#line 87
	Cycle = (bars->sl_prini).cycle;
#line 87
	if (++(bars->sl_prini).counter != (nprocs)) {
#line 87
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &Cancel);
#line 87
		while (Cycle == (bars->sl_prini).cycle) {
#line 87
			Error = pthread_cond_wait(&(bars->sl_prini).cv, &(bars->sl_prini).mutex);
#line 87
			if (Error != 0) {
#line 87
				break;
#line 87
			}
#line 87
		}
#line 87
		pthread_setcancelstate(Cancel, &Temp);
#line 87
	} else {
#line 87
		(bars->sl_prini).cycle = !(bars->sl_prini).cycle;
#line 87
		(bars->sl_prini).counter = 0;
#line 87
		Error = pthread_cond_broadcast(&(bars->sl_prini).cv);
#line 87
	}
#line 87
	pthread_mutex_unlock(&(bars->sl_prini).mutex);
#line 87
}
#else
   {
#line 89
	unsigned long	Error, Cycle;
#line 89
	long		Cancel, Temp;
#line 89

#line 89
	Error = pthread_mutex_lock(&(bars->barrier).mutex);
#line 89
	if (Error != 0) {
#line 89
		printf("Error while trying to get lock in barrier.\n");
#line 89
		exit(-1);
#line 89
	}
#line 89

#line 89
	Cycle = (bars->barrier).cycle;
#line 89
	if (++(bars->barrier).counter != (nprocs)) {
#line 89
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &Cancel);
#line 89
		while (Cycle == (bars->barrier).cycle) {
#line 89
			Error = pthread_cond_wait(&(bars->barrier).cv, &(bars->barrier).mutex);
#line 89
			if (Error != 0) {
#line 89
				break;
#line 89
			}
#line 89
		}
#line 89
		pthread_setcancelstate(Cancel, &Temp);
#line 89
	} else {
#line 89
		(bars->barrier).cycle = !(bars->barrier).cycle;
#line 89
		(bars->barrier).counter = 0;
#line 89
		Error = pthread_cond_broadcast(&(bars->barrier).cv);
#line 89
	}
#line 89
	pthread_mutex_unlock(&(bars->barrier).mutex);
#line 89
}
#endif
/* POSSIBLE ENHANCEMENT:  Here is where one might pin processes to
   processors to avoid migration. */

/* POSSIBLE ENHANCEMENT:  Here is where one might distribute
   data structures across physically distributed memories as
   desired.

   One way to do this is as follows.  The function allocate(START,SIZE,I)
   is assumed to place all addresses x such that
   (START <= x < START+SIZE) on node I.

   long d_size;
   unsigned long g_size;
   unsigned long mg_size;

   if (procid == MASTER) {
     g_size = ((jmx[numlev-1]-2)/xprocs+2)*((imx[numlev-1]-2)/yprocs+2)*siz
eof(double) +
              ((imx[numlev-1]-2)/yprocs+2)*sizeof(double *);

     mg_size = numlev*sizeof(double **);
     for (i=0;i<numlev;i++) {
       mg_size+=((imx[i]-2)/yprocs+2)*((jmx[i]-2)/xprocs+2)*sizeof(double)+
                ((imx[i]-2)/yprocs+2)*sizeof(double *);
     }
     for (i= 0;i<nprocs;i++) {
       d_size = 2*sizeof(double **);
       allocate((unsigned long) psi[i],d_size,i);
       allocate((unsigned long) psim[i],d_size,i);
       allocate((unsigned long) work1[i],d_size,i);
       allocate((unsigned long) work4[i],d_size,i);
       allocate((unsigned long) work5[i],d_size,i);
       allocate((unsigned long) work7[i],d_size,i);
       allocate((unsigned long) temparray[i],d_size,i);
       allocate((unsigned long) psi[i][0],g_size,i);
       allocate((unsigned long) psi[i][1],g_size,i);
       allocate((unsigned long) psim[i][0],g_size,i);
       allocate((unsigned long) psim[i][1],g_size,i);
       allocate((unsigned long) psium[i],g_size,i);
       allocate((unsigned long) psilm[i],g_size,i);
       allocate((unsigned long) psib[i],g_size,i);
       allocate((unsigned long) ga[i],g_size,i);
       allocate((unsigned long) gb[i],g_size,i);
       allocate((unsigned long) work1[i][0],g_size,i);
       allocate((unsigned long) work1[i][1],g_size,i);
       allocate((unsigned long) work2[i],g_size,i);
       allocate((unsigned long) work3[i],g_size,i);
       allocate((unsigned long) work4[i][0],g_size,i);
       allocate((unsigned long) work4[i][1],g_size,i);
       allocate((unsigned long) work5[i][0],g_size,i);
       allocate((unsigned long) work5[i][1],g_size,i);
       allocate((unsigned long) work6[i],g_size,i);
       allocate((unsigned long) work7[i][0],g_size,i);
       allocate((unsigned long) work7[i][1],g_size,i);
       allocate((unsigned long) temparray[i][0],g_size,i);
       allocate((unsigned long) temparray[i][1],g_size,i);
       allocate((unsigned long) tauz[i],g_size,i);
       allocate((unsigned long) oldga[i],g_size,i);
       allocate((unsigned long) oldgb[i],g_size,i);
       d_size = numlev * sizeof(long);
       allocate((unsigned long) gp[i].rel_num_x,d_size,i);
       allocate((unsigned long) gp[i].rel_num_y,d_size,i);
       allocate((unsigned long) gp[i].eist,d_size,i);
       allocate((unsigned long) gp[i].ejst,d_size,i);
       allocate((unsigned long) gp[i].oist,d_size,i);
       allocate((unsigned long) gp[i].ojst,d_size,i);
       allocate((unsigned long) gp[i].rlist,d_size,i);
       allocate((unsigned long) gp[i].rljst,d_size,i);
       allocate((unsigned long) gp[i].rlien,d_size,i);
       allocate((unsigned long) gp[i].rljen,d_size,i);

       allocate((unsigned long) q_multi[i],mg_size,i);
       allocate((unsigned long) rhs_multi[i],mg_size,i);
       allocate((unsigned long) &(gp[i]),sizeof(struct Global_Private),i);
     }
   }

*/

   t2a = (double **) oldga[procid];
   t2b = (double **) oldgb[procid];
   for (i=0;i<im;i++) {
     t1a = (double *) t2a[i];
     t1b = (double *) t2b[i];
     for (j=0;j<jm;j++) {
        t1a[j] = 0.0;
        t1b[j] = 0.0;
     }
   }

   firstcol = 1;
   lastcol = firstcol + gp[procid].rel_num_x[numlev-1] - 1;
   firstrow = 1;
   lastrow = firstrow + gp[procid].rel_num_y[numlev-1] - 1;
   numcols = gp[procid].rel_num_x[numlev-1];
   numrows = gp[procid].rel_num_y[numlev-1];
   j_off = gp[procid].colnum*numcols;

   if (procid > nprocs/2) {
      psinum = 2;
   } else {
      psinum = 1;
   }

/* every process gets its own copy of the timing variables to avoid
   contention at shared memory locations.  here, these variables
   are initialized.  */

   ttime = 0.0;
   dhour = 0.0;
   nstep = 0 ;
   day = 0.0;

   ysca1 = 0.5*ysca;
   if (procid == MASTER) {
     t1a = (double *) f;
     for (iindex = 0;iindex<=jmx[numlev-1]-1;iindex++) {
       y = ((double) iindex)*res;
       t1a[iindex] = f0+beta*(y-ysca1);
     }
   }

   t2a = (double **) psium[procid];
   if ((gp[procid].neighbors[UP] == -1) && (gp[procid].neighbors[LEFT] == -1)) {
     t2a[0][0]=0.0;
   }
   if ((gp[procid].neighbors[DOWN] == -1) && (gp[procid].neighbors[LEFT] == -1)) {
     t2a[im-1][0]=0.0;
   }
   if ((gp[procid].neighbors[UP] == -1) && (gp[procid].neighbors[RIGHT] == -1)) {
     t2a[0][jm-1]=0.0;
   }
   if ((gp[procid].neighbors[DOWN] == -1) && (gp[procid].neighbors[RIGHT] == -1)) {
     t2a[im-1][jm-1]=0.0;
   }
   if (gp[procid].neighbors[UP] == -1) {
     t1a = (double *) t2a[0];
     for(j=firstcol;j<=lastcol;j++) {
       t1a[j] = 0.0;
     }
   }
   if (gp[procid].neighbors[DOWN] == -1) {
     t1a = (double *) t2a[im-1];
     for(j=firstcol;j<=lastcol;j++) {
       t1a[j] = 0.0;
     }
   }
   if (gp[procid].neighbors[LEFT] == -1) {
     for(j=firstrow;j<=lastrow;j++) {
       t2a[j][0] = 0.0;
     }
   }
   if (gp[procid].neighbors[RIGHT] == -1) {
     for(j=firstrow;j<=lastrow;j++) {
       t2a[j][jm-1] = 0.0;
     }
   }

   for(i=firstrow;i<=lastrow;i++) {
     t1a = (double *) t2a[i];
     for(iindex=firstcol;iindex<=lastcol;iindex++) {
       t1a[iindex] = 0.0;
     }
   }
   t2a = (double **) psilm[procid];
   if ((gp[procid].neighbors[UP] == -1) && (gp[procid].neighbors[LEFT] == -1)) {
     t2a[0][0]=0.0;
   }
   if ((gp[procid].neighbors[DOWN] == -1) && (gp[procid].neighbors[LEFT] == -1)) {
     t2a[im-1][0]=0.0;
   }
   if ((gp[procid].neighbors[UP] == -1) && (gp[procid].neighbors[RIGHT] == -1)) {
     t2a[0][jm-1]=0.0;
   }
   if ((gp[procid].neighbors[DOWN] == -1) && (gp[procid].neighbors[RIGHT] == -1)) {
     t2a[im-1][jm-1]=0.0;
   }
   if (gp[procid].neighbors[UP] == -1) {
     t1a = (double *) t2a[0];
     for(j=firstcol;j<=lastcol;j++) {
       t1a[j] = 0.0;
     }
   }
   if (gp[procid].neighbors[DOWN] == -1) {
     t1a = (double *) t2a[im-1];
     for(j=firstcol;j<=lastcol;j++) {
       t1a[j] = 0.0;
     }
   }
   if (gp[procid].neighbors[LEFT] == -1) {
     for(j=firstrow;j<=lastrow;j++) {
       t2a[j][0] = 0.0;
     }
   }
   if (gp[procid].neighbors[RIGHT] == -1) {
     for(j=firstrow;j<=lastrow;j++) {
       t2a[j][jm-1] = 0.0;
     }
   }
   for(i=firstrow;i<=lastrow;i++) {
     t1a = (double *) t2a[i];
     for(iindex=firstcol;iindex<=lastcol;iindex++) {
       t1a[iindex] = 0.0;
     }
   }

   t2a = (double **) psib[procid];
   if ((gp[procid].neighbors[UP] == -1) && (gp[procid].neighbors[LEFT] == -1)) {
     t2a[0][0]=1.0;
   }
   if ((gp[procid].neighbors[UP] == -1) && (gp[procid].neighbors[RIGHT] == -1)) {
     t2a[0][jm-1]=1.0;
   }
   if ((gp[procid].neighbors[DOWN] == -1) && (gp[procid].neighbors[LEFT] == -1)) {
     t2a[im-1][0]=1.0;
   }
   if ((gp[procid].neighbors[DOWN] == -1) && (gp[procid].neighbors[RIGHT] == -1)) {
     t2a[im-1][jm-1]=1.0;
   }
   if (gp[procid].neighbors[UP] == -1) {
     t1a = (double *) t2a[0];
     for(j=firstcol;j<=lastcol;j++) {
       t1a[j] = 1.0;
     }
   }
   if (gp[procid].neighbors[DOWN] == -1) {
     t1a = (double *) t2a[im-1];
     for(j=firstcol;j<=lastcol;j++) {
       t1a[j] = 1.0;
     }
   }
   if (gp[procid].neighbors[LEFT] == -1) {
     for(j=firstrow;j<=lastrow;j++) {
       t2a[j][0] = 1.0;
     }
   }
   if (gp[procid].neighbors[RIGHT] == -1) {
     for(j=firstrow;j<=lastrow;j++) {
       t2a[j][jm-1] = 1.0;
     }
   }
   for(i=firstrow;i<=lastrow;i++) {
     t1a = (double *) t2a[i];
     for(iindex=firstcol;iindex<=lastcol;iindex++) {
       t1a[iindex] = 0.0;
     }
   }

/* wait until all processes have completed the above initialization  */
#if defined(MULTIPLE_BARRIERS)
   {
#line 341
	unsigned long	Error, Cycle;
#line 341
	long		Cancel, Temp;
#line 341

#line 341
	Error = pthread_mutex_lock(&(bars->sl_prini).mutex);
#line 341
	if (Error != 0) {
#line 341
		printf("Error while trying to get lock in barrier.\n");
#line 341
		exit(-1);
#line 341
	}
#line 341

#line 341
	Cycle = (bars->sl_prini).cycle;
#line 341
	if (++(bars->sl_prini).counter != (nprocs)) {
#line 341
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &Cancel);
#line 341
		while (Cycle == (bars->sl_prini).cycle) {
#line 341
			Error = pthread_cond_wait(&(bars->sl_prini).cv, &(bars->sl_prini).mutex);
#line 341
			if (Error != 0) {
#line 341
				break;
#line 341
			}
#line 341
		}
#line 341
		pthread_setcancelstate(Cancel, &Temp);
#line 341
	} else {
#line 341
		(bars->sl_prini).cycle = !(bars->sl_prini).cycle;
#line 341
		(bars->sl_prini).counter = 0;
#line 341
		Error = pthread_cond_broadcast(&(bars->sl_prini).cv);
#line 341
	}
#line 341
	pthread_mutex_unlock(&(bars->sl_prini).mutex);
#line 341
}
#else
   {
#line 343
	unsigned long	Error, Cycle;
#line 343
	long		Cancel, Temp;
#line 343

#line 343
	Error = pthread_mutex_lock(&(bars->barrier).mutex);
#line 343
	if (Error != 0) {
#line 343
		printf("Error while trying to get lock in barrier.\n");
#line 343
		exit(-1);
#line 343
	}
#line 343

#line 343
	Cycle = (bars->barrier).cycle;
#line 343
	if (++(bars->barrier).counter != (nprocs)) {
#line 343
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &Cancel);
#line 343
		while (Cycle == (bars->barrier).cycle) {
#line 343
			Error = pthread_cond_wait(&(bars->barrier).cv, &(bars->barrier).mutex);
#line 343
			if (Error != 0) {
#line 343
				break;
#line 343
			}
#line 343
		}
#line 343
		pthread_setcancelstate(Cancel, &Temp);
#line 343
	} else {
#line 343
		(bars->barrier).cycle = !(bars->barrier).cycle;
#line 343
		(bars->barrier).counter = 0;
#line 343
		Error = pthread_cond_broadcast(&(bars->barrier).cv);
#line 343
	}
#line 343
	pthread_mutex_unlock(&(bars->barrier).mutex);
#line 343
}
#endif
/* compute psib array (one-time computation) and integrate into psibi */

   istart = 1;
   iend = istart + gp[procid].rel_num_y[numlev-1] - 1;
   jstart = 1;
   jend = jstart + gp[procid].rel_num_x[numlev-1] - 1;
   ist = istart;
   ien = iend;
   jst = jstart;
   jen = jend;

   if (gp[procid].neighbors[UP] == -1) {
     istart = 0;
   }
   if (gp[procid].neighbors[LEFT] == -1) {
     jstart = 0;
   }
   if (gp[procid].neighbors[DOWN] == -1) {
     iend = im-1;
   }
   if (gp[procid].neighbors[RIGHT] == -1) {
     jend = jm-1;
   }

   t2a = (double **) rhs_multi[procid][numlev-1];
   t2b = (double **) psib[procid];
   for(i=istart;i<=iend;i++) {
     t1a = (double *) t2a[i];
     t1b = (double *) t2b[i];
     for(j=jstart;j<=jend;j++) {
       t1a[j] = t1b[j] * ressqr;
     }
   }
   t2a = (double **) q_multi[procid][numlev-1];
   if (gp[procid].neighbors[UP] == -1) {
     t1a = (double *) t2a[0];
     t1b = (double *) t2b[0];
     for(j=jstart;j<=jend;j++) {
       t1a[j] = t1b[j];
     }
   }
   if (gp[procid].neighbors[DOWN] == -1) {
     t1a = (double *) t2a[im-1];
     t1b = (double *) t2b[im-1];
     for(j=jstart;j<=jend;j++) {
       t1a[j] = t1b[j];
     }
   }
   if (gp[procid].neighbors[LEFT] == -1) {
     for(i=istart;i<=iend;i++) {
       t2a[i][0] = t2b[i][0];
     }
   }
   if (gp[procid].neighbors[RIGHT] == -1) {
     for(i=istart;i<=iend;i++) {
       t2a[i][jm-1] = t2b[i][jm-1];
     }
   }
#if defined(MULTIPLE_BARRIERS)
   {
#line 404
	unsigned long	Error, Cycle;
#line 404
	long		Cancel, Temp;
#line 404

#line 404
	Error = pthread_mutex_lock(&(bars->sl_psini).mutex);
#line 404
	if (Error != 0) {
#line 404
		printf("Error while trying to get lock in barrier.\n");
#line 404
		exit(-1);
#line 404
	}
#line 404

#line 404
	Cycle = (bars->sl_psini).cycle;
#line 404
	if (++(bars->sl_psini).counter != (nprocs)) {
#line 404
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &Cancel);
#line 404
		while (Cycle == (bars->sl_psini).cycle) {
#line 404
			Error = pthread_cond_wait(&(bars->sl_psini).cv, &(bars->sl_psini).mutex);
#line 404
			if (Error != 0) {
#line 404
				break;
#line 404
			}
#line 404
		}
#line 404
		pthread_setcancelstate(Cancel, &Temp);
#line 404
	} else {
#line 404
		(bars->sl_psini).cycle = !(bars->sl_psini).cycle;
#line 404
		(bars->sl_psini).counter = 0;
#line 404
		Error = pthread_cond_broadcast(&(bars->sl_psini).cv);
#line 404
	}
#line 404
	pthread_mutex_unlock(&(bars->sl_psini).mutex);
#line 404
}
#else
   {
#line 406
	unsigned long	Error, Cycle;
#line 406
	long		Cancel, Temp;
#line 406

#line 406
	Error = pthread_mutex_lock(&(bars->barrier).mutex);
#line 406
	if (Error != 0) {
#line 406
		printf("Error while trying to get lock in barrier.\n");
#line 406
		exit(-1);
#line 406
	}
#line 406

#line 406
	Cycle = (bars->barrier).cycle;
#line 406
	if (++(bars->barrier).counter != (nprocs)) {
#line 406
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &Cancel);
#line 406
		while (Cycle == (bars->barrier).cycle) {
#line 406
			Error = pthread_cond_wait(&(bars->barrier).cv, &(bars->barrier).mutex);
#line 406
			if (Error != 0) {
#line 406
				break;
#line 406
			}
#line 406
		}
#line 406
		pthread_setcancelstate(Cancel, &Temp);
#line 406
	} else {
#line 406
		(bars->barrier).cycle = !(bars->barrier).cycle;
#line 406
		(bars->barrier).counter = 0;
#line 406
		Error = pthread_cond_broadcast(&(bars->barrier).cv);
#line 406
	}
#line 406
	pthread_mutex_unlock(&(bars->barrier).mutex);
#line 406
}
#endif
   t2a = (double **) psib[procid];
   j = gp[procid].neighbors[UP];
   if (j != -1) {
     t1a = (double *) t2a[0];
     t1b = (double *) psib[j][im-2];
     for (i=1;i<jm-1;i++) {
       t1a[i] = t1b[i];
     }
   }
   j = gp[procid].neighbors[DOWN];
   if (j != -1) {
     t1a = (double *) t2a[im-1];
     t1b = (double *) psib[j][1];
     for (i=1;i<jm-1;i++) {
       t1a[i] = t1b[i];
     }
   }
   j = gp[procid].neighbors[LEFT];
   if (j != -1) {
     t2b = (double **) psib[j];
     for (i=1;i<im-1;i++) {
       t2a[i][0] = t2b[i][jm-2];
     }
   }
   j = gp[procid].neighbors[RIGHT];
   if (j != -1) {
     t2b = (double **) psib[j];
     for (i=1;i<im-1;i++) {
       t2a[i][jm-1] = t2b[i][1];
     }
   }

   t2a = (double **) q_multi[procid][numlev-1];
   t2b = (double **) psib[procid];
   fac = 1.0 / (4.0 - ressqr*eig2);
   for(i=ist;i<=ien;i++) {
     t1a = (double *) t2a[i];
     t1b = (double *) t2b[i];
     t1c = (double *) t2b[i-1];
     t1d = (double *) t2b[i+1];
     for(j=jst;j<=jen;j++) {
       t1a[j] = fac * (t1d[j]+t1c[j]+t1b[j+1]+t1b[j-1] -
                   ressqr*t1b[j]);
     }
   }

   multig(procid);

   for(i=istart;i<=iend;i++) {
     t1a = (double *) t2a[i];
     t1b = (double *) t2b[i];
     for(j=jstart;j<=jend;j++) {
       t1b[j] = t1a[j];
     }
   }
#if defined(MULTIPLE_BARRIERS)
   {
#line 464
	unsigned long	Error, Cycle;
#line 464
	long		Cancel, Temp;
#line 464

#line 464
	Error = pthread_mutex_lock(&(bars->sl_prini).mutex);
#line 464
	if (Error != 0) {
#line 464
		printf("Error while trying to get lock in barrier.\n");
#line 464
		exit(-1);
#line 464
	}
#line 464

#line 464
	Cycle = (bars->sl_prini).cycle;
#line 464
	if (++(bars->sl_prini).counter != (nprocs)) {
#line 464
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &Cancel);
#line 464
		while (Cycle == (bars->sl_prini).cycle) {
#line 464
			Error = pthread_cond_wait(&(bars->sl_prini).cv, &(bars->sl_prini).mutex);
#line 464
			if (Error != 0) {
#line 464
				break;
#line 464
			}
#line 464
		}
#line 464
		pthread_setcancelstate(Cancel, &Temp);
#line 464
	} else {
#line 464
		(bars->sl_prini).cycle = !(bars->sl_prini).cycle;
#line 464
		(bars->sl_prini).counter = 0;
#line 464
		Error = pthread_cond_broadcast(&(bars->sl_prini).cv);
#line 464
	}
#line 464
	pthread_mutex_unlock(&(bars->sl_prini).mutex);
#line 464
}
#else
   {
#line 466
	unsigned long	Error, Cycle;
#line 466
	long		Cancel, Temp;
#line 466

#line 466
	Error = pthread_mutex_lock(&(bars->barrier).mutex);
#line 466
	if (Error != 0) {
#line 466
		printf("Error while trying to get lock in barrier.\n");
#line 466
		exit(-1);
#line 466
	}
#line 466

#line 466
	Cycle = (bars->barrier).cycle;
#line 466
	if (++(bars->barrier).counter != (nprocs)) {
#line 466
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &Cancel);
#line 466
		while (Cycle == (bars->barrier).cycle) {
#line 466
			Error = pthread_cond_wait(&(bars->barrier).cv, &(bars->barrier).mutex);
#line 466
			if (Error != 0) {
#line 466
				break;
#line 466
			}
#line 466
		}
#line 466
		pthread_setcancelstate(Cancel, &Temp);
#line 466
	} else {
#line 466
		(bars->barrier).cycle = !(bars->barrier).cycle;
#line 466
		(bars->barrier).counter = 0;
#line 466
		Error = pthread_cond_broadcast(&(bars->barrier).cv);
#line 466
	}
#line 466
	pthread_mutex_unlock(&(bars->barrier).mutex);
#line 466
}
#endif
/* update the local running sum psibipriv by summing all the resulting
   values in that process's share of the psib matrix   */

   t2a = (double **) psib[procid];
   psibipriv=0.0;
   if ((gp[procid].neighbors[UP] == -1) && (gp[procid].neighbors[LEFT] == -1)) {
     psibipriv = psibipriv + 0.25*(t2a[0][0]);
   }
   if ((gp[procid].neighbors[UP] == -1) && (gp[procid].neighbors[RIGHT] == -1)) {
     psibipriv = psibipriv + 0.25*(t2a[0][jm-1]);
   }
   if ((gp[procid].neighbors[DOWN] == -1) && (gp[procid].neighbors[LEFT] == -1)) {
     psibipriv=psibipriv+0.25*(t2a[im-1][0]);
   }
   if ((gp[procid].neighbors[DOWN] == -1) && (gp[procid].neighbors[RIGHT] == -1)) {
     psibipriv=psibipriv+0.25*(t2a[im-1][jm-1]);
   }
   if (gp[procid].neighbors[UP] == -1) {
     t1a = (double *) t2a[0];
     for(j=firstcol;j<=lastcol;j++) {
       psibipriv = psibipriv + 0.5*t1a[j];
     }
   }
   if (gp[procid].neighbors[DOWN] == -1) {
     t1a = (double *) t2a[im-1];
     for(j=firstcol;j<=lastcol;j++) {
       psibipriv = psibipriv + 0.5*t1a[j];
     }
   }
   if (gp[procid].neighbors[LEFT] == -1) {
     for(j=firstrow;j<=lastrow;j++) {
       psibipriv = psibipriv + 0.5*t2a[j][0];
     }
   }
   if (gp[procid].neighbors[RIGHT] == -1) {
     for(j=firstrow;j<=lastrow;j++) {
       psibipriv = psibipriv + 0.5*t2a[j][jm-1];
     }
   }
   for(i=firstrow;i<=lastrow;i++) {
     t1a = (double *) t2a[i];
     for(iindex=firstcol;iindex<=lastcol;iindex++) {
       psibipriv = psibipriv + t1a[iindex];
     }
   }

/* update the shared variable psibi by summing all the psibiprivs
   of the individual processes into it.  note that this combined
   private and shared sum method avoids accessing the shared
   variable psibi once for every element of the matrix.  */

   {pthread_mutex_lock(&(locks->psibilock));}
     global->psibi = global->psibi + psibipriv;
   {pthread_mutex_unlock(&(locks->psibilock));}

/* initialize psim matrices

   if there is more than one process, then split the processes
   between the two psim matrices; otherwise, let the single process
   work on one first and then the other   */

   for(psiindex=0;psiindex<=1;psiindex++) {
     t2a = (double **) psim[procid][psiindex];
     if ((gp[procid].neighbors[UP] == -1) && (gp[procid].neighbors[LEFT] == -1)) {
       t2a[0][0] = 0.0;
     }
     if ((gp[procid].neighbors[DOWN] == -1) && (gp[procid].neighbors[LEFT] == -1)) {
       t2a[im-1][0] = 0.0;
     }
     if ((gp[procid].neighbors[UP] == -1) && (gp[procid].neighbors[RIGHT] == -1)) {
       t2a[0][jm-1] = 0.0;
     }
     if ((gp[procid].neighbors[DOWN] == -1) && (gp[procid].neighbors[RIGHT] == -1)) {
       t2a[im-1][jm-1] = 0.0;
     }
     if (gp[procid].neighbors[UP] == -1) {
       t1a = (double *) t2a[0];
       for(j=firstcol;j<=lastcol;j++) {
         t1a[j] = 0.0;
       }
     }
     if (gp[procid].neighbors[DOWN] == -1) {
       t1a = (double *) t2a[im-1];
       for(j=firstcol;j<=lastcol;j++) {
         t1a[j] = 0.0;
       }
     }
     if (gp[procid].neighbors[LEFT] == -1) {
       for(j=firstrow;j<=lastrow;j++) {
         t2a[j][0] = 0.0;
       }
     }
     if (gp[procid].neighbors[RIGHT] == -1) {
       for(j=firstrow;j<=lastrow;j++) {
         t2a[j][jm-1] = 0.0;
       }
     }
     for(i=firstrow;i<=lastrow;i++) {
       t1a = (double *) t2a[i];
       for(iindex=firstcol;iindex<=lastcol;iindex++) {
         t1a[iindex] = 0.0;
       }
     }
   }

/* initialize psi matrices the same way  */

   for(psiindex=0;psiindex<=1;psiindex++) {
     t2a = (double **) psi[procid][psiindex];
     if ((gp[procid].neighbors[UP] == -1) && (gp[procid].neighbors[LEFT] == -1)) {
       t2a[0][0] = 0.0;
     }
     if ((gp[procid].neighbors[UP] == -1) && (gp[procid].neighbors[RIGHT] == -1)) {
       t2a[0][jm-1] = 0.0;
     }
     if ((gp[procid].neighbors[DOWN] == -1) && (gp[procid].neighbors[LEFT] == -1)) {
       t2a[im-1][0] = 0.0;
     }
     if ((gp[procid].neighbors[DOWN] == -1) && (gp[procid].neighbors[RIGHT] == -1)) {
       t2a[im-1][jm-1] = 0.0;
     }
     if (gp[procid].neighbors[UP] == -1) {
       t1a = (double *) t2a[0];
       for(j=firstcol;j<=lastcol;j++) {
         t1a[j] = 0.0;
       }
     }
     if (gp[procid].neighbors[DOWN] == -1) {
       t1a = (double *) t2a[im-1];
       for(j=firstcol;j<=lastcol;j++) {
         t1a[j] = 0.0;
       }
     }
     if (gp[procid].neighbors[LEFT] == -1) {
       for(j=firstrow;j<=lastrow;j++) {
         t2a[j][0] = 0.0;
       }
     }
     if (gp[procid].neighbors[RIGHT] == -1) {
       for(j=firstrow;j<=lastrow;j++) {
         t2a[j][jm-1] = 0.0;
       }
     }
     for(i=firstrow;i<=lastrow;i++) {
       t1a = (double *) t2a[i];
       for(iindex=firstcol;iindex<=lastcol;iindex++) {
         t1a[iindex] = 0.0;
       }
     }
   }

/* compute input curl of wind stress */

   t2a = (double **) tauz[procid];
   ysca1 = .5*ysca;
   factor= -t0*pi/ysca1;
   if ((gp[procid].neighbors[UP] == -1) && (gp[procid].neighbors[LEFT] == -1)) {
     t2a[0][0] = 0.0;
   }
   if ((gp[procid].neighbors[DOWN] == -1) && (gp[procid].neighbors[LEFT] == -1)) {
     t2a[im-1][0] = 0.0;
   }
   if ((gp[procid].neighbors[UP] == -1) && (gp[procid].neighbors[RIGHT] == -1)) {
     sintemp = pi*((double) jm-1+j_off)*res/ysca1;
     sintemp = sin(sintemp);
     t2a[0][jm-1] = factor*sintemp;
   }
   if ((gp[procid].neighbors[DOWN] == -1) && (gp[procid].neighbors[RIGHT] == -1)) {
     sintemp = pi*((double) jm-1+j_off)*res/ysca1;
     sintemp = sin(sintemp);
     t2a[im-1][jm-1] = factor*sintemp;
   }
   if (gp[procid].neighbors[UP] == -1) {
     t1a = (double *) t2a[0];
     for(j=firstcol;j<=lastcol;j++) {
       sintemp = pi*((double) j+j_off)*res/ysca1;
       sintemp = sin(sintemp);
       curlt = factor*sintemp;
       t1a[j] = curlt;
     }
   }
   if (gp[procid].neighbors[DOWN] == -1) {
     t1a = (double *) t2a[im-1];
     for(j=firstcol;j<=lastcol;j++) {
       sintemp = pi*((double) j+j_off)*res/ysca1;
       sintemp = sin(sintemp);
       curlt = factor*sintemp;
       t1a[j] = curlt;
     }
   }
   if (gp[procid].neighbors[LEFT] == -1) {
     for(j=firstrow;j<=lastrow;j++) {
       t2a[j][0] = 0.0;
     }
   }
   if (gp[procid].neighbors[RIGHT] == -1) {
     sintemp = pi*((double) jm-1+j_off)*res/ysca1;
     sintemp = sin(sintemp);
     curlt = factor*sintemp;
     for(j=firstrow;j<=lastrow;j++) {
       t2a[j][jm-1] = curlt;
     }
   }
   for(i=firstrow;i<=lastrow;i++) {
     t1a = (double *) t2a[i];
     for(iindex=firstcol;iindex<=lastcol;iindex++) {
       sintemp = pi*((double) iindex+j_off)*res/ysca1;
       sintemp = sin(sintemp);
       curlt = factor*sintemp;
       t1a[iindex] = curlt;
     }
   }
#if defined(MULTIPLE_BARRIERS)
   {
#line 681
	unsigned long	Error, Cycle;
#line 681
	long		Cancel, Temp;
#line 681

#line 681
	Error = pthread_mutex_lock(&(bars->sl_onetime).mutex);
#line 681
	if (Error != 0) {
#line 681
		printf("Error while trying to get lock in barrier.\n");
#line 681
		exit(-1);
#line 681
	}
#line 681

#line 681
	Cycle = (bars->sl_onetime).cycle;
#line 681
	if (++(bars->sl_onetime).counter != (nprocs)) {
#line 681
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &Cancel);
#line 681
		while (Cycle == (bars->sl_onetime).cycle) {
#line 681
			Error = pthread_cond_wait(&(bars->sl_onetime).cv, &(bars->sl_onetime).mutex);
#line 681
			if (Error != 0) {
#line 681
				break;
#line 681
			}
#line 681
		}
#line 681
		pthread_setcancelstate(Cancel, &Temp);
#line 681
	} else {
#line 681
		(bars->sl_onetime).cycle = !(bars->sl_onetime).cycle;
#line 681
		(bars->sl_onetime).counter = 0;
#line 681
		Error = pthread_cond_broadcast(&(bars->sl_onetime).cv);
#line 681
	}
#line 681
	pthread_mutex_unlock(&(bars->sl_onetime).mutex);
#line 681
}
#else
   {
#line 683
	unsigned long	Error, Cycle;
#line 683
	long		Cancel, Temp;
#line 683

#line 683
	Error = pthread_mutex_lock(&(bars->barrier).mutex);
#line 683
	if (Error != 0) {
#line 683
		printf("Error while trying to get lock in barrier.\n");
#line 683
		exit(-1);
#line 683
	}
#line 683

#line 683
	Cycle = (bars->barrier).cycle;
#line 683
	if (++(bars->barrier).counter != (nprocs)) {
#line 683
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &Cancel);
#line 683
		while (Cycle == (bars->barrier).cycle) {
#line 683
			Error = pthread_cond_wait(&(bars->barrier).cv, &(bars->barrier).mutex);
#line 683
			if (Error != 0) {
#line 683
				break;
#line 683
			}
#line 683
		}
#line 683
		pthread_setcancelstate(Cancel, &Temp);
#line 683
	} else {
#line 683
		(bars->barrier).cycle = !(bars->barrier).cycle;
#line 683
		(bars->barrier).counter = 0;
#line 683
		Error = pthread_cond_broadcast(&(bars->barrier).cv);
#line 683
	}
#line 683
	pthread_mutex_unlock(&(bars->barrier).mutex);
#line 683
}
#endif

/***************************************************************
 one-time stuff over at this point
 ***************************************************************/

   while (!endflag) {
     while ((!dayflag) || (!dhourflag)) {
       dayflag = 0;
       dhourflag = 0;
       if (nstep == 1) {
         if (procid == MASTER) {
            {
#line 696
	struct timeval	FullTime;
#line 696

#line 696
	gettimeofday(&FullTime, NULL);
#line 696
	(global->trackstart) = (unsigned long)(FullTime.tv_usec + FullTime.tv_sec * 1000000);
#line 696
}
         }
	 if ((procid == MASTER) || (do_stats)) {
	   {
#line 699
	struct timeval	FullTime;
#line 699

#line 699
	gettimeofday(&FullTime, NULL);
#line 699
	(t1) = (unsigned long)(FullTime.tv_usec + FullTime.tv_sec * 1000000);
#line 699
};
           gp[procid].total_time = t1;
           gp[procid].multi_time = 0;
	 }
/* POSSIBLE ENHANCEMENT:  Here is where one might reset the
   statistics that one is measuring about the parallel execution */
       }

       slave2(procid,firstrow,lastrow,numrows,firstcol,lastcol,numcols);

/* update time and step number
   note that these time and step variables are private i.e. every
   process has its own copy and keeps track of its own time  */

       ttime = ttime + dtau;
       nstep = nstep + 1;
       day = ttime/86400.0;

       if (day > ((double) outday0)) {
         dayflag = 1;
         iday = (long) day;
         dhour = dhour+dtau;
         if (dhour >= 86400.0) {
	   dhourflag = 1;
         }
       }
     }
     dhour = 0.0;

     t2a = (double **) psium[procid];
     t2b = (double **) psim[procid][0];
     if ((gp[procid].neighbors[UP] == -1) && (gp[procid].neighbors[LEFT] == -1)) {
       t2a[0][0] = t2a[0][0]+t2b[0][0];
     }
     if ((gp[procid].neighbors[DOWN] == -1) && (gp[procid].neighbors[LEFT] == -1)) {
       t2a[im-1][0] = t2a[im-1][0]+t2b[im-1][0];
     }
     if ((gp[procid].neighbors[UP] == -1) && (gp[procid].neighbors[RIGHT] == -1)) {
       t2a[0][jm-1] = t2a[0][jm-1]+t2b[0][jm-1];
     }
     if ((gp[procid].neighbors[DOWN] == -1) && (gp[procid].neighbors[RIGHT] == -1)) {
       t2a[im-1][jm-1] = t2a[im-1][jm-1] +
				   t2b[im-1][jm-1];
     }
     if (gp[procid].neighbors[UP] == -1) {
       t1a = (double *) t2a[0];
       t1b = (double *) t2b[0];
       for(j=firstcol;j<=lastcol;j++) {
         t1a[j] = t1a[j]+t1b[j];
       }
     }
     if (gp[procid].neighbors[DOWN] == -1) {
       t1a = (double *) t2a[im-1];
       t1b = (double *) t2b[im-1];
       for(j=firstcol;j<=lastcol;j++) {
         t1a[j] = t1a[j] + t1b[j];
       }
     }
     if (gp[procid].neighbors[LEFT] == -1) {
       for(j=firstrow;j<=lastrow;j++) {
         t2a[j][0] = t2a[j][0]+t2b[j][0];
       }
     }
     if (gp[procid].neighbors[RIGHT] == -1) {
       for(j=firstrow;j<=lastrow;j++) {
         t2a[j][jm-1] = t2a[j][jm-1] +
				  t2b[j][jm-1];
       }
     }
     for(i=firstrow;i<=lastrow;i++) {
       t1a = (double *) t2a[i];
       t1b = (double *) t2b[i];
       for(iindex=firstcol;iindex<=lastcol;iindex++) {
         t1a[iindex] = t1a[iindex] + t1b[iindex];
       }
     }

/* update values of psilm array to psilm + psim[2]  */

     t2a = (double **) psilm[procid];
     t2b = (double **) psim[procid][1];
     if ((gp[procid].neighbors[UP] == -1) && (gp[procid].neighbors[LEFT] == -1)) {
       t2a[0][0] = t2a[0][0]+t2b[0][0];
     }
     if ((gp[procid].neighbors[DOWN] == -1) && (gp[procid].neighbors[LEFT] == -1)) {
       t2a[im-1][0] = t2a[im-1][0]+t2b[im-1][0];
     }
     if ((gp[procid].neighbors[UP] == -1) && (gp[procid].neighbors[RIGHT] == -1)) {
       t2a[0][jm-1] = t2a[0][jm-1]+t2b[0][jm-1];
     }
     if ((gp[procid].neighbors[DOWN] == -1) && (gp[procid].neighbors[RIGHT] == -1)) {
       t2a[im-1][jm-1] = t2a[im-1][jm-1] +
				   t2b[im-1][jm-1];
     }
     if (gp[procid].neighbors[UP] == -1) {
       t1a = (double *) t2a[0];
       t1b = (double *) t2b[0];
       for(j=firstcol;j<=lastcol;j++) {
         t1a[j] = t1a[j]+t1b[j];
       }
     }
     if (gp[procid].neighbors[DOWN] == -1) {
       t1a = (double *) t2a[im-1];
       t1b = (double *) t2b[im-1];
       for(j=firstcol;j<=lastcol;j++) {
         t1a[j] = t1a[j]+t1b[j];
       }
     }
     if (gp[procid].neighbors[LEFT] == -1) {
       for(j=firstrow;j<=lastrow;j++) {
         t2a[j][0] = t2a[j][0]+t2b[j][0];
       }
     }
     if (gp[procid].neighbors[RIGHT] == -1) {
       for(j=firstrow;j<=lastrow;j++) {
         t2a[j][jm-1] = t2a[j][jm-1] + t2b[j][jm-1];
       }
     }
     for(i=firstrow;i<=lastrow;i++) {
       t1a = (double *) t2a[i];
       t1b = (double *) t2b[i];
       for(iindex=firstcol;iindex<=lastcol;iindex++) {
         t1a[iindex] = t1a[iindex] + t1b[iindex];
       }
     }
     if (iday >= (long) outday3) {
       endflag = 1;
     }
  }
  if ((procid == MASTER) || (do_stats)) {
    {
#line 829
	struct timeval	FullTime;
#line 829

#line 829
	gettimeofday(&FullTime, NULL);
#line 829
	(t1) = (unsigned long)(FullTime.tv_usec + FullTime.tv_sec * 1000000);
#line 829
};
    gp[procid].total_time = t1-gp[procid].total_time;
  }
  print_timing_stats();
}

