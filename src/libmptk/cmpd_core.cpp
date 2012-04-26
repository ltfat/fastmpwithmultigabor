/******************************************************************************/
/*                                                                            */
/*                              cmpd_core.cpp                                 */
/*                                                                            */
/*                        Matching Pursuit Utilities                          */
/*                                                                            */
/* Bob L. Sturm																  */
/* RÈmi Gribonval                                                             */
/* Sacha Krstulovic                                           Thu Jun 08 2011 */
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

using namespace std;

/***************************/
/* CONSTRUCTORS/DESTRUCTOR */
/***************************/

/**********************/
/* Factory function:*/
/********************/

/* - signal+book only: */
MP_CMpd_Core_c* MP_CMpd_Core_c::create( MP_Signal_c *setSignal, MP_Book_c *setBook ) {

  const char* func = "MP_CMpd_Core_c::init(2 args)";
  
  MP_CMpd_Core_c* newCore;

  /* Check for NULL entries */
  if ( setSignal == NULL ) {
    mp_error_msg( func, "Can't initialize a MP_CMpd_Core_c from a NULL signal.\n" );
    return( NULL );
  }
  
  if ( setBook == NULL ) {
    mp_error_msg( func, "Can't initialize a MP_Mpd_Core_c from a NULL dictionary vector.\n" );
    return( NULL );
  } 

  /* Instantiate and check */
  newCore = new MP_CMpd_Core_c();
  
  if ( newCore == NULL ) {
    mp_error_msg( func, "Failed to create a new mpd core.\n" );
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
MP_CMpd_Core_c* MP_CMpd_Core_c::create( MP_Signal_c *setSignal, MP_Book_c *setBook, MP_Dict_c *setDict ) {
  
  const char* func = "MP_CMpd_Core_c::init(3 args)";
  MP_CMpd_Core_c* newCore;
  newCore = MP_CMpd_Core_c::create( setSignal, setBook );
  if ( newCore == NULL ) {
    mp_error_msg( func, "Failed to create a new mpd core.\n" );
    return( NULL );}
    
  if ( setDict ) newCore->change_dict(setDict);
  else {
    mp_error_msg( func, "Could not use a NULL dictionary.\n" );
    return( NULL );}
 
  return( newCore );
}

MP_CMpd_Core_c* MP_CMpd_Core_c::create( MP_Signal_c *setSignal, MP_Book_c *setBook, MP_Signal_c* setApproximant )
{
  const char* func = "MP_CMpd_Core_c::init(3 args)";
  MP_CMpd_Core_c* newCore;
  newCore = MP_CMpd_Core_c::create( setSignal, setBook );
  if ( newCore == NULL ) {
    mp_error_msg( func, "Failed to create a new mpd core.\n" );
    return( NULL );}
    
  if ( setApproximant ) newCore->plug_approximant(setApproximant);
  else {
    mp_error_msg( func, "Could not use a NULL approximant.\n" );
    return( NULL );}
    
  return( newCore );
}



/********************/
/* NULL constructor */
MP_CMpd_Core_c::MP_CMpd_Core_c() {
}

/**************/
/* Destructor */
MP_CMpd_Core_c::~MP_CMpd_Core_c() {
}


/***************************/
/* SET OBJECTS             */
/***************************/


/*************************/
/* Make one CMP iteration (with selection and atom refinements) */
unsigned short int MP_CMpd_Core_c::step() {

	const char			*func = "MP_CMpd_Core_c::step()";
	unsigned long int 	i,j;
	MP_Real_t			residualEnergyBeforeCycle;
    
	/* Reset the state info */
	state = 0;

	mp_debug_msg( MP_DEBUG_MPD_LOOP, func, "ENTERING iteration [%lu]/[%lu].\n", numIter+1, stopAfterIter );
	mp_debug_msg( MP_DEBUG_MPD_LOOP, func, "Next report hit is [%lu].\n", nextReportHit );
	mp_debug_msg( MP_DEBUG_MPD_LOOP, func, "Next save hit is   [%lu].\n", nextSaveHit );
	mp_debug_msg( MP_DEBUG_MPD_LOOP, func, "Next snr hit is    [%lu].\n", nextSnrHit );
	mp_debug_msg( MP_DEBUG_MPD_LOOP, func, "SNR is [%g]/[%g].\n", 10*log10(currentSnr), 10*log10(stopAfterSnr) );

	/* 1) Iterate: */
	dict->iterate_mp( book , approximant ); /* Note: if approximant is NULL, no computation will be performed on it. */
    itersincelastcycle++;
    
	residualEnergy = dict->signal->energy;
    currentSnr = initialEnergy / residualEnergy;
    
	//struct timeval startTime, endTime, TotalstartTime, TotalendTime;
    //double tS, tE;
    
    //cout << currentSnr << "/" << max_dB_stopcycle << "\n";
    //cout << book->numAtoms << "/" << max_iter_stopcycle << "\n";
    //cout << "Refine if " << (residualEnergyAfterCycle / residualEnergy) << " is greater than " << min_dB_beforecycle << "\n";
    //cout << max_iter_beforecycle << "\t" << itersincelastcycle << "\n";
    if ( book->numAtoms > 1 && num_cycles > 0 && book->numAtoms <= max_iter_stopcycle && currentSnr < max_dB_stopcycle &&
         itersincelastcycle >= max_iter_beforecycle && (residualEnergyAfterCycle / residualEnergy) >= min_dB_beforecycle ) {
		
        for (i=0; i < num_cycles; i++)
		{
			residualEnergyBeforeCycle = dict->signal->energy;
            //gettimeofday(&TotalstartTime, NULL);
			for (j=0; j < book->numAtoms; j++)
			{
                
                //gettimeofday(&startTime, NULL);
				dict->update();
                /*
                gettimeofday(&endTime, NULL);
                tS = startTime.tv_sec*1000 + (startTime.tv_usec)/1000.0;
                tE = endTime.tv_sec*1000  + (endTime.tv_usec)/1000.0;
                std::cout << "DICT UPDATE Total Time Taken: " << tE - tS << std::endl;
                 */
                 
				// add atom j to residual
                //gettimeofday(&startTime, NULL);
				book->atom[j]->substract_add( approximant , dict->signal );
                /*
                gettimeofday(&endTime, NULL);
                tS = startTime.tv_sec*1000 + (startTime.tv_usec)/1000.0;
                tE = endTime.tv_sec*1000  + (endTime.tv_usec)/1000.0;
                std::cout << "SUBSTRACT ADD Total Time Taken: " << tE - tS << std::endl;
                */

				// adjust touch in dict
				for ( int chanIdx=0; chanIdx < book->atom[j]->numChans; chanIdx++ )
				{
					dict->touch[chanIdx].pos = book->atom[j]->support[chanIdx].pos;
					dict->touch[chanIdx].len = book->atom[j]->support[chanIdx].len;
				}
				
				// now replace atom in book
                //gettimeofday(&startTime, NULL);
                if (!holdatoms)
                    dict->iterate_cmp( book, approximant, j );
                else
                    dict->iterate_cmphold( book, approximant, j);
                /*
                gettimeofday(&endTime, NULL);
                tS = startTime.tv_sec*1000 + (startTime.tv_usec)/1000.0;
                tE = endTime.tv_sec*1000  + (endTime.tv_usec)/1000.0;
                std::cout << "ITERATE CMP Total Time Taken: " << tE - tS << std::endl;
                */
                	
			}
            /*
            gettimeofday(&TotalendTime, NULL);
            std::cout << "Total Cycle Time: " << TotalendTime.tv_sec*1000 + (TotalendTime.tv_usec)/1000.0 - 
                TotalstartTime.tv_sec*1000 - (TotalstartTime.tv_usec)/1000.0 << std::endl;
            */
			
			residualEnergyAfterCycle = (double) dict->signal->energy;
			
            //cout << min_cycleimprovedB << " <? " << lastResidualEnergyAfterCycle/residualEnergyWithinCycleBefore << "\n";
			if (residualEnergyBeforeCycle/residualEnergyAfterCycle < min_cycleimprovedB) break;

		}
        itersincelastcycle = 0;
        cout << book->numAtoms << ": Energy reduction of " << 100*(1 - residualEnergyAfterCycle/residualEnergy) << "%" << 
            " from " << i << " of " << num_cycles << " possible cycles" << endl;
        
	}
    residualEnergyBefore = residualEnergy;
    residualEnergy = (double)dict->signal->energy;

	if ( useDecay ) {
		decay.append( residualEnergy );
		//decay.append( residualEnergyBefore );
	}
	numIter++;
    
	/* 2) Check for possible breakpoints: */

	if ( numIter == nextReportHit )
	{
	  mp_progress_msg( "At iteration", "[%lu] : the residual energy is [%g] and the SNR is [%g].\n",numIter, residualEnergy, 10*log10( initialEnergy / residualEnergy ) );
	  nextReportHit += reportHit;
	}

	if ( numIter == nextSaveHit )
	{
		save_result();
		nextSaveHit += saveHit;
	}

	if ( numIter == ULONG_MAX )
		state = ( state | MP_ITER_EXHAUSTED );
  
  /* 3) Check for possible stopping conditions: */
  if ( useStopAfterIter && (numIter >= stopAfterIter) )   state = ( state | MP_ITER_CONDITION_REACHED );
  if ( useStopAfterSnr  && (currentSnr >= stopAfterSnr) ) state = ( state | MP_SNR_CONDITION_REACHED );
  if ( residualEnergy < 0.0 ) state = ( state | MP_NEG_ENERGY_REACHED );
    
  if ( residualEnergy - residualEnergyBefore > 1e-15 ) {
    mp_warning_msg( func, "Iteration [%lu] increases the energy of the residual ! Before: [%g] Now: [%g] Diff [%g]\n",
		    numIter, residualEnergyBefore, residualEnergy, residualEnergyBefore - residualEnergy );
    mp_warning_msg( func, "Last atom found is sent to stderr.\n" );
  }

  mp_debug_msg( MP_DEBUG_MPD_LOOP, func, "EXITING iteration [%lu]/[%lu].\n", numIter, stopAfterIter );
  mp_debug_msg( MP_DEBUG_MPD_LOOP, func, "Next report hit is [%lu].\n", nextReportHit );
  mp_debug_msg( MP_DEBUG_MPD_LOOP, func, "Next save hit is   [%lu].\n", nextSaveHit );
  mp_debug_msg( MP_DEBUG_MPD_LOOP, func, "Next snr hit is    [%lu].\n", nextSnrHit );
  mp_debug_msg( MP_DEBUG_MPD_LOOP, func, "SNR is [%g]/[%g].\n",
		10*log10(currentSnr), 10*log10(stopAfterSnr) );

  return( state );
}


