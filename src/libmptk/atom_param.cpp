#include "mptk.h"

atom_map MP_Atom_Param_c::get_map(){
	mp_warning_msg("MP_Atom_Param_c::get_map","This method has not been implemented for the child class");
	return atom_map();
}

vector<string> MP_Atom_Param_c::get_param_names(){
	mp_warning_msg("MP_Atom_Param_c::get_param_names","This method has not been implemented for the child class");
	return vector<string>();
}

bool MP_Atom_Param_c::operator<(const MP_Atom_Param_c& arg)const{
  mp_warning_msg("MP_Atom_Param_c::<","Comparison between different types of atoms\n");
  cerr << typeid(*this).name() << endl;
  cerr << typeid(arg).name() << endl;
  return false;
}

bool MP_Atom_Param_c::operator>(const MP_Atom_Param_c& arg)const{
  mp_warning_msg("MP_Atom_Param_c::>","Comparison between different types of atoms\n");
  cerr << typeid(*this).name() << endl;
  cerr << typeid(arg).name() << endl;
  return false;
}

bool MP_Atom_Param_c::operator==(const MP_Atom_Param_c& arg)const{
  mp_warning_msg("MP_Atom_Param_c::==","Comparison between different types of atoms\n");
  cerr << typeid(*this).name() << endl;
  cerr << typeid(arg).name() << endl;
  return false;
}

bool MP_Atom_Param_c::operator!=(const MP_Atom_Param_c& arg)const{
  mp_warning_msg("MP_Atom_Param_c::!=","Comparison between different types of atoms\n");
  cerr << typeid(*this).name() << endl;
  cerr << typeid(arg).name() << endl;
  return false;
}

bool MP_Atom_Param_c::less::operator() (const MP_Atom_Param_c* atom1, const MP_Atom_Param_c* atom2)const{
  //cerr << "less" << endl; 
    return *atom1 < *atom2;
}
