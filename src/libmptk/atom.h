/******************************************************************************/
/*                                                                            */
/*                                 atom.h                                     */
/*                                                                            */
/*                        Matching Pursuit Library                            */
/*                                                                            */
/* Rémi Gribonval                                                             */
/* Sacha Krstulovic                                           Mon Feb 21 2005 */
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

/*************************************/
/*                                   */
/* DEFINITION OF THE BASE ATOM CLASS */
/*                                   */
/*************************************/
/*
 * SVN log:
 *
 * $Author: broy $
 * $Date: 2007-07-13 16:21:09 +0200 (Fri, 13 Jul 2007) $
 * $Revision: 1109 $
 *
 */


#ifndef __atom_h_
#define __atom_h_


#include <stdio.h>
#include <utility>
#include <mp_pthreads_barrier.h>

/***********************/
/* CONSTANTS           */
/***********************/

/* some type of properties for atom->satisfies() */
#define MP_LEN_PROP                    0 /** Length in samples \sa MP_Atom_c::satisfies()*/
#define MP_POS_PROP                    1 /** Position in samples \sa MP_Atom_c::satisfies()*/
#define MP_FREQ_PROP                   2 /** Normalized frequency, between 0 and 0.5 \sa MP_Atom_c::satisfies() */
#define MP_AMP_PROP                    3 /** Amplitude \sa MP_Atom_c::satisfies()*/
#define MP_PHASE_PROP                  4 /** Phase in radians \sa MP_Atom_c::satisfies()*/
#define MP_CHIRP_PROP                  5 /** Chirp factor \sa MP_Atom_c::satisfies() */
#define MP_TABLE_IDX_PROP              6 /** Index of the wave table in the wave server \sa MP_Atom_c::satisfies()*/
#define MP_ANYWAVE_TABLE_PROP          7 /** Pointer to the waveTable containing the waveform \sa MP_Atom_c::satisfies() */
#define MP_ANYWAVE_IDX_PROP            8 /** Index of the wave in the wave table \sa MP_Atom_c::satisfies() */
#define MP_REAL_PART_PROP              9 /** Part of the amplitude due to the real waveform \sa MP_Anywave_Hilbert_Atom_c::satisfies()*/
#define MP_HILBERT_PART_PROP          10 /** Part of the amplitude due to the hilbert waveform \sa MP_Anywave_Hilbert_Atom_c::satisfies()*/
#define MP_REAL_TABLE_IDX_PROP        11 /** Index of the wave table in the wave server \sa MP_Anywave_Hilbert_Atom_c::satisfies()*/
#define MP_ANYWAVE_REAL_TABLE_PROP    12 /** Pointer to the waveTable containing the waveform \sa MP_Anywave_Hilbert_Atom_c::satisfies() */
#define MP_HILBERT_TABLE_IDX_PROP     13 /** Index of the wave table in the wave server \sa MP_Anywave_Hilbert_Atom_c::satisfies()*/
#define MP_ANYWAVE_HILBERT_TABLE_PROP 14 /** Pointer to the waveTable containing the waveform \sa MP_Anywave_Hilbert_Atom_c::satisfies() */

#define MP_NUM_PROPS                  15

/* some test types for atom->satisfies() */
#define MP_SUPER 1 /** x >  val \todo document this \sa MP_Atom_c::satisfies() */
#define MP_SUPEQ 2 /** x >= val \todo document this \sa MP_Atom_c::satisfies() */
#define MP_EQ    3 /** x == val \todo document this \sa MP_Atom_c::satisfies() */
#define MP_INFEQ 4 /** x <= val \todo document this \sa MP_Atom_c::satisfies() */
#define MP_INFER 5 /** x <  val \todo document this \sa MP_Atom_c::satisfies() */
#define MP_HAS   6 /** \todo document this \sa MP_Atom_c::satisfies() */


/***********************/
/* ATOM CLASS          */
/***********************/
/**
 * \brief Atoms store time/frequency/scale/... and amplitude information
 * gathered about a signal at one step of Matching Pursuit.
 *
 * An atom is basically a storage space generated by a MP_Block_c with the method
 * MP_Block_c::create_atom() and stored in a book MP_Book_c with MP_Book_c::append().
 */
class MP_Atom_c {


