/******************************************************************************/
/*                                                                            */
/*                              gpd_core.cpp                                  */
/*                                                                            */
/*                        Matching Pursuit Utilities                          */
/*                  														  */ 
/* Thomas Blumensath                                                          */                                                          
/* R�mi Gribonval                                                             */
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
 * $Author: sacha $
 * $Date: 2007-09-14 17:37:25 +0200 (ven., 14 sept. 2007) $
 * $Revision: 1151 $
 *
 */

#include "mptk.h"
#include <iostream>
#include <fstream>

/*	#ifndef GPD_CORE_H_
	#include <gp_core.h>
	#warning  "-------UNDEFINED GP_CORE_H_ IN GPD_CORE.cpp-------"
	#endif
	#ifndef GPD_CORE_H_
	#include <gpd_core.h>
	#warning  "-------UNDEFINED GPD_CORE_H_ IN GPD_CORE.cpp-------"
	#endif
 */

using namespace std;

/***************************/
/* CONSTRUCTORS/DESTRUCTOR */
/***************************/

/**********************/
/* Factory function:*/
/********************/

/* - signal+book only: */
GPD_Core_c* GPD_Core_c::create( MP_Signal_c *setSignal, GP_Double_Index_Book_c *setBook ) {

	const char* func = "GPD_Core_c::init(2 args)";

	GPD_Core_c* newCore;

	/* Check for NULL entries */
	if ( setSignal == NULL ) {
		mp_error_msg( func, "Can't initialize a GPD_Core_c from a NULL signal.\n" );
		return( NULL );
	}

	if ( setBook == NULL ) {
		mp_error_msg( func, "Can't initialize a GPD_Core_c from a NULL dictionary vector.\n" );
		return( NULL );
	}

	/* Instantiate and check */
	newCore = new GPD_Core_c();

	if ( newCore == NULL ) {
		mp_error_msg( func, "Failed to create a new gpd core.\n" );
		return( NULL );
	}
	/* Plug the book */
	newCore->book = setBook;

	/* Plug the signal */
	newCore->residual = setSignal;
	newCore->residualEnergy = newCore->initialEnergy = setSignal->energy;
	newCore->decay.clear();
	newCore->decay.append( newCore->initialEnergy );


	return( newCore );
}

/* - signal+approximant+dict */
GPD_Core_c* GPD_Core_c::create( MP_Signal_c *setSignal, GP_Double_Index_Book_c *setBook, MP_Dict_c *setDict ) {

	const char* func = "GPD_Core_c::init(3 args)";
	GPD_Core_c* newCore;

	newCore = GPD_Core_c::create( setSignal, setBook );
	if ( newCore == NULL ) {
		mp_error_msg( func, "Failed to create a new gpd core.\n" );
		return( NULL );}

	if ( setDict ) newCore->change_dict(setDict);
	else {
		mp_error_msg( func, "Could not use a NULL dictionary.\n" );
		return( NULL );}

	//cerr << "gradient memory space == " << 3*setSignal->numChans*setDict->maxFilterLen << endl;
	newCore->gradient = new MP_Real_t[3*setSignal->numChans*setDict->maxFilterLen];
	if (!newCore->gradient){
		mp_error_msg( func, "Could not allocate the gradient buffer.\n" );
		return( NULL );
	}

	newCore->tmpBuffer = new MP_Real_t[3*setSignal->numChans*setDict->maxFilterLen];
	if (!newCore->tmpBuffer){
		mp_error_msg( func, "Could not allocate the tmpBuffer buffer.\n" );
		return( NULL );
	}

	return( newCore );
}

GPD_Core_c* GPD_Core_c::create( MP_Signal_c *setSignal, GP_Double_Index_Book_c *setBook, MP_Signal_c* setApproximant )
{
	const char* func = "GPD_Core_c::init(3 args)";
	GPD_Core_c* newCore;
	newCore = GPD_Core_c::create( setSignal, setBook );
	if ( newCore == NULL ) {
		mp_error_msg( func, "Failed to create a new gpd core.\n" );
		return( NULL );}

	if ( setApproximant ) newCore->plug_approximant(setApproximant);
	else {
		mp_error_msg( func, "Could not use a NULL approximant.\n" );
		return( NULL );}


	return( newCore );
}



