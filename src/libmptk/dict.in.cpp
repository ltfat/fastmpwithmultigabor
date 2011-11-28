/******************************************************************************/
/*                                                                            */
/*                                 dict.cpp                                   */
/*                                                                            */
/*                        Matching Pursuit Library                            */
/*                                                                            */
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
 * $Author:broy $
 * $Date:2007-01-22 12:12:05 +0100 (Mon, 22 Jan 2007) $
 * $Revision:832 $
 *
 */

/********************************************/
/*                                          */
/* dict.in.cpp: methods for class MP_Dict_c */
/*                                          */
/********************************************/

#include "mptk.h"
#include "mp_system.h"
#include "mp_pthreads_barrier.h"
#include "block_factory.h"

#cmakedefine MULTITHREAD 1
/***************************/
/* CONSTRUCTORS/DESTRUCTOR */
/***************************/

/**********************************************/
/* Initialization from a dictionary file name */

MP_Dict_c* MP_Dict_c::init(  const char *dictFileName )
{
	const char* func = "MP_Dict_c::init(dictFileName)";
	MP_Dict_c *newDict = NULL;

	// Instantiate and check
	newDict = new MP_Dict_c();
	if ( newDict == NULL )
    {
		mp_error_msg( func, "Failed to create a new dictionary.\n" );
		return( NULL );
    }
	
	// Add some blocks read from the dict file
	newDict->add_blocks( dictFileName );
	// Note: with a NULL signal, add_blocks will build all the signal-independent parts of the blocks. 
	// It is then necessary to run a dict.copy_signal(sig)
	// or a dict.plug_signal(sig) to actually use the dictionary.
	if ( newDict->numBlocks == 0 )
    {
		mp_error_msg( func, "The dictionary scanned from file [%s] contains no recognized blocks.\n",dictFileName );
		if ( newDict ) 
			delete( newDict );
		return( NULL );
    }

	return( newDict );
}

MP_Dict_c* MP_Dict_c::init(FILE *fid )
{
	const char* func = "MP_Dict_c::init(fid)";
	MP_Dict_c *newDict = NULL;
	
	// Instantiate and check
	newDict = new MP_Dict_c();
	if ( newDict == NULL )
    {
		mp_error_msg( func, "Failed to create a new dictionary.\n" );
		return( NULL );
    }
	
	// Add some blocks read from the dict file
	newDict->add_blocks( fid );
	// Note: with a NULL signal, add_blocks will build all the signal-independent parts of the blocks. 
	// It is then necessary to run a dict.copy_signal(sig)
	// or a dict.plug_signal(sig) to actually use the dictionary.
	if ( newDict->numBlocks == 0 )
    {
		mp_error_msg( func, "The dictionary scanned from stdin contains no recognized blocks.\n");
		if ( newDict ) 
			delete( newDict );
		return( NULL );
    }
	
	return( newDict );
}

/*************************************************/
/* Plain initialization, with no data whatsoever */
MP_Dict_c* MP_Dict_c::init( void )
{
	const char* func = "MP_Dict_c::init(void)";
	MP_Dict_c *newDict = NULL;

	// Instantiate and check
	newDict = new MP_Dict_c();
	if ( newDict == NULL )
    {
		mp_error_msg( func, "Failed to create a new dictionary.\n" );
		return( NULL );
    }

	return( newDict );
}


/**************/
/* NULL constructor */
MP_Dict_c::MP_Dict_c()
{
	const char* func = "MP_Dict_c::MP_Dict_c()";

	mp_debug_msg( MP_DEBUG_CONSTRUCTION, func, "Constructing dict...\n" );
	signal = NULL;
	sigMode = MP_DICT_NULL_SIGNAL;
	numBlocks = 0;
	block = NULL;
	blockWithMaxIP = UINT_MAX;
	touch = NULL;
	maxFilterLen = 0;
#ifdef MULTITHREAD
	threads = NULL;
	tasks = NULL;
	bar = NULL;
#endif
	mp_debug_msg( MP_DEBUG_CONSTRUCTION, func, "Done.\n" );
}


/**************/
/* Destructor */
MP_Dict_c::~MP_Dict_c()
{
	unsigned int i;
	const char* func = "MP_Dict_c::~MP_Dict_c()";
  
	mp_debug_msg( MP_DEBUG_DESTRUCTION, func, "Deleting dict...\n" );

	// Deleting the signal
	if ( (sigMode == MP_DICT_INTERNAL_SIGNAL) && ( signal != NULL ) ) 
		delete signal;
	// Deleting the block
	if ( block )
    {
		for ( i=0; i<numBlocks; i++ )
        {
			if ( block[i] ) 
				delete( block[i] );
        }
		free( block ); block = NULL;
    }
#ifdef MULTITHREAD
	//Cancel all Threads
	if (bar && numBlocks > 0) for (i = 0; i < numBlocks; ++i)
    {
		 tasks[i].exit = true ;
    }
	if (bar && numBlocks > 0) 
		bar[0]->wait();
	if (bar && numBlocks > 0) for (i = 0; i < numBlocks; ++i)
    {
		if (pthread_join(threads[i], NULL))
        {
			//To do:Create message if cancelation of threads failed
			mp_error_msg( func, "Failed to cancel threads.\n" );
        }
    }

	if (threads && numBlocks > 0) 
	   delete [] threads;
	if (bar && numBlocks > 0)
	{  
		for (i = 0; i < 2; ++i)
		{
			if (bar[i]) 
				delete  bar[i];
		}
		delete [] bar;
   }
   if (numBlocks > 0) delete [] val;
   if (numBlocks > 0) delete [] tasks ;

#else
#endif
  if ( touch ) free( touch );
  mp_debug_msg( MP_DEBUG_DESTRUCTION, func, "Done.\n" );
}


