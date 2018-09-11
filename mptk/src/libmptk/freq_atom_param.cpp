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

MP_Index_Atom_Param_c::MP_Index_Atom_Param_c(unsigned long int filterIdx){
	this->filterIdx=filterIdx;
}

atom_map MP_Index_Atom_Param_c::get_map()const{
	atom_map res;
	char idxString[100];
	sprintf(idxString, "%lu", filterIdx);
	res[string("filterIdx")]=string(idxString);
	return res;
}

vector<string> MP_Index_Atom_Param_c::get_param_names()const{
	vector<string>res;
	res[0]=string("filterIdx");
	return res;
}

bool MP_Index_Atom_Param_c::operator<(const MP_Atom_Param_c& atom)const{
	//cerr << "MP_Freq_Atom_Param_c::<" << endl;
    if (typeid(*this) != typeid(atom))
        return false;
    //cerr << "freq1 = " << freq << '\t' << "freq2 = " << (dynamic_cast<MP_Freq_Atom_Param_c&>(atom)).freq << endl;
    return filterIdx < (dynamic_cast<const MP_Index_Atom_Param_c&>(atom)).filterIdx;
}

bool MP_Index_Atom_Param_c::operator>(const MP_Atom_Param_c& atom)const{
	if (typeid(*this) != typeid(atom))
        return false;
    return filterIdx > (dynamic_cast<const MP_Index_Atom_Param_c&>(atom)).filterIdx;
}

bool MP_Index_Atom_Param_c::operator==(const MP_Atom_Param_c& atom)const{
	if (typeid(*this) != typeid(atom))
        return false;
    return filterIdx == (dynamic_cast<const MP_Index_Atom_Param_c&>(atom)).filterIdx;
}

bool MP_Index_Atom_Param_c::operator!=(const MP_Atom_Param_c& atom)const{
	if (typeid(*this) != typeid(atom))
        return false;
    return filterIdx != (dynamic_cast<const MP_Index_Atom_Param_c&>(atom)).filterIdx;
}

void swap(MP_Index_Atom_Param_c& atom1, MP_Index_Atom_Param_c& atom2){
	unsigned long int tmp;
	tmp = atom1.filterIdx;
	atom1.filterIdx = atom2.filterIdx;
	atom2.filterIdx = tmp;
}
