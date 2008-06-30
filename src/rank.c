/*!
 \file     rank.c
 \author   Trevor Williams  (phase1geo@gmail.com)
 \date     6/28/2008
*/

#include <stdio.h>
#include <assert.h>

#include "defines.h"
#include "expr.h"
#include "fsm.h"
#include "profiler.h"
#include "rank.h"
#include "util.h"
#include "vsignal.h"


extern char user_msg[USER_MSG_LENGTH];
extern const exp_info exp_op_info[EXP_OP_NUM];


/*!
 List of CDD filenames that need to be read in.
*/
static char** rank_in = NULL;

/*!
 Number of CDD filenames in the rank_in array.
*/
static int rank_in_num = 0;

/*!
 File to be used for outputting rank information.
*/
static char* rank_file = NULL;

/*!
 Array containing the number of coverage points for each metric for all compressed CDD coverage structures.
*/
unsigned int num_cps[CP_TYPE_NUM] = {0};


/*!
 \return Returns the number of bits that are set in the given unsigned char
*/
static inline unsigned int rank_count_bits_uchar(
  unsigned char v   /*!< Character to count bits for */
) {

  v = (v & 0x55) + ((v >> 1) & 0x55);
  v = (v & 0x33) + ((v >> 2) & 0x33);
  v = (v & 0x0f) + ((v >> 4) & 0x0f);

  return( (unsigned int)v );
  
}

/*!
 \return Returns the number of bits that are set in the given 32-bit unsigned integer.
*/
static inline unsigned int rank_count_bits_uint32(
  uint32 v  /*!< 32-bit value to count bits for */
) {

  v = (v & 0x55555555) + ((v >>  1) & 0x55555555);
  v = (v & 0x33333333) + ((v >>  2) & 0x33333333);
  v = (v & 0x0f0f0f0f) + ((v >>  4) & 0x0f0f0f0f);
  v = (v & 0x00ff00ff) + ((v >>  8) & 0x00ff00ff);
  v = (v & 0x0000ffff) + ((v >> 16) & 0x0000ffff);

  return( (unsigned int)v );

}

/*!
 \return Returns the number of bits that are set in the given 64-bit unsigned integer.
*/
static inline unsigned int rank_count_bits_uint64(
  uint64 v  /*!< 64-bit value to count bits for */
) {

  v = (v & 0x5555555555555555LL) + ((v >>  1) & 0x5555555555555555LL);
  v = (v & 0x3333333333333333LL) + ((v >>  2) & 0x3333333333333333LL);
  v = (v & 0x0f0f0f0f0f0f0f0fLL) + ((v >>  4) & 0x0f0f0f0f0f0f0f0fLL);
  v = (v & 0x00ff00ff00ff00ffLL) + ((v >>  8) & 0x00ff00ff00ff00ffLL);
  v = (v & 0x0000ffff0000ffffLL) + ((v >> 16) & 0x0000ffff0000ffffLL);
  v = (v & 0x00000000ffffffffLL) + ((v >> 32) & 0x00000000ffffffffLL);
  
  return( (unsigned int)v );

}

/*!
 Clears the contents of the given compressed CDD coverage structure.
*/
static void rank_clear_comp_cdd(
  comp_cdd_cov* comp_cov  /*!< Pointer to compressed CDD coverage structure to clear */
) { PROFILE(RANK_CLEAR_COMP_CDD);

  unsigned int i, j;

  comp_cov->total_cps  = 0;
  comp_cov->unique_cps = 0;
  
  for( i=0; i<CP_TYPE_NUM; i++ ) {
    for( j=0; j<((num_cps[i]>>3)+1); j++ ) {
      comp_cov->cps[i][j] = 0;
    }
  }

  PROFILE_END;

}

/*!
 Merges the contents of the other compressed CDD coverage structure into the base.
*/
static void rank_merge_comp_cdd(
  comp_cdd_cov* base,  /*!< Base compressed CDD coverage structure */
  comp_cdd_cov* other  /*!< Compressed CDD coverage that will be merged into the base */
) { PROFILE(RANK_MERGE);

  unsigned int i, j;

  base->total_cps  += other->unique_cps;
  base->unique_cps += other->unique_cps;

  for( i=0; i<CP_TYPE_NUM; i++ ) {
    for( j=0; j<((num_cps[i]>>3)+1); j++ ) {
      base->cps[i][j] |= other->cps[i][j];
    }
  }

  PROFILE_END;

}

