#ifndef __gp_pos_range_sub_book_h_
#define __gp_pos_range_sub_book_h_

using namespace std;

/* \brief A sub-book class that wraps a book in a handler to filter only the atoms that remain between two positions
 */
class GP_Pos_Range_Sub_Book_c;

/* \brief The iterator class to browse GP_Pos_Range_Sub_Book_c
 */
class GP_Pos_Range_Sub_Book_Iterator_c: public GP_Book_Iterator_c{
 public:
  
  /* \brief The browsed GP_Pos_Range_Sub_Book_c
   */
  GP_Pos_Range_Sub_Book_c* book;
  
  /* \brief An iterator to the book wrapped inside the GP_Pos_Range_Sub_Book_c
   */
  GP_Book_Iterator_c* iter;

  /* \brief Empty constructor
   */
  MPTK_LIB_EXPORT GP_Pos_Range_Sub_Book_Iterator_c():iter(NULL){}
  
  /* \brief Constructor to the first atom of a given GP_Pos_Range_Sub_Book_c
   */
  MPTK_LIB_EXPORT GP_Pos_Range_Sub_Book_Iterator_c(GP_Pos_Range_Sub_Book_c*);
  
  /* \brief Constructor to the first atom after a given position in a given GP_Pos_Range_Sub_Book_c
   * \param book: the book
   * \param iter: target position
   */
  MPTK_LIB_EXPORT GP_Pos_Range_Sub_Book_Iterator_c(GP_Pos_Range_Sub_Book_c*,
                                   const GP_Book_Iterator_c&);
  
  /* \brief Copy constructor
   * \remark book is not copied, iter is
   */
  MPTK_LIB_EXPORT GP_Pos_Range_Sub_Book_Iterator_c(const GP_Pos_Range_Sub_Book_Iterator_c&);

  MPTK_LIB_EXPORT ~GP_Pos_Range_Sub_Book_Iterator_c(void);
  
  /* \brief Copy function
   * \remark book is not copied, iter is
   */
  MPTK_LIB_EXPORT GP_Pos_Range_Sub_Book_Iterator_c* copy()const;
  
  /* \brief Incrementation operator
   * \return the iterator after incrementation
   */
  MPTK_LIB_EXPORT GP_Pos_Range_Sub_Book_Iterator_c& operator ++(void);
  
  /* \brief Go to the first next atom with position greater or equal than the parameter
   * \param pos: target position
   * \return the iterator after incrementation
   */
  MPTK_LIB_EXPORT GP_Pos_Range_Sub_Book_Iterator_c& go_to_pos(unsigned long int);
  
  /* \brief Go to the first next atom with block index strictly greater than the current one
   * \return the iterator after incrementation
   */
  MPTK_LIB_EXPORT GP_Pos_Range_Sub_Book_Iterator_c& go_to_next_block(void);

  /* \brief Dereference operator
   * \return A reference to the pointed atom
   */
  MPTK_LIB_EXPORT MP_Atom_c& operator *(void);
  
  /* \brief Pointed atom member access operator
   */
  MPTK_LIB_EXPORT MP_Atom_c* operator ->(void);

  /* \brief Equality operator
   * \param the other iterator to compare to
   * \return true if both iterators are at the end of the same book
   * or they both point to the same atom of the same book, 
   * false otherwise
   */
  MPTK_LIB_EXPORT bool operator == (const GP_Book_Iterator_c&)const;

  /* \brief Assignation operator
   * \param the iterator to assign to this
   * \result reference to *this after assignation
   */
  MPTK_LIB_EXPORT GP_Pos_Range_Sub_Book_Iterator_c& operator=(const GP_Pos_Range_Sub_Book_Iterator_c&);
};

class GP_Pos_Range_Sub_Book_c: public GP_Book_c{

 public:
 
  /* \brief wrapped book
   */
  GP_Book_c* book;
  
  /* \brief lower bound, included
   */
  unsigned long int minPos;
  
  /* \brief upper bound, excluded
   */
  unsigned long int maxPos;
  
  /* \brief beginning and end iterators
   * TODO: implement the caching system
   */
  GP_Pos_Range_Sub_Book_Iterator_c begIter, endIter;
  
  /* \brief Empty constructor
   */
  MPTK_LIB_EXPORT GP_Pos_Range_Sub_Book_c();
  
