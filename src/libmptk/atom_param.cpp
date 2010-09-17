#include "mptk.h"

bool MP_Atom_Param_c::less::operator() (const MP_Atom_Param_c* atom1, const MP_Atom_Param_c* atom2)const{
  //cerr << "less" << endl; 
    return *atom1 < *atom2;
}
