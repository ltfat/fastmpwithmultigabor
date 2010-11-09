#include "mptk.h"

GP_Param_Book_c::GP_Param_Book_c(unsigned int blockIdx,
		unsigned long int pos):
  blockIdx(blockIdx),
  pos(pos){
  begIter = GP_Param_Book_Iterator_c(this);
  endIter = GP_Param_Book_Iterator_c(this);
}

GP_Param_Book_c::GP_Param_Book_c(const GP_Param_Book_c& book):
  paramBookMap(book),
  blockIdx(book.blockIdx),
  pos(book.pos){
  begIter = GP_Param_Book_Iterator_c(this);
  endIter = GP_Param_Book_Iterator_c(this);
    endIter.paramIter = paramBookMap::end();
}
  
GP_Param_Book_c::~GP_Param_Book_c(){
  reset();
}

bool GP_Param_Book_c::contains(MP_Atom_Param_c& param){
  return find(&param) != paramBookMap::end();
}

bool GP_Param_Book_c::contains(unsigned int blockIdx,
		unsigned long int pos,
		MP_Atom_Param_c& param){
  if (blockIdx != this->blockIdx)
    return false;
  if (pos != this->pos)
    return false;
  return find(&param) != paramBookMap::end();
}

bool GP_Param_Book_c::contains(const MP_Atom_c& atom){
  MP_Atom_Param_c* param;
  bool res;
  if (blockIdx != atom.blockIdx)
    return false;
  if (pos != atom.get_pos())
    return false; 
  param = atom.get_atom_param();
  res = (find(atom.get_atom_param()) != paramBookMap::end());
  delete param;
  return res;
}

MP_Atom_c* GP_Param_Book_c::get_atom(MP_Atom_Param_c& param){
  iterator iter = find(&param);
  if (iter == paramBookMap::end())
    return NULL;
  return iter->second;
}

MP_Atom_c* GP_Param_Book_c::get_atom(unsigned int blockIdx ,
		unsigned long int pos,
		MP_Atom_Param_c& param){
  iterator iter;
  if (blockIdx != this->blockIdx)
    return NULL;
  if (pos != this->pos)
    return NULL;
  iter = find(&param);
  if (iter == paramBookMap::end())
    return NULL;
  return iter->second;
}

MP_Atom_c* GP_Param_Book_c::get_atom(const MP_Atom_c& atom){
  MP_Atom_Param_c* param;
  iterator iter;
  if (blockIdx != atom.blockIdx)
    return NULL;
  if (pos != atom.get_pos())
    return NULL;
  param = atom.get_atom_param();
  iter = find(param);
  delete param;
  if (iter == paramBookMap::end())
    return NULL;
  return iter->second;
}

int GP_Param_Book_c::append(MP_Atom_c* atom){
  pair<iterator, bool> res;
  iterator previous;
  MP_Atom_Param_c* param;
  
  if (!atom){
    cerr << "GP_Param_Book_c::append NULL atom" << endl;
    return false;
  }
  if (blockIdx != atom->blockIdx)
    return 0;
  if (pos != atom->get_pos())
    return 0;

  param = atom->get_atom_param();
  if (!param){
    cerr << "GP_Param_Book_c::append NULL param" << endl;
    return 0;
  }

  previous = find(param);
  if (previous!=paramBookMap::end()){
    previous->second->info(stderr);
    delete param;
    return 1;
  }
  res = insert(pair<MP_Atom_Param_c*, MP_Atom_c*>(param, atom));
  return res.second;
}

GP_Param_Book_c* GP_Param_Book_c::get_sub_book(unsigned int blockIdx){
  if (blockIdx != this->blockIdx)
    return NULL;
  return this;
}

GP_Param_Book_c* GP_Param_Book_c::get_sub_book(unsigned long int pos){
  if (pos != this->pos)
    return NULL;
  return this;
}

GP_Param_Book_c* GP_Param_Book_c::insert_sub_book(unsigned int blockIdx){
  if (blockIdx != this->blockIdx)
    return NULL;
  return this;
}

GP_Param_Book_c* GP_Param_Book_c::insert_sub_book(unsigned long int pos){
  if (pos != this->pos)
    return NULL;
  return this;
}

GP_Param_Book_c* GP_Param_Book_c::get_sub_book(unsigned long int minPos,
		unsigned long int maxPos){
  if (pos > maxPos)
    return NULL;
  if (pos < minPos)
    return NULL;
  return this;
}

void GP_Param_Book_c::reset(void){
	paramBookMap::iterator end(paramBookMap::end());
	for(paramBookMap::iterator iter(paramBookMap::begin());iter!=end;iter++)
		if (iter->second)
			delete(iter->second);
	clear();
}

