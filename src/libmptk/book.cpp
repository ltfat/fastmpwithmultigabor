/******************************************************************************/
/*                                                                            */
/*                                 book.cpp                                   */
/*                                                                            */
/*                        Matching Pursuit Library                            */
/*                                                                            */
/* RÈmi Gribonval                                                             */
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
/*
 * SVN log:
 *
 * $Author: broy $
 * $Date: 2007-09-14 17:37:25 +0200 (ven., 14 sept. 2007) $
 * $Revision: 1151 $
 *
 */

/******************************************/
/*                                        */
/* book.cpp: methods for class MP_Book_c  */
/*                                        */
/******************************************/

#include "mptk.h"
#include "mp_system.h"

/***************************/
/* CONSTRUCTORS/DESTRUCTOR */
/***************************/

/******************/
/* Constructor    */
MP_Book_c* MP_Book_c::create() 
{
	const char* func = "MP_Book_c::create()";
	MP_Book_c *newBook = NULL;

	// Instantiate and check
	newBook = new MP_Book_c();
	if ( newBook == NULL ) 
	{
		mp_error_msg( func, "Failed to create a new book.\n" );
		return( NULL );
	}

	// Allocate the atom array
	if ( (newBook->atom = (MP_Atom_c**) calloc( MP_BOOK_GRANULARITY, sizeof(MP_Atom_c*) )) == NULL ) 
	{
		mp_warning_msg( func, "Can't allocate storage space for [%lu] atoms in the new book. The atom array is left un-initialized.\n", MP_BOOK_GRANULARITY );
		newBook->atom = NULL;
		newBook->maxNumAtoms = 0;
	}
	else 
		newBook->maxNumAtoms = MP_BOOK_GRANULARITY;

	return( newBook );
}

MP_Book_c* MP_Book_c::create(MP_Chan_t setNumChans, unsigned long int setNumSamples, int setSampleRate ) {

const char* func = "MP_Book_c::create(MP_Chan_t numChans, unsigned long int numSamples, unsigned long int numAtoms )";
MP_Book_c *newBook = NULL;

/* Instantiate and check */
  newBook = create();
  if ( newBook == NULL ) {
    mp_error_msg( func, "Failed to create a new book.\n" );
    return( NULL );
  }
  newBook->numChans = setNumChans;
  newBook->numSamples = setNumSamples;
  newBook->sampleRate = setSampleRate;
  
 return( newBook );
}
/********************************************************/
/* Load from a stream, either in ascii or binary format */
MP_Book_c* MP_Book_c::create( FILE *fid ) 
{
  const char* func = "MP_Book_c::create(fid)";
  MP_Book_c *newBook = create();
  if(!newBook->load(fid, true))
  {
    mp_error_msg( func, "Created new book, but failed to load data into it.\n" );
    return( NULL );
  }
  return( newBook );
}
/***********************/
/* NULL constructor    */
MP_Book_c::MP_Book_c() 
{
  numAtoms    = 0;
  numChans    = 0;
  numSamples  = 0;
  sampleRate  = 0;
  atom = NULL;
  maxNumAtoms = 0;
}

/**************/
/* Destructor */
MP_Book_c::~MP_Book_c() {
	unsigned long int iIndex;
	mp_debug_msg( MP_DEBUG_DESTRUCTION, "MP_Book_c::~MP_Book_c()", "Deleting book...\n" );
	if(atom)
	{
		for(iIndex=0; iIndex<numAtoms; iIndex++)
			if(atom[iIndex])
				delete atom[iIndex];
		free(atom); 
		atom = NULL;
	}
	mp_debug_msg( MP_DEBUG_DESTRUCTION, "MP_Book_c::~MP_Book_c()", "Done.\n" );
}



/***************************/
/* I/O METHODS             */
/***************************/

