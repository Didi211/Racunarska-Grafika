
// Lab1View.cpp : implementation of the CLab1View class
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "Lab1.h"
#endif

#include "Lab1Doc.h"
#include "Lab1View.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CLab1View

IMPLEMENT_DYNCREATE(CLab1View, CView)

BEGIN_MESSAGE_MAP(CLab1View, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()

// CLab1View construction/destruction

CLab1View::CLab1View() noexcept
{
	// TODO: add construction code here
	this->gridOn = true;
}

CLab1View::~CLab1View()
{
}

BOOL CLab1View::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CLab1View drawing

void CLab1View::DrawCanvas(CDC* pDC)
{
	CPen* pen = new CPen(PS_NULL, 5, White); //canvas without border
	CPen* oldPen = pDC->SelectObject(pen);

	CBrush* brush = new CBrush(Gray);
	CBrush* oldBrush = pDC->SelectObject(brush);

	pDC->Rectangle(0, 0, 500, 500); //drawing canvas

	pDC->SelectObject(oldPen);
	pDC->SelectObject(oldBrush);
	delete pen;
	delete brush;
}

void CLab1View::DrawGrid(CDC* pDC)
{
	pDC->MoveTo(0, 0);
	CPen* pen = new CPen(PS_SOLID, 1, White);
	CPen* oldPen = pDC->SelectObject(pen);

	for (int i = 0; i < 21; i++) 
	{
		pDC->MoveTo(0, i * 25);
		pDC->LineTo(500, i * 25);
		pDC->MoveTo(i * 25, 0);
		pDC->LineTo(i * 25, 500);
	}
}

void CLab1View::DrawShape(CDC* pDC, CPoint* vertices, int numOfVertices, CPen* pen, CBrush* brush)
{
	CPen* oldPen = pDC->SelectObject(pen);
	CBrush* oldBrush = pDC->SelectObject(brush);

	pDC->Polygon(vertices, numOfVertices);

}

void CLab1View::DrawRegularPolygon(CDC* pDC, int cx, int cy, int r, int n, float rotAngle)
{
	CPen* pen = new CPen(PS_SOLID, 3, Red);
	CPen* oldPen = pDC->SelectObject(pen);

	//calculating the angle
	const double pi = 3.1415926535;
	double offset = pi * 2 / n; //offset for angles

	double x;
	double rotAngleOffset = 0;
	if (rotAngle != 0)
	{
		x = 360 / rotAngle;
		rotAngleOffset = 2 * pi / x;
	}


	int verticeIndex = 0;
	CPoint* points = new CPoint[n];
	double i = 0;
	while (i < 2 * pi)
	{
		double verticeX = cx + r * cos(i + rotAngleOffset);
		double verticeY = cy + r * sin(i + rotAngleOffset);
		points[verticeIndex] = CPoint(verticeX, verticeY);
		verticeIndex++;
		i += offset;
	}

	pDC->Polygon(points, n);
	pDC->SelectObject(oldPen);
	delete pen;
	delete points;

	
}

double CLab1View::GetDistance(CPoint a, CPoint b)
{
	return sqrt(pow((double)a.x - b.x, 2) + pow((double)a.y - b.y, 2));

}

CPoint CLab1View::Incenter(CPoint a, CPoint b, CPoint c)
{
	//length of each side of a triangle
	double sideA = this->GetDistance(b, c);
	double sideB = this->GetDistance(a, c);
	double sideC = this->GetDistance(a, b);

	int circumference = sideA + sideB + sideC;
	double x, y;
	x = (sideA * a.x + sideB * b.x + sideC * c.x) / circumference;
	y = (sideA * a.y + sideB * b.y + sideC * c.y) / circumference;

	return CPoint(x, y);
}

void CLab1View::Rotate(CDC* pDC, float alpha, bool rightMultiply)
{
	XFORM form;
	XFORM oldForm;
	pDC->GetWorldTransform(&oldForm);
	form.eDx = 0;
	form.eDy = 0;
	form.eM11 = cos(alpha);
	form.eM12 = sin(alpha);
	form.eM21= -sin(alpha);
	form.eM22 = cos(alpha);
	if (rightMultiply)
		pDC->ModifyWorldTransform(&form, MWT_RIGHTMULTIPLY);
	else
		pDC->ModifyWorldTransform(&form, MWT_LEFTMULTIPLY);
}

void CLab1View::OnDraw(CDC* pDC)
{
	CLab1Doc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: add draw code for native data here
	int step = 25;
	
	this->DrawCanvas(pDC);

	CPen* borderPen = new CPen(PS_SOLID, 5, Red);
	
	//pink triangle
 	CBrush* brush = new CBrush(Pink);
	CPoint* points = new CPoint[3];
	points[0] = CPoint(7 * step, 1 * step);
	points[1] = CPoint(19 * step, 1 * step);
	points[2] = CPoint(7 * step, 13 * step); 
	this->DrawShape(pDC, points, 3, borderPen, brush);
	CPoint center = this->Incenter(points[0], points[1], points[2]);
	this->DrawRegularPolygon(pDC, center.x, center.y, 40, 4, 0);

	//red triangle
	delete brush;
	brush = new CBrush(Red);
	points[0] = CPoint(19 * step, 13 * step);
	points[1] = CPoint(19 * step, 1 * step);
	points[2] = CPoint(7 * step, 13 * step);
	this->DrawShape(pDC, points, 3, borderPen, brush);

	//hatched square
	delete brush;
	brush = new CBrush(HS_DIAGCROSS, Blue);
	delete points;
	points = new CPoint[4];
 	points[0] = CPoint(1 * step, 7 * step);
	points[1] = CPoint(7 * step, 7 * step);
	points[2] = CPoint(7 * step, 13 * step);
	points[3] = CPoint(1 * step, 13 * step);
	this->DrawShape(pDC, points, 4, borderPen, brush);

	//yellow triangle
	delete brush;
	brush = new CBrush(Yellow);
	delete points;
	points = new CPoint[3];
	points[0] = CPoint(1 * step, 13 * step);
	points[1] = CPoint(7* step, 13 * step);
	points[2] = CPoint(1 * step, 19 * step);
	this->DrawShape(pDC, points, 3, borderPen, brush);
	center = this->Incenter(points[0], points[1], points[2]);
	this->DrawRegularPolygon(pDC, center.x, center.y, 25, 7, 0);

	//purple rhomboid
	delete brush;
	brush = new CBrush(Purple);
	delete points;
	points = new CPoint[4];
	points[0] = CPoint(7* step, 13 * step);
	points[1] = CPoint(13 * step, 13 * step);
	points[2] = CPoint(7 * step, 19 * step);
	points[3] = CPoint(1 * step, 19 * step);
	this->DrawShape(pDC, points, 4, borderPen, brush);

	//green triangle
	delete brush;
	brush = new CBrush(Green);
	delete points;
	points = new CPoint[3];
	points[0] = CPoint(13 * step, 13 * step);
	points[1] = CPoint(19 * step, 19 * step);
	points[2] = CPoint(7 * step, 19 * step);
	this->DrawShape(pDC, points, 3, borderPen, brush);
	center = this->Incenter(points[0], points[1], points[2]);
	this->DrawRegularPolygon(pDC, center.x, center.y, 30, 6, 0);

	//orangle triangle
	delete brush;
	brush = new CBrush(Orange);
	delete points;
	points = new CPoint[3];
	points[0] = CPoint(13 * step, 13 * step);
	points[1] = CPoint(19 * step, 13 * step);
	points[2] = CPoint(19 * step, 19 * step);
	this->DrawShape(pDC, points, 3, borderPen, brush);
	center = this->Incenter(points[0], points[1], points[2]);
	this->DrawRegularPolygon(pDC, center.x, center.y, 25, 8, 0);

	if (this->gridOn)
		this->DrawGrid(pDC);
}


// CLab1View printing

BOOL CLab1View::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CLab1View::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CLab1View::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}


// CLab1View diagnostics

#ifdef _DEBUG
void CLab1View::AssertValid() const
{
	CView::AssertValid();
}

void CLab1View::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CLab1Doc* CLab1View::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CLab1Doc)));
	return (CLab1Doc*)m_pDocument;
}
#endif //_DEBUG


// CLab1View message handlers


void CLab1View::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default
	if (nChar == ' ')
	{
		this->gridOn = !this->gridOn;
		Invalidate();
	}

	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}
