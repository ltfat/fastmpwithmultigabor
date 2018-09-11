/******************************************************************************/
/*                                                                            */
/*                                test_windowLen.cpp                          */
/*                                                                            */
/*                      Matching Pursuit Testing Suite                        */
/*                                                                            */
/* Ronan Le Boulch                                            Wed Jun 22 2011 */
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

#include <mptk.h>

void usage( void ) 
{
	fprintf( stdout, " \n" );
	fprintf( stdout, " Usage:\n" );
	fprintf( stdout, "     test_windowLen szConfigFile.xml szDictionaryFile.xml szSndFile.wav\n" );
	fprintf( stdout, " \n" );
	fprintf( stdout, " Synopsis:\n" );
	fprintf( stdout, "     This is a sratch test in order to try launching mpd when the window length of the dictionary is superior to the signal length.\n" );
	fprintf( stdout, " \n" );
	fprintf( stdout, " Mandatory arguments:\n" );
	fprintf( stdout, "     szConfigFile.xml       Use the configuration file (path.xml) situated under the mptk directory.\n" );
	fprintf( stdout, "     szDictionaryFile.xml   The dictionary which will be used to decompose the signal\n" );
	fprintf( stdout, "     szSndFile.wav          The audio signal to decompose\n" );
	fprintf( stdout, " \n" );
	return;
}

int main( int argc, char ** argv ) 
{
	const char		*func = "test_windowLen";
	char			*szConfigFile, *szDictFile, *szSndFile;
	FILE			*fid = NULL;
	int				iIndex = 0;
	MP_Dict_c		*dict = NULL;
	MP_Signal_c		*sig = NULL;
	MP_Book_c		*book = NULL;
	MP_Mpd_Core_c	*mpdCore = NULL;

	//----------------------------------------
	// 1) Parsing and testing the parameters
	//----------------------------------------
	if (argc == 1)
	{
		usage();
		return -1;
	}
	else if(argc == 4)
    {
		// Testing the 1st argument
		szConfigFile = argv[1];
		if((fid = fopen( szConfigFile, "rb" )) == NULL)
		{
			mp_error_msg(func,"The config file [%s] does not exist.\n",szConfigFile);
			return -1;
		}
		fclose( fid ); 
		// Testing the 2nd argument
		szDictFile = argv[2];
		if((fid = fopen( szDictFile, "rb" )) == NULL)
		{
			mp_error_msg(func,"The dictionary file [%s] does not exist.\n",szDictFile);
			return -1;
		}
		fclose( fid ); 
		// Testing the 3rd argument
		szSndFile = argv[3];
		if((fid = fopen( szSndFile, "rb" )) == NULL)
		{
			mp_error_msg(func,"The sound file [%s] does not exist.\n",szSndFile);
			return -1;
		}
		fclose( fid ); 
	}
	else
	{ 
		mp_error_msg(func,"Wrong number of arguments. Please follow the usage instructions of this executable.\n");
		usage();
		return( -1 );
	}

	//----------------------------------------
	// 2) Beginning of the test : presentation
	//----------------------------------------
	mp_info_msg( func, "--------------------------------------------------\n" );
	mp_info_msg( func, " Testing the WindowLen in order to make it scratch\n" );
	mp_info_msg( func, "--------------------------------------------------\n" );
	mp_info_msg( func, "The command line was:\n" );
	for (iIndex = 0 ; iIndex < argc ; iIndex++ )
		fprintf( stderr, "%s ", argv[iIndex] );
	fprintf( stderr, "\n");
	
	//----------------------------------------
	// 3) Lading the environment
	//----------------------------------------
	mp_info_msg(func,"Step 1 : Loading the environment...\n");
	szConfigFile = argv[1];
	if(! (MPTK_Env_c::get_env()->load_environment_if_needed(szConfigFile)) ) 
		return -1;

	//----------------------------------------
	// 4) Loading the dictionary
	//----------------------------------------
	mp_info_msg(func,"Step 2 : Loading the dictionary...\n");
	dict = MP_Dict_c::read_from_xml_file(szDictFile);
	if ( dict == NULL )
    {
		mp_error_msg( func, "Failed to create a dictionary from XML file [%s].\n", szDictFile);
		//free_mem( dict, book, sig, mpdCore );
		return -1;
    }
	if ( dict->numBlocks == 0 )
	{
		mp_error_msg( func, "The dictionary scanned from XML file [%s] contains no blocks.\n");
		//free_mem( dict, book, sig, mpdCore );
		return -1;
    }
    
	//----------------------------------------
	// 5) Loading the signal
	//----------------------------------------
	mp_info_msg(func,"Step 3 : Loading the signal...\n");
	sig = MP_Signal_c::init( szSndFile );
	if ( sig == NULL )
    {
		mp_error_msg( func, "Failed to initialize a signal from file [%s].\n", szSndFile );
		//free_mem( dict, book, sig, mpdCore );
		return -1;
    }
	
	//----------------------------------------
	// 6) Modifying the dictionary according to the signal length
	//----------------------------------------
	if(dict->maxFilterLen <= sig->numSamples)
		dict->block[0]->filterLen = sig->numSamples+2;
	
	//----------------------------------------
	// 7) Making the book
	//----------------------------------------
	mp_info_msg(func,"Step 4 : Making the book...\n");
	book = MP_Book_c::create(sig->numChans, sig->numSamples, sig->sampleRate );
	if ( book == NULL ) 
	{
		mp_error_msg( func, "Failed to create the book.\n" );
		//free_mem( dict, book, sig, mpdCore );
		return -1;
	}

	//----------------------------------------
	// 7) Making the mpd core
	//----------------------------------------
	mpdCore = MP_Mpd_Core_c::create( sig, book, dict );
	if (mpdCore == NULL)
		mp_info_msg( func, "Scratch Test passed... The behavior is normal\n" );
	else
		mp_info_msg( func, "Scratch Test failed... The behavior is not normal\n" );
	
	// Clean the house
	if ( mpdCore ) delete mpdCore;
	if ( book )  delete book; 
	if (dict) delete dict;
	if ( sig )  delete sig; 
	
	// Release Mptk environnement
	MPTK_Env_c::get_env()->release_environment();

	if(mpdCore == NULL)
		return 0;
	else
		return 1;
}
