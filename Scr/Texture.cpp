// @file Texture.cpp

#include "Texture.h"
#include <iostream>
#include <vector>
#include <stdint.h>
#include <stdio.h>
#include <sys/stat.h>

/*
* バイト列から数値を復元する.
*
* @param p			バイト列へのポインタ.
* @param offset		数値のオフセット
* @param size		数値のバイト数(1~4)
*
* @return 復元した数値.
*/
uint32_t Get(const uint8_t* p ,size_t offset, size_t size)
{
	uint32_t n = 0;
	p += offset;
	for (size_t i = 0; i < size;++i) {
		n += p[i] << (i * 8);
	}
	return n;
}

/*
* デストラクタ.
* もし作成されているなら後始末をする
*/
Texture::~Texture()
{
	if (texId) {
		glDeleteTextures(1,&texId);
	}
}


/*
* 各ファイルから取り出さないといけないデータ
*
*
*
*/

struct Texture::TemporaryData
{
	uint32_t width=0;
	uint32_t height=0;
	GLenum iformat=0;
	GLenum format=0;
	GLenum type = 0;
	std::vector<uint8_t> buf;
};

/*
* tgaを読み取る
* 
* @param tempDataPtr		テクスチャ作成に必要な一時データのポインタ.
*/
bool LoadTGA(Texture::TemporaryData* tempDataPtr,const char* filename) {

	//ファイルのサイズを取得する.
	struct stat st;
	if (stat(filename, &st)) {
		return false;
	}
	FILE* fp = fopen(filename, "rb");
	if (!fp) {
		return false;
	}

	//ファイルを読み込む.
	std::vector<uint8_t> buf;
	buf.resize(st.st_size);
	const size_t readSize = fread(buf.data(), 1, st.st_size, fp);
	fclose(fp);

	//readSize ファイルのサイズ
	//buf ファイルのデータ.
	//ヘッダーの情報を取得する.
	const uint8_t* pHeader = buf.data();

	const uint32_t imageId = Get(pHeader,0,1);
	size_t colorMapSize = 0;
	if (Get(pHeader, 1, 1)) {
		const uint32_t colorMapLength = Get(pHeader,5,2);
		const uint32_t colorMapEntrySize = Get(pHeader,7,1);
		colorMapSize = colorMapLength * colorMapEntrySize / 8;
	}

	//画像データが格納されてあるデータオフセット.
	const size_t offsetBytes = 18 + imageId + colorMapSize;

	tempDataPtr->width = Get(pHeader,12,2);
	tempDataPtr->height = Get(pHeader,14,2);
	const uint32_t pixelDepth = Get(pHeader,16,1);
	const size_t imageSize = tempDataPtr->width * tempDataPtr->height *pixelDepth / 8;
	std::cout << "whidthのサイズ::" << (int)tempDataPtr->width << std::endl;
	std::cout << "heightのサイズ::" << (int)tempDataPtr->height << std::endl;
	std::cout << "heightのサイズ::" << (long)imageSize << std::endl;


	tempDataPtr->buf.resize(imageSize);
	std::cout << "サイズ:" << tempDataPtr->buf.size();
	//データをbufに格納する.
	for (size_t i = 0; i < imageSize; ++i) {
		tempDataPtr->buf[i] = buf[offsetBytes + i];
		long test= tempDataPtr->buf[i];

	}

	//tgaは青緑赤透明なの
	tempDataPtr->format = GL_BGRA;
	tempDataPtr->type = GL_UNSIGNED_BYTE;
	if (Get(pHeader, 2, 1)==3) {
		tempDataPtr->format = GL_RED;
	}
	if (Get(pHeader, 16, 1) == 24) {
		tempDataPtr->format = GL_BGR;
	}
	else if (Get(pHeader,16,1)==16) {
		tempDataPtr->type = GL_UNSIGNED_SHORT_1_5_5_5_REV;
	}
	//GL_RGBA8で大丈夫だと思う.
	tempDataPtr->iformat = GL_RGBA8;

	return true;
}


