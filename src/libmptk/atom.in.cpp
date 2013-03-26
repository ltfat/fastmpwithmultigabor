/******************************************************************************/
/*                                                                            */
/*                                atom.cpp                                    */
/*                                                                            */
/*                        Matching Pursuit Library                            */
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
 * $Author: broy $
 * $Date: 2007-07-03 17:05:04 +0200 (Tue, 03 Jul 2007) $
 * $Revision: 1085 $
 *
 */

/******************************************/
/*                                        */
/* atoms.cpp: methods for class MP_Atom_c */
/*                                        */
/******************************************/

#include "mptk.h"
#include "mp_system.h"
#include "mp_pthreads_barrier.h"

#cmakedefine MULTITHREAD 1

/***************************/
/* CONSTRUCTORS/DESTRUCTOR */
/***************************/

/********************/
/* Void constructor */
MP_Atom_c::MP_Atom_c( MP_Dict_c* argdict ) {
	numChans = 0;
	support = NULL;
	numSamples = 0;
	amp = NULL;
	corr = NULL;
	totalChanLen = 0;
	blockIdx = 0;
	dict = argdict;
}


/***********************/
/* Internal allocation */
int MP_Atom_c::alloc_atom_param( const MP_Chan_t setNumChans ) {

	const char *func = "MP_Atom_c::alloc_atom_param(numChans)";
	/* Check the allocation size */
	if ((double)MP_MAX_SIZE_T / (double)setNumChans / (double)sizeof(MP_Real_t) <= 1.0) {
		mp_error_msg( func, "numChans [%lu] x sizeof(MP_Real_t) [%lu]"
				" is greater than the maximum value for a size_t [%lu]. Cannot use calloc"
				" for allocating space for the arrays. The arrays stay NULL.\n",
				setNumChans, sizeof(MP_Real_t), MP_MAX_SIZE_T);
		return( 1 );
	}

	/* Allocate the support array */

	if ( (support = (MP_Support_t*) calloc( setNumChans, sizeof(MP_Support_t) ) ) == NULL ) {
		mp_warning_msg( func, "Can't allocate support array"
				" in atom with [%u] channels. Support array and param array"
				" are left NULL.\n", setNumChans );
		return( 1 );
	}
	else numChans = setNumChans;

	/* Allocate the amp array */
	if ( (amp = (MP_Real_t*)calloc( numChans, sizeof(MP_Real_t)) ) == NULL ) {
		mp_warning_msg( func, "Failed to allocate the amp array for a new atom;"
				" amp stays NULL.\n" );
		return( 1 );
	}

	/* Allocate the ampCache array */
	if ( (ampCache = (MP_Real_t*)calloc( numChans, sizeof(MP_Real_t)) ) == NULL ) {
		mp_warning_msg( func, "Failed to allocate the ampCache array for a new atom;"
				" ampCache stays NULL.\n" );
		return( 1 );
	}

	/* Allocate the corr array */
	if ((corr = (MP_Real_t *) calloc(numChans, sizeof(MP_Real_t))) == NULL) {
		mp_warning_msg(func, "Failed to allocate the corr array for a new atom;"
				" corr stays NULL.\n");
		return (1);
	}
	return (0);
}

