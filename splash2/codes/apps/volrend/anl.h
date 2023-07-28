#line 270 "../../../../splash2/codes//null_macros/c.m4.null"

#line 1 "anl.H"
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

/*************************************************************************
*                                                                        *
*     anl.H:  ANL macros-related stuff, file should be included at end   *
*              of static definitions section before function definitions *
*                                                                        *
**************************************************************************/

#define PAD 256

struct GlobalMemory {
  volatile long Index,Counter;
  volatile long Queue[MAX_NUMPROC+1][PAD];
  
#line 29
struct {
#line 29
	pthread_mutex_t	mutex;
#line 29
	pthread_cond_t	cv;
#line 29
	unsigned long	counter;
#line 29
	unsigned long	cycle;
#line 29
} (SlaveBarrier);
#line 29

  
#line 30
struct {
#line 30
	pthread_mutex_t	mutex;
#line 30
	pthread_cond_t	cv;
#line 30
	unsigned long	counter;
#line 30
	unsigned long	cycle;
#line 30
} (TimeBarrier);
#line 30

  pthread_mutex_t (IndexLock);
  pthread_mutex_t (CountLock);
  pthread_mutex_t QLock[MAX_NUMPROC+1];
  };


