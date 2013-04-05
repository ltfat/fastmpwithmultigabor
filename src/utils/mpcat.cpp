/******************************************************************************/
/*                                                                            */
/*                                mpcat.cpp                                   */
/*                                                                            */
/*                        Matching Pursuit Utilities                          */
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
 * $Date: 2007-07-13 16:24:32 +0200 (Fri, 13 Jul 2007) $
 * $Revision: 1112 $
 *
 */

#include "mptk.h"
#include "libgetopt/getopt.h"

const char* func = "mpcat";
const size_t maxNumBooks = 64; // hard-coded for now. to revisit.


/********************/
/* Global constants */
/********************/
#define MPC_TRUE  (1==1)
#define MPC_FALSE (0==1)

/********************/
/* Error types      */
/********************/
#define ERR_ARG        1
#define ERR_BOOK       2
#define ERR_RES        3
#define ERR_SIG        4
#define ERR_BUILD      5
#define ERR_WRITE      6
#define ERR_LOADENV    7

/********************/
/* Global variables */
/********************/
int MPC_QUIET      = MPC_FALSE;
int MPC_VERBOSE    = MPC_FALSE;
int MPC_FORCE      = MPC_FALSE;

/* Input/output file names: */
char *bookOutFileName = NULL;
char *bookInFileName  = NULL;
const char *configFileName = NULL;


/**************************************************/
/* HELP FUNCTION                                  */
/**************************************************/
void usage( void ) 
{
	fprintf( stdout, " \n" );
	fprintf( stdout, " Usage:\n" );
	fprintf( stdout, "     mpcat [options] (book1.bin|-) (book2.bin|-) ... (bookN.bin|-) (bookOut.bin|-)\n" );
	fprintf( stdout, " \n" );
	fprintf( stdout, " Synopsis:\n" );
	fprintf( stdout, "     Concatenates the N books book1.bin...bookN.bin into the book file bookOut.bin.\n" );
	fprintf( stdout, " \n" );
	fprintf( stdout, " Main arguments:\n" );
	fprintf( stdout, "     (bookN.bin|-)        At least 2 books (or stdin) to concatenate.\n" );
	fprintf( stdout, "     (bookOut.bin|-)      A book where to store the concatenated books, or stdout\n" );
	fprintf( stdout, " \n" );
	fprintf( stdout, " Optional arguments:\n" );
	fprintf( stdout, "     -C<FILE>, --config-file=<FILE>  Use the specified configuration file, otherwise MPTK_CONFIG_FILENAME\n" );
	fprintf( stdout, "     -f, --force                     Force the overwriting of bookOut.bin.\n" );
	fprintf( stdout, "     -q, --quiet                     No text output.\n" );
	fprintf( stdout, "     -v, --verbose                   Verbose.\n" );
	fprintf( stdout, "     -V, --version                   Output the version and exit.\n" );
	fprintf( stdout, "     -h, --help                      This help.\n" );
	fprintf( stdout, " \n" );

	exit(0);
}


