#ifndef __gp_pos_range_sub_book_h_
#define __gp_pos_range_sub_book_h_

using namespace std;

class GP_Pos_Range_Sub_Book_c;

class GP_Pos_Range_Sub_Book_Iterator_c: public GP_Book_Iterator_c{
 public:
  GP_Pos_Range_Sub_Book_c* book;
  GP_Book_Iterator_c* iter;

  MPTK_LIB_EXPORT GP_Pos_Range_Sub_Book_Iterator_c():iter(NULL){}
  MPTK_LIB_EXPORT GP_Pos_Range_Sub_Book_Iterator_c(GP_Pos_Range_Sub_Book_c*);
  MPTK_LIB_EXPORT GP_Pos_Range_Sub_Book_Iterator_c(GP_Pos_Range_Sub_Book_c*,
                                   const GP_Book_Iterator_c&);
  MPTK_LIB_EXPORT GP_Pos_Range_Sub_Book_Iterator_c(const GP_Pos_Range_Sub_Book_Iterator_c&);

  MPTK_LIB_EXPORT ~GP_Pos_Range_Sub_Book_Iterator_c(void);
  
  MPTK_LIB_EXPORT GP_Pos_Range_Sub_Book_Iterator_c* copy()const;
  
  MPTK_LIB_EXPORT GP_Pos_Range_Sub_Book_Iterator_c& operator ++(void);
  MPTK_LIB_EXPORT GP_Pos_Range_Sub_Book_Iterator_c& go_to_pos(unsigned long int);
  MPTK_LIB_EXPORT GP_Pos_Range_Sub_Book_Iterator_c& go_to_next_block(void);

  // virtual GP_Book_Iterator_c& operator --(void)=0;
  MPTK_LIB_EXPORT MP_Atom_c& operator *(void);
  MPTK_LIB_EXPORT MP_Atom_c* operator ->(void);

  /* virtual comparison operators. This implementation 
   * will only be called if the run-time types of the 
   * iterators are different
   */
  MPTK_LIB_EXPORT bool operator == (const GP_Book_Iterator_c&)const;

  MPTK_LIB_EXPORT GP_Pos_Range_Sub_Book_Iterator_c& operator=(const GP_Pos_Range_Sub_Book_Iterator_c&);

    /*   virtual bool operator < (const GP_Book_Iterator_c&)const =0; */
    /*   virtual bool operator > (const GP_Book_Iterator_c&)const =0; */
    /*   virtual bool operator <= (const GP_Book_Iterator_c&)const =0; */
    /*   virtual bool operator >= (const GP_Book_Iterator_c&)const =0; */
};

/* a GP_Book_c containing atoms all belonging to the same position of the same block.
 * Relies on a map<MP_Atom_Param_c, GP_Atom_c>
 */

class GP_Pos_Range_Sub_Book_c: public GP_Book_c{

 public:
  GP_Book_c* book;
  unsigned long int minPos, maxPos;
  GP_Pos_Range_Sub_Book_Iterator_c begIter, endIter;

  MPTK_LIB_EXPORT GP_Pos_Range_Sub_Book_c();
  MPTK_LIB_EXPORT GP_Pos_Range_Sub_Book_c(GP_Book_c* book,
		  unsigned long int minPos,
		  unsigned long int maxPos);

  MPTK_LIB_EXPORT bool contains(unsigned int blockIdx,
		  unsigned long int pos,
		  MP_Atom_Param_c& param);
  MPTK_LIB_EXPORT bool contains(const MP_Atom_c&);

  MPTK_LIB_EXPORT MP_Atom_c* get_atom(unsigned int blockIdx ,
		  unsigned long int pos,
		  MP_Atom_Param_c& param);
  MPTK_LIB_EXPORT MP_Atom_c* get_atom(const MP_Atom_c&);

  MPTK_LIB_EXPORT int append(MP_Atom_c*);

  MPTK_LIB_EXPORT GP_Pos_Range_Sub_Book_c* get_sub_book(unsigned int blockIdx);
    
  MPTK_LIB_EXPORT GP_Book_c* get_sub_book(unsigned long int pos);
  
  MPTK_LIB_EXPORT GP_Pos_Range_Sub_Book_c* insert_sub_book(unsigned int blockIdx);
    
  MPTK_LIB_EXPORT GP_Book_c* insert_sub_book(unsigned long int pos);
    
  MPTK_LIB_EXPORT GP_Pos_Range_Sub_Book_c* get_sub_book(unsigned long int minPos,
		  unsigned long int maxPos);

  MPTK_LIB_EXPORT GP_Pos_Range_Sub_Book_Iterator_c& begin();
  MPTK_LIB_EXPORT GP_Pos_Range_Sub_Book_Iterator_c& end();
};

#endif /* __gp_pos_range_sub_book_h_ */
