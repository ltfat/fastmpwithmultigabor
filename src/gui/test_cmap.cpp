#include "MptkGuiColormaps.h"

/*****************/
/* Write on disk */
unsigned int dump_cmap( MP_Colormap_t **cmap, unsigned int size, char *fName ) {

  FILE *fid;
  unsigned int nWrite;
  
  if ( (fid = fopen( fName, "w" )) == NULL ) {
    fprintf( stderr, "Can't open file [%s] to dump a colormap.\n", fName );
    return( 0 );
  }
  nWrite = fwrite( *cmap, sizeof(MP_Colormap_t), size*3, fid );
  fclose( fid );
  
  return( nWrite );
}

/********/
/* MAIN */
int main( void ) {

#define CMAP_SIZE 67

  MP_Colormap_t** cmap;

  cmap = new_cmap_gray( CMAP_SIZE );
  dump_cmap( cmap, CMAP_SIZE, "colormap_gray.cmp" );
  delete_cmap( cmap );

  cmap = new_cmap_copper( CMAP_SIZE );
  dump_cmap( cmap, CMAP_SIZE, "colormap_copper.cmp" );
  delete_cmap( cmap );

  cmap = new_cmap_hot( CMAP_SIZE );
  dump_cmap( cmap, CMAP_SIZE, "colormap_hot.cmp" );
  delete_cmap( cmap );

  cmap = new_cmap_jet( CMAP_SIZE );
  dump_cmap( cmap, CMAP_SIZE, "colormap_jet.cmp" );

  revert_cmap( cmap, CMAP_SIZE );
  dump_cmap( cmap, CMAP_SIZE, "colormap_jet_rev.cmp" );

  delete_cmap( cmap );

  cmap = new_cmap_bone( CMAP_SIZE );
  dump_cmap( cmap, CMAP_SIZE, "colormap_bone.cmp" );
  delete_cmap( cmap );

  cmap = new_cmap_pink( CMAP_SIZE );
  dump_cmap( cmap, CMAP_SIZE, "colormap_pink.cmp" );
  delete_cmap( cmap );

  cmap = new_cmap_cool( CMAP_SIZE );
  dump_cmap( cmap, CMAP_SIZE, "colormap_cool.cmp" );
  delete_cmap( cmap );

  MptkGuiColormaps* map1 = new MptkGuiColormaps(COLORS16BITS,COPPER,0.1,0.5,LINEAR);
  MptkGuiColormaps* map2 = new MptkGuiColormaps(COLORS16BITS,COPPER,0.1,0.5,LOGARITHMIC);
  MP_Colormap_t* val;
  for(MP_Real_t i=0;i<0.6;i=i+0.01)
    {
      val = map1->give_color(i);
      printf("[%lf,%lf,%lf]   ||   ",val[0],val[1],val[2]);
      val = map2->give_color(i);
      printf("[%lf,%lf,%lf]\n",val[0],val[1],val[2]);
    }
  return( 0 );
}