/**************************************************/
/* PARSING OF THE ARGUMENTS                       */
/**************************************************/
int parse_args(int argc, char **argv) 
{
	int c, i;
	FILE *fid;

	struct option longopts[] = 
	{
		{"config-file",  required_argument, NULL, 'C'},
		{"force",   no_argument, NULL, 'f'},
		{"quiet",   no_argument, NULL, 'q'},
		{"verbose", no_argument, NULL, 'v'},
		{"version", no_argument, NULL, 'V'},
		{"help",    no_argument, NULL, 'h'},
		{0, 0, 0, 0}
	};

	opterr = 0;
	optopt = '!';

	while ((c = getopt_long(argc, argv, "C:fqvVh", longopts, &i)) != -1 ) 
	{
		switch (c) 
		{
			case 'C':
				mp_debug_msg( MP_DEBUG_PARSE_ARGS, func, "switch -C : optarg is [%s].\n", optarg );
				if (optarg == NULL)
				{
					mp_error_msg( func, "After switch -C or switch --config-file=.\n" );
					mp_error_msg( func, "the argument is NULL.\n" );
					mp_error_msg( func, "(Did you use --config-file without the '=' character ?).\n" );
					return( ERR_ARG );
				}
				else 
					configFileName = optarg;
				mp_debug_msg( MP_DEBUG_PARSE_ARGS, func, "Read config-file name [%s].\n", configFileName );
				break;
			case 'h':
				usage();
				break;
			case 'f':
				MPC_FORCE = MPC_TRUE;
				mp_debug_msg( MP_DEBUG_PARSE_ARGS, func, "MPC_FORCE is TRUE.\n" );
				break;
			case 'q':
				MPC_QUIET = MPC_TRUE;
				mp_debug_msg( MP_DEBUG_PARSE_ARGS, func, "MPC_QUIET is TRUE.\n" );
				break;
			case 'v':
				MPC_VERBOSE = MPC_TRUE;
				mp_debug_msg( MP_DEBUG_PARSE_ARGS, func, "MPC_VERBOSE is TRUE.\n" );
				break;
			case 'V':
				fprintf(stdout, "mpcat -- Matching Pursuit library version %s\n", VERSION);
				exit(0);
				break;
			default:
				mp_error_msg( func, "The command line contains the unrecognized option [%s].\n", argv[optind-1] );
				return( ERR_ARG );
		} // end switch
	} // end while

	mp_debug_msg( MP_DEBUG_PARSE_ARGS, func, "When exiting getopt, optind is [%d].\n", optind);
	mp_debug_msg( MP_DEBUG_PARSE_ARGS, func, "(argc is [%d].)\n", argc);

	// Check if some books are following the options
	if ( (argc-optind) < 3 ) 
	{
		mp_error_msg(func, "There must be at least two books (or - for stdin) to concatenate, plus a file name (or - for stdout) for the output book.\n");
		return ERR_ARG;
	}

	// Read the first book file name after the options
	bookOutFileName = argv[argc-1];
	mp_debug_msg( MP_DEBUG_PARSE_ARGS, func, "Read output book file name [%s].\n", bookOutFileName );

	//-----------------------
	// Basic options check 
	//-----------------------
	// Prevent an accidental erasing of an input file
	if ( strcmp( bookOutFileName, "-" ) && (!MPC_FORCE) ) 
	{
		if ( (fid = fopen( bookOutFileName, "rb" )) != NULL ) 
		{
			fclose( fid );
			mp_error_msg(func, "Output file [%s] exists. Delete it manually or use -f/--force if you want to overwrite it.\n", bookOutFileName );
			return ERR_ARG;      
		}
	}

	// Can't have quiet AND verbose (make up your mind, dude !)
	if ( MPC_QUIET && MPC_VERBOSE ) 
	{
		mp_error_msg(func, "Choose either one of --quiet or --verbose.\n");
		return( ERR_ARG );
	}

	return(0);
}


