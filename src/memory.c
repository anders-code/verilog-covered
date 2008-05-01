/*
 Copyright (c) 2006 Trevor Williams

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by the Free Software
 Foundation; either version 2 of the License, or (at your option) any later version.

 This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 See the GNU General Public License for more details.

 You should have received a copy of the GNU General Public License along with this program;
 if not, write to the Free Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

/*!
 \file     memory.c
 \author   Trevor Williams  (phase1geo@gmail.com)
 \date     9/24/2006
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "db.h"
#include "defines.h"
#include "func_unit.h"
#include "link.h"
#include "memory.h"
#include "obfuscate.h"
#include "ovl.h"
#include "vector.h"
#include "vsignal.h"
#include "util.h"


extern db**         db_list;
extern unsigned int curr_db;

extern bool   report_covered;
extern bool   report_instance;
extern char** leading_hierarchies;
extern int    leading_hier_num;
extern bool   leading_hiers_differ;
extern isuppl info_suppl;


/*!
 \param sig          Pointer to signal list to traverse for memories
 \param ae_total     Pointer to total number of addressable elements
 \param wr_hit       Pointer to total number of addressable elements written
 \param rd_hit       Pointer to total number of addressable elements read
 \param tog_total    Pointer to total number of bits in memories that can be toggled
 \param tog01_hit    Pointer to total number of bits toggling from 0->1
 \param tog10_hit    Pointer to total number of bits toggling from 1->0
 \param ignore_excl  If set to TRUE, ignores the current value of the excluded bit

 Calculates the total and hit memory coverage information for the given memory signal.
*/
void memory_get_stat(
  vsignal* sig,
  int*     ae_total,
  int*     wr_hit,
  int*     rd_hit,
  int*     tog_total,
  int*     tog01_hit,
  int*     tog10_hit,
  bool     ignore_excl
) { PROFILE(MEMORY_GET_STAT);

  int    i;       /* Loop iterator */
  int    wr;      /* Number of bits written within an addressable element */
  int    rd;      /* Number of bits read within an addressable element */
  vector vec;     /* Temporary vector used for storing one addressable element */
  int    pwidth;  /* Width of packed portion of memory */

  /* Calculate width of smallest addressable element */
  pwidth = 1;
  for( i=(sig->udim_num); i<(sig->udim_num + sig->pdim_num); i++ ) {
    if( sig->dim[i].msb > sig->dim[i].lsb ) {
      pwidth *= (sig->dim[i].msb - sig->dim[i].lsb) + 1;
    } else {
      pwidth *= (sig->dim[i].lsb - sig->dim[i].msb) + 1;
    }
  }

  /* Calculate total number of addressable elements and their write/read information */
  for( i=0; i<sig->value->width; i+=pwidth ) {
    if( (sig->suppl.part.excluded == 1) && !ignore_excl ) {
      (*wr_hit)++;
      (*rd_hit)++;
    } else {
      wr = 0;
      rd = 0;
      vector_mem_rw_count( sig->value, &wr, &rd );
      if( wr > 0 ) {
        (*wr_hit)++;
      }
      if( rd > 0 ) {
        (*rd_hit)++;
      }
    }
    (*ae_total)++;
  }

  /* Calculate toggle coverage information for the memory */
  *tog_total += sig->value->width;
  if( (sig->suppl.part.excluded == 1) && !ignore_excl ) {
    *tog01_hit += sig->value->width;
    *tog10_hit += sig->value->width;
  } else {
    vector_toggle_count( sig->value, tog01_hit, tog10_hit );
  }

}

/*!
 \param sigl       Pointer to signal list to traverse for memories
 \param ae_total   Pointer to total number of addressable elements
 \param wr_hit     Pointer to total number of addressable elements written
 \param rd_hit     Pointer to total number of addressable elements read
 \param tog_total  Pointer to total number of bits in memories that can be toggled
 \param tog01_hit  Pointer to total number of bits toggling from 0->1
 \param tog10_hit  Pointer to total number of bits toggling from 1->0
*/
void memory_get_stats(
            sig_link* sigl,
  /*@out@*/ int*      ae_total,
  /*@out@*/ int*      wr_hit,
  /*@out@*/ int*      rd_hit,
  /*@out@*/ int*      tog_total,
  /*@out@*/ int*      tog01_hit,
  /*@out@*/ int*      tog10_hit
) { PROFILE(MEMORY_GET_STATS);

  while( sigl != NULL ) {

    /* Calculate only for memory elements (must contain one or more unpacked dimensions) */
    if( (sigl->sig->suppl.part.type == SSUPPL_TYPE_MEM) && (sigl->sig->udim_num > 0) ) {

      memory_get_stat( sigl->sig, ae_total, wr_hit, rd_hit, tog_total, tog01_hit, tog10_hit, FALSE );

    }

    sigl = sigl->next;

  }

}

/*!
 \param funit_name  Name of functional unit to find memories in
 \param funit_type  Type of functional unit to find memories in
 \param total       Pointer to total number of memories in the given functional unit
 \param hit         Pointer to number of memories that received 100% coverage of all memory metrics

 \return Returns TRUE if we were able to determine the summary information properly; otherwise,
         returns FALSE to indicate that an error occurred.

 Retrieves memory summary information for a given functional unit made by a GUI request.
*/
bool memory_get_funit_summary( const char* funit_name, int funit_type, int* total, int* hit ) { PROFILE(MEMORY_GET_FUNIT_SUMMARY);

  bool        retval    = FALSE;  /* Return value for this function */
  funit_link* funitl;             /* Pointer to functional unit link containing the requested functional unit */
  sig_link*   sigl;               /* Pointer to current signal link in list */
  int         ae_total  = 0;      /* Total number of addressable elements */
  int         wr_hit    = 0;      /* Number of addressable elements written */
  int         rd_hit    = 0;      /* Number of addressable elements read */
  int         tog_total = 0;      /* Total number of toggle bits */
  int         tog01_hit = 0;      /* Number of bits toggling from 0->1 */
  int         tog10_hit = 0;      /* Number of bits toggling from 1->0 */

  if( (funitl = funit_link_find( funit_name, funit_type, db_list[curr_db]->funit_head )) != NULL ) {
  
    /* Initialize total and hit information */
    *total = 0;
    *hit   = 0;

    retval = TRUE;
    sigl   = funitl->funit->sig_head;

    while( sigl != NULL ) {

      /* Only evaluate a signal if it is a memory type */
      if( sigl->sig->suppl.part.type == SSUPPL_TYPE_MEM ) {

        /* Increment the total number */
        (*total)++;

        /* Figure out if this signal is 100% covered in memory coverage */
        memory_get_stat( sigl->sig, &ae_total, &wr_hit, &rd_hit, &tog_total, &tog01_hit, &tog10_hit, FALSE );

        if( (wr_hit > 0) && (rd_hit > 0) && (tog01_hit == tog_total) && (tog10_hit == tog_total) ) {
          (*hit)++;
        } 

      }

      sigl = sigl->next;

    }
    
  }

  return( retval );

}