/********************************/
/* Print some atoms to a stream */
unsigned long int MP_Book_c::printDict( const char *fName, FILE *fid) 
{
	const char			*func = "MP_Book_c::printDict(fid,mask)";
	unsigned long int	nAtom = 0;

	if(this->numAtoms==0)
	{
		mp_error_msg( func, "Error writing the dict file - cannot, since no atoms.\n" );
		return 0;
	}

	if(fName)
		this->atom[0]->dict->write_to_xml_file(fName); 
	else if(fid)
		this->atom[0]->dict->write_to_xml_file(fid, false);
	else
	{
		mp_error_msg( func, "Error writing the dict file - both fName and fid are null.\n" );
		return 0;
	}

	return( nAtom );
}

/********************************/
/* Print some atoms to a stream - note that this does not do the dict, just the inner <book> - print() does the full XML */
unsigned long int MP_Book_c::printBook( FILE *fid , const char mode, MP_Mask_c* mask) 
{
	//const char			*func = "MP_Book_c::printBook(fid,mask)";
	unsigned long int nAtom = 0;
	unsigned long int i;

	// determine how many atoms the printed book will contain
	if ( mask == NULL ) 
		nAtom = numAtoms;
	else 
	{
		for (i=0; i<numAtoms; i++)
			if (mask->sieve[i]) 
				nAtom++;
	}

	if(!printBook_opening(fid, mode, nAtom)){
		return 0;
	}
	if((nAtom = printBook_atoms(fid, mode, mask, nAtom))==0){
		return 0;
	}
	if(!printBook_closing(fid)){
		return 0;
	}
	return( nAtom );
}


bool MP_Book_c::printBook_opening( FILE *fid , const char mode, unsigned long int nAtom){
	// Print the book header, inc format
	const char			*func = "MP_Book_c::printBook_opening()";
	const char* formatString;
	switch(mode)
	{
		case MP_TEXT:
			formatString = "txt";
			break;
		case MP_BINARY:
			formatString = "bin";
			break;
		default:
			mp_error_msg( func, "Unknown write mode.\n" );
			return false;
	}
	fprintf( fid, "<book nAtom=\"%lu\" numChans=\"%d\" numSamples=\"%lu\" sampleRate=\"%d\" libVersion=\"%s\" format=\"%s\">\n", 
			nAtom, numChans, numSamples, sampleRate, VERSION, formatString );
	return true;
}

bool MP_Book_c::printBook_closing( FILE *fid){
	fprintf( fid, "</book>\n"); 
	return true;
}

unsigned long int MP_Book_c::printBook_atoms( FILE *fid , const char mode, MP_Mask_c* mask, unsigned long int nAtom){
	const char			*func = "MP_Book_c::printBook_atoms()";
	unsigned long int i;
	for ( i = 0, nAtom = 0; i < numAtoms; i++ ) 
	{
		if ( (mask==NULL) || (mask->sieve[i]) )
		{
			if ( mode == MP_TEXT ) 
			{
				fprintf( fid, "\t<atom type=\"");
				fprintf( fid, "%s", atom[i]->type_name() );
				fprintf( fid, "\">\n" );
				// Call the atom's write function
				atom[i]->write( fid, mode );
				fprintf( fid, "\t</atom>\n" );
			}
			else if( mode == MP_BINARY ) 
			{
				fprintf( fid, "%s\n", atom[i]->type_name() );
				// Call the atom's write function
				atom[i]->write( fid, mode );
			} 
			else 
				mp_error_msg( func, "Unknown write mode for Atom, Atom is skipped." );

			nAtom++;
		}
	}
	return nAtom;
}	


/******************************/
/* Print some atoms to a file */
unsigned long int MP_Book_c::print( const char *fName , const char mode, MP_Mask_c* mask) 
{
	FILE				*fid;
	unsigned long int	nAtom = 0;

	if ( ( fid = fopen( fName, "wb" ) ) == NULL ) 
	{
		mp_error_msg( "MP_Book_c::print(fname,mask)","Could not open file %s to print a book.\n", fName );
		return( 0 );
	}
	nAtom = print( fid, mode, mask);
	fclose( fid );
	return ( nAtom );
}