/***************************/
/* I/O METHODS             */
/***************************/
/*************************************************************************/
/* Parse the described blocks of a dictionary   */
int MP_Dict_c::parse_xml_file(TiXmlDocument doc)
{
	const char		*func = "MP_Dict_c::parse_xml_file(TiXmlDocument doc)";
	TiXmlNode		*nodeBlockProperties = NULL;
	TiXmlNode		*nodeBlock = NULL;
	TiXmlElement	*elementDict = NULL;
	TiXmlElement	*elementVersion = NULL;
	TiXmlHandle		handleDict = NULL;
	map<string, PropertiesMap, mp_ltstring> *propertyMap = new map<string, PropertiesMap, mp_ltstring>();
	int				count = 0;
	int				finalcount = 0;
	string			libVersion;
  
	// Get a handle on the document
	TiXmlHandle hdl(&doc);
	
	// Get a handle on the tags "dict"
	elementDict = hdl.FirstChildElement("dict").Element();
	if (elementDict == NULL)
    {
		mp_error_msg( func, "Error, cannot find the xml property :\"dict\".\n");
		return -1;
	}
	// save this for later
	handleDict=TiXmlHandle(elementDict);
		
	//----------------------------------
	// 1) Retrieving the library version
	//----------------------------------
	// Get a handle on the tags "libVersion"
	elementVersion = handleDict.FirstChildElement("libVersion").Element();
	if (elementVersion == NULL)
    {
		mp_error_msg( func, "Error, cannot find the xml property :\"libVersion\".\n");
		return -1;
	}
	libVersion = elementVersion->GetText();

	//--------------------------------------------
	// 2) Retrieving the block properties if exist
	//--------------------------------------------
	nodeBlockProperties = handleDict.FirstChild("blockproperties").ToNode();
	if (nodeBlockProperties != NULL)
    {
		while (nodeBlockProperties != NULL)
		{
			if (!parse_property(nodeBlockProperties,propertyMap))
			{
				mp_error_msg( func, "Error while processing properties for block.\n");
				if (propertyMap) delete(propertyMap);
				return  -1;
			}
			nodeBlockProperties = nodeBlockProperties->NextSibling("blockproperties");
		}
    }
	else
	{
		if (propertyMap) delete(propertyMap);
		propertyMap = NULL;
		mp_debug_msg( MP_DEBUG_CONSTRUCTION, func,"No properties tag for block.\n" );
	}

	//---------------------------------
	// 3) Retrieving the block if exist
	//---------------------------------
	nodeBlock = handleDict.FirstChild("block").ToNode();
	if (nodeBlock != NULL)
    {
		while(nodeBlock != NULL)
        {
			count= 0;
			count = parse_block(nodeBlock,propertyMap);
			finalcount += count;
			if (0 == count )
            {
				mp_error_msg( func, "Error while processing block.processing the remaining block\n");
            } 
			nodeBlock = nodeBlock->NextSibling("block");
        }
    }
	else
    {
		mp_error_msg( func, "No block node in the dictionnary structure file.\n");
		if (propertyMap) delete(propertyMap);
		return -1;
    }

	if (propertyMap) delete(propertyMap);
  
	return finalcount;
}

/*************************************************************************/
/* Load a dictionary file in xml format and parse the described blocks   */
int MP_Dict_c::load_xml_file(const char* fName)
{
  const char* func = "MP_Dict_c::load_xml_file(const char* fName)";
  TiXmlDocument doc;

  if (!doc.LoadFile(fName))
    {
      mp_error_msg( func, "Error while loading the dictionary file [%s].\n", fName );
      mp_error_msg( func, "Error ID: %u .\n", doc.ErrorId() );
      mp_error_msg( func, "Error description: %s .\n", doc.ErrorDesc());
      return  0;
    }

  else return parse_xml_file(doc);

}

/*************************************************************************/
/* Load a dictionary file in xml format and parse the described blocks   */
int MP_Dict_c::load_xml_file(FILE *fid)
{
	const char		*func = "MP_Dict_c::load_xml_file(FILE *fid)";
	char			line[MP_MAX_STR_LEN];
	char			szBuffer[10000];
	TiXmlDocument	doc;
 
	do
	{
		if ( fgets( line,MP_MAX_STR_LEN,fid) == NULL ) 
		{
			mp_error_msg( func, "Cannot get the format line. This book will remain un-changed.\n" );
			return 0;	
		}
		strcat(szBuffer,line);
	}
	while(strcmp(line,"</dict>\n"));
	
	if (!doc.Parse(szBuffer))
    {
		mp_error_msg( func, "Error while loading the stdin dictionary.\n");
		mp_error_msg( func, "Error ID: %u .\n", doc.ErrorId() );
		mp_error_msg( func, "Error description: %s .\n", doc.ErrorDesc());
		return  0;
    }
	else 
		return parse_xml_file(doc);
}

/******************************/
/* Addition of a single block */
int MP_Dict_c::add_block( MP_Block_c *newBlock )
{

  const char* func = "MP_Dict_c::add_block( newBlock )";
  MP_Block_c **tmp;

  if ( newBlock != NULL )
    {

      /* Increase the size of the array of blocks... */
      if ( (tmp = (MP_Block_c**) realloc( block, (numBlocks+1)*sizeof(MP_Block_c*) )) == NULL )
        {
          mp_error_msg( func, "Can't reallocate memory to add a new block to dictionary."
                        " No new block will be added, number of blocks stays [%d].\n",
                        numBlocks );
          return( 0 );
        }
      /* ... and store the reference on the newly created object. */
      else
        {
          block = tmp;
          block[numBlocks] = newBlock;
          numBlocks++;
          if (maxFilterLen < newBlock->filterLen)
            maxFilterLen = newBlock->filterLen;
        }
      return( 1 );

    }
  /* else, if the block is NULL, silently ignore it,
     just say that no block has been added. */
  else return( 0 );

}

/************************/
/* Scanning from a file */

int MP_Dict_c::add_blocks( const char *fName )
{
  return(  load_xml_file(fName) );
}

/************************/
/* Scanning from a file */

