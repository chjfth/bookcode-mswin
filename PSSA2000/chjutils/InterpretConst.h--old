#ifndef __InterpretConst_h_
#define __InterpretConst_h_

#include <tchar.h>

typedef unsigned long CONSTVAL_t;

struct Enum2Val_st
{
	const TCHAR *EnumName;
	CONSTVAL_t ConstVal;
};

struct Bitfield2Val_st
{
	const TCHAR *BitfieldName;
	CONSTVAL_t ConstVal;
};

struct Const2Val_st
{
	const TCHAR *ConstName;
	CONSTVAL_t ConstVal;
};

struct ConstSection_st
{
	CONSTVAL_t SectionMask;
	const Const2Val_st *arConst2Val;
	int nConst2Val; // element of arConst2Val[]
};

enum ITC_DisplayFormat_et
{
	ITC_DF_NameOnly = 0,
	ITC_DF_NameAndValue = 1,
};

class CItcString
{
public:
	CItcString(int need_chars)
	{
#ifdef ITC_DEBUG_PRINT
		_tprintf(_T("[%p]CItcString() ctor: %d\n"), this, need_chars);
#endif // ITC_DEBUG_PRINT
		
		m_chars = need_chars;
		m_str = new TCHAR[m_chars];
	}

	CItcString(const CItcString &itc)
	{
#ifdef ITC_DEBUG_PRINT
		_tprintf(_T("[%p]CItcString() ctor= %s\n"), this, itc.m_str);
#endif // ITC_DEBUG_PRINT
	
		m_chars = itc.m_chars;
		m_str = new TCHAR[m_chars];
		_sntprintf_s(m_str, m_chars, _TRUNCATE, _T("%s"), itc.m_str);
	}

	~CItcString()
	{
#ifdef ITC_DEBUG_PRINT
		_tprintf(_T("[%p]CItcString() dtor: %s\n"), this, m_str);
#endif // ITC_DEBUG_PRINT
		delete m_str;
		m_str = 0;
		m_chars = 0;
	}

	TCHAR *get(){ return m_str; }

	void put(const TCHAR *pstr)
	{
		if(m_str)
		{
			_sntprintf_s(m_str, m_chars, _TRUNCATE, _T("%s"), pstr);
		}
	}

	int bufsize(){ return m_chars; }

	operator TCHAR *(){ return m_str; }

private:
	TCHAR *m_str;
	int m_chars;
};


class CInterpretConst
{
public:
	virtual ~CInterpretConst();

	CInterpretConst(const Enum2Val_st *arEnum2Val, int nEnum2Val, 
		const TCHAR *valfmt=nullptr);
	
	CInterpretConst(const Bitfield2Val_st *arBitfield2Val, int nBitfield2Val, 
		const TCHAR *valfmt=nullptr);

	CInterpretConst(const ConstSection_st *arSections, int nSections, 
		const TCHAR *valfmt=nullptr);

	const TCHAR *Interpret(CONSTVAL_t val, ITC_DisplayFormat_et dispfmt,
		TCHAR *buf, int bufsize);

	CItcString Interpret(CONSTVAL_t val, ITC_DisplayFormat_et dispfmt);

	bool SetValFmt(const TCHAR *fmt);

private:
	enum { 
		WholeDisplayMaxChars = 400,
		OneDisplayMaxChars = 100,
		FmtSpecMaxChars = 10,
	};

private:
	void _reset(const TCHAR *valfmt);
	bool is_enum_ctor(){ return m_arSections==&m_EnumC2V; };
	static bool is_unique_mask(CONSTVAL_t oldmasks, CONSTVAL_t newmask);
	bool ensure_unique_masks();

	TCHAR *FormatOneDisplay(const TCHAR *szVal, CONSTVAL_t val, ITC_DisplayFormat_et dispfmt,
		TCHAR obuf[], int obufsize);

private:
	ConstSection_st m_EnumC2V;
	bool m_using_Bitfield_ctor;

	const TCHAR *m_valfmt; 
	// -- format string when showing value, "%d", "%X", "0x%04X" etc

private:
	ConstSection_st *m_arSections;
	int m_nSections;

	void *ptr_unused1, *ptr_unused2, *ptr_unused3, *ptr_unused4;
};


#define ITC_NAMEPAIR(macroname) { _T( #macroname ) , macroname }

#define ITCS(val, itcobj) itcobj.Interpret((CONSTVAL_t)val, ITC_DF_NameOnly).get()
// -- the "return" of ITCS() macro can be passed as snprintf's variadic params
// Note: ITCS() cannot be used in __try{} block, otherwise we'll get Compiler Error C2712.

#define ITCS1(val, itcobj) itcobj.Interpret((CONSTVAL_t)val, ITC_DF_NameAndValue).get()

#endif