/******************************/
/* Print some atoms to a file */
unsigned long int MP_Book_c::print( FILE *fid , const char mode, MP_Mask_c* mask) 
{
	unsigned long int	nAtom = 0;
	fprintf( fid, "<?xml version=\"1.0\" encoding=\"ISO-8859-1\" ?>\n"); 
	fprintf( fid, "<mptkbook formatVersion=\"1\">\n");
	printDict( NULL, fid);
	nAtom = printBook( fid, mode, mask );
	fprintf( fid, "</mptkbook>\n"); 
	return ( nAtom );
}

/***********************************/
/* Print all the atoms to a stream */
unsigned long int MP_Book_c::print( FILE *fid, const char mode ) {
	return( print( fid, mode, NULL));
}


/***********************************/
/* Print all the atoms to a file   */
unsigned long int MP_Book_c::print( const char *fName, const char mode ) {
	return( print( fName, mode, NULL) );
}


/********************************************************/
/* Load from a stream, either in ascii or binary format */
unsigned long int MP_Book_c::load( FILE *fid )
{
	return load(fid, true);
}
unsigned long int MP_Book_c::load( FILE *fid, bool withDict )
{
	const char				*func = "MP_Book_c::load(fid, bool)";
	int formatVersion; // Different variations of the XML stream format
	unsigned int			fidNumChans;
	int						fidSampleRate;
	unsigned long int		i, fidNumAtoms, fidNumSamples;
	unsigned long int		nRead = 0;
	char					mode;
	char					line[MP_MAX_STR_LEN];
	memset(line, 0, MP_MAX_STR_LEN);
	char					str[MP_MAX_STR_LEN];
	memset(str, 0, MP_MAX_STR_LEN);
	MP_Atom_c				*newAtom = NULL;
	MP_Dict_c				*dict;

	// xml declaration
	if ( fgets( line,MP_MAX_STR_LEN,fid) == NULL )
	{
		mp_error_msg( func, "Cannot scan the first line of the mptkbook. This book will remain un-changed.\n" );
		return( 0 );
	}
	if(strstr(line, "<?xml version=\"1.0\" encoding=\"ISO-8859-1\" ?>")==NULL){
		printf("%s -- warning, first line is not the expected XML declaration: %s\n", func, line);
	}
	// check if the <mptkbook> tag is present -- if so, read libversion -- if not, rewind the stream (!) and drop back to the old ways
	if ( fgets( line,MP_MAX_STR_LEN,fid) == NULL )
	{
		mp_error_msg( func, "Cannot scan the second line of the mptkbook. This book will remain un-changed.\n" );
		return( 0 );
	}
	if (sscanf( line, "<mptkbook formatVersion=\"%i\">\n", &formatVersion ) != 1 )
	{
		// Not an error - but we must rewind the stream and drop back to the old ways
		formatVersion = 0;
		rewind(fid);
	}

	if(withDict){
		// Retrieve the dictionary
		if((dict = MP_Dict_c::read_from_xml_file( fid )) == NULL)
		{
			mp_error_msg( func, "Failed to create a dictionary from XML stdin.\n");
			// TODO LIB2RER DICT
			return  1;
		}
	}	

	char fidFormatStr[3];
	switch(formatVersion){
		case 1:
			// Read the header
			if ( ( fgets( line,MP_MAX_STR_LEN,fid) == NULL ) || (sscanf( line, "<book nAtom=\"%lu\" numChans=\"%d\" numSamples=\"%lu\" sampleRate=\"%d\" libVersion=\"%[0-9a-z.]\" format=\"%[0-9a-z.]\">\n", &fidNumAtoms, &fidNumChans, &fidNumSamples, &fidSampleRate, str, fidFormatStr ) != 6 )) 
			{
			        printf("%s",line);
				mp_error_msg( func, "Cannot scan the book header. This book will remain un-changed.\n" );
				return( 0 );
			}
			if(!strcmp(fidFormatStr,"bin"))
				mode = MP_BINARY;
			else if(!strcmp(fidFormatStr,"txt"))
				mode = MP_TEXT;
			else
			{
				mp_error_msg( func, "Unknown format string \"%s\". This book will remain un-changed.\n", fidFormatStr );
				return 0;	
			}
			break;
		case 0:
			if ( fgets( line,MP_MAX_STR_LEN,fid) == NULL ) 
			{
				mp_error_msg( func, "Cannot get the format line. This book will remain un-changed.\n" );
				return 0;	
			}
			if(!strcmp(line,"bin\n"))
				mode = MP_BINARY;
			if(!strcmp(line,"txt\n"))
				mode = MP_TEXT;
			// Read the header
			if ( ( fgets( line,MP_MAX_STR_LEN,fid) == NULL ) || (sscanf( line, "<book nAtom=\"%lu\" numChans=\"%d\" numSamples=\"%lu\" sampleRate=\"%d\" libVersion=\"%[0-9a-z.]\">\n", &fidNumAtoms, &fidNumChans, &fidNumSamples, &fidSampleRate, str ) != 5 )) 
			{
				mp_error_msg( func, "Cannot scan the book header. This book will remain un-changed.\n" );
				return( 0 );
			}
			break;
		default:
			mp_error_msg( func, "Unknown <mptkbook> formatVersion: %i\n", formatVersion);
			return 1;
			break;
	}


	// Read the atoms
	if ( mode == MP_TEXT){  // using if rather than switch, so can declare variables inside
		MP_Atom_c* (*createAtomFromXml)( TiXmlElement *xmlobj, MP_Dict_c *dict);
		// read some xml into memory
		size_t xmlBufSize = 100000;
		char szBuffer[xmlBufSize];
		size_t bytesremain;
		// Reset the buffer for each atom
		bytesremain = xmlBufSize;
		memset(szBuffer, 0, xmlBufSize);
		do
		{
			// Then attempt to read one <atom>...</atom> block
			if ( fgets( line, MP_MAX_STR_LEN, fid) == NULL ) 
			{
				mp_error_msg( func, "Error reading XML data from file.\n" );
				return 0;
			}
			if ( strlen(line) > bytesremain )
			{
				mp_error_msg( func, "XML data for <atom> is larger than the in-memory buffer (size %lu bytes), cannot load.\n", xmlBufSize);
				return 0;
			}
			strcat(szBuffer,line);
			bytesremain -= strlen(line);

			// If we've ended an atom, then parse the atom xml
			// NB this is still not quite "proper" XML parsing, since wants </atom> on own line, but streaming is important
			if(strstr(line,"</atom>")){
				TiXmlDocument doc;
				if (!doc.Parse(szBuffer))
				{
					mp_error_msg( func, "Error while loading the XML <atom> data into tinyxml:\n");
					mp_error_msg( func, "Error ID: %u .\n", doc.ErrorId() );
					mp_error_msg( func, "Error description: %s .\n", doc.ErrorDesc());
					return  0;
				}
				// Get a handle on the document
				TiXmlHandle hdl(&doc);
				// Get a handle on the tag "atom"
				TiXmlElement *atomElement = hdl.FirstChildElement("atom").Element();
				if (atomElement == NULL)
				{
					mp_error_msg( func, "Error, cannot find the xml property :\"atom\".\n");
					return 0;
				}

				const char* str_type = atomElement->Attribute("type");
				if(str_type == NULL){
					mp_error_msg( func, "Cannot find 'type' attribute in the <atom> XML.\n" );
					return 0;
				}
				// Scan the hash map to get the create function of the atom
				createAtomFromXml = MP_Atom_Factory_c::get_atom_fromxml_creator( str_type );
				// Scan the hash map to get the create function of the atom
				if ( NULL != createAtomFromXml ){
					// Create the the atom
					newAtom = (*createAtomFromXml)(atomElement, dict);
				}else{
					mp_error_msg( func, "Cannot read atoms of type '%s'\n",str_type);
				}
				if ( newAtom == NULL )  
					mp_error_msg( func, "Failed to create an atom of type[%s].\n", str);
				else 
				{ 
					append( newAtom ); 
					++nRead;
				}
				// Reset the text buffer for each atom
				bytesremain = xmlBufSize;
				memset(szBuffer, 0, xmlBufSize);
			}
		}
		while(strstr(line,"</book>") == 0);  // NB this is still not quite "proper" XML parsing, since wants </book> on own line, but streaming is important

	} else if ( mode == MP_BINARY){
		MP_Atom_c* (*createAtomFromBinary)( FILE *fid, MP_Dict_c *dict);
		for ( i=0; i<fidNumAtoms; i++ )
		{
			if ( (  fgets( line, MP_MAX_STR_LEN, fid ) == NULL ) ||( sscanf( line, "%[a-z]\n", str ) != 1 ) ) 
			{
				mp_error_msg( func, "Cannot scan the atom type (in binary mode).\n");
				return 0;
			}
			// Scan the hash map to get the create function of the atom
			createAtomFromBinary = MP_Atom_Factory_c::get_atom_frombinary_creator( str );
			// Scan the hash map to get the create function of the atom
			if ( NULL != createAtomFromBinary) 
				// Create the the atom
				newAtom = (*createAtomFromBinary)(fid, dict);
			else 
				mp_error_msg( func, "Cannot read atoms of type '%s'\n",str);
	
			if ( newAtom == NULL )  
				mp_error_msg( func, "Failed to create an atom of type[%s].\n", str);
			else 
			{ 
				append( newAtom ); 
				++nRead;
			}
		}
	}else{
		mp_error_msg( func, "Unknown read mode in read_atom().\n");
		return 0;
	}

	// Check the global data fields
	if ( numChans != fidNumChans ) 
		mp_warning_msg( func, "The book object contains a number of channels [%i] different from the one read in the stream [%i].\n",numChans, fidNumChans );

	if ( numSamples != fidNumSamples ) 
	{
		mp_warning_msg( func, "The book object contains a number of samples [%lu] different from the one read in the stream [%lu].\n",numSamples, fidNumSamples );
		if(numSamples < fidNumSamples) 
		{
			mp_warning_msg(func, "The book.numSamples has been set to match the stream numSamples\n");
			mp_warning_msg(func, "This is a new behaviour in MPTK 0.5.6 which will become standard\n");
			numSamples = fidNumSamples;		
		} 
		else 
			mp_error_msg(func,"This is very weired, please check your book file\n");
	}

	if ( (sampleRate != 0) && (sampleRate != fidSampleRate) ) 
		mp_warning_msg( func, "The book object contains a sample rate [%i] different from the one read in the stream [%i]. Keeping the new sample rate [%i].\n",sampleRate, fidSampleRate, fidSampleRate );

	sampleRate = fidSampleRate;

  return( nRead );
}


