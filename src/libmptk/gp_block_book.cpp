#include "mptk.h"

//void GP_Block_Book_Iterator_c::print_book(){
//	cout << "(*iter)->book = " << book << endl;
//}

GP_Block_Book_c* GP_Block_Book_c::create(){
  return new GP_Block_Book_c();
}

GP_Block_Book_c* GP_Block_Book_c::create(unsigned int numBlocks){
  return new GP_Block_Book_c(numBlocks);
}

/* NULL constructor
 */
GP_Block_Book_c::GP_Block_Book_c(){
  begIter = GP_Block_Book_Iterator_c(this);
  endIter = GP_Block_Book_Iterator_c(this);
}

GP_Block_Book_c::GP_Block_Book_c(unsigned int numBlocks):
  blockBookVec(numBlocks, GP_Pos_Book_c(0)){
  begIter = GP_Block_Book_Iterator_c(this);
  endIter = GP_Block_Book_Iterator_c(this);
  iterator iter = blockBookVec::begin();
  for (unsigned int i = 0; i<numBlocks; i++)
    iter[i].blockIdx = i;
  endIter.blockIter = blockBookVec::end();
}


/* Destructor
 */
GP_Block_Book_c::~GP_Block_Book_c(){
  reset();
}

/* check if an atom is present
 */
bool GP_Block_Book_c::contains(unsigned int blockIdx,
		unsigned long int pos,
		MP_Atom_Param_c& param){
  GP_Pos_Book_c* subBook = get_block_book(blockIdx);
  if (!subBook)
    return false;
  return subBook->contains(pos, param);
}
  
bool GP_Block_Book_c::contains(const MP_Atom_c& atom){
  GP_Pos_Book_c* subBook = get_block_book(atom.blockIdx);
  if (!subBook)
    return false;
  return subBook->contains(atom.get_pos(), *(atom.get_atom_param()));
}

/* get an atom if present, NULL otherwise
 */
MP_Atom_c* GP_Block_Book_c::get_atom(unsigned int blockIdx,
		unsigned long int pos,
	    MP_Atom_Param_c& param){
  GP_Pos_Book_c* subBook = get_block_book(blockIdx);
  if (!subBook)
    return NULL;
  return subBook->get_atom(pos, param);
}

MP_Atom_c* GP_Block_Book_c::get_atom(const MP_Atom_c& atom){
  GP_Pos_Book_c* subBook = get_block_book(atom.blockIdx);
  if (!subBook)
    return NULL;
  return subBook->get_atom(atom.get_pos(), *(atom.get_atom_param()));
}


/** \brief Clear all the atoms from the book.
 */
void GP_Block_Book_c::reset( void ){
  blockBookVec::iterator end(blockBookVec::end());
  for (iterator i = blockBookVec::begin(); i != end; ++i)
    i->reset();
  clear();
}

/** \brief Add a new atom in the storage space, taking care of the necessary allocations 
 * \param newAtom a reference to an atom
 * \return the number of appended atoms (1 upon success, zero otherwise)
 * \remark The reference newAtom is not copied, it is stored and will be deleted when the book is destroyed
 * \remark \a numChans is set up if this is the first atom to be appended to the book,
 * otherwise, if the atom \a numChans does not match \a numChans, it is not appended.
 */
   
int GP_Block_Book_c::append( MP_Atom_c *newAtom ){
  return insert_block_book(newAtom->blockIdx)->append(newAtom);
}

/* get an index for the sub-book containing only atoms generated
 * by a given block.
 */
GP_Pos_Book_c* GP_Block_Book_c::get_block_book(unsigned int blockIdx){
  return &(blockBookVec::begin()[blockIdx]);
}
   
/* get an index for the sub-book containing only atoms 
 * at a given position
 */
GP_Pos_Range_Sub_Book_c* GP_Block_Book_c::get_pos_book(unsigned long int pos){
  return get_range_book(pos, pos+1);
}

GP_Pos_Book_c* GP_Block_Book_c::insert_block_book(unsigned int blockIdx){
  return &(blockBookVec::begin()[blockIdx]);
}
   
/* get an index for the sub-book containing only atoms 
 * at a given position
 */
GP_Pos_Range_Sub_Book_c* GP_Block_Book_c::insert_pos_book(unsigned long int pos){
  return get_range_book(pos, pos+1);
}
   
/* get an index for the sub-book containing only atoms 
 * between 2 given positions, included. This leaves 
 * the neighbourhood selection strategy to the upper level.
 */
GP_Pos_Range_Sub_Book_c* GP_Block_Book_c::get_range_book(unsigned long int minPos,
		unsigned long int maxPos){
  return new GP_Pos_Range_Sub_Book_c(this, minPos, maxPos);
}

GP_Block_Book_Iterator_c& GP_Block_Book_c::begin(void){
  begIter = GP_Block_Book_Iterator_c(this);
  return begIter;
}

