#include <stdlib.h>
#include <math.h>
#include "MptkGuiColormaps.h"
#include "iostream"

/***********************/
/* Allocate a colormap */
MP_Colormap_t** alloc_cmap( unsigned int numColors ) {

  MP_Colormap_t* cmap_storage;
  MP_Colormap_t* ptr;
  MP_Colormap_t** cmap;
  unsigned int i;

  /* Allocate the colormap array */
  if ( ( cmap_storage = (MP_Colormap_t*) calloc( numColors*3 , sizeof(MP_Colormap_t) ) ) == NULL ) {
    fprintf( error_stream, "MptkGuiColormap -- error : Can't allocate a new colormap storage space."
	     " Returning a NULL colormap.\n" );
    return( NULL );
  }
  /* Allocate the array of pointers to "fold" the array into a cmap[i][j] form */
  if ( ( cmap = (MP_Colormap_t**) calloc( numColors , sizeof(MP_Colormap_t*) ) ) == NULL ) {
    fprintf( error_stream, "MptkGuiColormap -- error : Can't allocate a new colormap array."
	     " Returning a NULL colormap.\n" );
    free( cmap_storage );
    return( NULL );
  }
  /* If everything went fine, fold the array */
  for ( i = 0, ptr = cmap_storage;
	i < numColors;
	i++,   ptr += 3 ) {
    cmap[i] = ptr;
  }

  return( cmap );
}


/*******************/
/* Free a colormap */
void delete_cmap( MP_Colormap_t** cmap ) {

  if ( cmap ) { /* => avoid to free NULL, which may bug on some systems */
    /* First, free the global storage space (pointed to by the first element
       of the array of pointers) */
    if ( *cmap ) free( *cmap );
    /* Then, free the array of pointers */
    free( cmap );
  }

}


/***************************/
/* Returns a gray colormap */
MP_Colormap_t** new_cmap_gray( unsigned int numColors ) {

  MP_Colormap_t** cmap;
  MP_Colormap_t castNumColors, val;
  unsigned int i;

  /* Allocate the colormap array */
  if ( ( cmap = alloc_cmap( numColors) ) == NULL ) {
    fprintf( error_stream, "MptkGuiColormap:gray -- error : Can't allocate a new colormap."
	     " Returning a NULL colormap.\n" );
    return( NULL );
  }

  /* Cast the number of colors (and avoid a division by 0) */
  castNumColors = (MP_Colormap_t)( numColors > 1 ? (numColors - 1) : 1 );

  /* Fill the colormap, same value in all the fields */
  for ( i = 0; i < numColors; i++ ) {
    val = (MP_Colormap_t)(i) / castNumColors;
    /* R */ cmap[i][0] = val;
    /* G */ cmap[i][1] = val;
    /* B */ cmap[i][2] = val;
  }

  return( cmap );
}


/***************************/
/* Returns a gray colormap */
MP_Colormap_t** new_cmap_cool( unsigned int numColors ) {

  MP_Colormap_t** cmap;
  MP_Colormap_t castNumColors, val;
  unsigned int i;

  /* Allocate the colormap array */
  if ( ( cmap = alloc_cmap( numColors) ) == NULL ) {
    fprintf( error_stream, "MptkGuiColormap:cool -- error : Can't allocate a new colormap."
	     " Returning a NULL colormap.\n" );
    return( NULL );
  }

  /* Cast the number of colors (and avoid a division by 0) */
  castNumColors = (MP_Colormap_t)( numColors > 1 ? (numColors - 1) : 1 );

  /* Fill the colormap, same value in all the fields */
  for ( i = 0; i < numColors; i++ ) {
    val = (MP_Colormap_t)(i) / castNumColors;
    /* R */ cmap[i][0] = val;
    /* G */ cmap[i][1] = 1.0 - val;
    /* B */ cmap[i][2] = 1.0;
  }

  return( cmap );
}


