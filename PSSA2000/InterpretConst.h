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


class CInterpretConst
{
public:
	virtual ~CInterpretConst();

	CInterpretConst(const Enum2Val_st *arEnum2Val, int nEnum2Val);
	CInterpretConst(const Bitfield2Val_st *arBitfield2Val, int nBitfield2Val);

	CInterpretConst(const ConstSection_st *arSections, int nSections);

	const TCHAR *Interpret(CONSTVAL_t val, TCHAR *buf, int bufsize);

private:
	void _reset();
	bool is_enum_ctor(){ return m_arSections==&m_EnumC2V; };
	static bool is_unique_mask(CONSTVAL_t oldmasks, CONSTVAL_t newmask);
	bool ensure_unique_masks();

private:
	ConstSection_st m_EnumC2V;
	bool m_using_Bitfield_ctor;

private:
	ConstSection_st *m_arSections;
	int m_nSections;

	void *ptr_unused1, *ptr_unused2, *ptr_unused3, *ptr_unused4;
};


#endif
