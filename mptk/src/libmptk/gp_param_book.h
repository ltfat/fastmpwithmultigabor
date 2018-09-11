 #ifndef __gp_param_book_index_h_
#define __gp_param_book_index_h_

using namespace std;

#include <map>

/* \brief the map under the implementation of GP_Param_Book_c
 */
typedef map<MP_Atom_Param_c*, MP_Atom_c*, MP_Atom_Param_c::less> paramBookMap;

/* \brief a GP_Book_c to store atoms with the same block index and position.
 * Based on an underlying map<MP_Atom_Param_c*, MP_Atom_c*>.
 */
class GP_Param_Book_c;

/* \brief an iterator to browse GP_Param_Book_c
 */
class GP_Param_Book_Iterator_c: public GP_Book_Iterator_c{
  
 public:
 
  /* \brief browsed book
   */
  GP_Param_Book_c* book;
  
  /* \brief iterator on the underlying map
   */
  paramBookMap::iterator paramIter;

  /* \brief Empty constructor
   */
  MPTK_LIB_EXPORT GP_Param_Book_Iterator_c(void);
  
  /* \brief Constructor that puts the iterator at the beginning of the book
   * \param the browsed book
   */
  MPTK_LIB_EXPORT GP_Param_Book_Iterator_c(GP_Param_Book_c*);
  
  /* \brief Constructor that puts the iterator at a given position of the book
   * \param book: the bwrosed book
   * \param iter: the position inside the map
   */
  MPTK_LIB_EXPORT GP_Param_Book_Iterator_c(GP_Param_Book_c*, const paramBookMap::iterator&);

  MPTK_LIB_EXPORT ~GP_Param_Book_Iterator_c(void);

  /* \brief Copy method
   * \return: a new iterator pointing on the same atom of the same book
   */
  MPTK_LIB_EXPORT GP_Param_Book_Iterator_c* copy()const;
  
  /* \brief make the iterator point to the next atom
   * \return iterator value after incrementation
   */
  MPTK_LIB_EXPORT virtual GP_Param_Book_Iterator_c& operator ++(void);
  
  /* \brief make the iterator point to the first atom after the current one (included) with a position greater or equal than the parameter.
   * \param pos: target position
   * \return the iterator after incrementation
   */
  MPTK_LIB_EXPORT virtual GP_Param_Book_Iterator_c& go_to_pos(unsigned long int);
  
  /* \brief make the iterator point to the first atom after the current one with a strictly higher block index.
   * \return the iterator after incrementation
   */
  MPTK_LIB_EXPORT virtual GP_Param_Book_Iterator_c& go_to_next_block(void);
  
  /* \brief got to the next atom that belongs to a different GP_Param_Book_c than the current one
   * \return the iterator after incrementation
   */
  MPTK_LIB_EXPORT GP_Param_Book_Iterator_c& go_to_next_frame();

  /* brief Get the pointed atom
   * \return the pointed atom
   */
  MPTK_LIB_EXPORT MP_Atom_c& operator *(void);
  
  /* \brief Access operator to the members of the pointed atom
   * \return the address of the pointed atom
   * \remark despite its signature, this operator is binary: it requires the name of the member after the arrow.
   * If you want to use the address of the pointed atom in an expression, use &(*iter) instead.
   */
  MPTK_LIB_EXPORT MP_Atom_c* operator ->(void);
  
  /* \brief Get the GP_Param_Book_c containing the current atom
   * \return a pointer to the book
   */
  MPTK_LIB_EXPORT GP_Param_Book_c* get_frame(void);

  /* \brief Equality operator
   * \param the other iterator to compare to
   * \return true if both iterators point to the same atom of the same book or are both at the end of the same book, false otherwise.
   */
  MPTK_LIB_EXPORT virtual bool operator == (const GP_Book_Iterator_c&)const;
};

class GP_Param_Book_c:public GP_Book_c, public paramBookMap{

 public:
  unsigned int blockIdx;
  unsigned long int pos;

  GP_Param_Book_Iterator_c begIter, endIter;
  
  MPTK_LIB_EXPORT GP_Param_Book_c(){}

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

  MPTK_LIB_EXPORT GP_Param_Book_c* get_block_book(unsigned int blockIdx);

  MPTK_LIB_EXPORT GP_Param_Book_c* get_pos_book(unsigned long int pos);
  
  MPTK_LIB_EXPORT GP_Param_Book_c* insert_block_book(unsigned int blockIdx);

  MPTK_LIB_EXPORT GP_Param_Book_c* insert_pos_book(unsigned long int pos);

  MPTK_LIB_EXPORT GP_Param_Book_c* get_range_book(unsigned long int minPos,
		  unsigned long int maxPos);

  MPTK_LIB_EXPORT void reset(void);

  MPTK_LIB_EXPORT GP_Param_Book_Iterator_c& begin();
  MPTK_LIB_EXPORT GP_Param_Book_Iterator_c& end();

  MPTK_LIB_EXPORT GP_Param_Book_c& operator =(const GP_Param_Book_c&);
  
  /** \brief rebuild the waveform of the combination of all atoms in the book
   * 
   * \param dict: the dictionary used to interprete the book
   * \param outBuffer: the buffer to store the result to
   */
   void build_waveform_amp(MP_Dict_c* dict, MP_Real_t* outBuffer);
   
   /** \brief rebuild the waveform of the combination of all atoms in the book,
    * using the correlations instead of the amplitudes of the atoms.
    * 
    * \param dict: the dictionary used to interprete the book
    * \param outBuffer: the buffer to store the result to
    */
   void build_waveform_corr(MP_Dict_c* dict, MP_Real_t* outBuffer);

   /** \brief tells whether a book is empty or not
    */
   bool is_empty();
};

MPTK_LIB_EXPORT void swap(GP_Param_Book_c&, GP_Param_Book_c&);

#endif /* __gp_param_book_h_ */
