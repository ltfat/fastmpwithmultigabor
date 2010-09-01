#ifndef __atom_param_h_
#define __atom_param_h_

/* abstract class to store all parameters necessary to identify an atom,
 * save its block index and position. Each block plugin will need a specific 
 * implementation that states which parameters it uses.
 */

#include <map>
#include <typeinfo>

typedef map<string,string,mp_ltstring> atom_map;

class MP_Atom_Param_c{
    
 protected:
  
  MP_Atom_Param_c(){}

 public:

  MPTK_LIB_EXPORT virtual atom_map get_map();
  MPTK_LIB_EXPORT virtual vector<string> get_param_names();

  MPTK_LIB_EXPORT virtual ~MP_Atom_Param_c(){}

  MPTK_LIB_EXPORT virtual bool operator<(const MP_Atom_Param_c&) const;
  MPTK_LIB_EXPORT virtual bool operator>(const MP_Atom_Param_c&) const;
  MPTK_LIB_EXPORT virtual bool operator==(const MP_Atom_Param_c&) const;
  MPTK_LIB_EXPORT virtual bool operator!=(const MP_Atom_Param_c&) const;
  
  class less{
    public:
     MPTK_LIB_EXPORT bool operator()(const MP_Atom_Param_c*, const MP_Atom_Param_c*) const;
  }; 
};

//void swap(MP_Atom_Param_c&, MP_Atom_Param_c&){}

#endif /* __atom_param_h_ */
