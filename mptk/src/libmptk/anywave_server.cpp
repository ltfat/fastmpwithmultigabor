/******************************************************************************/
/*                                                                            */
/*                           anywave_server.cpp                               */
/*                                                                            */
/*                        Matching Pursuit Library                            */
/*                                                                            */
/* RÈmi Gribonval                                                             */
/* Sacha Krstulovic                                                           */
/* Sylvain Lesage                                             Mon Feb 21 2005 */
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
 * CVS log:
 *
 * $Author: sacha $
 * $Date: 2007-06-27 16:52:43 +0200 (Wed, 27 Jun 2007) $
 * $Revision: 1082 $
 *
 */

/*************************************************************/
/*                                                           */
/* anywave_server.cpp: methods for class MP_Anywave_Server_c */
/*                                                           */
/*************************************************************/

#include "mptk.h"
#include "mp_system.h"
#include "md5sum.h"

/***************************/
/* CONSTRUCTORS/DESTRUCTOR */
/***************************/

/********************/
/* Void constructor */
MP_Anywave_Server_c::MP_Anywave_Server_c( void ) 
{
	numTables = 0;
	maxNumTables = 0;
	tables = NULL;
}

/**************/
/* destructor */
MP_Anywave_Server_c::~MP_Anywave_Server_c() 
{
	unsigned long int n;

	mp_debug_msg( MP_DEBUG_DESTRUCTION, "MP_Anywave_Server_c::~MP_Anywave_Server_c()", "Entering the waveform server destructor...\n" );
	// Free the buffers
	for ( n = 0; n < numTables; n++ ) 
	{
		if(tables[n]) 
			delete tables[n];
	}
	// Free the tables array
	if (tables != NULL) 
	{
		free( tables );
		tables = NULL;
	}
	// Reset the counters
	numTables = 0;
	maxNumTables = 0;
	mp_debug_msg( MP_DEBUG_DESTRUCTION, "MP_Anywave_Server_c::~MP_Anywave_Server_c()", "Exiting the waveform server destructor...\n" );
}

/***************************/
/* OTHER METHODS           */
/***************************/

//-------------------------------//
// Function : Test(void)
// Usage	: Test
//-------------------------------//
bool MP_Anywave_Server_c::test(void) 
{
	unsigned long int tableIdx;
	unsigned long int numTablesToSet = 5;
	MP_Anywave_Table_c* tablePtr;
	char szkeyTableName[50];
	
	fprintf( stdout, "\n-- Entering MP_Anywave_Server_c::test \n" );

	// Create a Anywave_Server
	MP_Anywave_Server_c* serverPtr = new MP_Anywave_Server_c();
	if (serverPtr == NULL) 
		return(false);

	// Add tables
    for ( tableIdx = 0 ; tableIdx < numTablesToSet ; tableIdx++ ) 
	{
		// Create the table
		if((tablePtr = new MP_Anywave_Table_c()) == NULL)
			return(false);

		// Allocate and add the keyName
		if((tablePtr->szKeyTable = (char *)calloc(MD5_ALLOC_SIZE,sizeof(char))) == NULL )
			return false;
		sprintf( szkeyTableName, "Table_%lu", tableIdx );
		tablePtr->set_key_table( szkeyTableName );

		if ( serverPtr->add( tablePtr ) == serverPtr->numTables) 
			return(false);
	}
  
	// test get_index
	for ( tableIdx = 0 ; tableIdx < numTablesToSet ; tableIdx++ ) 
	{
		sprintf( szkeyTableName, "Table_%lu", tableIdx );
		fprintf( stdout, "\nmplib TEST -- Filename %s -- expected index %lu -- result of get_index %li", szkeyTableName, tableIdx, serverPtr->get_index( szkeyTableName ) );
		if (tableIdx != serverPtr->get_index( szkeyTableName )) 
			return(false);
    }
    fflush( stdout );

	// test get_keyname
	for ( tableIdx = 0 ; tableIdx < numTablesToSet ; tableIdx++ ) 
	{
		sprintf( szkeyTableName, "Table_%lu", tableIdx );
		fprintf( stdout, "\nmplib TEST -- Index %lu -- expected filename %s -- result of get_filename %s", tableIdx, szkeyTableName, serverPtr->get_keyname( tableIdx ) );
		if (strcmp(szkeyTableName, serverPtr->get_keyname( tableIdx )) != 0)
			return(false);
    }
    fflush( stdout );

	// Destroy the anywave server
	delete( serverPtr );

	fprintf( stdout, "\n-- Exiting MP_Anywave_Server_c::test \n" );
	fflush( stdout );
	return( true );
}