  /********/
  /* DATA */
  /********/

public:
  /** \brief The number of channels of the atom */
  MP_Chan_t numChans;
  /** \brief The time support of the atom on each channel 
   * 
   * Examples : 
   * - support[1].pos is the index of the first sample 
   * in the support on the second channel
   * - support[0].len is the size of the support on the
   * first channel
   */
  MP_Support_t *support;
  /** \brief The sum of the support lengths over each channel.
   *
   * It is essentially used for efficiency to determine the size of the
   * output buffer in build_waveform()
   */
  unsigned long int totalChanLen;
  /** \brief The amplitude of the atom on each channel. 
   *
   * Example : the amplitude on the first channel is amp[0] */
  MP_Real_t *amp;
  /** \brief The maximum reach of the atom, in number of samples
   *
   * It is essentially used for efficiency
   */
  unsigned long int numSamples;

  /***********/
  /* METHODS */
  /***********/

  /***************************/
  /* CONSTRUCTORS/DESTRUCTOR */
  /***************************/

protected:

  /** \brief a constructor which initializes everything to zero or NULL */
  MP_Atom_c( void );

  /** \brief File reader */
  virtual int read( FILE *fid, const char mode );

public:

  /* Destructor */
  virtual ~MP_Atom_c();

  /** \brief Internal allocations of the local vectors */
   int alloc_atom_param( const MP_Chan_t setNumChans );
  /***************************/
  /* OUTPUT METHOD           */
  /***************************/
  
  /** \brief Write the atom to a stream in binary format
   * \param  fid A writable stream
   * \param  mode The writing mode (MP_TEXT or MP_BINARY)
   * \return The number of items written to the stream 
   *
   * \remark in MP_TEXT mode, NO enclosing XML tag <atom type="*"> ... </atom> is written
   * \sa write_atom()
   */
  virtual int write( FILE *fid, const char mode );


  /***************************/
  /* OTHER METHODS           */
  /***************************/

public :

  /** \brief Get the type of the atom as a string
   *
   * \return the type as a string */
  virtual char* type_name( void );

  /** \brief Substract /add the atom's multichannel waveform from / to a multichannel signal
   * \param sigSub signal from which the atom waveform is to be removed
   * \param sigAdd signal to which the atom waveform is to be added
   *
   * \remark Passing sigSub == NULL or sigAdd == NULL skips the corresponding substraction / addition.
   */
  void substract_add( MP_Signal_c *sigSub, MP_Signal_c *sigAdd );


  /** \brief Substract/add the atom's monochannel waveform from / to a multichannel signal
   *  with various amplitudes
   *
   * \param amp an array of the amplitudes to be used for each channel
   * \param numAmps the number of elements in the amp array (used for testing purposes)
   * \param sigSub signal from which the atom waveform is to be removed
   * \param sigAdd signal to which the atom waveform is to be added
   *
   * \remark Passing sigSub == NULL or sigAdd == NULL skips the corresponding substraction / addition.
   */
  void substract_add_var_amp( MP_Real_t *amp, MP_Chan_t numAmps, MP_Signal_c *sigSub, MP_Signal_c *sigAdd );


  /*****************************************************/
  /* VIRTUAL NULL METHODS, MANDATORY IN THE SUBCLASSES */
  /*****************************************************/

public :

  /** \brief Print human readable information about the atom to a stream
   * \param  fid A writable stream
   * \return The number of characters written to the stream */
  virtual int info( FILE *fid ) = 0;

  /** \brief Print human readable information about the atom to the default info handler
   * \return The number of characters written to the stream */
  virtual int info() = 0;

  /** \brief Build concatenated waveforms corresponding to each channel of an atom. 
   * \param outBuffer the array of size \b totalChanLen which is filled with the  concatenated 
   * waveforms of all channels. */ 
  virtual void build_waveform( MP_Real_t *outBuffer ) = 0;

  /** \brief Adds a pseudo Wigner-Ville of the atom to a time-frequency map 
   * \param tfmap the time-frequency map to which the atom distribution will be plotted
   * \param tfmapType an indicator of what to put in the tfmap, to be chosen among
   * MP_TFMAP_SUPPORTS or MP_TFMAP_PSEUDO_WIGNER (see tfmap.h for more).
   * \return one if the atom printed something into the map, zero otherwise
   */
  virtual int add_to_tfmap( MP_TF_Map_c *tfmap, const char tfmapType ) = 0;

  virtual MP_Real_t dist_to_tfpoint( MP_Real_t time, MP_Real_t freq , MP_Chan_t chanIdx );

