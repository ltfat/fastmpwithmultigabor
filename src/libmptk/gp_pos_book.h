#ifndef __gp_pos_book_h_
#define __gp_pos_book_h_

using namespace std;

#include <map>

/* \brief The map under the implementation of GP_Pos_Book_c
 */
typedef map<unsigned long int, GP_Param_Book_c> posBookMap;

/* \brief A book that stores atoms with the same block index,
 * sorted by increasing position.
 * Based on a map<pos, GP_Param_Book_c>.
 */
class GP_Pos_Book_c;

/* \brief An iterator to browse GP_Pos_Book_c
 */
class GP_Pos_Book_Iterator_c: public GP_Book_Iterator_c{

public:

	/* \brief Browsed book.
	 */
	GP_Pos_Book_c* book;

	/* \brief Iterator brwsing the underlying map
	 */
	posBookMap::iterator posIter;

	/* \brief Iterator browsing inside the GP_Param_Book_c pointed by posIter.
	 */
	GP_Param_Book_Iterator_c paramIter;

	/* \brief Empty constructor
	 */
	MPTK_LIB_EXPORT GP_Pos_Book_Iterator_c(void);

	/* \brief Constructor that puts the iterator at the beginning of a book
	 * \param the browsed book
	 */
	MPTK_LIB_EXPORT GP_Pos_Book_Iterator_c(GP_Pos_Book_c*);

	/* \brief Copy constructor
	 */
	MPTK_LIB_EXPORT GP_Pos_Book_Iterator_c(const GP_Pos_Book_Iterator_c&);

	/* \brief Constructor that puts the book at the beginning of a given position in a book
	 * \param book: the browsed book
	 * \param iter: the position to reach
	 */
	MPTK_LIB_EXPORT GP_Pos_Book_Iterator_c(GP_Pos_Book_c*, const posBookMap::iterator&);

	MPTK_LIB_EXPORT ~GP_Pos_Book_Iterator_c(void);

	/* \brief create a new copy of the iterator
	 * \return a pointer to the new copy
	 */
	MPTK_LIB_EXPORT GP_Pos_Book_Iterator_c* copy()const;

	/* \brief Get the pointed atom
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

	/* \brief make the iterator point to the next atom
	 * \return iterator value after incrementation
	 */
	MPTK_LIB_EXPORT virtual GP_Pos_Book_Iterator_c& operator ++(void);

	/* \brief make the iterator point to the first atom after the current one (included) with a position greater or equal than the parameter.
	 * \param pos: target position
	 * \return the iterator after incrementation
	 */
	MPTK_LIB_EXPORT virtual GP_Pos_Book_Iterator_c& go_to_pos(unsigned long int);

	/* \brief make the iterator point to the first atom after the current one with a strictly higher block index.
	 * \return the iterator after incrementation
	 */
	MPTK_LIB_EXPORT virtual GP_Pos_Book_Iterator_c& go_to_next_block(void);

	/* \brief got to the next atom that belongs to a different GP_Param_Book_c than the current one
	 * \return the iterator after incrementation
	 */
	MPTK_LIB_EXPORT GP_Pos_Book_Iterator_c& go_to_next_frame();

	/* \brief Equality operator
	 * \param the other iterator to compare to
	 * \return true if both iterators point to the same atom of the same book or are both at the end of the same book, false otherwise.
	 */
	MPTK_LIB_EXPORT virtual bool operator == (const GP_Book_Iterator_c&)const;
};

class GP_Pos_Book_c:public GP_Book_c, public posBookMap{

public:

	/* \brief Common block index for all atoms in the book
	 */
	unsigned int blockIdx;

	/* \brief Iterators pointing to the beginning and end of the book
	 * TODO: implement the caching system
	 */
	GP_Pos_Book_Iterator_c begIter, endIter;

	/* \brief Empty book constructor
	 * \param the common block index
	 */
	MPTK_LIB_EXPORT GP_Pos_Book_c(unsigned int blockIdx);

	/* \brief Copy constructor
	 * \param the book to copy
	 */
	MPTK_LIB_EXPORT GP_Pos_Book_c(const GP_Pos_Book_c&);


	MPTK_LIB_EXPORT ~GP_Pos_Book_c();


	/* \brief check if an atom  belongs to the book
	 * \param blockIdx: block of the atom
	 * \param pos: position of the atom
	 * \param param: atom parameters
	 * \return true if the atom is in the book, false otherwise
	 */
	MPTK_LIB_EXPORT virtual bool contains(unsigned int blockIdx,
			unsigned long int pos,
			MP_Atom_Param_c& param);

