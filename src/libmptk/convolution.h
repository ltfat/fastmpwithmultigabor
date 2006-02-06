/******************************************************************************/
/*                                                                            */
/*                              convolution.h                                 */
/*                                                                            */
/*                        Matching Pursuit Library                            */
/*                                                                            */
/* Rémi Gribonval                                                             */
/* Sacha Krstulovic                                                           */
/* Sylvain Lesage                                             Wed Dec 07 2005 */
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

/***************************************/
/*                                     */
/* DEFINITION OF THE CONVOLUTION CLASS */
/*                                     */
/***************************************/
/*
 * SVN log:
 *
 * $Author: sacha $
 * $Date$
 * $Revision$
 *
 */

/* This module allow to implement the computation of the inner
   products for anywave atoms in different ways, and to interface with
   the FFTW package */

#ifndef __convolution_h_
#define __convolution_h_

/** \brief computation of the inner products with anywave atoms by
 * direct method
 */
const unsigned short int MP_ANYWAVE_COMPUTE_DIRECT = 0;

/** \brief computation of the inner products with anywave atoms by
 * fast convolution method with overlap-add
 */
const unsigned short int MP_ANYWAVE_COMPUTE_FFT = 1;

/**  \brief number of methods for computing the inner products
 */
const unsigned short int MP_ANYWAVE_COMPUTE_NUM_METHODS = 2;

#include <fftw3.h>
#include <time.h>

/* Inheritance graph: all the classes inherit from
   the generic class (MP_Convolution_c):

   MP_Convolution_c |-> MP_Convolution_Fastest_c
                    |-> MP_Convolution_Direct_c
                    |-> MP_Convolution_FFT_c
*/


/*-------------------*/
/** \brief A generic class for performing inner products between a
 * signal and an anywave filter.
 * 
 * If the signal is longer than the anywave atom, then all the inner
 * products between a frame of signal and the filter are
 * performed. The frames have the same size than the filter and begin
 * every \a filterShift samples from the start
 *
 * \todo Use fft_interface instead of calling directly a specific
 * implementation of the FFT : FFTW
 * 
*/
/*-------------------*/

class MP_Convolution_c {

  /********/
  /* DATA */
  /********/

 protected:

  /** \brief length between two successive frames of signal
   **/
  unsigned long int filterShift;

  /** \brief the anywave table containing all the filters (of same
      length) that will be used to perform inner products with
      a signal
  */
  MP_Anywave_Table_c* anywaveTable;

  /***********/
  /* METHODS */
  /***********/

  /***************************/
  /* CONSTRUCTORS/DESTRUCTOR */
  /***************************/

 protected:

  /** \brief A generic constructor used only by non-virtual children
   * classes
   *
   * \param anywaveTable the anywave table containing the filters
   * \param filterShift length between two successive frames of signal   
   */
  MP_Convolution_c( MP_Anywave_Table_c* anywaveTable,
		   const unsigned long int filterShift );

 public:
  
  /** \brief Destructor */
  virtual ~MP_Convolution_c();


  /***************************/
  /* OTHER METHODS           */
  /***************************/

 public:
  /** \brief computes the inner products between frames of \a input
   * and the channel \a chanIdx of all the filters of the \a
   * anywaveTable.
   *
   * The first frame begins at \a input, and the following ones are
   * all shifted by \a filterShift samples. There are \f$ numFrames =
   * \lfloor \frac{inputLen - filterLen}{filterShift}\rfloor +1 \f$
   * frames. The inner product between the frame \f$ frameIdx \f$ and
   * the filter anywaveTable->wave[filterIdx][chanIdx] is saved in \a
   * output[frameIdx + filterIdx*numFrames]
   *
   * \param input the signal one wants to compute the inner product
   * with the filters
   *
   * \param inputLen the length of the input signal
   *
   * \param chanIdx the index of the channel of the filters
   *
   * \param output the address of the array of size \f$ numFrames *
   * anywaveTable->numFilters \f$ of the inner products between the
   * frames of \a input and the channel \a chanIdx of all the
   * filters. It must have been allocated before calling this
   * function.
   *
   * \remark inputLen shall not be lower than anywaveTable->filterLen
   **/
  virtual void compute_IP( MP_Sample_t* input, unsigned long int inputLen, unsigned short int chanIdx, double** output ) = 0;

