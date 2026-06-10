

template<typename FORMAT>
struct DataXTraits<ClockMode_et, FORMAT>
{
	static ClockMode_et FromString(const TCHAR* s)
	{
		CInterpretConst& itcClockMode = get_itcClockMode();
		bool is_err = false;
		ClockMode_et cm = (ClockMode_et)itcClockMode.NamesToVal(s, &is_err);
		// -- TODO? Thrown exception when is_err==true?
		
		return cm;
	}

	static Sdring ToString(ClockMode_et val)
	{
		CInterpretConst& itcClockMode = get_itcClockMode();
		Sdring text = ITCSnv(val, itcClockMode);
		return text;
	}
};

template<typename FORMAT>
struct DataXTraits<RECT, FORMAT>
{
	// Represent a RECT value into "left,top,right,down" string-form.

	static RECT FromString(const TCHAR* s)
	{
		RECT rect = {};
		_stscanf_s(s, _T("%d,%d,%d,%d"), &rect.left, &rect.top, &rect.right, &rect.bottom);
		return rect;
	}

	static Sdring ToString(const RECT& rect)
	{
		TCHAR sz[80];
		snTprintf(sz, _T("%d,%d,%d,%d"), rect.left, rect.top, rect.right, rect.bottom);
		return sz;
	}
};


template<>
struct DataXTraits<int, Format_int_as_HHMMSS>
{
	static int FromString(const TCHAR* s)
	{
		if (!s)
			return 0;

		return HMS_to_Seconds(s, false);
	}

	static Sdring ToString(int seconds)
	{
		return Seconds_to_HMS(seconds);
	}
};

template<>
struct DataXTraits<Uint, Format_COLORREF_as_RGB>
{
	static Uint FromString(const TCHAR* s)
	{
		if (!s)
			return 0;

		Uint r = 0, g = 0, b = 0;
		_stscanf_s(s, _T("RGB(%u,%u,%u)"), &r, &g, &b);
		return RGB(r, g, b);
	}

	static Sdring ToString(Uint colorref)
	{
		TCHAR sz[32];
		Uchar r = colorref, g = colorref>>8, b = colorref>>16;
		snTprintf(sz, _T("RGB(%u,%u,%u)"), r, g, b);
		return Sdring(sz);
	}
};

