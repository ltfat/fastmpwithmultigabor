/******************************************************************************/
/*                                                                            */
/*                              dirac_atom.h                                  */
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
/* DEFINITION OF THE DIRAC ATOM CLASS,               */
/*                                                   */
/*****************************************************/
/*
 * SVN log:
 *
 * $Author: broy $
 * $Date: 2007-03-15 18:00:50 +0100 (Thu, 15 Mar 2007) $
 * $Revision: 1013 $
 *
 */

#ifndef dirac_atom_plugin_h_
#define dirac_atom_plugin_h_

/*********************/
/* DIRAC ATOM CLASS  */
/*********************/

/**
 * \brief Diracs are the most basic atoms of the MP_Atom_c class.
 *
 * A (multichannel) dirac atom is an atom which waveform has
 * a single nonzero coefficient on each channel
 */
class MP_Dirac_Atom_Plugin_c: public MP_Atom_c {

  /********/
  /* DATA */
  /********/

public:
  /* VOID */

  /***********/
  /* METHODS */
  /***********/

  /***************************/
  /* CONSTRUCTORS/DESTRUCTOR */
  /***************************/

public:

  /* Specific empty factory function */
  static MP_Atom_c  * dirac_atom_create_empty(void);
  /* Specific factory function */
  
   /* File factory function */
  static MP_Atom_c* create( FILE *fid, const char mode );
  
protected:

  /* Void constructor */
  MP_Dirac_Atom_Plugin_c( void );



  /** \brief File reader */
  virtual int read( FILE *fid, const char mode );

public:

  /* Destructor */
  virtual ~MP_Dirac_Atom_Plugin_c( void );


  /***************************/
  /* OUTPUT METHOD           */
  /***************************/
  virtual int write( FILE *fid, const char mode );


  /***************************/
  /* OTHER METHODS           */
  /***************************/
  virtual const char * type_name(void);

  virtual int info( FILE *fid );
  virtual int info();

  virtual void build_waveform( MP_Real_t *outBuffer );

  virtual int add_to_tfmap( MP_TF_Map_c *tfmap, const char tfmapType );

  virtual MP_Real_t dist_to_tfpoint( MP_Real_t time, MP_Real_t freq , MP_Chan_t chanIdx );
  virtual int       has_field( int field );
  virtual MP_Real_t get_field( int field , MP_Chan_t chanIdx );

};



#endif /* dirac_atom_plugin_h_ */
