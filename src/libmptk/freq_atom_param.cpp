/*
 * freq_atom_param.cpp
 *
 *  Created on: 5 juil. 2010
 *      Author: rleboulc
 */

#include "freq_atom_param.h"

MP_Freq_Atom_Param_c::MP_Freq_Atom_Param_c(MP_Real_t freq){
	this->freq=freq;
}

atom_map MP_Freq_Atom_Param_c::get_map()const{
	atom_map res;
	char idxString[100];
	sprintf(idxString, "%f", freq);
	res[string("freq")]=string(idxString);
	return res;
}

vector<string> MP_Freq_Atom_Param_c::get_param_names()const{
	vector<string>res;
	res[0]=string("freq");
	return res;
}

bool MP_Freq_Atom_Param_c::operator<(const MP_Atom_Param_c& atom)const{
  //cerr << "MP_Freq_Atom_Param_c::<" << endl;
    if (typeid(*this) != typeid(atom))
        return false;
    //cerr << "freq1 = " << freq << '\t' << "freq2 = " << (dynamic_cast<MP_Freq_Atom_Param_c&>(atom)).freq << endl;
    return freq < (dynamic_cast<const MP_Freq_Atom_Param_c&>(atom)).freq;
}

bool MP_Freq_Atom_Param_c::operator>(const MP_Atom_Param_c& atom)const{
	if (typeid(*this) != typeid(atom))
        return false;
    return freq > (dynamic_cast<const MP_Freq_Atom_Param_c&>(atom)).freq;
}

bool MP_Freq_Atom_Param_c::operator==(const MP_Atom_Param_c& atom)const{
	if (typeid(*this) != typeid(atom))
        return false;
    return freq == (dynamic_cast<const MP_Freq_Atom_Param_c&>(atom)).freq;
}

bool MP_Freq_Atom_Param_c::operator!=(const MP_Atom_Param_c& atom)const{
	if (typeid(*this) != typeid(atom))
        return false;
    return freq != (dynamic_cast<const MP_Freq_Atom_Param_c&>(atom)).freq;
}

void swap(MP_Freq_Atom_Param_c& atom1, MP_Freq_Atom_Param_c& atom2){
	MP_Real_t tmp;
	tmp = atom1.freq;
	atom1.freq = atom2.freq;
	atom2.freq = tmp;
}
