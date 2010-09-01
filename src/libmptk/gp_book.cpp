#include "mptk.h"

GP_Book_c::~GP_Book_c(){
}

bool GP_Book_c::contains(const MP_Atom_c& atom){
  return contains(atom.blockIdx, atom.get_pos(), *(atom.get_atom_param()));
}

MP_Atom_c* GP_Book_c::get_atom(const MP_Atom_c& atom){
  return get_atom(atom.blockIdx, atom.get_pos(), *(atom.get_atom_param()));
}

bool GP_Book_Iterator_c::operator !=(const GP_Book_Iterator_c& arg)const{
 return !(*this == arg);
}
