
// Lab1View.h : interface of the CLab1View class
//

#pragma once

#define White RGB(255,255,255) 
#define Pink RGB(254,153,205)
#define Orange RGB(255,153,51)
#define Green RGB(50,204,51)
#define Blue RGB(0,0,255)
#define Purple RGB(152,1,204)
#define Red RGB(255,0,0)
#define Yellow RGB(255,255,0)
#define Gray RGB(220,221,220)


class CLab1View : public CView
{
protected: // create from serialization only
	CLab1View() noexcept;
	DECLARE_DYNCREATE(CLab1View)

// Attributes
public:
	CLab1Doc* GetDocument() const;
private:
	bool gridOn;
// Operations
public:
	void DrawCanvas(CDC* pDC);
	void DrawGrid(CDC* pDC);
	void DrawShape(CDC* pDC, CPoint* vertices, int numOfVertices, CPen* pen, CBrush* brush);
	void DrawRegularPolygon(CDC* pDC, int cx, int cy, int r, int n, float rotAngle);
	double GetDistance(CPoint a, CPoint b);
	CPoint Incenter(CPoint a, CPoint b, CPoint c);
	void Rotate(CDC* pDC, float alpha, bool rightMultiply);
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
	virtual ~CLab1View();
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

#ifndef _DEBUG  // debug version in Lab1View.cpp
inline CLab1Doc* CLab1View::GetDocument() const
   { return reinterpret_cast<CLab1Doc*>(m_pDocument); }
#endif