/*!
 Performs difference of the merged and other compressed CDD coverage structures, storing the results
 in the other compressed CDD coverage structure.
*/
static void rank_diff_comp_cdd(
  comp_cdd_cov* merged,  /*!< Merged version of all other compared CDD coverage structures */
  comp_cdd_cov* other    /*!< Singular compressed CDD coverage structure to check difference and populate with result */
) { PROFILE(RANK_DIFF);

  unsigned int  i, j;
  unsigned char diff;

  other->unique_cps = 0;

  for( i=0; i<CP_TYPE_NUM; i++ ) {
    for( j=0; j<((num_cps[i]>>3)+1); j++ ) {
      other->unique_cps += (uint64)rank_count_bits_uchar( merged->cps[i][j] ^ other->cps[i][j] );
    }
  }

  PROFILE_END;

}

/*!
 \return Returns a pointer to a newly allocated and initialized compressed CDD coverage structure.
*/
comp_cdd_cov* rank_create_comp_cdd_cov(
  const char* cdd_name,   /*!< Name of CDD file that this structure was created from */
  uint64      sim_events  /*!< Number of simulation events that occurred in the CDD */
) { PROFILE(RANK_CREATE_COMP_CDD_COV);

  comp_cdd_cov* comp_cov;
  unsigned int  i;

  /* Allocate and initialize */
  comp_cov             = (comp_cdd_cov*)malloc_safe( sizeof( comp_cdd_cov* ) );
  comp_cov->cdd_name   = strdup_safe( cdd_name );
  comp_cov->sim_events = sim_events;
  comp_cov->total_cps  = 0;
  comp_cov->unique_cps = 0;

  for( i=0; i<CP_TYPE_NUM; i++ ) {
    comp_cov->cps_index[i] = 0;
    if( num_cps[i] > 0 ) {
      comp_cov->cps[i] = (unsigned char*)calloc_safe( ((num_cps[i] >> 3) + 1), sizeof( unsigned char ) );
    }
  }

  PROFILE_END;

  return( comp_cov );

}

/*!
 Deallocates the specified compressed CDD coverage structure.
*/
void rank_dealloc_comp_cdd_cov(
  comp_cdd_cov* comp_cov  /*!< Pointer to compressed CDD coverage structure to deallocate */
) { PROFILE(RANK_DEALLOC_COMP_CDD_COV);

  if( comp_cov != NULL ) {

    unsigned int i;

    /* Deallocate name */
    free_safe( comp_cov->cdd_name, (strlen( comp_cov->cdd_name ) + 1) );

    /* Deallocate compressed coverage point information */
    for( i=0; i<CP_TYPE_NUM; i++ ) {
      free_safe( comp_cov->cps[i], (sizeof( unsigned char* ) * ((num_cps[i] >> 3) + 1)) );
    }

    /* Now deallocate ourselves */
    free_safe( comp_cov, sizeof( comp_cdd_cov* ) );

  }

  PROFILE_END;

}

/*!
 Outputs usage information to standard output for rank command.
*/
static void rank_usage() {

  printf( "\n" );
  printf( "Usage:  covered rank [<options>] <database_to_rank> <database_to_rank>+\n" );
  printf( "\n" );
  printf( "   Options:\n" );
  printf( "      -o <filename>           Name of file to output ranking information to.  Default is stdout.\n" );
  printf( "      -h                      Displays this help information.\n" );
  printf( "\n" );

}

/*!
 \throws anonymous Throw Throw Throw

 Parses the score argument list, placing all parsed values into
 global variables.  If an argument is found that is not valid
 for the rank operation, an error message is displayed to the
 user.
*/
static void rank_parse_args(
  int          argc,      /*!< Number of arguments in argument list argv */
  int          last_arg,  /*!< Index of last parsed argument from list */
  const char** argv       /*!< Argument list passed to this program */
) {

  int i;  /* Loop iterator */

  i = last_arg + 1;

  while( i < argc ) {

    if( strncmp( "-h", argv[i], 2 ) == 0 ) {

      rank_usage();
      Throw 0;

    } else if( strncmp( "-o", argv[i], 2 ) == 0 ) {

      if( check_option_value( argc, argv, i ) ) {
        i++;
        if( rank_file != NULL ) {
          print_output( "Only one -o option is allowed on the rank command-line.  Using first value...", WARNING, __FILE__, __LINE__ );
        } else {
          if( is_legal_filename( argv[i] ) ) {
            rank_file = strdup_safe( argv[i] );
          } else {
            unsigned int rv = snprintf( user_msg, USER_MSG_LENGTH, "Output file \"%s\" is unwritable", argv[i] );
            assert( rv < USER_MSG_LENGTH );
            print_output( user_msg, FATAL, __FILE__, __LINE__ );
            Throw 0;
          }
        }
      } else {
        Throw 0;
      } 

    } else {

      /* The name of a file to rank */
      if( file_exists( argv[i] ) ) {

        /* Add the specified rank file to the list */
        rank_in              = (char**)realloc_safe( rank_in, (sizeof( char* ) * rank_in_num), (sizeof( char* ) * (rank_in_num + 1)) );
        rank_in[rank_in_num] = strdup_safe( argv[i] );
        rank_in_num++;

      } else {

        unsigned int rv = snprintf( user_msg, USER_MSG_LENGTH, "CDD file (%s) does not exist", argv[i] );
        assert( rv < USER_MSG_LENGTH );
        print_output( user_msg, FATAL, __FILE__, __LINE__ );
        Throw 0;

      }

    }

    i++;

  }

  /* Check to make sure that the user specified at least two files to rank */
  if( rank_in_num < 2 ) {
    print_output( "Must specify at least two CDD files to rank", FATAL, __FILE__, __LINE__ );
    Throw 0;
  }

  /* If -o option was not specified, default its value to stdout */
  if( rank_file == NULL ) {
    rank_file = strdup_safe( "stdout" );
  }

}

