// loadBitmap.h
// This class loads a bitmap file into our program which we can then use for our textures
// The code has been adapted from the details in this thread: http://www.vbforums.com/showthread.php?t=261522

#ifndef LOAD_BMP_AAC
#define LOAD_BMP_AAC

#include<fstream>
#include<iostream>

class Bitmap
{
	public:
		void* LoadBitmap(const char *Filename);

	private:
		//BITMAPFILEHEADER
		typedef struct 
		{
			WORD bfType;
			DWORD bfSize;
			DWORD bfReserved;
			DWORD bfOffBits;
		} BITMAPFILEHEADER;

		//BITMAPINFOHEADER
		typedef struct 
		{
			DWORD biSize;
			LONG biWidth;
			LONG biHeight;
			WORD biPlanes;
			WORD biBitCount;
			DWORD biCompression;
			DWORD biSizeImage;
			LONG biXPelsPerMeter;
			LONG biYPelsPerMeter;
			DWORD biClrUsed;
			DWORD biClrImportant;
		} BITMAPINFOHEADER;

		BITMAPFILEHEADER FileHeader;
		BITMAPINFOHEADER InfoHeader;
};

void* Bitmap::LoadBitmap(const char *Filename)
{
	 Bitmap o_Bitmap;
	 unsigned char *bitmap_Image;
	 FILE *File=NULL;
	 unsigned int ImageIdx=0;

	 if(!Filename)
	 {
		 std::cout << "Can't find the file name!\n";
	 }
	 else
	 {
		File=fopen(Filename,"rb");
	 }

	 fread(&o_Bitmap.FileHeader,sizeof(BITMAPFILEHEADER),1,File);
	 
	 if(FileHeader.bfType != 0x4D42)
	 {
		 std::cout << "Incorrect file type!\n";
	 }

	 fread(&o_Bitmap.InfoHeader,sizeof(BITMAPINFOHEADER),1,File);
	 fseek(File,o_Bitmap.FileHeader.bfOffBits,SEEK_SET);
	 
	 if(o_Bitmap.InfoHeader.biSizeImage != 0)
	 {
		bitmap_Image=new unsigned char[o_Bitmap.InfoHeader.biSizeImage];
	 }

	 if(!bitmap_Image)
	 {
		free(bitmap_Image);
		fclose(File);
	 }

	 if(bitmap_Image==NULL)
	 {
		fclose(File);
	 }

	 for(ImageIdx=0;ImageIdx < o_Bitmap.InfoHeader.biSizeImage; ImageIdx+=3)
	 {
		 bitmap_Image[ImageIdx] = bitmap_Image[+2];
		 bitmap_Image[ImageIdx+2] = bitmap_Image[ImageIdx];
	 }

	 fclose(File);
	 return bitmap_Image;
};

#endif