int MP_Dict_c::add_blocks(FILE *fid)
{
	return(load_xml_file(fid) );
}

/************************/
/* Scanning from a tinyXML doc*/
int MP_Dict_c::add_blocks( TiXmlDocument doc )
{
  return(MP_Dict_c::parse_xml_file(doc));
}

/**********************/
/* Printing to a file */
int MP_Dict_c::print( const char *fName )
{    
	const char	*func = "MP_Dict_c::print( const char *fName )";
	FILE		*fid;

	if((fid = fopen(fName,"w")) == NULL)
	{
		mp_error_msg( func,"Could not open file %s to write a dictionary\n",fName );
		return 1;
    }
	if(!print(fid))
	{
		mp_error_msg( func,"Could not write the dictionary datas into the file %s\n",fName );
		return 1;
    }

	fclose ( fid );  
	return 0;
}

/**********************/
/* Printing to a file */
bool MP_Dict_c::print( FILE *fid )
{    
	const char	*func = "MP_Dict_c::print( FILE *fid )";
	map< string, string, mp_ltstring>* paramMap = NULL; 
	map< string, string, mp_ltstring>::iterator iter; 	
	TiXmlDocument doc;
	TiXmlElement* version;
	TiXmlElement* blockElement;
	TiXmlElement* paramElement;
	TiXmlDeclaration* decl;  
	TiXmlElement *root;

	// Declaring the header
	decl = new TiXmlDeclaration( "1.0", "ISO-8859-1", "" );  
	doc.LinkEndChild( decl );  

	// Declaring the "dict" tag
	root = new TiXmlElement( "dict" );  
	doc.LinkEndChild( root ); 
	 
	// Declaring the "libversion" tag
	version = new TiXmlElement( "libVersion" );  
	version->LinkEndChild( new TiXmlText( VERSION ));
	root->LinkEndChild(version);
	
	// Declaring all the "block" tags
	for ( unsigned int i = 0; i < numBlocks; i++ )
    { 
		blockElement = new TiXmlElement( "block" ); 
		root->LinkEndChild(blockElement);
		paramMap = block[i]->get_block_parameters_map();
		if (paramMap) 
		{
			for( iter = (*paramMap).begin(); iter != (*paramMap).end(); iter++ ) 
			{
  				paramElement = new TiXmlElement( "param" ); 
  				blockElement->LinkEndChild(paramElement);
  				paramElement->SetAttribute("name", iter->first.c_str());
				paramElement->SetAttribute("value", iter->second.c_str());
			}
		} 
		else 
		{  
			mp_error_msg( func,"paramMap illformed for block number %u \n",i);
			return false;
		}
	} 
	if (!doc.SaveFile( fid ))
	{ 
		mp_error_msg( func,"Could not save the file\n");
		return false;
	}
	return true;
}

/**********************/
/* Printing to a file */
bool MP_Dict_c::printMultiDict( const char *fName )
{    
    TiXmlDocument	doc;
	TiXmlHandle		handleDict = NULL;
	TiXmlElement	*elementDict = NULL;
	TiXmlElement	*elementBlock = NULL;
	TiXmlElement	*paramElement = NULL;
	map< string, string, mp_ltstring>* paramMap = NULL; 
	map< string, string, mp_ltstring>::iterator iter; 	
	bool			loadOkay;
	
	loadOkay = doc.LoadFile(fName);

    if ( loadOkay )
    {
		// Get a handle on the document
		TiXmlHandle hdl(&doc);
		
		// Get a handle on the tags "dict"
		elementDict = hdl.FirstChildElement("dict").Element();
		if (elementDict == NULL)
		{
			mp_error_msg( "Test", "Error, cannot find the xml property :\"dict\".\n");
			return false;
		}

		// Declaring all the "block" tags
		for ( unsigned int i = 0; i < numBlocks; i++ )
		{ 
			elementBlock = new TiXmlElement( "block" ); 
			elementDict->LinkEndChild(elementBlock);
			paramMap = block[i]->get_block_parameters_map();
			if (paramMap) 
			{
				for( iter = (*paramMap).begin(); iter != (*paramMap).end(); iter++ ) 
				{
					paramElement = new TiXmlElement( "param" ); 
					elementBlock->LinkEndChild(paramElement);
					paramElement->SetAttribute("name", iter->first.c_str());
					paramElement->SetAttribute("value", iter->second.c_str());
				}
			} 
		}
		if (!doc.SaveFile( fName ))
		{ 
			mp_error_msg( "Test","Could not save the file\n");
			return false;
		}
	}
	return true;
}

int MP_Dict_c::add_default_block( const char* blockName ){
	const char* func = "MP_Dict_c::add_default_block( const char* blockName )";
	 MP_Block_c *newBlock = NULL;
	 if (NULL == blockName ) {   mp_error_msg( func, "No block name specified" );
              return 0;
	 }
	map<string, string, mp_ltstring>* localDefaultMap = new map<string, string, mp_ltstring>();
	MP_Block_Factory_c::get_block_factory()->get_block_default_map(blockName)(localDefaultMap);
	
	          /*call the block creator*/
          MP_Block_c* (*blockCreator)( MP_Signal_c *setSignal, map<string, string, mp_ltstring> * paramMap ) = NULL;
          blockCreator = MP_Block_Factory_c::get_block_factory()->get_block_creator(blockName);

          if (NULL == blockCreator)
            {
              mp_error_msg( func, "The %s block type is not registred in the atom factory.\n", blockName );
              if (localDefaultMap) delete(localDefaultMap);
              return 0;
            }
            
          /*Create a new block*/  
          newBlock =  blockCreator(signal, localDefaultMap);

          /*Test if new block is NULL*/ 
          if (NULL != newBlock)
            {
              /*Test if new block has been added*/ 
              if ( add_block( newBlock ) != 1 )
                {
                  mp_warning_msg( func, "Failed to add the block of type %s ."
                                  " Proceeding with the remaining blocks.\n",
                                   blockName
                                );
              if (localDefaultMap) delete(localDefaultMap);
              return 0;
                }
              if (localDefaultMap) delete(localDefaultMap);
              return 1;
            }
          else
            {
              mp_warning_msg( func, "Failed to add the block of type %s ."
                              " Proceeding with the remaining blocks.\n",
                               blockName
                            );
              if (localDefaultMap) delete(localDefaultMap);              
              return 0;
            }

}