/********************/
/* Load from a file */
unsigned long int MP_Book_c::load( const char *fName ) 
{
	const char				*func = "MP_Book_c::load(const char *fName)";
	FILE					*fid;
	// Open the book file
	if ( ( fid = fopen(fName,"rb") ) == NULL ) 
	{
		mp_error_msg( func,"Could not open file %s.\n", fName );
		return( 0 );
	}
	unsigned long int nRead =  load( fid );
	fclose( fid );
	return( nRead );
}


/***************************************************************/
/* Print human readable information about the book to a stream */
int MP_Book_c::info( FILE *fid ) {
  
  int nChar = 0;
  FILE* bakStream;
  void (*bakHandler)( void );

  /* Backup the current stream/handler */
  bakStream = (FILE*) get_info_stream();
  bakHandler = get_info_handler();
  /* Redirect to the given file */
  set_info_stream( fid );
  set_info_handler( MP_FLUSH );
  /* Launch the info output */
  nChar += info();
  /* Reset to the previous stream/handler */
  set_info_stream( bakStream );
  set_info_handler( bakHandler );

  return( nChar );
}


/*******************************************************************************/
/* Print human readable information about the book to the default info handler */
int MP_Book_c::info() {
  
  unsigned long int i;
  int nChar = 0;

  nChar += (int)mp_info_msg( "BOOK", "Number of atoms              =[%lu]  (Current atom array size =[%lu])\n", numAtoms, maxNumAtoms );
  nChar += (int)mp_info_msg( "  |-", "Number of channels           =[%d]\n",    numChans );
  nChar += (int)mp_info_msg( "  |-", "Number of samples per channel=[%lu]\n",   numSamples );
  nChar += (int)mp_info_msg( "  |-", "Sampling rate                =[%d]\n",    sampleRate );
  for ( i=0; i<numAtoms; i++ ) {
    nChar += (int)mp_info_msg( "  |-", "--ATOM [%lu/%lu] info :\n", i+1, numAtoms );
    atom[i]->info();
  }
  nChar += (int)mp_info_msg( "  O-", "End of book.\n",    sampleRate );

  return( nChar );
}


