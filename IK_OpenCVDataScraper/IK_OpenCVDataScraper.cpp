#include <iostream>
#include <iostream>
#include <Windows.h>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>

BITMAPINFOHEADER CreateBitmapHeader(const int width, const int height)
{
	BITMAPINFOHEADER bi;

	// create a bitmap
	bi.biSize          = sizeof(BITMAPINFOHEADER);
	bi.biWidth         = width;
	bi.biHeight        = -height;  //this is the line that makes it draw upside down or not
	bi.biPlanes        = 1;
	bi.biBitCount      = 32;
	bi.biCompression   = BI_RGB;
	bi.biSizeImage     = 0;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrUsed       = 0;
	bi.biClrImportant  = 0;

	return bi;
}

cv::Mat CaptureScreenMat(const HWND hwnd)
{
	cv::Mat src;

	// get handles to a device context (DC)
	const HDC hwindowDC           = GetDC(hwnd);
	const HDC hwindowCompatibleDC = CreateCompatibleDC(hwindowDC);
	SetStretchBltMode(hwindowCompatibleDC, COLORONCOLOR);

	// define scale, height and width
	constexpr int screenX = 0;
	constexpr int screenY = 0;
	constexpr int width   = 2560;
	constexpr int height  = 1440;

	// create mat object
	src.create(height, width, CV_8UC4);

	// create a bitmap
	const HBITMAP hbwindow = CreateCompatibleBitmap(hwindowDC, width, height);
	BITMAPINFOHEADER bi    = CreateBitmapHeader(width, height);

	// use the previously created device context with the bitmap
	SelectObject(hwindowCompatibleDC, hbwindow);

	// copy from the window device context to the bitmap device context
	StretchBlt(hwindowCompatibleDC,
			   0,
			   0,
			   width,
			   height,
			   hwindowDC,
			   screenX,
			   screenY,
			   width,
			   height,
			   SRCCOPY);  //change SRCCOPY to NOTSRCCOPY for wacky colors !
	GetDIBits(hwindowCompatibleDC,
			  hbwindow,
			  0,
			  height,
			  src.data,
			  reinterpret_cast<BITMAPINFO*>(&bi),
			  DIB_RGB_COLORS);            //copy from hwindowCompatibleDC to hbwindow

	// avoid memory leak
	DeleteObject(hbwindow);
	DeleteDC(hwindowCompatibleDC);
	ReleaseDC(hwnd, hwindowDC);

	return src;
}

int main()
{
	const HWND hwnd   = GetDesktopWindow();
	const cv::Mat src = CaptureScreenMat(hwnd);
	
	namedWindow("Display window", cv::WINDOW_AUTOSIZE); // Create a window for display.
	imshow("Display window", src); // Show our image inside it.
	cv::waitKey(0); // Wait for a keystroke in the window
	return 0;
}
