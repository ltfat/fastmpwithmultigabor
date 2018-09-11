/******************************************************************************/
/*                                                                            */
/*                                general.h                                   */
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

/*****************************/
/*                           */
/* GENERAL PURPOSE FUNCTIONS */
/*                           */
/*****************************/
/*
 * SVN log:
 *
 * $Author: broy $
 * $Date: 2007-03-15 18:00:50 +0100 (Thu, 15 Mar 2007) $
 * $Revision: 1013 $
 *
 */

#ifndef __general_h_
#define __general_h_


/** \brief A wrapper for fread which byte-swaps if the machine is BIG_ENDIAN. */
MPTK_LIB_EXPORT size_t mp_fread( void *buf, size_t size, size_t n, FILE *fid );

/** \brief A wrapper for fwrite which byte-swaps if the machine is BIG_ENDIAN. */
MPTK_LIB_EXPORT size_t mp_fwrite( void *buf, size_t size, size_t n, FILE *fid );


/** \brief A utility that computes how many frames with a given frameLength
 * and frameShift fit inside an interval of a given length. */
#define len2numFrames( len , fLen , fShift )  ( fLen > len ? 0 : ( (len - fLen)/fShift + 1) )

/** \brief A utility that computes which frames intersect a given support */
void support2frame( MP_Support_t support, 
		    unsigned long int fLen ,
		    unsigned long int fShift,
		    unsigned long int *fromFrame,
		    unsigned long int *toFrame );

/** \brief Computes the amplitude and phase of the projection of a real valued signal on
 *  the space spanned by a complex atom and its conjugate transpose.
 *
 * \param re real part of the inner product between the signal and the atom
 * \param im imaginary part of the inner product between the signal and the atom
 * \param re_correl real part of the inner product between the atom and its conjugate 
 * \param im_correl imaginary part of the inner product between the atom and its conjugate 
 * \param amp pointer on the returned amplitude 
 * \param phase pointer on the returned phase, which is between -pi and pi (included)
 *
 * As explained in Section 3.2.3 (Eq. 3.30) of the Ph.D. thesis of Remi 
 * Gribonval, for a normalized atom \f$g\f$ with
 * \f$|\langle g,\overline{g}\rangle|<1\f$ the projection
 * is 
 * \f[
 * \langle \mbox{sig},g \rangle 
 * \Big(
 *   g-\langle g,\overline{g}\rangle \overline{g}
 * \Big)
 * + 
 * \langle \mbox{sig},\overline{g} \rangle 
 * \Big(
 *   \overline{g}-\overline{\langle g,\overline{g}\rangle} g
 * \Big)
 * \f]
 *
 * Denoting \f$(\mbox{re},\mbox{im}) = \langle \mbox{sig},g \rangle\f$
 * and \f$(\mbox{re\_correl},\mbox{im\_correl}) = \langle g,\overline{g}\rangle\f$
 * the projection can be written
 * 
 * \f[
 * \frac{\mbox{amp}}{2} \cdot \Big(
 * \exp \left( i \cdot \mbox{phase}\right) g + 
 * \exp \left(-i \cdot \mbox{phase}\right) \overline{g}
 * \Big)
 * \f]
 * where
 * \f$(\cos \mbox{phase},\sin \mbox{phase})\f$ is proportional to \f$(\mbox{real},\mbox{imag})\f$ 
 * and
 * \f$\mbox{amp} = 2 \sqrt{\mbox{real}*\mbox{real}+\mbox{imag}*\mbox{imag}}\f$
 * with
 * \f[\mbox{real} = \mbox{re}*(1-\mbox{re\_correl})+\mbox{im}*\mbox{im\_correl}\f] 
 * and
 * \f[\mbox{imag} = \mbox{im}*(1+\mbox{re\_correl})+\mbox{re}*\mbox{im\_correl}.\f]
 *
 *
 * In the case of a real valued atom (\f$\langle g,\overline{g}\rangle = 1\f$) or when 
 * \f$\langle g,\overline{g}\rangle\f$ is very small, the projection is simply 
 * \f[
 * \langle \mbox{sig},g \rangle g = \pm \mbox{amp} \cdot g 
 * \f]
 * with \f$\mbox{amp} = \sqrt{\mbox{re}^2+\mbox{imag}^2}\f$. 
 *
 */
 
void complex2amp_and_phase( double re, double im,
			    double re_correl, double im_correl,
			    double *amp, double *phase );

/** \brief compute an inner product between two signals */
double inner_product ( MP_Real_t *in1, MP_Real_t *in2, unsigned long int size );


/** \brief Remove any blank character from a string,
    and return the string pointer */
char* deblank( char *str );

/** \brief Checking if an integer is even or odd */
#define is_odd( a ) ( (a) & 0x1 )
#define is_even( a ) ( !( (a) & 0x1 ) )


/** \brief A simple variable-size array class */
template <class TYPE>
class MP_Var_Array_c {

public:
  TYPE* elem;
  unsigned long int nElem;
  unsigned long int maxNElem;
  unsigned long int blockSize;
  
#define MP_VAR_ARRAY_DEFAULT_BLOCK_SIZE 1024

  MPTK_LIB_EXPORT MP_Var_Array_c( void ) { elem = NULL; nElem = maxNElem = 0; blockSize = MP_VAR_ARRAY_DEFAULT_BLOCK_SIZE; }
  MPTK_LIB_EXPORT MP_Var_Array_c( unsigned long int setBlockSize ) { elem = NULL; nElem = maxNElem = 0; blockSize = setBlockSize; }
  MPTK_LIB_EXPORT ~MP_Var_Array_c() { if ( elem ) free( elem ); }

  MPTK_LIB_EXPORT MP_Var_Array_c& operator=(const MP_Var_Array_c& cSource);
  MPTK_LIB_EXPORT int append( TYPE newElem );
  MPTK_LIB_EXPORT unsigned long int save_ui_to_text ( const char* fName );
  MPTK_LIB_EXPORT unsigned long int save ( const char* fName );
  MPTK_LIB_EXPORT void clear() { if (elem != NULL) memset( elem, 0, maxNElem*sizeof( TYPE ) ); nElem = 0; }
};

//#include "general.cpp" // <-- tips here for link editing !!!
#endif /* __general_h_ */