/*!
 Parses the information line of a CDD file, extracts the information that is pertanent to
 coverage ranking, and allocates/initializes a new compressed CDD coverage structure.
*/
static void rank_parse_info(
  const char*    cdd_name,  /*!< Name of the CDD file that is being parsed */
  char**         line,      /*!< Read line from CDD file to parse */
  comp_cdd_cov** comp_cov,  /*!< Reference to compressed CDD coverage structure to create */
  bool           first      /*!< If set to TRUE, populate num_cps array with found information; otherwise,
                                 verify that our coverage point numbers match the global values and error
                                 if we do not match. */
) { PROFILE(RANK_PARSE_INFO);

  unsigned int version;
  isuppl       suppl;
  uint64       sim_events;
  unsigned int i;
  int          chars_read;

  if( sscanf( *line, "%x %x %lld%n", &version, &(suppl.all), &sim_events, &chars_read ) == 3 ) {
   
    *line += chars_read;

    /* Make sure that the CDD version matches our CDD version */
    if( version != CDD_VERSION ) {
      print_output( "CDD file being read is incompatible with this version of Covered", FATAL, __FILE__, __LINE__ );
      Throw 0;
    }

    /* Parse and handle coverage point information */
    for( i=0; i<CP_TYPE_NUM; i++ ) {
      unsigned int cp_num;
      if( sscanf( *line, "%u%n", &cp_num, &chars_read ) == 1 ) {
        *line += chars_read;
        if( first ) {
          num_cps[i] = cp_num;
        } else if( num_cps[i] != cp_num ) {
          unsigned int rv = snprintf( user_msg, USER_MSG_LENGTH, "Specified CDD file \"%s\" that is not mergeable with its previous CDD files", cdd_name );
          assert( rv < USER_MSG_LENGTH );
          print_output( user_msg, FATAL, __FILE__, __LINE__ );
          Throw 0;
        }
      } else {
        print_output( "CDD file being read is incompatible with this version of Covered", FATAL, __FILE__, __LINE__ );
        Throw 0;
      }
    }

    /* Now that we have the information we need, create and populate the compressed CDD coverage structure */
    *comp_cov = rank_create_comp_cdd_cov( cdd_name, sim_events );

  } else {

    print_output( "CDD file being read is incompatible with this version of Covered", FATAL, __FILE__, __LINE__ );
    Throw 0;

  }

  PROFILE_END;

}