  /* Wrapper constructor
   * \param book: the book to wrap
   * \param minPos: lower bound, included
   * \param maxPos: upper bound, excluded
   */
  MPTK_LIB_EXPORT GP_Pos_Range_Sub_Book_c(GP_Book_c* book,
		  unsigned long int minPos,
		  unsigned long int maxPos);

  /* \brief test if an atom belongs to the book
   * \param blockIdx: block index of the atom
   * \param pos: position of the atom
   * \param param: atom parameters
   * \return: true if an atom with those indices belongs to the book
   */ 
  MPTK_LIB_EXPORT bool contains(unsigned int blockIdx,
		  unsigned long int pos,
		  MP_Atom_Param_c& param);
  
  /* \brief test if an atom belongs to the book
   * \param the atom to look for
   * \return: true if an atom with the same indices as the argument
   * belongs to the book
   */
  MPTK_LIB_EXPORT bool contains(const MP_Atom_c&);

  /* \brief retrieve an atom
   * \param blockIdx: block of the atom
   * \param pos: position of the atom
   * \param param: atom parameters
   * \return a pointer to the atom if present, NULL otherwise
   */
  MPTK_LIB_EXPORT MP_Atom_c* get_atom(unsigned int blockIdx ,
		  unsigned long int pos,
		  MP_Atom_Param_c& param);
          
  /* \brief retrieve an atom
   * \param atom: the atom
   * \return a pointer to an atom with the same block, position and parameters as the argument if there is one, NULL otherwise
   */ 
  MPTK_LIB_EXPORT MP_Atom_c* get_atom(const MP_Atom_c&);

  /** \brief Add a new atom in the storage space, taking care of the necessary allocations 
   * \param newAtom a reference to an atom
   * \return the number of appended atoms (1 upon success, zero otherwise)
   * \remark The reference newAtom is not copied, 
   *  it is stored and will be deleted when the book is destroyed
   * \remark \a numChans is set up if this is the first atom to be appended to the book,
   * otherwise, if the atom \a numChans does not match \a numChans, it is not appended.
   */
  MPTK_LIB_EXPORT int append(MP_Atom_c*);

  /* \brief get an index for the sub-book containing only atoms generated
   * by a given block
   * \param blockIdx: target block
   * \return a pointer to the sub_book if it exists, NULL otherwise
   */
  MPTK_LIB_EXPORT GP_Pos_Range_Sub_Book_c* get_sub_book(unsigned int blockIdx);
    
  /* \brief get an index for the sub-book containing only atoms at a given position
   * \param pos: target position
   * \return a pointer to the sub_book if it exists, NULL otherwise
   */
  MPTK_LIB_EXPORT GP_Book_c* get_sub_book(unsigned long int pos);
  
  /* \brief get an index for the sub-book containing only atoms generated
   * by a given block, creating a new empty sub-book if needed
   * \param blockIdx: target block
   * \return a pointer to the sub_book
   */
  MPTK_LIB_EXPORT GP_Pos_Range_Sub_Book_c* insert_sub_book(unsigned int blockIdx);
    
  /* \brief get an index for the sub-book containing only atoms at a given position,
   * creating a new empty sub-book if needed
   * \param pos: target position
   * \return a pointer to the sub_book
   */
  MPTK_LIB_EXPORT GP_Book_c* insert_sub_book(unsigned long int pos);
    
  /* \brief get an index for the sub-book containing only atoms between two positions
   * \param minPos: lower bound (included)
   * \param maxPos: upper bound (excluded)
   * \return a pointer to the sub_book if it exists, NULL otherwise
   */
  MPTK_LIB_EXPORT GP_Pos_Range_Sub_Book_c* get_sub_book(unsigned long int minPos,
		  unsigned long int maxPos);

  /* \brief get an iterator pointing to the first atom of the book
   * \return the iterator
   */
  MPTK_LIB_EXPORT GP_Pos_Range_Sub_Book_Iterator_c& begin();
  
  /* \brief get an iterator pointing after the last atom of the book
   * \return the iterator
   */
  MPTK_LIB_EXPORT GP_Pos_Range_Sub_Book_Iterator_c& end();
};

#endif /* __gp_pos_range_sub_book_h_ */