/********************/
/* NULL constructor */
GPD_Core_c::GPD_Core_c() {

	/* File names */
	bookFileName  =  NULL;
	approxFileName   = NULL;

	/* Manipulated objects */
	dict = NULL;
	book = NULL;
	approximant = NULL;
	touchBook = NULL;

}

/**************/
/* Destructor */
GPD_Core_c::~GPD_Core_c() {
	if (bookFileName) free(bookFileName);
	if (approxFileName) free(approxFileName);
	if (touchBook) delete(touchBook);
	if (gradient) delete[] gradient;
	if (tmpBuffer) delete[] tmpBuffer;
}


/***************************/
/* SET OBJECTS             */
/***************************/

/************/
/* Set dict */
MP_Dict_c* GPD_Core_c::change_dict( MP_Dict_c *setDict ) {

	const char* func = "GPD_Core_c::change_dict( MP_Dict_c * )";

	MP_Dict_c* oldDict = dict;
	if ( setDict->signal == NULL ) {
		/* If there was a non-NULL dictionary before, detach the residual
     to avoid its destruction: */
		if ( oldDict ) residual = oldDict->detach_signal();

		/* Set the new dictionary: */
		dict = setDict;

		/* Plug dictionary to signal: */
		plug_dict_to_signal();

		return( oldDict );}

	else{ mp_error_msg( func, "Could not set a dictionary with a pluged signal.\n" );
	return( NULL );}
}

void GPD_Core_c::plug_dict_to_signal(){

	const char* func = "GPD_Core_c::plug_dict_to_signal()";
	/* If the new dictionary is not NULL, replug the residual: */
	if ( dict ) {
		if (residual){
			dict->plug_signal( residual );
		} else mp_error_msg( func, "Could not plug a dictionary with a null signal.\n" );
	} else mp_error_msg( func, "Could not plug a null dictionary .\n" );

	/* Note:
     - if a NULL dictionary is given, the residual is kept alive
     in the residual variable;
     - at the first use of set_dict(dict), the oldDict is NULL and
     the residual is copy-constructed from the signal at
     the gpdCore->init(signal,book) pahse. */

}


void GPD_Core_c::init_dict(){
	dict = MP_Dict_c::init();
}


int GPD_Core_c::add_default_block_to_dict( const char* blockName ){
	if (NULL!= dict) return dict->add_default_block(blockName);
	else return 0;
}
/********************/
/* Plug approximant */

void GPD_Core_c::plug_approximant( MP_Signal_c *setApproximant  ) {

	const char* func = "Toggle_approximant";

	if ( book ){
		approximant = setApproximant;
		if ( approximant == NULL ) {
			mp_error_msg( func, "Failed to create an approximant in the gpd core."
					" Returning NULL.\n" );
		} else
			// Rebuild from the book
			book->substract_add( NULL, approximant, NULL );
	}
}

/***************************/
/* OTHER METHODS           */
/***************************/

