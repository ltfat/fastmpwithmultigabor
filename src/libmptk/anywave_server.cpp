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

/***************************/
/* CONSTRUCTORS/DESTRUCTOR */
/***************************/

/********************/
/* Void constructor */
MP_Anywave_Server_c::MP_Anywave_Server_c( void ) {

  numTables = 0;
  maxNumTables = 0;
  tables = NULL;

}

/**************/
/* destructor */
MP_Anywave_Server_c::~MP_Anywave_Server_c() {
#ifndef NDEBUG
  fprintf( stderr, "mplib DEBUG -- ~MP_Anywave_Server_c -- Entering the waveform server destructor.\n" );
#endif

  release();

#ifndef NDEBUG
  fprintf( stderr, "mplib DEBUG -- ~MP_Anywave_Server_c -- Exiting the waveform server destructor.\n" );
#endif
}

/***************************/
/* OTHER METHODS           */
/***************************/

/* Test */
bool MP_Anywave_Server_c::test( void ) {

  fprintf( stdout, "\n-- Entering MP_Anywave_Server_c::test \n" );

  unsigned long int tableIdx;
  unsigned long int numTablesToSet = 5;
  MP_Anywave_Table_c* tablePtr;
  char tempFilename[50];

  /* Create a Anywave_Server */
  MP_Anywave_Server_c* serverPtr = new MP_Anywave_Server_c();
  if (serverPtr == NULL) {
    return(false);
  }
  /* Add tables */
  
  for ( tableIdx = 0;
	tableIdx < numTablesToSet;
	tableIdx++ ) {
    tablePtr = new MP_Anywave_Table_c();
    if (tablePtr == NULL) {
      return(false);
    }

    sprintf( tempFilename, "Table_%lu", tableIdx );
    tablePtr->set_table_file_name( tempFilename );

    if ( serverPtr->add( tablePtr ) == serverPtr->numTables) {
      return(false);
    }

  }
  
  /* test get_filename */
  for ( tableIdx = 0;
	tableIdx < numTablesToSet;
	tableIdx++ ) {
    sprintf( tempFilename, "Table_%lu", tableIdx );
    fprintf( stdout, 
	     "\nmplib TEST -- Filename %s -- expected index %lu -- result of get_index %li",
	     tempFilename,
	     tableIdx,
	     serverPtr->get_index( tempFilename ) );
    if (tableIdx != serverPtr->get_index( tempFilename )) {
      return(false);
    }
    fflush( stdout );
  }

  /* test get_index */
  for ( tableIdx = 0;
	tableIdx < numTablesToSet;
	tableIdx++ ) {
    sprintf( tempFilename, "Table_%lu", tableIdx );
    fprintf( stdout, 
	     "\nmplib TEST -- Index %lu -- expected filename %s -- result of get_filename %s",
	     tableIdx,
	     tempFilename,
	     serverPtr->get_filename( tableIdx ) );
    if (strcmp(tempFilename, serverPtr->get_filename( tableIdx )) != 0) {
      return(false);
    }
    fflush( stdout );
  }

  /* Destroy the anywave server */
  delete( serverPtr );

  fprintf( stdout, "\n-- Exiting MP_Anywave_Server_c::test \n" );
  fflush( stdout );
  return( true );
}

/* Memory release */
void MP_Anywave_Server_c::release( void ) {

  unsigned long int n;

#ifndef NDEBUG
  fprintf( stderr, "mplib DEBUG -- MP_Anywave_Server_c::release() -- Entering.\n" );
#endif

  /* Free the buffers */
  for ( n = 0; n < numTables; n++ ) {
    if ( tables[n]  ) delete ( tables[n] );
  }
  /* Free the tables array */
  if (tables != NULL) {
    free( tables );
    tables = NULL;
  }
  
  /* Reset the counters */
  numTables = 0;
  maxNumTables = 0;

#ifndef NDEBUG
  fprintf( stderr, "mplib DEBUG -- MP_Anywave_Server_c::release() -- Exiting.\n" );
#endif

}

