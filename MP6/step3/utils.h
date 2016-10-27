/*
     File        : utils.h

     Author      : Riccardo Bettati
     Modified    : 04/11/30

     Description : Various definitions (NULL/BOOL/TRUE/FALSE) and

*/

#ifndef _utils_h_
#define _utils_h_

/*---------------------------------------------------------------*/
/* GENERAL CONSTANTS */

#ifndef NULL
#   define NULL 0
#endif

#ifdef BOOLEAN
# undef BOOLEAN
#endif
#define BOOLEAN int

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif


#endif



