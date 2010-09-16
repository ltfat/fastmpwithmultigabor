#ifndef __gp_book_h_
#define __gp_book_h_

/* Abstract iterator class that browses the atoms of a book
 */
class GP_Book_Iterator_c{
 public:
  MPTK_LIB_EXPORT virtual ~GP_Book_Iterator_c(void){}
  
  /* factory method for generic copy */
  MPTK_LIB_EXPORT virtual GP_Book_Iterator_c* copy (void)const=0;
  
  MPTK_LIB_EXPORT virtual GP_Book_Iterator_c& operator ++(void)=0;
  MPTK_LIB_EXPORT virtual GP_Book_Iterator_c& go_to_pos(unsigned long int)=0;
  MPTK_LIB_EXPORT virtual GP_Book_Iterator_c& go_to_next_block(void)=0;
  // virtual GP_Book_Iterator_c& operator --(void)=0;

  MPTK_LIB_EXPORT virtual MP_Atom_c& operator *(void)=0;
  MPTK_LIB_EXPORT virtual MP_Atom_c* operator ->(void)=0;

  MPTK_LIB_EXPORT virtual void print_book(){}

  /* virtual comparison operators. This implementation 
   * will only be called if the run-time types of the 
   * iterators are different
   */
  MPTK_LIB_EXPORT virtual bool operator == (const GP_Book_Iterator_c&)const=0;
  MPTK_LIB_EXPORT bool operator != (const GP_Book_Iterator_c&)const;  
};

/* interface that has to be implemented by every kind of GP book or sub-book
 */
class GP_Book_c{

 public:

  MPTK_LIB_EXPORT virtual ~GP_Book_c()=0;

  /* check if an atom is present
   */
  MPTK_LIB_EXPORT virtual bool contains(unsigned int blockIdx,
			unsigned long int pos,
			MP_Atom_Param_c& param) =0;
  MPTK_LIB_EXPORT bool contains(const MP_Atom_c&);

  /* get an atom if present, NULL otherwise
   */
  MPTK_LIB_EXPORT virtual MP_Atom_c* get_atom(unsigned int blockIdx,
		  unsigned long int pos,
	      MP_Atom_Param_c& param) =0;
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

  /* get an index for the sub-book containing only atoms generated
   * by a given block if it exists, NULL otherwise.
   */
  MPTK_LIB_EXPORT virtual GP_Book_c* get_sub_book(unsigned int blockIdx)=0;

  /* get an index for the sub-book containing only atoms 
   * at a given position if it exists, NULL otherwise.
   */
  MPTK_LIB_EXPORT virtual GP_Book_c* get_sub_book(unsigned long int pos)=0;
  
  /* get an index for the sub-book containing only atoms 
   * between 2 given positions, included. This leaves 
   * the neighbourhood selection strategy to the upper level
   */
  MPTK_LIB_EXPORT virtual GP_Book_c* get_sub_book(unsigned long int minPos,
		  unsigned long int maxPos)=0;
                        
  /* get an index for the sub-book containing only atoms generated
   * by a given block if it exists, and insert a new one otherwise
   */
  MPTK_LIB_EXPORT virtual GP_Book_c* insert_sub_book(unsigned int blockIdx) =0;

  /* get an index for the sub-book containing only atoms 
   * at a given position if it exists, and insert a new 
   * one otherwise.
   */
  MPTK_LIB_EXPORT virtual GP_Book_c* insert_sub_book(unsigned long int pos) =0;

  MPTK_LIB_EXPORT virtual GP_Book_Iterator_c& begin()=0;
  MPTK_LIB_EXPORT virtual GP_Book_Iterator_c& end()=0;
};

#endif /* __gp_book_h_ */