GP_Block_Book_Iterator_c& GP_Block_Book_c::end(void){
  endIter.blockIter = blockBookVec::end();
  return endIter;
}

bool GP_Block_Book_c::empty(){
  blockBookVec::iterator end(blockBookVec::end());
  //cout <<"block book:: empty\n";
  for(blockBookVec::iterator iter = blockBookVec::begin(); iter != end; iter++){
    //cout << "1\n";
    if(!iter->empty()){
    //  cout <<"leaving\n";
      return false;
    }
  }
  //cout <<"leaving\n";
  return true;
}

GP_Block_Book_Iterator_c::GP_Block_Book_Iterator_c(GP_Block_Book_c* book):
  book(book),
  blockIter(book->blockBookVec::begin()){
  blockBookVec::iterator end = book->blockBookVec::end();
  while(blockIter != end && blockIter->empty())
    blockIter++;
  if(blockIter != end){
    posIter = blockIter->begin();
  }
}

GP_Block_Book_Iterator_c::GP_Block_Book_Iterator_c(const GP_Block_Book_Iterator_c& iter):
    book(iter.book),
    blockIter(iter.blockIter),
    posIter(iter.posIter){
}
    

GP_Block_Book_Iterator_c::~GP_Block_Book_Iterator_c(void){
}
  
GP_Block_Book_Iterator_c& GP_Block_Book_Iterator_c::operator ++(void){
  GP_Pos_Book_Iterator_c end;
//  if (blockIter == book->blockBookVec::end())
//    cout << "ENDENDENDNENDNENDNENDNENDNENDNENDNENDNENDNENDNENDNEND\n";
  end = blockIter->end();
  ++posIter;
  if(posIter == end){
//	  cout << "posIter == end" << endl;
    ++blockIter;
    while(blockIter != book->blockBookVec::end() && blockIter->empty())
      ++blockIter;
    if(blockIter != book->blockBookVec::end()){
    	//cout << "found an atom" << endl;
      posIter = blockIter->begin();
    }
  }
  return *this;
}

GP_Block_Book_Iterator_c& GP_Block_Book_Iterator_c::go_to_pos(unsigned long int pos){
  blockBookVec::iterator end = book->blockBookVec::end();
  
  if(blockIter != end && posIter->get_pos() < pos){
    posIter.go_to_pos(pos);
    while(blockIter != end && 
      posIter == blockIter->end()){
      go_to_next_block();
      if (blockIter == end)
        return *this;
      GP_Block_Book_Iterator_c::go_to_pos(pos);
    }
  }
  return *this;
}

GP_Block_Book_Iterator_c& GP_Block_Book_Iterator_c::go_to_next_block(void){
  do ++blockIter;
  while (blockIter != book->blockBookVec::end() && blockIter->empty());
  if (blockIter != book->blockBookVec::end()){
    posIter = blockIter->begin();
  }
  return *this;
}

GP_Block_Book_Iterator_c& GP_Block_Book_Iterator_c::go_to_next_frame(void){
    posIter.go_to_next_frame();
    while(posIter == blockIter->end()){ //next frame not found
        ++blockIter;
        if (blockIter == book->blockBookVec::end()){// reached the end
            break;
        }
            
        posIter = blockIter->begin();
    }
    return *this;
}

bool GP_Block_Book_Iterator_c::operator == (const GP_Book_Iterator_c& arg)const{
	//cout << "GP_Block_Book_c::==" << endl;
    if (typeid(*this) != typeid(arg)){
    	//cout << "different iterator types" << endl;
        return false;
    }
  if (book != (dynamic_cast<const GP_Block_Book_Iterator_c&>(arg)).book){
	 // cout << "different books" << endl;
	  return false;
  }
  if (blockIter == book->blockBookVec::end()){
	  //cout << "this at end" << endl;
	  if ((dynamic_cast<const GP_Block_Book_Iterator_c&>(arg)).blockIter == book->blockBookVec::end()){
		//  cout << "both at end" << endl;
		  return true;
	  }
	  else{
		  //cout << "arg not at end" << endl;
		  return false;
	  }

  }
  return (blockIter == (dynamic_cast<const GP_Block_Book_Iterator_c&>(arg)).blockIter && posIter == (dynamic_cast<const GP_Block_Book_Iterator_c&>(arg)).posIter);
}
 
GP_Block_Book_Iterator_c* GP_Block_Book_Iterator_c::copy(void)const{
    return new GP_Block_Book_Iterator_c(*this);
}

MP_Atom_c& GP_Block_Book_Iterator_c::operator *(void){
  return *(posIter.paramIter.paramIter->second);
}

MP_Atom_c* GP_Block_Book_Iterator_c::operator ->(void){
  return posIter.paramIter.paramIter->second;
}

GP_Param_Book_c* GP_Block_Book_Iterator_c::get_frame(void){
    return posIter.get_frame();
}
