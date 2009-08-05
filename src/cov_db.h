#ifndef __COV_DB_H__
#define __COV_DB_H__

/*
 Copyright (c) 2006-2009 Trevor Williams

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
 \file     cov_db.h
 \author   Trevor Williams  (phase1geo@gmail.com)
 \date     8/5/2009
 \brief    Contains functions for writing and reading contents of
           coverage database file.
*/

#include <stdio.h>

#include "defines.h"


/*!
 List of VCD tokens.
*/
enum cov_db_tokens {
  T_EOF,            /*!< 0 : End-of-file */
  T_STRING,         /*!< 1 : Other string */
  T_UNKNOWN         /*!< 2 : Unknown */
};


/*!
 \brief Writes the contents of the given coverage database.
*/
void cov_db_write(
  FILE*   ofile,
  cov_db* cdb
);

/*!
 \brief Reads the contents of the given coverage database, allocates and stores
        the read information into the given coverage database structure.
*/
void cov_db_read(
  const char* fname,
  cov_db*     cdb
);

#endif