/******************************/
/* Initialiser from XML data  */
int MP_Atom_c::init_fromxml(TiXmlElement* xmlobj){
	const char *func = "MP_Atom_c::init_fromxml(TiXmlElement* xmlobj)";
	int nItem = 0;
	char str[MP_MAX_STR_LEN];

	// First, MONOPHONIC FEATURES
	// Iterate children and:
	//      if item is par[type=numchans] store that
	TiXmlNode* kid = 0;
	TiXmlElement* kidel;
	const char* datatext;
	while((kid = xmlobj->IterateChildren("par", kid))){
		kidel = kid->ToElement();
		if(kidel != NULL){
			if(strcmp(kidel->Attribute("type"), "numChans")==0){
				datatext = kidel->GetText();
				if(datatext != NULL){
					numChans = strtol(datatext, NULL, 0);
					if ( numChans == 0 ) {
						mp_error_msg( func, "Cannot scan numChans.\n");
						return( 1 );
					}
				}
			}
		}
	}
	// After monophonic feature grabbing, error if any needed features not filled in
	if(numChans == 0){
		mp_error_msg( func, "Did not detect numChans declaration while parsing the XML for this atom.\n");
		return( 1 );
	}
	// Allocate the storage space
	if ( alloc_atom_param( numChans ) ) {
		mp_error_msg( func, "Failed to allocate some vectors in the new atom.\n" );
		return( 1 );
	}


	// Then, MULTICHANNEL FEATURES
	// Iterate children and:
	//      if item is par[type=amp][chan=x] then store that
	kid = 0;
	int count_support=0, count_amp=0;
	while((kid = xmlobj->IterateChildren(kid))){
		kidel = kid->ToElement();
		if(kidel != NULL){

			//      if item is support[chan=x] then store that
			if(strcmp(kidel->Value(), "support")==0){
				++count_support;
				// Get the channel, and check bounds (could cause bad mem writes otherwise)
				datatext = kidel->Attribute("chan");
				long chan = strtol(datatext, NULL, 0);
				if((chan<0) || (chan >= numChans)){
					mp_error_msg( func, "Found a <support> tag with channel number %i, which is outside the channel range for this atom [0,%i).\n", chan, numChans);
					return( 1 );
				}
				// Get the "p" (pos)
				datatext = kidel->FirstChild("p")->ToElement()->GetText();
				support[chan].pos = strtol(datatext, NULL, 0);
				// Get the "l" (len)
				datatext = kidel->FirstChild("l")->ToElement()->GetText();
				support[chan].len = strtol(datatext, NULL, 0);
			}

			//      if item is par[type=amp][chan=x] then store that
			else if((strcmp(kidel->Value(), "par")==0) && (strcmp(kidel->Attribute("type"), "amp")==0)){
				++count_amp;
				// Get the channel, and check bounds (could cause bad mem writes otherwise)
				datatext = kidel->Attribute("chan");
				long chan = strtol(datatext, NULL, 0);
				if((chan<0) || (chan >= numChans)){
					mp_error_msg( func, "Found a <support> tag with channel number %i, which is outside the channel range for this atom [0,%i).\n", chan, numChans);
					return( 1 );
				}
				datatext = kidel->GetText();
				amp[chan] = strtod(datatext, NULL);
			}
		}
	}

	if((count_amp != numChans) || (count_support != numChans)){
		mp_error_msg( func, "Scanned an atom with %i channels, but failed to get that number of 'amp' values (%i) and 'support' declarations (%i).\n",
				numChans, count_amp, count_support);
		return( 1 );
	}

	/* Compute the totalChanLen and the numSamples */
	unsigned long int val;
	totalChanLen = 0;
	for (long i=0; i<numChans; ++i ) {
		val = support[i].pos + support[i].len;
		if (numSamples < val ) numSamples = val;
		totalChanLen += support[i].len;
	}

	return( 0 );
}

