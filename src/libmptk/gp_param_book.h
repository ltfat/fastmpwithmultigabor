 #ifndef __gp_param_book_index_h_
#define __gp_param_book_index_h_

using namespace std;

class GP_Param_Book_c;

#include <map>

typedef map<MP_Atom_Param_c*, MP_Atom_c*, MP_Atom_Param_c::less> paramBookMap;

class GP_Param_Book_c;

/* a GP_Book_c containing atoms all belonging to the same position of the same block.
 * Relies on a map<MP_Atom_Param_c, GP_Atom_c>
 */

class GP_Param_Book_Iterator_c: public GP_Book_Iterator_c{
  
 public:
  GP_Param_Book_c* book;
  paramBookMap::iterator paramIter;

  MPTK_LIB_EXPORT GP_Param_Book_Iterator_c(void);
  MPTK_LIB_EXPORT GP_Param_Book_Iterator_c(GP_Param_Book_c*);
  MPTK_LIB_EXPORT GP_Param_Book_Iterator_c(GP_Param_Book_c*, const paramBookMap::iterator&);

  MPTK_LIB_EXPORT ~GP_Param_Book_Iterator_c(void);
  
  MPTK_LIB_EXPORT GP_Param_Book_Iterator_c* copy()const;
  
  MPTK_LIB_EXPORT virtual GP_Param_Book_Iterator_c& operator ++(void);
  MPTK_LIB_EXPORT virtual GP_Param_Book_Iterator_c& go_to_pos(unsigned long int);
  MPTK_LIB_EXPORT virtual GP_Param_Book_Iterator_c& go_to_next_block(void);

  MPTK_LIB_EXPORT MP_Atom_c& operator *(void);
  MPTK_LIB_EXPORT MP_Atom_c* operator ->(void);

  MPTK_LIB_EXPORT virtual bool operator == (const GP_Book_Iterator_c&)const;
};

class GP_Param_Book_c:public GP_Book_c, public paramBookMap{

 public:
  unsigned int blockIdx;
  unsigned long int pos;

  GP_Param_Book_Iterator_c begIter, endIter;

  MPTK_LIB_EXPORT GP_Param_Book_c(unsigned int blockIdx,
          unsigned long int pos);

  MPTK_LIB_EXPORT GP_Param_Book_c(const GP_Param_Book_c&);

  MPTK_LIB_EXPORT virtual ~GP_Param_Book_c();

  MPTK_LIB_EXPORT bool contains(MP_Atom_Param_c&);
  MPTK_LIB_EXPORT bool contains(unsigned int blockIdx,
        unsigned long int pos,
        MP_Atom_Param_c& param);
  MPTK_LIB_EXPORT bool contains(const MP_Atom_c&);

  MPTK_LIB_EXPORT MP_Atom_c* get_atom(MP_Atom_Param_c& param);
  MPTK_LIB_EXPORT MP_Atom_c* get_atom(unsigned int blockIdx ,
			  unsigned long int pos,
              MP_Atom_Param_c& param);
  MPTK_LIB_EXPORT MP_Atom_c* get_atom(const MP_Atom_c& atom);

  MPTK_LIB_EXPORT int append(MP_Atom_c*);

  MPTK_LIB_EXPORT GP_Param_Book_c* get_sub_book(unsigned int blockIdx);

  MPTK_LIB_EXPORT GP_Param_Book_c* get_sub_book(unsigned long int pos);
  
  MPTK_LIB_EXPORT GP_Param_Book_c* insert_sub_book(unsigned int blockIdx);

  MPTK_LIB_EXPORT GP_Param_Book_c* insert_sub_book(unsigned long int pos);

  MPTK_LIB_EXPORT GP_Param_Book_c* get_sub_book(unsigned long int minPos,
		  unsigned long int maxPos);

  MPTK_LIB_EXPORT void reset(void);

  MPTK_LIB_EXPORT GP_Param_Book_Iterator_c& begin();
  MPTK_LIB_EXPORT GP_Param_Book_Iterator_c& end();

  MPTK_LIB_EXPORT GP_Param_Book_c& operator =(const GP_Param_Book_c&);
};

MPTK_LIB_EXPORT void swap(GP_Param_Book_c&, GP_Param_Book_c&);

#endif /* __gp_param_book_h_ */
