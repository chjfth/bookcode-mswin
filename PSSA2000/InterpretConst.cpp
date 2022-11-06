#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <assert.h>
#include <stdio.h>
#include "InterpretConst.h"

void CInterpretConst::_reset()
{
	m_EnumC2V.SectionMask=0; 
	m_EnumC2V.arConst2Val=nullptr; 
	m_EnumC2V.nConst2Val=0; 

	m_using_Bitfield_ctor = false;

	m_arSections = nullptr;
	m_nSections = 0;
}

CInterpretConst::~CInterpretConst()
{
	if(m_using_Bitfield_ctor)
		delete []m_arSections;
}

CInterpretConst::CInterpretConst(const ConstSection_st *arSections, int nSections)
{
	_reset();

	m_arSections = const_cast<ConstSection_st*>(arSections);
	m_nSections = nSections;

	ensure_unique_masks();
}

CInterpretConst::CInterpretConst(const Enum2Val_st *arEnum2Val, int nEnum2Val)
{
	_reset();

	m_EnumC2V.SectionMask = 0xFFFFffff;
	m_EnumC2V.arConst2Val = reinterpret_cast<const Const2Val_st*>(arEnum2Val);
	m_EnumC2V.nConst2Val = nEnum2Val;

	m_arSections = &m_EnumC2V;
	m_nSections = 1;

	ensure_unique_masks();
}

CInterpretConst::CInterpretConst(const Bitfield2Val_st *arBitfield2Val, int nBitfield2Val)
{
	_reset();

	m_arSections = new ConstSection_st[nBitfield2Val];
	if(!m_arSections)
		return;

	m_nSections = nBitfield2Val;

	int i;
	for(i=0; i<nBitfield2Val; i++)
	{
		assert(arBitfield2Val[i].ConstVal != 0); // a common input mistake

		m_arSections[i].SectionMask = arBitfield2Val[i].ConstVal;
		m_arSections[i].arConst2Val = reinterpret_cast<const Const2Val_st*>(arBitfield2Val+i);
		m_arSections[i].nConst2Val = 1;
	}

	m_using_Bitfield_ctor = true;

	ensure_unique_masks();
}

bool CInterpretConst::is_unique_mask(CONSTVAL_t oldmasks, CONSTVAL_t newmask)
{
	// oldmasks and newmask must NOT have overlapped bits set.

	if( (oldmasks^newmask) == (oldmasks|newmask) )
		return true;
	else
		return false;
}

bool CInterpretConst::ensure_unique_masks()
{
	CONSTVAL_t accum_masks = 0;
	int sec;
	for(sec=0; sec<m_nSections; sec++)
	{
		bool ok_unique_mask	= is_unique_mask(accum_masks, m_arSections[sec].SectionMask);
		assert(ok_unique_mask);

		if(!ok_unique_mask)
			return false;
		
		accum_masks |= m_arSections[sec].SectionMask;
	}

	return true;
}

const TCHAR *CInterpretConst::Interpret(CONSTVAL_t input_val, TCHAR *buf, int bufsize)
{
	if(bufsize<=0)
		return NULL;

	buf[0] = 0;

	CONSTVAL_t remain_val = input_val;

	int sec = 0;
	for(sec=0; sec<m_nSections; sec++)
	{
		CONSTVAL_t secval = input_val & m_arSections[sec].SectionMask;

		auto c2v = m_arSections[sec].arConst2Val;
		int i;
		for(i=0; i<m_arSections[sec].nConst2Val; i++)
		{
			if(c2v[i].ConstVal==secval)
			{
				if(c2v[i].ConstName)
				{
					_sntprintf_s(buf, bufsize, _TRUNCATE, _T("%s%s|"), buf, c2v[i].ConstName);
				}

				break;
			}
		}

		// No designated name exists, we consider it an unrecognized value from
		// the running env, so should present its hex-value instead of mute on it.
		if(i==m_arSections[sec].nConst2Val)
		{
			if(!m_using_Bitfield_ctor || secval!=0)
			{
				_sntprintf_s(buf, bufsize, _TRUNCATE, 
					is_enum_ctor() ? _T("%s%u|") : _T("%s0x%X|"), 
					buf, secval);
			}
			else
			{
				// If m_using_Bitfield_ctor and secval==0, 
				// this 0-value is of course not consider unrecognized, so mute it here. 

				assert(secval==0);
			}
		}

		remain_val &= ~m_arSections[sec].SectionMask;
	}

	if(remain_val)
	{
		// present unrecognized value to user
		_sntprintf_s(buf, bufsize, _TRUNCATE, _T("%s0x%X|"), buf, remain_val);
	}

	// Remove trailing '|'
	int slen = (int)_tcslen(buf);
	if(slen>0 && buf[slen-1]=='|')
		buf[--slen] = '\0';

	// If output string empty, fill a '0'.
	if(buf[0]=='\0')
		buf[0] = '0';

	return buf;
}

