// @file OffscreenBuffer.h

#ifndef OFFSCREENBUFFER_H_INCLUDED
#define OFFSCREENBUFFER_H_INCLUDED

#include "Texture.h"
#include <GL/glew.h>
#include <memory>

class OffscreenBuffer;
using OffscreenBufferPtr = std::shared_ptr<OffscreenBuffer>;	///<ポインタ型.
using OffscreenBufferRef = OffscreenBuffer & ;					///<参照型.
/*
* オフスクリーンバッファ
*
*/
class OffscreenBuffer
{
public:
	static OffscreenBufferPtr Create(int w, int h);
	GLuint GetFrameBuffer()const { return framebuffer; }	///<フレームバッファを取得
	GLuint GetTexture()const { return tex->Id(); }			///<フレームバッファ用テクスチャを取得.

private:
	OffscreenBuffer() = default;
	~OffscreenBuffer();
	OffscreenBuffer(OffscreenBufferRef) = delete;
	OffscreenBufferRef operator=(OffscreenBufferRef) = delete;

	TexturePtr tex;	///フレームバッファ用テクスチャ.
	GLuint depthbuffer = 0;	///<深度バッファオブジェクト.
	GLuint framebuffer = 0;	///<フレームバッファオブジェクト,

};

#endif // !OFFSCREENBUFFER_H_INCLUDED

