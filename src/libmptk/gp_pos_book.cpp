#include "mptk.h"

/* empty book constructor
 */
GP_Pos_Book_c::GP_Pos_Book_c(unsigned int blockIdx):
  blockIdx(blockIdx){
  begIter = GP_Pos_Book_Iterator_c(this);
  endIter = GP_Pos_Book_Iterator_c(this);
}

GP_Pos_Book_c::GP_Pos_Book_c(const GP_Pos_Book_c& book):
  blockIdx(book.blockIdx){
  begIter = GP_Pos_Book_Iterator_c(this);
  endIter = GP_Pos_Book_Iterator_c(this);
  endIter.posIter = posBookMap::end();
}

GP_Pos_Book_c::~GP_Pos_Book_c(){
  reset();
}

/* check if an atom is present
 */
bool GP_Pos_Book_c::contains(unsigned int blockIdx,
		unsigned long int pos,
		MP_Atom_Param_c& param){
  if (blockIdx != this->blockIdx)
    return false;
  return get_sub_book(pos)->contains(param);
}

bool GP_Pos_Book_c::contains(unsigned long int pos,
			       MP_Atom_Param_c& param){
  return get_sub_book(pos)->contains(param);
}

bool GP_Pos_Book_c::contains(const MP_Atom_c& atom){
  if (blockIdx != atom.blockIdx)
    return false;
  return get_sub_book(atom.get_pos())->contains(*(atom.get_atom_param()));
}

/* get an atom if present, NULL otherwise
 */
MP_Atom_c* GP_Pos_Book_c::get_atom(unsigned int blockIdx,
		unsigned long int pos,
		MP_Atom_Param_c& param){
 if (blockIdx != this->blockIdx)
    return NULL;
 return get_sub_book(pos)->get_atom(param);
}

MP_Atom_c* GP_Pos_Book_c::get_atom(unsigned long int pos,
				     MP_Atom_Param_c& param){
  return get_sub_book(pos)->get_atom(param);
}

MP_Atom_c* GP_Pos_Book_c::get_atom(const MP_Atom_c& atom){
  if (blockIdx != atom.blockIdx)
    return NULL;
  return get_sub_book(atom.get_pos())->get_atom(*(atom.get_atom_param()));
}

/** \brief Clear all the atoms from the book.
 */
void GP_Pos_Book_c::reset( void ){
  GP_Param_Book_c* sub;
  for (iterator iter=posBookMap::begin(); iter!=posBookMap::end(); ++iter){
    sub = &(iter->second);
    sub->reset();
  }
  clear();
}

/** \brief Add a new atom in the storage space, taking care of the necessary allocations 
 * \param newAtom a reference to an atom
 * \return the number of appended atoms (1 upon success, zero otherwise)
 * \remark The reference newAtom is not copied, it is stored and will be deleted when the book is destroyed
 * \remark \a numChans is set up if this is the first atom to be appended to the book,
 * otherwise, if the atom \a numChans does not match \a numChans, it is not appended.
 */
int GP_Pos_Book_c::append( MP_Atom_c *newAtom ){
  pair<iterator,bool> sub;
  if (newAtom->blockIdx != blockIdx)
    return false;
  sub = insert(value_type(newAtom->get_pos(), GP_Param_Book_c(blockIdx, newAtom->get_pos())));
  return sub.first->second.append(newAtom);
}


/* get an index for the sub-book containing only atoms generated
 * by a given block.
 */
GP_Pos_Book_c* GP_Pos_Book_c::get_sub_book(unsigned int blockIdx){
  if (blockIdx != this->blockIdx)
    return NULL;
  return this;
}

/* get the sub-book containing only atoms 
 * at a given position. All changes to the sub-book will be backed to the 
 * original one.
 */
GP_Param_Book_c* GP_Pos_Book_c::get_sub_book(unsigned long int pos){
  iterator sub = find(pos);
  if (sub == posBookMap::end())
    return NULL;
  return &(sub->second);
}

/* get an index for the sub-book containing only atoms 
 * between 2 given positions, included. This leaves 
 * the neighbourhood selection strategy to the upper level.
 */
GP_Pos_Range_Sub_Book_c* GP_Pos_Book_c::get_sub_book(unsigned long int minPos,
		unsigned long int maxPos){
  return new GP_Pos_Range_Sub_Book_c(this, minPos, maxPos);
}

GP_Pos_Book_c* GP_Pos_Book_c::insert_sub_book(unsigned int blockIdx){
  if (blockIdx != this->blockIdx)
    return NULL;
  return this;
}

GP_Param_Book_c* GP_Pos_Book_c::insert_sub_book(unsigned long int pos){
  pair<iterator,bool> sub = insert(value_type(pos, GP_Param_Book_c(blockIdx, pos)));
  return &(sub.first->second);
}

GP_Pos_Book_Iterator_c& GP_Pos_Book_c::begin(void){
  begIter = GP_Pos_Book_Iterator_c(this);
  return begIter;
}