  virtual void compute_max_IP( MP_Signal_c* s, unsigned long int inputLen, unsigned long int fromSample, MP_Real_t* ampOutput, unsigned long int* idxOutput ) = 0;


};

/*********************************/
/*                               */
/* FASTEST IMPLEMENTATION        */
/*                               */
/*********************************/

/** \brief This class automatically chooses the fastest method among
 * the two following implementations : direct computation or fast
 * convolution based on FFT
 */
class MP_Convolution_Fastest_c:public MP_Convolution_c {

  /********/
  /* DATA */
  /********/

 private:

  /** \brief array containing a pointer to an instance of every method
   * for computing the inner products.
   *
   * this class automatically chooses the method to use depending on
   * the size of the signal and the size of the filter.
   */
  MP_Convolution_c* methods[MP_ANYWAVE_COMPUTE_NUM_METHODS];  
  
  /** \brief signal length at which the algorithm switches between
   * direct computation and FFT computation
   *
   * if \a inputLen > \a methodSwitchLimit, then the convolution is
   * done with the FFT based method. Else, it is done with direct
   * computation.
   */
  unsigned long int methodSwitchLimit;

  /***********/
  /* METHODS */
  /***********/

  /***************************/
  /* CONSTRUCTORS/DESTRUCTOR */
  /***************************/
 public:
  /** \brief The constructor
   *
   * calls the function initialize to perform the initializations
   *
   * \param waveTable the wavetable containing the filters (of the same size)
   * \param filterShift length between two successive frames of signal
   */
  MP_Convolution_Fastest_c( MP_Anywave_Table_c* anywaveTable,
			   const unsigned long int filterShift );

  /** \brief The constructor where only one method is used
   *
   * calls the function initialize to perform the initializations
   *
   * \param waveTable the wavetable containing the filters (of the same size)
   * \param filterShift length between two successive frames of signal
   * \param computationMethod index of the only method to use
   */
  MP_Convolution_Fastest_c( MP_Anywave_Table_c* anywaveTable,
			   const unsigned long int filterShift,
			   const unsigned short int computationMethod );

  /** \brief Destructor
   */
  ~MP_Convolution_Fastest_c();

  /** \brief Initializes the member \a methods and \a methodSwitchLimit
   *
   * Creates one instance of each method, and puts them in \a
   * methods. Tests them to find the limit \a methodSwitchLimit where
   * the methods switch.
   */
  void initialize(void);

  /** \brief Initializes the members \a methods and \a methodSwitchLimit
   *
   * Creates one instance of each method, and puts them in \a
   * methods. Sets \a methodSwitchLimit to anywaveTable->filterLen
   */
  void initialize( unsigned short int computationMethod );

  /** \brief Release all that is allocated in initialize()
   */
  void release(void);

  /***************************/
  /* OTHER METHODS           */
  /***************************/

 private:

  /** \brief finds the fastest method for a signal of size \a testInputLen
   *
   * If \f$ testInputLen > methodSwitchLimit \f$, then return
   * MP_ANYWAVE_COMPUTE_FFT. Else return
   * MP_ANYWAVE_COMPUTE_DIRECT
   *
   * \param testInputLen the input length to test
   *
   * \return the fastest method for this input length
   */
  unsigned short int find_fastest_method( unsigned long int testInputLen );

 public:
   

  /** \brief computes the inner products by the fastest method
   *
   * computes the inner products between frames of \a input and the
   * channel \a chanIdx of all the filters of the \a anywaveTable.
   *
   * The first frame begins at \a input, and the following ones are
   * all shifted by \a filterShift samples. There are \f$ numFrames =
   * \lfloor \frac{inputLen - filterLen}{filterShift}\rfloor +1 \f$
   * frames. The inner product between the frame \f$ frameIdx \f$ and
   * the filter anywaveTable->wave[filterIdx][chanIdx] is saved in \a
   * output[frameIdx + filterIdx*numFrames]
   *
   * \param input the signal one wants to compute the inner product
   * with the filters
   *
   * \param inputLen the length of the input signal
   *
   * \param chanIdx the index of the channel of the filters
   *
   * \param output the address of the array of size \f$ numFrames *
   * anywaveTable->numFilters \f$ of the inner products between the
   * frames of \a input and the channel \a chanIdx of all the
   * filters. It must have been allocated before calling this
   * function.
   *
   * \remark inputLen shall not be lower than anywaveTable->filterLen
   **/
  virtual void compute_IP( MP_Sample_t* input, unsigned long int inputLen, unsigned short int chanIdx, double** output );

