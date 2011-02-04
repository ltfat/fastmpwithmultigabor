/******************************************************************************/
/*                                                                            */
/*                            anywave_table.cpp                               */
/*                                                                            */
/*                        Matching Pursuit Library                            */
/*                                                                            */
/* RÈmi Gribonval                                                             */
/* Sacha Krstulovic                                                           */
/* Sylvain Lesage                                             Tue Nov 03 2005 */
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
 * $Date: 2007-06-28 16:48:30 +0200 (Thu, 28 Jun 2007) $
 * $Revision: 1083 $
 *
 */

#include "mptk.h"
#include "mp_system.h"
#include <iostream>
/***********************************************************/
/*                                                         */
/* anywave_table.cpp: methods for class MP_Anywave_Table_c */
/*                                                         */
/***********************************************************/

/***************************/
/* CONSTRUCTORS/DESTRUCTOR */
/***************************/

/********************/
/* Void constructor */
MP_Anywave_Table_c::MP_Anywave_Table_c( void )
{
  set_null();
}

/********************/
/* FILE constructor */
/*
  MP_Anywave_Table_c::MP_Anywave_Table_c( FILE* pFile )
  {

  set_null();
*/  /* parsing the file pFile */
/*
  if ( parse_file(pFile) == false )
  {
  mp_error_msg( "MP_Anywave_Table_c::MP_Anywave_Table_c", "Can't parse the file - Default initialization by 'void constructor'.\n");
  reset();
  }
  }
*/
/************************/
/* fileName constructor */
MP_Anywave_Table_c::MP_Anywave_Table_c( char* fileName )
{
	const char * func = "MP_Anywave_Table_c::MP_Anywave_Table_c";
  set_null();
  if (set_table_file_name( fileName ) == NULL)
    {
      mp_error_msg( func, "Can't alloc tableFileName to the size of fileName : %s  - Default initialization by 'void constructor'.\n", fileName );
      reset();
    }
  else
    {
      if ( parse_xml_file( fileName ) == false  ) 
        {
          mp_error_msg( func, "Can't parse the file %s - Default initialization by 'void constructor'.\n", fileName );
          reset();
        }
    }
}

/**************/
/* destructor */
MP_Anywave_Table_c::~MP_Anywave_Table_c()
{
#ifndef NDEBUG
  fprintf( stderr, "mplib DEBUG -- ~MP_Anywave_Table_c -- Entering the anywave table destructor.\n" );
#endif

  reset();
#ifndef NDEBUG
  fprintf( stderr, "mplib DEBUG -- ~MP_Anywave_Table_c -- Exiting the anywave table destructor.\n" );
#endif
}

/***************************/
/* OTHER METHODS           */
/***************************/

/* Test */
bool MP_Anywave_Table_c::test( char* filename )
{

  unsigned long int sampleIdx;

  fprintf( stdout, "\n\n-- Entering MP_Anywave_Table_c::test \n" );

  /* Create a Anywave_Table */
  MP_Anywave_Table_c* tablePtr = new MP_Anywave_Table_c( filename );
  if (tablePtr == NULL)
    {
      return(false);
    }

  /* print the first channel of the first filter before normalization */
  fprintf( stdout, "---- Printing the 10 first samples of the first channel of the first filter before normalization:\n" );

  for ( sampleIdx = 0;
        (sampleIdx < tablePtr->filterLen) && (sampleIdx < 10);
        sampleIdx ++)
    {
      fprintf( stdout, "%lf ", *(tablePtr->wave[0][0]+sampleIdx));
    }
  fprintf( stdout, "\n" );

  tablePtr->normalize();

  /* print the first channel of the first filter before normalization */
  fprintf( stdout, "---- Printing the 10 first samples of the first channel of the first filter after normalization:\n" );

  for ( sampleIdx = 0;
        (sampleIdx < tablePtr->filterLen) && (sampleIdx < 10);
        sampleIdx ++)
    {
      fprintf( stdout, "%lf ", *(tablePtr->wave[0][0]+sampleIdx));
    }
  fprintf( stdout, "\n" );


  /* Destroy the anywave server */
  delete( tablePtr );

  fprintf( stdout, "\n-- Exiting MP_Anywave_Table_c::test \n" );
  fflush( stdout );
  return( true );

}

