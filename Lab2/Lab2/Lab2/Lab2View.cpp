
// Lab2View.cpp : implementation of the CLab2View class
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "Lab2.h"
#endif

#include "Lab2Doc.h"
#include "Lab2View.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CLab2View

IMPLEMENT_DYNCREATE(CLab2View, CView)

BEGIN_MESSAGE_MAP(CLab2View, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()

// CLab2View construction/destruction

CLab2View::CLab2View() noexcept
{
	// TODO: add construction code here
	this->gridOn = true;
	this->bigAngle = 0;
	this->smallAngle = 0;

}

CLab2View::~CLab2View()
{
}

BOOL CLab2View::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CLab2View drawing

void CLab2View::Rotate(CDC* pDC, float angle, bool rightMultiply)
{
	XFORM xform = this->GetIdentityMatrix();
	float pi = 3.141592;
	double alpha = angle * pi / 180.0;
	xform.eM11 = cos(alpha);
	xform.eM12= sin(alpha);
	xform.eM21 = -sin(alpha);
	xform.eM22 = cos(alpha);

	pDC->ModifyWorldTransform(&xform, (rightMultiply) ? MWT_RIGHTMULTIPLY : MWT_LEFTMULTIPLY);
}

void CLab2View::DrawFigure(CDC* pDC)
{
	XFORM xform, oldxform;
	pDC->GetWorldTransform(&oldxform);
	xform = this->GetIdentityMatrix();

	int oldMapMode = pDC->GetMapMode();
	
	// center coordinates at the bottom of the cactus tree
	int xOrg = 10 * step;
	int yOrg = 17 * step;
	
	CPoint cactusLoc(0, 0);
	#pragma region stem
	this->Translate(pDC, xOrg, yOrg, false);
	this->Rotate(pDC, this->bigAngle, false);
	this->DrawCactus(pDC, L, true, cactusLoc);
	this->Rotate(pDC, -this->bigAngle, false);

	#pragma endregion

	#pragma region rootNode
	this->DrawNode(pDC, 0, 0);
	#pragma endregion 

	#pragma region pot
	this->Translate(pDC, -xOrg, -yOrg, false);
	this->DrawPot(pDC);
	#pragma endregion

	#pragma region main body
	this->Translate(pDC, xOrg, yOrg, false);
	this->Rotate(pDC, this->bigAngle, false);
	this->Translate(pDC, 0, -3 * step, false);
	pDC->GetWorldTransform(&xform);
	#pragma region left
	//small branch
    this->Rotate(pDC, -45, false);
	this->DrawCactus(pDC, S, false, cactusLoc);
	//large branch
	this->Translate(pDC, 0, -3 * step, false);
	this->Rotate(pDC, this->smallAngle, false);
	this->DrawCactus(pDC, L, true, cactusLoc);
	//node
	this->DrawNode(pDC, 0, 0);
	#pragma endregion

	#pragma region middle 
	pDC->SetWorldTransform(&xform);
	//small branch
	this->DrawCactus(pDC, S, false, cactusLoc);
	#pragma endregion

	#pragma region right
	//small branch
	this->Rotate(pDC, 45, false);
	this->DrawCactus(pDC, S, false, cactusLoc);
	//three branches in a loop
	this->Translate(pDC, 0, -3 * step, false);
	this->Rotate(pDC, -45, false);
	XFORM xformLoop;
	pDC->GetWorldTransform(&xformLoop);
	for (int i = 1; i <= 3; i++)
	{
		//small branch
		this->DrawCactus(pDC, S, false, cactusLoc);
		this->Translate(pDC, 0, -3 * step, false);
		//large branch
		this->DrawCactus(pDC, L, false, cactusLoc);
		//node between small and large branch
		this->DrawNode(pDC, 0, 0);
		//rotate for next branch
		if (i != 3) 
		{
			this->Translate(pDC, 0, 3 * step, false);
			this->Rotate(pDC, 45, false);
		}
	}
	//node between small branch and 3 branches
	pDC->SetWorldTransform(&xformLoop);
	this->DrawNode(pDC, 0, 0);

	#pragma endregion

	// node between stem and main body
	pDC->SetWorldTransform(&xform);
	this->DrawNode(pDC, 0, 0);
	#pragma endregion


	pDC->SetMapMode(oldMapMode);
	pDC->SetWorldTransform(&oldxform);
	//pDC->SetViewportOrg(0, 0);
}

void CLab2View::DrawPot(CDC* pDC)
{
	CPen* pen = new CPen(PS_SOLID, 1, Black);
	CPen* oldPen = pDC->SelectObject(pen);

	CBrush* brush = new CBrush(Orange);
	CBrush* oldBrush = pDC->SelectObject(brush);

	// lower part of the pot
	CPoint* lowerPoints = new CPoint[4];
	lowerPoints[0] = CPoint(8 * step, 18 * step);
	lowerPoints[1] = CPoint(12 * step, 18 * step);
	lowerPoints[2] = CPoint(11.67 * step, 20 * step);
	lowerPoints[3] = CPoint(8.33 * step, 20 * step);
	pDC->Polygon(lowerPoints, 4);

	//upper part of the pot
	CPoint p1 = CPoint(7.5 * step, 17.2 * step);
	CPoint p2 = CPoint(12.5 * step, 18 * step);
	pDC->Rectangle(p1.x, p1.y, p2.x, p2.y);

	pDC->SelectObject(oldPen);
	pDC->SelectObject(oldBrush);
	delete pen;
	delete brush;
	
	delete[] lowerPoints;
}

void CLab2View::DrawCactus(CDC* pDC, float size, bool light, CPoint location)
{
	//load cactus from meta file
	CString filename((light) ?
		"cactus_part_light.emf" :
		"cactus_part.emf");
	HENHMETAFILE cactusPart = GetEnhMetaFile(filename);
	//scale to proper size
	this->Scale(pDC, size, 1, false);
	//draw
	CPoint topLeft(location.x - step, location.y - 3 * step);
	CPoint bottomRight(location.x + step, location.y);
	pDC->PlayMetaFile(cactusPart, new CRect(topLeft, bottomRight));
	//revert scale 
	this->Scale(pDC, 1 / size, 1, false);
}

void CLab2View::DrawNode(CDC* pDC, int x, int y)
{
	CPen pen(PS_SOLID, 1, Black);
	CPen* oldPen = pDC->SelectObject(&pen);

	CBrush brush(Green);
	CBrush* oldBrush = pDC->SelectObject(&brush);

	double size = this->step / 2.0;
	CPoint a(x - size, y - size);
	CPoint b(x + size, y + size);
	pDC->Ellipse(a.x, a.y, b.x, b.y);

	pDC->SelectObject(oldPen);
	pDC->SelectObject(oldBrush);
}

void CLab2View::DrawSignature(CDC* pDC)
{
	CFont font;
	int oldBgMode = pDC->GetBkMode();
	pDC->SetBkMode(TRANSPARENT);

	font.CreateFont(30, 0, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 0, 0, CString("Arial"));
	CFont* oldFont = pDC->SelectObject(&font);
	pDC->TextOutW(step, step, CString("17269 Dimitrije Mitic"));

	COLORREF oldColor = pDC->SetTextColor(Yellow);
	pDC->TextOutW(step - 2, step + 2, CString("17269 Dimitrije Mitic"));

	pDC->SetBkMode(oldBgMode);
	pDC->SelectObject(oldFont);
	font.DeleteObject();
}

XFORM CLab2View::GetIdentityMatrix()
{
	XFORM xform;
	xform.eM11 = 1;
	xform.eM22 = 1;
	xform.eM12 = 0;
	xform.eM21 = 0;
	xform.eDx = 0;
	xform.eDy = 0;

	return xform;
}

void CLab2View::OnDraw(CDC* pDC)
{
	CLab2Doc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: add draw code for native data here
	this->DrawCanvas(pDC);
	int prevMode = pDC->SetGraphicsMode(GM_ADVANCED);

	//rotate whole picture
	pDC->SetViewportOrg(size, size);
	this->Rotate(pDC, -180, false);

	this->DrawFigure(pDC);

	if (this->gridOn)
		this->DrawGrid(pDC);

	this->DrawSignature(pDC);
	this->Rotate(pDC, 180, false);
	
	pDC->SetViewportOrg(0, 0);
	pDC->SetGraphicsMode(prevMode);
}


// CLab2View printing

BOOL CLab2View::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CLab2View::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CLab2View::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}