/***************************/
/* MISC METHODS            */
/***************************/

/* XML RELATED METHOD */
/*Parse the properties in the corresponding xml syntax if exists*/
bool MP_Dict_c::parse_property(TiXmlNode * pParent, map<string, PropertiesMap, mp_ltstring> *setPropertyMap)
{
  const char* func = "MP_Dict_c::parse_property(TiXmlNode * pParent, map<const char*, PropertiesMap, mp_ltstring> *setPropertyMap)";
  if ( !pParent )
    {mp_error_msg( func, "pParent pointer is NULL" );
      return false;
      
    }
  map<string, string, mp_ltstring> localParameterMap;
  if (pParent->ToElement()->Attribute("refines"))
    {
      if ((*setPropertyMap)[pParent->ToElement()->Attribute("refines")].size()>0)
        {
          /*Load the map to be refined*/
          localParameterMap = map<string, string, mp_ltstring> ((*setPropertyMap)[pParent->ToElement()->Attribute("refines")]);

          TiXmlElement *item = pParent->FirstChildElement("param");
          while(item !=0)
            {      
            	/*Refined the map with new values*/     	
            	localParameterMap[item->Attribute("name")] = item->Attribute("value");
		item = item->NextSiblingElement();
            }
        }
      else
        {
          mp_warning_msg(func,"block properties [%s] used is not define", pParent->ToElement()->Attribute("refines") );
          /*Create a map*/
          localParameterMap = map<string, string, mp_ltstring>();
          
          
          TiXmlElement *item = pParent->FirstChildElement("param");
          while(item !=0)
            {      
            	/*Put values in the map*/     	
            	localParameterMap[item->Attribute("name")] = item->Attribute("value");
		item = item->NextSiblingElement();
            }
          
        }

    }
  else
    {
      TiXmlElement *item = pParent->FirstChildElement("param");
      localParameterMap = map<string, string, mp_ltstring>();
      while (item !=0)
        {
          localParameterMap.insert(pair<string, string>(item->Attribute("name"),
							item->Attribute("value")));
	  item = item->NextSiblingElement();
        }

    }
  /* stock the parameter map in the property map: */
  (*setPropertyMap)[pParent->ToElement()->Attribute("name")] = localParameterMap;
  return true;

}

/*Create a block*/
int MP_Dict_c::create_block(MP_Signal_c * setSignal , map<string, string, mp_ltstring> * setPropertyMap)
{
	const char* func = "MP_Dict_c::create_block(MP_signal_c * setSignal , map<string, PropertiesMap, mp_ltstring> *setPropertyMap)";
	MP_Block_c *newBlock = NULL;
	// Call the block creator
	MP_Block_c* (*blockCreator)( MP_Signal_c *setSignal, map<string, string, mp_ltstring> * paramMap ) = NULL;
	blockCreator = MP_Block_Factory_c::get_block_factory()->get_block_creator((*setPropertyMap)["type"].c_str());

	if (NULL == setPropertyMap)
	{
		mp_error_msg( func, "The %s block type is not registred in the atom factory.\n",(*setPropertyMap)["type"].c_str() );
		delete(setPropertyMap);
		return 0;
	}
	if (NULL == blockCreator)
	{
		mp_error_msg( func, "The %s block creator is not type is not registred in the block factory.\n",(*setPropertyMap)["type"].c_str() );
		return 0;
	}
	//Create a new block
	newBlock =  blockCreator(setSignal, setPropertyMap);

	// Test if new block is NULL
	if (NULL != newBlock)
	{
		// Test if new block has been added
		if ( add_block( newBlock ) != 1 )
		{
			mp_warning_msg( func, "Failed to add the block of type %s . Proceeding with the remaining blocks.\n", (*setPropertyMap)["type"].c_str());
			delete(setPropertyMap);
			return 0;
		}               
		delete(setPropertyMap);          
		return 1;
	}
	else
	{
		mp_warning_msg( func, "Failed to add the block of type %s . Proceeding with the remaining blocks.\n",(*setPropertyMap)["type"].c_str());
		delete(setPropertyMap);              
		return 0;
	}
}