/* Add a waveform table */
unsigned long int MP_Anywave_Server_c::add( MP_Anywave_Table_c* table ){

  unsigned long int n = 0;
  MP_Anywave_Table_c** ptrTable = NULL;
  char* filename = NULL;
  
  /* Seek if the table exists */
  filename = table->tableFileName;
  n = get_index(filename);

  /* If the table is already present in the tables array 
     (i.e., if the above loop stopped before the last slot),
     return the index of the existing table */
  if (n < numTables) {
    return( n );
  }
  /* Else, add the table */
  else if (n == numTables) {

    /* If needed, add space for more tables */
    if (numTables == maxNumTables) { 
      /* check that the number of tables is not greater than the max of an unsigned long int */
      if ( MP_MAX_UNSIGNED_LONG_INT - MP_ANYWAVE_BLOCK_SIZE <= numTables) {
	 mp_error_msg( "MP_Anywave_Server_c::add", "numTables [%lu] + MP_ANYWAVE_BLOCK_SIZE [%lu] is greater than the max for an unsigned long int [%lu]. Cannot add new tables. Exiting from add().\n", numTables, MP_ANYWAVE_BLOCK_SIZE, MP_MAX_UNSIGNED_LONG_INT);
	 return( maxNumTables );
      }
      if ( (double)MP_MAX_SIZE_T / (double)(numTables + MP_ANYWAVE_BLOCK_SIZE) / (double)sizeof(MP_Anywave_Table_c*) <= 1.0) {
	mp_error_msg( "MP_Anywave_Server_c::add", "(numTables + MP_ANYWAVE_BLOCK_SIZE) [%lu] . sizeof(MP_Anywave_Table_c*) [%lu] is greater than the max for a size_t [%lu]. Cannot reallocate the array of tables. Exiting from add().\n", numTables + MP_ANYWAVE_BLOCK_SIZE, sizeof(MP_Anywave_Table_c*), MP_MAX_SIZE_T);
	 return( maxNumTables );
      }

      if ( (ptrTable = (MP_Anywave_Table_c**)realloc( tables, (numTables+MP_ANYWAVE_BLOCK_SIZE)*sizeof(MP_Anywave_Table_c*) )) == NULL ) {
	mp_error_msg( "MP_Anywave_Server_c::add", "Can't realloc to add a new table."
		 " Returning index maxNumTables (= max + 1).\n" );
	return( maxNumTables );
      } else {
	/* If the realloc succeeded, initialize */
	tables = ptrTable;
	
	maxNumTables = maxNumTables + MP_ANYWAVE_BLOCK_SIZE;
      }
    }

    /* Normalize the waveforms */
    if (table->normalized == 0) {
      table->normalize();
    }
    
    /* Verify that normalization performed well */
    if (table->normalized == 2) {
      mp_error_msg( "MP_Anywave_Server_c::add", "Can't normalize the waveforms. Returning index maxNumTables (= max + 1).\n");
      return( maxNumTables );
    } else {
      
      /* Add pointer to table to tables array */
      tables[numTables] = table;
      
      /* Count the new table */
      numTables = numTables + 1;
      
      /* Return the index of the added table */
      return( numTables - 1 );
    }
  }
  else {
    mp_error_msg( "MP_Anywave_Server_c::add", "Oooops, this code is theoretically"
	     " unreachable. Returning index maxNumTables (= max + 1).\n" );
    return( maxNumTables );
  }

}