// CLab2View diagnostics

#ifdef _DEBUG
void CLab2View::AssertValid() const
{
	CView::AssertValid();
}

void CLab2View::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CLab2Doc* CLab2View::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CLab2Doc)));
	return (CLab2Doc*)m_pDocument;
}
#endif //_DEBUG


// CLab2View message handlers

int CLab2View::mod(int k, int n)
{
	return ((k %= n) < 0) ? k + n : k;
}

void CLab2View::DrawCanvas(CDC* pDC)
{
	CPen* pen = new CPen(PS_SOLID, 5, BlueBg);
	CPen* oldPen = pDC->SelectObject(pen);
	CBrush* bgBrush = new CBrush(BlueBg);
	CBrush* oldBrush = pDC->SelectObject(bgBrush);

	pDC->Rectangle(0, 0, size, size);
	pDC->SelectObject(oldPen);
	pDC->SelectObject(oldBrush);

	delete pen; 
	delete bgBrush;
}

void CLab2View::DrawGrid(CDC* pDC)
{
	CPen* pen = new CPen(PS_SOLID, 2, GridColor);
	CPen* oldPen = pDC->SelectObject(pen);
	pDC->MoveTo(0, 0);
	for (int i = 1; i < 20; i++)
	{
		pDC->MoveTo(0, i * step);
		pDC->LineTo(size, i * step);
		pDC->MoveTo(i * step, 0);
		pDC->LineTo(i * step, size);
	}
}

void CLab2View::Translate(CDC* pDC, float dX, float dY, bool rightMultiply)
{
	XFORM xform = this->GetIdentityMatrix();
	xform.eDx = dX;
	xform.eDy = dY;
	pDC->ModifyWorldTransform(&xform, (rightMultiply) ? MWT_RIGHTMULTIPLY : MWT_LEFTMULTIPLY);
}

void CLab2View::Scale(CDC* pDC, float sX, float sY, bool rightMultiply)
{
	XFORM xform = this->GetIdentityMatrix();
	xform.eM11 = sX;
	xform.eM22 = sY;
	pDC->ModifyWorldTransform(&xform, (rightMultiply) ? MWT_RIGHTMULTIPLY : MWT_LEFTMULTIPLY);
}


void CLab2View::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default
	if (nChar == ' ')
	{
		this->gridOn = !this->gridOn;
		Invalidate();
	}
	else
	{
		switch (nChar)
		{
		case VK_LEFT:
			this->bigAngle = mod(this->bigAngle - 30, 360);
			Invalidate();
			break;
		case VK_RIGHT:
			this->bigAngle = mod(this->bigAngle + 30, 360);
			Invalidate();
			break;
		case 'A':
			this->smallAngle = mod(this->smallAngle - 30, 360);
			Invalidate();
			break;
		case 'D':
			this->smallAngle = mod(this->smallAngle + 30, 360);
			Invalidate();
			break;
		default:
			break;
		}
	}
	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}
