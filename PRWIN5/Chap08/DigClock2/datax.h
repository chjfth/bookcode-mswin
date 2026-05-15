

/*
const Enum2Val_st _e2v_ClockMode[] =
{
	ITC_NAMEPAIR(CM_WallTime),
	ITC_NAMEPAIR(CM_Countdown),
};
CInterpretConst itc_ClockMode(_e2v_ClockMode, ITCF_SINT);
*/

template<typename FORMAT>
struct DataXTraits<ClockMode_et, FORMAT>
{
	static ClockMode_et FromString(const TCHAR* s)
	{
		if (shp_stricmp(s, _T("CM_WallTime")) ==0 )
			return CM_WallTime;
		else if (shp_stricmp(s, _T("CM_Countdown")) == 0)
			return CM_Countdown;
		else
		{
			// TODO? Thrown exception to tell invalid INI-value?
			return CM_WallTime;
		}
	}

	static Sdring ToString(ClockMode_et val)
	{
		if(val==CM_Countdown)
			return _T("CM_Countdown");
		else
			return _T("CM_WallTime");
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

