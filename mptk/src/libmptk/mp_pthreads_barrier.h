/******************************************************************************/
/*                                                                            */
/*                              pthreads-barrier.h                            */
/*                                                                            */
/*                        Matching Pursuit Library                            */
/*                                                                            */
/* Benjamin ROY                                                               */
/*                                            Fri Dec 12 2006                 */
/* -------------------------------------------------------------------------- */
/*                                                                            */
/*  Copyright (C) 2005 IRISA                                                  */
/*                                                                            */
/*  This program is free software; you can redistribute it and/or             */
/*  modify it under the terms of the GNU General Public License               */
/*  as published by the Free Software Foundation; either version 2            */
/*  of the License, or (at your option) any later version.                    */
/*                                                                            */
/*  This program is distributed in the hope that it will be useful,           */
/*  but WITHOUT ANY WARRANTY; without even the implied warranty of            */
/*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             */
/*  GNU General Public License for more details.                              */
/*                                                                            */
/*  You should have received a copy of the GNU General Public License         */
/*  along with this program; if not, write to the Free Software               */
/*  Foundation, Inc., 59 Temple Place - Suite 330,                            */
/*  Boston, MA  02111-1307, USA.                                              */
/*                                                                            */
/******************************************************************************/


/*********************************************/
/*                                           */
/* mp_pthreads_barrier.h: barrier MP_Dict_c  */
/*                                           */
/*********************************************/
#ifndef BARRIER_H_
#define BARRIER_H_

#include <pthread.h>		// has pthread_ routines
#include "mptk.h"
#include "mp_system.h"

/** \brief The MP_Barrier class is used to synchronise the threads during decomposition
 * when MPTK is build using the multithread option.
 * 
 */ 
 
 
class MP_Barrier_c {

    /********/
    /* DATA */
    /********/
    
private:

/** \brief The number of threads participating
 */ 
  int nThreads;	
  		
/** \brief The number of threads that have arrived since last reset
 */   
  int count;
  
/** \brief The pthread_mutex_t used to lock the barrier
 */ 
  pthread_mutex_t barrierLock;
  
/** \brief The pthread_cond_t used to lock/unlock the barrier
 */
  pthread_cond_t mustWait;

public:

/** \brief The public constructor: 
 * \param n : the number of threads that will participate.
 */
 
  MP_Barrier_c(const int n) {
    nThreads = n;
    count = 0;
    pthread_mutex_init(&barrierLock, NULL);
    pthread_cond_init(&mustWait, NULL);

  }

/** \brief The public destructor:
 */
  virtual ~MP_Barrier_c(void) {
    pthread_cond_destroy(&mustWait);
    pthread_mutex_destroy(&barrierLock);

  }

  /* Function all threads should call "at the barrier". */
  void wait(void) {
    /* Get lock on shared variables.*/
    pthread_mutex_lock(&barrierLock);
    if (++count < nThreads) {
      /* not all threads have arrived yet -- wait*/
      pthread_cond_wait(&mustWait, &barrierLock);
    }
    else {
      /* all threads have arrived -- notify waiting threads
        and reset for next time */
 
      pthread_cond_broadcast(&mustWait);
      count = 0;
    }
    pthread_mutex_unlock(&barrierLock);
  }
  
private:

/** \brief The  private copy constructor
 */
  MP_Barrier_c(const MP_Barrier_c & bb);
  
/** \brief The  private assignment operator
 */
  MP_Barrier_c & operator= (const MP_Barrier_c & bb);
  
  
};

#endif // BARRIER_H_