/********************************/
/* Save the book/residual/decay */
void GPD_Core_c::save_result() {

	const char* func = "Save info";

	/* - Save the book: */
	if(bookFileName)
	{
		if ( (strcmp( bookFileName, "-" ) != 0) )
		{
			book->print( bookFileName, MP_BINARY);
			if ( verbose ) { if (numIter >0 ) mp_info_msg( func, "At iteration [%lu] : saved the book to file [%s].\n", numIter, bookFileName );
			else mp_info_msg( func, "Saved the book to file [%s]...\n", bookFileName );
			}
		}
	}
	/* - Save the approximant: */
	if ( approxFileName && approximant ) {
		if (approximant->wavwrite( approxFileName ) == 0 ) {
			mp_error_msg( func, "Can't write approximant signal to file [%s].\n", approxFileName );
		} else

			if ( verbose ){ if (numIter >0 ) mp_info_msg( func, "At iteration [%lu] : saved the approximant to file [%s].\n", numIter , approxFileName );
			else {mp_info_msg( func, "Saved the approximant signal to file [%s]...\n", approxFileName );
			mp_info_msg( func, "The resulting signal has [%lu] samples in [%d] channels, with sample rate [%d]Hz.\n",
					book->numSamples, book->numChans, book->sampleRate );

			}

			}
	}

	/* - Save the residual: */
	if ( resFileName ) {
		if ( residual->wavwrite( resFileName ) == 0 ) {
			mp_error_msg( func, "Can't write residual signal to file [%s].\n", resFileName );
		} else
			if ( verbose ) {if (numIter >0 ) mp_info_msg( func, "At iteration [%lu] : saved the residual signal to file [%s].\n", numIter , resFileName );
			else mp_info_msg( func, "Saved the residual signal to file [%s]...\n", resFileName );

			}

	}
	/* - the decay: */
	if ( decayFileName ) {
		unsigned long int nWrite;
		nWrite = save_decay(decayFileName);
		if( nWrite != (numIter+1) ) {
			mp_warning_msg( func, "Wrote less than the expected number of doubles to the energy decay file.\n" );
			mp_warning_msg( func, "([%lu] expected, [%lu] written.)\n", numIter+1, nWrite );
		}
		if ( verbose ) mp_info_msg( func, "At iteration [%lu] : saved the energy decay to file [%s].\n", numIter , decayFileName );
	}

}