/*!
 \param str     Pointer to string array to populate with packed dimension information
 \param sig     Pointer to signal that we are solving for
 \param prefix  Prefix string to append to the beginning of the newly created string
 \param dim     Current dimension to solve for

 Creates a string array for each bit in the given signal corresponding to its position
 in the packed array portion.
*/
static void memory_create_pdim_bit_array( char** str, vsignal* sig, char* prefix, int dim ) { PROFILE(MEMORY_CREATE_PDIM_BIT_ARRAY);

  char name[4096];  /* Temporary string */
  int  i;           /* Loop iterator */
  bool last_dim;    /* Specifies if this is the final dimension */

  /* Calculate final dimension */
  last_dim = (dim + 1) == (sig->pdim_num + sig->udim_num);

  if( sig->dim[dim].msb > sig->dim[dim].lsb ) {

    for( i=sig->dim[dim].lsb; i<=sig->dim[dim].msb; i++ ) {
      if( last_dim ) {
        unsigned int rv = snprintf( name, 4096, "%d", i );
        assert( rv < 4096 );
        *str = (char*)realloc_safe( *str, (strlen( *str ) + 1), (strlen( *str ) + strlen( prefix ) + strlen( name ) + 4) );
        strcat( *str, prefix );
        strcat( *str, "[" );
        strcat( *str, name );
        strcat( *str, "] " );
      } else {
        unsigned int rv = snprintf( name, 4096, "%s[%d]", prefix, i );
        assert( rv < 4096 );
        memory_create_pdim_bit_array( str, sig, name, (dim + 1) );
      }
    }

  } else {

    for( i=sig->dim[dim].lsb; i>=sig->dim[dim].msb; i-- ) {
      if( last_dim ) {
        unsigned int rv = snprintf( name, 4096, "%d", i );
        assert( rv < 4096 );
        *str = (char*)realloc_safe( *str, (strlen( *str ) + 1), (strlen( *str ) + strlen( prefix ) + strlen( name ) + 4) );
        strcat( *str, prefix );
        strcat( *str, "[" );
        strcat( *str, name );
        strcat( *str, "] " );
      } else {
        unsigned int rv = snprintf( name, 4096, "%s[%d]", prefix, i );
        assert( rv < 4096 );
        memory_create_pdim_bit_array( str, sig, name, (dim + 1) );
      }
    }

  }

}

/*!
 Creates memory array structure for Tcl and stores it into the mem_str parameter.
*/
static void memory_get_mem_coverage(
  char**    mem_str,          /*!< String containing memory information */
  vsignal*  sig,              /*!< Pointer to signal to get memory coverage for */
  int       offset,           /*!< Bit offset of signal vector to start retrieving information from */
  char*     prefix,           /*!< String containing memory prefix to output (initially this will be just the signal name) */
  int       dim,              /*!< Current dimension index (initially this will be 0) */
  int       parent_dim_width  /*!< Bit width of parent dimension (initially this will be the width of the signal) */
) { PROFILE(MEMORY_GET_MEM_COVERAGE);

  char name[4096];  /* Contains signal name */
  int  msb;         /* MSB of current dimension */
  int  lsb;         /* LSB of current dimension */
  int  be;          /* Big endianness of current dimension */
  int  i;           /* Loop iterator */
  int  dim_width;   /* Bit width of current dimension */

  assert( sig != NULL );
  assert( prefix != NULL );
  assert( dim < sig->udim_num );

  /* Calculate MSB, LSB and big endianness of current dimension */
  if( sig->dim[dim].msb > sig->dim[dim].lsb ) {
    msb = sig->dim[dim].msb;
    lsb = sig->dim[dim].lsb;
    be  = FALSE;
  } else {
    msb = sig->dim[dim].lsb;
    lsb = sig->dim[dim].msb;
    be  = TRUE;
  }

  /* Calculate current dimensional width */
  dim_width = parent_dim_width / ((msb - lsb) + 1);

  /* Only output memory contents if we have reached the lowest dimension */
  if( (dim + 1) == sig->udim_num ) {

    vector* vec = vector_create( dim_width, VTYPE_MEM, VDATA_U32, TRUE );
    int     tog01;
    int     tog10;
    int     wr;
    int     rd;
    char*   tog01_str;
    char*   tog10_str;
    char    hit_str[2];
    char    int_str[20];
    char*   dim_str;
    char*   entry_str;

    /* Iterate through each addressable element in the current dimension */
    for( i=0; i<((msb - lsb) + 1); i++ ) {

      unsigned int rv;
      unsigned int slen;

      /* Re-initialize the vector */
      if( be ) {
        vector_copy_range( vec, sig->value, ((dim_width * ((msb - lsb) - i)) + offset) );
      } else {
        vector_copy_range( vec, sig->value, ((dim_width * i) + offset) );
      }

      /* Create dimension string */
      rv = snprintf( int_str, 20, "%d", i );
      assert( rv < 20 );
      slen    = strlen( prefix ) + strlen( int_str ) + 5;
      dim_str = (char*)malloc_safe( slen );
      rv = snprintf( dim_str, slen, "%s\\[%d\\]", prefix, i );
      assert( rv < slen );

      /* Get toggle information */
      tog01 = 0;
      tog10 = 0;
      vector_toggle_count( vec, &tog01, &tog10 );

      /* Get toggle strings */
      tog01_str = vector_get_toggle01_uint32( vec->value.u32, vec->width );
      tog10_str = vector_get_toggle10_uint32( vec->value.u32, vec->width );

      /* Get write/read information */
      wr = 0;
      rd = 0;
      vector_mem_rw_count( vec, &wr, &rd );

      /* Output the addressable memory element if it is found to be lacking in coverage */
      if( (tog01 < dim_width) || (tog10 < dim_width) || (wr == 0) || (rd == 0) ) {
        strcpy( hit_str, "0" );
      } else {
        strcpy( hit_str, "1" );
      }

      /* Create a string list for this entry */
      slen      = strlen( dim_str ) + strlen( hit_str ) + strlen( tog01_str ) + strlen( tog10_str ) + 10;
      entry_str = (char*)malloc_safe( slen );
      rv = snprintf( entry_str, slen, "{%s %s %d %d %s %s}",
                dim_str, hit_str, ((wr == 0) ? 0 : 1), ((rd == 0) ? 0 : 1), tog01_str, tog10_str );
      assert( rv < slen );

      *mem_str = (char*)realloc_safe( *mem_str, (strlen( *mem_str ) + 1), (strlen( *mem_str ) + strlen( entry_str ) + 2) );
      strcat( *mem_str, " " );
      strcat( *mem_str, entry_str );

      /* Deallocate memory */
      free_safe( dim_str, (strlen( dim_str ) + 1) );
      free_safe( tog01_str, (strlen( tog01_str ) + 1) );
      free_safe( tog10_str, (strlen( tog10_str ) + 1) );
      free_safe( entry_str, (strlen( entry_str ) + 1) );

    }

    /* Deallocate vector */
    vector_dealloc( vec );

  /* Otherwise, go down one level */
  } else {

    /* Iterate through each entry in the current dimesion */
    for( i=0; i<((msb - lsb) + 1); i++ ) {

      /* Create new prefix */
      unsigned int rv = snprintf( name, 4096, "%s[%d]", prefix, i );
      assert( rv < 4096 );

      if( be ) {
        memory_get_mem_coverage( mem_str, sig, (offset + (dim_width * ((msb - lsb) - i))), name, (dim + 1), dim_width );
      } else {
        memory_get_mem_coverage( mem_str, sig, (offset + (dim_width * i)),                 name, (dim + 1), dim_width );
      }

    }

  }

}