/* load the data contained in dataFileName, store it in
   storage and update the pointers in wave */
bool MP_Anywave_Table_c::load_data( void )
{

  FILE * pFile = NULL;
  unsigned long int numElements;
  unsigned long int numReadElements;
  unsigned long int filterIdx;
  unsigned short int chanIdx;
  MP_Real_t *pSample;

  /* if storage is not empty, do nothing */
  if ( storage != NULL )
    {
      mp_error_msg( "MP_Anywave_Table_c::load_data", "the storage is already full. We leave this and do nothing\n" );
      return(false);
    }
  else
    {
      /* verify that all the parameters are available */
      if ( (numChans == 0) || (filterLen == 0) || (numFilters == 0) || (dataFileName == NULL) )
        {
          mp_error_msg( "MP_Anywave_Table_c::load_data", "can't try to load the waveform data, missing parameters. Leave the storage empty\n" );
          return(false);
        }
      else
        {
          /* try to open the file */
          if ((pFile = fopen (dataFileName,"rb")) == NULL )
            {
              mp_error_msg( "MP_Anywave_Table_c::load_data", "Can't open the file %s - Leave the storage empty'.\n", dataFileName );
              return(false);
            }
          else
            {
              /* number of elements to be read in the data file */
              if ((numChans != 0) && (numFilters != 0) && (filterLen != 0))
                {
                  /* check that multiplying the three dimensions will not go over the maximum size of an unsigned long int */
                  if ( ((double)MP_MAX_SIZE_T / (double) numChans / (double) numFilters / (double) filterLen) / (double)sizeof(MP_Real_t) <= 1.0)
                    {
                      mp_error_msg( "MP_Anywave_Table_c::load_data", "numChans [%lu] . numFilters [%lu] . filterLen [%lu] . sizeof(MP_Real_t) [%lu] is greater than the max for a size_t [%lu]. Cannot use malloc for allocating space for storing the anywave filters. Exiting from load_data().\n", numChans, numFilters, filterLen, sizeof(MP_Real_t), MP_MAX_SIZE_T);
                      return( false );
                    }
                }
              numElements = numChans * numFilters * filterLen;

              /* try to allocate room for storage */
              if ( ( storage = (MP_Real_t *)malloc( numElements * sizeof(MP_Real_t) ) ) == NULL )
                {
                  mp_error_msg( "MP_Anywave_Table_c::load_data", "Can't allocate room for storage - Leave the storage empty'.\n" );
                  return(false);
                }
              else
                {
                  /* try to read the file */

                  if ( (numReadElements = (unsigned long int)mp_fread( storage, sizeof(MP_Real_t), numElements, pFile )) != numElements )
                    {
                      mp_error_msg( "MP_Anywave_Table_c::load_data", "number of elements read is %lu instead of %lu. Clear the storage\n", numReadElements, numElements );
                      if (storage != NULL)
                        {
                          free( storage );
                        }
                      return(false);
                    }
                  else
                    {
                      /* try to allocate room for wave */
                      if ( alloc_wave() == false )
                        {
                          mp_error_msg( "MP_Anywave_Table_c::load_data", "Can't allocate room for wave - Leave the wave array empty.\n" );
                          return(false);
                        }
                      else
                        {
                          /* fill in the pointers in wave */
                          for (filterIdx = 0, pSample = storage;
                               filterIdx < numFilters;
                               filterIdx ++)
                            {
                              for (chanIdx = 0;
                                   chanIdx < numChans;
                                   chanIdx ++, pSample += filterLen)
                                {
                                  wave[filterIdx][chanIdx] = pSample;
                                }
                            }
                        }
                    }
                }
              fclose(pFile);
            }
        }
    }

  return(true);
}

/*Allocate the pointers array wave, using the dimensions
  numFilters and numChans */
