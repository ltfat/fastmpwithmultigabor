/******************************************************************************/
/*                                                                            */
/*                          mclt_abstract_block.h                             */
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

/*******************************************************************/
/*                                                		   */
/* DEFINITION OF THE MCLT_ABSTRACT BLOCK CLASS            	   */
/* ABSTRACT CLASS FOR THE MCLT/MDCT/MDST TIME-FREQUENCY TRANSFORMS */
/*                                                		   */
/*******************************************************************/


#ifndef __mclt_abstract_block_plugin_h_
#define __mclt_abstract_block_plugin_h_



/********************************/
/* MCLT_ABSTRACT BLOCK CLASS    */
/********************************/

/** \brief Abstract class for the MCLT/MDCT/MDST blocks class
 *
 */
class MP_Mclt_Abstract_Block_Plugin_c:public MP_Block_c {

  /********/
  /* DATA */
  /********/

public:
  /** \brief FFT interface, which includes the window with which the signal is multiplied */
  MP_FFT_Interface_c *fft;

  /* The FFT size */
  unsigned long int fftSize;

  /* The number of frequencies for the mclt/mdct/mdst: fftSize/2 */
  unsigned long int numFreqs;

  /** \brief A (fft->numFreqs x s->numChans) array which holds the frame-wise FFT results
      across channels */
  MP_Real_t *mag;

  /* A couple of buffers of size numFreqs to perform complex fft computations
     in create_atom. */
  MP_Real_t *fftRe;
  MP_Real_t *fftIm;

  /* A couple of buffers of size numFreqs for the output of the mclt transform */
  MP_Real_t *mcltOutRe;
  MP_Real_t *mcltOutIm;

  /* A couple of buffers to perform pre-modulation on the input frame */
  MP_Real_t *preModRe;
  MP_Real_t *preModIm;

  /* A couple of buffers to perform post-modulation on the fft output*/
  MP_Real_t *postModRe;
  MP_Real_t *postModIm;

  /***********/
  /* METHODS */
  /***********/

  /***************************/
  /* CONSTRUCTORS/DESTRUCTOR */
  /***************************/

public:

  /** \brief an initializer for the parameters which ARE related to the signal
   *  \param filterLen the length of the signal window, in number of samples
   *  \param filterShift the window shift, in number of samples
   *  \param fftSize the size of the FFT, including zero padding
   *  \param windowType the window type (see the doc of libdsp_windows.h)
   *  \param windowOption the optional window parameter.
   *  \param filterLen the length of the signal window, in number of samples
   *  \param windowType the window type (see the doc of libdsp_windows.h)
   *  \param windowOption the optional window parameter.
   *  \param blockOffset the block offset
   * 
   *  */
  virtual int plug_signal( MP_Signal_c *setSignal );

protected:
  /** \brief an initializer for the parameters which ARE NOT related to the signal */
  virtual int init_parameters( const unsigned long int setFilterLen,
			       const unsigned long int setFilterShift,
			       const unsigned long int setFftSize,
			       const unsigned char setWindowType,
			       const double setWindowOption,
			       const unsigned long int setBlockOffset );

  /** \brief nullification of the signal-related parameters */
  virtual void nullify_signal( void );

  /** \brief a constructor which initializes everything to zero or NULL */
  MP_Mclt_Abstract_Block_Plugin_c( void );

public:

  /* Destructor */
  virtual ~MP_Mclt_Abstract_Block_Plugin_c();


  /***************************/
  /* OTHER METHODS           */
  /***************************/

public:

  /** \brief Check the validity of the parameters: window size and fft size */
  int check_fftsize( const unsigned long int setFilterLen, const unsigned long int setFftSize );

  /** \brief Initialize the MCLT transform: allocate and fill the buffers to perform pre- and post-modulation */
  virtual void init_transform( void );

  /** \brief Compute the MCLT transform */
  virtual void compute_transform(MP_Real_t *in);
};
 

#endif /* __mclt_abstract_block_h_ */