/*************************/
/* Make one GP iteration */
unsigned short int GPD_Core_c::step() {

	//fprintf( stdout, " GPD_Core_c::step()\n" );
	const char* func = "GPD_Core_c::step()";
	//int chanIdx;
	MP_Atom_c *atom;
	unsigned int numAtoms, size;
	unsigned long int t, offset;
	GP_Pos_Range_Sub_Book_Iterator_c iter;
	MP_Real_t alpha, enCorr, enGrad;
	MP_Support_t gradSupport;
	MP_Chan_t c;
	ofstream file;

	/* Reset the state info */
	state = 0;

	mp_debug_msg( MP_DEBUG_MPD_LOOP, func, "ENTERING iteration [%lu]/[%lu].\n", numIter+1, stopAfterIter );
	mp_debug_msg( MP_DEBUG_MPD_LOOP, func, "Next report hit is [%lu].\n", nextReportHit );
	mp_debug_msg( MP_DEBUG_MPD_LOOP, func, "Next save hit is   [%lu].\n", nextSaveHit );
	mp_debug_msg( MP_DEBUG_MPD_LOOP, func, "Next snr hit is    [%lu].\n", nextSnrHit );
	mp_debug_msg( MP_DEBUG_MPD_LOOP, func, "SNR is [%g]/[%g].\n",
			10*log10(currentSnr), 10*log10(stopAfterSnr) );

	/* Check if a signal is present */
	if ( residual == NULL )
	{
		mp_error_msg( func, "There is no signal in the dictionary. You must"
				" plug or copy a signal before you can iterate.\n" );
		return( 1 );
	}

	/* 1/ refresh the inner products
	 * 2/ create the max atom and store it
	 * 3/ substract it from the signal and add it to the approximant
	 */

	/** 1/ (Re)compute the inner products according to the current 'touch' indicating where the signal
	 * may have been changed after the last update of the inner products */

	dict->update(book->sortBook);

	/** 2/ Create the max atom and store it in the book */
	numAtoms = dict->create_max_gp_atom( &atom );
	//cerr << "found atom = " << endl;
	//atom->info();
	if ( numAtoms == 0 )
	{
		mp_error_msg( func, "The Gradient Pursuit iteration failed. Dictionary, book"
				" and signal are left unchanged.\n" );
		return( 1 );
	}

	if ( book->append( atom ) != 1 )
	{
		mp_error_msg( func, "Failed to append the max atom to the book.\n" );
		return( 1 );
	}

	/* 3/ compute gradient*/
	if (touchBook)
		delete touchBook;
	touchBook = book->get_neighbours(atom, dict);
	//cerr << "touchBook->begin() = " << endl;
	//touchBook->begin()->info(stderr);

	gradSupport.len = touchBook->build_waveform_corr(dict, gradient, tmpBuffer);

	gradSupport.pos = residual->numSamples;
	size = 0;
	for (iter = touchBook->begin(); iter !=touchBook->end(); iter.go_to_next_frame()){
		//iter->info( stderr );
		if (gradSupport.pos > iter->support[0].pos)
			gradSupport.pos = iter->support[0].pos;
		size++;
	}

	for (c=0; c<dict->signal->numChans; c++){
		offset = c*gradSupport.len;
		enCorr = 0;
		enGrad = 0;

		for (iter = touchBook->begin(); iter !=touchBook->end(); ++iter)
			enCorr = enCorr + (iter->corr[c])*(iter->corr[c]);

		for (t = 0; t <gradSupport.len; t++)
			enGrad = enGrad + gradient[t+offset]*gradient[t+offset];

		alpha = enCorr/enGrad;

		// dump the gradient
//		if (numIter == 30){
//			file.open("res_old.txt");
//			for (t = 0; t < residual->numSamples; t++)
//				file << residual->channel[0][t] << endl;
//			file.close();
//
//			file.open("gradient.txt");
//			for (t = 0; t < gradSupport.len; t++)
//				file << gradient[t] << endl;
//			file.close();
//			cerr << "enCorr == " << enCorr << "\tenGrad == " << enGrad << "\talpha == " << alpha << endl;
//		}

		for (iter = touchBook->begin(); iter !=touchBook->end(); ++iter)
			iter->amp[c] += (iter->corr[c])*alpha;

		offset += gradSupport.pos;
		for (t = 0; t <gradSupport.len; t++)
			residual->channel[c][t+offset] -= gradient[t]*alpha;

		residual->refresh_energy();

		dict->touch[c].pos = touchBook->begin()->support[c].pos;
		dict->touch[c].len = 0;
		for (iter = touchBook->begin(); iter !=touchBook->end(); ++iter)
			if (dict->touch[c].len < iter->support[c].pos+dict->block[iter->blockIdx]->filterLen-1)
				dict->touch[c].len = iter->support[c].pos+dict->block[iter->blockIdx]->filterLen-1;
		dict->touch[c].len = dict->touch[c].len-dict->touch[c].pos+1;

		mp_debug_msg(MP_DEBUG_MPD_LOOP, func, "Touch support for next iteration = %lu %lu\n",
				dict->touch[c].pos, dict->touch[c].len);
	}
	/* Note: if approximant is NULL, no computation
 					     will be performed on it. */

	residualEnergyBefore = residualEnergy;
	residualEnergy = (double)residual->energy;
	if ( decayFileName ) decay.append( residualEnergy );

	numIter++;

	/* 6) Check for possible breakpoints: */
	if ( numIter == nextSnrHit ) {
		currentSnr = initialEnergy / residualEnergy;
		nextSnrHit += snrHit;
	}

	if ( numIter == nextReportHit ) {
		mp_progress_msg( "At iteration", "[%lu] : the residual energy is [%g] and the SNR is [%g].\n",
				numIter, residualEnergy, 10*log10( initialEnergy / residualEnergy ) );
		nextReportHit += reportHit;
	}

	if ( numIter == nextSaveHit ) { save_result(); nextSaveHit += saveHit; }

	if ( numIter == ULONG_MAX ) state = ( state | MP_ITER_EXHAUSTED );

	/* 3) Check for possible stopping conditions: */
	if ( useStopAfterIter && (numIter >= stopAfterIter) )   state = ( state | MP_ITER_CONDITION_REACHED );
	if ( useStopAfterSnr  && (currentSnr >= stopAfterSnr) ) state = ( state | MP_SNR_CONDITION_REACHED );
	if ( residualEnergy < 0.0 ) state = ( state | MP_NEG_ENERGY_REACHED );
	if ( residualEnergy >= residualEnergyBefore ) {
		mp_warning_msg( func, "Iteration [%lu] increases the energy of the residual ! Before: [%g] Now: [%g]\n",
				numIter, residualEnergyBefore, residualEnergy );
		mp_warning_msg( func, "Last atom found is sent to stderr.\n" );
		book->atom[book->numAtoms-1]->info( stderr );

		// dump the residual
		file.open("res.txt");
		for (t = 0; t < residual->numSamples; t++)
			file << residual->channel[0][t] << endl;
		file.close();
		//state = ( state | MP_INCREASING_ENERGY );
	}
	/*if ( (residualEnergyBefore - residualEnergy) < 5e-4 ) {
    mp_warning_msg( func, "At iteration [%lu]: the energy decreases very slowly !"
		    " Before: [%g] Now: [%g] Diff: [%g]\n",
		    numIter, residualEnergyBefore, residualEnergy, residualEnergyBefore - residualEnergy );
    mp_warning_msg( func, "Last atom found is sent to stderr.\n" );
    book->atom[book->numAtoms-1]->info( stderr );*/
	/* BORK BORK BORK */
	/* Export the considered signal portion */
	/* RES */
	/* MP_Signal_c *exportSig = MP_Signal_c::init( dict->signal, book->atom[book->numAtoms-1]->support[0] );
    if ( exportSig != NULL ) exportSig->dump_to_double_file( "res.dbl" );
    fprintf( stderr, "Exported [%hu] channels from support p[%lu]l[%lu] to file [res.dbl].\n",
	     exportSig->numChans,
	     book->atom[book->numAtoms-1]->support[0].pos,  book->atom[book->numAtoms-1]->support[0].len );*/
	/* ATOM */
	/*MP_Signal_c *atomSig = MP_Signal_c::init( book->atom[book->numAtoms-1], dict->signal->sampleRate );
    if ( atomSig != NULL ) atomSig->dump_to_double_file( "atom.dbl" );
    fprintf( stderr, "Exported [%hu] channels from atom of length [%lu] to file [atom.dbl].\n",
    atomSig->numChans, atomSig->numSamples );*/
	/* SUM */
	/*unsigned long int i;
    for ( i = 0; i < (exportSig->numSamples*exportSig->numChans); i++ ) {
      exportSig->storage[i] = exportSig->storage[i] + atomSig->storage[i];
    }
    exportSig->dump_to_double_file( "bad_signal.dbl" );
    exportSig->wavwrite( "bad_signal.wav" );
    fprintf( stderr, "Exported [%hu] channels from support p[%lu]l[%lu] to file [bad_signal.dbl].\n",
	     exportSig->numChans,
	     book->atom[book->numAtoms-1]->support[0].pos,  book->atom[book->numAtoms-1]->support[0].len );
    fflush( stderr );
    exit( 0 );*/
	/* \BORK BORK BORK */
	/*}*/

	mp_debug_msg( MP_DEBUG_MPD_LOOP, func, "EXITING iteration [%lu]/[%lu].\n", numIter, stopAfterIter );
	mp_debug_msg( MP_DEBUG_MPD_LOOP, func, "Next report hit is [%lu].\n", nextReportHit );
	mp_debug_msg( MP_DEBUG_MPD_LOOP, func, "Next save hit is   [%lu].\n", nextSaveHit );
	mp_debug_msg( MP_DEBUG_MPD_LOOP, func, "Next snr hit is    [%lu].\n", nextSnrHit );
	mp_debug_msg( MP_DEBUG_MPD_LOOP, func, "SNR is [%g]/[%g].\n",
			10*log10(currentSnr), 10*log10(stopAfterSnr) );

	return( state );
}