bool MP_Anywave_Table_c::alloc_wave( void )
{
  free_wave();

  unsigned long int filterIdx;

  if ((double)MP_MAX_SIZE_T / (double)numFilters / (double)sizeof(MP_Real_t **) <= 1.0)
    {
      mp_error_msg( "MP_Anywave_Table_c::alloc_wave", "numFilters [%lu] . sizeof(MP_Real_t**) [%lu] is greater than the max for a size_t [%lu]. Cannot use malloc for allocating space for arrays. Exiting from alloc_wave().\n", numFilters, sizeof(MP_Real_t**), MP_MAX_SIZE_T);
      return( false );
    }
  if ((double)MP_MAX_SIZE_T / (double)numChans / (double)sizeof(MP_Real_t *) <= 1.0)
    {
      mp_error_msg( "MP_Anywave_Table_c::alloc_wave", "numChans [%lu] . sizeof(MP_Real_t*) [%lu] is greater than the max for a size_t [%lu]. Cannot use malloc for allocating space for arrays. Exiting from alloc_wave().\n", numChans, sizeof(MP_Real_t*), MP_MAX_SIZE_T);
      return( false );
    }
  
  if ( (wave = (MP_Real_t ***) malloc( numFilters * sizeof(MP_Real_t **) ) ) == NULL )
    {
      mp_error_msg( "MP_Anywave_Table_c::alloc_wave", "Can't allocate room for wave - Leave the wave array empty" );
      return(false);
    }
  else
    {
      for ( filterIdx = 0;
            filterIdx < numFilters;
            filterIdx ++ )
        {
          if ( ( wave[filterIdx] = (MP_Real_t **) malloc( numChans * sizeof(MP_Real_t *) ) ) == NULL )
            {
              mp_error_msg( "MP_Anywave_Table_c::alloc_wave", "Can't allocate room for wave - Leave the wave array empty" );
              free_wave();
              return(false);
            }
        }
    }

  /* if succeed */
  return(true);

}

/* Free the pointer array wave */
void MP_Anywave_Table_c::free_wave( void )
{
  unsigned long int filterIdx;

  if ( wave != NULL )
    {
      for ( filterIdx = 0;
            filterIdx < numFilters;
            filterIdx ++ )
        {
          if ( wave[filterIdx] != NULL )
            {
              free( wave[filterIdx] );
              wave[filterIdx] = NULL;
            }
        }
      free(wave);
      wave = NULL;
    }
}

/* Initialize all the members */
void MP_Anywave_Table_c::set_null( void )
{

  set_table_file_name( "" );
  set_data_file_name( "" );
  numChans = 0;
  filterLen = 0;
  numFilters = 0;
  normalized = 0;
  centeredAndDenyquisted = 0;
  storage = NULL;
  wave = NULL;
}

/* Re-initialize all the members */
void MP_Anywave_Table_c::reset( void )
{

#ifndef NDEBUG
  fprintf( stderr, "mplib DEBUG -- MP_Anywave_Table_c::reset() -- Entering.\n" );
#endif

  free_wave();
  if (storage != NULL) {
    free(storage);
    storage = NULL;
  }
  set_table_file_name( "" );
  set_data_file_name( "" );
  numChans = 0;
  filterLen = 0;
  numFilters = 0;
  normalized = 0;
  centeredAndDenyquisted = 0;

#ifndef NDEBUG
  fprintf( stderr, "mplib DEBUG -- MP_Anywave_Table_c::reset() -- Exiting.\n" );
#endif

}

/** \brief Parses a anywave table import file and fills in the members of
 * a MP_Anywave_Table_Scan_Info_c object
 *
 * anywave_table_scanner() is a function of the file
 * anywave_table_scanner.lpp. It is a FLEX program used to parse a
 * anywave table import file (e.g. "PATH/anywave.xml").
 *
 * After calling anywave_table_scanner(), to fill in the members of the
 * MP_Anywave_Table_c object with the members of the
 * MP_Anywave_Table_Scan_Info_c interface, call the function
 * MP_Anywave_Table_Scan_Info_c::pop_table()
 *
 * \param fid : the stream of the file to parse (e.g. after opening
 * "PATH/anywave.xml")
 *
 * \param scanInfo : a pointer to MP_Anywave_Table_Scan_Info_c
 * interface, members of which will be filled in with the data parsed
 * from fid.
 *
 */
