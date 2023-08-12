
// April2021View.cpp : implementation of the CApril2021View class
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "April2021.h"
#endif

#include "April2021Doc.h"
#include "April2021View.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#include <sstream>
#include "DImage.h"

using namespace std;

// CApril2021View

IMPLEMENT_DYNCREATE(CApril2021View, CView)

BEGIN_MESSAGE_MAP(CApril2021View, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
//	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

// CApril2021View construction/destruction

CApril2021View::CApril2021View() noexcept
{
	// TODO: add construction code here
	this->gridOn = true;
	this->mouse = CPoint(0, 0);

	srand(time(nullptr));
	//std::srand(static_cast<unsigned int>(std::time(nullptr)));
	
	int n = flowersNumber = 3;
	rects = new CRect[n];
	arcs = new int[n];
	fillColors = new COLORREF[n];
	lineColors = new COLORREF[n];
	rotAngles = new float[n];
	strings = new CString[n];
	flowerLocations = new CPoint[n];

	for (int i = 0; i < flowersNumber; i++)
	{
		rects[i].top = 0;
		rects[i].left = 0;
		rects[i].bottom = 35;
		rects[i].right = 35;
		arcs[i] = 2;
		fillColors[i] = GetRandomColor();
		lineColors[i] = GetRandomColor();
		rotAngles[i] = rand() % 90;
		strings[i] = "Cvetic";
		flowerLocations[i].x = rand() % 450 + 50;
		flowerLocations[i].y = rand() % 400 + 50;
		//flowerLocations[i].x = 250;
		//flowerLocations[i].y = 250;
	}

}

CApril2021View::~CApril2021View()
{
}

BOOL CApril2021View::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

void CApril2021View::DrawPetal(CDC* pDC, CRect rect, int nArcs, COLORREF clrFill, COLORREF clrLine)
{
	CPen* pen = new CPen(PS_SOLID, 1, clrLine);
	CBrush* brush = new CBrush(clrFill);

	CPen* oldPen = pDC->SelectObject(pen);
	CBrush* oldBrush = pDC->SelectObject(brush);


	//petal lowerPart
	CPoint petalTopLeft(-rect.Width()/2, -rect.Height());
	CPoint petalTopRight(petalTopLeft.x + rect.Width(), petalTopLeft.y);
	CPoint petalBottomMiddle(0,0);

	int prevArcDir = pDC->SetArcDirection(AD_CLOCKWISE);
	double radius = (double) rect.Width() / (nArcs * 2);
	
	pDC->BeginPath();
	//drawing lower part
	pDC->MoveTo(petalTopRight);
	pDC->LineTo(petalBottomMiddle);
	pDC->LineTo(petalTopLeft);

	double radiusStep = 2 * radius;
	//drawing upper part
	double xStart = petalTopLeft.x;
	double y = petalTopLeft.y;
	double xEnd = xStart + radiusStep;
	int i = 0;
	while (i < nArcs)
	{
		//petal arc points
		if (i != 0) 
		{
			xStart += radiusStep;
			xEnd += radiusStep;
		}
		CPoint arcBegin(xStart, y);
		CPoint arcEnd(xEnd, y);
		CPoint arcCenter((arcBegin.x + arcEnd.x) / 2.0, arcBegin.y);
		CPoint arcRectTopLeft(arcCenter.x - radius, arcCenter.y - radius);
		CPoint arcRectBottomRight(arcCenter.x + radius, arcCenter.y + radius);
		CRect arcRect(arcRectTopLeft, arcRectBottomRight);
		pDC->ArcTo(arcRect, arcBegin, arcEnd);

		//CPoint arcRectTopLeft(xStart, y - radius);
		//CPoint arcRectBottomRight(xEnd, y);
		//CRect arcRect(arcRectTopLeft, arcRectBottomRight);
		//pDC->Arc(arcRect, arcBegin, arcEnd);


		i++;
	}
	


	pDC->EndPath();

	pDC->StrokeAndFillPath();


	pDC->SelectObject(oldPen);
	pDC->SelectObject(oldBrush);
	pDC->SetArcDirection(prevArcDir);
	delete pen;
	delete brush;
}

void CApril2021View::DrawFlower(CDC* pDC, CRect rect, int nArcs, COLORREF clrFill, COLORREF clrLine, float dRot, CString str)
{
	XFORM oldForm;
	//pDC->GetWorldTransform(&oldForm);
	
	this->Rotate(pDC, dRot, false);
	for (int i = 0; i < 4; i++)
	{
		this->DrawPetal(pDC, rect, nArcs, clrFill, clrLine);
		this->Rotate(pDC, 90, false);
	}

	this->Rotate(pDC, -dRot, false);
	//pDC->SetWorldTransform(&oldForm);

	int radius = rect.Height() * 0.3;
	CBrush brush(Yellow);
	CBrush* oldBrush = pDC->SelectObject(&brush);
	CPoint point(radius, radius);
	pDC->Ellipse(CRect(-point, point));

	pDC->SelectObject(oldBrush);

	this->DrawString(pDC, str, 1.5 * rect.Height());
}

void CApril2021View::DrawFlowers(CDC* pDC, CRect* aRect, int* anArcs, COLORREF* aClrFill, COLORREF* aClrLine, float* aDRot, CString* aStr, CPoint* aPt, int nFlower)
{
	for (int i = 0; i < nFlower; i++)
	{
		CPoint oldViewPort = pDC->SetViewportOrg(aPt[i]);
		this->DrawFlower(pDC, aRect[i], anArcs[i], aClrFill[i], aClrLine[i], aDRot[i], aStr[i]);
		pDC->SetViewportOrg(oldViewPort);
	}
}

void CApril2021View::SaveDC(CDC* pDC, CRect rcDC, CRect rcBmp)
{
	CBitmap bmp;
	bmp.CreateCompatibleBitmap(pDC, rcBmp.Width(), rcBmp.Height());
	CDC* memDC = new CDC();
	memDC->CreateCompatibleDC(pDC);
	memDC->SelectObject(&bmp);
	memDC->SetGraphicsMode(GM_ADVANCED);
	memDC->BitBlt(rcBmp.left, rcBmp.top, rcBmp.Width(), rcBmp.Height(), pDC, rcDC.left, rcDC.top, SRCCOPY);
	DImage* img = new DImage(bmp);
	img->Save(CString("E:\\Faks\\IV godina\\RG\\VS zadaci\\GDI\\April2021\\flowers.bmp"));
	
	memDC->DeleteDC();
	delete memDC;
}

// CApril2021View drawing

void CApril2021View::OnDraw(CDC* pDC)
{
	CApril2021Doc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;
	// TODO: add draw code for native data here
	int prevMode = pDC->GetGraphicsMode();
	pDC->SetGraphicsMode(GM_ADVANCED);

	if (this->gridOn)
		this->DrawGrid(pDC);

	//this->DrawFlower(pDC, rect, 2, Blue, Red, 0, CString("Cvetic"));

	this->DrawFlowers(pDC, rects, arcs, fillColors, lineColors, rotAngles, strings, flowerLocations, flowersNumber);
	DrawMouseCoordinates(pDC);
	SaveDC(pDC, CRect(0, 0, 1000, 500), CRect(0, 0, 1000, 500));
	pDC->SetGraphicsMode(prevMode);

}


// CApril2021View printing

void CApril2021View::Rotate(CDC* pDC, float angle, bool rightMultiply)
{
	float pi = 3.141592;
	XFORM xform = this->GetIdentityMatrix();
	double alpha = angle * pi / 180.0;

	xform.eM11 = cos(alpha);
	xform.eM12 = sin(alpha);
	xform.eM21 = -sin(alpha);
	xform.eM22 = cos(alpha);
	pDC->ModifyWorldTransform(&xform, rightMultiply ? MWT_RIGHTMULTIPLY : MWT_LEFTMULTIPLY);
}

void CApril2021View::Translate(CDC* pDC, float dX, float dY, bool rightMultiply)
{
	XFORM xform = this->GetIdentityMatrix();
	xform.eDx = dX;
	xform.eDy = dY;
	pDC->ModifyWorldTransform(&xform, (rightMultiply) ? MWT_RIGHTMULTIPLY : MWT_LEFTMULTIPLY);
}

void CApril2021View::DrawGrid(CDC* pDC)
{
	pDC->MoveTo(0, 0);
	CPen* pen = new CPen(PS_SOLID, 1, GridColor);
	CPen* oldPen = pDC->SelectObject(pen);
	for (int i = 0; i < 21; i++)
	{
		pDC->MoveTo(0, i * step);
		pDC->LineTo(size, i * 25);
		pDC->MoveTo(i * step, 0);
		pDC->LineTo(i * step, size);
	}
	pDC->SelectObject(oldPen);
	delete pen;
}

XFORM CApril2021View::GetIdentityMatrix()
{
	XFORM xform;
	xform.eM11 = 1.0;
	xform.eM12 = 0.0;
	xform.eM21 = 0.0;
	xform.eM22 = 1.0;
	xform.eDx = 0;
	xform.eDy = 0;
	return xform;
}

void CApril2021View::DrawString(CDC* pDC, CString str, double radius)
{
	int totalAngle = 90;
	double angleStep = double(totalAngle) / str.GetLength();

	XFORM oldForm;
	pDC->GetWorldTransform(&oldForm);
	int oldBkMode = pDC->GetBkMode();
	pDC->SetBkMode(TRANSPARENT);
	this->Rotate(pDC, 45, false);

	for (int i = 0; i < str.GetLength(); i++)
	{
		pDC->TextOutW(0, radius, (CString)str[i]);
		this->Rotate(pDC, -angleStep, false);
	}

	pDC->SetBkMode(oldBkMode);
	pDC->SetWorldTransform(&oldForm);


}

void CApril2021View::DrawMouseCoordinates(CDC* pDC)
{
	CFont font;
	int oldBgMode = pDC->GetBkMode();
	pDC->SetBkMode(TRANSPARENT);

	font.CreateFont(30, 0, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 0, 0, CString("Arial"));
	CFont* oldFont = pDC->SelectObject(&font);
	COLORREF oldColor = pDC->SetTextColor(Black);
	stringstream ss;
	ss << this->mouse.x << "," << this->mouse.y;
	pDC->TextOutW(600,100, CString(ss.str().c_str()));

	pDC->SetTextColor(oldColor);
	pDC->SetBkMode(oldBgMode);
	pDC->SelectObject(oldFont);
	font.DeleteObject();
}

COLORREF CApril2021View::GetRandomColor()
{
	int red = rand() % 256;
	int green = rand() % 256;
	int blue = rand() % 256;
	return RGB(red, green, blue);
}

BOOL CApril2021View::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CApril2021View::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CApril2021View::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}


// CApril2021View diagnostics

#ifdef _DEBUG
void CApril2021View::AssertValid() const
{
	CView::AssertValid();
}

void CApril2021View::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CApril2021Doc* CApril2021View::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CApril2021Doc)));
	return (CApril2021Doc*)m_pDocument;
}
#endif //_DEBUG


// CApril2021View message handlers


//void CApril2021View::OnMouseMove(UINT nFlags, CPoint point)
//{
	// TODO: Add your message handler code here and/or call default

//	this->mouse = point;
//	CView::OnMouseMove(nFlags, point);
//	Invalidate();
//
//}