/*****************************/
/* Returns a copper colormap */
MP_Colormap_t** new_cmap_copper( unsigned int numColors ) {

  MP_Colormap_t** cmap;
  MP_Colormap_t val;
  unsigned int i;

#define CR 1.25
#define CG 0.7812
#define CB 0.4975

  /* Make a gray colormap */
  if ( ( cmap = new_cmap_gray( numColors) ) == NULL ) {
    fprintf( error_stream, "MptkGuiColormap:copper -- error : Can't get a new gray colormap"
	     " to make a copper colormap. Returning a NULL colormap.\n" );
    return( NULL );
  }

  /* Modify the gray colormap (and avoid values bigger than 1) */
  for ( i = 0; i < numColors; i++ ) {
    /* R */ val = cmap[i][0] * CR; cmap[i][0] = ( val < 1 ? val : 1 );
    /* G */ val = cmap[i][1] * CG; cmap[i][1] = ( val < 1 ? val : 1 );
    /* B */ val = cmap[i][2] * CB; cmap[i][2] = ( val < 1 ? val : 1 );
  }

  return( cmap );
}


/***************************/
/* Returns a hot colormap */
MP_Colormap_t** new_cmap_hot( unsigned int numColors ) {

  MP_Colormap_t** cmap;
  MP_Colormap_t fracNumColors;
  unsigned int castFrac;
  unsigned int i;

  /* Allocate the colormap array */
  if ( ( cmap = alloc_cmap( numColors) ) == NULL ) {
    fprintf( error_stream, "MptkGuiColormap:hot -- error : Can't allocate a new colormap."
	     " Returning a NULL colormap.\n" );
    return( NULL );
  }

  /* Compute 3/8the of the number of colors */
  fracNumColors = (MP_Colormap_t) floor ( (double)( numColors ) * 3 / 8 );
  castFrac = (unsigned int)( fracNumColors );

  /* Fill the colormap */
  /* R */
  for ( i = 0; i < castFrac;  i++ ) cmap[i][0] = (MP_Colormap_t)( i + 1 ) / fracNumColors;
  for (      ; i < numColors; i++ ) cmap[i][0] = 1;
  /* G */
  for ( i = 0; i < castFrac;      i++ ) cmap[i][1] = 0;
  for (      ; i < (2*castFrac);  i++ ) cmap[i][1] = (MP_Colormap_t)( i - castFrac + 1 ) / fracNumColors;
  for (      ; i < numColors;     i++ ) cmap[i][1] = 1;
  /* B */
  for ( i = 0; i < (2*castFrac); i++ ) cmap[i][2] = 0;
  for (      ; i < numColors;    i++ )
    cmap[i][2] = (MP_Colormap_t)(i - 2*castFrac + 1) / (MP_Colormap_t)(numColors - 2*castFrac);

  return( cmap );
}


/***************************/
/* Returns a hot colormap */
MP_Colormap_t** new_cmap_bone( unsigned int numColors ) {

  MP_Colormap_t** cmap;
  MP_Colormap_t** cmap_gray;
  MP_Colormap_t** cmap_hot;
  unsigned int i;

  /* Allocate the colormap array */
  if ( ( cmap = alloc_cmap( numColors) ) == NULL ) {
    fprintf( error_stream, "MptkGuiColormap:bone -- error : Can't allocate a new colormap."
	     " Returning a NULL colormap.\n" );
    return( NULL );
  }

  cmap_gray = new_cmap_gray( numColors );
  cmap_hot  =  new_cmap_hot( numColors );

  for ( i = 0; i < numColors; i++ ) {
    /* R */ cmap[i][0] = ( 7*cmap_gray[i][0] + cmap_hot[i][2] ) / 8.0;
    /* G */ cmap[i][1] = ( 7*cmap_gray[i][1] + cmap_hot[i][1] ) / 8.0;
    /* B */ cmap[i][2] = ( 7*cmap_gray[i][2] + cmap_hot[i][0] ) / 8.0;
  }

  delete_cmap( cmap_gray );
  delete_cmap( cmap_hot );

  return( cmap );
}