//extern unsigned short int anywave_table_scanner( FILE *fid, MP_Anywave_Table_Scan_Info_c *scanInfo );

bool MP_Anywave_Table_c::parse_xml_file(const char* fName)
{
	const char* func = "MP_Anywave_Table_c::parse_xml_file(const char* fName)";
	TiXmlNode * node;
	TiXmlElement * param;
	TiXmlElement * properties;
	char*  convertEnd;
	bool numChansIsSet = false;
	bool filterLenIsSet = false;
	bool numFiltersIsSet = false;
	bool normalizedIsSet = false;
	bool centeredAndDenyquistedIsSet = false;
	TiXmlDocument doc(fName);
	if (!doc.LoadFile())
    {
		mp_error_msg( func, "Error while loading the table description file [%s].\n", fName );
		mp_error_msg( func, "Error ID: %u .\n", doc.ErrorId() );
		mp_error_msg( func, "Error description: %s .\n", doc.ErrorDesc());
		return  false;
	}
	TiXmlHandle hdl(&doc);

	properties = hdl.FirstChildElement("table").FirstChildElement("libVersion").Element();

	node = hdl.FirstChild("table").ToNode();
	param = node->FirstChildElement("param");
	if (param!=0)
    {
		for ( ; param!=0 ; param = param->NextSiblingElement("param"))
        {
			if(!(strcmp(param->Attribute("name"),"filterLen")))
            {
				filterLen =strtol(param->Attribute("value"), &convertEnd, 10);
				if (*convertEnd != '\0')
				{
					mp_error_msg( func, "cannot convert parameter filterLen in unsigned long int.\n" );
					return( false );
                }
				else filterLenIsSet = true;
            }
			if(!(strcmp(param->Attribute("name"),"numChans")))
            {
				numChans =(unsigned short int) strtol(param->Attribute("value"), &convertEnd, 10);
				if(*convertEnd != '\0')
                {
					mp_error_msg( func, "cannot convert parameter numChans in unsigned long int.\n" );
					return( false );
                }
				else numChansIsSet = true;
            }
			if(!(strcmp(param->Attribute("name"),"numFilters")))
            {
				numFilters =strtol(param->Attribute("value"), &convertEnd, 10);
				if (*convertEnd != '\0')
                {
					mp_error_msg( func, "cannot convert parameter numFilters in unsigned long int.\n" );
					return( false );
                }
				else numFiltersIsSet = true;
            }
			if(!(strcmp(param->Attribute("name"),"normalized")))
            {
				normalized =strtol(param->Attribute("value"), &convertEnd, 10);
				if (*convertEnd != '\0')
                {
					mp_error_msg( func, "cannot convert parameter normalized in unsigned long int.\n" );
					return( false );
                }
				else normalizedIsSet = true;
            }
			if(!(strcmp(param->Attribute("name"),"centeredAndDenyquisted")))
            {
				centeredAndDenyquisted =strtol(param->Attribute("value"), &convertEnd, 10);
				if (*convertEnd != '\0')
                {
					mp_error_msg( func, "cannot convert parameter numFilters in unsigned long int.\n" );
					return( false );
                }
				else centeredAndDenyquistedIsSet = true;
            }
			if(!(strcmp(param->Attribute("name"),"data")))
            {
				if ( set_data_file_name( param->Attribute("value") ) == NULL )
                {
					if ( !numFiltersIsSet) mp_error_msg( func, "No parameter numFilters to define the table in %s file.\n", fName );
					if ( !numChansIsSet) mp_error_msg( func, "No parameter numChans to define the table in %s file.\n", fName );
					if ( !filterLenIsSet) mp_error_msg( func, "No parameter filterLen to define the table in %s file.\n", fName );
					mp_error_msg( func,"setting dataFileName %s to the table failed. Returning 0.\n", dataFileName );
					return( false );
                }
				if ( load_data() == false )
                {
					if ( !numFiltersIsSet) mp_error_msg( func, "No parameter numFilters to define the table in %s file.\n", fName );
					if ( !numChansIsSet) mp_error_msg( func, "No parameter numChans to define the table in %s file.\n", fName );
					if ( !filterLenIsSet) mp_error_msg( func, "No parameter filterLen to define the table in %s file.\n", fName );
					mp_error_msg( func,"loading the data from the file %s failed. Returning 0.\n", dataFileName );
					return( false );
                }
            }
        }
		return( true );
    }
	else
    {
		mp_error_msg( func, "No parameter to define the table in %s file.\n", fName );
		return( false );
    }
}

