#define WIN32_LEAN_AND_MEAN
#include <tchar.h>
#include <windows.h>
#include <windowsx.h>
#include <stdlib.h>
#include <stdio.h>
#include "utility.h"

/*
 *  randInt
 *
 *  Generate a random integer between two values
 *
 */

int randInt( 
			int low,	// lower limit, inclusive
			int high	// upper limit, inclusive
			)
{
    int range = high - low;
    int num = rand() % range;
    return( num + low );
}

/*
 *  randDouble
 *
 *  Generate a random double between two values
 *
 */

double randDouble(
				  double low,	// lower limit, inclusive
				  double high	// upper limit, inclusive
				  )
{
    double range = high - low;
    double num = range * (double)rand()/(double)RAND_MAX;
    return( num + low );
}

/*
 *  DebugPrintf
 *
 *  Output a string to the debugger
 *
 */

void DebugPrintf( LPSTR fmt, ... )
{
    char    buff[2560];

    _sntprintf_s( buff, _TRUNCATE, fmt, (LPSTR)(&fmt+1) );

	OutputDebugString( buff );

}