/*Parse the block properties and create the associate blocks*/
int MP_Dict_c::parse_block(TiXmlNode * pParent, map<string, PropertiesMap, mp_ltstring> *setPropertyMap)
{
	const char* func = "MP_Dict_c::parse_block(TiXmlNode * pParent, map<const char*, PropertiesMap, mp_ltstring> *setPropertyMap)";
	TiXmlElement * varParam;
	TiXmlElement * param;
	TiXmlElement * properties;
	int count =0;
	map<string, string, mp_ltstring> * blockMap;
	map<string, list<string>, mp_ltstring> varParamMap;
	map<string, list<string>, mp_ltstring>::iterator varParamListIterator;
	
	//Test if Attribute "uses" exists to refer to the correct blockproperties map*/
    if ( !pParent )
    {
		mp_error_msg( func, "pParent pointer is NULL" );
		return 0;
    }
	if (pParent->ToElement()->Attribute("uses")!=0 && setPropertyMap != NULL )
    {
		if ( (*setPropertyMap)[pParent->ToElement()->Attribute("uses")].size()>0 )
        {
			//create the block map using the correct blockproperties map
			blockMap = new map<string, string, mp_ltstring> ((*setPropertyMap)[pParent->ToElement()->Attribute("uses")]);
        }
		else
        {
			//create an empty block map and warn
			blockMap = new map<string, string, mp_ltstring> ();
			mp_warning_msg(func,"block properties [%s] used is not define .\n", pParent->ToElement()->Attribute("uses") );
        }
    }
	else
    {
		blockMap = new map<string, string, mp_ltstring> ();
    }

	// over ride values in the map*/
	param = pParent->FirstChildElement("param");
	if (param!=0)
    {
		while (param!=0) 
        {
			(*blockMap)[param->Attribute("name")]= param->Attribute("value");
			param = param->NextSiblingElement("param");
        }
    }
	//test if block parameters have varparam
	varParam = pParent->FirstChildElement("varparam");
	if (varParam!=0)
    {
		while(varParam !=0)
        {
			if (varParam->Attribute("name")!= 0)
            {
				list<string> paramList;
				properties = varParam->FirstChildElement("var");
				while(properties != 0)
                {
					paramList.push_back(properties->GetText());
					properties = properties->NextSiblingElement("var");
                }
				varParamMap[varParam->Attribute("name")] = paramList;
            }
			else
            {
				mp_error_msg(func,"variable parameter name not define in the parsed block.\n");
				delete(blockMap);
				return 0;
            }
			varParam = varParam->NextSiblingElement("varparam");
        }

		// Parse recursivly the list of parameters to create all the block
		count = parse_param_list(varParamMap, blockMap );
		if (count ==0) mp_error_msg( func, "No bloc added.\n");
		delete(blockMap);
		return count;
    }
	else
    {
		// Create the block with parameter defines by uses tag
		if ((*blockMap)["type"].c_str() != 0)
        {
			count+=  create_block(signal , blockMap);
			if (count == 0)  
				mp_error_msg( func, "Cannot create block.\n");
			return count;
        }
		else
        {
			mp_error_msg( func, "Bloc type not define in dictionary structure file.\n");
			delete(blockMap);
			return 0;
        }
    }
}

/*Parse the varparam list of a block and create each associate blocks*/
int MP_Dict_c::parse_param_list(map<string, list<string>, mp_ltstring> setVarParam , map<string, string, mp_ltstring> *setPropertyMap)
{
  const char* func = "MP_Dict_c::parse_param_list(TiXmlNode * pParent, map<const char*, PropertiesMap, mp_ltstring> *setPropertyMap)";
  MP_Block_c *newBlock = NULL;
  int count = 0;

  /*Make a copy of the blockMap*/
  map<string, string, mp_ltstring> * blockMapLocal = new map<string, string, mp_ltstring> (*setPropertyMap);
  map<string, list<string>, mp_ltstring>::iterator varParamIterator;
  list<string>::iterator paramListIterator;

 /*Iterate on varparam list to create all parameters map for blocks*/
  varParamIterator = setVarParam.begin();
  if (varParamIterator != setVarParam.end())
    {
      string key = varParamIterator->first;
      list<string> listKeyValue = varParamIterator->second;
      setVarParam.erase(varParamIterator);

      for (paramListIterator = listKeyValue.begin(); paramListIterator != listKeyValue.end(); ++paramListIterator)
        {
          (*blockMapLocal)[key] = (*paramListIterator);
          count+=parse_param_list(setVarParam, blockMapLocal);     
          if (count == 0) mp_error_msg( func, "Error when parsing parameter list\n");
        }
    }
  else
    {
      

      if ((*blockMapLocal)["type"].c_str() != 0)
        {
          /* Call the block creator*/
          MP_Block_c* (*blockCreator)( MP_Signal_c *setSignal, map<string, string, mp_ltstring> * paramMap ) = NULL;
          blockCreator = MP_Block_Factory_c::get_block_factory()->get_block_creator((*blockMapLocal)["type"].c_str());

          if (NULL == blockCreator)
            {
              mp_error_msg( func, "The %s block is not registred in the atom factory.\n",(*blockMapLocal)["type"].c_str() );
              return 0;
            }
          /* Create the block */
          newBlock =  blockCreator(signal, blockMapLocal);
          if (NULL != newBlock)
            {

              if ( add_block( newBlock ) != 1 )
                {
                  mp_warning_msg( func, "Failed to add the  block."
                                  " Proceeding with the remaining blocks.\n"
                                );
                  delete(blockMapLocal);
                  return 0;
                }
                 delete(blockMapLocal);
                 return ++count;
            }
        }
      else
        {
          mp_warning_msg( func, "Current block has no type."
                          " Proceeding with the remaining blocks.\n");
          delete(blockMapLocal);
          return 0;               
        }
   
    }
     delete(blockMapLocal);
     if (count == 0) mp_warning_msg( func, "Current block has no type."
                          " Proceeding with the remaining blocks.\n");
     return count;
}



/* TEST */
bool MP_Dict_c::test( char* signalFileName, char* dicoFileName )
{
/*
  unsigned long int sampleIdx;
  unsigned int blockIdx;

  fprintf( stdout, "\n-- Entering MP_Dict_c::test \n" );
  fflush( stdout );

  MP_Dict_c* dictionary = MP_Dict_c::init( dicoFileName );
  if (dictionary == NULL)
    {
      return(false);
    }
  fprintf( stdout, "\n---- Dictionary created from file %s \n", dicoFileName );
  fflush( stdout );

  if ( dictionary->copy_signal( signalFileName ) )
    {
      return(false);
    }
  fprintf( stdout, "\n---- Plugged the signal from file %s \n", signalFileName );
  fflush( stdout );

  fprintf( stdout, "\n---- Printing the dictionary:\n" );
  fflush( stdout );
  dictionary->print( stdout );


  for (blockIdx = 0;
       blockIdx < dictionary->numBlocks;
       blockIdx ++)
    {
      if (strcmp((dictionary->block[blockIdx])->type_name(),"anywave") == 0)
        {
          fprintf( stdout, "---- Printing the 10 first samples of the first channel of the first waveform of the anywave table of the anywave block %i:\n", blockIdx);
          for ( sampleIdx = 0;
                (sampleIdx < (((MP_Anywave_Block_c*)(dictionary->block[blockIdx]))->anywaveTable)->filterLen) && (sampleIdx < 10);
                sampleIdx ++)
            {
              fprintf( stdout, "%lf ", *((((MP_Anywave_Block_c*)(dictionary->block[blockIdx]))->anywaveTable)->wave[0][0]+sampleIdx));
            }
        }
    }


  delete(dictionary);
  fprintf( stdout, "\n---- Dictionary deleted \n");
  fflush( stdout );

  fprintf( stdout, "\n-- Exiting MP_Dict_c::test \n" );
  fflush( stdout );
*/
  return(true);
}

