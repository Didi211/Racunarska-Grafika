
// Lab3View.cpp : implementation of the CLab3View class
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "Lab3.h"
#endif

#include "Lab3Doc.h"
#include "Lab3View.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
#include <string>
#include <sstream>




// CLab3View

IMPLEMENT_DYNCREATE(CLab3View, CView)

BEGIN_MESSAGE_MAP(CLab3View, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CView::OnFilePrintPreview)
	ON_WM_KEYDOWN()
END_MESSAGE_MAP()

// CLab3View construction/destruction

CLab3View::CLab3View() noexcept
{
	// TODO: add construction code here
	this->gridOn = true;
	this->puzzles = new DImage* [3];
	for (int i = 0; i < 3; i++)
	{
		this->puzzles[i] = new DImage[3];
		for (int j = 0; j < 3; j++)
		{
			std::stringstream ss;
			ss << "res/Puzzles/img_" << i+1 << "_" << j+1 << ".bmp";
			CString filename(ss.str().c_str());
			this->puzzles[i][j].Load(filename);
		}
	}
}

CLab3View::~CLab3View()
{
	for (int i = 0; i < 3; i++) {
		delete[] this->puzzles[i];
	}
	delete[] this->puzzles;
}

BOOL CLab3View::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CLab3View drawing

XFORM CLab3View::GetIdentityMatrix()
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

void CLab3View::PlacePuzzle(CDC* pDC, DImage& puzzle, double x, double y, float rotAngle, bool mirrorX, bool mirrorY, bool changeColor)
{
	float xOffset = -1.25 * step;
	float yOffset = -1.5 * step;

	#pragma region Transformations
	this->Translate(pDC, -128, -128, true);
	this->Rotate(pDC, rotAngle, true);
	this->Mirror(pDC, mirrorX, mirrorY, true);
	this->Translate(pDC, 128, 128, true);
	this->Translate(pDC, x + xOffset, y + yOffset, true);
	#pragma endregion


	#pragma region Transparency
	// 1) creating original bitmap from resource
	CBitmap* bmpOriginal = puzzle.GetBitmap();

	// applying color filter 
	this->ApplyColorFilter(pDC, bmpOriginal, changeColor);

	// 2) creating monochrome bitmap
	CBitmap* bmpMask = new CBitmap();
	BITMAP bm;
	bmpOriginal->GetBitmap(&bm);
	bm.bmWidth = 250;
	bm.bmHeight = 250;
 	bmpMask->CreateBitmap(bm.bmWidth, bm.bmHeight, 1, 1, NULL);

	// 3) creating DCs and selecting bitmaps
	CDC* srcDC = new CDC();
	CDC* dstDC = new CDC();
	srcDC->CreateCompatibleDC(pDC);
	dstDC->CreateCompatibleDC(pDC);
	CBitmap* oldSrcBmp = srcDC->SelectObject(bmpOriginal);
	CBitmap* oldDstBmp = dstDC->SelectObject(bmpMask);

	// 4) filling monochrome bitmap
	COLORREF colorForRemoval = srcDC->GetPixel(0, 0); // background color of a bitmap
	COLORREF srcPrevBkColor = srcDC->SetBkColor(colorForRemoval);
	dstDC->BitBlt(0, 0, bm.bmWidth, bm.bmHeight, srcDC, 0, 0, SRCCOPY);

	// 5) marking background in srcDC bitmap
	COLORREF clrSaveDstText = srcDC->SetTextColor(White); //white behaves like logical ! in SRCAND (all bits in RGB are 1)
	srcDC->SetBkColor(Black); //logical 0 in SRCAND (all bits in RGB are 0)
	// combining bitmap bits from dst in step 4 with src bitmap bits
	// dstDC - bg = 0 (white), fg = 1 (black)
	// srcDC - bg = 0 (black), fg = 1 (colors)
	// bgDst AND bgSRC => 0 (black)
	// fgDST AND fgSRC => 1 (colors)
	// result is image with black background (also changed bmpOriginal object)
	srcDC->BitBlt(0, 0, bm.bmWidth, bm.bmHeight, dstDC, 0, 0, SRCAND); 

	// 6) freeing resources
	dstDC->SetTextColor(clrSaveDstText);
	srcDC->SetBkColor(srcPrevBkColor);
	srcDC->SelectObject(oldSrcBmp);
	dstDC->SelectObject(oldDstBmp);
	srcDC->DeleteDC();
	dstDC->DeleteDC();
	delete srcDC;
	delete dstDC;

	// 7) memDC to pDC - preparation for final drawing 
	CDC* memDC = new CDC();
	memDC->CreateCompatibleDC(pDC);
	// first we need mask
	CBitmap* oldMemBmp = memDC->SelectObject(bmpMask);
	// memDC has monochrome bitmap (fg = 1, bg = 0)
	// result in pDC is bg from pDC and fg from memDC
	pDC->BitBlt(0, 0, bm.bmWidth, bm.bmHeight, memDC, 0, 0, SRCAND);

	//then we need original image
	memDC->SelectObject(bmpOriginal);
	

	//combining previously drawn mask with original in OR operation
	// bgMask = 0, fg = 1
	// bgOriginal = black (0), fg = colors
	// bgMask OR bgOriginal => 0
	// fgMask OR fgOriginal => 1 (in colors)
	//result is bg from pDC, and image from memDC
	pDC->BitBlt(0, 0, bm.bmWidth, bm.bmHeight, memDC, 0, 0, SRCPAINT); // SRCPAIN is logical OR
	memDC->SelectObject(oldMemBmp);
	delete memDC;

	#pragma endregion

	//reset transofrmations
	pDC->ModifyWorldTransform(nullptr, MWT_IDENTITY);
}

