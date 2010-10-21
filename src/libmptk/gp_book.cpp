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

void GP_Book_c::build_waveform_amp(MP_Dict_c* dict, MP_Real_t* outBuffer, MP_Real_t* tmpBuffer){
    unsigned long int offset = LONG_MAX, t, numSamples, beg;
    unsigned int i;
    GP_Book_Iterator_c* iter;
    GP_Book_c* subBook;
    
    // compute the offset
    for (iter = begin().copy(); *iter != end(); iter->go_to_next_frame())
        if ((*iter)->support[0].pos < offset)
        	offset = (*iter)->support[0].pos;
    
    delete(iter);
        
    for (i = 0; i < dict->numBlocks; i++){
        subBook = get_sub_book(i);
        beg = subBook->begin()->support[0].pos;
        numSamples = dict->block[i]->build_subbook_waveform_amp(subBook, tmpBuffer);
        for (t = 0; t < numSamples; t++)
            outBuffer[t+beg-offset] += tmpBuffer[t];           
    }
}
   
void GP_Book_c::build_waveform_corr(MP_Dict_c* dict, MP_Real_t* outBuffer, MP_Real_t* tmpBuffer){
	unsigned long int offset = LONG_MAX, t, numSamples, beg;
	unsigned int i;
	GP_Book_Iterator_c* iter;
	GP_Book_c* subBook;

	// compute the offset
	for (iter = begin().copy(); *iter != end(); iter->go_to_next_frame())
		if ((*iter)->support[0].pos < offset)
			offset = (*iter)->support[0].pos;

	delete(iter);

	for (i = 0; i < dict->numBlocks; i++){
		subBook = get_sub_book(i);
		beg = subBook->begin()->support[0].pos;
		numSamples = dict->block[i]->build_subbook_waveform_corr(subBook, tmpBuffer);
		for (t = 0; t < numSamples; t++)
			outBuffer[t+beg-offset] += tmpBuffer[t];
	}
}
