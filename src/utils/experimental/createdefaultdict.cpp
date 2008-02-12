/***************************************************************/
/* This program lists all the possible blocks and field values */
/* in a MPTK dictionnary - using the MP_Block_Factory_c */

#include <mptk.h>
#include <mptk_env.h>
#include <block_factory.h>
#include <vector>
#include <iostream>

//void printParamMap(map<string, string, mp_ltstring> * paramMap );

char* func = "create";
char* pn = "";  /* program name */

/* Usage help */
void usage() {
  fprintf (stdout,"%s -- Creates a default dictionnary using all known blocks in mptk\n",pn);
  fprintf (stdout,"  usage: %s -- Creates the default dictionnary in the current folder\n",pn);
  fprintf (stdout,"         %s dictName.xml -- Creates the default dictionnary in file dictName.xml\n\n",pn);
  return;
}

/* Print block parameters information */
void printParamMap(map<string, string, mp_ltstring> * paramMap )
{
  map<string, string, mp_ltstring>::iterator paramIterator;
  /*Iterate on varparam list to list all parameters map for blocks*/
  cout << "--" << endl;
  for (paramIterator = paramMap->begin(); paramIterator != paramMap->end(); ++paramIterator )
    {
      cout << "Key: " << paramIterator->first << ", value: " << paramIterator->second
      << endl;
    }
  cout << "--" << endl;
}

/* Main function */
int main(int argc, char **argv)
{

  char * dictName;
  dictName = (char*) malloc (512+1);
  pn = argv[0]; /* set program name in global var */
  // Parse command line
  if (argc==1) 
    {
      strcpy(dictName,"MPdefaultDictionnary.xml");
    }
  else
    {
      if ( (0==strcmp(argv[1],"-h")) || (0==strcmp(argv[1],"-help")) || (0==strcmp(argv[1],"--help")) ) 
	{
	  usage();
	  return(0);
	}
      else
	{
	  dictName = argv[1];
	}
    }
  
  // Charge l'environnement MPTK
  if (!MPTK_Env_c::get_env()->get_environment_loaded())
    MPTK_Env_c::get_env()->load_environment("");
  
  // Affiche la liste des Blocks
  mp_info_msg( func,"The following Block types have been successfully loaded:\n");
  vector< string >* nameVector = new vector< string >();
  MP_Block_Factory_c::get_block_factory()->get_registered_block_name( nameVector );
  for (unsigned int i= 0; i < nameVector->size(); i++)
    {
      cout << "block " << i << ":" << nameVector->at(i) << endl;
    }
  

  // Define a default stereo signal
  MP_Signal_c * signal = NULL;
  signal = MP_Signal_c::init(2,48000,48000);
  if (NULL == signal)
    {
      cout << " !!! Cannot init signal" << endl;
    }
  else
    {
      cout << "Signal initialized" << endl;
    }
  
  // Create a default dict with default blocks ...
  MP_Dict_c * myDict = NULL;
  myDict = MP_Dict_c::init();
  
  // Create a default bloc for each block type and display info about it
  for (unsigned int i= 0; i < nameVector->size(); i++)
    {
      if (strcmp(nameVector->at(i).c_str(),"anywavehilbert") and strcmp(nameVector->at(i).c_str(),"anywave")){
	cout << "Try to create a default block for type: [" << nameVector->at(i) << "] ... ";
	myDict->add_default_block(nameVector->at(i).c_str());
	cout << " OK" << endl;
	
	
	MP_Block_c *newBlock = NULL;
	map<string, string, mp_ltstring>* defaultMap = new map<string, string, mp_ltstring>();
	MP_Block_Factory_c::get_block_factory()->get_block_default_map(nameVector->at(i).c_str())(defaultMap);
	
	/*call the block creator*/
	MP_Block_c* (*blockCreator)( MP_Signal_c *setSignal, map<string, string, mp_ltstring> * paramMap ) = NULL;
	blockCreator = MP_Block_Factory_c::get_block_factory()->get_block_creator(nameVector->at(i).c_str());
	
	if (NULL == blockCreator)
	  {
	    cout << "[" << nameVector->at(i) << "]: this block type is not registred in the atom factory" << endl;
	    if (defaultMap) delete(defaultMap);
	    return 0;
	  }
	else
	  {
	    cout << "[" << nameVector->at(i) << "]: succesfully affected default parameter by the atom factory" << endl;
	  }
	
	printParamMap(defaultMap);
	
	/*Create a new block*/
	newBlock =  blockCreator(signal, defaultMap);
	cout << "Here" << endl;
	
	/*Test if new block is NULL*/
	if (NULL == newBlock)
	  {
	    cout << "Failed to create the block of type " << nameVector->at(i) << endl;
	    if (defaultMap) delete(defaultMap);
	    return 0;
	  }
	else
	  {
	    // Print Block info
	    cout << " Block of type [" << nameVector->at(i) << "] successfully created" << endl;
	  }
	// Delete loop objects
	if (defaultMap) delete(defaultMap);
	if (newBlock) delete(newBlock);
      }
    }

  myDict->print(dictName);
  delete(nameVector);
  // Erase dictionnary
  myDict->delete_all_blocks();
  delete(myDict);

  return 0;
}