void CLab3View::ApplyColorFilter(CDC* pDC, CBitmap* bitmap, bool changeColor)
{
	BITMAP bm;
	bitmap->GetBitmap(&bm);
	int bufSize = 500 * 500 * 4;
	unsigned char* buf = new unsigned char[bufSize];
	bitmap->GetBitmapBits(bufSize, buf);
	
	for (int i = 0; i < bufSize / 4; i++)
	{
		if (buf [4 * i + 1] == 255) //Bez ovog ima tackice. Kad promenim vrednost i stavim bilo i, i+2 ili i+3, opet ima bele tackice
			continue;

		char gr = 64 + (buf[4 * i] + buf[4 * i + 1] + buf[4 * i + 2]) / 3;
		gr = gr <= 255 ? gr : 255;
		if (changeColor)
		{
			buf[4 * i] = gr; //blue
			buf[4 * i + 1] = gr; //green
			buf[4 * i + 2] = 255; //red
		}
		else
		{
			buf[4 * i] = gr;
			buf[4 * i + 1] = gr;
			buf[4 * i + 2] = gr;
		}
	}
	bitmap->SetBitmapBits(bufSize, buf);
}

void CLab3View::OnDraw(CDC* pDC)
{
	CLab3Doc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: add draw code for native data here
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			std::stringstream ss;
			ss << "res/Puzzles/img_" << i + 1 << "_" << j + 1 << ".bmp";
			CString filename(ss.str().c_str());
			this->puzzles[i][j].Load(filename);
		}
	}

	this->DrawCanvas(pDC);
	int prevMode = pDC->SetGraphicsMode(GM_ADVANCED);

	float xOffset = -2.25;
	float yOffset = -2.5;

	if (this->gridOn)
		this->DrawGrid(pDC);

	this->PlacePuzzle(pDC, this->puzzles[0][0], 0, 3, 7, true, false);
	this->PlacePuzzle(pDC, this->puzzles[0][1], 6 * step + 8, 8, -88, true, false);
	this->PlacePuzzle(pDC, this->puzzles[0][2], 12 * step - 2, 2, 4, true, false);
	this->PlacePuzzle(pDC, this->puzzles[1][0], -2.5, 6 * step + 3, 32, true, false);
	this->PlacePuzzle(pDC, this->puzzles[1][1], 6*step +10, 6 * step + 5, 106, false, true);
	this->PlacePuzzle(pDC, this->puzzles[1][2], 12 * step+2, 6*step + 18, -8, false, true);
	this->PlacePuzzle(pDC, this->puzzles[2][0], 5, 12 * step, -1, false, false, true);
	this->PlacePuzzle(pDC, this->puzzles[2][1], 6 * step + 8, 12 * step + 5, -85, true, false);
	this->PlacePuzzle(pDC, this->puzzles[2][2], 12 * step + 3, 12 * step, -33, true, false);




	pDC->SetGraphicsMode(prevMode);

}


// CLab3View printing

BOOL CLab3View::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CLab3View::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CLab3View::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}


// CLab3View diagnostics

#ifdef _DEBUG
void CLab3View::AssertValid() const
{
	CView::AssertValid();
}

void CLab3View::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CLab3Doc* CLab3View::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CLab3Doc)));
	return (CLab3Doc*)m_pDocument;
}
#endif //_DEBUG


// CLab3View message handlers

void CLab3View::DrawCanvas(CDC* pDC)
{
	CPen* pen = new CPen(PS_NULL, 1, White);
	CPen* oldPen = pDC->SelectObject(pen);

	pDC->Rectangle(0, 0, size, size);
	pDC->SelectObject(oldPen);
	delete pen;
}

void CLab3View::DrawGrid(CDC* pDC)
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

void CLab3View::Translate(CDC* pDC, float dX, float dY, bool rightMultiply)
{
	XFORM xform = this->GetIdentityMatrix();
	xform.eDx = dX;
	xform.eDy = dY;
	pDC->ModifyWorldTransform(&xform, rightMultiply ? MWT_RIGHTMULTIPLY : MWT_LEFTMULTIPLY);
}

void CLab3View::Scale(CDC* pDC, float sX, float sY, bool rightMultiply)
{
	XFORM xform = this->GetIdentityMatrix();
	xform.eM11 = sX;
	xform.eM22 = sY;
	pDC->ModifyWorldTransform(&xform, rightMultiply ? MWT_RIGHTMULTIPLY : MWT_LEFTMULTIPLY);
}

void CLab3View::Rotate(CDC* pDC, float angle, bool rightMultiply)
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

void CLab3View::Mirror(CDC* pDC, bool mx, bool my, bool rightMultiply)
{
	XFORM xform = this->GetIdentityMatrix();
	xform.eM11 = mx ? -1 : 1;
	xform.eM22 = my ? -1 : 1;
	pDC->ModifyWorldTransform(&xform, rightMultiply ? MWT_RIGHTMULTIPLY : MWT_LEFTMULTIPLY);
}


void CLab3View::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: Add your message handler code here and/or call default
	if (nChar == ' ')
	{
		this->gridOn = !this->gridOn;
		Invalidate();
	}
	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}
