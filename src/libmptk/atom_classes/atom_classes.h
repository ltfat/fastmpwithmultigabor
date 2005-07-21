/******************************************************************************/
/*                                                                            */
/*                               atom_classes.h                               */
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

/*************************************************/
/*                                               */
/* INTERFACE BETWEEN SPECIFIC ATOM/BLOCK CLASSES */
/* AND THE REST OF THE LIBRARY                   */
/*                                               */
/*************************************************/
/*
 * CVS log:
 *
 * $Author$
 * $Date$
 * $Revision$
 *
 */

#ifndef __atom_classes_h_
#define __atom_classes_h_


/*************/
/* FUNCTIONS */
/*************/

/** \brief Read a stream in text or binary format to create a new atom
 *
 * \param  fid A readable stream
 * \param  mode MP_TEXT for a text stream or MP_BINARY for a binary stream
 * \return a new atom upon success, NULL if the atom could not be created
 * (e.g. for a lack of memory or a file format mismatch). 
 *
 * In MP_TEXT mode, enclosing XML tags <atom type="*"> ... </atom> are read to 
 * determine the type of the atom
 * \sa write_atom()
*/
MP_Atom_c* read_atom( FILE *fid, const char mode );

/** \brief Writes an atom to a stream in text or binary format
 *
 * \param  fid A writeable stream
 * \param  mode MP_TEXT for a text stream or MP_BINARY for a binary stream
 * \param  atom A reference on the atom to be written
 * \return The number of written characters or bytes. 
 *
 * In MP_TEXT mode, enclosing XML tags <atom type="*"> ... </atom> are written for 
 * compatibility with read_atom().
 * \sa read_atom() MP_Atom_c::write()
*/
int write_atom( FILE *fid, const char mode, MP_Atom_c *atom );


#endif /* __atom_classes_h_ */
