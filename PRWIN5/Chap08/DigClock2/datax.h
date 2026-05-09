
// PENDINGQ: This bespoke template class is ugly.
// How to split/separate DataXString_AutoSaveFile's code logic 
// from actual g_xini's save-file behavior?

template<typename TU, typename FORMAT=XStringFormatDefault>
class DataXString_AutoSaveFile : public DataXString<TU, FORMAT>
{
public:
	DataXString_AutoSaveFile(const TCHAR* default) : DataXString(default) {}

	DataXString_AutoSaveFile& operator= (const TU& val)
	{
		DataXString::SetValue(val);
		return *this;
	}

	virtual SetValue_ret SetValue(TU&& val) cxx11_override
	{
		SetValue_ret svret = (*this).DataXString::SetValue(std::move(val));

		if(svret==SetNew)
		{
			g_xini.SaveIni();
		}

		return svret;
	}

};


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

