#ifndef __gp_pos_book_h_
#define __gp_pos_book_h_

using namespace std;

#include <map>

typedef map<unsigned long int, GP_Param_Book_c> posBookMap;

class GP_Pos_Book_c;

class GP_Pos_Book_Iterator_c: public GP_Book_Iterator_c{
  
 public:
  GP_Pos_Book_c* book;
  posBookMap::iterator posIter;
  GP_Param_Book_Iterator_c paramIter;

  MPTK_LIB_EXPORT GP_Pos_Book_Iterator_c(void);
  MPTK_LIB_EXPORT GP_Pos_Book_Iterator_c(GP_Pos_Book_c*);
  MPTK_LIB_EXPORT GP_Pos_Book_Iterator_c(const GP_Pos_Book_Iterator_c&);
  MPTK_LIB_EXPORT GP_Pos_Book_Iterator_c(GP_Pos_Book_c*, const posBookMap::iterator&);

  MPTK_LIB_EXPORT ~GP_Pos_Book_Iterator_c(void);
  
  MPTK_LIB_EXPORT GP_Pos_Book_Iterator_c* copy()const;
  
  MPTK_LIB_EXPORT MP_Atom_c& operator *(void);
  MPTK_LIB_EXPORT MP_Atom_c* operator ->(void);
  
  MPTK_LIB_EXPORT virtual GP_Pos_Book_Iterator_c& operator ++(void);
  MPTK_LIB_EXPORT virtual GP_Pos_Book_Iterator_c& go_to_pos(unsigned long int);
  MPTK_LIB_EXPORT virtual GP_Pos_Book_Iterator_c& go_to_next_block(void);

  MPTK_LIB_EXPORT virtual bool operator == (const GP_Book_Iterator_c&)const;
};

class GP_Pos_Book_c:public GP_Book_c, public posBookMap{

 public:
  
  unsigned int blockIdx;
  GP_Pos_Book_Iterator_c begIter, endIter;

  /* empty book constructor
   */
  MPTK_LIB_EXPORT GP_Pos_Book_c(unsigned int blockIdx);
  MPTK_LIB_EXPORT GP_Pos_Book_c(const GP_Pos_Book_c&);

  MPTK_LIB_EXPORT ~GP_Pos_Book_c();


  /* check if an atom is present
   */
  MPTK_LIB_EXPORT virtual bool contains(unsigned int blockIdx,
		  unsigned long int pos,
		  MP_Atom_Param_c& param);
  MPTK_LIB_EXPORT bool contains(unsigned long int pos,
		MP_Atom_Param_c& param);
  MPTK_LIB_EXPORT bool contains(const MP_Atom_c&);

  /* get an atom if present, NULL otherwise
   */
  MPTK_LIB_EXPORT virtual MP_Atom_c* get_atom(unsigned int blockIdx ,
		  unsigned long int pos,
		  MP_Atom_Param_c& param);
  MPTK_LIB_EXPORT virtual MP_Atom_c* get_atom(unsigned long int pos,
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
  MPTK_LIB_EXPORT virtual int append( MP_Atom_c *newAtom );

  /* get an index for the sub-book containing only atoms generated
   * by a given block.
   */
  MPTK_LIB_EXPORT virtual GP_Pos_Book_c* get_sub_book(unsigned int blockIdx);

  /* get an index for the sub-book containing only atoms 
   * at a given position
   */
  MPTK_LIB_EXPORT virtual GP_Param_Book_c* get_sub_book(unsigned long int pos);
  
  MPTK_LIB_EXPORT virtual GP_Pos_Book_c* insert_sub_book(unsigned int blockIdx);
  
  MPTK_LIB_EXPORT virtual GP_Param_Book_c* insert_sub_book(unsigned long int pos);

  /* get an index for the sub-book containing only atoms 
   * between 2 given positions, included. This leaves 
   * the neighbourhood selection strategy to the upper level.
   */
  MPTK_LIB_EXPORT virtual GP_Pos_Range_Sub_Book_c* get_sub_book(unsigned long int minPos,
		  unsigned long int maxPos);

  MPTK_LIB_EXPORT bool empty();

  MPTK_LIB_EXPORT GP_Pos_Book_Iterator_c& begin();
  MPTK_LIB_EXPORT GP_Pos_Book_Iterator_c& end();

  MPTK_LIB_EXPORT GP_Pos_Book_c& operator = (const GP_Pos_Book_c&);
};

MPTK_LIB_EXPORT void swap(GP_Pos_Book_c&, GP_Pos_Book_c&);

#endif /* __gp_pos_book_h_ */
