/******************************************************************************/
/*                                                                            */
/*                                mpview.cpp                                  */
/*                                                                            */
/*                        Matching Pursuit Utilities                          */
/*                                                                            */
/* Rémi Gribonval                                                             */
/* Sacha Krstulovic                                           Mon Feb 21 2005 */
/* -------------------------------------------------------------------------- */
/*                                                                            */
/*  Copyright (C) 2005 IRISA                                                  */
/*                                                                            */
/*  This program is free software; you can redistribute it and/or             */
/*  modify it under the terms of the GNU General Public License               */
/*  as published by the Free Software Foundation; either version 2            */
/*  of the License, or (at your option) any later version.                    */
/*                                                                            */
/*  This program is distributed in the hope that it will be useful,           */
/*  but WITHOUT ANY WARRANTY; without even the implied warranty of            */
/*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             */
/*  GNU General Public License for more details.                              */
/*                                                                            */
/*  You should have received a copy of the GNU General Public License         */
/*  along with this program; if not, write to the Free Software               */
/*  Foundation, Inc., 59 Temple Place - Suite 330,                            */
/*  Boston, MA  02111-1307, USA.                                              */
/*                                                                            */
/******************************************************************************/
/*
 * CVS log:
 *
 * $Author: sacha $
 * $Date: 2005/07/04 09:55:32 $
 * $Revision: 1.4 $
 *
 */

#include <mptk.h>

#include <stdio.h>
#include <string.h>

int main( int argc, char **argv ) {

  MP_Book_c book;
  int numcols,numrows;

  /* print usage if necessary */
  if (argc < 5 || !strcmp(argv[1],"--help") || !strcmp(argv[1],"-h")) {
    fprintf(stderr,"usage : %s (IN|-) numCols numRows OUT.flt\n",argv[0]);
    fprintf(stderr,"displays a book in a pixmap of numCols x numRows pixels\n");
    fprintf(stderr,"Returns nonzero in case of failure, zero otherwise\n");
    return(1);
  }

  /* 1) Load the book */
  if (!strcmp(argv[1],"-")) {
    if (book.load(stdin)==0) return(1);
  }
  else {
    if (book.load(argv[1])==0) return(1);
  }

  /* 2) Read the size of the pixmap */
  if (sscanf(argv[2],"%d",&numcols)==0 || sscanf(argv[3],"%d",&numrows)==0) return(1);

  
  {
    MP_TF_Map_c* tfmap = new MP_TF_Map_c(numcols,numrows,book.numChans,0.0,0.0,book.numSamples,0.5);
    /* 3) Display the book */
    book.add_to_tfmap(tfmap,NULL);
    /* 4) Save the pixmap */
    tfmap->dump_to_float_file(argv[4],1);
    /* Clean the house */
    delete tfmap;
  }

  return(0);
}
