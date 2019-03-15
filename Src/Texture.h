// @file Texture.h

#ifndef TEXTURE_H_INCLUDED
#define TEXTURE_H_INCLUDED

#include <GL/glew.h>
#include <memory>

class Texture;
using TexturePtr = std::shared_ptr<Texture>;	///テクスチャのポインタ型
using TextureRef = Texture & ;

/*
* テクスチャクラス
*
*/
class Texture 
{
public:
	struct TemporaryData;

	static TexturePtr Create(
		int width, int height, GLenum iformat, GLenum format, const void* data, GLenum type);
	static TexturePtr Create(
		const TemporaryData&);
	//ファイル名からテクスチャファイルを作成する.
	static TexturePtr LoadFromFile(const char*);

	GLuint Id()const { return texId; }
	GLsizei Width()const { return width; }
	GLsizei Height()const { return height; }



private:
	Texture() = default;
	~Texture();
	Texture(TextureRef) = delete;
	TextureRef operator=(TextureRef) = delete;


	GLuint texId = 0;
	int width = 0;
	int height = 0;

};


#endif // !TEXTURE_H_INCLUDED
