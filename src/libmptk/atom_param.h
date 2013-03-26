#ifndef __atom_param_h_
#define __atom_param_h_

/* abstract class to store all parameters necessary to identify an atom,
 * save its block index and position. Each block plugin will need a specific 
 * implementation that states which parameters it uses.
 */

#include <map>
#include <typeinfo>

typedef map<string,string,mp_ltstring> atom_map;

/** \brief Abstract base class of indiviudal atom parameters. Used as key by the GP_Param_Book_c class.
 */

class MP_Atom_Param_c{
    
 public:
  
  /** \brief Empty constructor
   */
  MP_Atom_Param_c(){}

 /** \brief export the parameters as a map<string,string> for conversion to other formats
  */
  MPTK_LIB_EXPORT virtual atom_map get_map()const;
  /** \brief export the name of the parameters as a vector<string>. The elements of this vector are the keys of the paramater map
   */ 
  MPTK_LIB_EXPORT virtual vector<string> get_param_names()const;

/** \brief Empty destructor
 */
  MPTK_LIB_EXPORT virtual ~MP_Atom_Param_c(){}

/** \brief comparison operators
 */
  MPTK_LIB_EXPORT virtual bool operator<(const MP_Atom_Param_c&) const;
  MPTK_LIB_EXPORT virtual bool operator>(const MP_Atom_Param_c&) const;
  MPTK_LIB_EXPORT virtual bool operator==(const MP_Atom_Param_c&) const;
  MPTK_LIB_EXPORT virtual bool operator!=(const MP_Atom_Param_c&) const;
  
  /** \brief comparison functor between two MP_Atom_Param_c*.
   * The GP_Param_Book_c class accepts MP_Atom_Param_c* as keys to allow polymorphism,
   * and this functor orders the keys accorded to the pointed values, not the pointers themselves.
   */ 
  class less{
    public:
    /** \brief call operator that implements the comparison between pointed values
     */
     MPTK_LIB_EXPORT bool operator()(const MP_Atom_Param_c*, const MP_Atom_Param_c*) const;
  }; 
};

//void swap(MP_Atom_Param_c&, MP_Atom_Param_c&){}

#endif /* __atom_param_h_ */
