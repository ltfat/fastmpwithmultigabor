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
#include "base64.h"
#include "md5sum.h"
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
	set_table_file_name( "" );
	set_data_file_name( "" );
	numChans = 0;
	filterLen = 0;
	numFilters = 0;
	normalized = 0;
	centeredAndDenyquisted = 0;
	storage = NULL;
	wave = NULL;
	szKeyTable = NULL;
}

/**************/
/* destructor */
MP_Anywave_Table_c::~MP_Anywave_Table_c()
{
	mp_debug_msg( MP_DEBUG_DESTRUCTION, "MP_Anywave_Table_c::~MP_Anywave_Table_c()", "Entering the anywave table destructor...\n" );
	reset();
	mp_debug_msg( MP_DEBUG_DESTRUCTION, "MP_Anywave_Table_c::~MP_Anywave_Table_c()", "Exiting the anywave table destructor...\n" );
}

/***************************/
/* OTHER METHODS           */
/***************************/
/* Test */
bool MP_Anywave_Table_c::test( char* filename )
{
	unsigned long int sampleIdx;

	fprintf( stdout, "\n\n-- Entering MP_Anywave_Table_c::test \n" );
	// Create a Anywave_Table
	MP_Anywave_Table_c* tablePtr = new MP_Anywave_Table_c();
	if (NULL == tablePtr ) 
	{
		fprintf( stdout, "Can't create a anywave table - Returning false.\n");
		return false;
	} 
	if(!tablePtr->AnywaveCreator(filename))
	{
		fprintf( stdout, "Can't initialise a anywave table from paramMap - Returning false.\n");
		return false;
	} 

	// print the first channel of the first filter before normalization
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

/************************/
/* fileName Creator		*/
bool MP_Anywave_Table_c::AnywaveCreator( char* fileName )
{
	const char	*func = "MP_Anywave_Table_c::MP_Anywave_Table_c";

	if((szKeyTable = (char *)calloc(MD5_ALLOC_SIZE,sizeof(char))) == NULL)
	{
			mp_error_msg( func, "Can't allocate room for szKeyTable - returning false.\n");
			return false;
	}
	if (set_table_file_name( fileName ) == NULL)
    {
		mp_error_msg( func, "Can't alloc tableFileName to the size of fileName : %s  - Default initialization by 'void constructor'.\n", fileName );
		reset();
		return false;
	}
	if (!parse_xml_file( fileName )) 
    {
		mp_error_msg( func, "Can't parse the file %s - Default initialization by 'void constructor'.\n", fileName );
		reset();
		return false;
    }
	return true;
}


/************************/
/* paramMap Creator		*/
bool MP_Anywave_Table_c::AnywaveCreator( map<string, string, mp_ltstring> *paramMap )
{
	const char		*func = "MP_Anywave_Table_c::MP_Anywave_Table_c";
	char			*convertEnd;

	szKeyTable = (char *)calloc(MD5_ALLOC_SIZE,sizeof(char));

	// Set the parameters values from paramMap
	numChans = (unsigned short int)strtol((*paramMap)["numChans"].c_str(),&convertEnd, 10);
	if (*convertEnd != '\0')
	{
		mp_error_msg( func, "cannot convert parameter numChans in unsigned short int.\n" );
		return false;
    }
	filterLen = strtol((*paramMap)["filterLen"].c_str(),&convertEnd, 10);
	if (*convertEnd != '\0')
	{
		mp_error_msg( func, "cannot convert parameter filterLen in unsigned long int.\n" );
		return false;
    }
	numFilters = strtol((*paramMap)["numFilters"].c_str(),&convertEnd, 10);
	if (*convertEnd != '\0')
	{
		mp_error_msg( func, "cannot convert parameter numFilters in unsigned long int.\n" );
		return false;
    }

	// Load the datas
	if (!load_data_anywave((char *)(*paramMap)["data"].c_str()))
		return false;

	return true;
}

/* load the data contained in dataFileName, store it in storage and update the pointers in wave */
bool MP_Anywave_Table_c::load_data_anywave( void )
{
	const char			*func = "MP_Anywave_Table_c::load_data_anywave(void)";
	FILE				*pFile = NULL;
	unsigned long int	numElements;
	unsigned long int	numReadElements;
	unsigned long int	filterIdx;
	unsigned short int	chanIdx;
	MP_Real_t			*pSample;

	// 1) Verifying "storage" : If storage is not empty, do nothing
	if ( storage != NULL )
    {
		mp_error_msg(func, "the storage is already full. We leave this and do nothing\n" );
		return false;
    }
	// 2) Verifying that all the parameters are available
	if ( (numChans == 0) && (filterLen == 0) && (numFilters == 0) && (dataFileName == NULL) )
    {
		mp_error_msg(func, "can't try to load the waveform data, missing parameters. Leave the storage empty\n" );
		return false;
    }
	// 3) Trying to open the file
	if ((pFile = fopen (dataFileName,"rb")) == NULL )
    {
		mp_error_msg(func, "Can't open the file %s - Leave the storage empty'.\n", dataFileName );
		return false;
    }
	// 4) Calculating the number of elements to be read in the data file
	if ((numChans != 0) && (numFilters != 0) && (filterLen != 0))
    {
		// check that multiplying the three dimensions will not go over the maximum size of an unsigned long int
		if ( ((double)MP_MAX_SIZE_T / (double) numChans / (double) numFilters / (double) filterLen) / (double)sizeof(MP_Real_t) <= 1.0)
        {
			mp_error_msg(func, "numChans [%lu] . numFilters [%lu] . filterLen [%lu] . sizeof(MP_Real_t) [%lu] is greater than the max for a size_t [%lu]. Cannot use malloc for allocating space for storing the anywave filters. Exiting from load_data().\n", numChans, numFilters, filterLen, sizeof(MP_Real_t), MP_MAX_SIZE_T);
			if(pFile) { fclose(pFile); pFile = NULL;}
			return false;
        }
    }
	numElements = numChans * numFilters * filterLen;

	// 5) Trying to allocate room for storage
	if ( ( storage = (MP_Real_t *)malloc( numElements * sizeof(MP_Real_t) ) ) == NULL )
    {
		mp_error_msg(func, "Can't allocate room for storage - Leave the storage empty'.\n" );
		if(pFile) { fclose(pFile); pFile = NULL;}
		return false;
    }
    // 6) Trying to read the file
	if ( (numReadElements = (unsigned long int)mp_fread( storage, sizeof(MP_Real_t), numElements, pFile )) != numElements )
	{
		mp_error_msg(func, "number of elements read is %lu instead of %lu. Clear the storage\n", numReadElements, numElements );
		if(pFile) {fclose(pFile); pFile = NULL;}
		if (storage) {free( storage ); storage = NULL;}
		return false;
	}
	// 7) Trying to allocate room for wave
	if (!alloc_wave())
	{
		mp_error_msg(func, "Can't allocate room for wave - Leave the wave array empty.\n" );
		if(pFile) {fclose(pFile); pFile = NULL;}
		if (storage) {free( storage ); storage = NULL;}
		return false;
	}
	// 8) fill in the pointers in wave
	for (filterIdx = 0, pSample = storage ; filterIdx < numFilters ; filterIdx ++)
	{
		for (chanIdx = 0 ; chanIdx < numChans ; chanIdx ++, pSample += filterLen)
			wave[filterIdx][chanIdx] = pSample;
	}

	if(pFile) {fclose(pFile); pFile = NULL;}
	return true;
}

/* load the data contained in the szInputDatas, store it in storage and update the pointers in wave */
bool MP_Anywave_Table_c::load_data_anywave( char *szInputDatas )
{
	const char			*func = "MP_Anywave_Table_c::load_data_anywave(char *szInputDatas)";
	unsigned long int	numElements;
	unsigned long int	filterIdx;
	unsigned short int	chanIdx;
	MP_Real_t			*pSample;
	string				strEncoded, strDecoded;

	// 1) Verifying "storage" : If storage is not empty, do nothing
	if ( storage != NULL )
    {
		mp_error_msg(func, "the storage is already full. We leave this and do nothing\n" );
		return false;
    }
	// 2) Verifying that all the parameters are available
	if ( (numChans == 0) && (filterLen == 0) && (numFilters == 0) && (dataFileName == NULL) )
    {
		mp_error_msg(func, "can't try to load the waveform data, missing parameters. Leave the storage empty\n" );
		return false;
    }
	// 3) Calculating the number of elements to be read in the data file
	if ((numChans != 0) && (numFilters != 0) && (filterLen != 0))
    {
		// check that multiplying the three dimensions will not go over the maximum size of an unsigned long int
		if ( ((double)MP_MAX_SIZE_T / (double) numChans / (double) numFilters / (double) filterLen) / (double)sizeof(MP_Real_t) <= 1.0)
        {
			mp_error_msg(func, "numChans [%lu] . numFilters [%lu] . filterLen [%lu] . sizeof(MP_Real_t) [%lu] is greater than the max for a size_t [%lu]. Cannot use malloc for allocating space for storing the anywave filters. Exiting from load_data().\n", numChans, numFilters, filterLen, sizeof(MP_Real_t), MP_MAX_SIZE_T);
			return false;
        }
    }
	numElements = numChans * numFilters * filterLen;

	// 4) Trying to allocate room for storage
	if ( ( storage = (MP_Real_t *)malloc( numElements * sizeof(MP_Real_t) ) ) == NULL )
    {
		mp_error_msg(func, "Can't allocate room for storage - Leave the storage empty'.\n" );
		return false;
    }
	
	// 5) Trying to decode the base64 datas
	strEncoded = szInputDatas;
	strDecoded = base64_decode(strEncoded);

	// 6) Trying to modify it into MP_Real_t
	memcpy(storage,(double *)strDecoded.c_str(),numElements * sizeof(MP_Real_t));

	// 7) Trying to allocate room for wave
	if (!alloc_wave())
	{
		mp_error_msg(func, "Can't allocate room for wave - Leave the wave array empty.\n" );
		if (storage) {free( storage ); storage = NULL;}
		return false;
	}
	// 8) fill in the pointers in wave
	for (filterIdx = 0, pSample = storage ; filterIdx < numFilters ; filterIdx ++)
	{
		for (chanIdx = 0 ; chanIdx < numChans ; chanIdx ++, pSample += filterLen)
			wave[filterIdx][chanIdx] = pSample;
	}
	return true;
}

/*Allocate the pointers array wave, using the dimensions numFilters and numChans */
bool MP_Anywave_Table_c::alloc_wave( void )
{
	const char			*func = "MP_Anywave_Table_c::alloc_wave(void)";
	unsigned long int	filterIdx;
  
	// Freeing the wave before allocating
	free_wave();

	// Testing if we can allocate
	if ((double)MP_MAX_SIZE_T / (double)numFilters / (double)sizeof(MP_Real_t **) <= 1.0)
    {
      mp_error_msg(func, "numFilters [%lu] . sizeof(MP_Real_t**) [%lu] is greater than the max for a size_t [%lu]. Cannot use malloc for allocating space for arrays. Exiting from alloc_wave().\n", numFilters, sizeof(MP_Real_t**), MP_MAX_SIZE_T);
      return( false );
    }
	if ((double)MP_MAX_SIZE_T / (double)numChans / (double)sizeof(MP_Real_t *) <= 1.0)
    {
      mp_error_msg(func, "numChans [%lu] . sizeof(MP_Real_t*) [%lu] is greater than the max for a size_t [%lu]. Cannot use malloc for allocating space for arrays. Exiting from alloc_wave().\n", numChans, sizeof(MP_Real_t*), MP_MAX_SIZE_T);
      return( false );
    }
	if ( (wave = (MP_Real_t ***) malloc( numFilters * sizeof(MP_Real_t **) ) ) == NULL )
    {
      mp_error_msg(func, "Can't allocate room for wave - Leave the wave array empty" );
      return(false);
	}
	
	// Allocating
	for ( filterIdx = 0 ; filterIdx < numFilters ; filterIdx ++ )
    {
		if((wave[filterIdx] = (MP_Real_t **) malloc( numChans * sizeof(MP_Real_t *))) == NULL)
        {
			mp_error_msg(func, "Can't allocate room for wave - Leave the wave array empty" );
			free_wave();
			return(false);
        }
    }
  return true;
}

/* Free the pointer array wave */
void MP_Anywave_Table_c::free_wave( void )
{
	unsigned long int filterIdx;

	if ( wave != NULL )
    {
		for ( filterIdx = 0 ; filterIdx < numFilters ; filterIdx ++ )
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
	return;
}

/* Re-initialize all the members */
void MP_Anywave_Table_c::reset( void )
{
	mp_debug_msg( MP_DEBUG_DESTRUCTION, "MP_Anywave_Table_c::reset()", "Entering...\n" );

	free_wave();
	if (storage != NULL) 
	{
		free(storage);
		storage = NULL;
	}
	if(szKeyTable != NULL)
	{
		free(szKeyTable);
		szKeyTable = NULL;
	}
	set_table_file_name( "" );
	set_data_file_name( "" );
	numChans = 0;
	filterLen = 0;
	numFilters = 0;
	normalized = 0;
	centeredAndDenyquisted = 0;

	mp_debug_msg( MP_DEBUG_DESTRUCTION, "MP_Anywave_Table_c::reset()", "Exiting...\n" );
	return;
}

bool MP_Anywave_Table_c::parse_xml_file(const char* fName)
{
	const char		*func = "MP_Anywave_Table_c::parse_xml_file(const char* fName)";
	TiXmlNode		  *nodeFinal = NULL;
	TiXmlElement	*paramTable = NULL;
	TiXmlElement	*elementTable = NULL;
	TiXmlElement	*elementVersion = NULL;
	TiXmlHandle		handleFinal = NULL;
	char			    *convertEnd;
	string			  libVersion;

	// Get a handle on the document
	TiXmlDocument doc(fName);
	if (!doc.LoadFile())
    {
		mp_error_msg( func, "Error while loading the table description file [%s].\n", fName );
		mp_error_msg( func, "Error ID: %u .\n", doc.ErrorId() );
		mp_error_msg( func, "Error description: %s .\n", doc.ErrorDesc());
		return  false;
	}
	TiXmlHandle hdl(&doc);

	// Get a handle on the tags "table"
	elementTable = hdl.FirstChildElement("table").Element();
	if (elementTable == NULL)
    {
		mp_error_msg( func, "Error, cannot find the elementTable for xml property :\"table\".\n");
		return false;

	}
	nodeFinal = hdl.FirstChildElement("table").ToNode();
	if (nodeFinal == NULL)
    {
		mp_error_msg( func, "Error, cannot find the nodeFinal for xml property :\"table\".\n");
		return false;

	}
	// save this for later
	handleFinal=TiXmlHandle(elementTable);

	//----------------------------------
	// 1) Retrieving the library version
	//----------------------------------
	// Get a handle on the tags "libVersion"
	elementVersion = handleFinal.FirstChildElement("libVersion").Element();
	if (elementVersion == NULL)
    {
		mp_error_msg( func, "Error, cannot find the xml property :\"libVersion\".\n");
		return false;
	}
	libVersion = elementVersion->GetText();

	//------------------------------------
	// 2) Retrieving the others parameters
	//------------------------------------
	paramTable = nodeFinal->FirstChildElement("param");
	if (paramTable == NULL)    
	{
		mp_error_msg( func, "No parameter to define the table in %s file.\n", fName );
		return false;
    }
	for ( ; paramTable!=0 ; paramTable = paramTable->NextSiblingElement("param"))
    {
		if(!(strcmp(paramTable->Attribute("name"),"filterLen")))
        {
			filterLen =strtol(paramTable->Attribute("value"), &convertEnd, 10);
			if (*convertEnd != '\0')
			{
				mp_error_msg( func, "cannot convert parameter filterLen in unsigned long int.\n" );
				return false;
            }
        }
		if(!(strcmp(paramTable->Attribute("name"),"numChans")))
        {
			numChans =(unsigned short int) strtol(paramTable->Attribute("value"), &convertEnd, 10);
			if(*convertEnd != '\0')
            {
				mp_error_msg( func, "cannot convert parameter numChans in unsigned long int.\n" );
				return false;
            }
        }
		if(!(strcmp(paramTable->Attribute("name"),"numFilters")))
        {
			numFilters =strtol(paramTable->Attribute("value"), &convertEnd, 10);
			if (*convertEnd != '\0')
            {
				mp_error_msg( func, "cannot convert parameter numFilters in unsigned long int.\n" );
				return false;
            }
        }
		if(!(strcmp(paramTable->Attribute("name"),"data")))
        {
			if(set_data_file_name(paramTable->Attribute("value") ) == NULL)
				return false;
			if (!load_data_anywave())
				return false;
        }
    }
	return true;
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

/* set the dataFileName property to filename */
char* MP_Anywave_Table_c::set_key_table( const char* szkeyTableName )
{
	memcpy(szKeyTable,szkeyTableName,MD5_ALLOC_SIZE*sizeof(char));
	return(szKeyTable);
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
	fprintf( fidTable, "\t<param name=\"normalized\" value=\"%li\"/>\n", normalized );
	fprintf( fidTable, "\t<param name=\"centeredAndDenyquisted\" value=\"%li\"/>\n", centeredAndDenyquisted );
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

	// 1) Create a new table
	MP_Anywave_Table_c* newTable = new MP_Anywave_Table_c();
	// 2) Copy the paths
	newTable->set_table_file_name( tableFileName );
	newTable->set_data_file_name( dataFileName );
	// 3) Copy the parameters
	newTable->numChans = numChans;
	newTable->filterLen = filterLen;
	newTable->numFilters = numFilters;
	newTable->normalized = normalized;

	// 4) Allocate the keyName
	if((newTable->szKeyTable = (char *)calloc(MD5_ALLOC_SIZE,sizeof(char))) == NULL )
	{
			mp_error_msg( "MP_Anywave_Table_c::copy", "Can't allocate room for szKeyTable - Leave the storage empty.\n");
			return( NULL );
	}
	// 5) Allocate the storage
	newTable->storage = NULL;
	// number of elements to be read in the data file
	if ((numChans != 0) && (numFilters != 0) && (filterLen != 0))
    {
		// check that multiplying the three dimensions will not go over the maximum size of an unsigned long int
		if ( ((double)MP_MAX_SIZE_T / (double) numChans / (double) numFilters / (double) filterLen) / (double)sizeof(MP_Real_t) <= 1.0)
        {
			mp_error_msg( "MP_Anywave_Table_c::copy", "numChans [%lu] . numFilters [%lu] . filterLen [%lu] . sizeof(MP_Real_t) [%lu] is greater than the max for a size_t [%lu]. Cannot use malloc for allocating space for storing the anywave filters. Exiting from copy().\n", numChans, numFilters, filterLen, sizeof(MP_Real_t), MP_MAX_SIZE_T);
			return( NULL );
        }
    }
	numBytes = numChans * numFilters * filterLen * sizeof(MP_Real_t);
	// try to allocate room for storage
	if ( ( newTable->storage = (MP_Real_t *)malloc( numBytes ) ) == NULL )
    {
		mp_error_msg( "MP_Anywave_Table_c::copy", "Can't allocate room for storage - Leave the storage empty.\n" );
		return(NULL);
    }
    // fill in the storage
    memcpy( newTable->storage, storage, numBytes );

	// 6) Try to allocate room for wave
	if ( newTable->alloc_wave() == false )
	{
		mp_error_msg( "MP_Anywave_Table_c::copy", "Can't allocate room for wave - Leave the wave array empty.\n" );
		if (newTable->storage != NULL) 
			free(newTable->storage);
		return(NULL);
	}
	// fill in the pointers in wave
	for (filterIdx = 0, pSampleNew = newTable->storage ; filterIdx < numFilters ; filterIdx ++)
	{
		for (chanIdx = 0 ; chanIdx < numChans ; chanIdx ++, pSampleNew += filterLen)
		{
			newTable->wave[filterIdx][chanIdx] = pSampleNew;
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

  newTable->set_key_table( name );

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


/* Free the pointer array wave */
string MP_Anywave_Table_c::encodeBase64( char *szStorage, int iSizeToEncode)
{
	return base64_encode((const unsigned char *)szStorage,iSizeToEncode);
}