/*******************************************************************************/
/* Print human readable information about the book to the default info handler */
int MP_Book_c::short_info() {
  
  int nChar = 0;

  nChar += (int)mp_info_msg( "BOOK", "[%lu] atoms (current atom array size = [%lu])\n", numAtoms, maxNumAtoms );
  nChar += (int)mp_info_msg( "  |-", "[%lu] samples on [%d] channels; sample rate [%d]Hz.\n", numSamples, numChans, sampleRate );
  return( nChar );
}



/***************************/
/* MISC METHODS            */
/***************************/

/***********************/
/* Clear all the atoms */
void MP_Book_c::reset( void ) {

  unsigned long int i;

  if ( atom ) {
    for ( i=0; i<numAtoms; i++ ) {
      if ( atom[i] ) delete( atom[i] );
    }
  }
  numAtoms = 0;

}

/******************/
/* Replace an atom */
int MP_Book_c::replace( MP_Atom_c *newAtom, int atomIndex ) {
	
	//const char* func = "MP_Book_c::replace(index, *atom)";
	unsigned long int newLen;
	MP_Chan_t numChansAtom;
	
	/* Hook the passed atom */
	atom[atomIndex] = newAtom;
	
	/* Set the number of channels to the max among all the atoms */
	numChansAtom = newAtom->numChans;
	if ( numChans < numChansAtom ) numChans = numChansAtom;
	
	/* Rectify the numSamples if needed */
	newLen = newAtom->numSamples;
	if ( numSamples < newLen ) numSamples = newLen;
	
	return( 1 ); 	
}

