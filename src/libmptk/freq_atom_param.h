/*
 * freq_atom_param.h: implementation of atom_param with only 1 filterIdx parameter. To use with MDCT and anywave atoms.
 *
 *  Created on: 5 juil. 2010
 *      Author: rleboulc
 */

#ifndef FREQ_ATOM_PARAM_H_
#define FREQ_ATOM_PARAM_H_

#include "mptk.h"

class MP_Freq_Atom_Param_c: public MP_Atom_Param_c{
public:
	MP_Real_t freq;
	MPTK_LIB_EXPORT MP_Freq_Atom_Param_c(MP_Real_t);
	MPTK_LIB_EXPORT ~MP_Freq_Atom_Param_c(){}

	MPTK_LIB_EXPORT virtual atom_map get_map()const;
	MPTK_LIB_EXPORT virtual vector<string> get_param_names()const;

	MPTK_LIB_EXPORT virtual bool operator<(const MP_Atom_Param_c&)const;
    MPTK_LIB_EXPORT virtual bool operator>(const MP_Atom_Param_c&)const;
    MPTK_LIB_EXPORT virtual bool operator==(const MP_Atom_Param_c&)const;
    MPTK_LIB_EXPORT virtual bool operator!=(const MP_Atom_Param_c&)const;
};

MPTK_LIB_EXPORT void swap(MP_Freq_Atom_Param_c&, MP_Freq_Atom_Param_c&);

#endif /* FREQ_ATOM_PARAM_H_ */
