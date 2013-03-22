/******************************************************************************/
/*                                                                            */
/*                             template_atom.h                                */
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
/* DEFINITION OF THE TEMPLATE ATOM CLASS,            */
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


#ifndef __template_atom_plugin_h_
#define __template_atom_plugin_h_

/* YOUR includes go here. */


/*********************/
/* DIRAC ATOM CLASS  */
/*********************/

/**
 * \brief Explain tour atom here...
 *
 */
class MP_Template_Atom_Plugin_c: public MP_Atom_c {

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

  /** \brief Specific empty factory function */
  static MP_Atom_c  * Template_atom_create_empty(MP_Dict_c* dict);
  
  /** \brief Specific file factory function */
  static MP_Atom_c* create_fromxml( TiXmlElement *xmlobj, MP_Dict_c *dict );
  static MP_Atom_c* create_frombinary( FILE *fid, MP_Dict_c *dict );

protected:

  /* Void constructor */
  MP_Template_Atom_Plugin_c( MP_Dict_c* dict );

  /** \brief initialisers from data */
  virtual int init_fromxml(TiXmlElement* xmlobj);
  virtual int init_frombinary( FILE *fid );

public:

  /* Destructor */
  virtual ~MP_Template_Atom_Plugin_c( void );


  /***************************/
  /* OUTPUT METHOD           */
  /***************************/
  
   /** \brief Write the atom to a stream in binary format
   * \param  fid A writable stream
   * \param  mode The writing mode (MP_TEXT or MP_BINARY)
   * \return The number of items written to the stream 
   *
   * \remark in MP_TEXT mode, NO enclosing XML tag <atom type="*"> ... </atom> is written
   * \sa write_atom()
   */
  virtual int write( FILE *fid, const char mode );


  /***************************/
  /* OTHER METHODS           */
  /***************************/
  
 /** \brief Get the type of the atom as a string
  *
  * \return the type as a string
  */
  virtual char * type_name(void);

   /** \brief Print human readable information about the atom to a stream
   * \param  fid A writable stream
   * \return The number of characters written to the stream */
   
  virtual int info( FILE *fid );
    /** \brief Print human readable information about the atom to the default info handler
   * \return The number of characters written to the stream */
  virtual int info();

 /** \brief Build concatenated waveforms corresponding to each channel of an atom. 
   * \param outBuffer the array of size \b totalChanLen which is filled with the  concatenated 
   * waveforms of all channels. */ 
  virtual void build_waveform( MP_Real_t *outBuffer );

  /** \brief Adds a pseudo Wigner-Ville of the atom to a time-frequency map 
   * \param tfmap the time-frequency map to which the atom distribution will be plotted
   * \param tfmapType an indicator of what to put in the tfmap, to be chosen among
   * MP_TFMAP_SUPPORTS or MP_TFMAP_PSEUDO_WIGNER (see tfmap.h for more).
   * \return one if the atom printed something into the map, zero otherwise
   */
  virtual int add_to_tfmap( MP_TF_Map_c *tfmap, const char tfmapType );

  /** \brief Test if the atom has a given field 
   * \param field the field type
   * \return MP_TRUE if the atom has the field, MP_FALSE otherwise */
  virtual int       has_field( int field );
  
  /** \brief Gets a field of a channel of an atom 
   * \param field the field type
   * \param chanIdx the desired channel
   * \return the desired field value, zero if the atom has no such field */
  virtual MP_Real_t get_field( int field , MP_Chan_t chanIdx );

};


#endif /* __Template_atom_plugin_h_ */

