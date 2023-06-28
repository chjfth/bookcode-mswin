/*
This code is from PAWIN4 p699j .
We use C++ try/catch to catch SEH exception.
To achieve this goal, we need cl.exe option /EHa or /EHs- .
If passing /EHsc, try/catch will NOT catch SEH exceptions.
-- Verified on VC2010 SP1.

Key Windows function: _set_se_translator()
*/

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <eh.h>
#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>

typedef int ExcpFilterDecision_t;

class CSE {
public:
	// Call this function for each thread.
	static void MapSEtoCE() 
	{ 
		_set_se_translator(TranslateSEtoCE); 
	}

	operator DWORD() { return(m_er.ExceptionCode); }

private:
	static void _cdecl 
	TranslateSEtoCE(UINT dwEC, PEXCEPTION_POINTERS pep)
	{
		throw CSE(pep); // this creates a private CSE object
	}

	CSE(PEXCEPTION_POINTERS pep) // a private ctor
	{
		m_er = *pep->ExceptionRecord;
		m_context = *pep->ContextRecord;
	}
	
	EXCEPTION_RECORD m_er; // CPU independent exception information
	CONTEXT m_context;     // CPU dependent exception information
};

void Functastic(int divisor) 
{
	CSE::MapSEtoCE(); // Must be called before any exceptions are raised

	try {
		int x = 0;
		x = 5 / divisor; // Division by zero

		* (PBYTE) 0 = 0; // Access violation
	}
	catch (CSE& se) {
		switch (se) { // Calls the operator DWORD() member function
		case EXCEPTION_ACCESS_VIOLATION:
			// This code handles an access-violation exception
			printf("catch: EXCEPTION_ACCESS_VIOLATION\n");
			break;
		case EXCEPTION_INT_DIVIDE_BY_ZERO:
			// This code handles a division-by-zero exception
			printf("catch: EXCEPTION_INT_DIVIDE_BY_ZERO\n");
			break;
		default:
			// We don't handle any other exceptions
			throw; // Maybe another catch is looking for this
			break; // Never executes
		}
	}
}

int main(int argc, char* argv[])
{
	int divisor = argc>1 ? 1 : 0;

	Functastic(divisor);
	return 0;
}
