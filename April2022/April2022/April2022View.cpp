
// April2022View.cpp : implementation of the CApril2022View class
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "April2022.h"
#endif

#include "April2022Doc.h"
#include "April2022View.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include <string>
#include "DImage.h"

using namespace std;

// CApril2022View

IMPLEMENT_DYNCREATE(CApril2022View, CView)

BEGIN_MESSAGE_MAP(CApril2022View, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
END_MESSAGE_MAP()

// CApril2022View construction/destruction

CApril2022View::CApril2022View() noexcept
{
	// TODO: add construction code here

}

CApril2022View::~CApril2022View()
{
}

BOOL CApril2022View::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CApril2022View drawing

void CApril2022View::OnDraw(CDC* pDC)
{
	CApril2022Doc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: add draw code for native data here
	int prevMode = pDC->GetGraphicsMode();
	pDC->SetGraphicsMode(GM_ADVANCED);
	CPoint oldViewPort = pDC->SetViewportOrg(400, 200);

	int r = 150;
	int d = 2;
	int n = 16; 
	int k = 4;
	COLORREF clr = RGB(86,132,32);
	CPoint ptCenter(0, 0);
	float alpha = 45;
	
	CString* cabinNumbers = new CString[k];
	for (int i = 0; i < k; i++)
	{
		CString cabin(std::to_string(i+1).c_str());
		cabinNumbers[i] = cabin;
	}
	this->DrawObservationWheel(pDC, r, d, n, k, clr, cabinNumbers, ptCenter, alpha);


	pDC->SetViewportOrg(oldViewPort);
	SaveDC(pDC, CRect(0, 0, 800, 600), CRect(0, 0, 800, 600));
	pDC->SetGraphicsMode(prevMode);



}


// CApril2022View printing

void CApril2022View::DrawWheel(CDC* pDC, int r, int d, int n, COLORREF clr)
{
	CPen circlePen(PS_SOLID, 3*d, clr);
	CPen wirePen(PS_SOLID, d, clr);
	CPen* oldPen = pDC->SelectObject(&circlePen);

	int smallerRadius = 0.8 * r;
	float wireAngleStep = 360.0 / n;
	
	CRect elipseRect(-r, -r, r, r);
	CRect elipseRectSmaller(-smallerRadius, -smallerRadius, smallerRadius, smallerRadius);
	pDC->Ellipse(elipseRect);
	pDC->Ellipse(elipseRectSmaller);

	pDC->SelectObject(&wirePen);

	XFORM oldForm;
	pDC->GetWorldTransform(&oldForm);
	for (int i = 0; i < n; i++)
	{
		pDC->MoveTo(0, 0);
		CPoint end(0, r);
		pDC->LineTo(end);
		this->Rotate(pDC, wireAngleStep, false);
	}
	pDC->SetWorldTransform(&oldForm);
	
	CBrush brush(clr);
	CBrush* oldBrush = pDC->SelectObject(&brush);
	int axisRadius = 0.1 * r;
	CRect axisRect(-axisRadius, -axisRadius, axisRadius, axisRadius);
	pDC->Ellipse(axisRect);

	pDC->SelectObject(&oldPen);
	pDC->SelectObject(&oldBrush);

}

void CApril2022View::DrawObservationWheel(CDC* pDC, int r, int d, int n, int k, COLORREF clr, CString* arStr, CPoint ptCenter, float alpha)
{
	XFORM oldForm; //old form is for cabins to hang normal
	XFORM rotateForm;

	// draw wheel 
	pDC->GetWorldTransform(&oldForm); 
	this->Rotate(pDC, alpha, false);
	this->DrawWheel(pDC, r, d, n, clr);
	this->Rotate(pDC, -alpha, false);


	// draw cabins 
	int cabinAngleStep = 360 / k;
	int cabinAngle = alpha;

	for (int i = 0; i < k; i++)
	{
		float x = r * cos(cabinAngle * PI / 180.0);
		float y = r * sin(cabinAngle * PI / 180.0);

		//NOTE: rotation is not needed
		//pDC->GetWorldTransform(&rotateForm); 
		//pDC->SetWorldTransform(&oldForm); 

		this->Translate(pDC, x, y, false);
		this->DrawCabin(pDC, 0.2 * r, d, GetLightColor(clr, 0.4), arStr[i]);
		this->Translate(pDC, -x, -y, false); 

		//pDC->SetWorldTransform(&rotateForm);
		//this->Rotate(pDC, cabinAngleStep, false);

		cabinAngle += cabinAngleStep;
	}

	// draw infrastructure
	CPoint b(0.4 * r, 1.6 * r);
	CPoint c(-0.4 * r, 1.6 * r);
	CPoint points[4] = { ptCenter, b, c, ptCenter };
	CPen pen(PS_SOLID, d, clr);
	CPen* oldPen = pDC->SelectObject(&pen);


	pDC->MoveTo(ptCenter);
	pDC->Polyline(points, 4);

	pDC->SelectObject(oldPen);

}

COLORREF CApril2022View::GetLightColor(COLORREF color, double lightAmount)
{
	// Ensure amount is in the range [-1, 1]
	double amount = max(-1.0, min(1.0, lightAmount));
	int red = GetRValue(color);
	int green = GetGValue(color);
	int blue = GetBValue(color);

	red = this->EnlightColor(red, amount);
	green = this->EnlightColor(green, amount);
	blue = this->EnlightColor(blue, amount);

	return RGB(red, green, blue);
}

void CApril2022View::WriteString(CDC* pDC, CString str, int size)
{
	CFont font;
	font.CreateFont(size, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, CString("Arial"));
	CFont* oldFont = pDC->SelectObject(&font);

	int oldBkMode = pDC->GetBkMode();
	COLORREF prevTextColor = pDC->SetTextColor(White);
	pDC->SetBkMode(TRANSPARENT);
	int oldTextAlign = pDC->SetTextAlign(TA_CENTER);
	pDC->TextOutW(0, -size/2.0, str);

	pDC->SetTextColor(prevTextColor);
	pDC->SetBkMode(oldBkMode);
	pDC->SelectObject(oldFont);
	pDC->SetTextAlign(oldTextAlign);
}

void CApril2022View::SaveDC(CDC* pDC, CRect rcDC, CRect rcBmp)
{
	CBitmap bmp;
	bmp.CreateCompatibleBitmap(pDC, rcBmp.Width(), rcBmp.Height());

	CDC* memDC = new CDC();
	memDC->CreateCompatibleDC(pDC);
	memDC->SelectObject(&bmp);

	memDC->SetGraphicsMode(GM_ADVANCED);
	memDC->BitBlt(rcBmp.left, rcBmp.top, rcBmp.Width(), rcBmp.Height(), pDC, rcDC.left, rcDC.top, SRCCOPY);


	DImage* img = new DImage(bmp);
	img->Save(CString("E:\\Faks\\IV godina\\RG\\VS zadaci\\GDI\\April2022\\slika.bmp"));
}

BOOL CApril2022View::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CApril2022View::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CApril2022View::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}