/*!
 Parses a signal line of a CDD file and extracts the toggle and/or memory coverage information
 from the line, compressing the coverage point information and storing it into the comp_cov
 structure.
*/
static void rank_parse_signal(
  char**        line,     /*!< Line containing signal information from CDD file */
  comp_cdd_cov* comp_cov  /*!< Pointer to compressed CDD coverage structure */
) { PROFILE(RANK_PARSE_SIGNAL);

  func_unit curr_funit;
  vsignal*  sig;

  /* Initialize the signal list pointers */
  curr_funit.sig_head = curr_funit.sig_tail = NULL;

  /* Parse the signal */
  vsignal_db_read( line, &curr_funit );
  sig = curr_funit.sig_head->sig;

  /* Populate toggle coverage information */
  if( (sig->suppl.part.type != SSUPPL_TYPE_PARAM) &&
      (sig->suppl.part.type != SSUPPL_TYPE_ENUM)  &&
      (sig->suppl.part.type != SSUPPL_TYPE_MEM)  &&
      (sig->suppl.part.mba == 0) ) {

    unsigned int i;
    if( sig->suppl.part.excluded == 1 ) {
      for( i=0; i<sig->value->width; i++ ) {
        uint64 index = comp_cov->cps_index[CP_TYPE_TOGGLE]++;
        comp_cov->cps[CP_TYPE_TOGGLE][index >> 3] |= 0x1 << (index & 0x7);
        index = comp_cov->cps_index[CP_TYPE_TOGGLE]++;
        comp_cov->cps[CP_TYPE_TOGGLE][index >> 3] |= 0x1 << (index & 0x7);
      }
    } else {
      switch( sig->value->suppl.part.data_type ) {
        case VDATA_UL :
          for( i=0; i<sig->value->width; i++ ) {
            uint64 index = comp_cov->cps_index[CP_TYPE_TOGGLE]++;
            comp_cov->cps[CP_TYPE_TOGGLE][index >> 3] |= (sig->value->value.ul[UL_DIV(i)][VTYPE_INDEX_SIG_TOG01] >> UL_MOD(i)) << (index & 0x7);
            index = comp_cov->cps_index[CP_TYPE_TOGGLE]++;
            comp_cov->cps[CP_TYPE_TOGGLE][index >> 3] |= (sig->value->value.ul[UL_DIV(i)][VTYPE_INDEX_SIG_TOG10] >> UL_MOD(i)) << (index & 0x7);
          }
        break;
        default :  assert( 0 );  break;
      }
    }

  }

  /* Populate memory coverage information */
  if( (sig->suppl.part.type == SSUPPL_TYPE_MEM) && (sig->udim_num > 0) ) {
 
    unsigned int i;
    unsigned int pwidth = 1;

    for( i=(sig->udim_num); i<(sig->udim_num + sig->pdim_num); i++ ) {
      if( sig->dim[i].msb > sig->dim[i].lsb ) {
        pwidth *= (sig->dim[i].msb - sig->dim[i].lsb) + 1;
      } else {
        pwidth *= (sig->dim[i].lsb - sig->dim[i].msb) + 1;
      }
    }

    /* Calculate total number of addressable elements and their write/read information */
    for( i=0; i<sig->value->width; i+=pwidth ) {
      if( sig->suppl.part.excluded == 1 ) {
        uint64 index = comp_cov->cps_index[CP_TYPE_MEM]++;
        comp_cov->cps[CP_TYPE_MEM][index >> 3] |= 0x1 << (index & 0x7);
        index = comp_cov->cps_index[CP_TYPE_MEM]++;
        comp_cov->cps[CP_TYPE_MEM][index >> 3] |= 0x1 << (index & 0x7);
      } else {
        unsigned int wr = 0;
        unsigned int rd = 0;
        uint64       index;
        vector_mem_rw_count( sig->value, (int)i, (int)((i + pwidth) - 1), &wr, &rd );
        index = comp_cov->cps_index[CP_TYPE_MEM]++;
        comp_cov->cps[CP_TYPE_MEM][index >> 3] |= ((wr > 0) ? 1 : 0) << (index & 0x7);
        index = comp_cov->cps_index[CP_TYPE_MEM]++;
        comp_cov->cps[CP_TYPE_MEM][index >> 3] |= ((rd > 0) ? 1 : 0) << (index & 0x7);
      }
    }
  
    /* Calculate toggle coverage information for the memory */
    if( sig->suppl.part.excluded == 1 ) {
      for( i=0; i<sig->value->width; i++ ) {
        uint64 index = comp_cov->cps_index[CP_TYPE_MEM]++;
        comp_cov->cps[CP_TYPE_MEM][index >> 3] |= 0x1 << (index & 0x7);
        index = comp_cov->cps_index[CP_TYPE_MEM]++;
        comp_cov->cps[CP_TYPE_MEM][index >> 3] |= 0x1 << (index & 0x7);
      }
    } else {
      switch( sig->value->suppl.part.data_type ) {
        case VDATA_UL :
          for( i=0; i<sig->value->width; i++ ) {
            uint64 index = comp_cov->cps_index[CP_TYPE_TOGGLE]++;
            comp_cov->cps[CP_TYPE_MEM][index >> 3] |= (sig->value->value.ul[UL_DIV(i)][VTYPE_INDEX_MEM_TOG01] >> UL_MOD(i)) << (index & 0x7);
            index = comp_cov->cps_index[CP_TYPE_TOGGLE]++;
            comp_cov->cps[CP_TYPE_MEM][index >> 3] |= (sig->value->value.ul[UL_DIV(i)][VTYPE_INDEX_MEM_TOG10] >> UL_MOD(i)) << (index & 0x7);
          }
          break;
        default :  assert( 0 );  break;
      }
    }

  }

  /* Deallocate signal and signal list */
  sig_link_delete_list( curr_funit.sig_head, TRUE );

  PROFILE_END;

}

