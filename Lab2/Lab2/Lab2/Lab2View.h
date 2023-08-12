
// Lab2View.h : interface of the CLab2View class
//

#pragma once
#define White RGB(255,255,255)
#define BlueBg RGB(134,207,235)
#define GridColor RGB(224,237,245)
#define Black RGB(0,0,0)
#define Orange RGB(223,149,0)
#define Green RGB(1,204,1)
#define Yellow RGB(255,255,0)

#define L 1.2
#define M 0.85
#define S 0.4


class CLab2View : public CView
{
protected: // create from serialization only
	CLab2View() noexcept;
	DECLARE_DYNCREATE(CLab2View)

// Attributes
public:
	CLab2Doc* GetDocument() const;
private:
	bool gridOn;
	int step = 25;
	int size = 500;
	float bigAngle;
	float smallAngle;

// Operations
private:
	int mod(int k, int n);
public:
	void DrawCanvas(CDC* pDC);
	void DrawGrid(CDC* pDC);
	void Translate(CDC* pDC, float dX, float dY, bool rightMultiply = true);
	void Scale(CDC* pDC, float sX, float sY, bool rightMultiply = true);
	void Rotate(CDC* pDC, float angle, bool rightMultiply = true);
	void DrawFigure(CDC* pDC);
	void DrawPot(CDC* pDC);
	void DrawCactus(CDC* pDC, float size, bool light, CPoint location);
	void DrawNode(CDC* pDC, int x, int y);
	void DrawSignature(CDC* pDC);
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
	virtual ~CLab2View();
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

#ifndef _DEBUG  // debug version in Lab2View.cpp
inline CLab2Doc* CLab2View::GetDocument() const
   { return reinterpret_cast<CLab2Doc*>(m_pDocument); }
#endif