/*!
 \param funit_name   Name of functional unit containing the given signal
 \param funit_type   Type of functional unit containing the given signal
 \param signame      Name of signal to find memory coverage information for
 \param pdim_str     Pointer to string to store packed dimensional information
 \param pdim_array   Pointer to string to store packed dimensional array
 \param udim_str     Pointer to string to store unpacked dimensional information
 \param memory_info  Pointer to string to store memory information into
 \param excluded     Pointer to excluded indicator to store

 \return Returns TRUE if the given signal was found and its memory coverage information properly
         stored in the given pointers; otherwise, returns FALSE to indicate that an error occurred.

 Retrieves memory coverage information for the given signal in the specified functional unit.
*/
bool memory_get_coverage(
            const char* funit_name,
            int         funit_type,
            const char* signame,
  /*@out@*/ char**      pdim_str,
  /*@out@*/ char**      pdim_array,
  /*@out@*/ char**      udim_str,
  /*@out@*/ char**      memory_info,
  /*@out@*/ int*        excluded
) { PROFILE(MEMORY_GET_COVERAGE);

  bool        retval = FALSE;  /* Return value for this function */
  funit_link* funitl;          /* Pointer to found functional unit link */
  vsignal     sig;             /* Signal container used for searching */
  sig_link*   sigl;            /* Pointer to found signal link */
  int         i;               /* Loop iterator */
  char        tmp1[20];        /* Temporary string holder */
  char        tmp2[20];        /* Temporary string holder */

  if( (funitl = funit_link_find( funit_name, funit_type, db_list[curr_db]->funit_head )) != NULL ) {

    if( (sigl = sig_link_find( signame, funitl->funit->sig_head )) != NULL ) {

      unsigned int rv;

      /* Allocate and populate the pdim_array and pdim_width parameters */
      *pdim_array = (char*)malloc_safe( 1 );
      (*pdim_array)[0] = '\0';
      memory_create_pdim_bit_array( pdim_array, sigl->sig, "", sigl->sig->pdim_num );

      /* Allocate and populate the pdim_str string */
      *pdim_str = NULL;
      for( i=sigl->sig->udim_num; i<(sigl->sig->pdim_num + sigl->sig->udim_num); i++ ) {
        unsigned int slen;
        rv = snprintf( tmp1, 20, "%d", sigl->sig->dim[i].msb );
        assert( rv < 20 );
        rv = snprintf( tmp2, 20, "%d", sigl->sig->dim[i].lsb );
        assert( rv < 20 );
        slen = strlen( tmp1 ) + strlen( tmp2 ) + 4;
        *pdim_str = (char*)realloc_safe( *pdim_str, (strlen( *pdim_str ) + 1), slen );
        if( i == sigl->sig->udim_num ) {
          rv = snprintf( *pdim_str, slen, "[%s:%s]", tmp1, tmp2 );
          assert( rv < slen );
        } else {
          strcat( *pdim_str, "[" );
          strcat( *pdim_str, tmp1 );
          strcat( *pdim_str, ":" );
          strcat( *pdim_str, tmp2 );
          strcat( *pdim_str, "]" );
        }
      }

      /* Allocate and populate the udim_info string */
      *udim_str = NULL;
      for( i=0; i<sigl->sig->udim_num; i++ ) {
        unsigned int slen;
        rv = snprintf( tmp1, 20, "%d", sigl->sig->dim[i].msb );
        assert( rv < 20 );
        rv = snprintf( tmp2, 20, "%d", sigl->sig->dim[i].lsb );
        assert( rv < 20 );
        slen = strlen( tmp1 ) + strlen( tmp2 ) + 4;
        *udim_str = (char*)realloc_safe( *udim_str, (strlen( *udim_str ) + 1), slen );
        if( i == 0 ) {
          rv = snprintf( *udim_str, slen, "[%s:%s]", tmp1, tmp2 );
          assert( rv < slen );
        } else {
          strcat( *udim_str, "[" );
          strcat( *udim_str, tmp1 );
          strcat( *udim_str, ":" );
          strcat( *udim_str, tmp2 );
          strcat( *udim_str, "]" );
        }
      }

      /* Populate the memory_info string */
      *memory_info = (char*)malloc_safe( 1 );
      (*memory_info)[0] = '\0';
      memory_get_mem_coverage( memory_info, sigl->sig, 0, "", 0, sigl->sig->value->width );

      /* Populate the excluded value */
      *excluded = sigl->sig->suppl.part.excluded;
      
      retval = TRUE;

    }

  }

  return( retval );

}

/*!
 \param funit_name  Name of functional unit to collect memories for
 \param funit_type  Type of functional unit to collect memories for
 \param cov         Set to 0 to get uncovered memories or 1 to get covered memories
 \param head        Pointer to head of signal list containing retrieved signals
 \param tail        Pointer to tail of signal list containing retrieved signals

 \return Returns TRUE if we were able to find the given functional unit; otherwise, returns
         FALSE to indicate that an error occurred.

 Collects all signals that are memories and match the given coverage type and stores them
 in the given signal list.
*/
bool memory_collect(
  const char* funit_name,
  int         funit_type,
  int         cov,
  sig_link**  head,
  sig_link**  tail
) { PROFILE(MEMORY_COLLECT);

  bool        retval = FALSE;  /* Return value for this function */
  funit_link* funitl;          /* Pointer to found functional unit link */
  sig_link*   sigl;            /* Pointer to current signal link being evaluated */
  int         ae_total  = 0;   /* Total number of addressable elements */
  int         wr_hit    = 0;   /* Total number of addressable elements written */
  int         rd_hit    = 0;   /* Total number of addressable elements read */
  int         tog_total = 0;   /* Total number of toggle bits */
  int         hit01     = 0;   /* Number of bits that toggled from 0 to 1 */
  int         hit10     = 0;   /* Number of bits that toggled from 1 to 0 */

  if( (funitl = funit_link_find( funit_name, funit_type, db_list[curr_db]->funit_head )) != NULL ) {

    sigl = funitl->funit->sig_head;
    retval = TRUE;

    while( sigl != NULL ) {

      if( sigl->sig->suppl.part.type == SSUPPL_TYPE_MEM ) {

        ae_total = 0;
   
        memory_get_stat( sigl->sig, &ae_total, &wr_hit, &rd_hit, &tog_total, &hit01, &hit10, TRUE );

        /* If this signal meets the coverage requirement, add it to the signal list */
        if( ((cov == 1) && (wr_hit > 0) && (rd_hit > 0) && (hit01 == tog_total) && (hit10 == tog_total)) ||
            ((cov == 0) && ((wr_hit == 0) || (rd_hit == 0) || (hit01 < tog_total) || (hit10 < tog_total))) ) {

          sig_link_add( sigl->sig, head, tail );

        }

      }

      sigl = sigl->next;

    }

  }


  return( retval );

}

