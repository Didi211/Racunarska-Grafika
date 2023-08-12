
// April2022View.h : interface of the CApril2022View class
//

#pragma once
#define White RGB(255,255,255)
#define Black RGB(0,0,0)
#define Orange RGB(252,128,3)
#define OrangeLight RGB(253,179,104)

#define PI 3.14159


class CApril2022View : public CView
{
protected: // create from serialization only
	CApril2022View() noexcept;
	DECLARE_DYNCREATE(CApril2022View)

// Attributes
public:
	CApril2022Doc* GetDocument() const;

// Operations
private: 
	int EnlightColor(int color, double amount);
public:
	void Rotate(CDC* pDC, float angle, bool rightMultiply);
	void Translate(CDC* pDC, float dX, float dY, bool rightMultiply);
	XFORM GetIdentityMatrix();
	void DrawCabin(CDC* pDC, int w, int d, COLORREF clr, CString str);
	void DrawWheel(CDC* pDC, int r, int d, int n, COLORREF clr);
	void DrawObservationWheel(CDC* pDC, int r, int d, int n, int k, COLORREF clr, CString* arStr, CPoint ptCenter, float alpha);
	COLORREF GetLightColor(COLORREF color, double lightAmount);
	void WriteString(CDC* pDC, CString str, int size);
	void SaveDC(CDC* pDC, CRect rcDC, CRect rcBmp);

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
	virtual ~CApril2022View();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in April2022View.cpp
inline CApril2022Doc* CApril2022View::GetDocument() const
   { return reinterpret_cast<CApril2022Doc*>(m_pDocument); }
#endif

