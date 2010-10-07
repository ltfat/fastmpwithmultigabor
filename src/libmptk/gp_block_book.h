#ifndef __gp_block_book_h_
#define __gp_block_book_h_

#include <vector>

/* \brief the underlying vector to the GP_Block_Book_c class
 */
typedef vector<GP_Pos_Book_c> blockBookVec;

/* \brief a book sorting its atoms according to their block
 */
class GP_Block_Book_c;

/* \brief an iterator to browse GP_Block_Book_c
 */
class GP_Block_Book_Iterator_c: public GP_Book_Iterator_c{
  
 public:
  /* \brief book the iterator point to
   */
  GP_Block_Book_c* book;
  
  /* \brief iterator to browse the underlying vector
   */
  blockBookVec::iterator blockIter;
  
  /* \brief iterator to browse inside the atoms of a given block
   */
  GP_Pos_Book_Iterator_c posIter;

  MPTK_LIB_EXPORT GP_Block_Book_Iterator_c(){}
  
  /* \brief constructor from a book
   * \param the book to link the iterator to
   */
  MPTK_LIB_EXPORT GP_Block_Book_Iterator_c(GP_Block_Book_c*);
  
  /* \brief copy constructor
   * \param the iterator to copy
   */
  MPTK_LIB_EXPORT GP_Block_Book_Iterator_c(const GP_Block_Book_Iterator_c&);
  
  /* \brief constructor that starts at the beginning of a given block
   * \param the book to link the iterator to
   * \param a blockBookVec::iterator that points to the block to start to
   * \remark the blockBookVec::iterator should point in the range of the book
   */
  MPTK_LIB_EXPORT GP_Block_Book_Iterator_c(GP_Block_Book_c*, const blockBookVec::iterator&);
  
  MPTK_LIB_EXPORT ~GP_Block_Book_Iterator_c(void);
  
  /* \brief copy operator
   * \return a pointer to a new iterator pointing to the same atom of the same book as *this
   */
  MPTK_LIB_EXPORT GP_Block_Book_Iterator_c* copy()const;
  
  /* \brief dereference operator
   * \return reference to the pointed atom
   */
  MPTK_LIB_EXPORT MP_Atom_c& operator *(void);
  
  /* \brief atom member access operator
   */
  MPTK_LIB_EXPORT MP_Atom_c* operator ->(void);
  
  /* \brief Get the GP_Param_Book_c containing the current atom
   * \return a pointer to the book
   */
  MPTK_LIB_EXPORT GP_Param_Book_c* get_frame(void);
  
  /* \brief incrementation operator. Makes the iterator point to the next atom
   * \return the iterator after incrementation
   */
  MPTK_LIB_EXPORT GP_Block_Book_Iterator_c& operator ++(void);
  
  /* \brief go to the next atom with position greater or equal than the parameter
   * \param target position
   * \return the iterator after incrementation
   */
  MPTK_LIB_EXPORT GP_Block_Book_Iterator_c& go_to_pos(unsigned long int);
  
  /* \brief got to the next atom that belongs to a different GP_Param_Book_c than the current one
   * \return the iterator after incrementation
   */
  MPTK_LIB_EXPORT GP_Block_Book_Iterator_c& go_to_next_frame();
  
  /* go to the next atom with block index strictly greter than the current one
   * \return the iterator after incrementation
   */
  MPTK_LIB_EXPORT GP_Block_Book_Iterator_c& go_to_next_block(void);
  
  /* equality operator
   * \param the iterator to compare to
   * \return true if both iterators are at the end of the same book or point to the same atom of the same book,
   * false otherwise
   */
  MPTK_LIB_EXPORT bool operator == (const GP_Book_Iterator_c&)const;
};

class GP_Block_Book_c:public GP_Book_c, public vector<GP_Pos_Book_c>{

 public:

  /* \brief records of the begin and end of the book to avoid excess computations.
   * TODO: implement the caching system
   */
  GP_Block_Book_Iterator_c begIter, endIter;

  /* \brief empty constructor
   */
  MPTK_LIB_EXPORT GP_Block_Book_c();
  
  /* \brief construct of book with a given number of blocks
   * \param the total number of blocks
   */
  MPTK_LIB_EXPORT GP_Block_Book_c(unsigned int);

  /* Destructor
   */
  MPTK_LIB_EXPORT ~GP_Block_Book_c();

  /* \brief static creation method
   * \return a pointer to a new book
   */
  MPTK_LIB_EXPORT static GP_Block_Book_c* create();
  
