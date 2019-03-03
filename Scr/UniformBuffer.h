// @filename

#ifndef UNIFORMBUFFER_H_INCLUDED
#define UNIFORMBUFFER_H_INCLUDED
#include <GL/glew.h>
#include <string>
#include <memory>

class UniformBuffer;
using UniformBufferPtr = std::shared_ptr<UniformBuffer>;	///<UBOポインタ型.
using UniformBufferRef = UniformBuffer & ;
/*
* UBOクラス
*
*/
class UniformBuffer {
public:
	static UniformBufferPtr Create(
		GLsizeiptr size, GLuint bindingPoint,const char* name);

	bool BufferSubdata(const GLvoid* data,GLintptr offset=0,GLsizeiptr size=0);
	GLsizeiptr Size()const { return size; }
	GLuint BindingPoint()const { return bindingPoint; }
	const std::string& Name()const { return name; }

private:
	UniformBuffer() = default;
	~UniformBuffer();
	UniformBuffer(const UniformBufferRef) = delete;
	UniformBufferRef operator=(const UniformBufferRef) = delete;


	GLuint ubo = 0; ///< Uniform Buffer Object.
	GLsizeiptr size = 0; ///< UBOのバイトサイズ.
	GLuint bindingPoint; ///< UBOの割り当てて先バインディング・ポイント.
	std::string name; ///< UBO名.

};

#endif // UNIFORMBUFFER_H_INCLUDED
