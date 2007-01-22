/******************************************************************************/
/*                                                                            */
/*                                  mptk.h                                    */
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

/*
   NOTE: this mptk.h header is generated automatically.
   Any manual modification to it will be lost at the next build/install
   of the library. Please add potential modifications to the individual
   class-relevant header files instead.
*/

/* SVN log: $Author$ $Date$ $Revision$ */
#ifndef __mptk_h_
#define __mptk_h_
                   
	

#cmakedefine HAVE_MPTK_HEADER_H 1
# ifdef HAVE_MPTK_HEADER_H
#  include "header.h"
# endif

#cmakedefine HAVE_MPTK_CONFIG_H 1
# ifdef HAVE_MPTK_CONFIG_H
#  include "config.h"
# endif

#cmakedefine HAVE_MPTK_MP_SYSTEM_H 1
# ifdef HAVE_MPTK_MP_SYSTEM_H
#include "mp_system.h"
# endif

#cmakedefine HAVE_MPTK_REGRESSION_CONSTANT_H 1
# ifdef HAVE_MPTK_REGRESSION_CONSTANT_H
#include "regression_constants.h"
# endif

#cmakedefine HAVE_MPTK_MP_TYPE_H 1
# ifdef HAVE_MPTK_MP_TYPE_H
#include "mp_types.h"
# endif

#cmakedefine HAVE_MPTK_MP_MESSAGING_H 1
# ifdef HAVE_MPTK_MP_MESSAGING_H
#include "mp_messaging.h"  
# endif

#cmakedefine HAVE_MPTK_WIN_SERVER_H 1
# ifdef HAVE_MPTK_WIN_SERVER_H
#include "win_server.h"
# endif

#cmakedefine HAVE_MPTK_FFT_INTERFACE_H 1
# ifdef HAVE_MPTK_FFT_INTERFACE_H
#include "fft_interface.h"
# endif     

#cmakedefine HAVE_MPTK_GENERAL_H 1
# ifdef HAVE_MPTK_GENERAL_H
#include "general.h"
# endif       
   
#cmakedefine HAVE_MPTK_MTRAND_H 1
# ifdef HAVE_MPTK_MTRAND_H
#include "mtrand.h"
# endif
  
#cmakedefine HAVE_MPTK_REGRESSION_H 1
# ifdef HAVE_MPTK_REGRESSION_H
#include "regression.h"
# endif          

#cmakedefine HAVE_MPTK_MP_SIGNAL_H 1
# ifdef HAVE_MPTK_MP_SIGNAL_H
#include "mp_signal.h"
# endif             

#cmakedefine HAVE_MPTK_TFMAP_H 1
# ifdef HAVE_MPTK_TFMAP_H
#include "tfmap.h"
# endif

#cmakedefine HAVE_MPTK_ATOM_H 1
# ifdef HAVE_MPTK_ATOM_H
#include "atom.h"
# endif                 

#cmakedefine HAVE_MPTK_BLOCK_H 1
# ifdef HAVE_MPTK_BLOCK_H
#include "block.h"
# endif    

#cmakedefine HAVE_MPTK_MASK_H 1
# ifdef HAVE_MPTK_MASK_H
#include "mask.h"
# endif              

#cmakedefine HAVE_MPTK_BOOK_H 1
# ifdef HAVE_MPTK_BOOK_H
#include "book.h"
# endif     

#cmakedefine HAVE_MPTK_DICT_H 1
# ifdef HAVE_MPTK_DICT_H
#include "dict.h"
# endif 
             
#cmakedefine HAVE_MPTK_ANYWAVE_TABLE_H 1
# ifdef HAVE_MPTK_ANYWAVE_TABLE_H
#include "anywave_table.h"
# endif 

#cmakedefine HAVE_MPTK_ANYWAVE_SERVER_H 1
# ifdef HAVE_MPTK_ANYWAVE_SERVER_H
#include "anywave_server.h"
# endif 

#cmakedefine HAVE_MPTK_ANYWAVE_TABLE_IO_INTERFACE_H 1
# ifdef HAVE_MPTK_ANYWAVE_TABLE_IO_INTERFACE_H
#include "anywave_table_io_interface.h"
# endif

#cmakedefine HAVE_MPTK_CONVOLUTION_H 1
# ifdef HAVE_MPTK_CONVOLUTION_H
#include "convolution.h"
# endif

#cmakedefine HAVE_MPTK_DIRAC_ATOM_H 1
# ifdef HAVE_MPTK_DIRAC_ATOM_H
#include "atom_classes/base/dirac_atom.h"
# endif

#cmakedefine HAVE_MPTK_DIRAC_BLOCK_H 1
# ifdef HAVE_MPTK_DIRAC_BLOCK_H
#include "atom_classes/base/dirac_block.h"
# endif

#cmakedefine HAVE_MPTK_GABOR_ATOM_H 1
# ifdef HAVE_MPTK_GABOR_ATOM_H
#include "atom_classes/base/gabor_atom.h"
# endif

#cmakedefine HAVE_MPTK_GABOR_BLOCK_H 1
# ifdef HAVE_MPTK_GABOR_BLOCK_H
#include "atom_classes/base/gabor_block.h"
# endif        

