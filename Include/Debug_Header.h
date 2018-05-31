/******************************************************************************
 *
 * File Name:  Debug_Header.h
 *
 * Description:	This is a file that contains all debugging level macros.
 *
 ******************************************************************************/

#ifndef DEBUG_HEADER_H_
#define DEBUG_HEADER_H_

#define ENABLE_DEBUG_LEVEL 0

/* Lowest debug level */

#if ENABLE_DEBUG_LEVEL>=1
//#define DEBUG_LEVEL_1(X...) do{ printf("FILE:- %s  ",__FILE__); printf("API:- %s  ",__FUNCTION__); printf("LINE:- %d  ", __LINE__); printf ("DBGMSG:- "X); printf ("\n");}while(0)
#define DEBUG_LEVEL_1(X...) do{ printf("LINE:- %d  ", __LINE__); printf ("DBGMSG:- "X); printf ("\n");}while(0)
#else
#define DEBUG_LEVEL_1(X...)
#endif

/* Highest debug level, Enabling this enables all lower level debugs */

#if ENABLE_DEBUG_LEVEL>=2
#define DEBUG_LEVEL_2(X...) do{ printf("FILE:- %s  ",__FILE__); printf("API:- %s  ",__FUNCTION__); printf("LINE:- %d  ", __LINE__); printf ("DBGMSG:- "X); printf ("\n");}while(0)
#else
#define DEBUG_LEVEL_2(X...)
#endif

#endif /* DEBUG_HEADER_H_ */