/* Normalization of the waveforms */
unsigned long int MP_Anywave_Table_c::normalize( void )
{
	unsigned long int filterIdx = 0;
	MP_Real_t *pSample;
	MP_Real_t *pSampleStart;
	MP_Real_t *pSampleEnd;
	double energyCoeff;

	for (filterIdx = 0 ; filterIdx < numFilters ; filterIdx++)
	{
		pSampleStart = wave[filterIdx][0];
		pSampleEnd = pSampleStart + filterLen*(unsigned long int)numChans;

		energyCoeff = 0.0;
		for (pSample = pSampleStart ; pSample < pSampleEnd ; pSample++)
			energyCoeff += (*pSample) * (*pSample);

		energyCoeff = (MP_Real_t) 1 / sqrt( (double) energyCoeff );
		for (pSample = pSampleStart ; pSample < pSampleEnd ; pSample++)
			(*pSample) = (*pSample) * energyCoeff;
	}
	normalized = 1;
	return(normalized);
}

/* Sets the mean and the nyquist component of the waveforms to zero */
unsigned long int MP_Anywave_Table_c::center_and_denyquist( void )
{
  unsigned long int filterIdx = 0;
  unsigned short int chanIdx = 0;
  MP_Real_t *pSample;
  MP_Real_t *pSampleStart;
  MP_Real_t *pSampleEnd;
  double mean;
  double nyquist;

  for (filterIdx = 0;
       filterIdx < numFilters;
       filterIdx++)
    {

      for (chanIdx = 0;
           chanIdx < numChans;
           chanIdx ++)
        {

          pSampleStart = wave[filterIdx][chanIdx];
          pSampleEnd = pSampleStart + filterLen;

          if ((filterLen>>2)<<2 == filterLen)
            {
              mean = 0.0;
              nyquist = 0.0;
              for (pSample = pSampleStart;
                   pSample < pSampleEnd;
                   pSample+=2)
                {
                  mean += *pSample + *(pSample+1);
                  nyquist += *pSample - *(pSample+1);
                }
              mean /= filterLen;
              nyquist /= filterLen;
              for (pSample = pSampleStart;
                   pSample < pSampleEnd;
                   pSample+=2)
                {
                  (*pSample) -= mean + nyquist;
                  (*(pSample+1)) -= mean - nyquist;
                }
            }
          else
            {
              mean = 0.0;
              for (pSample = pSampleStart;
                   pSample < pSampleEnd;
                   pSample++)
                {
                  mean += *pSample;
                }
              mean /= filterLen;
              for (pSample = pSampleStart;
                   pSample < pSampleEnd;
                   pSample++)
                {
                  (*pSample) -= mean;
                }
            }
        }
    }
  centeredAndDenyquisted = 1;
  normalized = 0;

  return(centeredAndDenyquisted);
}

/* set the tableFileName property to filename */
char* MP_Anywave_Table_c::set_table_file_name( const char* filename )
{
  strcpy( tableFileName, filename );

  return(tableFileName);
}

/* set the dataFileName property to filename */
char* MP_Anywave_Table_c::set_data_file_name( const char* filename )
{
  strcpy( dataFileName, filename );

  return(dataFileName);
}

