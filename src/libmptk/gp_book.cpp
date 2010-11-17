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

unsigned long int GP_Book_c::build_waveform_amp(MP_Dict_c* dict, MP_Real_t* outBuffer, MP_Real_t* tmpBuffer){
	unsigned long int wavBeg = ULONG_MAX, wavEnd = 0, wavSize;
	unsigned long int blockBeg, blockSize;
	unsigned long int base = begin()->support[0].pos, wavOffset, blockOffset;
	unsigned int i;
	GP_Book_Iterator_c* iter;
	GP_Book_c* subBook;

	// compute the dimensions of the waveform
	for (iter = begin().copy(); *iter != end(); iter->go_to_next_frame()){
		if ((*iter)->support[0].pos < wavBeg)
			wavBeg = (*iter)->support[0].pos;
		if ((*iter)->support[0].pos + (*iter)->support[0].len > wavEnd)
			wavEnd = (*iter)->support[0].pos + (*iter)->support[0].len;
	}
	wavSize = wavEnd - wavBeg;
	delete(iter);
	memset(outBuffer, 0, dict->signal->numChans*wavSize*sizeof(MP_Real_t));

	for (i = 0; i < dict->numBlocks; i++){
		subBook = get_sub_book(i);
		blockBeg = subBook->begin()->support[0].pos;
		blockSize = dict->block[i]->build_subbook_waveform_amp(subBook, tmpBuffer);
		for (MP_Chan_t c = 0; c < dict->signal->numChans; c++){
			wavOffset = c*wavSize+blockBeg-base;
			blockOffset = c*blockSize;
			for (unsigned long int t = 0; t < blockSize; t++)
				outBuffer[t+wavOffset] += tmpBuffer[t+blockOffset];
		}
	}
	return wavSize;
}

unsigned long int GP_Book_c::build_waveform_corr(MP_Dict_c* dict, MP_Real_t* outBuffer, MP_Real_t* tmpBuffer){
	unsigned long int wavBeg = ULONG_MAX, wavEnd = 0, wavSize;
	unsigned long int blockBeg, blockSize;
	unsigned long int base, wavOffset, blockOffset;
	unsigned int i;
	GP_Book_Iterator_c* iter;
	GP_Book_c* subBook;

	// compute the dimensions of the waveform
	base = begin()->support[0].pos;

	for (iter = begin().copy(); *iter != end(); iter->go_to_next_frame()){
		if ((*iter)->support[0].pos < wavBeg)
			wavBeg = (*iter)->support[0].pos;
		if ((*iter)->support[0].pos + (*iter)->support[0].len > wavEnd)
			wavEnd = (*iter)->support[0].pos + (*iter)->support[0].len;
	}
	wavSize = wavEnd - wavBeg;
	delete(iter);
	memset(outBuffer, 0, dict->signal->numChans*wavSize*sizeof(MP_Real_t));

	for (i = 0; i < dict->numBlocks; i++){
		subBook = get_sub_book(i);
		if (!subBook->is_empty()){
			blockBeg = subBook->begin()->support[0].pos;
			blockSize = dict->block[i]->build_subbook_waveform_corr(subBook, tmpBuffer);

			for (MP_Chan_t c = 0; c < dict->signal->numChans; c++){
				wavOffset = c*wavSize+blockBeg-base;
				blockOffset = c*blockSize;
				for (unsigned long int t = 0; t < blockSize; t++)
					outBuffer[t+wavOffset] += tmpBuffer[t+blockOffset];
			}
		}
	}
	return wavSize;
}