/******************************************/
/* Perform one gradient pursuit iteration */
int GPD_Core_c::iterate_gp( MP_Book_c *book , MP_Dict_c *dict, MP_Signal_c *sigRecons )
{

	const char* func = "GPD_Core_c::iterate_gp(...)";
	//int chanIdx;
	//MP_Atom_c *atom;
	//unsigned int numAtoms;


	/* NEW OBJECTS REQUIRED ARE:
	 * MP_Real_t waveform_energy[];
	 * MP_Real_t ip_energy[];
	 * MP_Real_t gain[];
	 * 0/ MP_Support_t* touch;
	 * 1/ GP_Book_c* gp_book
	 * 2/ GpIdx_c* gpIdx
	 * 3/ Waveform_c* waveforem
	 * 4/ Gp_Atom_c* gp_atom
	 */

	/* Check if a signal is present */
	if ( residual == NULL )
	{
		mp_error_msg( func, "There is no signal in the dictionary. You must"
				" plug or copy a signal before you can iterate.\n" );
		return( 1 );
	}
	/* 1/ dict->update(touch,gp_book) , or dict->update_ip(touch,BkIdx)
	 * 2/ dict->create_gp_atom(&gp_atom)
	 * 3/ gp_book->add(gp_atom) //Also check if atom is in book already
	 * 3/ BkIdx = gp_book->get_neighbours(gp_atom, parameter)
	 * 4/ support = calculate_waveform(&waveform) calculate waveform
	 * 5/ calculate gain for the waveform
	 * 6/ Update coefficients in book
	 * 7/ substract the wavforem from the signal and add the wavform to the approximant
	 * 8/ calculate new touch value
	 */

	/******************************************************************/
	/** 1/ (Re)compute the inner products according to the current 'touch' indicating where the signal
	 * may have been changed after the last update of the inner products. Also update the inner products
	 * which are in the book
	 *
	 * Either we hand a handle to gp_book to update ips in book
	 * Or, we need to add a function to gp_book to return BookIdx elemet,
	 * Or, we keep bookIdx consistant with gp_book
	 * Or, does bookIdx contain the gp_book?
	 * Or, if gp-book contains bookIdx, then use either.
	 */
	// 	dict->update(touch,gp_book->bookIdx);
	//    OR
	// 	dict->update(touch,gp_book);

	/******************************************************************/
	/** 2/ Get the max atom
	 */
	// 	dict->create_gp_atom(&gp_atom);

	/******************************************************************/
	/* 3/ Add the atom to the book and get the local atoms to be updated by gradient descend*/
	/* Either jointly
	 */
	//    BkIdx = gp_book->add(gp_atom, parameter); //Individual or jointly
	/* OR Individual*/
	//   	 		gp_book->add(gp_atom);
	//    BkIdx = gp_book->get_neighbours(gp_atom, selectMethod)

	/******************************************************************/
	/* 4/ Build the waveform and calculate its energy
	 */
	// EITHER the constructor of waveform builds the waveform,
	//    MP_Waveform_c 	  waveform(BkIdx)
	// OR we call build explicitely
	//    MP_Waveform_c 	  waveform
	//		 	 		  waveform->build_waveform(BkIdx)
	//
	//
	//    signal_energy 	= waveform->energy
	//    support 	  	= waveform->support

	/******************************************************************/
	/* 5/ Calculate stepsize
	 */
	// EITHER, calculate ip_energy in gp_book or BkIdx class
	// 	ip_energy  = gp_book->ip_energy(BkIdx);
	// OR,
	//	ip = energy = 0;
	//	i, loop through BkIdx entries
	//    ip_energy += BkIdx[i]->ip * BkIdx[i]->ip
	// 	making sure we deal with size of ip apropriately.
	//
	// gain 		= ip_energy / waveform_energy;

	/******************************************************************/
	/* 6/ Update the coefficients in the book
	 * Do this in the gp_book or the calculator?
	 * The atom functionality has been designed,
	 * we just need to loop through the once in BkIdx
	 */
	// gp_book -> update_coefs(gain,BkIdx)

	/******************************************************************/
	/* 7/ Substract the atom's waveform from the analyzed signal
	 */
	// 	waveform->substract_add( residual , sigRecons ,gain);

	/******************************************************************/
	/* 8/ Keep track of the support where the signal has been modified
	 */
	//for ( chanIdx=0; chanIdx < atom->numChans; chanIdx++ )
	//{
	//     touch[chanIdx].pos = waveform->support[chanIdx].pos;
	//     [chanIdx].len = waveform->support[chanIdx].len;
	//}

	return( 0 );
}

