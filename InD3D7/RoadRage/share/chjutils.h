#ifndef __chjutils_h_20260313_
#define __chjutils_h_20260313_

// From book code >>>

#define SAFE_DELETE(p)       { if(p) { delete (p);     (p)=NULL; } }
#define SAFE_DELETE_ARRAY(p) { if(p) { delete[] (p);   (p)=NULL; } }
#define SAFE_RELEASE(p)      { if(p) { (p)->Release(); (p)=NULL; } }

void PrintMessage(HWND hwnd,char *message1, char *message2, int message_mode);

void PrintMemAllocated(int mem, char *message);

// From book code <<<


inline float Deg2Rad(float degrees) // degree to radian
{
	return float(degrees * 3.1415926535 / 180.0);
}


#endif
