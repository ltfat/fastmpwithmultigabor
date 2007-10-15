/******************************************************************************/
/*                                                                            */
/*                             mpd_demix_core.h                               */
/*                                                                            */
/*                        Matching Pursuit Utilities                          */
/*                                                                            */
/* Rémi Gribonval                                                             */
/* Sacha Krstulovic                                           we  Feb 22 2007 */
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
#include "mptk.h"

#ifndef MPD_DEMIX_CORE_H_
#define MPD_DEMIX_CORE_H_



/***********************/
/* MPD_CORE CLASS      */
/***********************/
/** \brief The MP_Mpd_Core_c class implements a standard run of
 *  the Matching Pursuit Decomposition (mpd) utility.
 */
class MP_Mpd_demix_Core_c:public MP_Abstract_Core_c
  {

    /********/
    /* DATA */
    /********/

  private:

    /* Manipulated objects */
    /** \brief A book array to store the atoms for each sources
     *  */
    std::vector<MP_Book_c*> *bookArray;
    /** \brief A dict array to parametrise the decomposition for each sources
     *  */
    std::vector<MP_Dict_c*> *dictArray;
    /** \brief A approximant array for each sources
    *  */
    std::vector<MP_Signal_c*> *approxArray;
    /** \brief A signal array after demixing each sources
    *  */
    std::vector<MP_Signal_c*> *sigArray;
    /** \brief A mixer to demixe each sources
    *  */
    MP_Mixer_c* mixer;

    /* Output file names */
    /** \brief A approximant array for each sources
    *  */
    std::vector<const char *> *bookFileNames;
    /** \brief A approximant array for each sources
    *  */
    std::vector<const char *> *approxFileNames;
    /** \brief The name for the book
    *  */
    char * bookFileName;
    /** \brief The source sequence for atoms extraction
    *  */
    double max, val;
    /** \brief The data for source management
    *  */
    unsigned short int maxSrc;
    /** \brief The data for blocks management
    *  */
    unsigned long int blockIdx, maxBlock;
    /** \brief The max atom buffer
    *  */
    MP_Atom_c   *maxAtom;
    /** \brief The value of the maximun amplitude found
    *  */
    double maxAmp;
    /** \brief The source sequence for atoms extraction
    *  */
    MP_Real_t *amp;
    /** \brief The source sequence for atoms extraction
    *  */
    MP_Var_Array_c<unsigned short int> srcSequences;
    char *srcSeqFileName;
    /** \brief Buffer for string conversion
    *  */
    char line[1024];


    /***********/
    /* METHODS */
    /***********/

    /***************************/
    /* CONSTRUCTORS/DESTRUCTOR */
    /***************************/
  public:
    /** \brief A static factory method
    * \param signal the signal to decompose
    * \param setMixer A mixer to demix the signal
    * \param setBookArray an array to stock the extracted atoms
    *  */
    static MP_Mpd_demix_Core_c* create( MP_Signal_c *signal, MP_Mixer_c* setMixer, std::vector<MP_Book_c*> *setBookArray );

  private:
    /** \brief A private constructor
     *  */
    MP_Mpd_demix_Core_c();

  public:
    /** \brief A public destructor
    *  */
    virtual ~MP_Mpd_demix_Core_c();

    /***************************/
    /* OTHER METHODS           */
    /***************************/

    /* Control object*/

    /* Set the dictionary */
    std::vector<MP_Dict_c*>* change_dict( std::vector<MP_Dict_c*> *setDictArray );

    /* Runtime settings */
    void plug_approximant( std::vector<MP_Signal_c*> *setApproxArray );
    /* Runtime */
    /** \brief make one step iterate
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
    virtual unsigned long int book_append(MP_Book_c *newBook);

    virtual void info_result( void );
    /** \brief print informations on the setting of decomposition
     *  */
    virtual void info_conditions( void );
    /** \brief Set informations for save hit
    * \param  setSaveHit an unsigned long int defining the step for automatic save
    * \param setBookFileName name for saving the book
    * \param setResFileName name for saving the residual
    * \param setDecayFileName name for saving the decay file
    * \param setSrcSeqFileName name for saving the sequence file
    *  */
    void set_save_hit( const unsigned long int setSaveHit,
                       const char* setBookFileName,
                       const char* setResFileName,
                       const char* setDecayFileName,
                       const char* setSrcSeqFileName );

  };


#endif /*MPD_DEMIX_CORE_H_*/