  /** \brief computes the inner product between ONE frame of signal
   * and ONE filter by the fastest method : the direct one
   *
   * The frame begins at \a input and its size is \a
   * anywaveTable->filterLen. The inner product between the frame and
   * the filter anywaveTable->wave[filterIdx][chanIdx] is returned.
   *
   * \param input the signal one wants to compute the inner product
   * with the filter
   *
   * \param filterIdx the index of the filter 
   *
   * \param chanIdx the channel index of the filter
   *
   * \return the inner product
   **/
  virtual double compute_IP( MP_Sample_t* input, unsigned long int filterIdx, unsigned short int chanIdx );

  virtual void compute_max_IP( MP_Signal_c* s, unsigned long int inputLen, unsigned long int fromSample, MP_Real_t* ampOutput, unsigned long int* idxOutput );


};

/*************************************/
/*                                   */
/* DIRECT COMPUTATION IMPLEMENTATION */
/*                                   */
/*************************************/

/** \brief The implementation of MP_Convolution_c computing the inner
 * products directly
 */
class MP_Convolution_Direct_c:public MP_Convolution_c {

  /********/
  /* DATA */
  /********/

 private:

  /***********/
  /* METHODS */
  /***********/

  /***************************/
  /* CONSTRUCTORS/DESTRUCTOR */
  /***************************/

 public:
  /** \brief The constructor
   *
   * \param waveTable the wavetable containing the filters (of the same size)
   * \param filterShift length between two successive frames of signal
   */
  MP_Convolution_Direct_c( MP_Anywave_Table_c* anywaveTable,
			  const unsigned long int filterShift );
  
  /** \brief Destructor
   */
  ~MP_Convolution_Direct_c();

  /***************************/
  /* OTHER METHODS           */
  /***************************/

 public:

  /** \brief computes the inner products directly
   *
   * computes the inner products between frames of \a input and the
   * channel \a chanIdx of all the filters of the \a anywaveTable.
   *
   * The first frame begins at \a input, and the following ones are
   * all shifted by \a filterShift samples. There are \f$ numFrames =
   * \lfloor \frac{inputLen - filterLen}{filterShift}\rfloor +1 \f$
   * frames. The inner product between the frame \f$ frameIdx \f$ and
   * the filter anywaveTable->wave[filterIdx][chanIdx] is saved in \a
   * output[frameIdx + filterIdx*numFrames]
   *
   * The inner products are computed by the following formula: \f[
   * output[frameIdx + filterIdx*numFrames] = \sum_{j=0}^{filterLen-1}
   * input[j + frameIdx.filterShift] . wave[filterIdx][chanIdx][j], \f] for
   * \f$ i \in [0,numFrames]\f$ .
   *
   * \param input the signal one wants to compute the inner product
   * with the filters
   *
   * \param inputLen the length of the input signal
   *
   * \param chanIdx the index of the channel of the filters
   *
   * \param output the address of the array of size \f$ numFrames *
   * anywaveTable->numFilters \f$ of the inner products between the
   * frames of \a input and the channel \a chanIdx of all the
   * filters. It must have been allocated before calling this
   * function.
   *
   * \remark inputLen shall not be lower than anywaveTable->filterLen
   **/
  virtual void compute_IP( MP_Sample_t* input, unsigned long int inputLen, unsigned short int chanIdx, double** output );

  /** \brief computes the inner product between ONE frame of signal
   * and ONE filter by the direct method
   *
   * The frame begins at \a input and its size is \a
   * anywaveTable->filterLen. The inner product between the frame and
   * the filter anywaveTable->wave[filterIdx][chanIdx] is returned.
   *
   * The inner product is computed by the following formula: \f[
   * output = \sum_{j=0}^{filterLen-1} input[j]
   * . anywaveTable->wave[filterIdx][chanIdx][j]. \f]
   *
   * \param input the signal one wants to compute the inner product
   * with the filter
   *
   * \param filterIdx the index of the filter 
   *
   * \param chanIdx the channel index of the filter
   *
   * \return the inner product
   **/
  inline virtual double compute_IP( MP_Sample_t* input, unsigned long int filterIdx, unsigned short int chanIdx );

