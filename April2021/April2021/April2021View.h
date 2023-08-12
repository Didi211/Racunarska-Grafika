
// April2021View.h : interface of the CApril2021View class
//

#pragma once
#define Black RGB(0,0,0)
#define White RGB(255,255,255)
#define Red RGB(255,0,0)
#define Green RGB(0,255,0)
#define Blue RGB(0,0,255)
#define Yellow RGB(255,255,0)
#define GridColor RGB(211,211,211)


class CApril2021View : public CView
{
protected: // create from serialization only
	CApril2021View() noexcept;
	DECLARE_DYNCREATE(CApril2021View)

// Attributes
private: 
	bool gridOn;
	int size = 500;
	int step = 25;
	CPoint mouse;
	int flowersNumber;
	CRect* rects;
	int* arcs;
	COLORREF* fillColors;
	COLORREF* lineColors;
	float* rotAngles;
	CString* strings;
	CPoint* flowerLocations;
public:
	CApril2021Doc* GetDocument() const;

// Operations
public:

// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

	void DrawPetal(CDC* pDC, CRect rect, int nArcs, COLORREF clrFill, COLORREF clrLine);
	void DrawFlower(CDC* pDC, CRect rect, int nArcs, COLORREF clrFill, COLORREF clrLine, float dRot, CString str);
	void DrawFlowers(CDC* pDC, CRect* aRect, int* anArcs, COLORREF* aClrFill, COLORREF* aClrLine, float* aDRot, CString* aStr, CPoint* aPt, int nFlower);
	void SaveDC(CDC* pDC, CRect rcDC, CRect rcBmp);
	void Rotate(CDC* pDC, float angle, bool rightMultiply = true);
	void Translate(CDC* pDC, float dX, float dY, bool rightMultiply);
	void DrawGrid(CDC* pDC);
	XFORM GetIdentityMatrix();
	void DrawString(CDC* pDC, CString str, double radius);
	void DrawMouseCoordinates(CDC* pDC);
	COLORREF GetRandomColor();
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// Implementation
public:
	virtual ~CApril2021View();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
public:
//	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
};

#ifndef _DEBUG  // debug version in April2021View.cpp
inline CApril2021Doc* CApril2021View::GetDocument() const
   { return reinterpret_cast<CApril2021Doc*>(m_pDocument); }
#endif