/**************************************************/
/* MAIN                                           */
/**************************************************/
int main( int argc, char **argv ) 
{
	//MP_Book_c			*book;
	//MP_Book_c			*combinedBook = NULL;
	MP_Book_c			**allBooks;
	size_t				numBooks = 0;
	size_t				nAtomRead = 0;
	char				mode;
	FILE		*fid;

	allBooks = (MP_Book_c**)malloc(maxNumBooks * sizeof(MP_Book_c));

	// Parse the command line
	if ( argc == 1 ) 
		usage();
	if ( parse_args( argc, argv ) ) 
	{
		mp_error_msg( func, "Please check the syntax of your command line. (Use --help to get some help.)\n" );
		exit( ERR_ARG );
	}
  
	// Load the MPTK environment
	if(! (MPTK_Env_c::get_env()->load_environment_if_needed(configFileName)) )
		exit(ERR_LOADENV);

	// Load all the books into memory (we can't stream one by one because of dict header)
	while ( optind < (argc-1) )
	{
		bookInFileName = argv[optind++];
		mp_debug_msg(MP_DEBUG_GENERAL, func, "Read book file name [%s] for book number [%lu].\n",bookInFileName, numBooks );

		if(numBooks >= maxNumBooks){
			mp_error_msg( func, "Number of books (%i) is greater than the fixed limit (%i).\n", numBooks, maxNumBooks);
			return (ERR_BOOK);
		}

		if ( !strcmp( bookInFileName, "-" ) ){
			if((allBooks[numBooks] = MP_Book_c::create(stdin)) == NULL)
			{
				mp_error_msg( func, "Can't create a new book from stdin.\n" );
				fflush( stderr );
				return( ERR_BOOK );
			}
		}else{
			FILE* fid;
			if((fid = fopen(bookInFileName, "r")) == NULL)
			{
				mp_error_msg( func,"Could not open file %s to read book\n", bookInFileName );
				return (ERR_BOOK);
			}

			if((allBooks[numBooks] = MP_Book_c::create(fid)) == NULL)
			{
				mp_error_msg( func, "Can't create a new book from file handle for %s.\n", bookInFileName );
				fflush( stderr );
				return( ERR_BOOK );
			}
			fclose ( fid );  
		}

		if ( allBooks[numBooks]->numAtoms == 0 ) 
		{
			if ( !MPC_QUIET ) 
			{
				fprintf ( stderr, "mpcat warning -- Can't read atoms for book number [%lu] from file [%s]. I'm skipping this book.\n",numBooks, bookInFileName );
				fflush( stderr );
			}
		}
		nAtomRead += allBooks[numBooks]->numAtoms;

		if ( MPC_VERBOSE ) 
			fprintf ( stderr, "mpcat msg -- Loaded [%lu] atoms for book number [%lu] from file [%s].\n", allBooks[numBooks]->numAtoms, numBooks, bookInFileName );
		++numBooks; // this must come at end of loop since used for indexing
	}


	// decide mode, open a filehandle if needed
	if ( strcmp( bookOutFileName, "-" ) ){
		mode = MP_TEXT;
		if((fid = fopen(bookOutFileName,"w")) == NULL)
		{
			mp_error_msg( func,"Could not open file %s to write\n", bookOutFileName );
			return 1;
		}
	}else{
		mode = MP_BINARY;
		fid = stdout;
	}

	////////////////////////////////////////////////////////////////////////////
	// Here we write the XML. Note, it "half"-uses TinyXml, because if many atoms there may be too much XML for memory.
	// Write the header
	fprintf( fid, "<?xml version=\"1.0\" encoding=\"ISO-8859-1\" ?>\n"); 
	fprintf( fid, "<mptkbook formatVersion=\"1\">\n");
	// Concatenate the dictionaries' XML and write them

	TiXmlDocument doc;
	TiXmlElement* version;
	TiXmlElement *root;
	root = new TiXmlElement( "dict" );  
	doc.LinkEndChild( root ); 
	version = new TiXmlElement( "libVersion" );  
	version->LinkEndChild( new TiXmlText( VERSION ));
	root->LinkEndChild(version);
	
	for(int i=0; i<numBooks; ++i){
		if(!allBooks[i]->atom[0]->dict->append_blocks_to_xml_root( root ))
		{
			mp_error_msg( func, "dict failed to append its blocks to XML in memory\n");
			//assert(false);
			return false;
		}
	}
	if (!doc.SaveFile( fid ))
	{ 
		mp_error_msg( func,"Could not save the file\n");
		return false;
	}


	// Write the book header
	if(!allBooks[0]->printBook_opening(fid, mode, nAtomRead)){
		fprintf ( stderr, "mpcat error -- could not write <book> opening\n" );
		return ERR_WRITE;
	}
	// Write all atoms
	for(int i=0; i<numBooks; ++i){
		if((allBooks[i]->printBook_atoms(fid, mode, NULL, nAtomRead))==0){
			fprintf ( stderr, "mpcat error -- could not write book number %i\n", i);
			return ERR_WRITE;
		}
	}
	// Write the book closer and the grand closer
	if(!allBooks[0]->printBook_closing(fid)){
		fprintf ( stderr, "mpcat error -- could not write <book> closing\n" );
		return ERR_WRITE;
	}
	fprintf( fid, "</mptkbook>\n");


	// close filehandle if needed
	if ( !strcmp( bookOutFileName, "-" ) ){
		fclose(fid);
	}

	if ( MPC_VERBOSE ) 
		fprintf( stderr, "mpcat msg -- The resulting book contains [%lu] atoms.\n", nAtomRead );

	// Clean the house
	for(int i=0; i<numBooks; ++i){
		delete allBooks[i];
	}
	delete( allBooks );
	// Release Mptk environnement
	MPTK_Env_c::get_env()->release_environment();
  
	return 0;
}