/*!
 Parses a signal line of a CDD file and extracts the line and/or combinational logic coverage information
 from the line, compressing the coverage point information and storing it into the comp_cov
 structure.
*/
static void rank_parse_expression(
  char**        line,
  comp_cdd_cov* comp_cov
) { PROFILE(RANK_PARSE_EXPRESSION);

  func_unit   curr_funit;
  expression* exp;

  /* Initialize the signal list pointers */
  curr_funit.exp_head = curr_funit.exp_tail = NULL;

  /* Parse the expression */
  expression_db_read( line, &curr_funit, FALSE );
  exp = curr_funit.exp_head->exp;

  /* Calculate line coverage information (NOTE:  we currently ignore the excluded status of the line */
  if( (exp->suppl.part.root == 1) &&
      (exp->op != EXP_OP_DELAY)   &&
      (exp->op != EXP_OP_CASE)    &&
      (exp->op != EXP_OP_CASEX)   &&
      (exp->op != EXP_OP_CASEZ)   &&
      (exp->op != EXP_OP_DEFAULT) &&
      (exp->op != EXP_OP_NB_CALL) &&
      (exp->op != EXP_OP_FORK)    &&
      (exp->op != EXP_OP_JOIN)    &&
      (exp->op != EXP_OP_NOOP)    &&
      (exp->line != 0) ) {
    uint64 index = comp_cov->cps_index[CP_TYPE_LINE]++;
    if( exp->exec_num > 0 ) {
      comp_cov->cps[CP_TYPE_LINE][index >> 3] |= 0x1 << (index & 0x7);
    }
  }

  /* Calculate combinational logic coverage information */
  if( EXPR_IS_MEASURABLE( exp ) == 1 ) {

    /* Calculate current expression combination coverage */
    if( !expression_is_static_only( exp ) ) {

      uint64 index;

      if( EXPR_IS_COMB( exp ) == 1 ) {
        if( exp_op_info[exp->op].suppl.is_comb == AND_COMB ) {
          index = comp_cov->cps_index[CP_TYPE_LOGIC]++;
          comp_cov->cps[CP_TYPE_LOGIC][index>>3] |= (exp->suppl.part.eval_00 | exp->suppl.part.eval_01) << (index & 0x7);
          index = comp_cov->cps_index[CP_TYPE_LOGIC]++;
          comp_cov->cps[CP_TYPE_LOGIC][index>>3] |= (exp->suppl.part.eval_00 | exp->suppl.part.eval_10) << (index & 0x7);
          index = comp_cov->cps_index[CP_TYPE_LOGIC]++;
          comp_cov->cps[CP_TYPE_LOGIC][index>>3] |= exp->suppl.part.eval_11 << (index & 0x7);
        } else if( exp_op_info[exp->op].suppl.is_comb == OR_COMB ) {
          uint64 index = comp_cov->cps_index[CP_TYPE_LOGIC]++;
          comp_cov->cps[CP_TYPE_LOGIC][index>>3] |= (exp->suppl.part.eval_10 | exp->suppl.part.eval_11) << (index & 0x7);
          index = comp_cov->cps_index[CP_TYPE_LOGIC]++;
          comp_cov->cps[CP_TYPE_LOGIC][index>>3] |= (exp->suppl.part.eval_01 | exp->suppl.part.eval_11) << (index & 0x7);
          index = comp_cov->cps_index[CP_TYPE_LOGIC]++;
          comp_cov->cps[CP_TYPE_LOGIC][index>>3] |= exp->suppl.part.eval_00 << (index & 0x7);
        } else {
          index = comp_cov->cps_index[CP_TYPE_LOGIC]++;
          comp_cov->cps[CP_TYPE_LOGIC][index>>3] |= exp->suppl.part.eval_00 << (index & 0x7);
          index = comp_cov->cps_index[CP_TYPE_LOGIC]++;
          comp_cov->cps[CP_TYPE_LOGIC][index>>3] |= exp->suppl.part.eval_01 << (index & 0x7);
          index = comp_cov->cps_index[CP_TYPE_LOGIC]++;
          comp_cov->cps[CP_TYPE_LOGIC][index>>3] |= exp->suppl.part.eval_10 << (index & 0x7);
          index = comp_cov->cps_index[CP_TYPE_LOGIC]++;
          comp_cov->cps[CP_TYPE_LOGIC][index>>3] |= exp->suppl.part.eval_11 << (index & 0x7);
        }
      } else if( EXPR_IS_EVENT( exp ) == 1 ) {
        index = comp_cov->cps_index[CP_TYPE_LOGIC]++;
        comp_cov->cps[CP_TYPE_LOGIC][index>>3] |= ESUPPL_WAS_TRUE( exp->suppl ) << (index & 0x7);
      } else {
        index = comp_cov->cps_index[CP_TYPE_LOGIC]++;
        comp_cov->cps[CP_TYPE_LOGIC][index>>3] |= ESUPPL_WAS_TRUE( exp->suppl ) << (index & 0x7);
        index = comp_cov->cps_index[CP_TYPE_LOGIC]++;
        comp_cov->cps[CP_TYPE_LOGIC][index>>3] |= ESUPPL_WAS_FALSE( exp->suppl ) << (index & 0x7);
      }

    }

  }

  /* Deallocate the expression link and expression */
  exp_link_delete_list( curr_funit.exp_head, TRUE );

  PROFILE_END;

}