/******************************/
/* Initialiser from bin data  */
int MP_Atom_c::init_frombinary( FILE *fid ) {

	const char *func = "MP_Atom_c::init_frombinary( FILE *fid )";
	int nItem = 0;
	char str[MP_MAX_STR_LEN];
	double fidAmp;
	MP_Chan_t i, iRead;
	unsigned long int val;

	/* Read numChans */
	if ( mp_fread( &numChans, sizeof(MP_Chan_t), 1, fid ) != 1 ) {
		mp_error_msg( func, "Cannot read numChans.\n");
		return( 1 );
	}

	/* Allocate the storage space... */
	if ( alloc_atom_param( numChans ) ) {
		mp_error_msg( func, "Failed to allocate some vectors in the new atom.\n" );
		return( 1 );
	}

	/* ... and upon success, read the support and amp information */
	/* Support */
	for ( i=0, nItem = 0; i<numChans; i++ ) {
		nItem += (int)mp_fread( &(support[i].pos), sizeof(uint32_t), 1, fid );
		nItem += (int)mp_fread( &(support[i].len), sizeof(uint32_t), 1, fid );
	}
	/* Amp */
	if ( mp_fread( amp,   sizeof(MP_Real_t), numChans, fid ) != (size_t)numChans ) {
		mp_error_msg( func, "Failed to read the amp array.\n" );
		return( 1 );
	}

	/* Check the support information */
	if ( nItem != ( 2 * (int)( numChans ) ) ) {
		mp_error_msg( func, "Problem while reading the supports :"
				" %lu read, %lu expected.\n",
				nItem, 2 * (int )( numChans ) );
		return( 1 );
	}

	/* Compute the totalChanLen and the numSamples */
	totalChanLen = 0;
	for ( i=0; i<numChans; i++ ) {
		val = support[i].pos + support[i].len;
		if (numSamples < val ) numSamples = val;
		totalChanLen += support[i].len;
	}

	return( 0 );
}

/**************/
/* Destructor */
MP_Atom_c::~MP_Atom_c() 
{
	if (support)
		free(support);
	if (corr)
		free(corr);
	if (ampCache)
		free(ampCache);
	if (amp)
		free(amp);
}


/***************************/
/* OUTPUT METHOD           */
/***************************/

int MP_Atom_c::write( FILE *fid, const char mode ) {
	const char * func = "MP_Atom_c::write";
	MP_Chan_t i;
	int nItem = 0;

	switch ( mode ) {

	case MP_TEXT:
		/* numChans */
		nItem += fprintf( fid, "\t\t<par type=\"numChans\">%d</par>\n", numChans );
		/* Support */
		for ( i=0; i<numChans; i++ )
			nItem += fprintf( fid, "\t\t<support chan=\"%u\"><p>%u</p><l>%u</l></support>\n",
					i, support[i].pos,support[i].len );
		/* Amp */
		for (i = 0; i<numChans; i++) {
			nItem += fprintf(fid, "\t\t<par type=\"amp\" chan=\"%hu\">%lg</par>\n", i, (double)amp[i]);
		}
		break;

	case MP_BINARY:
		/* numChans */
		nItem += (int)mp_fwrite( &numChans, sizeof(MP_Chan_t), 1, fid );
		/* Support */
		for ( i=0; i<numChans; i++ ) {
			nItem += (int)mp_fwrite( &(support[i].pos), sizeof(uint32_t), 1, fid );
			nItem += (int)mp_fwrite( &(support[i].len), sizeof(uint32_t), 1, fid );
		}
		/* Amp */
		nItem += (int)mp_fwrite( amp,   sizeof(MP_Real_t), numChans, fid );
		break;

	default:
		mp_warning_msg(func, "Unknown write mode. No output written.\n");
		nItem = 0;
		break;
	}

	return (nItem);
}


/***************************/
/* OTHER METHODS           */
/***************************/

/* Get the atom position to use with sorted books */

unsigned long int MP_Atom_c::get_pos( void )const{
	return support[0].pos;
}

/** Get the identifying parameters of the atom inside a block to use with sorted books
 */

MP_Atom_Param_c* MP_Atom_c::get_atom_param( void )const{
	const char* funcName = "MP_Atom_c::get_atom_param( void )";
	return new MP_Atom_Param_c();
}

/***************/
/* Name output */
const char * MP_Atom_c::type_name( void ) {
	return ("base_atom_class");
}


