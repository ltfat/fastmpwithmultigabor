/******************************************************************************/
/*                                                                            */
/*                                calculator.h                                  */
/*                                                                            */
/*                        Matching Pursuit Utilities                          */
/* Thomas Blumensath                                                          */
/* Benjamin Roy                                                               */
/* R�mi Gribonval                                                             */
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

/*****************************************/
/*                                       */
/* DEFINITION OF THE CALCULATOR CLASS    */
/*                                       */
/*****************************************/
/*
 * SVN log:
 *
 * $Author: sacha $
 * $Date: 2007-05-24 16:50:42 +0200 (Thu, 24 May 2007) $
 * $Revision: 1053 $
 *
 */

#include <mptk.h>

#ifndef GPD_CORE_H_
#define GPD_CORE_H_



/***********************/
/* _CORE CLASS      */
/***********************/
/** \brief The MP_Gpd_Core_c class implements a standard run of
 *  the Gradient Pursuit Decomposition (gpd) utility.
 */
class GPD_Core_c:public MP_Abstract_Core_c
  {

    /********/
    /* DATA */
    /********/

  public:
  GP_Double_Index_Book_c* book;
  private:
    /* Manipulated objects */
    /*\brief The book to stock the atoms */
   // GP_Double_Index_Book_c* book;
    /*\brief The dict to parametrise the decomposition */
    MP_Dict_c* dict;
    /** \brief the approximant*/
    MP_Signal_c *approximant;

    /** Sub-book where correlations need to be updated */
    GP_Pos_Range_Sub_Book_c* touchBook;
    
    /* Output file names */
    /** \brief Name for the book*/
    char *bookFileName;
    /** \brief Name for the approximant*/
    char *approxFileName;
    /** \brief Gradient buffer */
    MP_Real_t* gradient;
    /** \brief temporary buffer for computing the gradient */
    MP_Real_t* tmpBuffer;



    /***********/
    /* METHODS */
    /***********/

    /***************************/
    /* CONSTRUCTORS/DESTRUCTOR */
    /***************************/
  public:
    /** \brief A factory function for the MP_Gpd_Core_c
     * \param signal the signal to decompose
     * \param setBook the book to stock the atoms
     * \param setDict the dict to rule the decomposition
     */
    MPTK_LIB_EXPORT static GPD_Core_c* create( MP_Signal_c *setSignal, MP_Dict_c *setDict );
    MPTK_LIB_EXPORT static GPD_Core_c* create( MP_Signal_c *setSignal, GP_Double_Index_Book_c *setBook );
    MPTK_LIB_EXPORT static GPD_Core_c* create( MP_Signal_c *setSignal, GP_Double_Index_Book_c *setBook, MP_Dict_c *setDict );
    MPTK_LIB_EXPORT static GPD_Core_c* create( MP_Signal_c *setSignal, GP_Double_Index_Book_c *setBook, MP_Signal_c* setApproximant );
  
  private:
    /** \brief a private constructor */
    MPTK_LIB_EXPORT GPD_Core_c();

  public:
    /** \brief a public destructor */
    MPTK_LIB_EXPORT virtual ~GPD_Core_c();

    /***************************/
    /* I/O METHODS             */
    /***************************/

  public:

    /***************************/
    /* OTHER METHODS           */
    /***************************/

    /* Control object*/

    /** \brief Set the dictionary */
    MPTK_LIB_EXPORT MP_Dict_c* change_dict( MP_Dict_c* setDict );
    /** \brief Set a void dictionary */
    MPTK_LIB_EXPORT void init_dict();
    /** \brief Plug dictionary to a signal */
    MPTK_LIB_EXPORT void plug_dict_to_signal();
    /** \brief add custom block to a dictionnary
     * \param setPropertyMap a map containing the parameter of the block
     *  */
    MPTK_LIB_EXPORT void addCustomBlockToDictionary(map<string, string, mp_ltstring>* setPropertyMap);
    /** \brief add default block to a dictionnary
    * \param  blockName the name of the default block to add
    *  */
    MPTK_LIB_EXPORT int add_default_block_to_dict( const char* blockName );

    /** \brief save the dictionnary
    * \param  dictName the name for saving
    *  */
    MPTK_LIB_EXPORT bool save_dict( const char* dictName );
    /* Runtime settings */
    /** \brief plug an approximant in the core
    * \param  approximant a pointer on a MP_Signal
    *  */
    MPTK_LIB_EXPORT void plug_approximant( MP_Signal_c *approximant );

    /* Runtime */
    /** \brief make one step of gradient pursuit
     * 
    *  */
    MPTK_LIB_EXPORT unsigned short int step();

    /* Misc */
    /** \brief Save the result
    *  */
    MPTK_LIB_EXPORT virtual void save_result( void );

    /** \brief test if can step
    *  */
    MPTK_LIB_EXPORT MP_Bool_t can_step( void );

    /** \brief print informations on the result of decomposition
    *  */
    MPTK_LIB_EXPORT virtual void info_result( void );
    /** \brief print informations on the setting of decomposition
    *  */
    MPTK_LIB_EXPORT virtual void info_conditions( void );
    /** \brief Set informations for save hit
    * \param  setSaveHit an unsigned long int defining the step for automatic save
    * \param setBookFileName name for saving the book
    * \param setResFileName name for saving the residual
    * \param setDecayFileName name for saving the decay file
    *  */
    MPTK_LIB_EXPORT void set_save_hit( const unsigned long int setSaveHit,
                       const char* setBookFileName,
                       const char* setResFileName,
                       const char* setDecayFileName );
   /** \brief fill a vector with the filter length of blocks contained in dictionary
    * * \param filterLengthsVector the vector to fill
    *  */               
    MPTK_LIB_EXPORT void get_filter_lengths(vector<unsigned long int> * filterLengthsVector);
    
    /** \brief Perform one gradient pursuit iteration (update the blocks, find the max,
     * create the patch and append it to the book, and substract it from the analyzed signal).
     *
     * \param book The book where to append the selected atom
     * \param sigRecons A signal where to add the selected atom for online reconstruction
     * \return one upon success, zero otherwise
     * \remark Pass sigRecons == NULL to skip the reconstruction step
     * \remark Ideally, the number of arithmetic operations is kept to a minimum.
     */
    MPTK_LIB_EXPORT int iterate_gp( MP_Book_c* book , MP_Dict_c* dict, MP_Signal_c* sigRecons );
  };

#endif /* GPD_CORE_H_ */