/**********************************/
/* Check if some objects are null */
MP_Bool_t GPD_Core_c::can_step( void ) {
	const char* func = "can_step";
	/* Check that all of dict, book and signal are not NULL */
	if (dict  &&  book) {
		if (residual) return true;
		else { mp_error_msg( func,"book or dict are not set .\n");
		return false;}
	}
	else { mp_error_msg( func,"dict has no signal plugged .\n");
	return false;}
}


/* Infos */
void GPD_Core_c::info_conditions( void )
{

	const char* func = "Conditions";

	if ( useStopAfterIter ) mp_info_msg( func, "This run will perform [%lu] iterations, using [%lu] atoms.\n",
			stopAfterIter, dict->num_atoms() );
	if ( useStopAfterSnr ) mp_info_msg( func, "This run will iterate until the SNR goes above [%g], using [%lu] atoms.\n",
			10*log10( stopAfterSnr ), dict->num_atoms() );
	if ( bookFileName ) {
		if ( strcmp( bookFileName, "-" ) == 0 ) mp_info_msg( func, "The resulting book will be written"
				" to the standard output [%s].\n", bookFileName );
		else mp_info_msg( func, "The resulting book will be written to book file [%s].\n", bookFileName );
	}
	if ( resFileName ) mp_info_msg( func, "The residual will be written to file [%s].\n", resFileName );
	else mp_info_msg( func, "The residual will not be saved.\n" );
	if ( decayFileName ) mp_info_msg( func, "The energy decay will be written to file [%s].\n", decayFileName );
	else mp_info_msg( func, "The energy decay will not be saved.\n" );

}

