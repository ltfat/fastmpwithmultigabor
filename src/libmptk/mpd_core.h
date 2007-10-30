/******************************************************************************/
/*                                                                            */
/*                                mpd_core.h                                  */
/*                                                                            */
/*                        Matching Pursuit Utilities                          */
/* Benjamin Roy                                                               */
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

/***************************************/
/*                                     */
/* DEFINITION OF THE MPD_CORE CLASS    */
/*                                     */
/***************************************/
/*
 * SVN log:
 *
 * $Author: sacha $
 * $Date: 2007-05-24 16:50:42 +0200 (Thu, 24 May 2007) $
 * $Revision: 1053 $
 *
 */

#include <mptk.h>

#ifndef __mpd_core_h_
#define __mpd_core_h_



/***********************/
/* MPD_CORE CLASS      */
/***********************/
/** \brief The MP_Mpd_Core_c class implements a standard run of
 *  the Matching Pursuit Decomposition (mpd) utility.
 */
class MP_Mpd_Core_c:public MP_Abstract_Core_c
  {

    /********/
    /* DATA */
    /********/

  public:
  private:
    /* Manipulated objects */
    /*\brief The book to stock the atoms */
    MP_Book_c* book;
    /*\brief The dict to parametrise the decomposition */
    MP_Dict_c* dict;
    /** \brief the approximant*/
    MP_Signal_c *approximant;

    /* Output file names */
    /** \brief Name for the book*/
    char *bookFileName;
    /** \brief Name for the approximant*/
    char *approxFileName;


    /***********/
    /* METHODS */
    /***********/

    /***************************/
    /* CONSTRUCTORS/DESTRUCTOR */
    /***************************/
  public:
    /** \brief A factory function for the MP_Mpd_Core_c
     * \param signal the signal to decompose
     * \param setBook the book to stock the atoms
     */
    static MP_Mpd_Core_c* create( MP_Signal_c *signal, MP_Book_c *setBook );
    /** \brief A factory function for the MP_Mpd_Core_c
     * \param signal the signal to decompose
     * \param setBook the book to stock the atoms
     * \param setDict the dict to rule the decomposition
     */
    static MP_Mpd_Core_c* create( MP_Signal_c *setSignal, MP_Book_c *setBook, MP_Dict_c *setDict );
    /** \brief A factory function for the MP_Mpd_Core_c
    * \param signal the signal to decompose
    * \param setBook the book to stock the atoms
    * \param setApproximant  an approximant to reconstruct the signal
    * 
    */
    static MP_Mpd_Core_c* create( MP_Signal_c *setSignal, MP_Book_c *setBook, MP_Signal_c* setApproximant );
    /** \brief A factory function for the MP_Mpd_Core_c
     * \param signal the signal to decompose
     * \param setBook the book to stock the atoms
     * \param setDict the dict to rule the decomposition
     * \param setApproximant  an approximant to reconstruct the signal
     * 
     */
    static MP_Mpd_Core_c* create( MP_Signal_c *setSignal, MP_Book_c *setBook, MP_Dict_c *setDict, MP_Signal_c* setApproximant );

  private:
    /** \brief a private constructor */
    MP_Mpd_Core_c();

  public:
    /** \brief a public destructor */
    virtual ~MP_Mpd_Core_c();

    /***************************/
    /* I/O METHODS             */
    /***************************/

  public:

    /***************************/
    /* OTHER METHODS           */
    /***************************/

    /* Control object*/

    /** \brief Set the dictionary */
    MP_Dict_c* change_dict( MP_Dict_c* setDict );
    /** \brief Set a void dictionary */
    void init_dict();
    /** \brief Plug dictionary to a signal */
    void plug_dict_to_signal();
    /** \brief add custom block to a dictionnary
     * \param setPropertyMap a map containing the parameter of the block
     *  */
    void addCustomBlockToDictionary(map<string, string, mp_ltstring>* setPropertyMap);
    /** \brief add default block to a dictionnary
    * \param  blockName the name of the default block to add
    *  */
    int add_default_block_to_dict( const char* blockName );

    /** \brief save the dictionnary
    * \param  dictName the name for saving
    *  */
    bool save_dict( const char* dictName );
    /* Runtime settings */
    /** \brief plug an approximant in the core
    * \param  approximant a pointer on a MP_Signal
    *  */
    void plug_approximant( MP_Signal_c *approximant );

    /* Runtime */
    /** \brief make one step iterate
     * 
    *  */
    unsigned short int step();

    /* Misc */
    /** \brief Save the result
    *  */
    virtual void save_result( void );

    /** \brief test if can step
    *  */
    MP_Bool_t can_step( void );

    /** \brief print informations on the result of decomposition
    *  */
    virtual void info_result( void );
    /** \brief print informations on the setting of decomposition
    *  */
    virtual void info_conditions( void );
    /** \brief Set informations for save hit
    * \param  setSaveHit an unsigned long int defining the step for automatic save
    * \param setBookFileName name for saving the book
    * \param setResFileName name for saving the residual
    * \param setDecayFileName name for saving the decay file
    *  */
    void set_save_hit( const unsigned long int setSaveHit,
                       const char* setBookFileName,
                       const char* setResFileName,
                       const char* setDecayFileName );
    void get_filter_lengths(vector<unsigned long int> * filterLengthsVector);
  };

#endif /* __mpd_core_h_ */
