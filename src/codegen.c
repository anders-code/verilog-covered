/*!
 \file     codegen.c
 \author   Trevor Williams  (trevorw@charter.net)
 \date     4/11/2002
*/

#include <string.h>
#include <assert.h>

#include "defines.h"
#include "codegen.h"
#include "vector.h"


/*!
 \param expr       Pointer to root of expression tree to generate.
 \param line       Specifies last line number to generate.
 \param parent_op  Operation of parent.  If our op is the same, no surrounding parenthesis is needed.

 \return Returns a pointer to the generated Verilog string.

 Generates Verilog code from specified expression tree.  This Verilog
 snippet is used by the verbose coverage reporting functions for showing
 Verilog examples that were missed during simulation.  The line parameter
 specifies the last line number to generate for this expression tree.  If
 the value of line is -1, the entire expression tree is generated.
*/
char* codegen_gen_expr( expression* expr, int line, int parent_op ) {

  char* my_code = NULL;  /* Pointer to the code that is generated by the current expression           */
  char* right_code;      /* Pointer to the code that is generated by the right side of the expression */
  char* left_code;       /* Pointer to the code that is generated by the left side of the expression  */
  int   code_size = 0;   /* Number of bytes wide my_code is                                           */
  char  code_format[20]; /* Format for creating my_code string                                        */
  bool  both;            /* Specifies if both expressions or just right expression should be output   */
  char* tmpname;         /* Temporary signal name holder                                              */

  if( (expr != NULL) && ((line == -1) || (expr->line == line)) ) {

    /* 
     If the current expression is a case expression, display left even though it usually
     appears on a different line than the rest of the expression.
    */
    if( (SUPPL_OP( expr->suppl ) == EXP_OP_CASE)  ||
        (SUPPL_OP( expr->suppl ) == EXP_OP_CASEX) ||
        (SUPPL_OP( expr->suppl ) == EXP_OP_CASEZ) ) {
      left_code = codegen_gen_expr( expr->left,  -1, SUPPL_OP( expr->suppl ) );
    } else {
      left_code = codegen_gen_expr( expr->left, line, SUPPL_OP( expr->suppl ) );
    }

    right_code = codegen_gen_expr( expr->right, line, SUPPL_OP( expr->suppl ) );

    if( SUPPL_OP( expr->suppl ) == EXP_OP_LAST ) {

      my_code = NULL;

    } else if( SUPPL_OP( expr->suppl ) == EXP_OP_STATIC ) {

      switch( vector_get_type( expr->value ) ) {

        case DECIMAL :
          snprintf( code_format, 20, "%d", vector_to_int( expr->value ) );
          my_code = strdup( code_format );
          break;

        case BINARY :
          my_code = vector_to_string( expr->value, BINARY );
          break;

        case OCTAL :
          my_code = vector_to_string( expr->value, OCTAL );
          break;

        case HEXIDECIMAL :
          my_code = vector_to_string( expr->value, HEXIDECIMAL );
          break;

        default :  break;

      }

    } else if( (SUPPL_OP( expr->suppl ) == EXP_OP_SIG) ||
               (SUPPL_OP( expr->suppl ) == EXP_OP_PARAM) ) {

      assert( expr->sig != NULL );

      if( expr->sig->name[0] == '#' ) {
        tmpname = expr->sig->name + 1;
      } else {
        tmpname = expr->sig->name;
      }

      switch( strlen( tmpname ) ) {
        case 0 :  assert( strlen( tmpname ) > 0 );  break;
        case 1 :
          my_code = (char*)malloc_safe( 4 );
          snprintf( my_code, 4, " %s ", tmpname );
          break;
        case 2 :
          my_code = (char*)malloc_safe( 4 );
          snprintf( my_code, 4, " %s", tmpname );
          break;
        default :
          my_code = strdup( tmpname );
          break;
      }

    } else if( (SUPPL_OP( expr->suppl ) == EXP_OP_SBIT_SEL) ||
               (SUPPL_OP( expr->suppl ) == EXP_OP_PARAM_SBIT) ) {

      assert( expr->sig != NULL );

      if( expr->sig->name[0] == '#' ) {
        tmpname = expr->sig->name + 1;
      } else {
        tmpname = expr->sig->name;
      }

      code_size = strlen( tmpname ) + strlen( left_code ) + 3;
      my_code   = (char*)malloc_safe( code_size );
      snprintf( my_code, code_size, "%s[%s]", tmpname, left_code );

    } else if( (SUPPL_OP( expr->suppl ) == EXP_OP_MBIT_SEL) ||
               (SUPPL_OP( expr->suppl ) == EXP_OP_PARAM_MBIT) ) {

      assert( expr->sig != NULL );

      if( expr->sig->name[0] == '#' ) {
        tmpname = expr->sig->name + 1;
      } else {
        tmpname = expr->sig->name;
      }

      code_size = strlen( tmpname ) + strlen( left_code ) + strlen( right_code ) + 4;
      my_code   = (char*)malloc_safe( code_size );
      if( SUPPL_WAS_SWAPPED( expr->suppl ) ) {
        snprintf( my_code, code_size, "%s[%s:%s]", tmpname, right_code, left_code );
      } else {
        snprintf( my_code, code_size, "%s[%s:%s]", tmpname, left_code, right_code );
      }

    } else if( SUPPL_OP( expr->suppl ) == EXP_OP_DEFAULT ) {

      my_code = strdup( "default :" );

    } else {

      if( parent_op == SUPPL_OP( expr->suppl ) ) {

        switch( SUPPL_OP( expr->suppl ) ) {
          case EXP_OP_XOR      :  code_size = 4;  strcpy( code_format, "%s ^ %s" );         both = TRUE;   break;
          case EXP_OP_MULTIPLY :  code_size = 4;  strcpy( code_format, "%s * %s" );         both = TRUE;   break;
          case EXP_OP_DIVIDE   :  code_size = 4;  strcpy( code_format, "%s / %s" );         both = TRUE;   break;
          case EXP_OP_MOD      :  code_size = 4;  strcpy( code_format, "%s %% %s" );        both = TRUE;   break;
          case EXP_OP_ADD      :  code_size = 4;  strcpy( code_format, "%s + %s" );         both = TRUE;   break;
          case EXP_OP_SUBTRACT :  code_size = 4;  strcpy( code_format, "%s - %s" );         both = TRUE;   break;
          case EXP_OP_AND      :  code_size = 4;  strcpy( code_format, "%s & %s" );         both = TRUE;   break;
          case EXP_OP_OR       :  code_size = 4;  strcpy( code_format, "%s | %s" );         both = TRUE;   break;
          case EXP_OP_NAND     :  code_size = 5;  strcpy( code_format, "%s ~& %s" );        both = TRUE;   break;
          case EXP_OP_NOR      :  code_size = 5;  strcpy( code_format, "%s ~| %s" );        both = TRUE;   break;
          case EXP_OP_NXOR     :  code_size = 5;  strcpy( code_format, "%s ~^ %s" );        both = TRUE;   break;
          case EXP_OP_LT       :  code_size = 4;  strcpy( code_format, "%s < %s" );         both = TRUE;   break;
          case EXP_OP_GT       :  code_size = 4;  strcpy( code_format, "%s > %s" );         both = TRUE;   break;
          case EXP_OP_LSHIFT   :  code_size = 5;  strcpy( code_format, "%s << %s" );        both = TRUE;   break;
          case EXP_OP_RSHIFT   :  code_size = 5;  strcpy( code_format, "%s >> %s" );        both = TRUE;   break;
          case EXP_OP_EQ       :  code_size = 5;  strcpy( code_format, "%s == %s" );        both = TRUE;   break;
          case EXP_OP_CEQ      :  code_size = 6;  strcpy( code_format, "%s === %s" );       both = TRUE;   break;
          case EXP_OP_LE       :  code_size = 5;  strcpy( code_format, "%s <= %s" );        both = TRUE;   break;
          case EXP_OP_GE       :  code_size = 5;  strcpy( code_format, "%s >= %s" );        both = TRUE;   break;
          case EXP_OP_NE       :  code_size = 5;  strcpy( code_format, "%s != %s" );        both = TRUE;   break;
          case EXP_OP_CNE      :  code_size = 6;  strcpy( code_format, "%s !== %s" );       both = TRUE;   break;
          case EXP_OP_LOR      :  code_size = 5;  strcpy( code_format, "%s || %s" );        both = TRUE;   break;
          case EXP_OP_LAND     :  code_size = 5;  strcpy( code_format, "%s && %s" );        both = TRUE;   break;
          default              :  break;
        }

      } else {

        switch( SUPPL_OP( expr->suppl ) ) {
          case EXP_OP_XOR      :  code_size = 6;  strcpy( code_format, "(%s ^ %s)" );         both = TRUE;   break;
          case EXP_OP_MULTIPLY :  code_size = 6;  strcpy( code_format, "(%s * %s)" );         both = TRUE;   break;
          case EXP_OP_DIVIDE   :  code_size = 6;  strcpy( code_format, "(%s / %s)" );         both = TRUE;   break;
          case EXP_OP_MOD      :  code_size = 6;  strcpy( code_format, "(%s %% %s)" );        both = TRUE;   break;
          case EXP_OP_ADD      :  code_size = 6;  strcpy( code_format, "(%s + %s)" );         both = TRUE;   break;
          case EXP_OP_SUBTRACT :  code_size = 6;  strcpy( code_format, "(%s - %s)" );         both = TRUE;   break;
          case EXP_OP_AND      :  code_size = 6;  strcpy( code_format, "(%s & %s)" );         both = TRUE;   break;
          case EXP_OP_OR       :  code_size = 6;  strcpy( code_format, "(%s | %s)" );         both = TRUE;   break;
          case EXP_OP_NAND     :  code_size = 7;  strcpy( code_format, "(%s ~& %s)" );        both = TRUE;   break;
          case EXP_OP_NOR      :  code_size = 7;  strcpy( code_format, "(%s ~| %s)" );        both = TRUE;   break;
          case EXP_OP_NXOR     :  code_size = 7;  strcpy( code_format, "(%s ~^ %s)" );        both = TRUE;   break;
          case EXP_OP_LT       :  code_size = 6;  strcpy( code_format, "(%s < %s)" );         both = TRUE;   break;
          case EXP_OP_GT       :  code_size = 6;  strcpy( code_format, "(%s > %s)" );         both = TRUE;   break;
          case EXP_OP_LSHIFT   :  code_size = 7;  strcpy( code_format, "(%s << %s)" );        both = TRUE;   break;
          case EXP_OP_RSHIFT   :  code_size = 7;  strcpy( code_format, "(%s >> %s)" );        both = TRUE;   break;
          case EXP_OP_EQ       :  code_size = 7;  strcpy( code_format, "(%s == %s)" );        both = TRUE;   break;
          case EXP_OP_CEQ      :  code_size = 8;  strcpy( code_format, "(%s === %s)" );       both = TRUE;   break;
          case EXP_OP_LE       :  code_size = 7;  strcpy( code_format, "(%s <= %s)" );        both = TRUE;   break;
          case EXP_OP_GE       :  code_size = 7;  strcpy( code_format, "(%s >= %s)" );        both = TRUE;   break;
          case EXP_OP_NE       :  code_size = 7;  strcpy( code_format, "(%s != %s)" );        both = TRUE;   break;
          case EXP_OP_CNE      :  code_size = 8;  strcpy( code_format, "(%s !== %s)" );       both = TRUE;   break;
          case EXP_OP_LOR      :  code_size = 7;  strcpy( code_format, "(%s || %s)" );        both = TRUE;   break;
          case EXP_OP_LAND     :  code_size = 7;  strcpy( code_format, "(%s && %s)" );        both = TRUE;   break;
          default              :  break;
        }

      }

      if( code_size == 0 ) {

        switch( SUPPL_OP( expr->suppl ) ) {
          case EXP_OP_COND     :  code_size = 4;  strcpy( code_format, "%s ? %s" );           both = TRUE;   break;
          case EXP_OP_COND_SEL :  code_size = 4;  strcpy( code_format, "%s : %s" );           both = TRUE;   break;
          case EXP_OP_UINV     :  code_size = 2;  strcpy( code_format, "~%s" );               both = FALSE;  break;
          case EXP_OP_UAND     :  code_size = 2;  strcpy( code_format, "&%s" );               both = FALSE;  break;
          case EXP_OP_UNOT     :  code_size = 2;  strcpy( code_format, "!%s" );               both = FALSE;  break;
          case EXP_OP_UOR      :  code_size = 2;  strcpy( code_format, "|%s" );               both = FALSE;  break;
          case EXP_OP_UXOR     :  code_size = 2;  strcpy( code_format, "^%s" );               both = FALSE;  break;
          case EXP_OP_UNAND    :  code_size = 3;  strcpy( code_format, "~&%s" );              both = FALSE;  break;
          case EXP_OP_UNOR     :  code_size = 3;  strcpy( code_format, "~|%s" );              both = FALSE;  break;
          case EXP_OP_UNXOR    :  code_size = 3;  strcpy( code_format, "~^%s" );              both = FALSE;  break;
          case EXP_OP_EXPAND   :  code_size = 5;  strcpy( code_format, "{%s{%s}}" );          both = TRUE;   break;
          case EXP_OP_LIST     :  code_size = 3;  strcpy( code_format, "%s, %s" );            both = TRUE;   break;
          case EXP_OP_CONCAT   :  code_size = 3;  strcpy( code_format, "{%s}" );              both = FALSE;  break;
          case EXP_OP_PEDGE    :  code_size = 9;  strcpy( code_format, "posedge %s" );        both = FALSE;  break;
          case EXP_OP_NEDGE    :  code_size = 9;  strcpy( code_format, "negedge %s" );        both = FALSE;  break;
          case EXP_OP_AEDGE    :  code_size = 1;  strcpy( code_format, "%s" );                both = FALSE;  break;
          case EXP_OP_EOR      :  code_size = 5;  strcpy( code_format, "%s or %s" );          both = TRUE;   break;
          case EXP_OP_CASE     :  code_size = 13; strcpy( code_format, "case( %s ) %s :" );   both = TRUE;   break;
          case EXP_OP_CASEX    :  code_size = 14; strcpy( code_format, "casex( %s ) %s :" );  both = TRUE;   break;
          case EXP_OP_CASEZ    :  code_size = 15; strcpy( code_format, "casez( %s ) %s :" );  both = TRUE;   break;
          default:  break;
        }

      }

      if( both ) {

        /* If the left or right code is not on the same line as its parent, we could get a NULL value(s). */
        if( left_code == NULL ) {

          my_code = strdup( " ..." );

        } else {
          
          if( right_code == NULL ) {
            right_code = strdup( " ..." );
          }

          code_size = strlen( right_code ) + strlen( left_code ) + code_size;
          my_code   = (char*)malloc_safe( code_size );
          snprintf( my_code, code_size, code_format, left_code, right_code );

        }

      } else {

        assert( right_code != NULL );
        code_size = strlen( right_code ) + code_size;
        my_code   = (char*)malloc_safe( code_size );
        snprintf( my_code, code_size, code_format, right_code );

      }

    }

    if( right_code != NULL ) {
      free_safe( right_code );
    }

    if( left_code != NULL ) {
      free_safe( left_code );
    }

  }

  return( my_code );

}


