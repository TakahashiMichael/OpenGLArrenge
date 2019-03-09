// @file Texture.cpp

#include "Texture.h"
#include <iostream>
#include <vector>
#include <stdint.h>
#include <stdio.h>
#include <sys/stat.h>

/*
* �o�C�g�񂩂琔�l�𕜌�����.
*
* @param p			�o�C�g��ւ̃|�C���^.
* @param offset		���l�̃I�t�Z�b�g
* @param size		���l�̃o�C�g��(1~4)
*
* @return �����������l.
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
* �f�X�g���N�^.
* �����쐬����Ă���Ȃ��n��������
*/
Texture::~Texture()
{
	if (texId) {
		glDeleteTextures(1,&texId);
	}
}


/*
* �e�t�@�C��������o���Ȃ��Ƃ����Ȃ��f�[�^
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
* tga��ǂݎ��
* 
* @param tempDataPtr		�e�N�X�`���쐬�ɕK�v�Ȉꎞ�f�[�^�̃|�C���^.
*/
bool LoadTGA(Texture::TemporaryData* tempDataPtr,const char* filename) {

	//�t�@�C���̃T�C�Y���擾����.
	struct stat st;
	if (stat(filename, &st)) {
		return false;
	}
	FILE* fp = fopen(filename, "rb");
	if (!fp) {
		return false;
	}

	//�t�@�C����ǂݍ���.
	std::vector<uint8_t> buf;
	buf.resize(st.st_size);
	const size_t readSize = fread(buf.data(), 1, st.st_size, fp);
	fclose(fp);

	//readSize �t�@�C���̃T�C�Y
	//buf �t�@�C���̃f�[�^.
	//�w�b�_�[�̏����擾����.
	const uint8_t* pHeader = buf.data();

	const uint32_t imageId = Get(pHeader,0,1);
	size_t colorMapSize = 0;
	if (Get(pHeader, 1, 1)) {
		const uint32_t colorMapLength = Get(pHeader,5,2);
		const uint32_t colorMapEntrySize = Get(pHeader,7,1);
		colorMapSize = colorMapLength * colorMapEntrySize / 8;
	}

	//�摜�f�[�^���i�[����Ă���f�[�^�I�t�Z�b�g.
	const size_t offsetBytes = 18 + imageId + colorMapSize;

	tempDataPtr->width = Get(pHeader,12,2);
	tempDataPtr->height = Get(pHeader,14,2);
	const uint32_t pixelDepth = Get(pHeader,16,1);
	const size_t imageSize = tempDataPtr->width * tempDataPtr->height *pixelDepth / 8;
	std::cout << "whidth�̃T�C�Y::" << (int)tempDataPtr->width << std::endl;
	std::cout << "height�̃T�C�Y::" << (int)tempDataPtr->height << std::endl;
	std::cout << "height�̃T�C�Y::" << (long)imageSize << std::endl;


	tempDataPtr->buf.resize(imageSize);
	std::cout << "�T�C�Y:" << tempDataPtr->buf.size();
	//�f�[�^��buf�Ɋi�[����.
	for (size_t i = 0; i < imageSize; ++i) {
		tempDataPtr->buf[i] = buf[offsetBytes + i];
		long test= tempDataPtr->buf[i];

	}

	//tga�͐ΐԓ����Ȃ�
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
	//GL_RGBA8�ő��v���Ǝv��.
	tempDataPtr->iformat = GL_RGBA8;

	return true;
}


/**
* 2D�e�N�X�`�����쐬����.
*
* @param width   �e�N�X�`���̕�(�s�N�Z����).
* @param height  �e�N�X�`���̍���(�s�N�Z����).
* @param iformat �e�N�X�`���̃f�[�^�`��.
* @param format  �A�N�Z�X����v�f.
* @param data    �e�N�X�`���f�[�^�ւ̃|�C���^.
*
* @return �쐬�ɐ��������ꍇ�̓e�N�X�`���|�C���^��Ԃ�.
*         ���s�����ꍇ��nullptr�Ԃ�.
*/
TexturePtr Texture::Create(
	int width, int height, GLenum iformat, GLenum format, const void* data ,GLenum type)
{
	struct Impl :Texture {};	//�������邽�߂�struct���`.
	TexturePtr p = std::make_shared<Impl>();

	p->width = width;
	p->height = height;
	glGenTextures(1,&p->texId);
	glBindTexture(GL_TEXTURE_2D,p->texId);
	glTexImage2D(
		GL_TEXTURE_2D,0,iformat,width,height,0,format,type,data);
	const GLenum result = glGetError();
	if (result != GL_NO_ERROR) {
		std::cerr << "ERORR �e�N�X�`���̍쐬�Ɏ��s: 0x" << std::hex << result << std::endl;
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
* �t�@�C������2D�e�N�X�`����ǂݍ���.
*
* @param filename �t�@�C����.
*
* @return �쐬�ɐ��������ꍇ�̓e�N�X�`���|�C���^��Ԃ�.
*         ���s�����ꍇ��nullptr�Ԃ�.
*/
TexturePtr Texture::LoadFromFile(const char* filename)
{
	TemporaryData tempData;

	//�t�@�C���̃T�C�Y���擾����.
	struct stat st;
	if (stat(filename, &st)) {
		return {};
	}
	////�t�@�C���T�C�Y��BMP�t�@�C���̏���ێ��ł���T�C�Y�����Ȃ��̃I�u�W�F�N�g��Ԃ�.
	//const size_t bmpFileHeaderSize = 14;	//�r�b�g�p�b�v�t�@�C���w�b�_�̃o�C�g��.
	//const size_t windowsV1HeaderSize = 40; // �r�b�g�}�b�v���w�b�_�̃o�C�g��.
	//if (st.st_size < bmpFileHeaderSize + windowsV1HeaderSize) {
	//	return {};
	//}
	FILE* fp = fopen(filename,"rb");
	if (!fp) {
		return {};
	}
	
	//�t�@�C����ǂݍ���.
	std::vector<uint8_t> buf;
	buf.resize(st.st_size);
	const size_t readSize = fread(buf.data(),1,st.st_size,fp);
	fclose(fp);
	//�ǂݍ��񂾃t�@�C���T�C�Y���m�F.
	if (readSize != st.st_size) {
		return {};
	}
	//BMP�t�@�C�����ǂ������m�F����.
	const uint8_t* pHeader = buf.data();
	if (pHeader[0] == 'B' && pHeader[1] == 'M') {

		//BMP�t�@�C���̏����擾����.
		const size_t offsetBytes = Get(pHeader, 10, 4);
		const uint32_t infoSize = Get(pHeader, 14, 4);
		const uint32_t width = Get(pHeader, 18, 4);
		const uint32_t height = Get(pHeader, 22, 4);
		const uint32_t bitCount = Get(pHeader, 28, 2);
		const uint32_t compression = Get(pHeader, 30, 4);
		const size_t pixelBytes = bitCount / 8;

		const size_t windowsV1HeaderSize = 40; // �r�b�g�}�b�v���w�b�_�̃o�C�g��.

		if (infoSize != windowsV1HeaderSize || bitCount != 24 || compression) {
			return {};

		}
		const size_t stride = ((width * pixelBytes + 3) / 4) * 4; // ���ۂ̉��o�C�g��.
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
	//�w�肵���t�@�C���ӊO�Ȃ�null��Ԃ�
	return {};

}