	/* \brief check if an atom  belongs to the book
	 * \param pos: position of the atom
	 * \param param: atom parameters
	 * \return true if the atom is in the book, false otherwise
	 */
	MPTK_LIB_EXPORT bool contains(unsigned long int pos,
			MP_Atom_Param_c& param);

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
	MPTK_LIB_EXPORT virtual MP_Atom_c* get_atom(unsigned int blockIdx ,
			unsigned long int pos,
			MP_Atom_Param_c& param);

	/* \brief retrieve an atom
	 * \param pos: position of the atom
	 * \param param: atom parameters
	 * \return a pointer to the atom if present, NULL otherwise
	 */
	MPTK_LIB_EXPORT virtual MP_Atom_c* get_atom(unsigned long int pos,
			MP_Atom_Param_c& param);

	/* \brief retrieve an atom
	 * \param atom: the atom
	 * \return a pointer to an atom with the same block, position and parameters as the argument if there is one, NULL otherwise
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
	MPTK_LIB_EXPORT virtual int append( MP_Atom_c *newAtom );

	/* \brief get an index for the sub-book containing only atoms generated
	 * by a given block
	 * \param blockIdx: target block
	 * \return a pointer to the sub_book if it exists, NULL otherwise
	 */
	MPTK_LIB_EXPORT virtual GP_Pos_Book_c* get_sub_book(unsigned int blockIdx);

	/* \brief get an index for the sub-book containing only atoms at a given position
	 * \param pos: target position
	 * \return a pointer to the sub_book if it exists, NULL otherwise
	 */
	MPTK_LIB_EXPORT virtual GP_Param_Book_c* get_sub_book(unsigned long int pos);

	/* \brief get an index for the sub-book containing only atoms generated
	 * by a given block, creating a new empty sub-book if needed
	 * \param blockIdx: target block
	 * \return a pointer to the sub_book
	 */
	MPTK_LIB_EXPORT virtual GP_Pos_Book_c* insert_sub_book(unsigned int blockIdx);

	/* \brief get an index for the sub-book containing only atoms at a given position,
	 * creating a new empty sub-book if needed
	 * \param pos: target position
	 * \return a pointer to the sub_book
	 */
	MPTK_LIB_EXPORT virtual GP_Param_Book_c* insert_sub_book(unsigned long int pos);

	/* get an index for the sub-book containing only atoms
	 * between 2 given positions, included. This leaves
	 * the neighbourhood selection strategy to the upper level.
	 */
	MPTK_LIB_EXPORT virtual GP_Pos_Range_Sub_Book_c* get_sub_book(unsigned long int minPos,
			unsigned long int maxPos);

	/* \brief Test if a book is empty.
	 * \return true if the book contains no atoms, false otherwise.
	 */
	MPTK_LIB_EXPORT bool empty();

	/* \brief get an iterator pointing to the first atom of the book
	 * \return the iterator
	 */
	MPTK_LIB_EXPORT GP_Pos_Book_Iterator_c& begin();

	/* \brief get an iterator pointing after the last atom of the book
	 * \return the iterator
	 * \remark the end() iterator is invalid: it cannot be dereferenced
	 * \remark any change to an iterator that makes it invalid should make it equal to end()
	 */
	MPTK_LIB_EXPORT GP_Pos_Book_Iterator_c& end();

	/* \brief Assignation operator
	 * \param the book to assing to this
	 * \return a reference to *this aftyer assignation
	 */
	MPTK_LIB_EXPORT GP_Pos_Book_c& operator = (const GP_Pos_Book_c&);

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
	MPTK_LIB_EXPORT void substract_add_grad(MP_Dict_c* dict, MP_Real_t step,
			MP_Signal_c* sigSub, MP_Signal_c* sigAdd);

	/** \brief rebuild the waveform of the combination of all atoms in the book
	 *
	 * \param dict: the dictionary used to interprete the book
	 * \param outBuffer: the buffer to store the result to. Has to be initially filled with zeroes.
	 */
	void build_waveform_amp(MP_Dict_c* dict, MP_Real_t* outBuffer);

	/** \brief rebuild the waveform of the combination of all atoms in the book,
	 * using the correlations instead of the amplitudes of the atoms.
	 *
	 * \param dict: the dictionary used to interprete the book
	 * \param outBuffer: the buffer to store the result to. Has to be initially filled with zeroes.
	 */
	void build_waveform_corr(MP_Dict_c* dict, MP_Real_t* outBuffer);
};

/* \brief swap method
 * \param two books to swap
 */
MPTK_LIB_EXPORT void swap(GP_Pos_Book_c&, GP_Pos_Book_c&);

#endif /* __gp_pos_book_h_ */
