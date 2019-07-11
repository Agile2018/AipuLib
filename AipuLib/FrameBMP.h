#ifndef FrameBMP_h
#define FrameBMP_h

#include <windows.h>
#include <objbase.h>
#include <gdiplus.h>

#pragma comment(lib,"gdiplus.lib")

using namespace Gdiplus;

class FrameBMP
{
public:
	FrameBMP();
	~FrameBMP();
	int BitmapFromMat(int channels, int width,
		int height, int step1, unsigned char* data,
		int cols, int rows, int elemSize1, int step);
	HBITMAP GetHbitmapFromBitmap();
private:
	ULONG_PTR gdiplusToken;
	Bitmap* bitmapFrame;
	
};


#endif // !Image_h