/******************************/
/* Return the number of atoms */
unsigned long int MP_Dict_c::num_atoms(void)
{

  unsigned long int numAtoms = 0;
  unsigned int i;

  for (i = 0; i < numBlocks; i++)
    numAtoms += block[i]->num_atoms();

  return(numAtoms);
}


/*****************************************/
/* Copy a new signal into the dictionary */
int MP_Dict_c::copy_signal( MP_Signal_c *setSignal )
{

  const char* func = "MP_Dict_c::copy_signal( signal )";
  unsigned int i;
  int check = 0;

  if ( signal )
    {
      delete( signal );
      signal = NULL;
    }
  if ( setSignal != NULL )
    {
      signal = new MP_Signal_c( *setSignal );
      sigMode = MP_DICT_INTERNAL_SIGNAL;
    }
  else
    {
      signal = NULL;
      sigMode = MP_DICT_NULL_SIGNAL;
    }

  /* Allocate the touch array
     (alloc_touch() will automatically manage the NULL signal case) */
  if ( alloc_touch() )
    {
      mp_error_msg( func, "Failed to allocate and initialize the touch array"
                    " in the dictionary constructor. Signal and touch will stay NULL.\n" );
      delete( signal );
      signal = NULL;
      sigMode = MP_DICT_NULL_SIGNAL;
      return( 1 );
    }

  /* Then plug the signal into all the blocks */
  for ( i = 0; i < numBlocks; i++ )
    {
      if ( block[i]->plug_signal( signal ) )
        {
          mp_error_msg( func, "Failed to plug the given signal in the %u-th block."
                        " Proceeding with the remaining blocks.\n", i );
          check = 1;
        }
    }

  return( check );
}

/*****************************************/
/* Load a new signal into the dictionary,
   from a file */
int MP_Dict_c::copy_signal( const char *fName )
{

  const char* func = "MP_Dict_c::copy_signal( fileName )";
  unsigned int i;
  int check = 0;

  if ( fName == NULL )
    {
      mp_error_msg( func, "Passed a NULL string for the file name.\n" );
      return( 1 );
    }

  if ( signal )
    {
      delete( signal );
      signal = NULL;
    }
  signal = MP_Signal_c::init( fName );
  if ( signal == NULL )
    {
      mp_error_msg( func, "Failed to instantiate a signal from file [%s]\n",
                    fName );
      sigMode = MP_DICT_NULL_SIGNAL;
      alloc_touch(); /* -> Nullifies the touch array when signal is NULL. */
      return( 1 );
    }
  /* else  */
  sigMode = MP_DICT_INTERNAL_SIGNAL;

  /* Allocate the touch array */
  if ( alloc_touch() )
    {
      mp_error_msg( func, "Failed to allocate and initialize the touch array"
                    " in the dictionary constructor. Signal and touch will stay NULL.\n" );
      delete( signal );
      signal = NULL;
      sigMode = MP_DICT_NULL_SIGNAL;
      return( 1 );
    }

  /* Then plug the signal into all the blocks */
  for ( i = 0; i < numBlocks; i++ )
    {
      if ( block[i]->plug_signal( signal ) )
        {
          mp_error_msg( func, "Failed to plug the given signal in the %u-th block."
                        " Proceeding with the remaining blocks.\n", i );
          check = 1;
        }
    }

  return( check );
}


/*****************************************/
/* Copy a new signal into the dictionary */
int MP_Dict_c::plug_signal( MP_Signal_c *setSignal )
{

  const char* func = "MP_Dict_c::plug_signal( signal )";
  int check = 0;
  unsigned int i;

  if ( setSignal != NULL )
    {
      signal = setSignal;
      sigMode = MP_DICT_EXTERNAL_SIGNAL;
    }
  else
    {
      signal = NULL;
      sigMode = MP_DICT_NULL_SIGNAL;
      mp_warning_msg( func, "The signal to plug is NULL.\n" );
    }

  /* Allocate the touch array
     (alloc_touch() will automatically manage the NULL signal case) */
  if ( alloc_touch() )
    {
      mp_error_msg( func, "Failed to allocate and initialize the touch array"
                    " in the dictionary constructor. Signal and touch will stay NULL.\n" );
      signal = NULL;
      sigMode = MP_DICT_NULL_SIGNAL;
      return( 1 );
    }

  /* Then plug the signal into all the blocks */
  for ( i = 0; i < numBlocks; i++ )
    {
      if ( block[i]->plug_signal( signal ) )
        {
          mp_error_msg( func, "Failed to plug the given signal in the %u-th block."
                        " Proceeding with the remaining blocks.\n", i );
          check = 1;
        }
    }

  /* If we are in multithread mode, then instantiate all the data for the threads */
#ifdef MULTITHREAD
  bar =new MP_Barrier_c* [2];
  val = new volatile MP_Real_t[numBlocks];
  bar[0]=new MP_Barrier_c(numBlocks+1);
  bar[1]=new MP_Barrier_c(numBlocks+1);
  threads = new pthread_t[numBlocks];
  tasks = new ParallelConstruct[numBlocks];
  pthread_attr_t attr;
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_JOINABLE);
  for ( i = 0; i < numBlocks; i++ )
    {
      tasks[i].blocknumber = i ;
      tasks[i].val = val + i ;
      tasks[i].parent = this ;
      tasks[i].exit = false ;
      if (pthread_create(&threads[i], NULL, &ParallelConstruct::c_run, &tasks[i]))
        {
          mp_error_msg( func, "Failed to create Threads\n" );
        }
    }