/*!
 \param ofile   Pointer to file to display coverage results to.
 \param name    Name of instance being reported
 \param hits01  Pointer to number of bits in memory toggled from 0 -> 1.
 \param hits10  Pointer to number of bits in memory toggled from 1 -> 0.
 \param total   Pointer to total number of bits in memories.

 \return Returns TRUE if any bits were found to be untoggled; otherwise, returns FALSE.

 Calculates and outputs the memory toggle summary coverage results for a given instance.
*/
static bool memory_display_toggle_instance_summary(
  FILE*       ofile,
  const char* name,
  int         hits01,
  int         hits10,
  int         total
) { PROFILE(MEMORY_DISPLAY_TOGGLE_INSTANCE_SUMMARY);

  float percent01;    /* Percentage of bits toggling from 0 -> 1 */
  float percent10;    /* Percentage of bits toggling from 1 -> 0 */
  int   miss01  = 0;  /* Number of bits that did not toggle from 0 -> 1 */
  int   miss10  = 0;  /* Number of bits that did not toggle from 1 -> 0 */

  calc_miss_percent( hits01, total, &miss01, &percent01 );
  calc_miss_percent( hits10, total, &miss10, &percent10 );

  /* Output toggle information */
  fprintf( ofile, "  %-43.43s    %5d/%5d/%5d      %3.0f%%         %5d/%5d/%5d      %3.0f%%\n",
           name, hits01, miss01, total, percent01, hits10, miss10, total, percent10 );

  return( (miss01 > 0) || (miss10 > 0) );

}

/*!
 \param ofile        File to output coverage information to.
 \param root         Instance node in the functional unit instance tree being evaluated.
 \param parent_inst  Name of parent instance.
 \param hits01       Pointer to accumulated toggle 0 -> 1 hit count
 \param hits10       Pointer to accumulated toggle 1 -> 0 hit count
 \param total        Pointer to total number of memory bits

 \return Returns TRUE if any bits were found to be not toggled; otherwise, returns FALSE.

 Displays the memory instance summarization to the specified file.  Recursively
 iterates through functional unit instance tree, outputting the toggle information that
 is found at that instance.
*/
static bool memory_toggle_instance_summary(
            FILE*       ofile,
            funit_inst* root,
            char*       parent_inst,
  /*@out@*/ int*        hits01,
  /*@out@*/ int*        hits10,
  /*@out@*/ int*        total
) { PROFILE(MEMORY_TOGGLE_INSTANCE_SUMMARY);

  funit_inst* curr;                /* Pointer to current child functional unit instance of this node */
  char        tmpname[4096];       /* Temporary name holder for instance */
  char*       pname;               /* Printable version of instance name */
  bool        miss_found = FALSE;  /* Set to TRUE if at least one bit is found to not be toggled */

  assert( root != NULL );
  assert( root->stat != NULL );

  /* Get printable version of this instance */
  pname = scope_gen_printable( root->name );
  if( db_is_unnamed_scope( pname ) ) {
    strcpy( tmpname, parent_inst );
  } else if( strcmp( parent_inst, "*" ) == 0 ) {
    strcpy( tmpname, pname );
  } else {
    unsigned int rv = snprintf( tmpname, 4096, "%s.%s", parent_inst, pname );
    assert( rv < 4096 );
  }

  free_safe( pname, (strlen( pname ) + 1) );

  if( root->stat->show && !funit_is_unnamed( root->funit ) &&
      ((info_suppl.part.assert_ovl == 0) || !ovl_is_assertion_module( root->funit )) ) {

    miss_found |= memory_display_toggle_instance_summary( ofile, tmpname, root->stat->mem_tog01_hit, root->stat->mem_tog10_hit, root->stat->mem_tog_total );

    /* Update accumulated coverage information */
    *hits01 += root->stat->mem_tog01_hit;
    *hits10 += root->stat->mem_tog10_hit;
    *total  += root->stat->mem_tog_total;

  } 

  /* If this is an assertion module, don't output any further */
  if( (info_suppl.part.assert_ovl == 0) || !ovl_is_assertion_module( root->funit ) ) {

    curr = root->child_head;
    while( curr != NULL ) {
      miss_found |= memory_toggle_instance_summary( ofile, curr, tmpname, hits01, hits10, total );
      curr = curr->next;
    }

  }

  return( miss_found );

}

/*!
 \param ofile   Pointer to file to output coverage summary information to
 \param name    Name of instance being displayed
 \param wr_hit  Number of addressable elements that were written
 \param rd_hit  Number of addressable elements that were read
 \param total   Number of all addressable elements

 \return Returns TRUE if at least one miss was found; otherwise, returns FALSE.

 Calculates the miss and hit percentage statistics for the given instance and outputs this information
 to the given output file.
*/
static bool memory_display_ae_instance_summary(
  FILE* ofile,
  char* name,
  int   wr_hit,
  int   rd_hit,
  int   total
) { PROFILE(MEMORY_DISPLAY_AE_INSTANCE_SUMMARY);

  float wr_percent;  /* Percentage of addressable elements written */
  float rd_percent;  /* Percentage of addressable elements read */
  int   wr_miss;     /* Number of addressable elements that were not written */
  int   rd_miss;     /* Number of addressable elements that were not read */

  calc_miss_percent( wr_hit, total, &wr_miss, &wr_percent );
  calc_miss_percent( rd_hit, total, &rd_miss, &rd_percent );

  /* Output toggle information */
  fprintf( ofile, "  %-43.43s    %5d/%5d/%5d      %3.0f%%         %5d/%5d/%5d      %3.0f%%\n",
           name, wr_hit, wr_miss, total, wr_percent, rd_hit, rd_miss, total, rd_percent );

  return( (wr_miss > 0) || (rd_miss > 0) );

}

/*!
 \param ofile        File to output coverage information to.
 \param root         Instance node in the functional unit instance tree being evaluated.
 \param parent_inst  Name of parent instance.
 \param wr_hits      Pointer to accumulated number of addressable elements written
 \param rd_hits      Pointer to accumulated number of addressable elements read
 \param total        Pointer to the total number of addressable elements

 \return Returns TRUE if any bits were found to be not toggled; otherwise, returns FALSE.

 Displays the memory instance summarization to the specified file.  Recursively
 iterates through functional unit instance tree, outputting the toggle information that
 is found at that instance.
*/
static bool memory_ae_instance_summary(
            FILE*       ofile,
            funit_inst* root,
            char*       parent_inst,
  /*@out@*/ int*        wr_hits,
  /*@out@*/ int*        rd_hits,
  /*@out@*/ int*        total
) { PROFILE(MEMORY_AE_INSTANCE_SUMMARY);

  funit_inst* curr;                /* Pointer to current child functional unit instance of this node */
  char        tmpname[4096];       /* Temporary name holder for instance */
  char*       pname;               /* Printable version of instance name */
  bool        miss_found = FALSE;  /* Set to true if a coverage type was missed */

  assert( root != NULL );
  assert( root->stat != NULL );

  /* Get printable version of this instance */
  pname = scope_gen_printable( root->name );
  if( db_is_unnamed_scope( pname ) ) {
    strcpy( tmpname, parent_inst );
  } else if( strcmp( parent_inst, "*" ) == 0 ) {
    strcpy( tmpname, pname );
  } else {
    unsigned int rv = snprintf( tmpname, 4096, "%s.%s", parent_inst, pname );
    assert( rv < 4096 );
  }

  free_safe( pname, (strlen( pname ) + 1) );

  if( root->stat->show && !funit_is_unnamed( root->funit ) &&
      ((info_suppl.part.assert_ovl == 0) || !ovl_is_assertion_module( root->funit )) ) {

    miss_found |= memory_display_ae_instance_summary( ofile, tmpname, root->stat->mem_wr_hit, root->stat->mem_rd_hit, root->stat->mem_ae_total );

    /* Update accumulated stats */
    *wr_hits += root->stat->mem_wr_hit;
    *rd_hits += root->stat->mem_rd_hit;
    *total   += root->stat->mem_ae_total;

  } 

  /* If this is an assertion module, don't output any further */
  if( (info_suppl.part.assert_ovl == 0) || !ovl_is_assertion_module( root->funit ) ) {

    curr = root->child_head;
    while( curr != NULL ) {
      miss_found |= memory_ae_instance_summary( ofile, curr, tmpname, wr_hits, rd_hits, total );
      curr = curr->next;
    }

  }

  return( miss_found );

}