/*!
 Parses a signal line of a CDD file and extracts the FSM state/state transition coverage information
 from the line, compressing the coverage point information and storing it into the comp_cov
 structure.
*/
static void rank_parse_fsm(
  char**        line,
  comp_cdd_cov* comp_cov
) { PROFILE(RANK_PARSE_FSM);

  func_unit  curr_funit;
  fsm_table* table;

  /* Initialize the signal list pointers */
  curr_funit.fsm_head = curr_funit.fsm_tail = NULL;

  /* Parse the FSM */
  fsm_db_read( line, &curr_funit );
  table = curr_funit.fsm_head->table->table;

  /* We can only create a compressed version of FSM coverage information if it is known */
  if( table->suppl.part.known == 0 ) {

    int*         state_hits = (int*)malloc_safe( sizeof( int ) * table->num_fr_states );
    unsigned int i;

    /* Initialize state_hits array */
    for( i=0; i<table->num_fr_states; i++ ) {
      state_hits[i] = 0;
    }

    /* Iterate through arc transition array and count unique hits */
    for( i=0; i<table->num_arcs; i++ ) {
      uint64 index;
      if( (table->arcs[i]->suppl.part.hit || table->arcs[i]->suppl.part.excluded) ) {
        if( state_hits[table->arcs[i]->from]++ == 0 ) {
          index = comp_cov->cps_index[CP_TYPE_FSM]++;
          comp_cov->cps[CP_TYPE_FSM][index >> 3] |= 0x1 << (index & 0x7);
        }
        index = comp_cov->cps_index[CP_TYPE_FSM]++;
        comp_cov->cps[CP_TYPE_FSM][index >> 3] |= 0x1 << (index & 0x7);
      } else {
        if( state_hits[table->arcs[i]->from]++ == 0 ) {
          comp_cov->cps_index[CP_TYPE_FSM]++;
        }
        comp_cov->cps_index[CP_TYPE_FSM]++;
      }
    }

    /* Deallocate state_hits array */
    free_safe( state_hits, (sizeof( int ) * table->num_fr_states) );

  }

  /* Deallocate FSM and FSM list */
  fsm_link_delete_list( curr_funit.fsm_head );

  PROFILE_END;

}

/*!
 Parses the given CDD name and stores its coverage point information in a compressed format.
*/
static void rank_read_cdd(
            const char*     cdd_name,     /*!< Filename of CDD file to read in */
            bool            first,        /*!< Set to TRUE if this if the first CDD being read */
  /*@out@*/ comp_cdd_cov*** comp_cdds,    /*!< Pointer to compressed CDD array */
  /*@out@*/ unsigned int*   comp_cdd_num  /*!< Number of compressed CDD structures in comp_cdds array */
) { PROFILE(RANK_READ_CDD);

  FILE*        ifile;           /* Pointer to CDD file handle */
  char*        curr_line;       /* Pointer to currently read CDD line */
  char*        rest_line;       /* The line that has not been parsed */
  unsigned int curr_line_size;  /* Number of bytes allocated for curr_line */
  int          type;            /* Current line type */
  int          chars_read;      /* Number of characters read from current line */

  /* Open the CDD file */
  if( (ifile = fopen( cdd_name, "r" )) != NULL ) {

    comp_cdd_cov* comp_cov = NULL;
    unsigned int  rv;

    /* Read the entire next line */
    while( util_readline( ifile, &curr_line, &curr_line_size ) ) {

      /* If the line contains a legal CDD file line, continue to parse it */
      if( sscanf( curr_line, "%d%n", &type, &chars_read ) == 1 ) {

        rest_line = curr_line + chars_read;    

        /* Determine the current information type, and if it contains coverage information, send it the proper parser */
        switch( type ) {
          case DB_TYPE_INFO       :  rank_parse_info( cdd_name, &rest_line, &comp_cov, first );  break;
          case DB_TYPE_SIGNAL     :  rank_parse_signal( &rest_line, comp_cov );      break;
          case DB_TYPE_EXPRESSION :  rank_parse_expression( &rest_line, comp_cov );  break;
          case DB_TYPE_FSM        :  rank_parse_fsm( &rest_line, comp_cov );         break;
          default                 :  break;
        }

      } else {

        rv = snprintf( user_msg, USER_MSG_LENGTH, "CDD file \"%s\" is not formatted correctly", cdd_name );
        assert( rv < USER_MSG_LENGTH );
        print_output( user_msg, FATAL, __FILE__, __LINE__ );
        Throw 0;

      }

    }

    /* Close the CDD file */
    rv = fclose( ifile );
    assert( rv == 0 );

    /* Store the new coverage point structure in the global list if it contains information */
    if( comp_cov != NULL ) {
      *comp_cdds = (comp_cdd_cov**)realloc_safe( *comp_cdds, (sizeof( comp_cdd_cov* ) * (*comp_cdd_num)), (sizeof( comp_cdd_cov* ) * ((*comp_cdd_num) + 1)) );
      (*comp_cdds)[*comp_cdd_num] = comp_cov;
      (*comp_cdd_num)++;
    }

  } else {

    unsigned int rv = snprintf( user_msg, USER_MSG_LENGTH, "Unable to read CDD file \"%s\" for ranking", cdd_name );
    assert( rv < USER_MSG_LENGTH );
    print_output( user_msg, FATAL, __FILE__, __LINE__ );
    Throw 0;

  }

  PROFILE_END;

}

