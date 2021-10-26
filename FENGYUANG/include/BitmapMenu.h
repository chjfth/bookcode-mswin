#pragma once

//-----------------------------------------------------------------------------------//
//              Windows Graphics Programming: Win32 GDI and DirectDraw               //
//                             ISBN  0-13-086985-6                                   //
//                                                                                   //
//  Written            by  Yuan, Feng                             www.fengyuan.com   //
//  Copyright (c) 2000 by  Hewlett-Packard Company                www.hp.com         //
//  Published          by  Prentice Hall PTR, Prentice-Hall, Inc. www.phptr.com      //
//                                                                                   //
//  FileName   : bitmapmenu.h					                                     //
//  Description: Use bitmap on the menu                                              //
//  Version    : 1.00.000, May 31, 2000                                              //
//-----------------------------------------------------------------------------------//

class KBitmapMenu
{
	enum { MAX_BITMAP = 16 };

	HBITMAP m_hBitmap[MAX_BITMAP];
	int     m_nBitmap;
	HMENU   m_hMenu;
	int		m_nChecked;
	int		m_nFirstCommand;

public:
	KBitmapMenu()
	{
		m_nBitmap = 0;
	}

	~KBitmapMenu()
	{
		for (int i=0; i<m_nBitmap; i++)
			DeleteObject(m_hBitmap[i]);
	}

	void AddToMenu(HMENU hMenu, int nCount, 
		HMODULE hModule, const int arBitmapResID[], 
		int nFirstCmdidOfMenuitem);
	// -- Add some new menuitems to hMenu.
	// Each of the menuitems is displayed as a bitmap from hModule's resource.
	// nFirstCmdidOfMenuitem+0, nFirstCmdidOfMenuitem+1, nFirstCmdidOfMenuitem+2 ... 
	// are these menuitem's cmdid.
	// nFirstCmdidOfMenuitem+0 uses arBitmapResID[0] as display image,
	// nFirstCmdidOfMenuitem+1 uses arBitmapResID[1] as display image...

	int OnCommand(int nCmdId);

	HBITMAP GetChecked(int & nChecked)
	{
		nChecked = m_nChecked;
		return m_hBitmap[nChecked];
	}
};
