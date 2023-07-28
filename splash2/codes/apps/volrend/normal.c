#line 270 "../../../../splash2/codes//null_macros/c.m4.null"

#line 1 "normal.C"
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

/******************************************************************************
*                                                                             *
*   normal.c:  Compute normal map from density map, assuming gradient         *
*              operator is 1x3 and shading transition region is 0.            *
*                                                                             *
******************************************************************************/

#include <string.h>
#include "incl.h"

/* The following declarations show the layout of the .norm file.             */
/* If changed, the version number must be incremented and code               */
/* written to handle loading of both old and current versions.               */

				/* Version for new .norm files:              */
#define	NORM_CUR_VERSION   1	/*   Initial release                         */
short norm_version;		/* Version of this .norm file                */

short norm_len[NM];		/* Size of this normal map                   */

int norm_length;		/* Total number of normals in map            */
				/*   (= NM * product of lens)                */
NORMAL *norm_address;		/* Pointer to normal map                     */

/* End of layout of .norm file.                                              */

float nmag_epsilon;

extern int thrIndex;


#line 47
#include <pthread.h>
#line 47
#include <sys/time.h>
#line 47
#include <unistd.h>
#line 47
#include <stdlib.h>
#line 47
#include "TriggerActionDecl.h"
#line 47
extern pthread_t PThreadTable[];
#line 47


#include "anl.h"

void Compute_Normal()
{
  long i;

  for (i=0; i<NM; i++) {
    norm_len[i] = map_len[i]-2*INSET;
  }

  norm_length = norm_len[X] * norm_len[Y] * norm_len[Z];

  Allocate_Normal(&norm_address, norm_length);

  printf("    Computing normal...\n");

  Global->Index = NODE0;

  int curr_index = thrIndex;
#ifndef SERIAL_PREPROC
  for (i=1; i<num_nodes; i++) {
#line 69
	long	i, Error;
#line 69

#line 69
	for (i = 0; i < () - 1; i++) {
#line 69
		Error = pthread_create(&PThreadTable[i], NULL, (void * (*)(void *))(Normal_Compute), (void*)thrIndex);
#line 69
    thrIndex++;
#line 69
		if (Error != 0) {
#line 69
			printf("Error in pthread_create().\n");
#line 69
			exit(-1);
#line 69
		}
#line 69
	}
#line 69
	Normal_Compute((void*)0);
#line 69
}
#endif

  Normal_Compute(curr_index);
}


void Allocate_Normal(address, length)
     NORMAL **address;
     long length;
{
  long i;

  printf("    Allocating normal map of %ld bytes...\n",
	 length*sizeof(NORMAL));

  *address = (NORMAL *)malloc(length*sizeof(NORMAL));;

  if (*address == NULL)
    Error("    No space available for map.\n");

/*  POSSIBLE ENHANCEMENT:  Here's where one might distribute the
    normal map among physical memories if one wanted to.
*/

  for (i=0; i<length; i++) *(*address+i) = 0;

}


