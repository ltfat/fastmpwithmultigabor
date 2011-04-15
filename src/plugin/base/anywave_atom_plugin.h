/******************************************************************************/
/*                                                                            */
/*                              anywave_atom.h                                */
/*                                                                            */
/*                        Matching Pursuit Library                            */
/*                                                                            */
/* Rémi Gribonval                                                             */
/* Sacha Krstulovic                                                           */
/* Sylvain Lesage                                             Thu Nov 03 2005 */
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
/* DEFINITION OF THE anywave ATOM CLASS,            */
/* RELEVANT TO THE anywave TIME-FREQUENCY TRANSFORM */
/*                                                   */
/*****************************************************/
/*
 * CVS log:
 *
 * $Author: broy $
 * $Date: 2007-05-25 17:36:47 +0200 (Fri, 25 May 2007) $
 * $Revision: 1056 $
 *
 */


#ifndef __anywave_atom_plugin_h_
#define __anywave_atom_plugin_h_


/***********************/
/* anywave ATOM CLAS  */
/***********************/

/**
 * \brief Anywave atoms add the specification of an anywave atom to the base Atom class.
 *
 * An anywave atom is only specified by :
 * - a waveform, that is the series of the time samples, and
 * - an amplitude that multiplies the waveform.
 *
 * The waveforms are not stored in the MP_Anywave_Atom_c, but in a
 * MP_Anywave_Table_c object, that contains a set of similar
 * waveforms. The anywave tables are themselves stored in a global
 * instance of the MP_Anywave_Server_c class called
 * MP_GLOBAL_ANYWAVE_SERVER.
 *
 * The anywave atom has a member called tableIdx that indicate which
 * table to use in the anywave server (also refered by a pointer :
 * anywave table), and a member called anywaveIdx that indicate which
 * waveform to use in the anywave table.
 *
 * If the waveform has only one channel, then the anywave atom has as
 * many channels as the signal to decompose, and there is one
 * amplitude for each channel, ie, the anywave atom has the same
 * waveform on all its channels, with a different amplitude. 
 *
 * If the waveform has as many channels as the signal to decompose,
 * then the amplitude is the same on all its channels, ie, the anywave
 * atom is proportionnal to the multichannel waveform.
 **/
class MP_Anywave_Atom_Plugin_c: public MP_Atom_c {
	
	/********/
	/* DATA */
	/********/
	
public:
	/** \brief Index of the anywave table
	 *
	 * Index of the anywave table that contains the waveform, in the global anywave server
	 * MP_GLOBAL_ANYWAVE_SERVER
	 */
	unsigned long int tableIdx;
	/** \brief pointer to the anywave table
	 *
	 * it is NULL if no anywave table is associated to the atom. Otherwise,
	 * it points to a anywave table, generally stored in the global
	 * anywave server MP_GLOBAL_ANYWAVE_SERVER.
	 */
	MP_Anywave_Table_c* anywaveTable;
	/** \brief Index of the waveform in the anywave table
	 */
	unsigned long int anywaveIdx;
	
	/***********/
	/* METHODS */
	/***********/
	
	/***************************/
	/* CONSTRUCTORS/DESTRUCTOR */
	/***************************/
	
public:
	
	/** \brief Specific factory function
	 **/
	static MP_Atom_c  * anywave_atom_create_empty(void);
	
	/** \brief File factory function
	 *
	 * construct the atom from a stream, in general when reading a
	 * book. The stream shall follow the syntax of books, e.g. for text
	 * formatted stream :
	 *
	 * - \<par type="filename"\>./anywaveTable.bin\</par\>
	 * - \<par type="waveIdx"\>13\</par\>
	 * - \<anywavePar chan = "1"\>
	 *   - \<par type="amp"\>12.7\</par\>
	 * - \</anywavePar\>
	 * - ...
	 *
	 * \param fid the stream 
	 *
	 * \param mode MP_TEXT or MP_BINARY
	 **/
	static MP_Atom_c* create( FILE *fid, const char mode );
	
	/** \brief File reader */
	virtual int read( FILE *fid, const char mode );
	
	
	
protected:
	
	/** \brief Default void constructor 
	 *
	 * set the amp array and anywaveTable to NULL, anywaveIdx and
	 * tableIdx to zero, and the other members are initialized by the
	 * default MP_Atom_c constructor.
	 *
	 **/
	MP_Anywave_Atom_Plugin_c( void );
	
	
	
public:
	
	/** \brief Default destructor 
	 **/
	virtual ~MP_Anywave_Atom_Plugin_c( void );
	
	
	/***************************/
	/* OUTPUT METHOD           */
	/***************************/
	
