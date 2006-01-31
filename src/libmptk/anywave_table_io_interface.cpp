/******************************************************************************/
/*                                                                            */
/*                     anywave_table_io_interface.cpp                         */
/*                                                                            */
/*                        Matching Pursuit Library                            */
/*                                                                            */
/* Rémi Gribonval                                                             */
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
 * SVN log:
 *
 * $Author: sacha $
 * $Date$
 * $Revision$
 *
 */


#include "mptk.h"
#include "mp_system.h"

/**********************************************************************************/
/*                                                                                */
/* anywave_table_io_interface.cpp: methods for class MP_Anywave_Table_Scan_Info_c */
/*                                                                                */
/**********************************************************************************/

/* Constructor */
MP_Anywave_Table_Scan_Info_c::MP_Anywave_Table_Scan_Info_c() {
#ifndef NDEBUG
  fprintf(stderr,"mplib DEBUG -- constructing MP_Anywave_Table_Scan_Info...\n");
#endif

  reset_all();

#ifndef NDEBUG
  fprintf(stderr,"mplib DEBUG -- Done.\n");
#endif
}


/* Destructor */
MP_Anywave_Table_Scan_Info_c::~MP_Anywave_Table_Scan_Info_c() {
#ifndef NDEBUG
  fprintf(stderr,"mplib DEBUG -- deleting MP_Anywave_Table_Scan_Info.\n");
#endif
}


/* Resetting the local variables */
void MP_Anywave_Table_Scan_Info_c::reset( void ) {

  filterLen = 0;
  filterLenIsSet = false;
  
  numChans = 0;
  numChansIsSet = false;

  numFilters = 0;
  numFiltersIsSet = false;

  strcpy( dataFileName, "" );
  dataFileNameIsSet = false;
}

/* Resetting the global variables */
void MP_Anywave_Table_Scan_Info_c::reset_all( void ) {
  
  reset();
  
  strcpy( libVersion, VERSION );
  
  globFilterLen = 0;
  globFilterLenIsSet = false;
  
  globNumChans = 0;
  globNumChansIsSet = false;

  globNumFilters = 0;
  globNumFiltersIsSet = false;

  strcpy( globDataFileName, "" );
  globDataFileNameIsSet = false;

}


/* Pop a block */
bool MP_Anywave_Table_Scan_Info_c::pop_table( MP_Anywave_Table_c* table ) {

  /* - filterLen: */
  if (!filterLenIsSet) {
    if (globFilterLenIsSet) {
      filterLen = globFilterLen;
      filterLenIsSet = true;
    } else {
      mp_error_msg( "MP_Anywave_Table_Scan_Info_c::pop_table()","table has no filterLen."
	       " Returning 0.\n" );
      reset();
      return( false );
    }
  }
  /* - numChans: */
  if (!numChansIsSet) {
    if (globNumChansIsSet) {
      numChans = globNumChans;
      numChansIsSet = true;
    } else {
      mp_error_msg( "MP_Anywave_Table_Scan_Info_c::pop_table()","table has no numChans."
	       " Returning 0.\n" );
      reset();
      return( false );
    }
  }
  /* - numFilters: */
  if (!numFiltersIsSet) {
    if (globNumFiltersIsSet) {
      numFilters = globNumFilters;
      numFiltersIsSet = true;
    } else {
      mp_error_msg( "MP_Anywave_Table_Scan_Info_c::pop_table()","table has no numFilters."
	       " Returning 0.\n" );
      reset();
      return( false );
    }
  }
  /* - filename: */
  if (!dataFileNameIsSet) {
    if (globDataFileNameIsSet) {
      strcpy( dataFileName, globDataFileName );
      dataFileNameIsSet = true;
    } else {
      mp_error_msg( "MP_Anywave_Table_Scan_Info_c::pop_table()","table has no filename."
	       " Returning 0.\n" );
      reset();
      return( false );
    }
  }

  /********************/
  /* Fill the table : */
  /********************/
  
  table->filterLen = filterLen;
  table->numChans = numChans;
  table->numFilters = numFilters;
  if ( table->set_data_file_name( dataFileName ) == NULL ) {
    mp_error_msg( "MP_Anywave_Table_Scan_Info_c::pop_table()","setting dataFileName %s to the table failed."
	     " Returning 0.\n", dataFileName );
    reset();
    return( false );
  }
  if ( table->load_data() == false ) {
    mp_error_msg( "MP_Anywave_Table_Scan_Info_c::pop_table()","loading the data from the file %s failed."
	     " Returning 0.\n", dataFileName );
    reset();
    return( false );
  }  

  /********************/
  /* Reset the local table variables in the MP_Anywave_Table_Scan_Info structure */
  reset();

  /* Return true (success) */
  return( true );
}
