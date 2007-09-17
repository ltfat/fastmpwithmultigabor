/******************************************************************************/
/*                                                                            */
/*                                 mixer.h                                    */
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

/********************************************/
/*                                          */
/* DEFINITION OF THE MIXER INTERFACE CLASS  */
/*                                          */
/********************************************/








#ifndef MIXER_H_
#define MIXER_H_

#include <stdio.h>
#include <utility>
#include <vector.h>

/** \brief The MP_Abstract_Mixer class is an abstract base class used to define the interface between MPTK
 * and various mixer class
 * 
 */ 

class MP_Abstract_Mixer_c
  {
    /********/
    /* DATA */
    /********/

  public:
   
    /***********/
    /* METHODS */
    /***********/
    /** Direct filtering method */
	virtual void applyDirect( const std::vector<MP_Signal_c*> *sourceSignalArray, MP_Signal_c *mixedSignal ) = 0;
	/** Inverse filtering method */
	virtual void applyAdjoint( std::vector<MP_Signal_c*> *sourceSignalArray, const MP_Signal_c *mixedSignal ) = 0;
    /***************************/
    /* FACTORY METHOD          */
    /***************************/
    static MP_Abstract_Mixer_c* creator (FILE *mixerFID);
    
    /***************************/
    /* CONSTRUCTORS/DESTRUCTOR */
    /***************************/

  protected:

    /** \brief A generic constructor used only by non-virtual children classes */
    MP_Abstract_Mixer_c( void );

  public:
  /** \brief Public destructor */
    virtual ~MP_Abstract_Mixer_c();
  
  /** \brief File reader */
  virtual int read( FILE *fid) = 0 ;
  
  /*****************************************************/
  /* VIRTUAL NULL METHODS, MANDATORY IN THE SUBCLASSES */
  /*****************************************************/
  
    /** \brief Print human readable information about the atom to a stream
   * \param  fid A writable stream
   * \return The number of characters written to the stream */
 
 
  };


/** \brief The MP_Mixer class is a concrete implementation of the MP_Abstract_Mixer_c 
 * it allows to load mixer from txt files
 * 
 */ 
 
class MP_Mixer_c : public MP_Abstract_Mixer_c
  {
  	public:
  	static MP_Mixer_c* creator_from_txt_file( const char *mixerFileName );
    static MP_Mixer_c* creator_from_txt_file( FILE * mixerFID );
    
    /********/
    /* DATA */
    /********/
    
     /** \brief number of sources in the mixed signal */
    unsigned short int numSources;

    /** \brief number of channels of the mixed signal */
    MP_Chan_t numChans;
    
    /** \brief the mixer */
    MP_Real_t *mixer;
    /** \brief buffer */
    MP_Real_t *p;
    /** \brief buffer */
    MP_Real_t *Ah;
    
    
     /***********/
    /* METHODS */
    /***********/

    /***************************/
    /* FACTORY METHOD          */
    /***************************/
    
    /***************************/
    /* CONSTRUCTORS/DESTRUCTOR */
    /***************************/

  //protected:
    public:

    /** \brief A generic constructor used only by non-virtual children classes */
    MP_Mixer_c( void );

  public:
    /** \brief A public destructor */
    virtual ~MP_Mixer_c();
    
    /****************/
    /* MISC METHODS */
    /****************/
    
    virtual int read( FILE *fid);
    /** Direct filtering method */
	virtual void applyDirect( const std::vector<MP_Signal_c*> *sourceSignalArray, MP_Signal_c *mixedSignal );
	/** Inverse filtering method */
	virtual void applyAdjoint( std::vector<MP_Signal_c*> *sourceSignalArray, const MP_Signal_c *mixedSignal );

  };


#endif /*MIXER_H_*/
