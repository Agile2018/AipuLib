#include "FrameBMP.h"
//#include <opencv2/core/core.hpp>
//#include <opencv2/highgui/highgui.hpp>
//#include <opencv2/imgproc/imgproc.hpp>
//
//using namespace cv;
//using namespace Gdiplus;

//ULONG_PTR gdiplusToken;

FrameBMP::FrameBMP()
{
	GdiplusStartupInput gdiplusStartupInput;	
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
}

FrameBMP::~FrameBMP()
{
	GdiplusShutdown(gdiplusToken);
}

int FrameBMP::BitmapFromMat(int channels, int width, 
	int height, int step1, unsigned char* data, 
	int cols, int rows, int elemSize1, int step) {
	/*Mat frame;	
	Mat frameBackgroud;
	cvtColor(frame, frameBackgroud, COLOR_RGB2BGR);*/
	int result;
	PixelFormat pxFormat;
	switch (channels)
	{
		case 1: pxFormat = PixelFormat8bppIndexed; break;
		case 3: pxFormat = PixelFormat24bppRGB;    break;
		case 4: pxFormat = PixelFormat32bppARGB;   break;
		default: return 2;
	}

	//cv::Size size = frameBackgroud.size();
	Bitmap* bmp = new Bitmap(width, height, 
		step1, pxFormat, data);

	Gdiplus::BitmapData bmpData;
	Gdiplus::Rect rect(0, 0, cols, rows);

	switch (bmp->LockBits(&rect, ImageLockModeWrite, pxFormat, &bmpData))
	{
		case Ok:  result = 1; break;
		case GenericError: return 2; break;
		case InvalidParameter: return 3; break;
		default: return 4; break;
	}

	if (elemSize1 == 1) // 1 Byte per channel (8 bit gray scale palette)
	{		
		BYTE* u8_Src = data;
		BYTE* u8_Dst = (BYTE*)bmpData.Scan0;

		int s32_RowLen = cols * channels; // != i_Mat.step !!

														// The Windows Bitmap format requires all rows to be DWORD aligned (always!)
														// while OpenCV by default stores bitmap data sequentially.
		for (int i = 0; i < rows; i++)
		{
			memcpy(u8_Dst, u8_Src, s32_RowLen);
			u8_Src += step;    // step may be e.g 3729
			u8_Dst += bmpData.Stride; // while Stride is 3732
		}
	}

	bmp->UnlockBits(&bmpData);
	if (bmp->GetLastStatus() == Gdiplus::Ok)
	{
		bitmapFrame = new Gdiplus::Bitmap(bmp->GetWidth(),
			bmp->GetHeight());
		Gdiplus::Graphics g(bitmapFrame);
		g.DrawImage(bmp, 0, 0, bmp->GetWidth(),
			bmp->GetHeight());
	}
	delete bmp;
	
	return result;
}

HBITMAP FrameBMP::GetHbitmapFromBitmap() {
	HBITMAP hBitmap = NULL;
	Status s = bitmapFrame->GetHBITMAP(Color::White, &hBitmap);
	return hBitmap;
}