void Normal_Compute(void *vpIndex)
{
  int index = (unsigned int)vpIndex;
  {
#line 102
    init_stats(index);
#line 102
    };

  long inx,iny,inz;	/* Voxel location in object space            */
  long outx,outy,outz;	/* Loop indices in image space               */
  NORMAL *local_norm_address;
  double inv_mag_shift,magnitude,norm_lshift,grd_x,grd_y,grd_z;
  long zstart,zstop;
  long xnorm,ynorm,znorm,norm0;
  long num_xqueue,num_yqueue,num_zqueue,num_queue;
  long xstart,xstop,ystart,ystop;
  long my_node;

  {pthread_mutex_lock(&(Global->IndexLock));};
  my_node = Global->Index++;
  {pthread_mutex_unlock(&(Global->IndexLock));};
  my_node = my_node%num_nodes;

/*  POSSIBLE ENHANCEMENT:  Here's where one might bind the process to a
    processor, if one wanted to.
*/

  /* use these to save work in loop */
  norm0 = (long)(NORM_LSHIFT);
  norm_lshift = -NORM_LSHIFT;
  nmag_epsilon = magnitude_epsilon;

  num_xqueue = ROUNDUP((float)norm_len[X]/(float)voxel_section[X]);
  num_yqueue = ROUNDUP((float)norm_len[Y]/(float)voxel_section[Y]);
  num_zqueue = ROUNDUP((float)norm_len[Z]/(float)voxel_section[Z]);
  num_queue = num_xqueue * num_yqueue * num_zqueue;
  xstart = (my_node % voxel_section[X]) * num_xqueue;
  xstop = MIN(xstart+num_xqueue,norm_len[X]);
  ystart = ((my_node / voxel_section[X]) % voxel_section[Y]) * num_yqueue;
  ystop = MIN(ystart+num_yqueue,norm_len[Y]);
  zstart = (my_node / (voxel_section[X] * voxel_section[Y])) * num_zqueue;
  zstop = MIN(zstart+num_zqueue,norm_len[Z]);

#ifdef SERIAL_PREPROC
  zstart=0;
  zstop = norm_len[Z];
  ystart=0;
  ystop = norm_len[Y];
  xstart=0;
  xstop = norm_len[X];
#endif

  for (outz=zstart; outz<zstop; outz++) {
    for (outy=ystart; outy<ystop; outy++) {
      for (outx=xstart; outx<xstop; outx++) {

	inx = INSET + outx;
	iny = INSET + outy;
	inz = INSET + outz;

	/* Compute voxel gradient assuming gradient operator is 1x3     */
	grd_x = (double)((long)MAP(inz,iny,inx+1) - (long)MAP(inz,iny,inx-1));
	grd_y = (double)((long)MAP(inz,iny+1,inx) - (long)MAP(inz,iny-1,inx));
	grd_z = (double)((long)MAP(inz+1,iny,inx) - (long)MAP(inz-1,iny,inx));

	/* Compute (magnitude*grd_divisor)**2 of gradient               */
	/* Reduce (magnitude*grd_divisor)**2 to magnitude*grd_divisor   */
	/* Reduce magnitude*grd_divisor to magnitude                    */
	magnitude = grd_x*grd_x+grd_y*grd_y+grd_z*grd_z;

	local_norm_address = NORM_ADDRESS(outz,outy,outx,X);
	if (magnitude > SMALL) {
	  inv_mag_shift = norm_lshift/sqrt(magnitude);
	  if (grd_x*inv_mag_shift > 0.0) xnorm = 1;
	  else xnorm = 0;
	  ynorm = (long)(grd_y*inv_mag_shift);
	  znorm = (long)(grd_z*inv_mag_shift);
	  *local_norm_address = TADDR(znorm,ynorm,xnorm);
	}
	else {
	  *local_norm_address = TADDR(norm0,2,1);
	}
      }
    }
  }

#ifndef SERIAL_PREPROC
  {
#line 183
	unsigned long	Error, Cycle;
#line 183
	long		Cancel, Temp;
#line 183

#line 183
	Error = pthread_mutex_lock(&(Global->SlaveBarrier).mutex);
#line 183
	if (Error != 0) {
#line 183
		printf("Error while trying to get lock in barrier.\n");
#line 183
		exit(-1);
#line 183
	}
#line 183

#line 183
	Cycle = (Global->SlaveBarrier).cycle;
#line 183
	if (++(Global->SlaveBarrier).counter != (num_nodes)) {
#line 183
		pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, &Cancel);
#line 183
		while (Cycle == (Global->SlaveBarrier).cycle) {
#line 183
			Error = pthread_cond_wait(&(Global->SlaveBarrier).cv, &(Global->SlaveBarrier).mutex);
#line 183
			if (Error != 0) {
#line 183
				break;
#line 183
			}
#line 183
		}
#line 183
		pthread_setcancelstate(Cancel, &Temp);
#line 183
	} else {
#line 183
		(Global->SlaveBarrier).cycle = !(Global->SlaveBarrier).cycle;
#line 183
		(Global->SlaveBarrier).counter = 0;
#line 183
		Error = pthread_cond_broadcast(&(Global->SlaveBarrier).cv);
#line 183
	}
#line 183
	pthread_mutex_unlock(&(Global->SlaveBarrier).mutex);
#line 183
};
#endif
  print_timing_stats();
}


void Load_Normal(filename)
     char filename[];
{
  char local_filename[FILENAME_STRING_SIZE];
  int fd;

  strcpy(local_filename,filename);
  strcat(local_filename,".norm");
  fd = Open_File(local_filename);

  Read_Shorts(fd,(unsigned char *)&norm_version, (long)sizeof(norm_version));
  if (norm_version != NORM_CUR_VERSION)
    Error("    Can't load version %d file\n",norm_version);

  Read_Shorts(fd,(unsigned char *)norm_len,(long)sizeof(map_len));

  Read_Longs(fd,(unsigned char *)&norm_length,(long)sizeof(norm_length));

  Allocate_Normal(&norm_address,norm_length);

  printf("    Loading normal map from .norm file...\n");
  Read_Shorts(fd,(unsigned char *)norm_address,(long)(norm_length*sizeof(NORMAL)));
  fflush(stdout);
  Close_File(fd);
}


void Store_Normal(filename)
char filename[];
{
  char local_filename[FILENAME_STRING_SIZE];
  int fd;

  strcpy(local_filename,filename);
  strcat(local_filename,".norm");
  fd = Create_File(local_filename);

  norm_version = NORM_CUR_VERSION;
  strcpy(local_filename,filename);
  strcat(local_filename,".norm");
  fd = Create_File(local_filename);
  Write_Shorts(fd,(unsigned char *)&norm_version,(long)sizeof(norm_version));

  Write_Shorts(fd,(unsigned char *)norm_len,(long)sizeof(norm_len));
  Write_Longs(fd,(unsigned char *)&norm_length,(long)sizeof(norm_length));

  printf("    Storing normal map into .norm file...\n");
  Write_Shorts(fd,(unsigned char *)norm_address,(long)(norm_length*sizeof(NORMAL)));
  Close_File(fd);
}


void Deallocate_Normal(address)
NORMAL **address;
{
  printf("    Deallocating normal map...\n");

/*  free(*address);;  */

  *address = NULL;
}