//---------------------------------------------//
// Function : add( MP_Anywave_Table_c* table )
// Usage	: Add a waveform table from a table
//---------------------------------------------//
unsigned long int MP_Anywave_Server_c::add( MP_Anywave_Table_c* table )
{
	const char			*func =  "MP_Anywave_Server_c::add( MP_Anywave_Table_c* table )";
	unsigned long int	n = 0;

	// Search if the table exists */
	n = get_index(table->szKeyTable);

	// If the table is already present in the tables array i.e., if the above loop stopped before the last slot), return the index of the existing table */
	if (n < numTables)
		return( n );

	// If needed, add space for more tables
	if (numTables == maxNumTables) 
	{ 
		if(!reallocate())
		{
			mp_error_msg( func, "Can't reallocate the table - Returning index maxNumTables (= max + 1).\n");
			return( maxNumTables );
		}
	}
	// Normalize the waveforms
	if (table->normalized == 0) 
	{
		if(table->normalize() == 2)
		{
			mp_error_msg( func, "Can't normalize the waveforms. Returning index maxNumTables (= max + 1).\n");
			return( maxNumTables );
		} 
	}
	// Add pointer to table to tables array
	tables[numTables] = table;
	// Count the new table
	numTables = numTables + 1;
	// Return the index of the added table
	return( numTables - 1 );
}

//---------------------------------------------//
// Function : add( char* filename )
// Usage	: Add a waveform table from a file
//---------------------------------------------//
unsigned long int MP_Anywave_Server_c::add( char* filename )
{
	const char			*func =  "MP_Anywave_Server_c::add(char *)";
	unsigned long int	n = 0;
	MP_Anywave_Table_c	*table = NULL;
	char				szkeyTableName[MD5_ALLOC_SIZE] = "";

	// Transform the filename string into mp5 checksum 
	encodeMd5(filename, strlen(filename), szkeyTableName);
	
	// Search if the table exists */
	n = get_index(szkeyTableName);

	// If the table is already present in the tables array (i.e., if the above loop stopped before the last slot), return the index of the existing table */
	if (n < numTables) 
		return( n );
	// create the table
	table = new MP_Anywave_Table_c();
	if (NULL == table ) 
	{
		mp_error_msg( func, "Can't create a anywave table - Returning index maxNumTables (= max + 1).\n");
		return( maxNumTables );
	} 
	if(!table->AnywaveCreator(filename))
	{
		mp_error_msg( func, "Can't initialise a anywave table from file %s - Returning index maxNumTables (= max + 1).\n", filename );
		return( maxNumTables );
	} 
	// close the file and set the property tableFileName of the table to filename
	if (table->set_table_file_name(filename) == NULL) 
	{
		mp_error_msg( func, "Can't modify the datafilename property of the table to %s - Returning index maxNumTables (= max + 1).\n", filename );
		return( maxNumTables );
	} 
	// close the file and set the property tableFileName of the table to filename
	if (table->set_key_table(szkeyTableName) == NULL) 
	{
		mp_error_msg( func, "Can't modify the key table property of the table to %s - Returning index maxNumTables (= max + 1).\n", szkeyTableName );
		return( maxNumTables );
	} 
	
	return add(table);
}


//-------------------------------------------------------------//
// Function : add( map<string, string, mp_ltstring> *paramMap )
// Usage	: Add a waveform table from a map
//-------------------------------------------------------------//
unsigned long int MP_Anywave_Server_c::add( map<string, string, mp_ltstring> *paramMap )
{
	const char			*func =  "MP_Anywave_Server_c::add(char *)";
	unsigned long int	n = 0;
	MP_Anywave_Table_c	*table = NULL;
	char				szkeyTableName[MD5_ALLOC_SIZE] = "";

	// Creates an empty new table
	table = new MP_Anywave_Table_c();
	if (NULL == table ) 
	{
		mp_error_msg( func, "Can't create a anywave table - Returning index maxNumTables (= max + 1).\n");
		return( maxNumTables );
	} 

	// Testing the "doubledata" parameter
	if((*paramMap)["doubledata"].size() > 0)
			// Encode the datas into base64 string
			(*paramMap)["data"] = table->encodeBase64((char *)(*paramMap)["doubledata"].c_str(),(*paramMap)["doubledata"].size());

	// Transform the filename string into md5 checksum
	encodeMd5((char *)(*paramMap)["data"].c_str(), (*paramMap)["data"].size(), szkeyTableName);

	// Search if the table exists */
	n = get_index(szkeyTableName);

	// If the table is already present in the tables array (i.e., if the above loop stopped before the last slot), return the index of the existing table */
	if (n < numTables) 
		return( n );
	// create the table
	if(!table->AnywaveCreator(paramMap))
	{
		mp_error_msg( func, "Can't initialise a anywave table from paramMap - Returning index maxNumTables (= max + 1).\n");
		return( maxNumTables );
	} 
	// close the file and set the property tableFileName of the table to filename
	if (table->set_key_table(szkeyTableName) == NULL) 
	{
		mp_error_msg( func, "Can't modify the key table property of the table to %s - Returning index maxNumTables (= max + 1).\n", szkeyTableName );
		return( maxNumTables );
	} 
	
	return add(table);
}