/***************************/
/* Returns a hot colormap */
MP_Colormap_t** new_cmap_pink( unsigned int numColors ) {

  MP_Colormap_t** cmap;
  MP_Colormap_t** cmap_gray;
  MP_Colormap_t** cmap_hot;
  unsigned int i;

  /* Allocate the colormap array */
  if ( ( cmap = alloc_cmap( numColors) ) == NULL ) {
    fprintf( error_stream, "MptkGuiColormap:pink -- error : Can't allocate a new colormap."
	     " Returning a NULL colormap.\n" );
    return( NULL );
  }

  cmap_gray = new_cmap_gray( numColors );
  cmap_hot  =  new_cmap_hot( numColors );

  for ( i = 0; i < numColors; i++ ) {
    /* R */ cmap[i][0] = (MP_Colormap_t) sqrt( ( 2*(double)(cmap_gray[i][0]) + (double)(cmap_hot[i][0]) ) / 3.0 );
    /* G */ cmap[i][1] = (MP_Colormap_t) sqrt( ( 2*(double)(cmap_gray[i][1]) + (double)(cmap_hot[i][1]) ) / 3.0 );
    /* B */ cmap[i][2] = (MP_Colormap_t) sqrt( ( 2*(double)(cmap_gray[i][2]) + (double)(cmap_hot[i][2]) ) / 3.0 );
  }

  delete_cmap( cmap_gray );
  delete_cmap( cmap_hot );

  return( cmap );
}


/***************************/
/* Returns a hot colormap */
MP_Colormap_t** new_cmap_jet( unsigned int numColors ) {

  MP_Colormap_t** cmap;
  MP_Colormap_t fracNumColors;
  unsigned int castFrac;
  unsigned int idxLen;
  unsigned int i;

  /* Allocate the colormap array */
  if ( ( cmap = alloc_cmap( numColors) ) == NULL ) {
    fprintf( error_stream, "MptkGuiColormap:jet -- error : Can't allocate a new colormap."
	     " Returning a NULL colormap.\n" );
    return( NULL );
  }

  /* Compute some useful sizes */
  fracNumColors = (MP_Colormap_t) ceil( (double)( numColors ) / 4.0 );
  castFrac = (unsigned int)( fracNumColors );
  //fprintf(stderr,"castFrac: %u\n", castFrac );fflush(stderr);
  idxLen = 3*castFrac;

  {
    MP_Colormap_t u[idxLen];
    long int rIdx[idxLen];
    long int gIdx[idxLen];
    long int bIdx[idxLen];
    unsigned int bIdxCount = 0;
    unsigned int offset;

    /* Make the vector of color values */
    for ( i = 0; i < castFrac;     i++ ) u[i] = (MP_Colormap_t)( i + 1 ) / fracNumColors;
    for (      ; i < (2*castFrac-1); i++ ) u[i] = 1;
    for (      ; i < (3*castFrac); i++ ) u[i] = (MP_Colormap_t)( 3*castFrac - i -1 ) / fracNumColors;

    /* Make some color indexes */
    offset = (unsigned int) ceil ( (double)(fracNumColors) / 2 )
      - ( (numColors % 4) == 1 ? 1 : 0 );
    for ( i = 0; i < idxLen; i++ ) {
      gIdx[i] = offset + i;
      rIdx[i] = offset + i + castFrac;
      bIdx[i] = offset + i - castFrac;
      if ( bIdx[i] > 0 ) bIdxCount++;
    }
    //fprintf(stderr,"BIDXCOUNT: %u\n", bIdxCount );fflush(stderr);
    
    /* Fill the colormap at the right indexes */
    for ( i = 0; i < idxLen; i++ ) {
      /* R */
      if ( rIdx[i] < (long int)(numColors) ) cmap[rIdx[i]][0] = u[i];
      /* G */
      if ( gIdx[i] < (long int)(numColors) ) cmap[gIdx[i]][1] = u[i];
      /* B */
      if ( bIdx[i] > 0 ) cmap[bIdx[i]-1][2] = u[i-1];
    }

  }

  return( cmap );
}