const Const2Val_st Demo1Sec1[] =
{
	{ _T("SEC1_VAL0"), 0 },
	{ _T("SEC1_VAL1"), 1 },
	{ _T("SEC1_VAL2"), 2 },
	{ _T("SEC1_VAL3"), 3 },
};
const Const2Val_st Demo1Sec2[] =
{
	{ _T("SEC2_VAL0"), 0<<2 },
	{ _T("SEC2_VAL1"), 1<<2 },
	{ _T("SEC2_VAL2"), 2<<2 },
	{ nullptr, 3<<2 },
	{ _T("SEC2_VAL4"), 4<<2 },
	{ _T("SEC2_VAL5"), 5<<2 },
	{ _T("SEC2_VAL6"), 6<<2 },
//	{ _T("SEC2_VAL7"), 7<<2 },
};

const ConstSection_st ar_Demo1CS[] =
{
	{ 0x3, Demo1Sec1, ARRAYSIZE(Demo1Sec1) },
	{ 0x7<<2, Demo1Sec2, ARRAYSIZE(Demo1Sec2) },
};

CInterpretConst g_itc1(ar_Demo1CS, ARRAYSIZE(ar_Demo1CS));

void test_itc1()
{
	TCHAR buf[80] = {};
	int arVals[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 16, 17, 0xff};
	int i;
	for(i=0; i<ARRAYSIZE(arVals); i++)
	{
		_tprintf(_T("%3d : %s\n"), arVals[i],
			g_itc1.Interpret(arVals[i], buf, ARRAYSIZE(buf)));
	}
/*
  0 : SEC1_VAL0|SEC2_VAL0
  1 : SEC1_VAL1|SEC2_VAL0
  2 : SEC1_VAL2|SEC2_VAL0
  3 : SEC1_VAL3|SEC2_VAL0
  4 : SEC1_VAL0|SEC2_VAL1
  5 : SEC1_VAL1|SEC2_VAL1
  6 : SEC1_VAL2|SEC2_VAL1
  7 : SEC1_VAL3|SEC2_VAL1
  8 : SEC1_VAL0|SEC2_VAL2
  9 : SEC1_VAL1|SEC2_VAL2
 10 : SEC1_VAL2|SEC2_VAL2
 11 : SEC1_VAL3|SEC2_VAL2
 12 : SEC1_VAL0
 16 : SEC1_VAL0|SEC2_VAL4
 17 : SEC1_VAL1|SEC2_VAL4
255 : SEC1_VAL3|0x1C|0xE0
*/
}

void test_itc_enum()
{
	const Enum2Val_st arE2V[] =
	{
		{_T("Sunday"), 0},
		{_T("Monday"), 1},
		{_T("Tuesday"), 2},
		{_T("Wednesday"), 3},
		{_T("Thursday"), 4},
		{_T("Friday"), 5},
		{_T("Saturday"), 6},
	};

	CInterpretConst itc_weekday(arE2V, ARRAYSIZE(arE2V));

	TCHAR buf[80] = {};
	int arVals[] = {0,1,2, 5,6,7};
	for(int i=0; i<ARRAYSIZE(arVals); i++)
	{
		itc_weekday.Interpret(arVals[i], buf, ARRAYSIZE(buf));
		_tprintf(_T("%3d : %s\n"), arVals[i], buf);
	}
/*
  0 : Sunday
  1 : Monday
  2 : Tuesday
  5 : Friday
  6 : Saturday
  7 : 7
*/
}

void test_itc_bitfields()
{
	const Bitfield2Val_st arBf2V[] =
	{
		{_T("bit0"), 1<<0},
		{_T("bit1"), 1<<1},
		{_T("bit2"), 1<<2},
		{_T("bit4"), 1<<4},
		{_T("bit5and6"), 32+64}, // bit 5&6 must both be set to signify this name
	};

	CInterpretConst itc_bitfields(arBf2V, ARRAYSIZE(arBf2V));

	TCHAR buf[80] = {};
	int arVals[] = {0,1,2,3,4,5,6,7,8,9, 16,17, 32,64,96};
	for(int i=0; i<ARRAYSIZE(arVals); i++)
	{
		itc_bitfields.Interpret(arVals[i], buf, ARRAYSIZE(buf));
		_tprintf(_T("%3d : %s\n"), arVals[i], buf);
	}
/*
  0 : 0
  1 : bit0
  2 : bit1
  3 : bit0|bit1
  4 : bit2
  5 : bit0|bit2
  6 : bit1|bit2
  7 : bit0|bit1|bit2
  8 : 0x8
  9 : bit0|0x8
 16 : bit4
 17 : bit0|bit4
 32 : 0x20
 64 : 0x40
 96 : bit5and6
*/
}

void Test_bad_masks()
{
	const Bitfield2Val_st arBf2V[] =
	{
		{_T("bit0"), 1},
		{_T("bit1"), 2},
		{_T("bit1and2"), 6}, 
	};

	CInterpretConst itc_bitfields(arBf2V, ARRAYSIZE(arBf2V));
}

void test_itc()
{
	printf("==== Multi-section\n");
	test_itc1();

	printf("==== Enums\n");
	test_itc_enum();

	printf("==== Bitfields\n");
	test_itc_bitfields();

	printf("==== Test_bad_masks\n"); Test_bad_masks(); // This will assert()
}
