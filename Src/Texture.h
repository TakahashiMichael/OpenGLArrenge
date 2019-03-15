// @file Texture.h

#ifndef TEXTURE_H_INCLUDED
#define TEXTURE_H_INCLUDED

#include <GL/glew.h>
#include <memory>

class Texture;
using TexturePtr = std::shared_ptr<Texture>;	///�e�N�X�`���̃|�C���^�^
using TextureRef = Texture & ;

/*
* �e�N�X�`���N���X
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
	//�t�@�C��������e�N�X�`���t�@�C�����쐬����.
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
