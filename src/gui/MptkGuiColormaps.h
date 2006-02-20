/******************************************************************************/
/*                                                                            */
/*                              MptkGuiFrame.h                                */
/*                                                                            */
/*                           Matching Pursuit GUI                             */
/******************************************************************************/

/******************************************************************************/
/*                                       				      */
/*                         DEFINITION OF THE COLOR MAP	 		      */
/*                                                                            */
/******************************************************************************/

#ifndef MPTKGUICOLORMAP_H
#define MPTKGUICOLORMAP_H

#include <mptk.h>
#include "mp_system.h"

#define MP_Colormap_t double
#define error_stream stderr

  /*************/
 /* CONSTANTS */
/*************/

/** \brief Some constants to clarify the use of parameters */

#define LINEAR 0
#define LOGARITHMIC 1

#define COLORS8BITS 256
#define COLORS16BITS 65536

#define GRAY   0
#define COOL   1
#define COPPER 2
#define HOT    3
#define BONE   4
#define PINK   5
#define JET    6

/** \brief Allocates a numColors x 3 array of RGB values
    of type MP_Colormap_t. */
MP_Colormap_t** alloc_cmap( unsigned int numColors );


/** \brief Frees a colormap array. */
void delete_cmap( MP_Colormap_t** cmap );


/** \brief Returns a numColors x 3 gray colormap. */
MP_Colormap_t** new_cmap_gray( unsigned int numColors );


/** \brief Returns a numColors x 3 cool colormap. */
MP_Colormap_t** new_cmap_cool( unsigned int numColors );


/** \brief Returns a numColors x 3 copper colormap. */
MP_Colormap_t** new_cmap_copper( unsigned int numColors );


/** \brief Returns a numColors x 3 hot colormap. */
MP_Colormap_t** new_cmap_hot( unsigned int numColors );


/** \brief Returns a numColors x 3 bone colormap. */
MP_Colormap_t** new_cmap_bone( unsigned int numColors );


/** \brief Returns a numColors x 3 pink colormap. */
MP_Colormap_t** new_cmap_pink( unsigned int numColors );


/** \brief Returns a numColors x 3 jet colormap. */
MP_Colormap_t** new_cmap_jet( unsigned int numColors );


/** \brief Reverts a colormap. */
void revert_cmap( MP_Colormap_t** cmap, unsigned int numColors );


/** \brief 
 * This class offers the definition and tools for a color map, 
 * basically a 3*n array, n being the number of colors, each row containing a RGB code 
 */
class MptkGuiColormaps {

  /******************/
 /* PUBLIC METHODS */
/******************/
 public :
  
   /** \brief Constructor parameters : 
   * \param numColors : number of colors for the colormap (constants COLORS8BITS and COLORS16BITS could be used)
   * \param type : colormap type (GRAY, COOL, COPPER, HOT, BONE, PINK or JET are available)
   * \param m : minimum intensity. Any intensity below m will appear as a zero (no energy)
   * \param M : maximum intensity. Any intensity above M will appear as the same color (maximum energy)
   * \param meth : method for color search in colormap (LINEAR and LOGARITHMIC are available)
   */
  MptkGuiColormaps(unsigned int numColors, int type, MP_Real_t m, MP_Real_t M, short int meth);
  /**  \brief Class destructor */
    ~MptkGuiColormaps();

  /** \brief Getters */
  MP_Real_t getMax();
  MP_Real_t getMin();
  unsigned int getColorNumber();
  int getColormapType();
  MP_Colormap_t* getRow(unsigned int row);

  /** \brief Setters */
  void setColorNumber(unsigned int numColors);
  void setMin(MP_Real_t m);
  void setMax(MP_Real_t M);
  void setSearchMethod(short int meth);
  void setColormapType(int type);

  /** \brief returns the color corresponding to the intensity 
   * \param f : the intensity which the color is wished
   * \return an array containing the RGB code for the color
  */
  MP_Colormap_t* give_color(MP_Real_t f);

  /****************/
 /* PRIVATE DATA */
/****************/
 private : 

  MP_Colormap_t** colorMap;
  MP_Real_t min,max;
  /** \brief Method for color search in colormap (LINEAR and LOGARITHMIC are available) */
  short int method;
  unsigned int nbColors;
  int colormapType;
  /**  \brief Sets the color */
  void setColormap(void);
  
};


#endif /* MPTKGUICOLORMAP_H */
