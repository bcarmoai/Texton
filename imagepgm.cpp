// ImagePgm.cpp: implementation of the CImagePgm class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Texton.h"
#include "ImagePgm.h"
#include <stdio.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define MAX_GREYL 256

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CImagePgm::CImagePgm()
{
	imgData = NULL;	
	imgHeight = 0;
	imgWidth = 0;

}

CImagePgm::~CImagePgm()
{
	if(imgData != NULL)
		delete imgData;

}

int CImagePgm::LoadPgm(CString fileName)
{
	int scan_res;
	long char_pos;
	char line[70];
	int iWidth, iHeight;
	int img_maxgrey;
	int temp_char;
	
	FILE* fDataFile = fopen(fileName, "r");
	
	if(fDataFile == NULL)
	{
		char szChar[100];
		sprintf(szChar, "Error opening image file %s.\n\n", fileName);
		AfxMessageBox(szChar);
		return 0;
	}
	
	rewind(fDataFile);
	
	/* check PGM magic number */
	char_pos = NextCharPos(fDataFile);
	if(char_pos == -1)
	{

		AfxMessageBox("error: source file does not contain P2 identifier (rewind).");
		return NULL;
	}
	else
	{
		fseek(fDataFile, char_pos, SEEK_SET);
		scan_res = fscanf(fDataFile, "%s", line);
	}
	
	/* check for valid string */
	if(scan_res == 0 || scan_res == EOF)
	{
		AfxMessageBox("error: source file does not contain P2 identifier (EOF).");
		return NULL;
	}
	
	/* check for PGM magic number (we do two separate checks because
	if we reached EOF in the first check then line[] is invalid) */
	if( strcmp(line, "P2") != 0)
	{
		AfxMessageBox("error: source file does not contain P2 identifier (comp).");
		return NULL;
	}
	
	/* get image parameters */
	/* image width */
	char_pos = NextCharPos(fDataFile);
	if(char_pos == -1)
	{
		AfxMessageBox("error: invalid image width\n");
		return NULL;
	}
	else
	{
		fseek(fDataFile, char_pos, SEEK_SET);
		scan_res =	fscanf(fDataFile, "%i", &imgWidth);
		
		if(scan_res <= 0 || scan_res == EOF)
		{
			AfxMessageBox("error: invalid image width.");
			return NULL;
		}
	}
	/* image height */
	char_pos = NextCharPos(fDataFile);
	if(char_pos == -1)
	{
		AfxMessageBox("error: invalid image height\n");
		return NULL;
	}
	else
	{
		fseek(fDataFile, char_pos, SEEK_SET);
		scan_res =	fscanf(fDataFile, "%i", &imgHeight);
		
		if(scan_res <= 0 || scan_res == EOF)
		{
			AfxMessageBox("error: invalid image height\n");
			return NULL;
		}
	}
	
	if(imgData != NULL)
	{
		delete imgData;
		imgData = NULL;
	}

	imgData = new int[imgWidth*imgHeight];
	
	/* image maximum grey level */
	char_pos = NextCharPos(fDataFile);
	if(char_pos == -1)
	{
		AfxMessageBox("error: invalid image maximum grey level\n");
		return NULL;
	}
	else
	{
		fseek(fDataFile, char_pos, SEEK_SET);
		scan_res =	fscanf(fDataFile, "%i", &img_maxgrey);
		
		if(scan_res <= 0 || scan_res == EOF || img_maxgrey > (MAX_GREYL+1))
		{
			AfxMessageBox("error: invalid image maximum grey level.");
			return NULL;
		}
	}
	
	if(imgWidth <= 0 || imgHeight <= 0 || img_maxgrey <= 0 )
	{
		AfxMessageBox("error: invalid image parameters.");
		return NULL;
	}
		
	/* load matrix values */
	for(iHeight=0; iHeight<imgHeight; iHeight++)
		for(iWidth=0; iWidth<imgWidth; iWidth++)
		{
			if(fscanf(fDataFile, "%i", &temp_char)==EOF)
			{
				imgData[iHeight*imgWidth+iWidth] = temp_char;
				AfxMessageBox("error: file has too few grey values.");
				return NULL;
			}
			else
				imgData[iHeight*imgWidth+iWidth] = temp_char;
			
			if( imgData[iHeight*imgWidth+iWidth] < 0)
			{
				AfxMessageBox("error: file has negative grey values.");
				return NULL;
			}
			
		}
		
		/* check that the whole file's been used */
		if(iHeight != imgHeight || iWidth != imgWidth)
		{	
			char szChar[100];
			sprintf(szChar, "warning: file is bigger than image size, \
				\n only the first %i grey\
				values have been used.\n", iHeight * iWidth);
			AfxMessageBox(szChar);
		}
		/* close file */
		fclose(fDataFile);
  
  return 1;
  
}

long CImagePgm::NextCharPos(FILE * oFile)
{
	/* positions file pointer before a valid digit or letter */
	/* skips any line that's a comment, assuming it hasn't been called 
	   in the middle of a comment */
	/* return codes: 1 - success, 0 - fail */
		/* to be used in conjuntion with load_pgm */
		/* COPYRIGHT (c) 1998 Bernardo S Carmo */

	int buf;
	long pos;

	/* BELOW: workaround for Microsoft bug */
	pos = ftell(oFile);
	fseek(oFile, pos, SEEK_SET);
	/* END of bug workaround */

	buf = fgetc(oFile);

	/* skip all whitespace and control characters */
	while(isspace(buf) || iscntrl(buf))
	{
		if(buf == EOF)
			return -1;
		buf = fgetc(oFile);
	}

	/* skip all comment lines */
	if(buf == '#')
	{
		while(buf != '\n')
		{
			if(buf == EOF)
				return -1;
			
			/* BELOW: workaround for Microsoft bug */
			pos = ftell(oFile);
			fseek(oFile, pos, SEEK_SET); // BSC changed from -1, SEEK_CUR 11/10/03
			/* END of bug workaround */

			buf = fgetc(oFile);
		}

		while(isspace(buf) || iscntrl(buf))
		{
			if(buf == EOF)
				return -1;
			buf = fgetc(oFile);
		}

		/* skip further comments */
		if(fseek(oFile, pos, SEEK_SET)!=0) // BSC changed from -1, SEEK_CUR 11/10/03
			return -1;
		return NextCharPos(oFile);

	}
	else
	{
		/* found a letter or digit so position file pointer */
		if(fseek(oFile, pos, SEEK_SET)!=0)
			return -1;
		else
			return ftell(oFile);
	}
}

void CImagePgm::SaveImagePgm(char * filename, int scale)
{
	FILE* theFile = fopen(filename, "w");
	
	if(theFile == NULL)
	{
		char szChar[100];
		sprintf(szChar, "Error opening image file %s.\n\n", filename);
		AfxMessageBox(szChar);
		return;
	}
	
	rewind(theFile);

	fprintf(theFile, "P2\n%i %i\n255\n\n", imgWidth, imgHeight);

	int x, y;
	int lines = 0;
	int toOut;
	
	for(y=0; y<imgHeight; y++)
	{
		for(x=0; x<imgWidth; x++)
		{
			toOut = imgData[imgWidth*y+x]*scale;

			if(toOut < 0 && toOut >= -255)
				toOut = 255+toOut;
			else
				if(toOut < 0)
					toOut = toOut *-1;

			fprintf(theFile, "%i ", toOut);
		}

		lines++;
		if(lines == 10)
		{
			fprintf(theFile, "\n");
			lines = 0;
		}
	}

	fclose(theFile);
	

}
