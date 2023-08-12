
// Decembar2020View.cpp : implementation of the CDecembar2020View class
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "Decembar2020.h"
#endif

#include "Decembar2020Doc.h"
#include "Decembar2020View.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include <iostream>
#include "DImage.h"
using namespace std;

// CDecembar2020View

IMPLEMENT_DYNCREATE(CDecembar2020View, CView)

BEGIN_MESSAGE_MAP(CDecembar2020View, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
END_MESSAGE_MAP()

// CDecembar2020View construction/destruction

CDecembar2020View::CDecembar2020View() noexcept
{
	// TODO: add construction code here

}

CDecembar2020View::~CDecembar2020View()
{
}

BOOL CDecembar2020View::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CDecembar2020View drawing

float CDecembar2020View::GetLength(CPoint p1, CPoint p2)
{
	float xSquared = pow(p1.x - p2.x, 2);
	float ySquared = pow(p1.y - p2.y, 2);
	return sqrt(xSquared + ySquared);
}

void CDecembar2020View::DrawCycle(CDC* pDC, int r, int n, int d)
{
	if (n < 4)
		throw "Polygon can't have less than 4 sides.";
	if (n % 2 == 1)
		throw "Polygon must have even number of sides.";

	XFORM oldForm;
	pDC->GetWorldTransform(&oldForm);

	//drawing polygon with lines
	int angle = 0;
	int step = 360.0 / n;
	CPoint* points = new CPoint[n];
	CPoint* lineEndPoints = new CPoint[n];
	for (int i = 0; i < n; i++)
	{
		float x = r * cos(angle * PI / 180.0);
		float xLine = 3 * r * cos(angle * PI / 180.0);
		float y = r * sin(angle * PI / 180.0);
		float yLine = 3 * r * sin(angle * PI / 180.0);

		points[i] = CPoint(x, y);
		lineEndPoints[i] = CPoint(xLine, yLine);

		pDC->MoveTo(points[i]);
		pDC->LineTo(lineEndPoints[i]);

		angle += step;
		Rotate(pDC, step*10, false);
	}

	pDC->Polygon(points, n);

	pDC->SetWorldTransform(&oldForm);

	//drawing rectangle
	float polygonSideLength = this->GetLength(points[0], points[1]);
	float triangleHeight = sqrt(r * r - (polygonSideLength / 2));

	CPoint topLeft(-polygonSideLength / 2.0, triangleHeight);
	CPoint bottomRigth(+polygonSideLength / 2.0, triangleHeight + 8*r);
	CRect rect(topLeft, bottomRigth);
	pDC->Rectangle(rect);




}

void CDecembar2020View::DrawVirus(CDC* pDC, CPoint ptCenter, int rad, int m, int r, int n, int d, int alpha, int size, CString strText, COLORREF color)
{
	// draw virus
	CPen pen(PS_SOLID, d, Black);
	CPen* oldPen = pDC->SelectObject(&pen);
	CBrush* brush = new CBrush(color);
	CBrush* oldBrush = pDC->SelectObject(brush);

	CPoint oldOrg = pDC->SetViewportOrg(ptCenter);
	
	pDC->Ellipse(CRect(-rad, -rad, rad, rad));
	brush->DeleteObject();
	brush = new CBrush(White);
	pDC->SelectObject(brush);
	pDC->Ellipse(CRect(-rad*0.8, -rad*0.8, rad*0.8, rad*0.8));

	pDC->SelectObject(oldBrush);
	delete brush;

	XFORM oldForm;
	pDC->GetWorldTransform(&oldForm);

	// draw legs 
	float angleStep = 360.0 / m;
	int legAngle = -90;
	float legCenter = rad + 9*r;

	for (int i = 0; i < m; i++)
	{
		Translate(pDC, 0, -legCenter, false); //moving up from the logical center
		DrawCycle(pDC, r, n, d);
		Translate(pDC, 0, legCenter, false); //getting back to the center

  		Rotate(pDC, angleStep, false);
	}

	pDC->SetWorldTransform(&oldForm);

	// draw string 
	WriteString(pDC, strText, size, alpha, color);

	pDC->SetViewportOrg(oldOrg);
	pDC->SelectObject(oldPen);
}

void CDecembar2020View::SaveDC(CDC* pDC, CRect rcDC, CRect rcBmp)
{
	CBitmap bmp;
	bmp.CreateCompatibleBitmap(pDC, rcBmp.Width(), rcBmp.Height());

	CDC* memDC = new CDC();
	memDC->CreateCompatibleDC(pDC);
	memDC->SelectObject(&bmp);

	memDC->SetGraphicsMode(GM_ADVANCED);
	memDC->BitBlt(rcBmp.left, rcBmp.top, rcBmp.Width(), rcBmp.Height(), pDC, rcDC.left, rcDC.top, SRCCOPY);


	DImage* img = new DImage(bmp);
	img->Save(CString("E:\\Faks\\IV godina\\RG\\VS zadaci\\GDI\\Decembar2020\\slika.bmp"));
}

void CDecembar2020View::Rotate(CDC* pDC, float alpha, bool rightMultiply)
{
	XFORM xform = GetIdentityMatrix();
	double alphaRad = alpha * PI / 180.0; // convert to radians
	xform.eM11 = cos(alphaRad);
	xform.eM12 = sin(alphaRad);
	xform.eM21 = -sin(alphaRad);
	xform.eM22 = cos(alphaRad);
	pDC->ModifyWorldTransform(&xform, rightMultiply ? MWT_RIGHTMULTIPLY : MWT_LEFTMULTIPLY);

}

void CDecembar2020View::Translate(CDC* pDC, float dX, float dY, bool rightMultiply)
{
	XFORM xform = GetIdentityMatrix();
	xform.eDx = dX;
	xform.eDy = dY;
	pDC->ModifyWorldTransform(&xform, rightMultiply ? MWT_RIGHTMULTIPLY : MWT_LEFTMULTIPLY);
}

void CDecembar2020View::WriteString(CDC* pDC, CString str, int size, float alpha, COLORREF color)
{
	CFont font;
	font.CreateFontW(size, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CString("Arial"));
	int oldBkMode = pDC->SetBkMode(TRANSPARENT);
	COLORREF oldTextColor = pDC->SetTextColor(color);
	int oldTextAlign = pDC->SetTextAlign(TA_CENTER);
	CFont* oldFont = pDC->SelectObject(&font);

	Rotate(pDC, alpha, false);
	pDC->TextOutW(0, -size/2.0, str);
	Rotate(pDC, -alpha, false);

	pDC->SetBkMode(oldBkMode);
	pDC->SetTextColor(oldTextColor);
	pDC->SetTextAlign(oldTextAlign);
	pDC->SelectObject(oldFont);
}

XFORM CDecembar2020View::GetIdentityMatrix()
{
	XFORM xform;
	xform.eDx = 0;
	xform.eDy = 0;
	xform.eM11 = 1;
	xform.eM12 = 0;
	xform.eM21 = 0;
	xform.eM22 = 1;
	return xform;
}

void CDecembar2020View::OnDraw(CDC* pDC)
{
	CDecembar2020Doc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: add draw code for native data here

	int prevMode = pDC->SetGraphicsMode(GM_ADVANCED);

	int textSize = 15;
	int alpha = -30;
	int virusRadius = 80;
	int numberOfLegs = 6;
	int legRadius = virusRadius * 0.2;
	int n = 6;
	int lineWidth = 3;
	try 
	{
		DrawVirus(pDC, CPoint(400, 250), virusRadius,numberOfLegs, legRadius, n, lineWidth, alpha, textSize, CString("COVID 19"), LilacColor);
		SaveDC(pDC, CRect(0, 0, 800, 600), CRect(0, 0, 800, 600));

	}
	catch (char* ex)
	{
		cout << ex;
	}

	pDC->SetGraphicsMode(prevMode);
}


// CDecembar2020View printing

BOOL CDecembar2020View::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CDecembar2020View::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CDecembar2020View::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}


// CDecembar2020View diagnostics

#ifdef _DEBUG
void CDecembar2020View::AssertValid() const
{
	CView::AssertValid();
}

void CDecembar2020View::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CDecembar2020Doc* CDecembar2020View::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CDecembar2020Doc)));
	return (CDecembar2020Doc*)m_pDocument;
}
#endif //_DEBUG


// CDecembar2020View message handlers