/* Add waveform tables from file */
unsigned long int MP_Anywave_Server_c::add( char* filename ){
  char * func =  "MP_Anywave_Server_c::add(char *)";
  /* Check if the table already exists in the tables array */
  unsigned long int n = 0;
  MP_Anywave_Table_c** ptrTable = NULL;
  MP_Anywave_Table_c* table = NULL;
  FILE * pFile = NULL;

  /* Seek if the table exists */
  n = get_index(filename);

  /* If the table is already present in the tables array 
     (i.e., if the above loop stopped before the last slot),
     return the index of the existing table */
  if (n < numTables) {
    return( n );
  }
  /* Else, add the table */
  else if (n == numTables) {

    /* test if the file exists either in the current directory or in the default table directory */
    pFile = fopen (filename,"rt");
    if (NULL == pFile) {
      mp_error_msg(func , "Can't open the file %s - Returning index maxNumTables (= max + 1).\n", filename );
      return( maxNumTables );
    } else {
      
      /* create the table */
      table = new MP_Anywave_Table_c( filename );
      if (NULL == table ) {
	mp_error_msg( func, "Can't create a anywave table from file %s - Returning index maxNumTables (= max + 1).\n", filename );
	return( maxNumTables );
    fclose( pFile );
      } else {

	/* close the file and set the property tableFileName of the table to filename */
	fclose( pFile );
	if (table->set_table_file_name(filename) == NULL) {
	  mp_error_msg( func, "Can't modify the tableFileName property of the table to %s - Returning index maxNumTables (= max + 1).\n", filename );
	  return( maxNumTables );
	} else {

	  /* If needed, add space for more tables */
	  if (numTables == maxNumTables) { 
	    /* check that the number of tables is not greater than the max of an unsigned long int */
	    if ( MP_MAX_UNSIGNED_LONG_INT - MP_ANYWAVE_BLOCK_SIZE <= numTables) {
	      mp_error_msg(func, "numTables [%lu] + MP_ANYWAVE_BLOCK_SIZE [%lu] is greater than the max for an unsigned long int [%lu]. Cannot add new tables. Exiting from add().\n", numTables, MP_ANYWAVE_BLOCK_SIZE, MP_MAX_UNSIGNED_LONG_INT);
	      return( maxNumTables );
	    }
	    if ( (double)MP_MAX_SIZE_T / (double)(numTables + MP_ANYWAVE_BLOCK_SIZE) / (double)sizeof(MP_Anywave_Table_c*) <= 1.0) {
	      mp_error_msg( func, "(numTables + MP_ANYWAVE_BLOCK_SIZE) [%lu] . sizeof(MP_Anywave_Table_c*) [%lu] is greater than the max for a size_t [%lu]. Cannot reallocate the array of tables. Exiting from add().\n", numTables + MP_ANYWAVE_BLOCK_SIZE, sizeof(MP_Anywave_Table_c*), MP_MAX_SIZE_T);
	      return( maxNumTables );
	    }
    
	    if ( (ptrTable = (MP_Anywave_Table_c**)realloc( tables, (numTables+MP_ANYWAVE_BLOCK_SIZE)*sizeof(MP_Anywave_Table_c*) )) == NULL ) {
	      mp_error_msg( func, "Can't realloc to add a new table."
		       " Returning index maxNumTables (= max + 1).\n" );
	      return( maxNumTables );
	    } else {
	      /* If the realloc succeeded, initialize */
	      tables = ptrTable;
	      maxNumTables = maxNumTables + MP_ANYWAVE_BLOCK_SIZE;
	    }
	  }
	  
	  /* Normalize the waveforms */
	  if (table->normalized == 0) {
	    table->normalize();
	  }
    
	  /* Verify that normalization performed well */
	  if (table->normalized == 2) {
	    mp_error_msg( func, "Can't normalize the waveforms. Returning index maxNumTables (= max + 1).\n");
	    return( maxNumTables );
	  } else {
	    
	    /* Add pointer to table to tables array */
	    tables[numTables] = table;
	    
	    /* Count the new table */
	    numTables = numTables + 1;
	    
	    /* Return the index of the added table */
	    return( numTables - 1 );
	  }
	}
      }
    }
  } else {
    mp_error_msg( func, "Oooops, this code is theoretically"
	     " unreachable. Returning index maxNumTables (= max + 1).\n" );
    return( maxNumTables );
  }
}

/* Get filename associated to the table number "index" */
char* MP_Anywave_Server_c::get_filename( unsigned long int index ) {
  if (index < numTables) {
    return(tables[index]->tableFileName);
  } else {
    mp_error_msg( "MP_Anywave_Server_c::get_filename", "the table index is bigger than the number of tables. Returning NULL string\n");
    return(NULL);
  }
}

/* Get the table number associated to filename */
unsigned long int MP_Anywave_Server_c::get_index( char* filename ) {

  unsigned long int n = 0;
  MP_Anywave_Table_c** ptrTable = NULL;
  if ( filename != NULL ) {
    for (  n = 0,   ptrTable = tables;
	   n < numTables;
	   n++,     ptrTable++ ) {
      if ( strcmp(filename,(*ptrTable)->tableFileName) == 0 ) break;
    }
  } else {
    n = numTables;
  }
  return(n);
}