/*************************************************************/
/* Substract / add an atom via Add_Worker from / to signals. */
void MP_Atom_c::substract_add( MP_Signal_c *sigSub, MP_Signal_c *sigAdd ) {

	const char *func = "MP_Atom_c::substract_add(...)";

	// Will initialize allocated_totalChanLen with the first value with which this function is called
	static unsigned long int allocated_totalChanLen = 0;
	static MP_Real_t *totalBuffer=NULL;

	// Check that the addition / substraction can take place :
	// the signal and atom should have the same number of channels
	if ( ( sigSub ) && ( sigSub->numChans != numChans ) ) {
		mp_error_msg( func, "Incompatible number of channels between the atom and the subtraction"
				" signal. Returning without any addition or subtraction.\n" );
		return;
	}
	if ( ( sigAdd ) && ( sigAdd->numChans != numChans ) ) {
		mp_error_msg( func, "Incompatible number of channels between the atom and the addition"
				" signal. Returning without any addition or subtraction.\n" );
		return;
	}


	// (Re)allocating
	totalBuffer = (MP_Real_t*) malloc (totalChanLen*sizeof(MP_Real_t)) ;
	if(NULL==totalBuffer) {
		mp_error_msg(func,"Could not allocate buffer. Returning without any addition or subtraction.\n" );
		return;
	}
	allocated_totalChanLen = totalChanLen;
	// build the atom waveform
	build_waveform(totalBuffer);

	MP_Real_t *sigIn = NULL;
	MP_Chan_t chanIdx;
	unsigned int t;
	std::pair < double, double >addSigEnergy;
	double sigEBefAdd = 0.0;
	double sigEAftAdd = 0.0;
	double sigEBefSub = 0.0;
	double sigEAftSub = 0.0;
	double sigVal;
	MP_Real_t *atomIn;
	MP_Real_t *ps;
	MP_Real_t *pa;
	unsigned long int len;
	unsigned long int pos;
	unsigned long int tmpLen;

	/* loop on channels, seeking the right location in the totalBuffer */
	for ( chanIdx = 0 , atomIn = totalBuffer; chanIdx < numChans; chanIdx++ ) {

		/* Dereference the atom support in the current channel once and for all */
		len = support[chanIdx].len;
		pos = support[chanIdx].pos;
		/* ADD the atomic waveform to the second signal */
		get_add_worker()->run_add (this, sigAdd, pos, len, chanIdx, atomIn) ;
		/* SUBTRACT the atomic waveform from the first signal */
		if ( (sigSub) && (pos < sigSub->numSamples) ) {

			/* Avoid to write outside of the signal array */
			//assert( (pos + len) <= sigSub->numSamples );
			tmpLen = sigSub->numSamples - pos;
			tmpLen = ( len < tmpLen ? len : tmpLen ); /* min( len, tmpLen ) */

			/* Seek the right location in the signal */
			sigIn  = sigSub->channel[chanIdx] + pos;

			/* Waveform SUBTRACTION */
			for ( t = 0,   ps = sigIn, pa = atomIn;
					t < tmpLen;
					t++,     ps++,       pa++ ) {
				/* Dereference the signal value */
				sigVal   = (double)(*ps);
				/* Accumulate the energy before the subtraction */
				sigEBefSub += (sigVal*sigVal);
				/* Subtract the atom sample from the signal sample */
				sigVal   = sigVal - *pa;
				/* Accumulate the energy after the subtraction */
				sigEAftSub += (sigVal*sigVal);
				/* Record the new signal sample value */
				*ps = (MP_Real_t)(sigVal);
			}

		} /* end SUBTRACT */
		addSigEnergy = get_add_worker()->wait();
		sigEBefAdd = addSigEnergy.first;
		sigEAftAdd = addSigEnergy.second;

		/* Go to the next channel */
		atomIn += len;

	} /* end for chanIdx */

	/* Update the energies of the signals */
	if ( sigSub ) sigSub->energy = sigSub->energy - sigEBefSub + sigEAftSub;
	if ( sigAdd ) sigAdd->energy = sigAdd->energy - sigEBefAdd + sigEAftAdd;

}