  /* \brief static creation method with a given number of blocks
   * \param the total number of blocks
   * \return pointer to the new book
   */
  MPTK_LIB_EXPORT static GP_Block_Book_c* create(unsigned int numBlocks);

  /* \brief check if an atom is present
   * \param blockIdx: index of the block of the atom
   * \param pos: atom position
   * \param param: atom parameters
   * \return: true if an atom with these indices exists in the book, false otherwise
   */
  MPTK_LIB_EXPORT virtual bool contains(unsigned int blockIdx,
		  unsigned long int pos,
	      MP_Atom_Param_c& param);
          
  /* \brief check if an atom is present
   * \param atom: searched atom
   * \return: true if an atom with the same indices as the parameter exists in the book, false otherwise
   */
  MPTK_LIB_EXPORT bool contains(const MP_Atom_c&);

  /* \brief retrive an atom if present
   * \param blockIdx: index of the block of the atom
   * \param pos: atom position
   * \param param: atom parameters
   * \return: a pointer to the atom if present, NULL otherwise
   */
  MPTK_LIB_EXPORT virtual MP_Atom_c* get_atom(unsigned int blockIdx,
		  unsigned long int pos,
		  MP_Atom_Param_c& param);
          
  /* \brief retrive an atom if present
   * \param atom: searched atom
   * \return: the pointer to an atom with the same indices as the parameter if present, false otherwise
   */
  MPTK_LIB_EXPORT MP_Atom_c* get_atom(const MP_Atom_c&);


  /** \brief Clear all the atoms from the book.
   */
  MPTK_LIB_EXPORT void reset( void );

  /** \brief Add a new atom in the storage space, taking care of the necessary allocations 
   * \param newAtom a reference to an atom
   * \return the number of appended atoms (1 upon success, zero otherwise)
   * \remark The reference newAtom is not copied, it is stored and will be deleted when the book is destroyed
   * \remark \a numChans is set up if this is the first atom to be appended to the book,
   * otherwise, if the atom \a numChans does not match \a numChans, it is not appended.
   */
   
  MPTK_LIB_EXPORT int append( MP_Atom_c *newAtom );

  /* \brief get the sub-book containing only atoms generated
   * by a given block
   * \param the block index
   * \return a pointer to the corresponding GP_Pos_Book_c if it exists, NULL otherwise 
   */
  MPTK_LIB_EXPORT virtual GP_Pos_Book_c* get_sub_book(unsigned int blockIdx);
  
  /* \brief get the sub-book containing only atoms generated
   * by a given block, creating it if necessary
   * \param the block index
   * \return a pointer to the corresponding GP_Pos_Book_c. 
   */
  MPTK_LIB_EXPORT virtual GP_Pos_Book_c* insert_sub_book(unsigned int blockIdx);
   
  /* \brief get the sub-book containing only atoms generated
   * by a given position
   * \param target position
   * \return a pointer to a GP_Pos_Range_Sub_Book_c restraining the book between pos and pos+1. 
   */
  MPTK_LIB_EXPORT virtual GP_Pos_Range_Sub_Book_c* get_sub_book(unsigned long int pos);
  
  /* \brief get the sub-book containing only atoms generated
   * by a given position
   * \param target position
   * \return a pointer to a GP_Pos_Range_Sub_Book_c restraining the book between pos and pos+1. 
   */
  MPTK_LIB_EXPORT virtual GP_Pos_Range_Sub_Book_c* insert_sub_book(unsigned long int pos);
   
  /* \brief get the sub-book containing only atoms 
   * between 2 given positions.
   * \param minPos: lower bound (included)
   * \param maxPos: upper bound(excluded)
   * \return apointer to the corresponding GP_Pos_Range_Sub_Book_c
   */
  MPTK_LIB_EXPORT virtual GP_Pos_Range_Sub_Book_c* get_sub_book(unsigned long int minPos,unsigned long int maxPos);

  /* \brief get an iterator pointing to the first atom
   * \return the iterator
   */
  MPTK_LIB_EXPORT GP_Block_Book_Iterator_c& begin();
  
  /* \brief get an iterator pointing after the last atom
   * \return the iterator
   */
  MPTK_LIB_EXPORT GP_Block_Book_Iterator_c& end();

  /* \brief test if the book is empty
   * \return true if the book contains no atoms, false otherwise
   */
  MPTK_LIB_EXPORT bool empty();
  
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
 //                                        MP_Signal_c* sigSub, MP_Signal_c* sigAdd);
};

#endif /* __gp_block_book_h_ */