	/** \brief Write the atom to the stream
	 *
	 * write the atom to the stream, in general when writing a book. The
	 * syntax, for text formatted stream, is like in the following
	 * example :
	 *
	 * - \<par type="filename"\>./anywaveTable.bin\</par\>
	 * - \<par type="waveIdx"\>13\</par\>
	 * - \<anywavePar chan = "1"\>
	 *   - \<par type="amp"\>12.7\</par\>
	 * - \</anywavePar\>
	 * - ...
	 *
	 * \param fid the stream
	 *
	 * \param mode MP_TEXT or MP_BINARY
	 **/
	virtual int write( FILE *fid, const char mode );
	
	/***************************/
	/* OTHER METHODS           */
	/***************************/
	
	/** \brief Test function
	 *
	 * called by the test executable test_anywave
	 *
	 * \todo explain what is done
	 **/
	static bool test( char* filename );
	
	/** \brief returns the type of the atom : "anywave"
	 * \return the string "anywave"
	 **/
	virtual const char * type_name(void);
	
	/** \brief Print the details of the anywave atom to the stream
	 * \param fid the stream
	 **/
	virtual int info( FILE *fid );
	virtual int info();
	
	/** \brief Build the waveform of an anywave atom. 
	 *
	 * if the waveform has only one channel, then, for each channel,
	 * copy this monochannel waveform to outBuffer, multiplied by the
	 * amplitude of the channel
	 * 
	 * if the waveform has as many channels as the signal, then, for
	 * each channel \a chanIdx, copy the channel \a chanIdx of the
	 * waveform, multiplied by the amplitude of the channel \a
	 * amp[chanIdx].
	 *
	 * \param outBuffer the buffer containing the samples of the output
	 * waveform, channel after channel
	 **/
	virtual void build_waveform( MP_Real_t *outBuffer );
	virtual void build_waveform_norm( MP_Real_t *outBuffer );
	
	/** \brief NOT IMPLEMENTED
	 **/
	virtual int add_to_tfmap( MP_TF_Map_c *tfmap, const char tfmapType );
	
	/** \brief Get the identifying parameters of the atom inside a block to use with sorted books
	 *	
	 * \return a pointer to a map containing the parameters */
	virtual MP_Atom_Param_c* get_atom_param( void )const;
	
	
	/** \brief Check if the atom has the field
	 *
	 * The following fields will return MP_TRUE in this class, in
	 * addition to those of the class MP_Atom_c :
	 * - MP_TABLE_IDX_PROP (tableIdx)
	 * - MP_ANYWAVE_TABLE_PROP (anywaveTable)
	 * - MP_ANYWAVE_IDX_PROP (anywaveIdx)
	 * - MP_AMP_PROP (amp)
	 *
	 * \param field the field to check, eg, MP_TABLE_IDX_PROP for the
	 * member tableIdx
	 *
	 * \return MP_TRUE if the field exists, MP_FALSE else
	 **/
	virtual int       has_field( int field );
	
	/** \brief Get the value of the field \a field on the channel \a chanIdx
	 *
	 * If the atom has the field, return its value on the channel
	 * chanIdx. For example, get_field( MP_AMP_PROP, 1 ) returns the
	 * amplitude on the second channel.
	 * 
	 * The available fields for the get_value() function are :
	 * - MP_TABLE_IDX_PROP (tableIdx)
	 * - MP_ANYWAVE_IDX_PROP (anywaveIdx)
	 * - MP_AMP_PROP (amp)
	 *
	 * \param field the field 
	 *
	 * \param chanIdx the index of the channel
	 *
	 * \return the value of the field on the specified channel
	 **/
	virtual MP_Real_t get_field( int field , MP_Chan_t chanIdx );
	
private:
	/** \brief Read the string containing the filename in the line, and copy it to str
	 *
	 * \param line the string line from the file
	 *
	 * \param pattern the pattern preceding the string containing the filename, eg, "\t\t\<par type=\"filename\"\>%s"
	 *
	 * \param str the string containing the filename
	 *
	 * \return true for success, and false for failure
	 **/
	static bool read_filename_txt( const char* line, const char* pattern, char* str);
	
	/** \brief Read the string containing the filename in the stream, and copy it to str
	 *
	 * \param fid the stream
	 *
	 * \param str the string containing the filename
	 *
	 * \return true for success, and false for failure
	 **/
	static bool read_filename_bin( FILE* fid, char* str);
	
};

#endif /* __anywave_atom_plugin_h_ */