GP_Param_Book_Iterator_c& GP_Param_Book_c::begin(void){
  begIter = GP_Param_Book_Iterator_c(this);
  return begIter;
}

GP_Param_Book_Iterator_c& GP_Param_Book_c::end(void){
  endIter.paramIter = paramBookMap::end();
  return endIter;
}

GP_Param_Book_c& GP_Param_Book_c::operator = (const GP_Param_Book_c& book){
  paramBookMap(*this) = paramBookMap(book);
  begIter = GP_Param_Book_Iterator_c(this);
  endIter = GP_Param_Book_Iterator_c(this);
  endIter.paramIter = paramBookMap::end();
  return *this;
}

/*void GP_Param_Book_c::substract_add_grad(MP_Dict_c* dict, MP_Real_t step,
                                         MP_Signal_c* sigSub, MP_Signal_c* sigAdd){
    dict->block[blockIdx]->substract_add_grad(this, step, sigSub, sigAdd);
}*/

void GP_Param_Book_c::build_waveform_amp(MP_Dict_c* dict, MP_Real_t* outBuffer){
    dict->block[blockIdx]->build_frame_waveform_amp(this, outBuffer);
}

void GP_Param_Book_c::build_waveform_corr(MP_Dict_c* dict, MP_Real_t* outBuffer){
    dict->block[blockIdx]->build_frame_waveform_corr(this, outBuffer);
}

bool GP_Param_Book_c::is_empty(){
	paramBookMap::iterator iter;
	for (iter = paramBookMap::begin(); iter != paramBookMap::end(); ++iter)
		if (iter->second)
			return false;
	return true;
}

void swap(GP_Param_Book_c& book1, GP_Param_Book_c& book2){
  GP_Param_Book_Iterator_c tmpIter;
  unsigned long int tmpPos(book1.pos);
  unsigned int tmpIdx;

  swap((paramBookMap&) book1, (paramBookMap&) book2);

  tmpIter = book1.begIter;
  book1.begIter = book2.begIter;
  book2.begIter = tmpIter;

  tmpIter = book1.endIter;
  book1.endIter = book2.endIter;
  book2.endIter = tmpIter;

  book1.pos = book2.pos;
  book2.pos = tmpPos;

  tmpIdx = book1.blockIdx;
  book1.blockIdx = book2.blockIdx;
  book2.blockIdx = tmpIdx;
}

// Iterator methods

GP_Param_Book_Iterator_c::GP_Param_Book_Iterator_c():
  paramIter(){
  book = NULL;
}

GP_Param_Book_Iterator_c::GP_Param_Book_Iterator_c(GP_Param_Book_c* book):
  book(book),
  paramIter(book->paramBookMap::begin()){
}

GP_Param_Book_Iterator_c::GP_Param_Book_Iterator_c(GP_Param_Book_c* book,
                           const paramBookMap::iterator& paramIter):
  book(book),
  paramIter(paramIter){
}

GP_Param_Book_Iterator_c::~GP_Param_Book_Iterator_c(void){
}

GP_Param_Book_Iterator_c& GP_Param_Book_Iterator_c::operator ++(void){
  ++paramIter;
  return *this;
}

GP_Param_Book_Iterator_c& GP_Param_Book_Iterator_c::go_to_next_block(void){
  paramIter = book->paramBookMap::end();
  return *this;
}

GP_Param_Book_Iterator_c& GP_Param_Book_Iterator_c::go_to_pos(unsigned long int pos){
  if (book->pos < pos)
    paramIter = book->paramBookMap::end();
  return *this;
}

GP_Param_Book_Iterator_c& GP_Param_Book_Iterator_c::go_to_next_frame(){
    paramIter = book->paramBookMap::end();
    return *this;
}

MP_Atom_c& GP_Param_Book_Iterator_c::operator *(void){
  return *(paramIter->second);
}

MP_Atom_c* GP_Param_Book_Iterator_c::operator ->(void){
  return paramIter->second;
}

GP_Param_Book_c* GP_Param_Book_Iterator_c:: get_frame(void){
    return book;
}

bool GP_Param_Book_Iterator_c::operator == (const GP_Book_Iterator_c& arg)const{
    if (typeid(*this) != typeid(arg))
        return false;
  return paramIter == (dynamic_cast<const GP_Param_Book_Iterator_c&>(arg)).paramIter;
}

GP_Param_Book_Iterator_c* GP_Param_Book_Iterator_c::copy(void)const{
    return new GP_Param_Book_Iterator_c(*this);
}