/*!
 \param ofile  Pointer to file to output functional unit summary results
 \param name   Name of functional unit being reported
 \param fname  Filename containing the functional unit being reported
 \param hit01  Number of memory bits that transitioned from a value of 0 to a value of 1 during simulation for this functional unit
 \param hit10  Number of memory bits that transitioned from a value of 1 to a value of 0 during simulation for this functional unit
 \param total  Number of total memory bits in the given functional unit

 \return Returns TRUE if at least one bit was found to not have transitioned during simulation.

 Calculates and outputs the toggle coverage for all memories in the given functional unit.
*/
static bool memory_display_toggle_funit_summary(
  FILE*       ofile,
  const char* name,
  const char* fname,
  int         hit01,
  int         hit10,
  int         total
) { PROFILE(MEMORY_DISPLAY_TOGGLE_FUNIT_SUMMARY);

  float percent01;  /* Percentage of bits that toggled from 0 to 1 */
  float percent10;  /* Percentage of bits that toggled from 1 to 0 */
  int   miss01;     /* Number of bits that did not toggle from 0 to 1 */
  int   miss10;     /* Number of bits that did not toggle from 1 to 0 */

  calc_miss_percent( hit01, total, &miss01, &percent01 );
  calc_miss_percent( hit10, total, &miss10, &percent10 );

  fprintf( ofile, "  %-20.20s    %-20.20s   %5d/%5d/%5d      %3.0f%%         %5d/%5d/%5d      %3.0f%%\n",
           name, fname, hit01, miss01, total, percent01, hit10, miss10, total, percent10 );

  return( (miss01 > 0) || (miss10 > 0) );

}

/*!
 \param ofile   Pointer to file to display coverage results to.
 \param head    Pointer to head of functional unit list to parse.
 \param hits01  Pointer to number of bits in memory toggled from 0 -> 1.
 \param hits10  Pointer to number of bits in memory toggled from 1 -> 0.
 \param total   Pointer to total number of bits in memories.

 \return Returns TRUE if any bits were found to be untoggled; otherwise, returns FALSE.

 Iterates through the functional unit list displaying the memory toggle coverage summary for
 each functional unit.
*/
static bool memory_toggle_funit_summary(
            FILE*       ofile,
            funit_link* head,
  /*@out@*/ int*        hits01,
  /*@out@*/ int*        hits10,
  /*@out@*/ int*        total
) { PROFILE(MEMORY_TOGGLE_FUNIT_SUMMARY);

  bool  miss_found = FALSE;  /* Set to TRUE if missing toggles were found */
  char* pname;               /* Printable version of the functional unit name */

  while( head != NULL ) {

    /* If this is an assertion module, don't output any further */
    if( head->funit->stat->show && !funit_is_unnamed( head->funit ) &&
        ((info_suppl.part.assert_ovl == 0) || !ovl_is_assertion_module( head->funit )) ) {

      /* Get printable version of functional unit name */
      pname = scope_gen_printable( funit_flatten_name( head->funit ) );

      miss_found |= memory_display_toggle_funit_summary( ofile, pname, get_basename( obf_file( head->funit->filename ) ),
                                                         head->funit->stat->mem_tog01_hit, head->funit->stat->mem_tog10_hit, head->funit->stat->mem_tog_total );

      /* Update accumulated information */
      *hits01 += head->funit->stat->mem_tog01_hit;
      *hits10 += head->funit->stat->mem_tog10_hit;
      *total  += head->funit->stat->mem_tog_total;

      free_safe( pname, (strlen( pname ) + 1) );

    }

    head = head->next;

  }

  return( miss_found );

}

/*!
 \param ofile    Pointer to output file that will be written
 \param name     Name of functional unit being reported
 \param fname    Filename containing the given functional unit being reported
 \param wr_hits  Number of addressable memory elements that were written in this functional unit during simulation
 \param rd_hits  Number of addressable memory elements that were read in this functional unit during simulation
 \param total    Number of addressable memory elements in the given functional unit

 \return Returns TRUE if at least one addressable memory element was not written or read during simulation; otherwise,
         returns FALSE.

 Calculates and outputs the summary addressable memory element coverage information for a given functional unit.
*/
static bool memory_display_ae_funit_summary(
  FILE*       ofile,
  const char* name,
  const char* fname,
  int         wr_hits,
  int         rd_hits,
  int         total
) { PROFILE(MEMORY_DISPLAY_AE_FUNIT_SUMMARY);

  float wr_percent;  /* Percentage of addressable elements that were written */
  float rd_percent;  /* Percentage of addressable elements that were read */
  int   wr_miss;     /* Number of addressable elements that were not written */
  int   rd_miss;     /* Number of addressable elements that were not read */

  calc_miss_percent( wr_hits, total, &wr_miss, &wr_percent );
  calc_miss_percent( rd_hits, total, &rd_miss, &rd_percent );

  fprintf( ofile, "  %-20.20s    %-20.20s   %5d/%5d/%5d      %3.0f%%         %5d/%5d/%5d      %3.0f%%\n",
           name, fname, wr_hits, wr_miss, total, wr_percent, rd_hits, rd_miss, total, rd_percent );

  return( (wr_miss > 0) || (rd_miss > 0) );

}

/*!
 \param ofile    Pointer to file to display coverage results to.
 \param head     Pointer to head of functional unit list to parse.
 \param wr_hits  Pointer to number of bits in memory toggled from 0 -> 1.
 \param rd_hits  Pointer to number of bits in memory toggled from 1 -> 0.
 \param total    Pointer to total number of bits in memories.

 \return Returns TRUE if any bits were found to be untoggled; otherwise, returns FALSE.

 Iterates through the functional unit list displaying the memory toggle coverage summary for
 each functional unit.
*/
static bool memory_ae_funit_summary(
            FILE*       ofile,
            funit_link* head,
  /*@out@*/ int*        wr_hits,
  /*@out@*/ int*        rd_hits,
  /*@out@*/ int*        total
) { PROFILE(MEMORY_AE_FUNIT_SUMMARY);

  bool  miss_found = FALSE;  /* Set to TRUE if missing toggles were found */
  char* pname;               /* Printable version of the functional unit name */

  while( head != NULL ) {

    /* If this is an assertion module, don't output any further */
    if( head->funit->stat->show && !funit_is_unnamed( head->funit ) &&
        ((info_suppl.part.assert_ovl == 0) || !ovl_is_assertion_module( head->funit )) ) {

      /* Get printable version of functional unit name */
      pname = scope_gen_printable( funit_flatten_name( head->funit ) );

      miss_found |= memory_display_ae_funit_summary( ofile, pname, get_basename( obf_file( head->funit->filename ) ),
                                                     head->funit->stat->mem_wr_hit, head->funit->stat->mem_rd_hit, head->funit->stat->mem_ae_total );

      /* Update accumulated information */
      *wr_hits += head->funit->stat->mem_wr_hit;
      *rd_hits += head->funit->stat->mem_rd_hit;
      *total   += head->funit->stat->mem_ae_total; 

      free_safe( pname, (strlen( pname ) + 1) );

    }

    head = head->next;

  }

  return( miss_found );

}

