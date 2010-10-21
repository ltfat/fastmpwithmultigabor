#include "mptk.h"

GP_Pos_Range_Sub_Book_c::GP_Pos_Range_Sub_Book_c(){
	begIter = GP_Pos_Range_Sub_Book_Iterator_c(this);
	endIter = GP_Pos_Range_Sub_Book_Iterator_c(this);
}

GP_Pos_Range_Sub_Book_c::GP_Pos_Range_Sub_Book_c(GP_Book_c* book,
		unsigned long int minPos,
		unsigned long int maxPos):
  book(book),
  minPos(minPos),
  maxPos(maxPos){
  begIter = GP_Pos_Range_Sub_Book_Iterator_c(this);
  endIter = GP_Pos_Range_Sub_Book_Iterator_c(this);
}

bool GP_Pos_Range_Sub_Book_c::contains(unsigned int blockIdx,
		unsigned long int pos,
	    MP_Atom_Param_c& param){
  if (pos < minPos || pos >= maxPos)
    return false;
  return book->contains(blockIdx, pos, param);
}

bool GP_Pos_Range_Sub_Book_c::contains(const MP_Atom_c& atom){
  if (atom.get_pos() < minPos || atom.get_pos() >= maxPos)
    return false;
  return book->contains(atom);
}

MP_Atom_c* GP_Pos_Range_Sub_Book_c::get_atom(unsigned int blockIdx,
		unsigned long int pos,
		MP_Atom_Param_c& param){
  if (pos < minPos || pos >= maxPos)
    return NULL;
  return book->get_atom(blockIdx, pos, param);
}

MP_Atom_c* GP_Pos_Range_Sub_Book_c::get_atom(const MP_Atom_c& atom){
  if (atom.get_pos() < minPos || atom.get_pos() >= maxPos)
    return NULL;
  return book->get_atom(atom);
}

int GP_Pos_Range_Sub_Book_c::append(MP_Atom_c* newAtom){
  if (newAtom->get_pos() < minPos || newAtom->get_pos() >= maxPos)
    return 0;
  return book->append(newAtom);
}

GP_Pos_Range_Sub_Book_c* GP_Pos_Range_Sub_Book_c::get_sub_book(unsigned int blockIdx){
  GP_Book_c* subBook = book->get_sub_book(blockIdx);
  if (!subBook)
    return NULL;
  if (subBook == book)
    return this;
  return new GP_Pos_Range_Sub_Book_c(subBook, minPos, maxPos);
}

GP_Book_c* GP_Pos_Range_Sub_Book_c::get_sub_book(unsigned long int pos){
  if (pos<minPos)
    return NULL;
  if (pos>maxPos)
    return NULL;
  return book->get_sub_book(pos);
}

GP_Pos_Range_Sub_Book_c* GP_Pos_Range_Sub_Book_c::insert_sub_book(unsigned int blockIdx){
  GP_Book_c* subBook = book->get_sub_book(blockIdx);
  if (!subBook)
    return NULL;
  if (subBook == book)
    return new GP_Pos_Range_Sub_Book_c(*this);
  return new GP_Pos_Range_Sub_Book_c(subBook, minPos, maxPos);
}

GP_Book_c* GP_Pos_Range_Sub_Book_c::insert_sub_book(unsigned long int pos){
  if (pos<minPos)
    return NULL;
  if (pos>maxPos)
    return NULL;
  return book->insert_sub_book(pos);
}

GP_Pos_Range_Sub_Book_c* GP_Pos_Range_Sub_Book_c::get_sub_book(unsigned long int minPos,
		unsigned long int maxPos){
  return new GP_Pos_Range_Sub_Book_c(book,
				     max(minPos, this->minPos),
				     min(maxPos, this->maxPos));
}

GP_Pos_Range_Sub_Book_Iterator_c& GP_Pos_Range_Sub_Book_c::begin(){
    if (begIter.iter){
      delete begIter.iter;
      begIter.iter = NULL;
    }
    begIter = GP_Pos_Range_Sub_Book_Iterator_c(this);
    return begIter;
}

GP_Pos_Range_Sub_Book_Iterator_c& GP_Pos_Range_Sub_Book_c::end(){
    if (endIter.iter)
      delete endIter.iter;
    endIter.iter = book->end().copy();
    return endIter;
}

// Iterator methods