/******************/
/* Append an atom */
int MP_Book_c::append( MP_Atom_c *newAtom ) {

  const char* func = "MP_Book_c::append(*atom)";
  unsigned long int newLen;
  MP_Chan_t numChansAtom;

  /* If the passed atom is NULL, silently ignore (but return 0 as the number of added atoms) */
  if( newAtom == NULL ) return( 0 );
  /* Else: */
  else {

    /* Re-allocate if the max storage capacity is attained for the list: */
    if (numAtoms == maxNumAtoms) {
      MP_Atom_c **tmp;
      /* re-allocate the atom array */
      if ( (tmp = (MP_Atom_c**) realloc( atom, (maxNumAtoms+MP_BOOK_GRANULARITY)*sizeof(MP_Atom_c*) )) == NULL ) {
	mp_error_msg( func, "Can't allocate space for [%d] more atoms."
		      " The book is left untouched, the passed atom is not saved.\n",
		      MP_BOOK_GRANULARITY );
	return( 0 );
      }
      else {
	atom = tmp;
	maxNumAtoms += MP_BOOK_GRANULARITY;
      }
    }
    
    /* Hook the passed atom */
    atom[numAtoms] = newAtom;
    numAtoms++;
    
    /* Set the number of channels to the max among all the atoms */
    numChansAtom = newAtom->numChans;
    if ( numChans < numChansAtom ) numChans = numChansAtom;
    
    /* Rectify the numSamples if needed */
    newLen = newAtom->numSamples;
    if ( numSamples < newLen ) numSamples = newLen;
  }

  return( 1 );
}
/******************/
/* Append a book  */
unsigned long int MP_Book_c::append( MP_Book_c *newBook ) {
	const char* func = "MP_Book_c::append(*book)";
	unsigned long int nAppend = 0;
//	MP_Dict_c* newBookDict = newBook->atom[0]->dict;
	if (is_compatible_with(newBook)){
/* ??? is this the right idea?
		// append the dictionary
		for(unsigned int blk=0; blk < newBookDict->numBlocks; ++blk){
			if(atom[0]->dict->add_block( newBookDict->block[blk] )==0){
				mp_error_msg( func, "Unable to append blocks from other dictionary to this one.\n");
				return (0);
			}
		}
*/		// append the found atoms
		for (unsigned long int i = 0 ; i< newBook->numAtoms; i++){
			if (append( newBook->atom[i] ) ){
				++nAppend;
			}
		}
		return (nAppend);
	}
	else {
		mp_error_msg( func, "Books do not have the same parameters - not appending.\n");
		return (0);
	}
}