/* printing to a stream */
void MP_Anywave_Table_c::writeTable( FILE *fidTable, const char *szDatasName )
{
	/* Print the xml declaration */
	fprintf( fidTable, "<?xml version=\"1.0\" encoding=\"ISO-8859-1\"?>\n" );
	/* Print opening <table> tag */
	fprintf( fidTable, "<table>\n" );
	/* Print the lib version */
	fprintf( fidTable, "<libVersion>%s</libVersion>\n", VERSION );
	/* Print the parameters */
	fprintf( fidTable, "\t<param name=\"numChans\" value=\"%i\"/>\n", numChans );
	fprintf( fidTable, "\t<param name=\"filterLen\" value=\"%li\"/>\n", filterLen );
	fprintf( fidTable, "\t<param name=\"numFilters\" value=\"%li\"/>\n", numFilters );
	fprintf( fidTable, "\t<param name=\"data\" value=\"%s\"/>\n", szDatasName );
	/* Print the closing </table> tag */
	fprintf( fidTable, "</table>\n");
	
	return;
}

/* printing to a stream */
void MP_Anywave_Table_c::writeDatas( FILE *fidDatas )
{
	unsigned long int iFilterIdx, iChanIdx;
	
	for (iFilterIdx = 0 ; iFilterIdx < numFilters ; iFilterIdx++)
		for(iChanIdx = 0 ; iChanIdx < numChans ; iChanIdx++)
			fwrite(wave[iFilterIdx][iChanIdx], sizeof(double), filterLen, fidDatas);

	return;
}

/**********************/
/* Printing to a file */
unsigned long int MP_Anywave_Table_c::write( const char *szTableName, const char *szDatasName )
{

	FILE *fidTable, *fidDatas;

	if((fidTable = fopen(szTableName,"wb")) == NULL)
    {
		mp_error_msg( "MP_Anywave_Table_c::print", "Could not open file %s to write a table\n",szTableName);
		return(false);
    }
	writeTable(fidTable, szDatasName);
	fclose (fidTable);
	
	if((fidDatas = fopen(szDatasName,"wb")) == NULL)
    {
		mp_error_msg( "MP_Anywave_Table_c::print", "Could not open file %s to write the wave datas\n",szDatasName);
		return(false);
	}
	writeDatas(fidDatas);
	fclose (fidDatas);
	
  return(true);

}

MP_Anywave_Table_c* MP_Anywave_Table_c::copy( void )
{

  size_t numBytes;
  unsigned long int filterIdx;
  unsigned short int chanIdx;
  MP_Real_t *pSampleNew;


  MP_Anywave_Table_c* newTable = new MP_Anywave_Table_c();
  newTable->set_table_file_name( tableFileName );
  newTable->set_data_file_name( dataFileName );
  newTable->numChans = numChans;
  newTable->filterLen = filterLen;
  newTable->numFilters = numFilters;
  newTable->normalized = normalized;

  newTable->storage = NULL;


  /* number of elements to be read in the data file */
  if ((numChans != 0) && (numFilters != 0) && (filterLen != 0))
    {
      /* check that multiplying the three dimensions will not go over the maximum size of an unsigned long int */
      if ( ((double)MP_MAX_SIZE_T / (double) numChans / (double) numFilters / (double) filterLen) / (double)sizeof(MP_Real_t) <= 1.0)
        {
          mp_error_msg( "MP_Anywave_Table_c::copy", "numChans [%lu] . numFilters [%lu] . filterLen [%lu] . sizeof(MP_Real_t) [%lu] is greater than the max for a size_t [%lu]. Cannot use malloc for allocating space for storing the anywave filters. Exiting from copy().\n", numChans, numFilters, filterLen, sizeof(MP_Real_t), MP_MAX_SIZE_T);

          return( NULL );
        }
    }
  numBytes = numChans * numFilters * filterLen * sizeof(MP_Real_t);

  /* try to allocate room for storage */
  if ( ( newTable->storage = (MP_Real_t *)malloc( numBytes ) ) == NULL )
    {
      mp_error_msg( "MP_Anywave_Table_c::copy", "Can't allocate room for storage - Leave the storage empty'.\n" );
      return(NULL);
    }
  else
    {
      /* fill in the storage */
      memcpy( newTable->storage, storage, numBytes );

      /* try to allocate room for wave */
      if ( newTable->alloc_wave() == false )
        {
          mp_error_msg( "MP_Anywave_Table_c::copy", "Can't allocate room for wave - Leave the wave array empty.\n" );
          if (newTable->storage != NULL) free(newTable->storage);
          return(NULL);
        }
      else
        {
          /* fill in the pointers in wave */
          for (filterIdx = 0, pSampleNew = newTable->storage;
               filterIdx < numFilters;
               filterIdx ++)
            {
              for (chanIdx = 0;
                   chanIdx < numChans;
                   chanIdx ++, pSampleNew += filterLen)
                {
                  newTable->wave[filterIdx][chanIdx] = pSampleNew;
                }
            }
        }
    }

  return( newTable );
}