#else

#endif
  return( check );

}


/*****************************************/
/* Detach the signal from the dictionary */
MP_Signal_c* MP_Dict_c::detach_signal( void )
{

  MP_Signal_c *s = signal;

  plug_signal( NULL );

  return( s );
}


/*********************************/
/* Allocation of the touch array */
int MP_Dict_c::alloc_touch( void )
{

  const char* func = "MP_Dict_c::alloc_touch()";

  /* If touch already exists, free it; guarantee that it is NULL. */
  if ( touch )
    {
      free( touch );
      touch = NULL;
    }

  /* Check if a signal is present: */
  if ( signal != NULL )
    {

      /* Allocate the touch array */
      if ( (touch = (MP_Support_t*) malloc( signal->numChans*sizeof(MP_Support_t) )) == NULL )
        {
          mp_error_msg( func, "Failed to allocate the array of touched supports"
                        " in the dictionary. The touch array is left NULL.\n" );
          return( 1 );
        }
      /* If the allocation went OK, initialize the array:*/
      else
        {
          int i;
          /* Initially we have to consider the whole signal as touched */
          for ( i = 0; i < signal->numChans; i++ )
            {
              touch[i].pos = 0;
              touch[i].len = signal->numSamples;
            }
          return( 0 );
        }

    }
  else
    {
      mp_error_msg( func, "Trying to allocate the touch array from"
                    " a NULL signal. The touch array will remain NULL.\n" );
      return( 1 );
    }

}


/*************************/
/* Delete all the blocks */
int MP_Dict_c::delete_all_blocks( void )
{

  unsigned int i;
  unsigned int oldNumBlocks = numBlocks;
  for ( i=0; i<numBlocks; i++)
    {
      if ( block[i] ) delete( block[i] );
    }
  if ( block ) free( block );
  block = NULL;
  numBlocks = 0;
  blockWithMaxIP = 0;

  return( oldNumBlocks );
}


void MP_Dict_c::calcul_max_per_block(ParallelConstruct* f)
{
  while (1)
    {
      /* wait for main to signal start computing: */
      bar[0]->wait();
      if (f->exit)
        {
          pthread_exit(0) ;
        }
      else
        {
          /* each threads computes this function: */
          f->val[0] = block[f->blocknumber]->update_max(block[f->blocknumber]->update_ip(touch));
          /* each threads signal main that its computation is over: */
          bar[1]->wait();
        }
    }
}

#ifdef MULTITHREAD
/************************************/
/* Update of all the inner products which need to be updated, according to the 'touch' field */
MP_Real_t MP_Dict_c::update( void )
{

  unsigned int k;
  MP_Real_t tempMax = -1.0;
  /* Loach the parallel computation: */
  parallel_computing(val);

  for ( k=0; k<numBlocks; k++)
    {
      /* Recompute and locate in the threads output tab the max inner product within a block */
      if ( val[k] > tempMax )
        {
          tempMax = val[k];
          blockWithMaxIP = k;
        }
      /* Locate the max inner product across the blocks */
    }

  return ( tempMax );
}

MP_Real_t MP_Dict_c::update( GP_Book_c* touchBook )
{

  unsigned int k;
  MP_Real_t tempMax = -1.0;
  /* Loach the parallel computation: */
  parallel_computing(val);

  for ( k=0; k<numBlocks; k++)
    {
      /* Recompute and locate in the threads output tab the max inner product within a block */
      if ( val[k] > tempMax )
        {
          tempMax = val[k];
          blockWithMaxIP = k;
        }
      /* Locate the max inner product across the blocks */
    }

  return ( tempMax );
}

#else
/************************************/
/* Update of all the inner products which need to be updated, according to the 'touch' field */
MP_Real_t MP_Dict_c::update( void )
{

  unsigned int i;
  MP_Real_t tempMax = -1.0;
  MP_Real_t val;
  MP_Support_t frameSupport;

  for ( i=0; i<numBlocks; i++)
    {
      /* Recompute the inner products */
      frameSupport = block[i]->update_ip( touch );
      /* Recompute and locate the max inner product within a block */
      val = block[i]->update_max( frameSupport );
      /* Locate the max inner product across the blocks */
      if ( val > tempMax )
        {
          tempMax = val;
          blockWithMaxIP = i;
        }
    }

  return ( tempMax );
}

MP_Real_t MP_Dict_c::update( GP_Block_Book_c* touchBook )
{

  unsigned int i;
  MP_Real_t tempMax = -1.0;
  MP_Real_t val;
  MP_Support_t frameSupport;

  for ( i=0; i<numBlocks; i++)
    {
      /* Recompute the inner products */
      frameSupport = block[i]->update_ip( touch, touchBook->get_block_book(i) );
      /* Recompute and locate the max inner product within a block */
      val = block[i]->update_max( frameSupport );
      /* Locate the max inner product across the blocks */
      if ( val > tempMax )
        {
          tempMax = val;
          blockWithMaxIP = i;
        }
    }

  return ( tempMax );
}

#endif

void MP_Dict_c::parallel_computing(volatile MP_Real_t* val)
{
  /* Loach the the parallel computation for each threads: */
  bar[0]->wait();
  /* Wait the end of computation of each threads: */
  bar[1]->wait();
}



/**********************************************/
/* Forces an update of all the inner products */
MP_Real_t MP_Dict_c::update_all( void )
{

  unsigned int i;
  MP_Real_t tempMax = -1.0;
  MP_Real_t val;
  MP_Support_t frameSupport;

  for ( i=0; i<numBlocks; i++)
    {
      /* (Re)compute all the inner products */
      frameSupport = block[i]->update_ip( NULL );
      /* Refresh and locate the max inner product within a block */
      val = block[i]->update_max( frameSupport );
      /* Locate the max inner product across the blocks */
      if ( val > tempMax )
        {
          tempMax = val;
          blockWithMaxIP = i;
        }
    }
  return( tempMax );
}

