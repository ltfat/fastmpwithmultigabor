#ifndef __gp_block_book_h_
#define __gp_block_book_h_

#include <vector>

typedef vector<GP_Pos_Book_c> blockBookVec;

class GP_Block_Book_c;

class GP_Block_Book_Iterator_c: public GP_Book_Iterator_c{
  
 public:
  GP_Block_Book_c* book;
  blockBookVec::iterator blockIter;
  GP_Pos_Book_Iterator_c posIter;

  MPTK_LIB_EXPORT GP_Block_Book_Iterator_c(){}
  MPTK_LIB_EXPORT GP_Block_Book_Iterator_c(GP_Block_Book_c*);
  MPTK_LIB_EXPORT GP_Block_Book_Iterator_c(const GP_Block_Book_Iterator_c&);
  MPTK_LIB_EXPORT GP_Block_Book_Iterator_c(GP_Block_Book_c*, const blockBookVec::iterator&);
  
  MPTK_LIB_EXPORT ~GP_Block_Book_Iterator_c(void);
  
  MPTK_LIB_EXPORT GP_Block_Book_Iterator_c* copy()const;
  
  MPTK_LIB_EXPORT MP_Atom_c& operator *(void);
  MPTK_LIB_EXPORT MP_Atom_c* operator ->(void);
  
  MPTK_LIB_EXPORT GP_Block_Book_Iterator_c& operator ++(void);
  MPTK_LIB_EXPORT GP_Block_Book_Iterator_c& go_to_pos(unsigned long int);
  MPTK_LIB_EXPORT GP_Block_Book_Iterator_c& go_to_next_block(void);
  
  MPTK_LIB_EXPORT bool operator == (const GP_Book_Iterator_c&)const;

  MPTK_LIB_EXPORT virtual void print_book();
};

class GP_Block_Book_c:public GP_Book_c, public vector<GP_Pos_Book_c>{

 public:

  GP_Block_Book_Iterator_c begIter, endIter;

  /* NULL constructor
   */
  MPTK_LIB_EXPORT GP_Block_Book_c();
  MPTK_LIB_EXPORT GP_Block_Book_c(unsigned int);

  /* Destructor
   */
  MPTK_LIB_EXPORT ~GP_Block_Book_c();

  MPTK_LIB_EXPORT static GP_Block_Book_c* create();
  MPTK_LIB_EXPORT static GP_Block_Book_c* create(unsigned int numBlocks);

  /* check if an atom is present
   */
  MPTK_LIB_EXPORT virtual bool contains(unsigned int blockIdx,
		  unsigned long int pos,
	      MP_Atom_Param_c& param);
  MPTK_LIB_EXPORT bool contains(const MP_Atom_c&);

  /* get an atom if present, NULL otherwise
   */
  MPTK_LIB_EXPORT virtual MP_Atom_c* get_atom(unsigned int blockIdx,
		  unsigned long int pos,
		  MP_Atom_Param_c& param);
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

  /* get an index for the sub-book containing only atoms generated
   * by a given block if it exists, NULL otherwise.
   */
  MPTK_LIB_EXPORT virtual GP_Pos_Book_c* get_sub_book(unsigned int blockIdx);
  MPTK_LIB_EXPORT virtual GP_Pos_Book_c* insert_sub_book(unsigned int blockIdx);
   
  /* get an index for the sub-book containing only atoms 
   * at a given position.
   */
  MPTK_LIB_EXPORT virtual GP_Pos_Range_Sub_Book_c* get_sub_book(unsigned long int pos);
  MPTK_LIB_EXPORT virtual GP_Pos_Range_Sub_Book_c* insert_sub_book(unsigned long int pos);
   
  /* get an index for the sub-book containing only atoms 
   * between 2 given positions, included. This leaves 
   * the neighbourhood selection strategy to the upper level.
   */
  MPTK_LIB_EXPORT virtual GP_Pos_Range_Sub_Book_c* get_sub_book(unsigned long int minPos,unsigned long int maxPos);

  MPTK_LIB_EXPORT GP_Block_Book_Iterator_c& begin();
  MPTK_LIB_EXPORT GP_Block_Book_Iterator_c& end();

  MPTK_LIB_EXPORT bool empty();
};

#endif /* __gp_block_book_h_ */