GP_Pos_Book_Iterator_c& GP_Pos_Book_c::end(void){
  endIter.posIter = posBookMap::end();
  return endIter;
}

bool GP_Pos_Book_c::empty(){
  posBookMap::iterator iter, end = posBookMap::end();
  for(iter = posBookMap::begin(); iter != end; iter++)
    if(!iter->second.empty())
      return false;
  return true;
}

GP_Pos_Book_c& GP_Pos_Book_c::operator =(const GP_Pos_Book_c& book){

  posBookMap(*this) = posBookMap(book);
  begIter = GP_Pos_Book_Iterator_c(this);
  endIter = GP_Pos_Book_Iterator_c(this);
  endIter.posIter = posBookMap::end();
  return *this;
}

void swap (GP_Pos_Book_c& book1, GP_Pos_Book_c& book2){
  unsigned int tmpIdx;
  GP_Pos_Book_Iterator_c tmpIter;

  swap((posBookMap&) book1, (posBookMap&) book2);

  tmpIdx = book1.blockIdx;
  book1.blockIdx = book2.blockIdx;
  book2.blockIdx = tmpIdx;

  tmpIter = book1.begIter;
  book1.begIter = book2.begIter;
  book2.begIter = tmpIter;

  tmpIter = book1.endIter;
  book1.endIter = book2.endIter;
  book2.endIter = tmpIter;
}

// Iterator methods

GP_Pos_Book_Iterator_c::GP_Pos_Book_Iterator_c(void){
  book = NULL;
}

GP_Pos_Book_Iterator_c::GP_Pos_Book_Iterator_c(const GP_Pos_Book_Iterator_c& iter):
    book(iter.book),
    posIter(iter.posIter),
    paramIter(iter.paramIter){
}

GP_Pos_Book_Iterator_c::GP_Pos_Book_Iterator_c(GP_Pos_Book_c* book):
  book(book),
  posIter(book->posBookMap::begin()){
  posBookMap::iterator end(book->posBookMap::end());
  while (posIter != end && posIter->second.empty())
    posIter++;
  if (posIter != book->posBookMap::end())
    paramIter = posIter->second.begin();
}

GP_Pos_Book_Iterator_c::GP_Pos_Book_Iterator_c(GP_Pos_Book_c* book,
                           const posBookMap::iterator& iter):
  book(book),
  posIter(iter){
  posBookMap::iterator end(book->posBookMap::end());
  while (posIter != end && posIter->second.empty())
    posIter++;
  if (posIter != book->posBookMap::end())
    paramIter = posIter->second.begin();
}

GP_Pos_Book_Iterator_c::~GP_Pos_Book_Iterator_c(void){
}
  
GP_Pos_Book_Iterator_c& GP_Pos_Book_Iterator_c::operator ++(void){
  posBookMap::iterator end(book->posBookMap::end());

  if(posIter == end)
    return *this;
  ++paramIter;
  if (paramIter == posIter->second.end()){
    posIter++;
    while (posIter != end && posIter->second.empty())
      posIter++;
    if(posIter != book->posBookMap::end())
      paramIter = posIter->second.begin();
  }
  return *this;
}

GP_Pos_Book_Iterator_c& GP_Pos_Book_Iterator_c::go_to_pos(unsigned long int pos){
  posBookMap::iterator end = book->posBookMap::end();
  
  if (posIter != end && posIter->second.pos < pos){
    // get to the right position
    posIter = book->posBookMap::lower_bound(pos);
    // get to the first non empty sub-book
    while (posIter != end && posIter->second.empty())
      posIter++;
    // get to the first atom of that book
    if(posIter != end)
      paramIter = posIter->second.begin();
  }
  return *this;
}

GP_Pos_Book_Iterator_c& GP_Pos_Book_Iterator_c::go_to_next_block(void){
  posIter = book->posBookMap::end();
  return *this;
}

bool GP_Pos_Book_Iterator_c::operator == (const GP_Book_Iterator_c& arg)const{
    if (typeid(*this) != typeid(arg))
        return false;
  return (book == (dynamic_cast < const GP_Pos_Book_Iterator_c&>(arg)).book) && ((posIter == book->posBookMap::end() && (dynamic_cast < const GP_Pos_Book_Iterator_c&>(arg)).posIter == book->posBookMap::end()) || ((posIter == (dynamic_cast<const GP_Pos_Book_Iterator_c&>(arg)).posIter) && (paramIter == (dynamic_cast<const GP_Pos_Book_Iterator_c&>(arg)).paramIter)));
}

GP_Pos_Book_Iterator_c* GP_Pos_Book_Iterator_c::copy(void)const{
    return new GP_Pos_Book_Iterator_c(*this);
}

MP_Atom_c& GP_Pos_Book_Iterator_c::operator *(void){
  return *(paramIter.paramIter->second);
}

MP_Atom_c* GP_Pos_Book_Iterator_c::operator ->(void){
  return paramIter.paramIter->second;
}
