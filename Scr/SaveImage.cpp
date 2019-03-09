#include "SaveImage.h"
#include <iostream>
#include <vector>
#include <stdint.h>
#include <stdio.h>
#include <sys/stat.h>
#include <GL/glew.h>

namespace SaveImage {
	//あれをあれする構造体
	struct ImageData {
		size_t width;
		size_t height;
		std::vector<uint8_t> colordata;

	};

	//TGAファイルを作成する.
	void saveTGA(ImageData& data)
	{
		//ファイルを作成するバイト列.
		std::vector<uint8_t> header;

		std::cout << (int)data.width << std::endl;
		std::cout << (int)data.height << std::endl;


		//高さと幅を8ビットにばらして格納する.
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
				//圧縮無しカラー画像を設定.
				header[i] = 2;
			}
			//幅のピクセル設定
			else if (i==12) {
				header[i] = width0;
			}
			else if (i == 13) {
				header[i] = width1;
			}
			//高さのピクセル設定.
			else if (i == 14) {
				header[i] = height0;
			}
			else if (i == 15) {
				header[i] = height1;
			}
			else if (i==16) {
				//32bit形式 rgba8,
				header[i] = 32;
			}
			else if (i == 17) {
				//よくわからないからとりあえず縦=上から、横=左からで格納.
				//属性設定に関してが全く分からない.
				header[i] = 32;
			}
		}
		//サイズを変更する
		data.colordata.insert(data.colordata.begin(),header.begin(),header.end());//連結
		FILE* fp;
		fp = fopen("Res/Save/SaveImage.tga","wb");
		//TGAファイルを保存する.
		fwrite(data.colordata.data(),sizeof(uint8_t),data.colordata.size(),fp);
		fclose(fp);
	}
	

	/*
	* ファイルを保存する
	*
	* @param :FILEFORMAT	ファイルの拡張子.
	* @param :width			幅
	* @param :height		高さ
	*/
	void saveImage(FILEFORMAT format ,const unsigned int width,const unsigned int height )
	{
		const unsigned int channnelNum = 4; // RGBなら3, RGBAなら4

		ImageData buf;
		buf.width = width;
		buf.height = height;
		size_t imageSize = width * height*channnelNum;
		buf.colordata.resize(imageSize);

		// 読み取るOpneGLのバッファを指定 GL_FRONT:フロントバッファ　GL_BACK:バックバッファ
		glReadBuffer(GL_BACK);

		// OpenGLで画面に描画されている内容をバッファに格納
		glReadPixels(
			0,                 //読み取る領域の左下隅のx座標
			0,                 //読み取る領域の左下隅のy座標 //0 or getCurrentWidth() - 1
			width,             //読み取る領域の幅
			height,            //読み取る領域の高さ
			GL_BGRA,				//it means GL_BGR,           //取得したい色情報の形式
			GL_UNSIGNED_BYTE,  //読み取ったデータを保存する配列の型
			buf.colordata.data()      //ビットマップのピクセルデータ（実際にはバイト配列）へのポインタ
		);

		saveTGA(buf);
	}



}//namespaceSaveImage,