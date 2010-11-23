#ifndef __gp_book_h_
#define __gp_book_h_

class MP_Dict_c;
class GP_Param_Book_c;

/* Interface that has to be implemented by all the iterators operating on GP books and sub-books.
 */
class GP_Book_Iterator_c{
 public:
  MPTK_LIB_EXPORT virtual ~GP_Book_Iterator_c(void){}
  
  /* \brief create a new copy of the iterator
   * \return a pointer to the new copy
   */
  MPTK_LIB_EXPORT virtual GP_Book_Iterator_c* copy (void)const=0;
  
  /* \brief make the iterator point to the next atom
   * \return iterator value after incrementation
   */
  MPTK_LIB_EXPORT virtual GP_Book_Iterator_c& operator ++(void)=0;
  
  /* \brief make the iterator point to the first atom after the current one (included) with a position greater or equal than the parameter.
   * \param pos: target position
   * \return the iterator after incrementation
   */
  MPTK_LIB_EXPORT virtual GP_Book_Iterator_c& go_to_pos(unsigned long int)=0;
  
  /* \brief make the iterator point to the first atom after the current one with a strictly higher block index.
   * \return the iterator after incrementation
   */
  MPTK_LIB_EXPORT virtual GP_Book_Iterator_c& go_to_next_block(void)=0;
  
  /* \brief got to the next atom that belongs to a different GP_Param_Book_c than the current one
   * \return the iterator after incrementation
   */
  MPTK_LIB_EXPORT virtual GP_Book_Iterator_c& go_to_next_frame()=0;

  /* \brief Get the pointed atom
   * \return the pointed atom
   */
  MPTK_LIB_EXPORT virtual MP_Atom_c& operator *(void)=0;
  
  /* \brief Access operator to the members of the pointed atom
   * \return the address of the pointed atom
   * \remark despite its signature, this operator is binary: it requires the name of the member after the arrow.
   * If you want to use the address of the pointed atom in an expression, use &(*iter) instead.
   */
  MPTK_LIB_EXPORT virtual MP_Atom_c* operator ->(void)=0;
  
  /* \brief Get the GP_Param_Book_c containing the current atom
   * \return a pointer to the book
   */
  MPTK_LIB_EXPORT virtual GP_Param_Book_c* get_frame(void)=0;

  /* \brief Equality operator
   * \param the other iterator to compare to
   * \return true if both iterators point to the same atom of the same book or are both at the end of the same book, false otherwise.
   */
  MPTK_LIB_EXPORT virtual bool operator == (const GP_Book_Iterator_c&)const=0;
  
  /* \brief Inequality operator
   * \param the other iterator to compare to
   * \return false if both iterators point to the same atom of the same book or are both at the end of the same book,
   * true otherwise.
   */
  MPTK_LIB_EXPORT bool operator != (const GP_Book_Iterator_c&)const;  
};

/* interface that has to be implemented by every kind of GP book or sub-book
 */
class GP_Book_c{

 public:

  MPTK_LIB_EXPORT virtual ~GP_Book_c();

  /* \brief check if an atom  belongs to the book
   * \param blockIdx: block of the atom
   * \param pos: position of the atom
   * \param param: atom parameters
   * \return true if the atom is in the book, false otherwise
   */
  MPTK_LIB_EXPORT virtual bool contains(unsigned int blockIdx,
			unsigned long int pos,
			MP_Atom_Param_c& param) =0;
            
  /* \brief check if an atom  belongs to the book
   * \param atom: the atom
   * \return true if an atom with the same block, position and parameters as the argument one is in the book, false otherwise
   */     
  MPTK_LIB_EXPORT bool contains(const MP_Atom_c&);

  /* \brief retrieve an atom
   * \param blockIdx: block of the atom
   * \param pos: position of the atom
   * \param param: atom parameters
   * \return a pointer to the atom if present, NULL otherwise
   */
  MPTK_LIB_EXPORT virtual MP_Atom_c* get_atom(unsigned int blockIdx,
		  unsigned long int pos,
	      MP_Atom_Param_c& param) =0;
          
  /* \brief retrieve an atom
   * \param atom: the atom
   * \return a pointer to an atom with the same block, position and parameters as the argument if there is one, NULL otherwise
   */ 
  MPTK_LIB_EXPORT MP_Atom_c* get_atom(const MP_Atom_c&);

  /** \brief Clear all the atoms from the book.
   */
  //virtual void reset(void) =0;