GP_Pos_Range_Sub_Book_Iterator_c::GP_Pos_Range_Sub_Book_Iterator_c(GP_Pos_Range_Sub_Book_c* book):
  book(book),
  iter(NULL){
  if (book && book->book)
    iter = book->book->begin().copy();
  go_to_pos(book->minPos);
}

GP_Pos_Range_Sub_Book_Iterator_c::GP_Pos_Range_Sub_Book_Iterator_c(GP_Pos_Range_Sub_Book_c* book,
                                                                   const GP_Book_Iterator_c& iter):
  book(book){
  this->iter = iter.copy();
}

GP_Pos_Range_Sub_Book_Iterator_c::GP_Pos_Range_Sub_Book_Iterator_c(const GP_Pos_Range_Sub_Book_Iterator_c& arg):
  book(arg.book){
  iter = arg.iter->copy();
}

GP_Pos_Range_Sub_Book_Iterator_c::~GP_Pos_Range_Sub_Book_Iterator_c(void){
  delete iter;
}
  
GP_Pos_Range_Sub_Book_Iterator_c& GP_Pos_Range_Sub_Book_Iterator_c::operator ++(void){
  ++(*iter);
  //iter->print_book();
  //cout << "book->book = " << book->book << endl;
  if((*iter)!=book->book->end()){
	//  cout << "&atom == " << &**iter << endl;
    if ((*iter)->get_pos() >= book->maxPos){
    	//cout << "go_to_next_block" << endl;
      go_to_next_block();}
    else if ((*iter)->get_pos() < book->minPos){
    	//cout << "go_to_pos" << endl;
      go_to_pos(book->minPos);}
  }
  return *this;
}

GP_Pos_Range_Sub_Book_Iterator_c& 
GP_Pos_Range_Sub_Book_Iterator_c::go_to_pos(unsigned long int pos){
  GP_Book_Iterator_c& end = book->book->end();
  unsigned long int truePos = (pos > book->minPos ? pos : book->minPos);
  iter->go_to_pos(truePos);
  while ((*iter) != end && (*iter)->get_pos() >= book->maxPos){
    iter->go_to_next_block();
    if ((*iter) == end)
      return *this;
    iter->go_to_pos(truePos);
  }
  return *this;
}

GP_Pos_Range_Sub_Book_Iterator_c& GP_Pos_Range_Sub_Book_Iterator_c::go_to_next_block(void){
  do{
    iter->go_to_next_block();
    iter->go_to_pos(book->minPos);
  }
  while((*iter) != book->book->end() && (*iter)->get_pos() >= book->maxPos);
  return *this;
}

GP_Pos_Range_Sub_Book_Iterator_c& GP_Pos_Range_Sub_Book_Iterator_c::go_to_next_frame(void){
  do{
    iter->go_to_next_frame();
    iter->go_to_pos(book->minPos);
  }
  while((*iter) != book->book->end() && (*iter)->get_pos() >= book->maxPos);
  return *this;
}

MP_Atom_c& GP_Pos_Range_Sub_Book_Iterator_c::operator *(void){
  return *(*iter);
}

MP_Atom_c* GP_Pos_Range_Sub_Book_Iterator_c::operator ->(void){
  return &**iter;
}

GP_Param_Book_c* GP_Pos_Range_Sub_Book_Iterator_c::get_frame(void){
    return iter->get_frame();
}

bool GP_Pos_Range_Sub_Book_Iterator_c::operator ==
(const GP_Book_Iterator_c& arg)const{
    if (typeid(*this) != typeid(arg))
        return false;
  return book == (dynamic_cast<const GP_Pos_Range_Sub_Book_Iterator_c&>(arg)).book && *iter == *((dynamic_cast<const GP_Pos_Range_Sub_Book_Iterator_c&>(arg)).iter);
}

GP_Pos_Range_Sub_Book_Iterator_c* GP_Pos_Range_Sub_Book_Iterator_c::copy(void)const{
    return new GP_Pos_Range_Sub_Book_Iterator_c(*this);
}

GP_Pos_Range_Sub_Book_Iterator_c& GP_Pos_Range_Sub_Book_Iterator_c::operator =(const GP_Pos_Range_Sub_Book_Iterator_c& arg){
    if (this == &arg)
      return *this;
    if (iter)
      delete iter;
    book = arg.book;
    iter = arg.iter->copy();
    return *this;
}
    
