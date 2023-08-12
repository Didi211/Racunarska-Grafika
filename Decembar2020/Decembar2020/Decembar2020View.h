
// Decembar2020View.h : interface of the CDecembar2020View class
//

#pragma once
#define Black RGB(0,0,0)
#define White RGB(255,255,255)
#define LilacColor RGB(233, 191, 255)

#define PI 3.141592


class CDecembar2020View : public CView
{
protected: // create from serialization only
	CDecembar2020View() noexcept;
	DECLARE_DYNCREATE(CDecembar2020View)

// Attributes
public:
	CDecembar2020Doc* GetDocument() const;

// Operations
private:
	float GetLength(CPoint p1, CPoint p2);
public:
	void DrawCycle(CDC* pDC, int r, int n, int d);
	void DrawVirus(CDC* pDC, CPoint ptCenter, int rad, int m, int r, int n, int d, int alpha, int size, CString strText, COLORREF color);
	void SaveDC(CDC* pDC, CRect rcDC, CRect rcBmp);
	void Rotate(CDC* pDC, float alpha, bool rightMultiply);
	void Translate(CDC* pDC, float dX, float dY, bool rightMultiply);
	void WriteString(CDC* pDC, CString str, int size, float alpha, COLORREF color);
	XFORM GetIdentityMatrix();
// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// Implementation
public:
	virtual ~CDecembar2020View();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in Decembar2020View.cpp
inline CDecembar2020Doc* CDecembar2020View::GetDocument() const
   { return reinterpret_cast<CDecembar2020Doc*>(m_pDocument); }
#endif

