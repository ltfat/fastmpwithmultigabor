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
class MP_Mpd_demix_Core_c:public MP_Abstract_Core_c {

  /********/
  /* DATA */
  /********/

private:

  /* Manipulated objects */
    std::vector<MP_Book_c*> *bookArray;
    std::vector<MP_Dict_c*> *dictArray;
    std::vector<MP_Signal_c*> *approxArray;
    std::vector<MP_Signal_c*> *sigArray;
    MP_Mixer_c* mixer;
    
    /* Output file names */
    std::vector<const char *> *bookFileNames;
    std::vector<const char *> *approxFileNames;
    char * bookFileName;
    
    double max, val;
    unsigned short int maxSrc;
    unsigned long int blockIdx, maxBlock;
    MP_Atom_c   *maxAtom;
    double maxAmp;
    MP_Real_t *amp;
    
    MP_Var_Array_c<unsigned short int> srcSequences;
    char *srcSeqFileName;
    char line[1024];
    
    
  /***********/
  /* METHODS */
  /***********/

  /***************************/
  /* CONSTRUCTORS/DESTRUCTOR */
  /***************************/
public:
  static MP_Mpd_demix_Core_c* create( MP_Signal_c *signal, MP_Mixer_c* setMixer, std::vector<MP_Book_c*> *setBookArray );

private:
  MP_Mpd_demix_Core_c();

public:
  ~MP_Mpd_demix_Core_c();

  /***************************/
  /* OTHER METHODS           */
  /***************************/
 
   /* Control object*/
  
  /* Set the dictionary */
  std::vector<MP_Dict_c*>* change_dict( std::vector<MP_Dict_c*> *setDictArray );

  /* Runtime settings */
  virtual void plug_approximant( MP_Signal_c *setApproximant );
  void plug_approximant( std::vector<MP_Signal_c*> *setApproxArray );
  /* Runtime */
  unsigned short int step();

  /* Misc */
  virtual void save_result( void );
  MP_Bool_t can_step( void );  
  virtual unsigned long int book_append(MP_Book_c *newBook);
  virtual void info_result( void );
  virtual void info_conditions( void );
  void set_save_hit( const unsigned long int setSaveHit,
                   const char* setBookFileName,   
                   const char* setResFileName,
                   const char* setDecayFileName,
                   const char* setSrcSeqFileName );

};


#endif /*MPD_DEMIX_CORE_H_*/