  virtual void compute_max_IP( MP_Signal_c* s, unsigned long int inputLen, unsigned long int fromSample, MP_Real_t* ampOutput, unsigned long int* idxOutput );

};

/***********************************/
/*                                 */
/* FAST CONVOLUTION IMPLEMENTATION */
/*                                 */
/***********************************/

/** \brief The implementation of MP_Convolution_c using
 * fast convolution with overlap-add
 * - Decomposition of the convolution in modules
 * - Add of the compute_max_ip method
 */
class MP_Convolution_FFT_c:public MP_Convolution_c {

  /********/
  /* DATA */
  /********/

 private:

  /* FFTW parameters */

  /** \brief length of the real part of the spectrum
   *
   * It is also the size of the complex signals (fftw_complex)
   *
   * \f[ fftRealSize = \frac{fftCplxSize}{2} + 1  \f]
   */
  unsigned long int fftRealSize;
  /** \brief length of the whole spectrum 
   *
   * It is also the size of the padded real signals (double)
   *
   * \f[ fftCplxSize = 2 * anywaveTable->filterLen \f]
   */
  unsigned long int fftCplxSize;

  /** \brief manage the FFT of \a signalBuffer and its saving to \a
   * signalFftBuffer
   */
  fftw_plan fftPlan;

  /** \brief manage the inverse FFT of \a outputFftBuffer and its saving to
   * \a outputBuffer
   */
  fftw_plan ifftPlan;

  /** \brief input of the \a fftPlan of length fftCplxSize */
  double *signalBuffer;
  /** \brief output of the \a fftPlan of length fftRealSize (because using real-valued signals)*/
  fftw_complex *signalFftBuffer;

  /** \brief input of the \a ifftPlan of length fftRealSize (because using real-valued signals) */
  fftw_complex *outputFftBuffer;
  /** \brief output of the \a ifftPlan of length fftCplxSize */
  double *outputBuffer;

  /** \brief output of the \a ifftPlan of length fftCplxSize */
  double *circularBuffer;

  /** \brief storage of the FFT of the filters of length fftRealSize *
   * anywaveTable->numFilters * anywaveTable->numChans
   */
  fftw_complex* filterFftStorage;

  /** \brief tab for accessing the FFT of the filters in \a
   * filterFftStorage. For example
   * filterFftBuffer[filterIdx][chanIdx][j] corresponds to
   * filterFftStorage[j + filterIdx*fftRealSize +
   * chanIdx*numFilters]
   */
  fftw_complex*** filterFftBuffer;

  /** brief array of size numFilters of pointers to the output of the
   * first slice of the method circular_convolution
   *
   */
  double** outputBufferAdd;

  /** brief array of size numFilters of pointers to the output of the
   * second slice of the method circular_convolution
   *
   */
  double** outputBufferNew;

    
  /***********/
  /* METHODS */
  /***********/

  /***************************/
  /* CONSTRUCTORS/DESTRUCTOR */
  /***************************/
 public:
  /** \brief The constructor
   *
   * Initialize the members to default value (calls release).
   *
   * \param waveTable the wavetable containing the filters (of the same size)
   * \param filterShift length between two successive frames of signal
   */
  MP_Convolution_FFT_c( MP_Anywave_Table_c* anywaveTable,
			    const unsigned long int filterShift );

  /** \brief Destructor
   */
  ~MP_Convolution_FFT_c();

 private:

  /** \brief Initialize the members
   *
   * initialize :
   * - \a fftCplxSize \f[ fftCplxSize = 2 * anywaveTable->filterLen \f]
   * - \a fftRealSize \f[ fftRealSize = \frac{fftCplxSize}{2} + 1 \f]
   * 
   * allocate (using fftw_malloc)
   * - \a signalFftBuffer  (size \a fftRealSize) (and copies zeros on the second half of the buffer)
   * - \a outputFftBuffer  (size \a fftRealSize)
   * - \a signalBuffer     (size \a fftCplxSize for zero-padding)
   * - \a outputBuffer     (size \a fftCplxSize for zero-padding)
   *
   * allocate (using fftw_malloc) and compute the DFTs to fill in
   * - \a filterFftStorage (size \a fftRealSize * \a anywaveTable->numFilters * \a anywaveTable->numChans)
   * - \a filterFftBuffer  (size \a numFilters)
   *
   * create (using fftw_plan_dft_r2c_1d and fftw_plan_dft_c2r_1d)
   * - \a fftPlan
   * - \a ifftPlan
   */
  void initialize(void);

