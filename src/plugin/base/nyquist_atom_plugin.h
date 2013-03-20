/******************************************************************************/
/*                                                                            */
/*                             nyquist_atom.h                                */
/*                                                                            */
/*                        Matching Pursuit Library                            */
/*                                                                            */
/* Rémi Gribonval                                                             */
/* Sacha Krstulovic                                                           */
/* Sylvain Lesage                                             Mon Apr 03 2006 */
/* -------------------------------------------------------------------------- */
/*                                                                            */
/*  Copyright (C) 2006 IRISA                                                  */
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
/* DEFINITION OF THE NYQUIST ATOM CLASS,            */
/*                                                   */
/*****************************************************/
/*
 * SVN log:
 *
 * $Author: sacha $
 * $Date: 2007-03-15 18:00:50 +0100 (Thu, 15 Mar 2007) $
 * $Revision: 1013 $
 *
 */
 
#ifndef __nyquist_atom_plugin_h_
#define __nyquist_atom_plugin_h_

/*********************/
/* DIRAC ATOM CLASS  */
/*********************/

/**
 * \brief Nyquist atoms are the atoms that oscillate the most quickly,
 * with a rectangular windows, of unit norm. Their waveform is [1 -1 1
 * -1 ...]. If it is even, the last sample is -1, and the atom
 * corresponds to the Nyquist frequency, else it is 1, and is only the
 * most quickly oscillating atom for this length. (We used 1 for
 * commodity but, it is 1/sqrt(len))
 *
 */
 
class MP_Nyquist_Atom_Plugin_c:public MP_Atom_c {

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
  
  static MP_Atom_c  * nyquist_atom_create_empty(MP_Dict_c* dict);
  
  /** \brief A factory function that reads from a file
   *
   * \param  fid A readable stream
   * \param  mode The reading mode (MP_TEXT or MP_BINARY) 
   *
   * \remark in MP_TEXT mode, NO enclosing XML tag &lt;atom type="*"&gt; ... &lt;/atom&gt; is looked for
   * \sa read_atom()
   */

  static MP_Atom_c* create( FILE *fid, MP_Dict_c *dict, const char mode );
  
protected:

  /* constructor */
  MP_Nyquist_Atom_Plugin_c( MP_Dict_c* dict );



public:

  /* Destructor */
  virtual ~MP_Nyquist_Atom_Plugin_c( void );

  /** \brief File reader */
  virtual int read( FILE *fid, const char mode );

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

  virtual int       has_field( int field );
  virtual MP_Real_t get_field( int field , MP_Chan_t chanIdx );

};

#endif /* nyquist_atom_plugin_h_ */