/*!
 Outputs the contents of the given memory in verbose output format.
*/
static void memory_display_memory(
  FILE*    ofile,       /*!< Pointer to output file */
  vsignal* sig,         /*!< Pointer to the current memory element to output */
  int      offset,      /*!< Bit offset of signal vector value to start interrogating */
  char*    prefix,      /*!< String containing memory prefix to output (initially this will be just the signal name) */
  int      dim,         /*!< Current dimension index (initially this will be 0) */
  int parent_dim_width  /*!< Bit width of parent dimension (initially this will be the width of the signal) */
) { PROFILE(MEMORY_DISPLAY_MEMORY);

  char name[4096];  /* Contains signal name */
  int  msb;         /* MSB of current dimension */
  int  lsb;         /* LSB of current dimension */
  int  be;          /* Big endianness of current dimension */
  int  i;           /* Loop iterator */
  int  dim_width;   /* Bit width of current dimension */

  assert( sig != NULL );
  assert( prefix != NULL );
  assert( dim < sig->udim_num );

  /* Calculate MSB, LSB and big endianness of current dimension */
  if( sig->dim[dim].msb > sig->dim[dim].lsb ) {
    msb = sig->dim[dim].msb;
    lsb = sig->dim[dim].lsb;
    be  = FALSE;
  } else {
    msb = sig->dim[dim].lsb;
    lsb = sig->dim[dim].msb;
    be  = TRUE;
  }

  /* Calculate current dimensional width */
  dim_width = parent_dim_width / ((msb - lsb) + 1);

  /* Only output memory contents if we have reached the lowest dimension */
  if( (dim + 1) == sig->udim_num ) {

    vector* vec = vector_create( dim_width, VTYPE_MEM, VDATA_U32, TRUE );
    int     tog01;
    int     tog10;
    int     wr;
    int     rd;

    /* Iterate through each addressable element in the current dimension */
    for( i=0; i<((msb - lsb) + 1); i++ ) {

      if( be ) {
        vector_copy_range( vec, sig->value, ((dim_width * ((msb - lsb) - i)) + offset) );
      } else {
        vector_copy_range( vec, sig->value, ((dim_width * i) + offset) );
      }

      /* Get toggle information */
      tog01 = 0;
      tog10 = 0;
      vector_toggle_count( vec, &tog01, &tog10 );

      /* Get write/read information */
      wr = 0;
      rd = 0;
      vector_mem_rw_count( vec, &wr, &rd );

      /* Output the addressable memory element if it is found to be lacking in coverage */
      if( (tog01 < dim_width) || (tog10 < dim_width) || (wr == 0) || (rd == 0) ) {

        int j;

        unsigned int rv = snprintf( name, 4096, "%s[%d]", prefix, i );
        assert( rv < 4096 );
        fprintf( ofile, "        %s  Written: %d  0->1: ", name, ((wr == 0) ? 0 : 1) );
        vector_display_toggle01_uint32( vec->value.u32, vec->width, ofile );
        fprintf( ofile, "\n" );
        fprintf( ofile, "        " );
        for( j=0; j<strlen( name ); j++ ) {
          fprintf( ofile, "." );
        }
        fprintf( ofile, "  Read   : %d  1->0: ", ((rd == 0) ? 0 : 1) );
        vector_display_toggle10_uint32( vec->value.u32, vec->width, ofile );
        fprintf( ofile, " ...\n" );
      }

    } 

    /* Deallocate the vector */
    vector_dealloc( vec );

  /* Otherwise, go down one level */
  } else {

    /* Iterate through each entry in the current dimesion */
    for( i=0; i<((msb - lsb) + 1); i++ ) {

      /* Create new prefix */
      unsigned int rv = snprintf( name, 4096, "%s[%d]", prefix, i );
      assert( rv < 4096 );

      if( be ) {
        memory_display_memory( ofile, sig, (offset + (dim_width * ((msb - lsb) - i))), name, (dim + 1), dim_width );
      } else {
        memory_display_memory( ofile, sig, (offset + (dim_width * i)),                 name, (dim + 1), dim_width );
      }

    }

  }

}

/*!
 \param ofile  Pointer to file to output results to.
 \param sigl   Pointer to signal list head.

 Displays the memories that did not achieve 100% toggle coverage and/or 100%
 write/read coverage to standard output from the specified signal list.
*/
static void memory_display_verbose( FILE* ofile, sig_link* sigl ) { PROFILE(MEMORY_DISPLAY_VERBOSE);

  sig_link* curr_sig;   /* Pointer to current signal link being evaluated */
  int       hit01;      /* Number of bits that toggled from 0 to 1 */
  int       hit10;      /* Number of bits that toggled from 1 to 0 */
  char*     pname;      /* Printable version of signal name */
  int       i;          /* Loop iterator */

  if( report_covered ) {
    fprintf( ofile, "    Memories getting 100%% coverage\n\n" );
  } else {
    fprintf( ofile, "    Memories not getting 100%% coverage\n\n" );
  }

  curr_sig = sigl;

  while( curr_sig != NULL ) {

    hit01 = 0;
    hit10 = 0;

    /* Get printable version of the signal name */
    pname = scope_gen_printable( curr_sig->sig->name );

    if( curr_sig->sig->suppl.part.type == SSUPPL_TYPE_MEM ) {

      fprintf( ofile, "      ---------------------------------------------------------------------------------------------------------\n" );
      fprintf( ofile, "      Memory name:  %s", pname );
      for( i=0; i<curr_sig->sig->udim_num; i++ ) {
        fprintf( ofile, "[%d:%d]", curr_sig->sig->dim[i].msb, curr_sig->sig->dim[i].lsb );
      }
      fprintf( ofile, "\n" );
      fprintf( ofile, "      ---------------------------------------------------------------------------------------------------------\n" );

      vector_toggle_count( curr_sig->sig->value, &hit01, &hit10 );

      if( report_covered ) {

        if( (hit01 == curr_sig->sig->value->width) && (hit10 == curr_sig->sig->value->width) ) {

          fprintf( ofile, "      %-24s\n", pname );

        }

      } else {

        memory_display_memory( ofile, curr_sig->sig, 0, curr_sig->sig->name, 0, curr_sig->sig->value->width );

      }

    }

    free_safe( pname, (strlen( pname ) + 1) );

    curr_sig = curr_sig->next;

  }

}