/*-----------------------------------------------------------------------------------------------------------------------*/

/*!
 Performs the task of ranking the CDD files and rearranging them in the comp_cdds array such that the
 first CDD file is located at index 0.
*/
static void rank_perform(
  /*@out@*/ comp_cdd_cov** comp_cdds,    /*!< Pointer to array of compressed CDD coverage structures to rank */
            unsigned int   comp_cdd_num  /*!< Number of allocated structures in comp_cdds array */
) { PROFILE(RANK_PERFORM);

  unsigned int i, j, k;
  uint16*      merged;
  uint16       merged_index = 0;
  uint64       total        = 0;
  unsigned int next_cdd     = 0;
  unsigned int most_unique;

  /* Calculate the total number of needed merged entries to store accumulated information */
  for( i=0; i<comp_cdd_num; i++ ) {
    total += num_cps[i];
  }
  assert( total > 0 );

  /* Allocate merged array */
  merged = (uint16*)malloc_safe_nolimit( sizeof( uint16 ) * total );

  /* Step 1 - Initialize merged results array, calculate uniqueness and total values of each compressed CDD coverage structure */
  for( i=0; i<CP_TYPE_NUM; i++ ) {
    for( j=0; j<num_cps[i]; j++ ) {
      uint16       bit_total = 0;
      unsigned int set_cdd;
      for( k=0; k<comp_cdd_num; k++ ) {
        if( comp_cdds[k]->cps[i][j>>3] & (0x1 << (j & 0x7)) ) {
          comp_cdds[k]->total_cps++;
          set_cdd = k;
          bit_total++;
        }
      }
      merged[merged_index++] = bit_total;

      /* If we found exactly one CDD file that hit this coverage point, mark it in the corresponding CDD file */
      if( bit_total == 1) {
        comp_cdds[set_cdd]->unique_cps++;
      }
    }
  }

  /* Step 2 - Start with the most unique CDDs */
  do {
    most_unique = next_cdd;
    for( i=(next_cdd+1); i<comp_cdd_num; i++ ) {
      if( comp_cdds[i]->unique_cps > comp_cdds[most_unique]->unique_cps ) {
        most_unique = i;
      }
    }
    if( comp_cdds[most_unique]->unique_cps > 0 ) {

      /* Move the most unique CDD to the next position */
      comp_cdd_cov* tmp      = comp_cdds[next_cdd];
      comp_cdds[next_cdd]    = comp_cdds[most_unique];
      comp_cdds[most_unique] = tmp;

      /* Subtract all of the set coverage points from the merged value */
      merged_index = 0;
      for( i=0; i<CP_TYPE_NUM; i++ ) {
        for( j=0; j<num_cps[i]; j++ ) {
          if( comp_cdds[next_cdd]->cps[i][j>>3] & (0x1 << (j & 0x7)) ) {
            merged[merged_index] = 0;
          }
        }
      }
      next_cdd++;
    }
  } while( (next_cdd < comp_cdd_num) && (comp_cdds[most_unique]->unique_cps > 0) );

  /* Step 3 - TBD */

  /* Deallocate merged CDD coverage structure */
  free_safe( merged, (sizeof( uint16 ) * total ) );

  PROFILE_END;

}