  /** \brief Add a new atom in the storage space, taking care of the necessary allocations 
   * \param newAtom a reference to an atom
   * \return the number of appended atoms (1 upon success, zero otherwise)
   * \remark The reference newAtom is not copied, 
   *  it is stored and will be deleted when the book is destroyed
   * \remark \a numChans is set up if this is the first atom to be appended to the book,
   * otherwise, if the atom \a numChans does not match \a numChans, it is not appended.
   */
  MPTK_LIB_EXPORT virtual int append(MP_Atom_c* newAtom) =0;

  /* \brief get an index for the sub-book containing only atoms generated
   * by a given block
   * \param blockIdx: target block
   * \return a pointer to the sub_book if it exists, NULL otherwise
   */
  MPTK_LIB_EXPORT virtual GP_Book_c* get_block_book(unsigned int blockIdx)=0;

  /* \brief get an index for the sub-book containing only atoms at a given position
   * \param pos: target position
   * \return a pointer to the sub_book if it exists, NULL otherwise
   */
  MPTK_LIB_EXPORT virtual GP_Book_c* get_pos_book(unsigned long int pos)=0;
  
  /* \brief get an index for the sub-book containing only atoms between two positions
   * \param minPos: lower bound (included)
   * \param maxPos: upper bound (excluded)
   * \return a pointer to the sub_book if it exists, NULL otherwise
   */
  MPTK_LIB_EXPORT virtual GP_Book_c* get_range_book(unsigned long int minPos,
		  unsigned long int maxPos)=0;
                        
  /* \brief get an index for the sub-book containing only atoms generated
   * by a given block, creating a new empty sub-book if needed
   * \param blockIdx: target block
   * \return a pointer to the sub_book
   */
  MPTK_LIB_EXPORT virtual GP_Book_c* insert_block_book(unsigned int blockIdx) =0;

  /* \brief get an index for the sub-book containing only atoms at a given position,
   * creating a new empty sub-book if needed
   * \param pos: target position
   * \return a pointer to the sub_book
   */
  MPTK_LIB_EXPORT virtual GP_Book_c* insert_pos_book(unsigned long int pos) =0;

  /* \brief get an iterator pointing to the first atom of the book
   * \return the iterator
   */
  MPTK_LIB_EXPORT virtual GP_Book_Iterator_c& begin()=0;
  
  /* \brief get an iterator pointing after the last atom of the book
   * \return the iterator
   * \remark the end() iterator is invalid: it cannot be dereferenced
   * \remark any change to an iterator that makes it invalid should make it equal to end()
   */
  MPTK_LIB_EXPORT virtual GP_Book_Iterator_c& end()=0;
  
  /** \brief Substract/add all the atoms in a given frame from / to a multichannel signal
   *  with amplitudes proportional to their correlations with the residual.
   *
   * \param dict: the dictionary used to interprete this book
   * \param step: the gradient step
   * \param sigSub signal from which the atom waveform is to be removed
   * \param sigAdd signal to which the atom waveform is to be added
   *
   * \remark Passing sigSub == NULL or sigAdd == NULL skips the corresponding substraction / addition.
   */
 //MPTK_LIB_EXPORT void substract_add_grad(MP_Dict_c* dict, MP_Real_t step, 
 //                                        MP_Signal_c* sigSub, MP_Signal_c* sigAdd)=0;

  /** \brief rebuild the waveform of the combination of all atoms in the book
   * 
   * \param dict: the dictionary used to interprete the book
   * \param outBuffer: the buffer to store the result to. Has to be initially filled with zeroes.
   * \param tmpBuffer: buffer used for storing temporary results. Should be the same size as outBuffer.
   */
   MPTK_LIB_EXPORT unsigned long int build_waveform_amp(MP_Dict_c* dict, MP_Real_t* outBuffer, MP_Real_t* tmpBuffer);
   
   /** \brief rebuild the waveform of the combination of all atoms in the book,
    * using the correlations instead of the amplitudes of the atoms.
    * 
    * \param dict: the dictionary used to interprete the book
    * \param outBuffer: the buffer to store the result to. Has to be initially filled with zeroes.
    * \param tmpBuffer: buffer used for storing temporary results. Should be the same size as outBuffer.
    */
   MPTK_LIB_EXPORT unsigned long int build_waveform_corr(MP_Dict_c* dict, MP_Real_t* outBuffer, MP_Real_t* tmpBuffer);

   MPTK_LIB_EXPORT inline bool is_empty(){return begin() == end();}
};

#endif /* __gp_book_h_ */