/*!
 \param ofile        Pointer to file to display coverage results to.
 \param root         Pointer to root of instance functional unit tree to parse.
 \param parent_inst  Name of parent instance.

 Displays the verbose memory coverage results to the specified output stream on
 an instance basis.  The verbose memory coverage includes the signal names,
 the bits that did not receive 100% toggle and addressable elements that did not
 receive 100% write/read coverage during simulation.
*/
static void memory_instance_verbose( FILE* ofile, funit_inst* root, char* parent_inst ) { PROFILE(MEMORY_INSTANCE_VERBOSE);

  funit_inst* curr_inst;      /* Pointer to current instance being evaluated */
  char        tmpname[4096];  /* Temporary name holder of instance */
  char*       pname;          /* Printable version of the name */

  assert( root != NULL );

  /* Get printable version of the signal */
  pname = scope_gen_printable( root->name );

  if( db_is_unnamed_scope( pname ) ) {
    strcpy( tmpname, parent_inst );
  } else if( strcmp( parent_inst, "*" ) == 0 ) {
    strcpy( tmpname, pname );
  } else {
    unsigned int rv = snprintf( tmpname, 4096, "%s.%s", parent_inst, pname );
    assert( rv < 4096 );
  }

  free_safe( pname, (strlen( pname ) + 1) );

  if( !funit_is_unnamed( root->funit ) &&
      ((root->stat->mem_tog01_hit < root->stat->mem_tog_total) ||
       (root->stat->mem_tog10_hit < root->stat->mem_tog_total) ||
       (root->stat->mem_wr_hit    < root->stat->mem_ae_total)  ||
       (root->stat->mem_rd_hit    < root->stat->mem_ae_total)) ) {

    fprintf( ofile, "\n" );
    switch( root->funit->type ) {
      case FUNIT_MODULE       :  fprintf( ofile, "    Module: " );       break;
      case FUNIT_ANAMED_BLOCK :
      case FUNIT_NAMED_BLOCK  :  fprintf( ofile, "    Named Block: " );  break;
      case FUNIT_AFUNCTION    :
      case FUNIT_FUNCTION     :  fprintf( ofile, "    Function: " );     break;
      case FUNIT_ATASK        :
      case FUNIT_TASK         :  fprintf( ofile, "    Task: " );         break;
      default                 :  fprintf( ofile, "    UNKNOWN: " );      break;
    }
    pname = scope_gen_printable( funit_flatten_name( root->funit ) );
    fprintf( ofile, "%s, File: %s, Instance: %s\n", pname, obf_file( root->funit->filename ), tmpname );
    fprintf( ofile, "    -------------------------------------------------------------------------------------------------------------\n" );
    free_safe( pname, (strlen( pname ) + 1) );

    memory_display_verbose( ofile, root->funit->sig_head );

  }

  curr_inst = root->child_head;
  while( curr_inst != NULL ) {
    memory_instance_verbose( ofile, curr_inst, tmpname );
    curr_inst = curr_inst->next;
  }

}

/*!
 \param ofile  Pointer to file to display coverage results to.
 \param head   Pointer to head of functional unit list to parse.

 Displays the verbose memory coverage results to the specified output stream on
 a functional unit basis (combining functional units that are instantiated multiple times).
 The verbose memory coverage includes the signal names, the bits that
 did not receive 100% toggle, and the addressable elements that did not receive 100%
 write/read coverage during simulation.
*/
static void memory_funit_verbose( FILE* ofile, funit_link* head ) { PROFILE(MEMORY_FUNIT_VERBOSE);

  while( head != NULL ) {

    if( !funit_is_unnamed( head->funit ) &&
        ((head->funit->stat->mem_tog01_hit < head->funit->stat->mem_tog_total) ||
         (head->funit->stat->mem_tog10_hit < head->funit->stat->mem_tog_total) ||
         (head->funit->stat->mem_wr_hit    < head->funit->stat->mem_ae_total)  ||
         (head->funit->stat->mem_rd_hit    < head->funit->stat->mem_ae_total)) ) {

      fprintf( ofile, "\n" );
      switch( head->funit->type ) {
        case FUNIT_MODULE       :  fprintf( ofile, "    Module: " );       break;
        case FUNIT_ANAMED_BLOCK :
        case FUNIT_NAMED_BLOCK  :  fprintf( ofile, "    Named Block: " );  break;
        case FUNIT_AFUNCTION    :
        case FUNIT_FUNCTION     :  fprintf( ofile, "    Function: " );     break;
        case FUNIT_ATASK        :
        case FUNIT_TASK         :  fprintf( ofile, "    Task: " );         break;
        default                 :  fprintf( ofile, "    UNKNOWN: " );      break;
      }
      fprintf( ofile, "%s, File: %s\n", obf_funit( funit_flatten_name( head->funit ) ), obf_file( head->funit->filename ) );
      fprintf( ofile, "    -------------------------------------------------------------------------------------------------------------\n" );

      memory_display_verbose( ofile, head->funit->sig_head );

    }

    head = head->next;

  }

}

/*!
 \param ofile    Pointer to output file to write
 \param verbose  Specifies if verbose coverage information should be output
*/
void memory_report( FILE* ofile, bool verbose ) { PROFILE(MEMORY_REPORT);

  bool       missed_found  = FALSE;  /* If set to TRUE, indicates that untoggled bits were found */
  char       tmp[4096];              /* Temporary string value */
  inst_link* instl;                  /* Pointer to current instance link */
  int        acc_hits01    = 0;      /* Accumulated hits 0 -> 1 count */
  int        acc_hits10    = 0;      /* Accumulated hits 1 -> 0 count */
  int        acc_tog_total = 0;      /* Accumulated bit toggle count */
  int        acc_wr_hits   = 0;      /* Accumulated number of addressable elements written */
  int        acc_rd_hits   = 0;      /* Accumulated number of addressable elements read */
  int        acc_ae_total  = 0;      /* Accumulated number of addressable elements */

  fprintf( ofile, "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n" );
  fprintf( ofile, "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~   MEMORY COVERAGE RESULTS   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n" );
  fprintf( ofile, "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n" );

  if( report_instance ) {

    if( leading_hiers_differ ) {
      strcpy( tmp, "<NA>" );
    } else {
      assert( leading_hier_num > 0 );
      strcpy( tmp, leading_hierarchies[0] );
    }

    fprintf( ofile, "                                                           Toggle 0 -> 1                       Toggle 1 -> 0\n" );
    fprintf( ofile, "Instance                                           Hit/ Miss/Total    Percent hit      Hit/ Miss/Total    Percent hit\n" );
    fprintf( ofile, "---------------------------------------------------------------------------------------------------------------------\n" );

    instl = db_list[curr_db]->inst_head;
    while( instl != NULL ) {
      missed_found |= memory_toggle_instance_summary( ofile, instl->inst, ((instl->next == NULL) ? tmp : "*"), &acc_hits01, &acc_hits10, &acc_tog_total );
      instl = instl->next;
    }
    fprintf( ofile, "---------------------------------------------------------------------------------------------------------------------\n" );
    (void)memory_display_toggle_instance_summary( ofile, "Accumulated", acc_hits01, acc_hits10, acc_tog_total );

    fprintf( ofile, "\n" );
    fprintf( ofile, "                                                    Addressable elements written         Addressable elements read\n" );
    fprintf( ofile, "                                                   Hit/ Miss/Total    Percent hit      Hit/ Miss/Total    Percent hit\n" );
    fprintf( ofile, "---------------------------------------------------------------------------------------------------------------------\n" );

    instl = db_list[curr_db]->inst_head;
    while( instl != NULL ) {
      missed_found |= memory_ae_instance_summary( ofile, instl->inst, ((instl->next == NULL) ? tmp : "*"), &acc_wr_hits, &acc_rd_hits, &acc_ae_total );
      instl = instl->next;
    }
    fprintf( ofile, "---------------------------------------------------------------------------------------------------------------------\n" );
    (void)memory_display_ae_instance_summary( ofile, "Accumulated", acc_wr_hits, acc_rd_hits, acc_ae_total );

    if( verbose && missed_found ) {
      fprintf( ofile, "---------------------------------------------------------------------------------------------------------------------\n" );
      instl = db_list[curr_db]->inst_head;
      while( instl != NULL ) {
        memory_instance_verbose( ofile, instl->inst, ((instl->next == NULL) ? tmp : "*") );
        instl = instl->next;
      }
    }

  } else {

    fprintf( ofile, "                                                           Toggle 0 -> 1                       Toggle 1 -> 0\n" );
    fprintf( ofile, "Module/Task/Function      Filename                 Hit/ Miss/Total    Percent hit      Hit/ Miss/Total    Percent hit\n" );
    fprintf( ofile, "---------------------------------------------------------------------------------------------------------------------\n" );

    missed_found |= memory_toggle_funit_summary( ofile, db_list[curr_db]->funit_head, &acc_hits01, &acc_hits10, &acc_tog_total );
    fprintf( ofile, "---------------------------------------------------------------------------------------------------------------------\n" );
    (void)memory_display_toggle_funit_summary( ofile, "Accumulated", "", acc_hits01, acc_hits10, acc_tog_total );

    fprintf( ofile, "\n" );
    fprintf( ofile, "                                                    Addressable elements written         Addressable elements read\n" );
    fprintf( ofile, "                                                   Hit/ Miss/Total    Percent hit      Hit/ Miss/Total    Percent hit\n" );
    fprintf( ofile, "---------------------------------------------------------------------------------------------------------------------\n" );

    missed_found |= memory_ae_funit_summary( ofile, db_list[curr_db]->funit_head, &acc_wr_hits, &acc_rd_hits, &acc_ae_total );
    fprintf( ofile, "---------------------------------------------------------------------------------------------------------------------\n" );
    (void)memory_display_ae_funit_summary( ofile, "Accumulated", "", acc_wr_hits, acc_rd_hits, acc_ae_total );

    if( verbose && missed_found ) {
      fprintf( ofile, "---------------------------------------------------------------------------------------------------------------------\n" );
      memory_funit_verbose( ofile, db_list[curr_db]->funit_head );
    }

  }

  fprintf( ofile, "\n\n" );

}


