/******************************************************************************/
/*                                                                            */
/*                             test_anywave.cpp                               */
/*                                                                            */
/*                      Matching Pursuit Testing Suite                        */
/*                                                                            */
/* Rémi Gribonval                                                             */
/* Sacha Krstulovic                                                           */
/* Sylvain Lesage                                             Fri Nov 10 2005 */
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

/** \file test_anywave.cpp A file with some code that serves as a test
 * that it is properly working.
 */
#include <mptk.h>

#include <stdio.h>
#include <stdlib.h>

void usage() {

  fprintf(stderr, "\n test_anywave signal.wav anywave_table.bin dict.xml");
  fprintf(stderr, "\n");
  fprintf(stderr, "\n Calls the test functions of the following classes :");
  fprintf(stderr, "\n    MP_Anywave_Server_c, MP_Anywave_Table_c, MP_Anywave_Atom_c, MP_Anywave_Block_c, MP_Dict_c");
  fprintf(stderr, "\n");
  fprintf(stderr, "\n");
  fprintf(stderr, "\n signal.wav : a signal in wave format, with as many channels as you want");
  fprintf(stderr, "\n");
  fprintf(stderr, "\n anywave_table.bin : a file defining an anywave table. ");
  fprintf(stderr, "\n   The waveforms must have either one channel, either as many channels as the signal");
  fprintf(stderr, "\n");
  fprintf(stderr, "\n dict.xml : a dictionary defining the atoms. Use a dictionary including anywave atoms in order to test them.");  
  fprintf(stderr, "\n");
  fprintf(stderr, "\n");
  fprintf(stderr, "\n");
}

int main( int argc, char **argv ) {

  char *signalFileName;
  char *tableFileName;
  char *dictFileName;

  bool serverOK;
  bool tableOK;
  bool atomOK;
  bool blockOK;
  bool dictOK;

  if (argc != 4) {
    usage();
    return(0);
  }

  signalFileName = argv[1];
  tableFileName = argv[2];
  dictFileName = argv[3];
  
  fprintf(stderr, "\n\nsignalFileName = %s",signalFileName); 
  fprintf(stderr, "\ntableFileName = %s",tableFileName); 
  fprintf(stderr, "\ndictFileName = %s\n",dictFileName); 
  
  
  serverOK = MP_Anywave_Server_c::test();

  tableOK = MP_Anywave_Table_c::test( tableFileName );

  atomOK = MP_Anywave_Atom_c::test( tableFileName );

  blockOK = MP_Anywave_Block_c::test( signalFileName, 25, tableFileName);

  dictOK = MP_Dict_c::test( signalFileName, dictFileName);  

  fprintf(stderr, "\n");
  if (serverOK) {
    fprintf(stderr, "\nTEST MP_Anywave_Server_c : OK" );
  } else {
    fprintf(stderr, "\nTEST MP_Anywave_Server_c : ERROR" );
  }
  if (tableOK) {
    fprintf(stderr, "\nTEST MP_Anywave_Table_c  : OK" );
  } else {
    fprintf(stderr, "\nTEST MP_Anywave_Table_c  : ERROR" );
  }
  if (atomOK) {
    fprintf(stderr, "\nTEST MP_Anywave_Atom_c   : OK" );
  } else {
    fprintf(stderr, "\nTEST MP_Anywave_Atom_c   : ERROR" );
  }
  if (blockOK) {
    fprintf(stderr, "\nTEST MP_Anywave_Block_c  : OK" );
  } else {
    fprintf(stderr, "\nTEST MP_Anywave_Block_c  : ERROR" );
  }
  if (dictOK) {
    fprintf(stderr, "\nTEST MP_Anywave_Dict_c   : OK" );
  } else {
    fprintf(stderr, "\nTEST MP_Anywave_Dict_c   : ERROR" );
  }

  fprintf(stderr, "\n");

  return( 0 );
}