/************************************/
/* Create a new atom corresponding to the best atom of the best block. */
unsigned int MP_Dict_c::create_max_atom( MP_Atom_c** atom )
{

  const char* func = "MP_Dict_c::create_max_atom(**atom)";
  unsigned long int frameIdx;
  unsigned long int filterIdx;
  unsigned int numAtoms;

  /* 1) select the best atom of the best block */
  frameIdx =  block[blockWithMaxIP]->maxIPFrameIdx;
  filterIdx = block[blockWithMaxIP]->maxIPIdxInFrame[frameIdx];

  mp_debug_msg( MP_DEBUG_MP_ITERATIONS, func,
                "Found max atom in block [%lu], at frame [%lu], with filterIdx [%lu].\n",
                blockWithMaxIP, frameIdx, filterIdx );

  /* 2) create it using the method of the block */
  numAtoms = block[blockWithMaxIP]->create_atom( atom, frameIdx, filterIdx );
  if ( numAtoms == 0 )
    {
      mp_error_msg( func, "Failed to create the max atom from block[%ul].\n",
                    blockWithMaxIP );
      return( 0 );
    }

  (*atom)->blockIdx=blockWithMaxIP;
  (*atom)->dict = this;

  return( numAtoms );
}

unsigned int MP_Dict_c::create_max_gp_atom( MP_Atom_c** atom )
{
    const char* func = "MP_Dict_c::create_max_atom(**atom)";
  unsigned long int frameIdx;
  unsigned long int filterIdx;
  unsigned int numAtoms;

  /* 1) select the best atom of the best block */
  frameIdx =  block[blockWithMaxIP]->maxIPFrameIdx;
  filterIdx = block[blockWithMaxIP]->maxIPIdxInFrame[frameIdx];

  mp_debug_msg( MP_DEBUG_MP_ITERATIONS, func,
                "Found max atom in block [%lu], at frame [%lu], with filterIdx [%lu].\n",
                blockWithMaxIP, frameIdx, filterIdx );

  /* 2) create it using the method of the block */
  numAtoms = block[blockWithMaxIP]->create_atom( atom, frameIdx, filterIdx );
  if ( numAtoms == 0 )
    {
      mp_error_msg( func, "Failed to create the max atom from block[%ul].\n",
                    blockWithMaxIP );
      return( 0 );
    }

  for (MP_Chan_t c=0;c<(*atom)->numChans;c++){
	  (*atom)->corr[c] = (*atom)->amp[c];
	  (*atom)->amp[c] = 0;
  }
  (*atom)->blockIdx=blockWithMaxIP;

  return( numAtoms );
}

/******************************************/
/* Perform one matching pursuit iteration */
int MP_Dict_c::iterate_mp( MP_Book_c *book , MP_Signal_c *sigRecons )
{

  const char* func = "MP_Dict_c::iterate_mp(...)";
  int chanIdx;
  MP_Atom_c *atom;
  unsigned int numAtoms;

  /* Check if a signal is present */
  if ( signal == NULL )
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
  update();

  /** 2/ Create the max atom and store it in the book */
  numAtoms = create_max_atom( &atom );
  if ( numAtoms == 0 )
    {
      mp_error_msg( func, "The Matching Pursuit iteration failed. Dictionary, book"
                    " and signal are left unchanged.\n" );
      return( 1 );
    }

  if ( book->append( atom ) != 1 )
    {
      mp_error_msg( func, "Failed to append the max atom to the book.\n" );
      return( 1 );
    }

  /* 3/ Substract the atom's waveform from the analyzed signal */
  atom->substract_add( signal , sigRecons );

  /* 4/ Keep track of the support where the signal has been modified */
  for ( chanIdx=0; chanIdx < atom->numChans; chanIdx++ )
    {
      touch[chanIdx].pos = atom->support[chanIdx].pos;
      touch[chanIdx].len = atom->support[chanIdx].len;
    }

  return( 0 );
}

/******************************************/
/* Perform one cyclic matching pursuit iteration */
int MP_Dict_c::iterate_cmp( MP_Book_c *book , MP_Signal_c *sigRecons, int atomIndex )
{
	const char		*func = "MP_Dict_c::iterate_cmp(...)";
	int 			chanIdx;
	MP_Atom_c 		*atom;
	unsigned int 	numAtoms;
	
	/* Check if a signal is present */
	if ( signal == NULL )
	{
		mp_error_msg( func, "There is no signal in the dictionary. You must plug or copy a signal before you can iterate.\n" );
		return( 1 );
	}
	
	// 1/ refresh the inner products
	// 2/ create the max atom and store it
	// 3/ substract it from the signal and add it to the approximant
	
	//----------- Part 1 -----------
	// (Re)compute the inner products according to the current 'touch' indicating where the signal
	// may have been changed after the last update of the inner products */
	update();
	
	//----------- Part 2 -----------
	// Create the max atom and store it in the book */
	numAtoms = create_max_atom( &atom );
	
	if ( numAtoms == 0 )
	{
		mp_error_msg( func, "The Matching Pursuit iteration failed. Dictionary, book and signal are left unchanged.\n" );
		return( 1 );
	}
	
	if ( book->replace( atom, atomIndex ) != 1 )
	{
		mp_error_msg( func, "Failed to replace the max atom to the book.\n" );
		return( 1 );
	}
	
	//----------- Part 3 -----------
	// Substract the atom's waveform from the analyzed signal */
	atom->substract_add( signal , sigRecons );
	
	//----------- Part 4 -----------
	// Keep track of the support where the signal has been modified */
	for ( chanIdx=0; chanIdx < atom->numChans; chanIdx++ )
	{
		touch[chanIdx].pos = atom->support[chanIdx].pos;
		touch[chanIdx].len = atom->support[chanIdx].len;
	}

	return( 0 );
}