/*************************************************/
/* add an atom via Add_Worker from / to signals. */
/*************************************************/
std::pair< double, double> MP_Atom_c::add(MP_Signal_c * sigAdd, 
		unsigned long int pos,
		unsigned long int len,
		MP_Chan_t chanIdx,
		MP_Real_t * atomIn) {
	unsigned long int tmpLen ;
	MP_Real_t * sigIn ;
	unsigned int t;
	MP_Real_t *ps;
	MP_Real_t *pa;
	double sigVal ;
	double sigEBefAdd = 0.0;
	double sigEAftAdd = 0.0;

	// ADD the atomic waveform to the second signal
	if ( (sigAdd) && (pos < sigAdd->numSamples) )
	{
		// Avoid to write outside of the signal array
		tmpLen = sigAdd->numSamples - pos;
		tmpLen = ( len < tmpLen ? len : tmpLen ); // min( len, tmpLen )
		// Seek the right location in the signal
		sigIn  = sigAdd->channel[chanIdx] + pos;
		// Waveform ADDITION
		for ( t = 0,   ps = sigIn, pa = atomIn;t < tmpLen;t++,ps++,pa++ )
		{
			// Dereference the signal value
			sigVal   = (double)(*ps);
			// Accumulate the energy before the addition
			sigEBefAdd += (sigVal*sigVal);
			// Add the atom sample to the signal sample
			sigVal   = sigVal + *pa;
			// Accumulate the energy after the addition
			sigEAftAdd += (sigVal*sigVal);
			// Record the new signal sample value
			*ps = (MP_Real_t)(sigVal);
		}
	} // end ADD
	return std::pair< double, double>(sigEBefAdd,sigEAftAdd);
}

/*****************************************************/
/* Create a singleton of MP_Atom_c::Add_Worker class */
/*****************************************************/
MP_Atom_c::Add_Worker * MP_Atom_c::get_add_worker() {
	if (!myAddWorker) {
		MP_Atom_c::myAddWorker = new MP_Atom_c::Add_Worker() ;
	} 
	return myAddWorker;
}

/*Initialise pointer to the MP_Atom_c::Add_Worker instance */
MP_Atom_c::Add_Worker * MP_Atom_c::myAddWorker = NULL ;

#ifdef MULTITHREAD

/* run method for Add_Worker if MULTITHREAD mode On */	
/* This is a static function to run the threads and pass the object context via the self pointer */
void* MP_Atom_c::Add_Worker::run(void* a)
{
	MP_Atom_c::Add_Worker * myAddWorker = static_cast<MP_Atom_c::Add_Worker *>(a);
	/* Call the the thread function in Object context */
	myAddWorker->run_add_parallel () ;
	pthread_exit(0) ;
	return 0;
}

/********************/
/* Void constructor */

/* Add_Worker constructor if MULTITHREAD mode On */
MP_Atom_c::Add_Worker::Add_Worker() : result (0.0, 0.0) {	
	const char *func = "MP_Atom_c::AddWorker::AddWorker()";
	pthread_t thread_id ;

	/* create synchronization primitives and worker thread */
	/*task class */
	task = new Parallel_Add();

	/* tab of barrier */
	bar = new MP_Barrier_c* [2];

	bar[0]=new MP_Barrier_c(2);
	bar[1]=new MP_Barrier_c(2);

	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	task->exit = false;
	/*Create the threads*/
	if (pthread_create(&thread_id, NULL, &run, this) )
	{
		mp_error_msg( func, "Failed to create Threads\n" );
	}
}

