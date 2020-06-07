/* this file contains the actual definitions of */
/* the IIDs and CLSIDs */

/* link this file in with the server and any clients */


/* File created by MIDL compiler version 5.01.0164 */
/* at Sun Oct 25 16:51:14 1998
 */
/* Compiler settings for ocr.idl:
    Oicf (OptLev=i2), W1, Zp8, env=Win32, ms_ext, c_ext
    error checks: allocation ref bounds_check enum stub_data 
*/
//@@MIDL_FILE_HEADING(  )
#ifdef __cplusplus
extern "C"{
#endif 


#ifndef __IID_DEFINED__
#define __IID_DEFINED__

typedef struct _IID
{
    unsigned long x;
    unsigned short s1;
    unsigned short s2;
    unsigned char  c[8];
} IID;

#endif // __IID_DEFINED__

#ifndef CLSID_DEFINED
#define CLSID_DEFINED
typedef IID CLSID;
#endif // CLSID_DEFINED

const IID IID_IOcr = {0xD9F23D61,0xA647,0x11d1,{0xAB,0xCD,0x00,0x20,0x78,0x10,0xD5,0xFE}};


const IID IID_ISpell = {0xD9F23D63,0xA647,0x11d1,{0xAB,0xCD,0x00,0x20,0x78,0x10,0xD5,0xFE}};


const IID LIBID_OCREngineLib = {0x36EFD0B1,0xB326,0x11d1,{0xAB,0xDE,0x00,0x20,0x78,0x10,0xD5,0xFE}};


const CLSID CLSID_CoOCREngine = {0xDF22A6B2,0xA58A,0x11D1,{0xAB,0xCC,0x00,0x20,0x78,0x10,0xd5,0xfe}};


#ifdef __cplusplus
}
#endif