/***********************************/
/* Re-check the number of samples  */
int MP_Book_c::recheck_num_samples() {
  unsigned long int i;
  unsigned long int checkedNumSamples = 0;
  MP_Bool_t ret = MP_TRUE;

  for ( i = 0; i < numAtoms; i++ ) {
    if ( checkedNumSamples < atom[i]->numSamples ) checkedNumSamples = atom[i]->numSamples;
  }
  ret = ( checkedNumSamples == numSamples );
  numSamples = checkedNumSamples;

  return( ret );
}


/***********************************/
/* Re-check the number of channels */
int MP_Book_c::recheck_num_channels() {
  unsigned long int i;
  MP_Chan_t checkedNumChans = 0;
  MP_Bool_t ret = MP_TRUE;

  for ( i = 0; i < numAtoms; i++ ) {
    if ( checkedNumChans < atom[i]->numChans ) checkedNumChans = atom[i]->numChans;
  }
  ret = ( checkedNumChans == numChans );
  numChans = checkedNumChans;

  return( ret );
}


/***************************************************************/
/* Substract or add the sum of (some) atoms from / to a signal */
unsigned long int MP_Book_c::substract_add( MP_Signal_c *sigSub, MP_Signal_c *sigAdd, MP_Mask_c* mask ) {

  unsigned long int i;
  unsigned long int n = 0;
  
  if (mask == NULL) {
    for (i = 0; i < numAtoms; i++) atom[i]->substract_add(sigSub,sigAdd);
    n = numAtoms;
  }
  else {
    for (i = 0; i < numAtoms; i++) {
      if ( mask->sieve[i] ) { atom[i]->substract_add(sigSub,sigAdd); n++; }
    }
  }
  return( n );
}

/******************************************************/
/* Adds the sum of the pseudo Wigner-Ville distributions
   of some atoms to a time-frequency map */
unsigned long int MP_Book_c::add_to_tfmap(MP_TF_Map_c *tfmap, const char tfmapType, MP_Mask_c* mask ) {

  unsigned long int i;
  unsigned long int n = 0;
  
  if (mask == NULL) {
    for (i = 0; i < numAtoms; i++) atom[i]->add_to_tfmap( tfmap, tfmapType );
    n = numAtoms;
  }
  else {
    for (i = 0; i < numAtoms; i++) {
      if ( mask->sieve[i] ) { atom[i]->add_to_tfmap( tfmap, tfmapType ); n++; }
    }
  }
  return( n );
}

/******************************************************/
/*  Returns the atom which is the closest to a given 
 *  time-frequency location, as well as its index in the book->atom[] array
 */