/* run_add method for Add_Worker if MULTITHREAD mode On */		
void MP_Atom_c::Add_Worker::run_add(MP_Atom_c * self,
		MP_Signal_c *sigAdd,
		unsigned long int pos,
		unsigned long int len,
		MP_Chan_t chanIdx,
		MP_Real_t * atomIn) {
	/*tell the worker thread to start working*/
	task->myAtom = self ;
	task->sigAdd = sigAdd ;
	task->pos = pos ;
	task->len = len ;
	task->chanIdx = chanIdx ;
	task->atomIn = atomIn ;
	bar[0]->wait() ;
}

/* wait method for Add_Worker if MULTITHREAD mode Off */	    
std::pair<double,double> MP_Atom_c::Add_Worker::wait() {
	/* wait fot the worker thread to finish working */
	bar[1]->wait() ;

	return result ;
}

/* run_add_parallel method for Add_Worker if MULTITHREAD mode Off */	    
void MP_Atom_c::Add_Worker::run_add_parallel () {
	/* the worker thread's function*/
	/* wait for work */
	while (true) {

		bar[0]->wait() ;
		/* test if the task is over*/
		if (task->exit) pthread_exit(0) ;

		result = task->myAtom->add (task->sigAdd, task->pos, task->len, task->chanIdx, task->atomIn) ;
		bar[1]->wait() ;
	}
}

#else

/* run method for Add_Worker if MULTITHREAD mode Off */	
void* MP_Atom_c::Add_Worker::run(void* a)
{
	return 0;
}

/* Add_Worker constructor if MULTITHREAD mode Off */
MP_Atom_c::Add_Worker::Add_Worker() : result (0.0, 0.0) {
}

/* run_add method for Add_Worker if MULTITHREAD mode Off */	
void MP_Atom_c::Add_Worker::run_add(MP_Atom_c * self,
		MP_Signal_c *sigAdd,
		unsigned long int pos,
		unsigned long int len,
		MP_Chan_t chanIdx,
		MP_Real_t * atomIn) {
	result = self->add(sigAdd, pos, len, chanIdx, atomIn) ;
}

/* wait for Add_Worker if MULTITHREAD mode Off */	
std::pair<double,double> MP_Atom_c::Add_Worker::wait() {
	return result ;
}

#endif

