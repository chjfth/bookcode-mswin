#include <string.h>
#include <iostream.h>

template <class T>
T inline max(T a, T b)
{
	return (a > b) ? a : b;
}

template<> char* max(char* a, char* b)
{
	return (strlen(a) > strlen(b)) ? a : b;
}

void main()
{
	cout << max(10, 12) << endl;
	cout << max(10.8, 12.5) << endl;
	cout << max('a', 'b') << endl;
	cout << max(true, false) << endl;
	cout << max("hello", "goodbye") << endl;
}