// @file SaveImage.h

#ifndef SAVEIMAGE_H_INCLUDED
#define SAVEIMAGE_H_INCLUDED

namespace SaveImage {
	enum class FILEFORMAT{
		FILEFORMAT_TGA,
	};
	void saveImage(FILEFORMAT format,const unsigned int width,const unsigned int height);


}//navespace SaveImage


#endif // !SAVEIMAGE_H_INCLUDED