/* $Log$
/* Revision 1.21  2002/10/25 03:44:39  phase1geo
/* Fixing bug in comb.c that caused statically allocated string to be exceeded
/* which caused memory corruption problems.  Full regression now passes.
/*
/* Revision 1.20  2002/10/24 23:19:38  phase1geo
/* Making some fixes to report output.  Fixing bugs.  Added long_exp1.v diagnostic
/* to regression suite which finds a current bug in the report underlining
/* functionality.  Need to look into this.
/*
/* Revision 1.19  2002/10/23 03:39:06  phase1geo
/* Fixing bug in MBIT_SEL expressions to calculate the expression widths
/* correctly.  Updated diagnostic testsuite and added diagnostic that
/* found the original bug.  A few documentation updates.
/*
/* Revision 1.18  2002/10/01 13:21:24  phase1geo
/* Fixing bug in report output for single and multi-bit selects.  Also modifying
/* the way that parameters are dealt with to allow proper handling of run-time
/* changing bit selects of parameter values.  Full regression passes again and
/* all report generators have been updated for changes.
/*
/* Revision 1.17  2002/07/16 00:05:31  phase1geo
/* Adding support for replication operator (EXPAND).  All expressional support
/* should now be available.  Added diagnostics to test replication operator.
/* Rewrote binding code to be more efficient with memory use.
/*
/* Revision 1.16  2002/07/14 05:10:42  phase1geo
/* Added support for signal concatenation in score and report commands.  Fixed
/* bugs in this code (and multiplication).
/*
/* Revision 1.15  2002/07/10 16:27:17  phase1geo
/* Fixing output for single/multi-bit select signals in reports.
/*
/* Revision 1.14  2002/07/10 04:57:07  phase1geo
/* Adding bits to vector nibble to allow us to specify what type of input
/* static value was read in so that the output value may be displayed in
/* the same format (DECIMAL, BINARY, OCTAL, HEXIDECIMAL).  Full regression
/* passes.
/*
/* Revision 1.13  2002/07/09 17:27:25  phase1geo
/* Fixing default case item handling and in the middle of making fixes for
/* report outputting.
/*
/* Revision 1.12  2002/07/05 00:37:37  phase1geo
/* Small update to CASE handling in scope to avoid future errors.
/*
/* Revision 1.11  2002/07/05 00:10:18  phase1geo
/* Adding report support for case statements.  Everything outputs fine; however,
/* I want to remove CASE, CASEX and CASEZ expressions from being reported since
/* it causes redundant and misleading information to be displayed in the verbose
/* reports.  New diagnostics to check CASE expressions have been added and pass.
/*
/* Revision 1.10  2002/07/03 21:30:52  phase1geo
/* Fixed remaining issues with always statements.  Full regression is running
/* error free at this point.  Regenerated documentation.  Added EOR expression
/* operation to handle the or expression in event lists.
/*
/* Revision 1.9  2002/07/03 19:54:35  phase1geo
/* Adding/fixing code to properly handle always blocks with the event control
/* structures attached.  Added several new diagnostics to test this ability.
/* always1.v is still failing but the rest are passing.
/*
/* Revision 1.8  2002/07/02 19:52:50  phase1geo
/* Removing unecessary diagnostics.  Cleaning up extraneous output and
/* generating new documentation from source.  Regression passes at the
/* current time.
/*
/* Revision 1.7  2002/07/02 18:42:18  phase1geo
/* Various bug fixes.  Added support for multiple signals sharing the same VCD
/* symbol.  Changed conditional support to allow proper simulation results.
/* Updated VCD parser to allow for symbols containing only alphanumeric characters.
/*
/* Revision 1.6  2002/06/27 21:18:48  phase1geo
/* Fixing report Verilog output.  simple.v verilog diagnostic now passes.
/*
/* Revision 1.5  2002/05/13 03:02:58  phase1geo
/* Adding lines back to expressions and removing them from statements (since the line
/* number range of an expression can be calculated by looking at the expression line
/* numbers).
/* */
