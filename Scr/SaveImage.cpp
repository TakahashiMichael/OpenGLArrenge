#include "SaveImage.h"
#include <iostream>
#include <vector>
#include <stdint.h>
#include <stdio.h>
#include <sys/stat.h>
#include <GL/glew.h>

namespace SaveImage {
	//��������ꂷ��\����
	struct ImageData {
		size_t width;
		size_t height;
		std::vector<uint8_t> colordata;

	};

	//TGA�t�@�C�����쐬����.
	void saveTGA(ImageData& data)
	{
		//�t�@�C�����쐬����o�C�g��.
		std::vector<uint8_t> header;

		std::cout << (int)data.width << std::endl;
		std::cout << (int)data.height << std::endl;


		//�����ƕ���8�r�b�g�ɂ΂炵�Ċi�[����.
		uint8_t width0 = (uint8_t)(0x00ff & data.width);
		uint8_t width1 = (uint8_t)((0xff00 & data.width) >> 8);
		uint8_t height0 = (uint8_t)(0x00ff & data.height);
		uint8_t height1 = (uint8_t)((0xff00 & data.height) >> 8);

		std::cout << (int)data.width << std::endl;
		std::cout << (int)data.height << std::endl;

		std::cout << "w0" <<std::hex<< (int)width0 << std::endl;
		std::cout << "w1" << std::hex << (int)width1 << std::endl;
		std::cout << "h0" << std::hex << (int)height0 << std::endl;
		std::cout << "h1" << std::hex << (int)height1 << std::endl;

		for (int i = 0; i < 18; ++i) {
			header.push_back(0);
			if (i == 2) {
				//���k�����J���[�摜��ݒ�.
				header[i] = 2;
			}
			//���̃s�N�Z���ݒ�
			else if (i==12) {
				header[i] = width0;
			}
			else if (i == 13) {
				header[i] = width1;
			}
			//�����̃s�N�Z���ݒ�.
			else if (i == 14) {
				header[i] = height0;
			}
			else if (i == 15) {
				header[i] = height1;
			}
			else if (i==16) {
				//32bit�`�� rgba8,
				header[i] = 32;
			}
			else if (i == 17) {
				//�悭�킩��Ȃ�����Ƃ肠�����c=�ォ��A��=������Ŋi�[.
				//�����ݒ�Ɋւ��Ă��S��������Ȃ�.
				header[i] = 32;
			}
		}
		//�T�C�Y��ύX����
		data.colordata.insert(data.colordata.begin(),header.begin(),header.end());//�A��
		FILE* fp;
		fp = fopen("Res/Save/SaveImage.tga","wb");
		//TGA�t�@�C����ۑ�����.
		fwrite(data.colordata.data(),sizeof(uint8_t),data.colordata.size(),fp);
		fclose(fp);
	}
	

	/*
	* �t�@�C����ۑ�����
	*
	* @param :FILEFORMAT	�t�@�C���̊g���q.
	* @param :width			��
	* @param :height		����
	*/
	void saveImage(FILEFORMAT format ,const unsigned int width,const unsigned int height )
	{
		const unsigned int channnelNum = 4; // RGB�Ȃ�3, RGBA�Ȃ�4

		ImageData buf;
		buf.width = width;
		buf.height = height;
		size_t imageSize = width * height*channnelNum;
		buf.colordata.resize(imageSize);

		// �ǂݎ��OpneGL�̃o�b�t�@���w�� GL_FRONT:�t�����g�o�b�t�@�@GL_BACK:�o�b�N�o�b�t�@
		glReadBuffer(GL_BACK);

		// OpenGL�ŉ�ʂɕ`�悳��Ă�����e���o�b�t�@�Ɋi�[
		glReadPixels(
			0,                 //�ǂݎ��̈�̍�������x���W
			0,                 //�ǂݎ��̈�̍�������y���W //0 or getCurrentWidth() - 1
			width,             //�ǂݎ��̈�̕�
			height,            //�ǂݎ��̈�̍���
			GL_BGRA,				//it means GL_BGR,           //�擾�������F���̌`��
			GL_UNSIGNED_BYTE,  //�ǂݎ�����f�[�^��ۑ�����z��̌^
			buf.colordata.data()      //�r�b�g�}�b�v�̃s�N�Z���f�[�^�i���ۂɂ̓o�C�g�z��j�ւ̃|�C���^
		);

		saveTGA(buf);
	}



}//namespaceSaveImage,