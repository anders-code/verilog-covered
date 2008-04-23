#ifndef __GENPROF_H__
#define __GENPROF_H__

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
 \file    genprof.h
 \author  Trevor Williams  (phase1geo@gmail.com)
 \date    12/10/2007
*/

#include "defines.h"

#define NUM_PROFILES 926

#ifdef DEBUG
#define UNREGISTERED 0
#define ARC_GET_ENTRY_WIDTH 1
#define ARC_SET_WIDTH 2
#define ARC_GET_WIDTH 3
#define ARC_SET_MAX_SIZE 4
#define ARC_GET_MAX_SIZE 5
#define ARC_SET_CURR_SIZE 6
#define ARC_GET_CURR_SIZE 7
#define ARC_SET_SUPPL 8
#define ARC_GET_SUPPL 9
#define ARC_SET_STATES 10
#define ARC_SET_ENTRY_SUPPL 11
#define ARC_GET_ENTRY_SUPPL 12
#define ARC_FIND 13
#define ARC_CREATE 14
#define ARC_ADD 15
#define ARC_COMPARE_STATES 16
#define ARC_COMPARE_ALL_STATES 17
#define ARC_STATE_TOTAL 18
#define ARC_STATE_HITS 19
#define ARC_TRANSITION_TOTAL 20
#define ARC_TRANSITION_HITS 21
#define ARC_GET_STATS 22
#define ARC_DB_WRITE 23
#define ARC_READ_GET_NEXT_VALUE 24
#define ARC_DB_READ 25
#define ARC_STATE_TO_STRING 26
#define ARC_DB_MERGE 27
#define ARC_MERGE 28
#define ARC_GET_STATES 29
#define ARC_GET_TRANSITIONS 30
#define ARC_ARE_ANY_EXCLUDED 31
#define ARC_DEALLOC 32
#define ASSERTION_PARSE 33
#define ASSERTION_PARSE_ATTR 34
#define ASSERTION_GET_STATS 35
#define ASSERTION_DISPLAY_INSTANCE_SUMMARY 36
#define ASSERTION_INSTANCE_SUMMARY 37
#define ASSERTION_DISPLAY_FUNIT_SUMMARY 38
#define ASSERTION_FUNIT_SUMMARY 39
#define ASSERTION_DISPLAY_VERBOSE 40
#define ASSERTION_INSTANCE_VERBOSE 41
#define ASSERTION_FUNIT_VERBOSE 42
#define ASSERTION_REPORT 43
#define ASSERTION_GET_FUNIT_SUMMARY 44
#define ASSERTION_COLLECT 45
#define ASSERTION_GET_COVERAGE 46
#define ATTRIBUTE_CREATE 47
#define ATTRIBUTE_PARSE 48
#define ATTRIBUTE_DEALLOC 49
#define BIND_ADD 50
#define BIND_APPEND_FSM_EXPR 51
#define BIND_REMOVE 52
#define BIND_FIND_SIG_NAME 53
#define BIND_PARAM 54
#define BIND_SIGNAL 55
#define BIND_TASK_FUNCTION_PORTS 56
#define BIND_TASK_FUNCTION_NAMEDBLOCK 57
#define BIND_PERFORM 58
#define BIND_DEALLOC 59
#define CODEGEN_CREATE_EXPR_HELPER 60
#define CODEGEN_CREATE_EXPR 61
#define CODEGEN_GEN_EXPR 62
#define COMBINATION_CALC_DEPTH 63
#define COMBINATION_DOES_MULTI_EXP_NEED_UL 64
#define COMBINATION_MULTI_EXPR_CALC 65
#define COMBINATION_IS_EXPR_MULTI_NODE 66
#define COMBINATION_GET_TREE_STATS 67
#define COMBINATION_RESET_COUNTED_EXPRS 68
#define COMBINATION_RESET_COUNTED_EXPR_TREE 69
#define COMBINATION_GET_STATS 70
#define COMBINATION_GET_FUNIT_SUMMARY 71
#define COMBINATION_DISPLAY_INSTANCE_SUMMARY 72
#define COMBINATION_INSTANCE_SUMMARY 73
#define COMBINATION_DISPLAY_FUNIT_SUMMARY 74
#define COMBINATION_FUNIT_SUMMARY 75
#define COMBINATION_DRAW_LINE 76
#define COMBINATION_DRAW_CENTERED_LINE 77
#define COMBINATION_UNDERLINE_TREE 78
#define COMBINATION_PREP_LINE 79
#define COMBINATION_UNDERLINE 80
#define COMBINATION_UNARY 81
#define COMBINATION_EVENT 82
#define COMBINATION_TWO_VARS 83
#define COMBINATION_MULTI_VAR_EXPRS 84
#define COMBINATION_MULTI_EXPR_OUTPUT_LENGTH 85
#define COMBINATION_MULTI_EXPR_OUTPUT 86
#define COMBINATION_MULTI_VARS 87
#define COMBINATION_GET_MISSED_EXPR 88
#define COMBINATION_LIST_MISSED 89
#define COMBINATION_OUTPUT_EXPR 90
#define COMBINATION_DISPLAY_VERBOSE 91
#define COMBINATION_INSTANCE_VERBOSE 92
#define COMBINATION_FUNIT_VERBOSE 93
#define COMBINATION_COLLECT 94
#define COMBINATION_GET_EXCLUDE_LIST 95
#define COMBINATION_GET_EXPRESSION 96
#define COMBINATION_GET_COVERAGE 97
#define COMBINATION_REPORT 98
#define DB_CREATE 99
#define DB_CLOSE 100
#define DB_CHECK_FOR_TOP_MODULE 101
#define DB_WRITE 102
#define DB_READ 103
#define DB_MERGE_FUNITS 104
#define DB_SCALE_TO_PRECISION 105
#define DB_CREATE_UNNAMED_SCOPE 106
#define DB_IS_UNNAMED_SCOPE 107
#define DB_SET_TIMESCALE 108
#define DB_GET_CURR_FUNIT 109
#define DB_ADD_INSTANCE 110
#define DB_ADD_MODULE 111
#define DB_END_MODULE 112
#define DB_ADD_FUNCTION_TASK_NAMEDBLOCK 113
#define DB_END_FUNCTION_TASK_NAMEDBLOCK 114
#define DB_ADD_DECLARED_PARAM 115
#define DB_ADD_OVERRIDE_PARAM 116
#define DB_ADD_VECTOR_PARAM 117
#define DB_ADD_DEFPARAM 118
#define DB_ADD_SIGNAL 119
#define DB_ADD_ENUM 120
#define DB_END_ENUM_LIST 121
#define DB_ADD_TYPEDEF 122
#define DB_FIND_SIGNAL 123
#define DB_ADD_GEN_ITEM_BLOCK 124
#define DB_FIND_GEN_ITEM 125
#define DB_FIND_TYPEDEF 126
#define DB_GET_CURR_GEN_BLOCK 127
#define DB_CURR_SIGNAL_COUNT 128
#define DB_CREATE_EXPRESSION 129
#define DB_BIND_EXPR_TREE 130
#define DB_CREATE_EXPR_FROM_STATIC 131
#define DB_ADD_EXPRESSION 132
#define DB_CREATE_SENSITIVITY_LIST 133
#define DB_PARALLELIZE_STATEMENT 134
#define DB_CREATE_STATEMENT 135
#define DB_ADD_STATEMENT 136
#define DB_REMOVE_STATEMENT_FROM_CURRENT_FUNIT 137
#define DB_REMOVE_STATEMENT 138
#define DB_CONNECT_STATEMENT_TRUE 139
#define DB_CONNECT_STATEMENT_FALSE 140
#define DB_GEN_ITEM_CONNECT_TRUE 141
#define DB_GEN_ITEM_CONNECT_FALSE 142
#define DB_GEN_ITEM_CONNECT 143
#define DB_STATEMENT_CONNECT 144
#define DB_CREATE_ATTR_PARAM 145
#define DB_PARSE_ATTRIBUTE 146
#define DB_REMOVE_STMT_BLKS_CALLING_STATEMENT 147
#define DB_GEN_CURR_INST_SCOPE 148
#define DB_SYNC_CURR_INSTANCE 149
#define DB_SET_VCD_SCOPE 150
#define DB_VCD_UPSCOPE 151
#define DB_ASSIGN_SYMBOL 152
#define DB_SET_SYMBOL_CHAR 153
#define DB_SET_SYMBOL_STRING 154
#define DB_DO_TIMESTEP 155
#define ENUMERATE_ADD_ITEM 156
#define ENUMERATE_END_LIST 157
#define ENUMERATE_RESOLVE 158
#define ENUMERATE_DEALLOC 159
#define ENUMERATE_DEALLOC_LIST 160
#define EXPRESSION_CREATE_TMP_VECS 161
#define EXPRESSION_CREATE_VALUE 162
#define EXPRESSION_CREATE 163
#define EXPRESSION_SET_VALUE 164
#define EXPRESSION_SET_SIGNED 165
#define EXPRESSION_RESIZE 166
#define EXPRESSION_GET_ID 167
#define EXPRESSION_GET_FIRST_LINE_EXPR 168
#define EXPRESSION_GET_LAST_LINE_EXPR 169
#define EXPRESSION_GET_CURR_DIMENSION 170
#define EXPRESSION_FIND_RHS_SIGS 171
#define EXPRESSION_FIND_PARAMS 172
#define EXPRESSION_FIND_ULINE_ID 173
#define EXPRESSION_FIND_EXPR 174
#define EXPRESSION_CONTAINS_EXPR_CALLING_STMT 175
#define EXPRESSION_GET_ROOT_STATEMENT 176
#define EXPRESSION_ASSIGN_EXPR_IDS 177
#define EXPRESSION_DB_WRITE 178
#define EXPRESSION_DB_WRITE_TREE 179
#define EXPRESSION_DB_READ 180
#define EXPRESSION_DB_MERGE 181
#define EXPRESSION_MERGE 182
#define EXPRESSION_STRING_OP 183
#define EXPRESSION_STRING 184
#define EXPRESSION_OP_FUNC__XOR 185
#define EXPRESSION_OP_FUNC__XOR_A 186
#define EXPRESSION_OP_FUNC__MULTIPLY 187
#define EXPRESSION_OP_FUNC__MULTIPLY_A 188
#define EXPRESSION_OP_FUNC__DIVIDE 189
#define EXPRESSION_OP_FUNC__DIVIDE_A 190
#define EXPRESSION_OP_FUNC__MOD 191
#define EXPRESSION_OP_FUNC__MOD_A 192
#define EXPRESSION_OP_FUNC__ADD 193
#define EXPRESSION_OP_FUNC__ADD_A 194
#define EXPRESSION_OP_FUNC__SUBTRACT 195
#define EXPRESSION_OP_FUNC__SUB_A 196
#define EXPRESSION_OP_FUNC__AND 197
#define EXPRESSION_OP_FUNC__AND_A 198
#define EXPRESSION_OP_FUNC__OR 199
#define EXPRESSION_OP_FUNC__OR_A 200
#define EXPRESSION_OP_FUNC__NAND 201
#define EXPRESSION_OP_FUNC__NOR 202
#define EXPRESSION_OP_FUNC__NXOR 203
#define EXPRESSION_OP_FUNC__LT 204
#define EXPRESSION_OP_FUNC__GT 205
#define EXPRESSION_OP_FUNC__LSHIFT 206
#define EXPRESSION_OP_FUNC__LSHIFT_A 207
#define EXPRESSION_OP_FUNC__RSHIFT 208
#define EXPRESSION_OP_FUNC__RSHIFT_A 209
#define EXPRESSION_OP_FUNC__ARSHIFT 210
#define EXPRESSION_OP_FUNC__ARSHIFT_A 211
#define EXPRESSION_OP_FUNC__EQ 212
#define EXPRESSION_OP_FUNC__CEQ 213
#define EXPRESSION_OP_FUNC__LE 214
#define EXPRESSION_OP_FUNC__GE 215
#define EXPRESSION_OP_FUNC__NE 216
#define EXPRESSION_OP_FUNC__CNE 217
#define EXPRESSION_OP_FUNC__LOR 218
#define EXPRESSION_OP_FUNC__LAND 219
#define EXPRESSION_OP_FUNC__COND 220
#define EXPRESSION_OP_FUNC__COND_SEL 221
#define EXPRESSION_OP_FUNC__UINV 222
#define EXPRESSION_OP_FUNC__UAND 223
#define EXPRESSION_OP_FUNC__UNOT 224
#define EXPRESSION_OP_FUNC__UOR 225
#define EXPRESSION_OP_FUNC__UXOR 226
#define EXPRESSION_OP_FUNC__UNAND 227
#define EXPRESSION_OP_FUNC__UNOR 228
#define EXPRESSION_OP_FUNC__UNXOR 229
#define EXPRESSION_OP_FUNC__NULL 230
#define EXPRESSION_OP_FUNC__SIG 231
#define EXPRESSION_OP_FUNC__SBIT 232
#define EXPRESSION_OP_FUNC__MBIT 233
#define EXPRESSION_OP_FUNC__EXPAND 234
#define EXPRESSION_OP_FUNC__LIST 235
#define EXPRESSION_OP_FUNC__CONCAT 236
#define EXPRESSION_OP_FUNC__PEDGE 237
#define EXPRESSION_OP_FUNC__NEDGE 238
#define EXPRESSION_OP_FUNC__AEDGE 239
#define EXPRESSION_OP_FUNC__EOR 240
#define EXPRESSION_OP_FUNC__SLIST 241
#define EXPRESSION_OP_FUNC__DELAY 242
#define EXPRESSION_OP_FUNC__TRIGGER 243
#define EXPRESSION_OP_FUNC__CASE 244
#define EXPRESSION_OP_FUNC__CASEX 245
#define EXPRESSION_OP_FUNC__CASEZ 246
#define EXPRESSION_OP_FUNC__DEFAULT 247
#define EXPRESSION_OP_FUNC__BASSIGN 248
#define EXPRESSION_OP_FUNC__FUNC_CALL 249
#define EXPRESSION_OP_FUNC__TASK_CALL 250
#define EXPRESSION_OP_FUNC__NB_CALL 251
#define EXPRESSION_OP_FUNC__FORK 252
#define EXPRESSION_OP_FUNC__JOIN 253
#define EXPRESSION_OP_FUNC__DISABLE 254
#define EXPRESSION_OP_FUNC__REPEAT 255
#define EXPRESSION_OP_FUNC__EXPONENT 256
#define EXPRESSION_OP_FUNC__PASSIGN 257
#define EXPRESSION_OP_FUNC__MBIT_POS 258
#define EXPRESSION_OP_FUNC__MBIT_NEG 259
#define EXPRESSION_OP_FUNC__NEGATE 260
#define EXPRESSION_OP_FUNC__IINC 261
#define EXPRESSION_OP_FUNC__PINC 262
#define EXPRESSION_OP_FUNC__IDEC 263
#define EXPRESSION_OP_FUNC__PDEC 264
#define EXPRESSION_OP_FUNC__DLY_ASSIGN 265
#define EXPRESSION_OP_FUNC__DLY_OP 266
#define EXPRESSION_OP_FUNC__REPEAT_DLY 267
#define EXPRESSION_OP_FUNC__WAIT 268
#define EXPRESSION_OP_FUNC__FINISH 269
#define EXPRESSION_OP_FUNC__STOP 270
#define EXPRESSION_OPERATE 271
#define EXPRESSION_OPERATE_RECURSIVELY 272
#define EXPRESSION_IS_STATIC_ONLY 273
#define EXPRESSION_IS_ASSIGNED 274
#define EXPRESSION_IS_BIT_SELECT 275
#define EXPRESSION_IS_LAST_SELECT 276
#define EXPRESSION_IS_IN_RASSIGN 277
#define EXPRESSION_SET_ASSIGNED 278
#define EXPRESSION_ASSIGN 279
#define EXPRESSION_DEALLOC 280
#define FSM_CREATE 281
#define FSM_ADD_ARC 282
#define FSM_CREATE_TABLES 283
#define FSM_DB_WRITE 284
#define FSM_DB_READ 285
#define FSM_DB_MERGE 286
#define FSM_MERGE 287
#define FSM_TABLE_SET 288
#define FSM_GET_STATS 289
#define FSM_GET_FUNIT_SUMMARY 290
#define FSM_GATHER_SIGNALS 291
#define FSM_COLLECT 292
#define FSM_GET_COVERAGE 293
#define FSM_DISPLAY_INSTANCE_SUMMARY 294
#define FSM_INSTANCE_SUMMARY 295
#define FSM_DISPLAY_FUNIT_SUMMARY 296
#define FSM_FUNIT_SUMMARY 297
#define FSM_DISPLAY_STATE_VERBOSE 298
#define FSM_DISPLAY_ARC_VERBOSE 299
#define FSM_DISPLAY_VERBOSE 300
#define FSM_INSTANCE_VERBOSE 301
#define FSM_FUNIT_VERBOSE 302
#define FSM_REPORT 303
#define FSM_DEALLOC 304
#define FSM_ARG_PARSE_STATE 305
#define FSM_ARG_PARSE 306
#define FSM_ARG_PARSE_VALUE 307
#define FSM_ARG_PARSE_TRANS 308
#define FSM_ARG_PARSE_ATTR 309
#define FSM_VAR_ADD 310
#define FSM_VAR_IS_OUTPUT_STATE 311
#define FSM_VAR_BIND_EXPR 312
#define FSM_VAR_ADD_EXPR 313
#define FSM_VAR_BIND_STMT 314
#define FSM_VAR_BIND_ADD 315
#define FSM_VAR_STMT_ADD 316
#define FSM_VAR_BIND 317
#define FSM_VAR_DEALLOC 318
#define FSM_VAR_REMOVE 319
#define FSM_VAR_CLEANUP 320
#define FUNC_ITER_DISPLAY 321
#define FUNC_ITER_SORT 322
#define FUNC_ITER_COUNT_STMT_ITERS 323
#define FUNC_ITER_ADD_STMT_ITERS 324
#define FUNC_ITER_INIT 325
#define FUNC_ITER_GET_NEXT_STATEMENT 326
#define FUNC_ITER_DEALLOC 327
#define FUNIT_INIT 328
#define FUNIT_CREATE 329
#define FUNIT_GET_CURR_MODULE 330
#define FUNIT_GET_CURR_MODULE_SAFE 331
#define FUNIT_GET_CURR_FUNCTION 332
#define FUNIT_GET_CURR_TASK 333
#define FUNIT_GET_PORT_COUNT 334
#define FUNIT_FIND_PARAM 335
#define FUNIT_FIND_SIGNAL 336
#define FUNIT_REMOVE_STMT_BLKS_CALLING_STMT 337
#define FUNIT_GEN_TASK_FUNCTION_NAMEDBLOCK_NAME 338
#define FUNIT_SIZE_ELEMENTS 339
#define FUNIT_DB_WRITE 340
#define FUNIT_DB_READ 341
#define FUNIT_DB_MERGE 342
#define FUNIT_MERGE 343
#define FUNIT_FLATTEN_NAME 344
#define FUNIT_FIND_BY_ID 345
#define FUNIT_IS_TOP_MODULE 346
#define FUNIT_IS_UNNAMED 347
#define FUNIT_IS_UNNAMED_CHILD_OF 348
#define FUNIT_IS_CHILD_OF 349
#define FUNIT_DISPLAY_SIGNALS 350
#define FUNIT_DISPLAY_EXPRESSIONS 351
#define STATEMENT_ADD_THREAD 352
#define FUNIT_PUSH_THREADS 353
#define STATEMENT_DELETE_THREAD 354
#define FUNIT_CLEAN 355
#define FUNIT_DEALLOC 356
#define GEN_ITEM_STRINGIFY 357
#define GEN_ITEM_DISPLAY 358
#define GEN_ITEM_DISPLAY_BLOCK_HELPER 359
#define GEN_ITEM_DISPLAY_BLOCK 360
#define GEN_ITEM_COMPARE 361
#define GEN_ITEM_FIND 362
#define GEN_ITEM_REMOVE_IF_CONTAINS_EXPR_CALLING_STMT 363
#define GEN_ITEM_GET_GENVAR 364
#define GEN_ITEM_VARNAME_CONTAINS_GENVAR 365
#define GEN_ITEM_CALC_SIGNAL_NAME 366
#define GEN_ITEM_CREATE_EXPR 367
#define GEN_ITEM_CREATE_SIG 368
#define GEN_ITEM_CREATE_STMT 369
#define GEN_ITEM_CREATE_INST 370
#define GEN_ITEM_CREATE_TFN 371
#define GEN_ITEM_CREATE_BIND 372
#define GEN_ITEM_RESIZE_STMTS_AND_SIGS 373
#define GEN_ITEM_ASSIGN_EXPR_IDS 374
#define GEN_ITEM_DB_WRITE 375
#define GEN_ITEM_DB_WRITE_EXPR_TREE 376
#define GEN_ITEM_CONNECT 377
#define GEN_ITEM_RESOLVE 378
#define GEN_ITEM_BIND 379
#define GENERATE_RESOLVE 380
#define GENERATE_REMOVE_STMT_HELPER 381
#define GENERATE_REMOVE_STMT 382
#define GEN_ITEM_DEALLOC 383
#define FUNIT_GET_LIST 384
#define FUNIT_GET_FILENAME 385
#define FUNIT_GET_START_AND_END_LINES 386
#define INFO_INITIALIZE 387
#define INFO_DB_WRITE 388
#define INFO_DB_READ 389
#define ARGS_DB_READ 390
#define INFO_DEALLOC 391
#define INSTANCE_DISPLAY_TREE_HELPER 392
#define INSTANCE_DISPLAY_TREE 393
#define INSTANCE_CREATE 394
#define INSTANCE_GEN_SCOPE 395
#define INSTANCE_COMPARE 396
#define INSTANCE_FIND_SCOPE 397
#define INSTANCE_FIND_BY_FUNIT 398
#define INSTANCE_ADD_CHILD 399
#define INSTANCE_COPY 400
#define INSTANCE_ATTACH_CHILD 401
#define INSTANCE_PARSE_ADD 402
#define INSTANCE_RESOLVE_INST 403
#define INSTANCE_RESOLVE_HELPER 404
#define INSTANCE_RESOLVE 405
#define INSTANCE_READ_ADD 406
#define INSTANCE_DB_WRITE 407
#define INSTANCE_FLATTEN_HELPER 408
#define INSTANCE_FLATTEN 409
#define INSTANCE_REMOVE_STMT_BLKS_CALLING_STMT 410
#define INSTANCE_REMOVE_PARMS_WITH_EXPR 411
#define INSTANCE_DEALLOC_SINGLE 412
#define INSTANCE_DEALLOC_TREE 413
#define INSTANCE_DEALLOC 414
#define STMT_ITER_RESET 415
#define STMT_ITER_COPY 416
#define STMT_ITER_NEXT 417
#define STMT_ITER_REVERSE 418
#define STMT_ITER_FIND_HEAD 419
#define STMT_ITER_GET_NEXT_IN_ORDER 420
#define STMT_ITER_GET_LINE_BEFORE 421
#define LEXER_KEYWORD_1995_CODE 422
#define LEXER_KEYWORD_2001_CODE 423
#define LEXER_KEYWORD_SV_CODE 424
#define LEXER_CSTRING_A 425
#define LEXER_KEYWORD_A 426
#define LEXER_ESCAPED_NAME 427
#define LEXER_SYSTEM_CALL 428
#define LINE_DIRECTIVE 429
#define LINE_DIRECTIVE2 430
#define PROCESS_TIMESCALE_TOKEN 431
#define PROCESS_TIMESCALE 432
#define LEXER_YYWRAP 433
#define RESET_LEXER 434
#define CHECK_FOR_PRAGMA 435
#define LINE_GET_STATS 436
#define LINE_COLLECT 437
#define LINE_GET_FUNIT_SUMMARY 438
#define LINE_DISPLAY_INSTANCE_SUMMARY 439
#define LINE_INSTANCE_SUMMARY 440
#define LINE_DISPLAY_FUNIT_SUMMARY 441
#define LINE_FUNIT_SUMMARY 442
#define LINE_DISPLAY_VERBOSE 443
#define LINE_INSTANCE_VERBOSE 444
#define LINE_FUNIT_VERBOSE 445
#define LINE_REPORT 446
#define STR_LINK_ADD 447
#define STMT_LINK_ADD_HEAD 448
#define STMT_LINK_ADD_TAIL 449
#define STMT_LINK_MERGE 450
#define EXP_LINK_ADD 451
#define SIG_LINK_ADD 452
#define FSM_LINK_ADD 453
#define FUNIT_LINK_ADD 454
#define GITEM_LINK_ADD 455
#define INST_LINK_ADD 456
#define STR_LINK_FIND 457
#define STMT_LINK_FIND 458
#define EXP_LINK_FIND 459
#define SIG_LINK_FIND 460
#define FSM_LINK_FIND 461
#define FUNIT_LINK_FIND 462
#define GITEM_LINK_FIND 463
#define INST_LINK_FIND_BY_SCOPE 464
#define INST_LINK_FIND_BY_FUNIT 465
#define STR_LINK_REMOVE 466
#define EXP_LINK_REMOVE 467
#define GITEM_LINK_REMOVE 468
#define FUNIT_LINK_REMOVE 469
#define INST_LINK_FLATTEN 470
#define STR_LINK_DELETE_LIST 471
#define STMT_LINK_UNLINK 472
#define STMT_LINK_DELETE_LIST 473
#define EXP_LINK_DELETE_LIST 474
#define SIG_LINK_DELETE_LIST 475
#define FSM_LINK_DELETE_LIST 476
#define FUNIT_LINK_DELETE_LIST 477
#define GITEM_LINK_DELETE_LIST 478
#define INST_LINK_DELETE_LIST 479
#define VCDID 480
#define VCD_CALLBACK 481
#define LXT_PARSE 482
#define LXT2_RD_EXPAND_INTEGER_TO_BITS 483
#define LXT2_RD_EXPAND_BITS_TO_INTEGER 484
#define LXT2_RD_ITER_RADIX 485
#define LXT2_RD_ITER_RADIX0 486
#define LXT2_RD_BUILD_RADIX 487
#define LXT2_RD_REGENERATE_PROCESS_MASK 488
#define LXT2_RD_PROCESS_BLOCK 489
#define LXT2_RD_INIT 490
#define LXT2_RD_CLOSE 491
#define LXT2_RD_GET_FACNAME 492
#define LXT2_RD_ITER_BLOCKS 493
#define LXT2_RD_LIMIT_TIME_RANGE 494
#define LXT2_RD_UNLIMIT_TIME_RANGE 495
#define MEMORY_GET_STAT 496
#define MEMORY_GET_STATS 497
#define MEMORY_GET_FUNIT_SUMMARY 498
#define MEMORY_CREATE_PDIM_BIT_ARRAY 499
#define MEMORY_GET_MEM_COVERAGE 500
#define MEMORY_GET_COVERAGE 501
#define MEMORY_COLLECT 502
#define MEMORY_DISPLAY_TOGGLE_INSTANCE_SUMMARY 503
#define MEMORY_TOGGLE_INSTANCE_SUMMARY 504
#define MEMORY_DISPLAY_AE_INSTANCE_SUMMARY 505
#define MEMORY_AE_INSTANCE_SUMMARY 506
#define MEMORY_DISPLAY_TOGGLE_FUNIT_SUMMARY 507
#define MEMORY_TOGGLE_FUNIT_SUMMARY 508
#define MEMORY_DISPLAY_AE_FUNIT_SUMMARY 509
#define MEMORY_AE_FUNIT_SUMMARY 510
#define MEMORY_DISPLAY_MEMORY 511
#define MEMORY_DISPLAY_VERBOSE 512
#define MEMORY_INSTANCE_VERBOSE 513
#define MEMORY_FUNIT_VERBOSE 514
#define MEMORY_REPORT 515
#define COMMAND_MERGE 516
#define OBFUSCATE_SET_MODE 517
#define OBFUSCATE_NAME 518
#define OBFUSCATE_DEALLOC 519
#define OVL_IS_ASSERTION_NAME 520
#define OVL_IS_ASSERTION_MODULE 521
#define OVL_IS_COVERAGE_POINT 522
#define OVL_ADD_ASSERTIONS_TO_NO_SCORE_LIST 523
#define OVL_GET_FNIT_STATS 524
#define OVL_GET_COVERAGE_POINT 525
#define OVL_DISPLAY_VERBOSE 526
#define OVL_COLLECT 527
#define OVL_GET_COVERAGE 528
#define MOD_PARM_FIND 529
#define MOD_PARM_FIND_EXPR_AND_REMOVE 530
#define MOD_PARM_ADD 531
#define INST_PARM_FIND 532
#define INST_PARM_ADD 533
#define INST_PARM_ADD_GENVAR 534
#define INST_PARM_BIND 535
#define DEFPARAM_ADD 536
#define DEFPARAM_DEALLOC 537
#define PARAM_FIND_AND_SET_EXPR_VALUE 538
#define PARAM_SET_SIG_SIZE 539
#define PARAM_SIZE_FUNCTION 540
#define PARAM_EXPR_EVAL 541
#define PARAM_HAS_OVERRIDE 542
#define PARAM_HAS_DEFPARAM 543
#define PARAM_RESOLVE_DECLARED 544
#define PARAM_RESOLVE_OVERRIDE 545
#define PARAM_RESOLVE 546
#define PARAM_DB_WRITE 547
#define MOD_PARM_DEALLOC 548
#define INST_PARM_DEALLOC 549
#define PARSE_READLINE 550
#define PARSE_DESIGN 551
#define PARSE_AND_SCORE_DUMPFILE 552
#define PARSER_PORT_DECLARATION_A 553
#define PARSER_PORT_DECLARATION_B 554
#define PARSER_PORT_DECLARATION_C 555
#define PARSER_STATIC_EXPR_PRIMARY_A 556
#define PARSER_STATIC_EXPR_PRIMARY_B 557
#define PARSER_EXPRESSION_LIST_A 558
#define PARSER_EXPRESSION_LIST_B 559
#define PARSER_EXPRESSION_LIST_C 560
#define PARSER_EXPRESSION_LIST_D 561
#define PARSER_IDENTIFIER_A 562
#define PARSER_GENERATE_CASE_ITEM_A 563
#define PARSER_GENERATE_CASE_ITEM_B 564
#define PARSER_GENERATE_CASE_ITEM_C 565
#define PARSER_STATEMENT_BEGIN_A 566
#define PARSER_STATEMENT_FORK_A 567
#define PARSER_STATEMENT_FOR_A 568
#define PARSER_CASE_ITEM_A 569
#define PARSER_CASE_ITEM_B 570
#define PARSER_CASE_ITEM_C 571
#define PARSER_DELAY_VALUE_A 572
#define PARSER_DELAY_VALUE_B 573
#define PARSER_PARAMETER_VALUE_BYNAME_A 574
#define PARSER_GATE_INSTANCE_A 575
#define PARSER_GATE_INSTANCE_B 576
#define PARSER_GATE_INSTANCE_C 577
#define PARSER_GATE_INSTANCE_D 578
#define PARSER_LIST_OF_NAMES_A 579
#define PARSER_LIST_OF_NAMES_B 580
#define VLERROR 581
#define VLWARN 582
#define PARSER_DEALLOC_SIG_RANGE 583
#define PARSER_COPY_CURR_RANGE 584
#define PARSER_COPY_RANGE_TO_CURR_RANGE 585
#define PARSER_EXPLICITLY_SET_CURR_RANGE 586
#define PARSER_IMPLICITLY_SET_CURR_RANGE 587
#define PARSER_CHECK_GENERATION 588
#define PERF_GEN_STATS 589
#define PERF_OUTPUT_MOD_STATS 590
#define PERF_OUTPUT_INST_REPORT_HELPER 591
#define PERF_OUTPUT_INST_REPORT 592
#define DEF_LOOKUP 593
#define IS_DEFINED 594
#define DEF_MATCH 595
#define DEF_START 596
#define DEFINE_MACRO 597
#define DO_DEFINE 598
#define DEF_IS_DONE 599
#define DEF_FINISH 600
#define DEF_UNDEFINE 601
#define INCLUDE_FILENAME 602
#define DO_INCLUDE 603
#define YYWRAP 604
#define RESET_PPLEXER 605
#define RACE_BLK_CREATE 606
#define RACE_FIND_HEAD_STATEMENT_CONTAINING_STATEMENT_HELPER 607
#define RACE_FIND_HEAD_STATEMENT_CONTAINING_STATEMENT 608
#define RACE_GET_HEAD_STATEMENT 609
#define RACE_FIND_HEAD_STATEMENT 610
#define RACE_CALC_STMT_BLK_TYPE 611
#define RACE_CALC_EXPR_ASSIGNMENT 612
#define RACE_CALC_ASSIGNMENTS 613
#define RACE_HANDLE_RACE_CONDITION 614
#define RACE_CHECK_ASSIGNMENT_TYPES 615
#define RACE_CHECK_ONE_BLOCK_ASSIGNMENT 616
#define RACE_CHECK_RACE_COUNT 617
#define RACE_CHECK_MODULES 618
#define RACE_DB_WRITE 619
#define RACE_DB_READ 620
#define RACE_GET_STATS 621
#define RACE_REPORT_SUMMARY 622
#define RACE_REPORT_VERBOSE 623
#define RACE_REPORT 624
#define RACE_COLLECT_LINES 625
#define RACE_BLK_DELETE_LIST 626
#define REENTRANT_COUNT_AFU_BITS 627
#define REENTRANT_STORE_DATA_BITS 628
#define REENTRANT_RESTORE_DATA_BITS 629
#define REENTRANT_CREATE 630
#define REENTRANT_DEALLOC 631
#define REPORT_PARSE_METRICS 632
#define REPORT_PARSE_ARGS 633
#define REPORT_GATHER_INSTANCE_STATS 634
#define REPORT_GATHER_FUNIT_STATS 635
#define REPORT_PRINT_HEADER 636
#define REPORT_GENERATE 637
#define REPORT_READ_CDD_AND_READY 638
#define REPORT_CLOSE_CDD 639
#define REPORT_SAVE_CDD 640
#define COMMAND_REPORT 641
#define SCOPE_FIND_FUNIT_FROM_SCOPE 642
#define SCOPE_FIND_PARAM 643
#define SCOPE_FIND_SIGNAL 644
#define SCOPE_FIND_TASK_FUNCTION_NAMEDBLOCK 645
#define SCOPE_GET_PARENT_FUNIT 646
#define SCOPE_GET_PARENT_MODULE 647
#define SCORE_GENERATE_TOP_VPI_MODULE 648
#define SCORE_GENERATE_PLI_TAB_FILE 649
#define READ_COMMAND_FILE 650
#define SCORE_PARSE_DEFINE 651
#define SCORE_ADD_ARG 652
#define SCORE_PARSE_ARGS 653
#define COMMAND_SCORE 654
#define SEARCH_INIT 655
#define SEARCH_ADD_INCLUDE_PATH 656
#define SEARCH_ADD_DIRECTORY_PATH 657
#define SEARCH_ADD_FILE 658
#define SEARCH_ADD_NO_SCORE_FUNIT 659
#define SEARCH_ADD_EXTENSIONS 660
#define SEARCH_FREE_LISTS 661
#define SIM_CURRENT_THREAD 662
#define SIM_THREAD_POP_HEAD 663
#define SIM_THREAD_INSERT_INTO_DELAY_QUEUE 664
#define SIM_THREAD_PUSH 665
#define SIM_EXPR_CHANGED 666
#define SIM_CREATE_THREAD 667
#define SIM_ADD_THREAD 668
#define SIM_KILL_THREAD 669
#define SIM_KILL_THREAD_WITH_FUNIT 670
#define SIM_ADD_STATICS 671
#define SIM_EXPRESSION 672
#define SIM_THREAD 673
#define SIM_SIMULATE 674
#define SIM_INITIALIZE 675
#define SIM_STOP 676
#define SIM_FINISH 677
#define SIM_DEALLOC 678
#define STATISTIC_CREATE 679
#define STATISTIC_MERGE 680
#define STATISTIC_IS_EMPTY 681
#define STATISTIC_DEALLOC 682
#define STATEMENT_CREATE 683
#define STATEMENT_QUEUE_ADD 684
#define STATEMENT_QUEUE_COMPARE 685
#define STATEMENT_SIZE_ELEMENTS 686
#define STATEMENT_DB_WRITE 687
#define STATEMENT_DB_WRITE_TREE 688
#define STATEMENT_DB_WRITE_EXPR_TREE 689
#define STATEMENT_DB_READ 690
#define STATEMENT_ASSIGN_EXPR_IDS 691
#define STATEMENT_CONNECT 692
#define STATEMENT_GET_LAST_LINE_HELPER 693
#define STATEMENT_GET_LAST_LINE 694
#define STATEMENT_FIND_RHS_SIGS 695
#define STATEMENT_FIND_HEAD_STATEMENT 696
#define STATEMENT_FIND_STATEMENT 697
#define STATEMENT_CONTAINS_EXPR_CALLING_STMT 698
#define STATEMENT_DEALLOC_RECURSIVE 699
#define STATEMENT_DEALLOC 700
#define STATIC_EXPR_GEN_UNARY 701
#define STATIC_EXPR_GEN 702
#define STATIC_EXPR_CALC_LSB_AND_WIDTH_PRE 703
#define STATIC_EXPR_CALC_LSB_AND_WIDTH_POST 704
#define STATIC_EXPR_DEALLOC 705
#define STRUCT_UNION_LENGTH 706
#define STRUCT_UNION_ADD_MEMBER 707
#define STRUCT_UNION_ADD_MEMBER_VOID 708
#define STRUCT_UNION_ADD_MEMBER_SIG 709
#define STRUCT_UNION_ADD_MEMBER_TYPEDEF 710
#define STRUCT_UNION_ADD_MEMBER_ENUM 711
#define STRUCT_UNION_ADD_MEMBER_STRUCT_UNION 712
#define STRUCT_UNION_CREATE 713
#define STRUCT_UNION_MEMBER_DEALLOC 714
#define STRUCT_UNION_DEALLOC 715
#define STRUCT_UNION_DEALLOC_LIST 716
#define SYMTABLE_ADD_SYM_SIG 717
#define SYMTABLE_INIT 718
#define SYMTABLE_CREATE 719
#define SYMTABLE_ADD 720
#define SYMTABLE_SET_VALUE 721
#define SYMTABLE_ASSIGN 722
#define SYMTABLE_DEALLOC 723
#define TCL_FUNC_GET_RACE_REASON_MSGS 724
#define TCL_FUNC_GET_FUNIT_LIST 725
#define TCL_FUNC_GET_INSTANCES 726
#define TCL_FUNC_GET_INSTANCE_LIST 727
#define TCL_FUNC_GET_FILENAME 728
#define TCL_FUNC_GET_FUNIT_START_AND_END 729
#define TCL_FUNC_COLLECT_UNCOVERED_LINES 730
#define TCL_FUNC_COLLECT_COVERED_LINES 731
#define TCL_FUNC_COLLECT_RACE_LINES 732
#define TCL_FUNC_COLLECT_UNCOVERED_TOGGLES 733
#define TCL_FUNC_COLLECT_COVERED_TOGGLES 734
#define TCL_FUNC_COLLECT_MEMORIES 735
#define TCL_FUNC_GET_TOGGLE_COVERAGE 736
#define TCL_FUNC_GET_MEMORY_COVERAGE 737
#define TCL_FUNC_COLLECT_COMBS 738
#define TCL_FUNC_GET_COMB_EXPRESSION 739
#define TCL_FUNC_GET_COMB_COVERAGE 740
#define TCL_FUNC_COLLECT_FSMS 741
#define TCL_FUNC_GET_FSM_COVERAGE 742
#define TCL_FUNC_COLLECT_ASSERTIONS 743
#define TCL_FUNC_GET_ASSERT_COVERAGE 744
#define TCL_FUNC_OPEN_CDD 745
#define TCL_FUNC_CLOSE_CDD 746
#define TCL_FUNC_SAVE_CDD 747
#define TCL_FUNC_MERGE_CDD 748
#define TCL_FUNC_GET_LINE_SUMMARY 749
#define TCL_FUNC_GET_TOGGLE_SUMMARY 750
#define TCL_FUNC_GET_MEMORY_SUMMARY 751
#define TCL_FUNC_GET_COMB_SUMMARY 752
#define TCL_FUNC_GET_FSM_SUMMARY 753
#define TCL_FUNC_GET_ASSERT_SUMMARY 754
#define TCL_FUNC_PREPROCESS_VERILOG 755
#define TCL_FUNC_GET_SCORE_PATH 756
#define TCL_FUNC_GET_INCLUDE_PATHNAME 757
#define TCL_FUNC_GET_GENERATION 758
#define TCL_FUNC_SET_LINE_EXCLUDE 759
#define TCL_FUNC_SET_TOGGLE_EXCLUDE 760
#define TCL_FUNC_SET_MEMORY_EXCLUDE 761
#define TCL_FUNC_SET_COMB_EXCLUDE 762
#define TCL_FUNC_FSM_EXCLUDE 763
#define TCL_FUNC_SET_ASSERT_EXCLUDE 764
#define TCL_FUNC_GENERATE_REPORT 765
#define TCL_FUNC_INITIALIZE 766
#define TOGGLE_GET_STATS 767
#define TOGGLE_COLLECT 768
#define TOGGLE_GET_COVERAGE 769
#define TOGGLE_GET_FUNIT_SUMMARY 770
#define TOGGLE_DISPLAY_INSTANCE_SUMMARY 771
#define TOGGLE_INSTANCE_SUMMARY 772
#define TOGGLE_DISPLAY_FUNIT_SUMMARY 773
#define TOGGLE_FUNIT_SUMMARY 774
#define TOGGLE_DISPLAY_VERBOSE 775
#define TOGGLE_INSTANCE_VERBOSE 776
#define TOGGLE_FUNIT_VERBOSE 777
#define TOGGLE_REPORT 778
#define TREE_ADD 779
#define TREE_FIND 780
#define TREE_REMOVE 781
#define TREE_DEALLOC 782
#define SET_OUTPUT_SUPPRESSION 783
#define CHECK_OPTION_VALUE 784
#define IS_VARIABLE 785
#define IS_FUNC_UNIT 786
#define IS_LEGAL_FILENAME 787
#define GET_BASENAME 788
#define GET_DIRNAME 789
#define DIRECTORY_EXISTS 790
#define DIRECTORY_LOAD 791
#define FILE_EXISTS 792
#define UTIL_READLINE 793
#define SUBSTITUTE_ENV_VARS 794
#define SCOPE_EXTRACT_FRONT 795
#define SCOPE_EXTRACT_BACK 796
#define SCOPE_EXTRACT_SCOPE 797
#define SCOPE_GEN_PRINTABLE 798
#define SCOPE_COMPARE 799
#define SCOPE_LOCAL 800
#define CONVERT_FILE_TO_MODULE 801
#define GET_NEXT_VFILE 802
#define GEN_SPACE 803
#define GET_FUNIT_TYPE 804
#define CALC_MISS_PERCENT 805
#define VCD_PARSE_DEF_IGNORE 806
#define VCD_PARSE_DEF_VAR 807
#define VCD_PARSE_DEF_SCOPE 808
#define VCD_PARSE_DEF 809
#define VCD_PARSE_SIM_VECTOR 810
#define VCD_PARSE_SIM_IGNORE 811
#define VCD_PARSE_SIM 812
#define VCD_PARSE 813
#define VECTOR_INIT 814
#define VECTOR_CREATE 815
#define VECTOR_COPY 816
#define VECTOR_COPY_RANGE 817
#define VECTOR_CLONE 818
#define VECTOR_DB_WRITE 819
#define VECTOR_DB_READ 820
#define VECTOR_DB_MERGE 821
#define VECTOR_MERGE 822
#define VECTOR_GET_EVAL_A 823
#define VECTOR_GET_EVAL_B 824
#define VECTOR_GET_EVAL_C 825
#define VECTOR_GET_EVAL_D 826
#define VECTOR_GET_EVAL_AB_COUNT 827
#define VECTOR_GET_EVAL_ABC_COUNT 828
#define VECTOR_GET_EVAL_ABCD_COUNT 829
#define VECTOR_GET_TOGGLE01_UINT32 830
#define VECTOR_GET_TOGGLE10_UINT32 831
#define VECTOR_DISPLAY_TOGGLE01_UINT32 832
#define VECTOR_DISPLAY_TOGGLE10_UINT32 833
#define VECTOR_TOGGLE_COUNT 834
#define VECTOR_MEM_RW_COUNT 835
#define VECTOR_SET_ASSIGNED 836
#define VECTOR_SET_COVERAGE_AND_ASSIGN 837
#define VECTOR_SET_VALUE 838
#define VECTOR_PART_SELECT 839
#define VECTOR_SET_UNARY_EVALS 840
#define VECTOR_SET_AND_COMB_EVALS 841
#define VECTOR_SET_OR_COMB_EVALS 842
#define VECTOR_SET_OTHER_COMB_EVALS 843
#define VECTOR_BIT_FILL 844
#define VECTOR_IS_UKNOWN 845
#define VECTOR_IS_NOT_ZERO 846
#define VECTOR_SET_TO_X 847
#define VECTOR_IS_SET 848
#define VECTOR_TO_INT 849
#define VECTOR_TO_UINT64 850
#define VECTOR_TO_SIM_TIME 851
#define VECTOR_FROM_INT 852
#define VECTOR_FROM_UINT64 853
#define VECTOR_SET_STATIC 854
#define VECTOR_TO_STRING 855
#define VECTOR_FROM_STRING 856
#define VECTOR_VCD_ASSIGN 857
#define VECTOR_BITWISE_AND_OP 858
#define VECTOR_BITWISE_NAND_OP 859
#define VECTOR_BITWISE_OR_OP 860
#define VECTOR_BITWISE_NOR_OP 861
#define VECTOR_BITWISE_XOR_OP 862
#define VECTOR_BITWISE_NXOR_OP 863
#define VECTOR_OP_LT 864
#define VECTOR_OP_LE 865
#define VECTOR_OP_GT 866
#define VECTOR_OP_GE 867
#define VECTOR_OP_EQ 868
#define VECTOR_OP_CEQ 869
#define VECTOR_OP_CXEQ 870
#define VECTOR_OP_CZEQ 871
#define VECTOR_OP_NE 872
#define VECTOR_OP_CNE 873
#define VECTOR_OP_LOR 874
#define VECTOR_OP_LAND 875
#define VECTOR_OP_LSHIFT 876
#define VECTOR_OP_RSHIFT 877
#define VECTOR_OP_ARSHIFT 878
#define VECTOR_OP_ADD 879
#define VECTOR_OP_NEGATE 880
#define VECTOR_OP_SUBTRACT 881
#define VECTOR_OP_MULTIPLY 882
#define VECTOR_OP_DIVIDE 883
#define VECTOR_OP_MODULUS 884
#define VECTOR_OP_INC 885
#define VECTOR_OP_DEC 886
#define VECTOR_UNARY_INV 887
#define VECTOR_UNARY_AND 888
#define VECTOR_UNARY_NAND 889
#define VECTOR_UNARY_OR 890
#define VECTOR_UNARY_NOR 891
#define VECTOR_UNARY_XOR 892
#define VECTOR_UNARY_NXOR 893
#define VECTOR_UNARY_NOT 894
#define VECTOR_OP_EXPAND 895
#define VECTOR_OP_LIST 896
#define VECTOR_DEALLOC_VALUE 897
#define VECTOR_DEALLOC 898
#define SYM_VALUE_STORE 899
#define ADD_SYM_VALUES_TO_SIM 900
#define COVERED_VALUE_CHANGE 901
#define COVERED_END_OF_SIM 902
#define COVERED_CB_ERROR_HANDLER 903
#define GEN_NEXT_SYMBOL 904
#define COVERED_CREATE_VALUE_CHANGE_CB 905
#define COVERED_PARSE_TASK_FUNC 906
#define COVERED_PARSE_SIGNALS 907
#define COVERED_PARSE_INSTANCE 908
#define COVERED_SIM_CALLTF 909
#define COVERED_REGISTER 910
#define VSIGNAL_INIT 911
#define VSIGNAL_CREATE 912
#define VSIGNAL_CREATE_VEC 913
#define VSIGNAL_DUPLICATE 914
#define VSIGNAL_DB_WRITE 915
#define VSIGNAL_DB_READ 916
#define VSIGNAL_DB_MERGE 917
#define VSIGNAL_MERGE 918
#define VSIGNAL_PROPAGATE 919
#define VSIGNAL_VCD_ASSIGN 920
#define VSIGNAL_ADD_EXPRESSION 921
#define VSIGNAL_FROM_STRING 922
#define VSIGNAL_CALC_WIDTH_FOR_EXPR 923
#define VSIGNAL_CALC_LSB_FOR_EXPR 924
#define VSIGNAL_DEALLOC 925

extern profiler profiles[NUM_PROFILES];
#endif

extern unsigned int profile_index;

#endif