/**
* 2Dテクスチャを作成する.
*
* @param width   テクスチャの幅(ピクセル数).
* @param height  テクスチャの高さ(ピクセル数).
* @param iformat テクスチャのデータ形式.
* @param format  アクセスする要素.
* @param data    テクスチャデータへのポインタ.
*
* @return 作成に成功した場合はテクスチャポインタを返す.
*         失敗した場合はnullptr返す.
*/
TexturePtr Texture::Create(
	int width, int height, GLenum iformat, GLenum format, const void* data ,GLenum type)
{
	struct Impl :Texture {};	//生成するためにstructを定義.
	TexturePtr p = std::make_shared<Impl>();

	p->width = width;
	p->height = height;
	glGenTextures(1,&p->texId);
	glBindTexture(GL_TEXTURE_2D,p->texId);
	glTexImage2D(
		GL_TEXTURE_2D,0,iformat,width,height,0,format,type,data);
	const GLenum result = glGetError();
	if (result != GL_NO_ERROR) {
		std::cerr << "ERORR テクスチャの作成に失敗: 0x" << std::hex << result << std::endl;
		return {};
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);


	glBindTexture(GL_TEXTURE_2D,0);

	return p;
}

TexturePtr Texture::Create(const TemporaryData& tempRef) {
	return Create(tempRef.width, tempRef.height, tempRef.iformat, tempRef.format, tempRef.buf.data(), tempRef.type);

}

/**
* ファイルから2Dテクスチャを読み込む.
*
* @param filename ファイル名.
*
* @return 作成に成功した場合はテクスチャポインタを返す.
*         失敗した場合はnullptr返す.
*/
TexturePtr Texture::LoadFromFile(const char* filename)
{
	TemporaryData tempData;

	//ファイルのサイズを取得する.
	struct stat st;
	if (stat(filename, &st)) {
		return {};
	}
	////ファイルサイズがBMPファイルの情報を保持できるサイズ未満なら空のオブジェクトを返す.
	//const size_t bmpFileHeaderSize = 14;	//ビットパップファイルヘッダのバイト数.
	//const size_t windowsV1HeaderSize = 40; // ビットマップ情報ヘッダのバイト数.
	//if (st.st_size < bmpFileHeaderSize + windowsV1HeaderSize) {
	//	return {};
	//}
	FILE* fp = fopen(filename,"rb");
	if (!fp) {
		return {};
	}
	
	//ファイルを読み込む.
	std::vector<uint8_t> buf;
	buf.resize(st.st_size);
	const size_t readSize = fread(buf.data(),1,st.st_size,fp);
	fclose(fp);
	//読み込んだファイルサイズを確認.
	if (readSize != st.st_size) {
		return {};
	}
	//BMPファイルかどうかを確認する.
	const uint8_t* pHeader = buf.data();
	if (pHeader[0] == 'B' && pHeader[1] == 'M') {

		//BMPファイルの情報を取得する.
		const size_t offsetBytes = Get(pHeader, 10, 4);
		const uint32_t infoSize = Get(pHeader, 14, 4);
		const uint32_t width = Get(pHeader, 18, 4);
		const uint32_t height = Get(pHeader, 22, 4);
		const uint32_t bitCount = Get(pHeader, 28, 2);
		const uint32_t compression = Get(pHeader, 30, 4);
		const size_t pixelBytes = bitCount / 8;

		const size_t windowsV1HeaderSize = 40; // ビットマップ情報ヘッダのバイト数.

		if (infoSize != windowsV1HeaderSize || bitCount != 24 || compression) {
			return {};

		}
		const size_t stride = ((width * pixelBytes + 3) / 4) * 4; // 実際の横バイト数.
		const size_t imageSize = stride * height;
		if (buf.size() < offsetBytes + imageSize) {
			return {};

		}
		std::cout << buf.size() << std::endl;
		return Create(width, height, GL_RGB8, GL_BGR, buf.data() + offsetBytes, GL_UNSIGNED_BYTE);

	}
	else if(LoadTGA(&tempData,filename)){
		return Create(tempData);

	}
	//指定したファイル意外ならnullを返す
	return {};

}