#cmakedefine HAVE_MPTK_HARMONIC_ATOM_H 1
# ifdef HAVE_MPTK_HARMONIC_ATOM_H
#include "atom_classes/base/harmonic_atom.h"
# endif

#cmakedefine HAVE_MPTK_HARMONIC_BLOCK_H 1
# ifdef HAVE_MPTK_HARMONIC_BLOCK_H
#include "atom_classes/base/harmonic_block.h" 
# endif 

#cmakedefine HAVE_MPTK_CHIRP_BLOCK_H 1
# ifdef HAVE_MPTK_CHIRP_BLOCK_H
#include "atom_classes/base/chirp_block.h" 
# endif 

#cmakedefine HAVE_MPTK_ANYWAVE_ATOM_H 1
# ifdef HAVE_MPTK_ANYWAVE_ATOM_H
#include "atom_classes/base/anywave_atom.h" 
# endif       

#cmakedefine HAVE_MPTK_ANYWAVE_BLOCK_H 1
# ifdef HAVE_MPTK_ANYWAVE_BLOCK_H
#include "atom_classes/base/anywave_block.h"
# endif 
   
#cmakedefine HAVE_MPTK_ANYWAVE_HILBERT_ATOM_H 1
# ifdef HAVE_MPTK_ANYWAVE_HILBERT_ATOM_H
#include "atom_classes/base/anywave_hilbert_atom.h"
# endif

#cmakedefine HAVE_MPTK_ANYWAVE_HILBERT_BLOCK_H 1
# ifdef HAVE_MPTK_ANYWAVE_HILBERT_BLOCK_H
#include "atom_classes/base/anywave_hilbert_block.h"
# endif 

#cmakedefine HAVE_MPTK_CONSTANT_ATOM_H 1
# ifdef HAVE_MPTK_CONSTANT_ATOM_H
#include "atom_classes/base/constant_atom.h"
# endif

#cmakedefine HAVE_MPTK_CONSTANT_BLOCK_H 1
# ifdef HAVE_MPTK_CONSTANT_BLOCK_H
#include "atom_classes/base/constant_block.h"
# endif 

#cmakedefine HAVE_MPTK_NYQUIST_ATOM_H 1
# ifdef HAVE_MPTK_NYQUIST_ATOM_H
#include "atom_classes/base/nyquist_atom.h"
# endif

#cmakedefine HAVE_MPTK_NYQUIST_BLOCK_H 1
# ifdef HAVE_MPTK_NYQUIST_BLOCK_H
#include "atom_classes/base/nyquist_block.h"
# endif 

#cmakedefine HAVE_MPTK_MCLT_ABSTRACT_BLOCK_H 1
# ifdef HAVE_MPTK_MCLT_ABSTRACT_BLOCK_H
#include "atom_classes/contrib/lam/mclt_abstract_block.h"
# endif 

#cmakedefine HAVE_MPTK_MCDT_BLOCK_H 1
# ifdef HAVE_MPTK_MCDT_BLOCK_H
#include "atom_classes/contrib/lam/mdct_block.h"
# endif

#cmakedefine HAVE_MPTK_MCDT_ATOM_H 1
# ifdef HAVE_MPTK_MCDT_ATOM_H
#include "atom_classes/contrib/lam/mdct_atom.h"
# endif

#cmakedefine HAVE_MPTK_MCLT_BLOCK_H 1
# ifdef HAVE_MPTK_MCLT_BLOCK_H
#include "atom_classes/contrib/lam/mclt_block.h"
# endif	 

#cmakedefine HAVE_MPTK_MCLT_ATOM_H 1
# ifdef HAVE_MPTK_MCLT_ATOM_H
#include "atom_classes/contrib/lam/mclt_atom.h"
# endif

#cmakedefine HAVE_MPTK_MDST_BLOCK_H 1
# ifdef HAVE_MPTK_MDST_BLOCK_H
#include "atom_classes/contrib/lam/mdst_block.h"
# endif	 

#cmakedefine HAVE_MPTK_MDST_ATOM_H 1
# ifdef HAVE_MPTK_MDST_ATOM_H
#include "atom_classes/contrib/lam/mdst_atom.h"
# endif
	
#cmakedefine HAVE_MPTK_BLOCK_IO_INTERFACE_H 1
# ifdef HAVE_MPTK_BLOCK_IO_INTERFACE_H
#include "atom_classes/block_io_interface.h"
# endif 

#cmakedefine HAVE_MPTK_ATOM_CLASSES_H 1
# ifdef HAVE_MPTK_ATOM_CLASSES_H
#include "atom_classes/atom_classes.h"
# endif 

#cmakedefine HAVE_MPTK_MPD_CORE_H 1
# ifdef HAVE_MPTK_MPD_CORE_H
#include "mpd_core.h"
# endif

#cmakedefine HAVE_MPTK_MP_ENV_H 1
# ifdef HAVE_MPTK_MP_ENV_H
#include "mptk_env.h"
# endif
  
        




#endif /* __mptk_h_ */