void GPD_Core_c::info_result( void )
{
	const char* func = "Result";
	mp_info_msg( func, "[%lu] iterations have been performed.\n", numIter );
	mp_info_msg( func, "([%lu] atoms have been selected out of the [%lu] atoms of the dictionary.)\n",
			numIter, dict->num_atoms() );
	mp_info_msg( func, "The initial signal energy was [%g].\n", initialEnergy );
	mp_info_msg( func, "The residual energy is now [%g].\n", residualEnergy );
	mp_info_msg( func, "The SNR is now [%g].\n", 10*log10( initialEnergy / residualEnergy ) );
}

void GPD_Core_c::set_save_hit( const unsigned long int setSaveHit,
		const char* setBookFileName,
		const char* setResFileName,
		const char* setDecayFileName )
{
	const char* func = "set_save_hit";
	char* newBookFileName = NULL;
	char* newResFileName = NULL;
	char* newDecayFileName =NULL;

	if (setSaveHit>0)saveHit = setSaveHit;
	if (setSaveHit>0)nextSaveHit = numIter + setSaveHit;

	/*reallocate memory and copy name */
	if (setBookFileName && strlen(setBookFileName) > 1 ) {
		newBookFileName = (char*) realloc((void *)bookFileName  , ((strlen(setBookFileName)+1 ) * sizeof(char)));
		if ( newBookFileName == NULL )
		{
			mp_error_msg( func,"Failed to re-allocate book file name to store book [%s] .\n",
					setBookFileName );
		}
		else bookFileName = newBookFileName;
		strcpy(bookFileName, setBookFileName);
	}

	if ( setResFileName && strlen(setResFileName) > 1 ){
		newResFileName = (char*) realloc((void *)resFileName  , ((strlen(setResFileName)+1 ) * sizeof(char)));
		if ( newResFileName == NULL )
		{
			mp_error_msg( func,"Failed to re-allocate residual file name to store residual [%s] .\n",
					setResFileName);
		}
		else resFileName = newResFileName;
		strcpy(resFileName, setResFileName); }

	if ( setDecayFileName && strlen(setDecayFileName)> 1 ){
		newDecayFileName = (char*) realloc((void *)decayFileName  , ((strlen(setDecayFileName)+1 ) * sizeof(char)));
		if ( newDecayFileName == NULL )
		{
			mp_error_msg( func,"Failed to re-allocate residual file name to store residual [%s] .\n",
					setDecayFileName);
		}
		else decayFileName = newDecayFileName;
		strcpy(decayFileName, setDecayFileName); }

}

void GPD_Core_c::addCustomBlockToDictionary(map<string, string, mp_ltstring>* setPropertyMap){
	dict->create_block(dict->signal , setPropertyMap);
}

bool GPD_Core_c::save_dict( const char* dictName ){
	if (dict) { dict->print(dictName);
	return true;
	} else return false;
}

void GPD_Core_c::get_filter_lengths(vector<unsigned long int> * filterLengthsVector){
	if (dict && filterLengthsVector->size() == dict->numBlocks) {

		for ( unsigned int i= 0; i < dict->numBlocks; i++) {
			filterLengthsVector->at(i)= dict->block[i]->filterLen;
		}
	}
}