/**********************************************/
/* Substract / add a monochannel atom from / to multichannel signals. */
void MP_Atom_c::substract_add_var_amp( MP_Real_t *amp, MP_Chan_t numAmps,
		MP_Signal_c *sigSub, MP_Signal_c *sigAdd ) {

	const char *func = "MP_Atom_c::substract_add_var_amp(...)";
	MP_Real_t *sigIn;
	MP_Chan_t chanIdx;
	unsigned int t;
	double sigEBefAdd = 0.0;
	double sigEAftAdd = 0.0;
	double sigEBefSub = 0.0;
	double sigEAftSub = 0.0;
	double sigVal;

	static unsigned long int allocated_totalChanLen = 0;
	static unsigned long int allocated_numAmps = 0;
	static MP_Real_t *totalBuffer = 0;
	if (!totalBuffer|| allocated_totalChanLen != totalChanLen || allocated_numAmps != numAmps ) {
		if (totalBuffer) free(totalBuffer) ;
		allocated_totalChanLen = totalChanLen ;
		allocated_numAmps = numAmps;
		totalBuffer = (MP_Real_t*) malloc (allocated_totalChanLen*allocated_numAmps*sizeof(MP_Real_t)) ;
	}

	// MP_Real_t totalBuffer[numAmps*totalChanLen];
	MP_Real_t *atomIn;
	MP_Real_t *ps;
	MP_Real_t *pa;
	unsigned long int len;
	unsigned long int pos;
	unsigned long int tmpLen;

	/* Check that the addition / substraction can take place :
     the amp vector should have as many elements as channels in the signals */
	if ( ( sigSub ) && ( sigSub->numChans != numAmps ) ) {
		mp_error_msg( func, "The number of amplitudes is incompatible with the number of channels"
				" in the subtraction signal. Returning without any addition or subtraction.\n" );
		return;
	}
	if ( ( sigAdd ) && ( sigAdd->numChans != numAmps ) ) {
		mp_error_msg( func, "The number of amplitudes is incompatible with the number of channels"
				" in the addition signal. Returning without any addition or subtraction.\n" );
		return;
	}
	/* The original atom should be mono-channel */
	if ( numChans != 1 ) {
		mp_error_msg( func, "This method applies to mono-channel atoms only."
				" Returning without any addition or subtraction.\n" );
		return;
	}

	/* Dereference the atom support once and for all */
	len = support[0].len;
	pos = support[0].pos;

	/* build the atom waveform "template", in the first segment of the buffer */
	build_waveform( totalBuffer );
	/* Replicate the template and multiply it with the various amplitudes,
	 * in the segments that come after the first one */
	for (chanIdx = 0; chanIdx < numAmps; chanIdx++) {
		sigVal = amp[chanIdx];
		//mp_progress_msg("substract_add_var_amp", "corr = %lf\n", sigVal);
		for (t = 1, pa = totalBuffer, ps = (totalBuffer + len * chanIdx);
				t < len; t++, pa++, ps++) {
			(*ps) = sigVal * (*pa);
		}
	}
	/* - Then, correct the amplitude of the initial template */
	sigVal = amp[0];
	for (t = 0, pa = totalBuffer; t < len; t++, pa++) {
		(*pa) = sigVal * (*pa);
	}

	/* loop on channels, seeking the right location in the totalBuffer */
	chanIdx = 0;
	atomIn = totalBuffer;
	while (chanIdx < numAmps) {

		/* SUBTRACT the atomic waveform from the first signal */
		if ((sigSub) && (pos < sigSub->numSamples)) {

			/* Avoid to write outside of the signal array */
			// assert( (pos + len) <= sigSub->numSamples );
			tmpLen = sigSub->numSamples - pos;
			tmpLen = (len < tmpLen ? len : tmpLen);	/* min( len,
			 * tmpLen ) */

			/* Seek the right location in the signal */
			sigIn = sigSub->channel[chanIdx] + pos;

			/* Waveform SUBTRACTION */
			for (t = 0, ps = sigIn, pa = atomIn; t < tmpLen;
					t++, ps++, pa++) {
				/* Dereference the signal value */
				sigVal = (double) (*ps);
				/* Accumulate the energy before the subtraction */
				sigEBefSub += (sigVal * sigVal);
				/* Subtract the atom sample from the signal sample */
				sigVal = sigVal - *pa;
				/* Accumulate the energy after the subtraction */
				sigEAftSub += (sigVal * sigVal);
				/* Record the new signal sample value */
				*ps = (MP_Real_t) (sigVal);
			}
		}			/* end SUBTRACT */

		/* ADD the atomic waveform to the second signal */
		if ((sigAdd) && (pos < sigAdd->numSamples)) {

			/* Avoid to write outside of the signal array */
			// assert( (pos + len) <= sigAdd->numSamples );
			tmpLen = sigAdd->numSamples - pos;
			tmpLen = (len < tmpLen ? len : tmpLen);	/* min( len,
			 * tmpLen ) */

			/* Seek the right location in the signal */
			sigIn = sigAdd->channel[chanIdx] + pos;

			/* Waveform ADDITION */
			for (t = 0, ps = sigIn, pa = atomIn; t < len; t++, ps++, pa++) {
				/* Dereference the signal value */
				sigVal = (double) (*ps);
				/* Accumulate the energy before the subtraction */
				sigEBefAdd += (sigVal * sigVal);
				/* Add the atom sample to the signal sample */
				sigVal = sigVal + *pa;
				/* Accumulate the energy after the subtraction */
				sigEAftAdd += (sigVal * sigVal);
				/* Record the new signal sample value */
				*ps = (MP_Real_t) (sigVal);
			}
		}/* end ADD */
		/* Go to the next channel */
		atomIn += len;
		chanIdx++;
	}				/* end for chanIdx */

	/*
	 * Update the energies of the signals
	 */
	if (sigSub)
		sigSub->energy = sigSub->energy - sigEBefSub + sigEAftSub;
	if (sigAdd)
		sigAdd->energy = sigAdd->energy - sigEBefAdd + sigEAftAdd;

}