/**********************/
/* Reverts a colormap */
void revert_cmap( MP_Colormap_t **cmap, unsigned int numColors ) {

  unsigned int i;
  unsigned int half;
  MP_Colormap_t val;

  half = (numColors >> 1);

  for ( i = 0; i < half; i++ ) {
    /* R */
    val = cmap[i][0];
    cmap[i][0] = cmap[numColors-i-1][0];
    cmap[numColors-i-1][0] = val;
    /* G */
    val = cmap[i][1];
    cmap[i][1] = cmap[numColors-i-1][1];
    cmap[numColors-i-1][1] = val;
    /* B */
    val = cmap[i][2];
    cmap[i][2] = cmap[numColors-i-1][2];
    cmap[numColors-i-1][2] = val;
  }

}

/***************/
/* Constructor */
MptkGuiColormaps::MptkGuiColormaps(unsigned int numColors, int type, MP_Real_t m, MP_Real_t M, short int meth)
{

  min = m;
  max = M;

  if((meth>1)||(meth<0))method=LINEAR; // if the user chooses a wrong number, the search method will be LINEAR
  else method=meth;
  
  nbColors = numColors;
  colormapType = type;
  setColormap();
}

/**************/
/* Destructor */
MptkGuiColormaps::~MptkGuiColormaps()
{
  delete_cmap(colorMap);
}

/***********/
/* getters */
MP_Real_t
MptkGuiColormaps::getMin()
{
  return min;
}

MP_Real_t
MptkGuiColormaps::getMax()
{
  return max;
}

unsigned int
MptkGuiColormaps::getColorNumber()
{
  return nbColors;
}

int
MptkGuiColormaps:: getColormapType()
{
  return colormapType;
}

MP_Colormap_t *
MptkGuiColormaps:: getRow(unsigned int row)
{
  if(row>=nbColors)return NULL;
  return colorMap[row];
}

/***********/
/* setters */
void
MptkGuiColormaps::setMin(MP_Real_t m)
{
  min = m;
}

void
MptkGuiColormaps::setMax(MP_Real_t M)
{
  max = M;
}

void 
MptkGuiColormaps::setColorNumber(unsigned int numColors)
{
  delete_cmap(colorMap);
  nbColors = numColors;
  setColormap();
}

void 
MptkGuiColormaps::setColormapType(int type)
{
  delete_cmap(colorMap);
  colormapType = type;
  setColormap();
}

void
MptkGuiColormaps::setSearchMethod(short int meth)
{
  if((meth>1)||(meth<0))method=LINEAR; // if the user chooses a wrong number, the search method will be LINEAR
  else method=meth;
}

/****************************************************************/
/* Returns an RGB array according to min, max and search method */
MP_Colormap_t*
MptkGuiColormaps::give_color(MP_Real_t f)
{
  unsigned int i;
  if(f>=max)i=nbColors-1;//i=0;
  else if(f<=min)i=0;//i=nbColors-1;
  else 
    {
      if(method==LINEAR)
	{
	  i =((unsigned int) ((f*nbColors-1)/max));
	}
      else // method is logarithmic
	{
	  //i = (unsigned int) 20*log10(/*nbColors -*/ (unsigned int) (f/max*nbColors));
	  i = (unsigned int) ( ( log10(f/min)/log10(max/min) )*(double)(nbColors-1) );
	}
    }
  return colorMap[i];
}

/*************************************************************************************/
/* A private function which is used to create the right colormap (easier to maintain)*/
void
MptkGuiColormaps::setColormap()
{
  switch(colormapType){ // the default colorMap is the gray one
  case(1) : colorMap = new_cmap_cool(nbColors);break;
  case(2) : colorMap = new_cmap_copper(nbColors);break;
  case(3) : colorMap = new_cmap_hot(nbColors);break;
  case(4) : colorMap = new_cmap_bone(nbColors);break;
  case(5) : colorMap = new_cmap_pink(nbColors);break;
  case(6) : colorMap = new_cmap_jet(nbColors);break;
  default : colorMap = new_cmap_gray(nbColors);break;
  }
}