  /** \brief Release all that is allocated in initialize()
   */
  void release(void);

  /***************************/
  /* OTHER METHODS           */
  /***************************/

 private: 
  /** \brief points to the first sample of the slice #sliceIdx
   *
   * The signal is cut in slices of length 2*filterLen, in order to
   * compute fast convolution via the overlapp-add technique. As the
   * first slice begins with the signal, and as the delay between two
   * successive slices is filterLen, the first sample of the slice
   * #sliceIdx is the sample #sliceIdx*filterLen
   *
   * \param sliceIdx the index of the slice
   *
   * \param inputStart pointer to the first sample of the input
   *
   * \return a pointer on the first sample of the sliceIdx'th slice
   */
  MP_Sample_t* slice( unsigned long int sliceIdx, MP_Sample_t* inputStart );

  /** \brief perform the circular convolution of the slice pointed
   * with slicePtr with the channel chanIdx of all the filters of the
   * anywave table
   *
   * performs the FFT of the slice, then multiplies it with the FFT of
   * each filter, and then perform inverse FFT to fill the output
   * buffer ( sliceBuffer, size : numFilters * 2 * filterLen ) in.
   *
   * \param pSlice a pointer to the start of slice
   *
   * \param pNextSlice a pointer to the first sample after the end of
   * the slice
   *
   * \param chanIdx channel of the filters used in the computation
   *
   * \param outputBufferAdd array of numFilters pointers to the outputs where to add the contribution of the second slice
   */
  void circular_convolution( MP_Sample_t* pSlice, MP_Sample_t* pNextSlice, unsigned short int chanIdx, double** outputBufferAdd, double** outputBufferNew, unsigned long int firstFrameSample, unsigned long int numFramesAdd, unsigned long int numFramesNew );

 public:

