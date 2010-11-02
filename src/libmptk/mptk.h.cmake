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

/* SVN log: $Author: broy $ $Date: 2007-06-29 18:03:53 +0200 (Fri, 29 Jun 2007) $ $Revision: 1084 $ */
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

#cmakedefine HAVE_MPTK_MP_DLL_H 1
# ifdef HAVE_MPTK_MP_DLL_H
#include "dll.h"
# endif

#cmakedefine HAVE_MPTK_WIN_SERVER_H 1
# ifdef HAVE_MPTK_WIN_SERVER_H
#include "win_server.h"
# endif

#cmakedefine HAVE_MPTK_FFT_INTERFACE_H 1
# ifdef HAVE_MPTK_FFT_INTERFACE_H
#include "fft_interface.h"
# endif  
   
#cmakedefine HAVE_MPTK_DCT_INTERFACE_H 1
# ifdef HAVE_MPTK_DCT_INTERFACE_H
#include "dct_interface.h"
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

#cmakedefine HAVE_MPTK_ATOM_PARAM_H 1
# ifdef HAVE_MPTK_ATOM_PARAM_H
#include "atom_param.h"
# endif

#cmakedefine HAVE_MPTK_FREQ_ATOM_PARAM_H 1
# ifdef HAVE_MPTK_FREQ_ATOM_PARAM_H
#include "freq_atom_param.h"
# endif

#cmakedefine HAVE_MPTK_ATOM_H 1
# ifdef HAVE_MPTK_ATOM_H
#include "atom.h"
# endif  

#cmakedefine HAVE_MPTK_GP_BOOK_H 1
# ifdef HAVE_MPTK_GP_BOOK_H
#include "gp_book.h"
# endif     

  #cmakedefine HAVE_MPTK_GP_POS_RANGE_SUB_BOOK_H 1
# ifdef HAVE_MPTK_GP_POS_RANGE_SUB_BOOK_H
#include "gp_pos_range_sub_book.h"
# endif

#cmakedefine HAVE_MPTK_GP_PARAM_BOOK_H 1
# ifdef HAVE_MPTK_GP_PARAM_BOOK_H
#include "gp_param_book.h"
# endif

#cmakedefine HAVE_MPTK_GP_POS_BOOK_H 1
# ifdef HAVE_MPTK_GP_POS_BOOK_H
#include "gp_pos_book.h"
# endif

#cmakedefine HAVE_MPTK_GP_BLOCK_BOOK_H 1
# ifdef HAVE_MPTK_GP_BLOCK_BOOK_H
#include "gp_block_book.h"
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

#cmakedefine HAVE_MPTK_MP_MIXER_H 1
# ifdef HAVE_MPTK_MP_MIXER_H
#include "mixer.h"
# endif

#cmakedefine HAVE_MPTK_MP_CORE_H 1
# ifdef HAVE_MPTK_MP_CORE_H
#include "mp_core.h"
# endif

#cmakedefine HAVE_MPTK_MPD_CORE_H 1
# ifdef HAVE_MPTK_MPD_CORE_H
#include "mpd_core.h"
# endif

#cmakedefine HAVE_MPTK_MPD_DEMIX_CORE_H 1
# ifdef HAVE_MPTK_MPD_DEMIX_CORE_H
#include "mpd_demix_core.h"
# endif

#cmakedefine HAVE_MPTK_ATOM_FACTORY_H 1
# ifdef HAVE_MPTK_ATOM_FACTORY_H
#include "atom_factory.h"
# endif

#cmakedefine HAVE_MPTK_BLOCK_FACTORY_H 1
# ifdef HAVE_MPTK_BLOCK_FACTORY_H
#include "block_factory.h"
# endif

#cmakedefine HAVE_MPTK_MP_ENV_H 1
# ifdef HAVE_MPTK_MP_ENV_H
#include "mptk_env.h"
# endif

#cmakedefine HAVE_MPTK_MP_PTHREADS_BARRIER_H 1
# ifdef HAVE_MPTK_MP_PTHREADS_BARRIER_H
#include "mp_pthreads_barrier.h"
# endif

#cmakedefine HAVE_MPTK_DOUBLE_INDEX_BOOK_H 1
# ifdef HAVE_MPTK_DOUBLE_INDEX_BOOK_H
#include "double_index_book.h"
# endif

#cmakedefine HAVE_MPTK_GPD_CORE_H 1
# ifdef HAVE_MPTK_GPD_CORE_H
#include "gpd_core.h"
# endif

#endif /* __mptk_h_ */