  /** \brief Tests if a field of an atom satisfies a property on a given channel 
   * \param field the type of the property (ex: MP_AMP_PROP, MP_FREQ_PROP, ...)
   * \param test the type of test made on the property
   * \param val the value used for comparison in the test
   * \param chanIdx the index of the channel of the field which must be tested
   * \remark the test can be one of the following:
   * - MP_SUPER  test if field >  val 
   * - MP_SUPEQ  test if field >= val
   * - MP_EQ     test if field == val
   * - MP_INFEQ  test if field <= val
   * - MP_INFER  test if field <  val
   * - MP_HAS    test if the atom has the desired field
   * \return MP_TRUE if the test is succesful, MP_FALSE otherwise
   * \sa has_field() get_field()
   */
  int satisfies( int field, int test, MP_Real_t val, MP_Chan_t chanIdx );
  /** \brief Tests if a field of an atom satisfies a property on ALL channels
   * \sa satisfies
   */
  int satisfies( int field, int test, MP_Real_t val );

  /** \brief Test if the atom has a given field 
   * \param field the field type
   * \return MP_TRUE if the atom has the field, MP_FALSE otherwise */
  virtual int      has_field( int field );
  
  /** \brief Gets a field of a channel of an atom 
   * \param field the field type
   * \param chanIdx the desired channel
   * \return the desired field value, zero if the atom has no such field */
  virtual MP_Real_t get_field( int field , MP_Chan_t chanIdx );
  
  protected :
  
 /**  \brief do the add part of subsctract_add 
  * \param sigAdd signal to which the atom waveform is to be added
  * \param pos : the position in the signal to add to
  * \param len :  the lenght to add to
  * \param chanIdx : channel to work on
  * \param atomIn : waveform to add
  */
  std::pair<double,double> add ( MP_Signal_c *sigAdd, 
  								 unsigned long int pos, 
  								 unsigned long int len,
							     MP_Chan_t chanIdx,
							     MP_Real_t * atomIn);


/***********************/
/* PARALLEL_ADD CLASS  */
/***********************/
/**
 * \brief The Parallel_add class allows to stock data for thread parrallel computation
 */
 
  class Parallel_Add
  {
  public:	
 	/* \brief signal to which the atom waveform is to be added */
 	MP_Signal_c *sigAdd ;
  	
  	/* \brief the position in the signal to add to */
  	unsigned long int pos ; 
  	
  	/* \brief the lenght to add to*/
  	unsigned long int len ;
    
    /* \brief channel to work on */
    MP_Chan_t chanIdx ;
    
    /* \brief  waveform to add */
    MP_Real_t * atomIn ;	
    
    /* \brief Self pointer to pass object context to the threads */
    MP_Atom_c* myAtom;
    
    /* \brief boolean to determine if the thread should exit */ 
    bool exit ; 
  };   

/***********************/
/* ADD_WORKER CLASS    */
/***********************/
/**
 * \brief The Add_Worker class allows to create a worker (ie a threads and its context)
 * inside of the atom class to perform the substract add between signal and atom
 */
 
  class Add_Worker {
  	public:
  	
  		/* \brief Public constructor to create the structure that will add atoms to a signal*/
  		Add_Worker() ;
  		
  		/* \brief request that an addition be performed between an atom and a signal
  		 * \param self : the atom to perform
  		 * \param sigAdd: the signal to perform
  		 * 
  		 */
  		void run_add(MP_Atom_c * self,
  				    MP_Signal_c *sigAdd, 
  				    unsigned long int pos, 
  				    unsigned long int len,
				    MP_Chan_t chanIdx,
				    MP_Real_t * atomIn) ;
		
		/* \brief wait for the addition to be completed and get the resulting energies
		 */					     
 		std::pair<double,double> wait() ;

	protected:
        /* \brief the function to pass to pthread_create 
         */
    	static void* run(void* a) ;

		/* \brief the function, in object context a thread will be executing
		 * Exiting this function will end the thread
		 */
	   void run_add_parallel () ;

        /* \brief barrier to synchronize the add workers*/
    	MP_Barrier_c **bar ;
    	
    	/*Store results*/
		std::pair<double,double> result ;
		
		/*Description of the task the ParrallelAdd must perform*/
		Parallel_Add * task ;				     	
  };
  
protected:

/** \brief Pointer on a Add_worker Instance */
  static Add_Worker * myAddWorker ;
public:

/** \brief Method to get an Add_Worker */
  static Add_Worker * get_add_worker() ;
};


#endif /* __atom_h_ */