/*
 $Log$
 Revision 1.28.2.2  2008/04/23 05:20:44  phase1geo
 Completed initial pass of code updates.  I can now begin testing...  Checkpointing.

 Revision 1.28.2.1  2008/04/22 23:01:43  phase1geo
 More updates.  Completed initial pass of expr.c and fsm_arg.c.  Working
 on memory.c.  Checkpointing.

 Revision 1.28  2008/04/15 20:37:11  phase1geo
 Fixing database array support.  Full regression passes.

 Revision 1.27  2008/04/08 19:50:36  phase1geo
 Removing LAST operator for PEDGE, NEDGE and AEDGE expression operations and
 replacing them with the temporary vector solution.

 Revision 1.26  2008/03/17 22:02:31  phase1geo
 Adding new check_mem script and adding output to perform memory checking during
 regression runs.  Completed work on free_safe and added realloc_safe function
 calls.  Regressions are pretty broke at the moment.  Checkpointing.

 Revision 1.25  2008/03/17 05:26:16  phase1geo
 Checkpointing.  Things don't compile at the moment.

 Revision 1.24  2008/01/30 05:51:50  phase1geo
 Fixing doxygen errors.  Updated parameter list syntax to make it more readable.

 Revision 1.23  2008/01/16 23:10:31  phase1geo
 More splint updates.  Code is now warning/error free with current version
 of run_splint.  Still have regression issues to debug.

 Revision 1.22  2008/01/16 05:01:22  phase1geo
 Switched totals over from float types to int types for splint purposes.

 Revision 1.21  2008/01/10 04:59:04  phase1geo
 More splint updates.  All exportlocal cases are now taken care of.

 Revision 1.20  2008/01/09 23:54:15  phase1geo
 More splint updates.

 Revision 1.19  2008/01/07 23:59:55  phase1geo
 More splint updates.

 Revision 1.18  2007/12/11 05:48:25  phase1geo
 Fixing more compile errors with new code changes and adding more profiling.
 Still have a ways to go before we can compile cleanly again (next submission
 should do it).

 Revision 1.17  2007/11/20 05:28:58  phase1geo
 Updating e-mail address from trevorw@charter.net to phase1geo@gmail.com.

 Revision 1.16  2007/09/13 17:03:30  phase1geo
 Cleaning up some const-ness corrections -- still more to go but it's a good
 start.

 Revision 1.15  2007/07/26 22:23:00  phase1geo
 Starting to work on the functionality for automatic tasks/functions.  Just
 checkpointing some work.

 Revision 1.14  2007/07/16 22:24:38  phase1geo
 Fixed bugs in accumulated coverage output and updated regression files for this
 change.  VCS simulated results are not contained here, however.

 Revision 1.13  2007/07/16 18:39:59  phase1geo
 Finishing adding accumulated coverage output to report files.  Also fixed
 compiler warnings with static values in C code that are inputs to 64-bit
 variables.  Full regression was not run with these changes due to pre-existing
 simulator problems in core code.

 Revision 1.12  2007/07/16 12:39:33  phase1geo
 Started to add support for displaying accumulated coverage results for
 each metric.  Finished line and toggle and am half-way done with memory
 coverage (still have combinational logic, FSM and assertion coverage
 to complete before this feature is fully functional).

 Revision 1.11  2007/04/03 18:55:57  phase1geo
 Fixing more bugs in reporting mechanisms for unnamed scopes.  Checking in more
 regression updates per these changes.  Checkpointing.

 Revision 1.10  2007/04/03 04:15:17  phase1geo
 Fixing bugs in func_iter functionality.  Modified functional unit name
 flattening function (though this does not appear to be working correctly
 at this time).  Added calls to funit_flatten_name in all of the reporting
 files.  Checkpointing.

 Revision 1.9  2007/04/02 20:19:37  phase1geo
 Checkpointing more work on use of functional iterators.  Not working correctly
 yet.

 Revision 1.8  2006/10/12 22:48:46  phase1geo
 Updates to remove compiler warnings.  Still some work left to go here.

 Revision 1.7  2006/10/06 22:45:57  phase1geo
 Added support for the wait() statement.  Added wait1 diagnostic to regression
 suite to verify its behavior.  Also added missing GPL license note at the top
 of several *.h and *.c files that are somewhat new.

 Revision 1.6  2006/10/03 22:47:00  phase1geo
 Adding support for read coverage to memories.  Also added memory coverage as
 a report output for DIAGLIST diagnostics in regressions.  Fixed various bugs
 left in code from array changes and updated regressions for these changes.
 At this point, all IV diagnostics pass regressions.

 Revision 1.5  2006/10/02 22:41:00  phase1geo
 Lots of bug fixes to memory coverage functionality for GUI.  Memory coverage
 should now be working correctly.  We just need to update the GUI documentation
 as well as other images for the new feature add.

 Revision 1.4  2006/09/27 21:38:35  phase1geo
 Adding code to interract with data in memory coverage verbose window.  Majority
 of code is in place; however, this has not been thoroughly debugged at this point.
 Adding mem3 diagnostic for GUI debugging purposes and checkpointing.

 Revision 1.3  2006/09/26 22:36:38  phase1geo
 Adding code for memory coverage to GUI and related files.  Lots of work to go
 here so we are checkpointing for the moment.

 Revision 1.2  2006/09/25 22:22:28  phase1geo
 Adding more support for memory reporting to both score and report commands.
 We are getting closer; however, regressions are currently broken.  Checkpointing.

 Revision 1.1  2006/09/25 04:15:03  phase1geo
 Starting to add support for new memory coverage metric.  This includes changes
 for the report command only at this point.

*/
