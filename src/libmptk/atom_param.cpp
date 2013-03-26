#include "mptk.h"

bool MP_Atom_Param_c::less::operator() (const MP_Atom_Param_c* atom1, const MP_Atom_Param_c* atom2)const{
  //cerr << "less" << endl; 
    return *atom1 < *atom2;
}

atom_map MP_Atom_Param_c::get_map()const{
	return atom_map();
}

vector<string> MP_Atom_Param_c::get_param_names()const{
	return vector<string>();
}

bool MP_Atom_Param_c::operator<(const MP_Atom_Param_c& param) const{
	return false;
}

bool MP_Atom_Param_c::operator>(const MP_Atom_Param_c& param)const{
	return false;
}

bool MP_Atom_Param_c::operator==(const MP_Atom_Param_c& param)const{
	return typeid(!this)==typeid(param);
}

bool MP_Atom_Param_c::operator!=(const MP_Atom_Param_c& param)const{
	return typeid(!this)!=typeid(param);
}

