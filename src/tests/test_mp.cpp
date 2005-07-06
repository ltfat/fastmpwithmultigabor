/******************************************************************************/
/*                                                                            */
/*                                test_mp.cpp                                 */
/*                                                                            */
/*                      Matching Pursuit Testing Suite                        */
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
/*
 * CVS log:
 *
 * $Author: sacha $
 * $Date: 2005/07/04 09:55:24 $
 * $Revision: 1.15 $
 *
 */

#include <mptk.h>

int main( void ) {

  MP_Signal_c sig( 1, 8000 , 8000);
  MP_Dict_c *dico;
  MP_Book_c book;
  MP_Block_c *bl;
  int i;
  unsigned long int atomIdx, frameIdx, freqIdx;
  char str[1024];

  /*************************/
  /* Read signal form file */
  sig.read_from_float_file( "signals/2_atoms.flt" );

  /**************************/
  /* Build a new dictionary */
  dico = new MP_Dict_c( sig );
  add_gabor_block( dico,  32, 32, 256, DSP_GAUSS_WIN, DSP_GAUSS_DEFAULT_OPT );
  add_gabor_block( dico,  64, 32, 256, DSP_HAMMING_WIN, 0.0 );
  add_gabor_block( dico, 128, 32, 256, DSP_HAMMING_WIN, 0.0 );
  add_gabor_block( dico, 256, 32, 256, DSP_HAMMING_WIN, 0.0 );

  /*****************************/
  /* Init the matching pursuit */
  dico->update_all();

  /* Output inner products to file */
  // dico->dump_ip( "signals/out_2atoms_specgram.dbl" );
  /* Check the max */
  fprintf( stdout, "Max is now found in block %u\n",
	   dico->blockWithMaxIP );
  bl = dico->block[dico->blockWithMaxIP];
  atomIdx = bl->maxAtomIdx;
  fprintf( stdout, "    with atom index %lu\n", atomIdx );
  frameIdx = atomIdx / bl->numFilters;
  freqIdx  = atomIdx - frameIdx*bl->numFilters;
  fprintf( stdout, "    (frame #%lu , frequency #%lu )\n", frameIdx, freqIdx );

  /****************/
  /* Find 2 atoms */
  book.numSamples = dico->signal->numSamples;
  fprintf( stdout, "STARTING ITERATIONS:\n" );
  for ( i=0; i<2; i++ ) {

    fprintf( stdout, "------ ITERATION [%i]...\n", i );
    dico->iterate_mp( &book , NULL );

    /* Check the signal */
    sprintf( str, "signals/2_atoms_after_iter_%d.flt", i );
    dico->signal->dump_to_float_file( str );
    /* Check the spectrogram */
    sprintf( str, "signals/2_atoms_spec_iter_%d.dbl", i );
    // dico->dump_ip( str );
    /* Check the support */
    fprintf( stdout, "Touched support: %lu %lu\n",
	     dico->touch[0].pos, dico->touch[0].len );
    /* Check the max */
    fprintf( stdout, "Max is now found in block %u\n",
	     dico->blockWithMaxIP );
    bl = dico->block[dico->blockWithMaxIP];
    atomIdx = bl->maxAtomIdx;
    fprintf( stdout, "    with atom index %lu\n", atomIdx );
    frameIdx = atomIdx / bl->numFilters;
    freqIdx  = atomIdx - frameIdx*bl->numFilters;
    fprintf( stdout, "    (frame #%lu , frequency #%lu )\n", frameIdx, freqIdx );

  }
  fprintf( stdout, "------ DONE.\n" );


  /****************************/
  /* I/O TEST                 */
  /****************************/

  /******************/
  /* Print the BOOK */

  fprintf(stderr,"BOOK info :\n" );
  book.info( stdout );
  fprintf(stderr,"END BOOK INFO.\n\n" );

  fprintf(stderr,"BOOK PRINT (TEXT MODE) :\n" );
  book.print( stdout, MP_TEXT );
  book.print( "book_from_test_mp.xml", MP_TEXT );

  fprintf(stderr,"RELOADED BOOK :\n" );
  fprintf( stderr, "Reloading..." );
  book.load( "book_from_test_mp.xml" );
  fprintf( stderr, "Done. [%lu] atoms have been reloaded.\n", book.numAtoms );
  fflush( stderr );
  book.print( stdout, MP_TEXT );
  book.print( "book_from_test_mp_after_reload.xml", MP_TEXT );
  fprintf(stderr,"END BOOK PRINT/RELOAD (TEXT).\n\n" );

  fprintf(stderr,"BOOK PRINT (BINARY MODE) :\n" );
  book.print( "book_from_test_mp.bin", MP_BINARY );
  fprintf( stderr, "Reloading..." );
  book.load( "book_from_test_mp.bin" );
  fprintf( stderr, "Done. [%lu] atoms have been reloaded.\n", book.numAtoms );
  fflush( stderr );
  book.print( stdout, MP_TEXT );
  book.print( "book_from_test_mp_after_reload_from_bin.xml", MP_TEXT );
  fprintf(stderr,"END BOOK PRINT/RELOAD (BINARY).\n\n" );


  /*************************/
  /* Print the DICTIONNARY */

  fprintf(stderr,"DICO PRINT :\n" );
  dico->print( stdout );
  dico->print( "dico_from_test_mp.xml" );

  fprintf(stderr,"Deleting all blocks..." );
  dico->delete_all_blocks();
  fprintf(stderr,"Done.\n" );

  fprintf(stderr,"Reloading..." );
  dico->add_blocks( "dico_from_test_mp.xml" );
  fprintf(stderr,"Done.\n" );

  fprintf(stderr,"RELOADED DICO :\n" );
  dico->print( stdout );
  dico->print( "dico_from_test_mp_after_reload.xml" );

  fprintf(stderr,"Adding more blocks..." );
  dico->add_blocks( "dico_from_test_mp.xml" );
  fprintf(stderr,"Done.\n" );

  fprintf(stderr,"DUPLICATED DICO :\n" );
  dico->print( stdout );
  
  fprintf(stderr,"END DICO PRINT/RELOAD.\n\n" );

  /****************************/
  /* TFMAP test               */
  {
    MP_TF_Map_c* tfmap = new MP_TF_Map_c( 640, 480, dico->signal->numChans,
					  0.0, 0.0, dico->signal->numSamples,
					  0.5 );
    char mask[book.numAtoms];
    unsigned long int n;
    for ( n = 0; n < book.numAtoms; n++ ) mask[n] = 0;
    mask[0] = 1;
    mask[1] = 1;
    mask[2] = 1;
    tfmap->info( stdout );

    book.add_to_tfmap( tfmap, mask );

    tfmap->dump_to_float_file( "signals/tfmap.flt", 1 );
    delete tfmap;
  }

  /* Clean the house */
  delete dico;

  return(0);
}