// CApril2022View diagnostics

#ifdef _DEBUG
void CApril2022View::AssertValid() const
{
	CView::AssertValid();
}

void CApril2022View::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CApril2022Doc* CApril2022View::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CApril2022Doc)));
	return (CApril2022Doc*)m_pDocument;
}
#endif //_DEBUG


// CApril2022View message handlers

int CApril2022View::EnlightColor(int color, double amount)
{
	return (int)(color + (255 - color) * amount);
}

void CApril2022View::Rotate(CDC* pDC, float angle, bool rightMultiply)
{
	float pi = 3.141592;
	XFORM xform = GetIdentityMatrix();
	double alpha = angle * pi / 180.0;
	xform.eM11 = cos(alpha);
	xform.eM12 = sin(alpha);
	xform.eM21 = -sin(alpha);
	xform.eM22 = cos(alpha);
	pDC->ModifyWorldTransform(&xform, rightMultiply ? MWT_RIGHTMULTIPLY : MWT_LEFTMULTIPLY);
}

void CApril2022View::Translate(CDC* pDC, float dX, float dY, bool rightMultiply)
{
	XFORM xform = GetIdentityMatrix();
	xform.eDx = dX;
	xform.eDy = dY;
	pDC->ModifyWorldTransform(&xform, rightMultiply ? MWT_RIGHTMULTIPLY : MWT_LEFTMULTIPLY);
}

XFORM CApril2022View::GetIdentityMatrix()
{
	XFORM xform;
	xform.eDx = 0;
	xform.eDy = 0;
	xform.eM11 = 1;
	xform.eM22 = 1;
	xform.eM12 = 0;
	xform.eM21 = 0;
	return xform;
}

void CApril2022View::DrawCabin(CDC* pDC, int w, int d, COLORREF clr, CString str)
{
	float radius = w / 2.0;
	float cableLenght = 0.25 * w;
	CPen pen(PS_SOLID, 3, clr);
	CPen* oldPen = pDC->SelectObject(&pen);
	CBrush brush(clr);
	CBrush* oldBrush = pDC->SelectObject(&brush);

	//cabin lower part
	CPoint lowerCabinStart(-radius, -radius);
	CPoint lowerCabinEnd(radius, radius);

	//cabin upper part
	CPoint arcStart(lowerCabinStart);
	CPoint arcEnd(lowerCabinStart);
	arcEnd.x += w;
	CPoint arcCenter(0, -radius);

	CRect arcRect(
		CPoint(arcCenter.x - radius, arcCenter.y - radius),
		CPoint(arcCenter.x + radius, arcCenter.y + radius)
	);
	int prevAD = pDC->SetArcDirection(AD_CLOCKWISE);

	//cabin cable
	CPoint cableStart(0, -w);
	CPoint cableEnd(cableStart);
	cableEnd.y -= cableLenght;

	pDC->BeginPath();
	this->Translate(pDC, 0, (w + cableLenght),false); //lowering the coordinate system for the cabin size 
													  // since drawing begins from the center of lower part of cabin

	pDC->MoveTo(0, 0);
	//lower cabin part
	pDC->Rectangle(CRect(lowerCabinStart,lowerCabinEnd));
	// upper cabin part
	pDC->Arc(arcRect, arcStart, arcEnd);
	//cabin cable
	pDC->MoveTo(cableStart);
	pDC->LineTo(cableEnd);

	pDC->EndPath();
	//pDC->StrokePath();
	pDC->StrokeAndFillPath();

	this->WriteString(pDC, str, radius);
	this->Translate(pDC, 0, -(w + cableLenght), false);

	pDC->SelectObject(oldPen);
	pDC->SelectObject(oldBrush);
	pDC->SetArcDirection(prevAD);
}