MP_Atom_c* MP_Book_c::get_closest_atom(MP_Real_t time, MP_Real_t freq,
				       MP_Chan_t chanIdx, MP_Mask_c* mask,
				       unsigned long int *nClosest ) {

  unsigned long int i;
  MP_Atom_c *atomClosest = NULL;
  MP_Real_t dist, distClosest;
  
  //distClosest = 1e700;
  distClosest = 1.7e308;
  if (mask == NULL) {
    for (i = 0; i < numAtoms; i++) {
      dist = atom[i]->dist_to_tfpoint( time, freq , chanIdx );
      if (NULL == atomClosest || dist < distClosest) {
	atomClosest = atom[i];
	*nClosest = i;
	distClosest = dist;
      }
    }
  }
  else {
    for (i = 0; i < numAtoms; i++) {
      if ( mask->sieve[i] ) { 
	dist = atom[i]->dist_to_tfpoint( time, freq , chanIdx );
	if (NULL == atomClosest || dist < distClosest) {
	  atomClosest = atom[i];
	  *nClosest = i;
	  distClosest = dist;
	}
      }
    }
  }
  return( atomClosest );
}
MP_Bool_t MP_Book_c::can_append( FILE * fid ){
	
  const char* func = "MP_Book_c::can_append(fid)";
  unsigned int fidNumChans;
  int fidSampleRate;
  unsigned long int fidNumAtoms, fidNumSamples;
  char mode;
  char line[MP_MAX_STR_LEN];
  char str[MP_MAX_STR_LEN];
    /* Read the format */
  if ( fgets( line,MP_MAX_STR_LEN,fid) == NULL ) {
    mp_error_msg( func, "Cannot get the format line. This book will remain un-changed.\n" );
    fseek ( fid , 0L , SEEK_SET );
    return( false );
  }

  /* Try to determine the format */
  if      ( !strcmp( line, "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n" ) ) mode = MP_TEXT;
  else if ( !strcmp( line, "bin\n" ) ) mode = MP_BINARY;
  else {
    mp_error_msg( func, "The loaded book has an erroneous file format."
		  " This book will remain un-changed.\n" );
    fseek ( fid , 0L , SEEK_SET );
    return( false );
  }

  /* Read the header */
  if ( ( fgets( line,MP_MAX_STR_LEN,fid) == NULL ) ||
       (sscanf( line, "<book nAtom=\"%lu\" numChans=\"%d\" numSamples=\"%lu\""
		" sampleRate=\"%d\" libVersion=\"%[0-9a-z.]\">\n",
		&fidNumAtoms, &fidNumChans, &fidNumSamples, &fidSampleRate, str ) != 5 )
       ) {
    mp_error_msg( func, "Cannot scan the book header. This book will remain un-changed.\n" );
    fseek ( fid , 0L , SEEK_SET );
    return( false );
  } else
  /* test compatibility */
  if ( ((sampleRate != 0) && (sampleRate == fidSampleRate)) && (( numChans != 0 ) && ( numChans == fidNumChans )) && ((numSamples != 0) && (numSamples == fidNumSamples)) ) {
  fseek ( fid , 0L , SEEK_SET );
  return( true );
  } else return false;
}
/***********************************/
/* Check compatibility with a mask */
MP_Bool_t MP_Book_c::is_compatible_with( MP_Mask_c mask ) {
  return( numAtoms == mask.numAtoms );
}

/*****************************************/
/* Check compatibility betwenn two books */
MP_Bool_t MP_Book_c::is_compatible_with( MP_Book_c *book ){
   return ( (numChans == book->numChans) && (numSamples == book->numSamples) && (sampleRate == book->sampleRate) );
}

/***************************************/
/* Check compatibility with parameters */
MP_Bool_t MP_Book_c::is_compatible_with( MP_Chan_t testedNumChans, int testedSampleRate ){
return ( (numChans ==  testedNumChans) && (sampleRate == testedSampleRate ) );

}

/***********************************/
/* Check compatibility with signal */
MP_Bool_t MP_Book_c::is_compatible_with( MP_Signal_c *sig ){
 return( (numChans == sig->numChans)  && (sampleRate == sig->sampleRate) );
 // && (numSamples == sig->numSamples)
}