/*-----------------------------------------------------------------------------------------------------------------------*/

/*!
 Outputs the ranking of the CDD files to the output file specified from the rank command line.
*/
static void rank_output(
  comp_cdd_cov** comp_cdds,    /*!< Pointer to array of ranked CDD coverage structures */
  unsigned int   comp_cdd_num  /*!< Number of allocated structures in comp_cdds array */
) { PROFILE(RANK_OUTPUT);

  FILE* ofile;

  if( (ofile = fopen( rank_file, "w" )) != NULL ) {

    unsigned int rv;
    unsigned int i;
    uint64       acc_sim_events = 0;
    bool         unique_found   = TRUE;
    float        total_cps      = 0;

    /* Calculate the total number of coverage points */
    for( i=0; i<CP_TYPE_NUM; i++ ) {
      total_cps = num_cps[i];
    }

    /* TBD - Header information output */

    for( i=0; i<comp_cdd_num; i++ ) {
      acc_sim_events += comp_cdds[i]->sim_events; 
      if( (comp_cdds[i]->unique_cps == 0) && unique_found ) {
        fprintf( ofile, "\n--------------------------------  The following CDD files add no additional coverage  --------------------------------\n\n" );
        unique_found = FALSE;
      }
      fprintf( ofile, "%3.0f%%  %s  %3.0f%%  %lld  %lld\n",
               (comp_cdds[0]->unique_cps / total_cps), comp_cdds[i]->cdd_name, (comp_cdds[0]->total_cps / total_cps), comp_cdds[i]->sim_events, acc_sim_events );
    }

    /* TBD - Footer information output - if any needed */

    rv = fclose( ofile );
    assert( rv == 0 );

  } else {

    unsigned int rv = snprintf( user_msg, USER_MSG_LENGTH, "Unable to open ranking file \"%s\" for writing", rank_file );
    assert( rv < USER_MSG_LENGTH );
    print_output( user_msg, FATAL, __FILE__, __LINE__ );
    Throw 0;

  }

  PROFILE_END;

}

/*!
 \param argc      Number of arguments in command-line to parse.
 \param last_arg  Index of last parsed argument from list.
 \param argv      List of arguments from command-line to parse.

 Performs merge command functionality.
*/
void command_rank(
  int          argc,      /*!< Number of arguments in command-line to parse */
  int          last_arg,  /*!< Index of last parsed argument from list */
  const char** argv       /*!< List of arguments from command-line to parse */
) { PROFILE(COMMAND_RANK);

  int            i, j;
  unsigned int   rv;
  comp_cdd_cov** comp_cdds    = NULL;
  unsigned int   comp_cdd_num = 0;

  /* Output header information */
  rv = snprintf( user_msg, USER_MSG_LENGTH, COVERED_HEADER );
  assert( rv < USER_MSG_LENGTH );
  print_output( user_msg, NORMAL, __FILE__, __LINE__ );

  Try {

    /* Parse score command-line */
    rank_parse_args( argc, last_arg, argv );

    /* Read in databases to merge */
    for( i=0; i<rank_in_num; i++ ) {
      rv = snprintf( user_msg, USER_MSG_LENGTH, "Reading CDD file \"%s\"", rank_in[i] );
      assert( rv < USER_MSG_LENGTH );
      print_output( user_msg, NORMAL, __FILE__, __LINE__ );
      rank_read_cdd( rank_in[i], (i == 0), &comp_cdds, &comp_cdd_num );
    }

    /* Peaform the ranking algorithm */
    rank_perform( comp_cdds, comp_cdd_num );

    /* Output the results */
    rank_output( comp_cdds, comp_cdd_num );

  } Catch_anonymous {}

  /* Deallocate other allocated variables */
  for( i=0; i<rank_in_num; i++ ) {
    free_safe( rank_in[i], (strlen( rank_in[i] ) + 1) );
  }
  free_safe( rank_in, (sizeof( char* ) * rank_in_num) );

  /* Deallocate the compressed CDD coverage structures */
  for( i=0; i<comp_cdd_num; i++ ) {
    rank_dealloc_comp_cdd_cov( comp_cdds[i] );
  }
  free_safe( comp_cdds, (sizeof( comp_cdd_cov* ) * comp_cdd_num) );

  free_safe( rank_file, (strlen( rank_file ) + 1) );

  PROFILE_END;

}

/*
 $Log$
 Revision 1.1.2.2  2008/06/30 23:02:42  phase1geo
 More work on the rank command.  Checkpointing.

 Revision 1.1.2.1  2008/06/30 13:14:22  phase1geo
 Starting to work on new 'rank' command.  Checkpointing.

*/