MP_Anywave_Table_c* MP_Anywave_Table_c::create_hilbert_dual( char* name )
{

  unsigned long int filterIdx;
  unsigned short int chanIdx;
  MP_Real_t *pSample;
  MP_Real_t *pSampleNew;

  MP_Real_t *real;
  MP_Real_t *imag;
  MP_Real_t *pImag;
  MP_Real_t *pReal;
  MP_Real_t *pRealEnd;
  MP_Real_t *pImagEnd;
  MP_Real_t temp;

  bool nyquistPresent;
  unsigned long int fftLen;

  MP_FFT_Interface_c* fftInt = MP_FFT_Interface_c::init( filterLen, DSP_RECTANGLE_WIN, 0.0, filterLen );

  MP_Anywave_Table_c* newTable = copy();
  if (newTable == NULL)
    {
      mp_error_msg( "MP_Anywave_Table_c::create_hilbert_dual", "Can't copy the anywave table before modiiying it. Returning a NULL anywave table\n" );
      return(NULL);
    }

  /* try to allocate room for real */
  if ( ( real = (MP_Real_t *)malloc( filterLen * sizeof(MP_Real_t) ) ) == NULL )
    {
      mp_error_msg( "MP_Anywave_Table_c::create_hilbert_dual", "Can't allocate room for real - Leave the real empty'.\n" );
      delete(newTable);
      return(NULL);
    }
  /* try to allocate room for imag */
  if ( ( imag = (MP_Real_t *)malloc( filterLen * sizeof(MP_Real_t) ) ) == NULL )
    {
      mp_error_msg( "MP_Anywave_Table_c::create_hilbert_dual", "Can't allocate room for imag - Leave the imag empty'.\n" );
      delete(newTable);
      return(NULL);
    }

  newTable->set_table_file_name( name );

  fftLen = filterLen / 2 + 1;
  nyquistPresent = ( (fftLen-1)*2  == filterLen);

  for (filterIdx = 0;
       filterIdx < numFilters;
       filterIdx ++) {

    for (chanIdx = 0;
	 chanIdx < numChans;
	 chanIdx ++) {

      pSampleNew = newTable->wave[filterIdx][chanIdx];
      pSample = wave[filterIdx][chanIdx];

      /* fft of the filter */
      fftInt->exec_complex( pSample, real, imag );

      /* Setting the mean to zero */
      *real = (MP_Real_t)0.0;
      *imag = (MP_Real_t)0.0;
      pRealEnd = real + fftLen - 1;
      pImagEnd = imag + fftLen - 1;
      /* multiplying the complex numbers by -i */
      for (pReal = real + 1, pImag = imag + 1;
	   pReal < pRealEnd;
	   pReal ++, pImag ++)
	{
	  temp = *pReal;
	  *pReal = *pImag;
	  *pImag = - temp;
	}
      /* Setting the Nyquist component to zero */
      if (nyquistPresent)
	{
	  *pRealEnd = (MP_Real_t)0.0;
	  *pImagEnd = (MP_Real_t)0.0;
	}

      /* ifft of the filter */
      fftInt->exec_complex_inverse( real, imag, pSampleNew );

    }
  }
  
  newTable->centeredAndDenyquisted = 1;

  if (real) {
    free (real);
  }
  if (imag) {
    free (imag);
  }
   
  if (fftInt) {
    delete (fftInt);
  }
  return(newTable);
}
