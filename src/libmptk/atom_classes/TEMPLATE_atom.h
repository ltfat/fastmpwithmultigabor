/******************************************************************************/
/*                                                                            */
/*                              TEMPLATE_atom.h                               */
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

/*****************************************************/
/*                                                   */
/* DEFINITION OF THE TEMPLATE ATOM CLASS,            */
/* RELEVANT TO THE TEMPLATE TIME-FREQUENCY TRANSFORM */
/*                                                   */
/*****************************************************/
/*
 * SVN log:
 *
 * $Author$
 * $Date$
 * $Revision$
 *
 */


#ifndef __TEMPLATE_atom_h_
#define __TEMPLATE_atom_h_


/***********************/
/* TEMPLATE ATOM CLAS  */
/***********************/

/**
 * \brief TEMPLATE_Atoms add the specification of a TEMPLATE atom to the base Atom class.
 *
 * Add YOUR explanations here about the meaning and use of TEMPLATE atoms
 */
class MP_TEMPLATE_Atom_c: public MP_Atom_c {

  /********/
  /* DATA */
  /********/

public:


  /* YOUR specific data members go here. */


  /***********/
  /* METHODS */
  /***********/

  /***************************/
  /* CONSTRUCTORS/DESTRUCTOR */
  /***************************/

  /* Void constructor */
  MP_TEMPLATE_Atom_c( void );
  /* Specific constructor */
  MP_TEMPLATE_Atom_c( const unsigned int setNumChans
		      /* YOUR parameters */ );
  /* File constructor */
  MP_TEMPLATE_Atom_c( FILE *fid, const char mode );
  /* Destructor */
  virtual ~MP_TEMPLATE_Atom_c( void );


  /***************************/
  /* OUTPUT METHOD           */
  /***************************/
  virtual int write( FILE *fid, const char mode );


  /***************************/
  /* OTHER METHODS           */
  /***************************/
  virtual char * type_name(void);

  virtual int info( FILE *fid );

  /** Build the window and the modulated waveform of a given channel of a TEMPLATE atom, 
   * 
   *  YOUR explainations of how this is done go here.
   */
  virtual void build_waveform( MP_Sample_t *outBuffer );

  virtual int add_to_tfmap( MP_TF_Map_c *tfmap, const char tfmapType );

  virtual int       has_field( int field );
  virtual MP_Real_t get_field( int field , int chanIdx );

};


#endif /* __TEMPLATE_atom_h_ */
