#ifndef __Util_h__
#define __Util_h__

//
// Util.h - Shared utilities
//
//#include <strstrea.h> // old style, only for VC6
#include <strstream>
using namespace std; // OK for VC6 SP6 and VC2010

namespace Util
{
	void Trace(char* szLabel, char* szText, HRESULT hr) ;

	void vTrace(HRESULT hr, char *szLabel, char *fmt, ...);
	void vTrace(char *szLabel, char *fmt, ...);

	void ErrorMessage(HRESULT hr, bool useMsgbox=false) ;
} ;


//
// Overloaded insertion operator for converting from
// Unicode (wchar_t) to non-Unicode.
//
ostream& operator<< ( ostream& os, const wchar_t* wsz ) ;

#endif // __Util_h__