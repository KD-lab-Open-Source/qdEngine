#pragma once

inline bool SaveTga(const char* filename,int width,int height,unsigned char* buf,
			 int byte_per_pixel);

inline bool LoadTGA(const char* filename,int& dx,int& dy,unsigned char*& buf,
			 int& byte_per_pixel);


/////////////////////////////internal/////////////////////////////
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <io.h>
#include <stdio.h>
#include <string.h>


#pragma pack(push,1)

struct TGAHeader
{
unsigned char idFieldLength;
unsigned char colorMapType;
unsigned char imageType;
unsigned short indexFirstColorMapEntry;
unsigned short countColorMapEntries;
unsigned char numberOfBitsPerColorMapEntry;
unsigned short startX;
unsigned short startY;
unsigned short width;
unsigned short height;
unsigned char bitsPerPixel;
unsigned char flags;
};
#pragma pack(pop)

bool SaveTga(const char* filename,int width,int height,unsigned char* buf,int byte_per_pixel)
{
	bool bHasAlpha=false;
	int file=_open(filename,_O_WRONLY|_O_TRUNC|_O_CREAT|_O_BINARY,_S_IREAD|_S_IWRITE);
	if(file==-1)
	{
		return false;
	}

	TGAHeader Hdr;
	Hdr.idFieldLength=0;
	Hdr.colorMapType=0;
	Hdr.imageType=byte_per_pixel==1?3:2;
	Hdr.indexFirstColorMapEntry=0;
	Hdr.countColorMapEntries=0;
	Hdr.numberOfBitsPerColorMapEntry=0;
	Hdr.startX=0;
	Hdr.startY=0;
	Hdr.width=(unsigned short)width;
	Hdr.height=(unsigned short)height;
	Hdr.bitsPerPixel=(unsigned char)(byte_per_pixel*8);
	Hdr.flags=(bHasAlpha?8:0)|0x20;

	unsigned long Numbytes=Hdr.width*Hdr.height*(Hdr.bitsPerPixel>>3);

	_write(file,&Hdr,18);
	_write(file,buf,Numbytes);
	_close(file);

	return true;
}

bool LoadTGA(const char* filename,int& dx,int& dy,unsigned char*& buf,
			 int& byte_per_pixel)
{
	int file=_open(filename,_O_RDONLY|_O_BINARY);
	if(file==-1)
	{
		return false;
	}

	TGAHeader Hdr;

	_read(file,&Hdr,18);

	byte_per_pixel=Hdr.bitsPerPixel/8;

	int colormapsize=(Hdr.countColorMapEntries*Hdr.numberOfBitsPerColorMapEntry)/8;
	_lseek(file,18+colormapsize,SEEK_SET);
	dx=Hdr.width;
	dy=Hdr.height;

	unsigned long Numbytes=Hdr.width*Hdr.height*(Hdr.bitsPerPixel>>3);

	buf=new unsigned char[Hdr.width*Hdr.height*byte_per_pixel];
	_read(file,buf,Numbytes);

	_close(file);

	bool updown=(Hdr.flags&0x20)?false:true;

	if(updown)
	{
		int maxy=dy/2;
		int size=dx*byte_per_pixel;
		unsigned char* tmp=new unsigned char[size];

		for(int y=0;y<maxy;y++)
		{
			unsigned char* p1=buf+y*size;
			unsigned char* p2=buf+(dy-1-y)*size;
			memcpy(tmp,p1,size);
			memcpy(p1,p2,size);
			memcpy(p2,tmp,size);
		}

		delete tmp;
	}

	return true;
}

