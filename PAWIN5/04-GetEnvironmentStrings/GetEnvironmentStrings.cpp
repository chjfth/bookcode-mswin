#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <locale.h>
#include <windows.h>
#include <strsafe.h>

// From PAWIN5 p77-78

void DumpEnvStrings() 
{
	PTSTR pEnvBlock = GetEnvironmentStrings();
	// Parse the block with the following format:
	// =::=::\
	// =...
	// var=value\0
	// ...
	// var=value\0\0

	// Note that some other strings might begin with '='.
	// Here is an example when the application is started from a network share.
	// [0] =::=::\
	// [1] =C:=C:\Windows\System32
	// [2] =ExitCode=00000000
	//
	TCHAR szName[MAX_PATH] = {};
	TCHAR szValue[MAX_PATH] = {};
	PTSTR pszCurrent = pEnvBlock;
	HRESULT hr = S_OK;
	PCTSTR pszPos = NULL;
	int current = 0;
	while (pszCurrent != NULL) 
	{
		// Skip the meaningless strings like:
		// "=::=::\"
		if (*pszCurrent != TEXT('=')) 
		{
			// Look for '=' separator.
			pszPos = _tcschr(pszCurrent, TEXT('='));

			// Point now to the first character of the value.
			pszPos++;

			// Copy the variable name.
			size_t cbNameLength = // Without the' ='
				(size_t)pszPos - (size_t)pszCurrent - sizeof(TCHAR);

			hr = StringCbCopyN(szName, MAX_PATH, pszCurrent, cbNameLength);
			if (FAILED(hr)) {
				break;
			}

			// Copy the variable value with the last NULL character
			// and allow truncation because this is for UI only.
			hr = StringCchCopyN(szValue, MAX_PATH, pszPos, _tcslen(pszPos)+1);
			if (SUCCEEDED(hr)) {
				_tprintf(TEXT("[%u] %s=%s\r\n"), current, szName, szValue);
			} 
			else { 
				// something wrong happened; check for truncation.
				if (hr == STRSAFE_E_INSUFFICIENT_BUFFER) {
					_tprintf(TEXT("[%u] %s=%s...\r\n"), current, szName, szValue);
				} else { // This should never occur.
					_tprintf(
						TEXT("[%u] %s=???\r\n"), current, szName
						);
					break;
				}
			}
		} 
		else 
		{
			_tprintf(TEXT("[%u] %s\r\n"), current, pszCurrent);
		}

		// Next variable please.
		current++;

		// Move to the end of the string.
		while (*pszCurrent != TEXT('\0'))
			pszCurrent++;

		pszCurrent++;

		// Check if it was not the last string.
		if (*pszCurrent == TEXT('\0'))
			break;
	};

	// Don't forget to free the memory.
	FreeEnvironmentStrings(pEnvBlock);
}

void DumpEnvVariables(PTSTR pEnvBlock[]) 
{
	int current = 0;
	PTSTR* pElement = (PTSTR*)pEnvBlock;
	PTSTR pCurrent = NULL;
	while (pElement != NULL) {
		pCurrent = (PTSTR)(*pElement);
		if (pCurrent == NULL) {
			// No more environment variable.
			pElement = NULL;
		} else {
			_tprintf(TEXT("[%u] %s\r\n"), current, pCurrent);
			current++;
			pElement++;
		}
	}
}

int _tmain(int argc, TCHAR* argv[], TCHAR *env[])
{
	setlocale(LC_ALL, "");

	printf("==== GetEnvironmentStrings() / FreeEnvironmentStrings()\n");
	DumpEnvStrings();

	printf("\n\n");
	printf("==== Dump by main()'s env parameter.\n");
	DumpEnvVariables(env);
	return 0;
}
