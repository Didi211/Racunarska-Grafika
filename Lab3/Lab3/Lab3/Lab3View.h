
// Lab3View.h : interface of the CLab3View class
//
#include "DImage.h"
#pragma once
#define White RGB(255,255,255)
#define Black RGB(0,0,0)
#define GridColor RGB(211,211,211)


class CLab3View : public CView
{
protected: // create from serialization only
	CLab3View() noexcept;
	DECLARE_DYNCREATE(CLab3View)

// Attributes
private:
	bool gridOn;
	int size = 500;
	int step = 25;
	DImage** puzzles;
public:
	CLab3Doc* GetDocument() const;

// Operations
public:
	void DrawCanvas(CDC* pDC);
	void DrawGrid(CDC* pDC);
	void Translate(CDC* pDC, float dX, float dY, bool rightMultiply = true);
	void Scale(CDC* pDC, float sX, float sY, bool rightMultiply = true);
	void Rotate(CDC* pDC, float angle, bool rightMultiply = true);
	void Mirror(CDC* pDC, bool mx, bool my, bool rightMultiply);
	XFORM GetIdentityMatrix();
	void PlacePuzzle(CDC* pDC, DImage& puzzle, double x = 0, double y = 0, float rotAngle = 0, bool mirrorX = false, bool mirrorY = false, bool changeColor = false);
	void ApplyColorFilter(CDC* pDC, CBitmap* bitmap, bool changeColor);

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
	virtual ~CLab3View();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
};

#ifndef _DEBUG  // debug version in Lab3View.cpp
inline CLab3Doc* CLab3View::GetDocument() const
   { return reinterpret_cast<CLab3Doc*>(m_pDocument); }
#endif