  /** \brief computes the inner products using fast convolution with
   * overlap-add
   *
   * computes the inner products between frames of \a input and the
   * channel \a chanIdx of all the filters of the \a anywaveTable.
   * 
   * A loop upon the filters allows to fill \a output in
   * iteratively. For each filter, the following is done.
   *
   * The first frame begins at \a input, and the following ones are
   * all shifted by \a filterShift samples. There are \f$ numFrames =
   * \lfloor \frac{inputLen - filterLen}{filterShift}\rfloor +1 \f$
   * frames. The inner product between the frame \f$ frameIdx \f$ and
   * the filter anywaveTable->wave[filterIdx][chanIdx] (now denoted by
   * \a filter) is saved in \a output[frameIdx + filterIdx*numFrames]
   *
   * The inner products are computed via convolution. This means in
   * more details, that a convolution is computed between the signal
   * and the flipped filter, corresponding to all the inner products
   * between the filter and a frame of the signal. To finish, we then
   * pick the only inner products corresponding to the frames defined
   * by \a filterShift. This means that more inner products than
   * needed are computed.
   *
   * The only selected inner products, for the given channel chanIdx,
   * are saved in a buffer \a output :
   * \f[ (*output)[frameIdx+filterIdx.numFrames] =
   * \sum_{q=0}^{filterLen-1}input[p_{frameIdx}+q].filter[q]\f] , with
   * \f$ p_{frameIdx} = frameIdx.filterShift \f$ and for \f$ frameIdx
   * \in [0,numFrames]\f$ .
   *
   * More largely, denoting \a conv a buffer that would contain the
   * convolution between the input and the filter, \f[ conv[p] =
   * \sum_{q=\max(0,filterLen-1-p)}^{\min(filterLen-1,signalLen+filterLen-2-p)}input[p+q-filterLen+1].filter[q].\f]
   *
   * This convolution is not achieved directly, but going through the
   * Fourier domain and using the overlap-add technique. More
   * precisely, slices of the signal \a input of length
   * \f$filterLen\f$ are copied successively to \a signalBuffer of
   * size \f$2.filterLen\f$, with zero-padding on the last
   * \f$filterLen\f$ samples. The last slice of signal is the one
   * containing the last sample of \a input, i.e. there are \f$\lceil
   * \frac{inputLen}{filterLen} \rceil \f$ slices.
   *
   * \a signalFftBuffer is the discrete Fourier transform (DFT) of \a
   * signalBuffer :
   * \f[ signalFftBuffer[k]=\sum_{m=0}^{2.filterLen-1}signalBuffer[m].\exp
   * (-2\pi \imath \frac{km}{2.filterLen} ).\f] \a
   * filterFftBuffer[filterIdx][chanIdx] is the DFT of \a
   * anywaveTable->wave[filterIdx][chanIdx] (assuming there is
   * zero-padding for \f$ n \ge filterLen \f$ ; note that these DFT are
   * computed at the creation of the MP_convolution_c object) : \f[
   * filterFftBuffer[filterIdx][chanIdx][k]=\sum_{n=0}^{2.filterLen-1}filter[n].\exp
   * (-2\pi \imath \frac{kn}{2.filterLen} ).\f]
   *
   * \a outputFftBuffer is the products between the two DFT of the
   * signal and the current filter \f$outputFftBuffer[k] =
   * signalFftBuffer[k].filterFftBuffer[filterIdx][chanIdx][k] \f$
   *
   * Last, \a outputBuffer is the inverse DFT of \a outputFftBuffer :
   * \f[
   * inverseOut[p]=\frac{1}{2.filterLen}.\sum_{k=0}^{2.filterLen-1}
   * outputFftBuffer[k].\exp (2\pi \imath \frac{kp}{2.filterLen}).\f]
   *
   * Then, theoretically, the slices \a outputBuffer, each shifted by
   * \a filterLen, are summed to give the whole
   * convolution. Practically, we only need the inner products between
   * the filters and the frames of the \a input (all the filterShift
   * samples of \a input). To do so, for each slice \a sliceIdx, one
   * finds the indices of the frames of \ input that are involved in
   * the current convolution: one finds \a minFrameIdx and \a
   * maxFrameIdx, and then one adds, for \a frameIdx between \a
   * minFrameIdx and \a maxFrameIdx : \f[ output[frameIdx +
   * filterIdx*numFrames] += outputBuffer[frameIdx.filterShift -
   * sliceIdx.filterLen].\f]
   *
   * The overlap-add technique allows to perform the convolution using
   * only FFT of size 2.filterLen, instead of signalLen. This leads to
   * a large gain of speed and memory.
   * 
   * For computing efficiency, as the signals and filters are real, we
   * use the function fftw_plan_dft_r2c_1d for DFT that only gives the
   * real part of the spectrum, thus \a signalFftBuffer, \a
   * filterFftBuffer and \a outputFftBuffer have half the size of \a
   * signalBuffer, padded filters and \a outputBuffer. To perform the
   * inverse DFT, we use the function fftw_plan_dft_c2r_1d.
   *
   * \param input the signal one wants to compute the inner product
   * with the filters
   *
   * \param inputLen the length of the input signal
   *
   * \param chanIdx the index of the channel of the filters
   *
   * \param output the address of the array of size \f$ numFrames *
   * anywaveTable->numFilters \f$ of the inner products between the
   * frames of \a input and the channel \a chanIdx of all the
   * filters. It must have been allocated before calling this
   * function.
   *
   * \remark inputLen shall not be lower than anywaveTable->filterLen
   **/
  virtual void compute_IP( MP_Sample_t* input, unsigned long int inputLen, unsigned short int chanIdx, double** output );