//---------------------------------------------------------------------------//
// Function : reallocate( void )
// Usage	: Reallocate memory if we arrive at the maximum of the num tables
//---------------------------------------------------------------------------//
bool MP_Anywave_Server_c::reallocate(void)
{
	const char			*func =  "MP_Anywave_Server_c::reallocate(char *)";
	MP_Anywave_Table_c	**ptrTable = NULL;

	// check that the number of tables is not greater than the max of an unsigned long int
	if ( MP_MAX_UNSIGNED_LONG_INT - MP_ANYWAVE_BLOCK_SIZE <= numTables) 
	{
		mp_error_msg(func, "numTables [%lu] + MP_ANYWAVE_BLOCK_SIZE [%lu] is greater than the max for an unsigned long int [%lu]. Cannot add new tables. Exiting from add().\n", numTables, MP_ANYWAVE_BLOCK_SIZE, MP_MAX_UNSIGNED_LONG_INT);
		return false;
	}
	if ( (double)MP_MAX_SIZE_T / (double)(numTables + MP_ANYWAVE_BLOCK_SIZE) / (double)sizeof(MP_Anywave_Table_c*) <= 1.0) 
	{
		mp_error_msg( func, "(numTables + MP_ANYWAVE_BLOCK_SIZE) [%lu] . sizeof(MP_Anywave_Table_c*) [%lu] is greater than the max for a size_t [%lu]. Cannot reallocate the array of tables. Exiting from add().\n", numTables + MP_ANYWAVE_BLOCK_SIZE, sizeof(MP_Anywave_Table_c*), MP_MAX_SIZE_T);
		return false;
	}
	if ( (ptrTable = (MP_Anywave_Table_c**)realloc( tables, (numTables+MP_ANYWAVE_BLOCK_SIZE)*sizeof(MP_Anywave_Table_c*) )) == NULL ) 
	{
		mp_error_msg( func, "Can't realloc to add a new table. Returning index maxNumTables (= max + 1).\n" );
		return false;
	} 
	// If the realloc succeeded, initialize
	tables = ptrTable;
	maxNumTables = maxNumTables + MP_ANYWAVE_BLOCK_SIZE;
	return true;
}



//---------------------------------------------------------------------------//
// Function : get_keyname( unsigned long int index ) 
// Usage	: Get filename associated to the table number "index"
//---------------------------------------------------------------------------//
char* MP_Anywave_Server_c::get_keyname( unsigned long int index ) 
{
	if (index < numTables) 
		return(tables[index]->szKeyTable);
	else 
	{
		mp_error_msg( "MP_Anywave_Server_c::get_keyname", "the table index is bigger than the number of tables. Returning NULL string\n");
		return(NULL);
	}
}

//---------------------------------------------------------------------------//
// Function : get_keyname_size(void) 
// Usage	: Return the keyname size
//---------------------------------------------------------------------------//
int MP_Anywave_Server_c::get_keyname_size(void)
{
	return MD5_ALLOC_SIZE;
}

//---------------------------------------------------------------------------//
// Function : get_index ( char* szKeyTableName )
// Usage	: Get the table number associated to szKeyTable
//---------------------------------------------------------------------------//
unsigned long int MP_Anywave_Server_c::get_index( char* szKeyTableName ) 
{
	unsigned long int n = 0;
	MP_Anywave_Table_c** ptrTable = NULL;
  
	if ( szKeyTableName != NULL ) 
	{
		for (  n = 0,   ptrTable = tables ; n < numTables ; n++,ptrTable++ ) 
		{
			if ( memcmp(szKeyTableName,(*ptrTable)->szKeyTable,MD5_ALLOC_SIZE) == 0 ) 
				break;
		}
	} 
	else 
	    n = numTables;

  return(n);
}

//-------------------------------------------------------------------------------//
// Function : encodeMd5 (char *szInputEncode, int iInputSize, char *OutputEncode)
// Usage	: Encode an input string into md5
//-------------------------------------------------------------------------------//
void MP_Anywave_Server_c::encodeMd5(char *szInputEncode, int iInputSize, char *OutputEncode)
{
	md5_encode((unsigned char *)szInputEncode, iInputSize, (unsigned char *)OutputEncode);
	return;
}
