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
 * SVN log:
 *
 * $Author$
 * $Date$
 * $Revision$
 *
 */

#include <mptk.h>

int main( void ) {

  MP_Signal_c sig( 1, 8000 , 8000);
  unsigned long int check;
  MP_Dict_c *dico;
  MP_Book_c book;
  MP_Block_c *bl;
  int i;
  unsigned long int atomIdx, frameIdx, freqIdx;
  char str[1024];

  /*************************/
  /* Read signal form file */
  check = sig.read_from_float_file( "signals/2_atoms.flt" );

  /**************************/
  /* Build a new dictionary */
  dico = new MP_Dict_c( &sig, MP_DICT_SIG_HOOK );
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

  fprintf(stderr,"BOOK info :\n" ); fflush( stderr );
  book.info( stdout ); fflush( stdout );
  fprintf(stderr,"END BOOK INFO.\n\n" ); fflush( stderr );

  fprintf(stderr,"BOOK PRINT (TEXT MODE) :\n" ); fflush( stderr );
  book.print( stdout, MP_TEXT ); fflush( stdout );
  book.print( "book_from_test_mp.xml", MP_TEXT );

  fprintf(stderr,"RELOADED BOOK :\n" ); fflush( stderr );
  fprintf( stderr, "Reloading..." ); fflush( stderr );
  book.load( "book_from_test_mp.xml" );
  fprintf( stderr, "Done. [%lu] atoms have been reloaded.\n", book.numAtoms );
  fflush( stderr );
  book.print( stdout, MP_TEXT );
  book.print( "book_from_test_mp_after_reload.xml", MP_TEXT );
  fprintf(stderr,"END BOOK PRINT/RELOAD (TEXT).\n\n" ); fflush( stderr );

  fprintf(stderr,"BOOK PRINT (BINARY MODE) :\n" ); fflush( stderr );
  book.print( "book_from_test_mp.bin", MP_BINARY );
  fprintf( stderr, "Reloading..." ); fflush( stderr );
  book.load( "book_from_test_mp.bin" );
  fprintf( stderr, "Done. [%lu] atoms have been reloaded.\n", book.numAtoms );
  fflush( stderr );
  book.print( stdout, MP_TEXT ); fflush( stdout );
  book.print( "book_from_test_mp_after_reload_from_bin.xml", MP_TEXT );
  fprintf(stderr,"END BOOK PRINT/RELOAD (BINARY).\n\n" ); fflush( stderr );


  /*************************/
  /* Print the DICTIONNARY */

  fprintf(stderr,"DICO PRINT :\n" ); fflush( stderr );
  dico->print( stdout ); fflush( stdout );
  dico->print( "dico_from_test_mp.xml" );

  fprintf(stderr,"Deleting all blocks..." ); fflush( stderr );
  dico->delete_all_blocks();
  fprintf(stderr,"Done.\n" ); fflush( stderr );

  fprintf(stderr,"Reloading..." ); fflush( stderr );
  dico->add_blocks( "dico_from_test_mp.xml" );
  fprintf(stderr,"Done.\n" ); fflush( stderr );

  fprintf(stderr,"RELOADED DICO :\n" ); fflush( stderr );
  dico->print( stdout ); fflush( stdout );
  dico->print( "dico_from_test_mp_after_reload.xml" );

  fprintf(stderr,"Adding more blocks..." ); fflush( stderr );
  dico->add_blocks( "dico_from_test_mp.xml" );
  fprintf(stderr,"Done.\n" ); fflush( stderr );

  fprintf(stderr,"DUPLICATED DICO :\n" ); fflush( stderr );
  dico->print( stdout ); fflush( stdout );
  
  fprintf(stderr,"END DICO PRINT/RELOAD.\n\n" ); fflush( stderr );

  /****************************/
  /* TFMAP test               */
  {
    MP_TF_Map_c* tfmap = new MP_TF_Map_c( 640, 480, dico->signal->numChans,
					  0, dico->signal->numSamples,
					  0.0, 0.5 );
    MP_Mask_c mask( book.numAtoms );
    mask.reset_all_false();
    mask.set_true(0);
    mask.set_true(1);
    mask.set_true(2);
    book.info( stdout );
    tfmap->info( stdout );
    fflush( stdout );

    book.add_to_tfmap( tfmap, MP_TFMAP_SUPPORTS, &mask );
    tfmap->dump_to_file( "tfmap.flt", 0 );
    tfmap->dump_to_file( "tfmap_upsidedown.flt", 1 );

    tfmap->reset();
    book.add_to_tfmap( tfmap, MP_TFMAP_PSEUDO_WIGNER, &mask );
    tfmap->dump_to_file( "tfmap_wigner.flt", 1 );

    delete tfmap;
  }

  /* Clean the house */
  delete dico;

  return(0);
}