  /** \brief finds the max inner product, for every frame, between the
   * input and all the filters, using fast convolution with
   * overlap-add
   *
   * computes the inner products between frames of \a input and all
   * the channels of all the filters of the \a anywaveTable, and then
   * compute the amplitude of the inner products, and select only the
   * maximum one for each frame.
   * 
   * A loop upon the filters allows to fill \a output in
   * iteratively. For each filter, the following is done.
   *
   * The first frame begins at \a input, and the following ones are
   * all shifted by \a filterShift samples. There are \f$ numFrames =
   * \lfloor \frac{inputLen - filterLen}{filterShift}\rfloor +1 \f$
   * frames. The inner products between the frame \f$ frameIdx \f$ and
   * the filters anywaveTable->wave[filterIdx][chanIdx] (now denoted
   * by \a filter) are performed, for every channel of every filter,
   * and then the amplitudes are computed, and the max for the given
   * frame is saved in \a output[frameIdx + filterIdx*numFrames]
   *
   * The inner products are computed via convolution. This means in
   * more details, that a convolution is computed between the signal
   * and the flipped filter, corresponding to all the inner products
   * between the filter and a frame of the signal. To finish, we then
   * pick the only inner products corresponding to the frames defined
   * by \a filterShift. This means that more inner products than
   * needed are computed.
   *
   * The convolution is not achieved directly, but going through the
   * Fourier domain and using the overlap-add technique. More
   * precisely, slices of the signal \a input of length
   * \f$filterLen\f$ are copied successively to \a signalBuffer of
   * size \f$2.filterLen\f$, with zero-padding on the last
   * \f$filterLen\f$ samples. The last slice of signal is the one
   * containing the last sample of \a input, i.e. there are \f$\lceil
   * \frac{inputLen}{filterLen} \rceil \f$ slices.
   *
   * \a signalFftBuffer is the discrete Fourier transform (DFT) of \a
   * signalBuffer :
   * \f[ signalFftBuffer[k]=\sum_{m=0}^{2.filterLen-1}signalBuffer[m].\exp
   * (-2\pi \imath \frac{km}{2.filterLen} ).\f] \a
   * filterFftBuffer[filterIdx][chanIdx] is the DFT of \a
   * anywaveTable->wave[filterIdx][chanIdx] (assuming there is
   * zero-padding for \f$ n \ge filterLen \f$ ; note that these DFT are
   * computed at the creation of the MP_convolution_c object) : \f[
   * filterFftBuffer[filterIdx][chanIdx][k]=\sum_{n=0}^{2.filterLen-1}filter[n].\exp
   * (-2\pi \imath \frac{kn}{2.filterLen} ).\f]
   *
   * \a outputFftBuffer is the products between the two DFT of the
   * signal and the current filter \f$outputFftBuffer[k] =
   * signalFftBuffer[k].filterFftBuffer[filterIdx][chanIdx][k] \f$
   *
   * Last, \a outputBuffer is the inverse DFT of \a outputFftBuffer :
   * \f[
   * inverseOut[p]=\frac{1}{2.filterLen}.\sum_{k=0}^{2.filterLen-1}
   * outputFftBuffer[k].\exp (2\pi \imath \frac{kp}{2.filterLen}).\f]
   *
   * Then, theoretically, the slices \a outputBuffer, each shifted by
   * \a filterLen, are summed to give the whole
   * convolution. Practically, we only need the inner products between
   * the filters and the frames of the \a input (all the filterShift
   * samples of \a input). To do so, for each slice \a sliceIdx, one
   * finds the indices of the frames of \ input that are involved in
   * the current convolution: one finds \a minFrameIdx and \a
   * maxFrameIdx, and then one adds, for \a frameIdx between \a
   * minFrameIdx and \a maxFrameIdx : \f[ output[frameIdx +
   * filterIdx*numFrames] += outputBuffer[frameIdx.filterShift -
   * sliceIdx.filterLen].\f]
   *
   * The overlap-add technique allows to perform the convolution using
   * only FFT of size 2.filterLen, instead of signalLen. This leads to
   * a large gain of speed and memory.
   * 
   * For computing efficiency, as the signals and filters are real, we
   * use the function fftw_plan_dft_r2c_1d for DFT that only gives the
   * real part of the spectrum, thus \a signalFftBuffer, \a
   * filterFftBuffer and \a outputFftBuffer have half the size of \a
   * signalBuffer, padded filters and \a outputBuffer. To perform the
   * inverse DFT, we use the function fftw_plan_dft_c2r_1d.
   *
   * \param s the signal one wants to compute the inner product
   * with the filters
   *
   * \param inputLen the length of the piece of the input signal to process
   *
   * \param fromSample the first sample of the piece of the input signal to process
   *
   * \param ampOutput the address of the array of size \f$ numFrames
   * \f$ of the amplitudes of the max inner products between the
   * frames of \a input and the filters. It must have been allocated
   * before calling this function.
   *
   * \param idxOutput the address of the array of size \f$ numFrames
   * \f$ of the filter index of the max inner products between the
   * frames of \a input and the filters. It must have been allocated
   * before calling this function.
   *
   * \remark inputLen shall not be lower than anywaveTable->filterLen
   **/
  virtual void compute_max_IP( MP_Signal_c* s, unsigned long int inputLen, unsigned long int fromSample, MP_Real_t* ampOutput, unsigned long int* idxOutput );
};


#endif /* __convolution_h_ */
