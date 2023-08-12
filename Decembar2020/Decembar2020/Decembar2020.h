
// Decembar2020.h : main header file for the Decembar2020 application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CDecembar2020App:
// See Decembar2020.cpp for the implementation of this class
//

class CDecembar2020App : public CWinApp
{
public:
	CDecembar2020App() noexcept;


// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation
	UINT  m_nAppLook;
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CDecembar2020App theApp;