/***********************************************************************/
/*
 * Sorting function which characterizes various properties of the atom,
 * across all channels 
 */
int
MP_Atom_c::satisfies(int field, int test, MP_Real_t val)
{

	MP_Chan_t chanIdx;
	int retVal = MP_TRUE;

	for (chanIdx = 0; chanIdx < numChans; chanIdx++) {
		retVal = (retVal && satisfies(field, test, val, chanIdx));
	}

	return (retVal);
}


/***********************************************************************/
/*
 * Sorting function which characterizes various properties of the atom,
 * along one channel 
 */
int MP_Atom_c::satisfies(int field, int test, MP_Real_t val,
		MP_Chan_t chanIdx)
{

	const char *func = "MP_Atom_c::satisfies(...)";
	MP_Real_t x;
	int has = has_field(field);

	if (test == MP_HAS) {
		return (has);
	} else {
		if (has == MP_FALSE) {
			mp_warning_msg(func, "Unknown field. Returning TRUE.\n");
			return (MP_TRUE);
		} else {
			x = (MP_Real_t) get_field(field, chanIdx);
			switch (test) {
			case MP_SUPER:
				return (x > val);
			case MP_SUPEQ:
				return (x >= val);
			case MP_EQ:
				return (x == val);
			case MP_INFEQ:
				return (x <= val);
			case MP_INFER:
				return (x < val);
			default:
				mp_warning_msg(func, "Unknown test. Returning TRUE.\n");
				return (MP_TRUE);
			}
		}
	}
}

int MP_Atom_c::has_field(int field)
{
	switch (field) {
	case MP_LEN_PROP:
		return (MP_TRUE);
	case MP_POS_PROP:
		return (MP_TRUE);
	case MP_AMP_PROP:
		return (MP_TRUE);
	default:
		return (MP_FALSE);
	}
}

MP_Real_t MP_Atom_c::get_field(int field, MP_Chan_t chanIdx)
{
	MP_Real_t x;
	switch (field) {
	case MP_LEN_PROP:
		x = (MP_Real_t)(support[chanIdx].len);
		break;
	case MP_POS_PROP:
		x = (MP_Real_t)(support[chanIdx].pos);
		break;
	case MP_AMP_PROP:
		x = amp[chanIdx];
		break;
	default:
		x = (MP_Real_t)0.0;
	}
	return(x);
}


MP_Real_t MP_Atom_c::dist_to_tfpoint( MP_Real_t /* time */, MP_Real_t /* freq */, MP_Chan_t /* chanIdx */)
{
	return(1e6);
}

// TODO: make normalized waveforms the default for all atoms
void MP_Atom_c::build_waveform_norm(MP_Real_t *outBuffer )
{
	MP_Chan_t c;
	//Save amp and temporarily set it to 1
	for (c = 0; c<numChans; c++){
		ampCache[c] = amp[c];
		amp[c] = 1;
	}
	build_waveform(outBuffer);

	//Restore amp
	for (c = 0; c<numChans; c++){
		amp[c] = ampCache[c];
	}
}

void MP_Atom_c::build_waveform_corr(MP_Real_t *outBuffer )
{
	MP_Chan_t c;
	//Save amp and temporarily set it to 1
	for (c = 0; c<numChans; c++){
		ampCache[c] = amp[c];
		amp[c] = corr[c];
	}
	build_waveform(outBuffer);

	//Restore amp
	for (c = 0; c<numChans; c++){
		amp[c] = ampCache[c];
	}
}


