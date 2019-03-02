
#include <iostream>
#include "GLFWEW.h"
#include "Geometry.h"
#include <vector>

const Vertex vertices[] = {
{ {-0.5f, -0.43f, 0.5f}, {0.0f, 0.0f, 1.0f, 1.0f} },
{ { 0.5f, -0.43f, 0.5f}, {0.0f, 1.0f, 0.0f, 1.0f} },
{ { 0.0f,  0.43f, 0.5f}, {1.0f, 0.0f, 0.0f, 1.0f} },
};



/*
* VBOを作成する
* @param size 頂点データのサイズ.
* @param data 頂点データへのポインタ.
*
* @retval 作成したVBO
*/
GLuint CreateVBO(GLsizeiptr size, const GLvoid* data)
{
	GLuint vbo = 0;
	glGenBuffers(1,&vbo);
	glBindBuffer(GL_ARRAY_BUFFER,vbo);
	glBufferData(GL_ARRAY_BUFFER,size,data,GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER,0);
	return vbo;
}

/**
* 頂点アトリビュートを設定する.
*
* @param index 頂点アトリビュートのインデックス.
* @param cls   頂点データ型名.
* @param mbr   頂点アトリビュートに設定するclsのメンバ変数名.
*/
#define SetVertexAttribPointer(index, cls, mbr) SetVertexAttribPointerI( \
  index, \
  sizeof(cls::mbr) / sizeof(float), \
  sizeof(cls), \
  reinterpret_cast<GLvoid*>(offsetof(cls, mbr)))
void SetVertexAttribPointerI(
	GLuint index, GLint size, GLsizei stride, const GLvoid* pointer)
{
	glEnableVertexAttribArray(index);
	glVertexAttribPointer(index, size, GL_FLOAT, GL_FALSE, stride, pointer);
}


/**
* Vertex Array Objectを作成する.
*
* @param vbo VAOに関連付けられるVBO.
*
* @return 作成したVAO.
*/
GLuint CreateVAO(GLuint vbo)
{
	GLuint vao = 0;
	glGenVertexArrays(1,&vao);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER,vbo);
	SetVertexAttribPointer(0, Vertex, position);
	SetVertexAttribPointer(1, Vertex, color);
	glBindVertexArray(0);
	glDeleteBuffers(1,&vbo);
	return vao;
}

/**
* シェーダコードをコンパイルする.
*
* @param type シェーダの種類.
* @param string シェーダコードへのポインタ.
*
* @return 作成したシェーダオブジェクト.
*/
GLuint CompileShader(GLenum type ,const GLchar* string)
{
	GLuint shader = glCreateShader(type);
	glShaderSource(shader,1,&string,nullptr);
	glCompileShader(shader);
	GLint compiled = 0;
	glGetShaderiv(shader,GL_COMPILE_STATUS,&compiled);
	if (!compiled) {
		GLint infoLen = 0;
		glGetShaderiv(shader,GL_INFO_LOG_LENGTH,&infoLen);
		if (infoLen) {
			std::vector<char> buf;
			buf.resize(infoLen);
			if (static_cast<int>(buf.size()) >= infoLen) {
				glGetShaderInfoLog(shader,infoLen,NULL,buf.data());
				std::cerr << "ERROR: シェーダーのコンパイルに失敗\n" << buf.data() << std::endl;
			}
		}
		//失敗した場合.シェーダを削除して終了.
		glDeleteShader(shader);
		return 0;
	}
	return shader;
}

/**
* プログラムオブジェクトを作成する.
*
* @param vsCode 頂点シェーダコードへのポインタ.
* @param fsCode フラグメントシェーダコードへのポインタ.
*
* @return 作成したプログラムオブジェクト.
*/
GLuint CreateShaderProgram(const GLchar* vsCode, const GLchar* fsCode)
{
	GLuint vs = CompileShader(GL_VERTEX_SHADER,vsCode);
	GLuint fs = CompileShader(GL_FRAGMENT_SHADER,fsCode);
	if (!vs || !fs) {
		return 0;
	}
	GLuint program = glCreateProgram();
	glAttachShader(program, fs);
	glDeleteShader(fs);
	glAttachShader(program, vs);
	glDeleteShader(vs);
	glLinkProgram(program);
	GLint linkStatus = GL_FALSE;
	glGetProgramiv(program,GL_LINK_STATUS,&linkStatus);
	if (linkStatus != GL_TRUE) {
		GLint infoLen = 0;
		glGetProgramiv(program ,GL_INFO_LOG_LENGTH,&infoLen);
		if (infoLen) {
			std::vector<char> buf;
			buf.resize(infoLen);
			if (static_cast<int>(buf.size()) >= infoLen) {
				glGetProgramInfoLog(program,infoLen,NULL,buf.data());
				std::cerr << "ERROR:シェーダのリンクに失敗\n" << buf.data() << std::endl;
			}
		}
		glDeleteProgram(program);
		return 0;
	}
	return program;
}

/// 頂点シェーダ.
static const char* vsCode =
"#version 410 \n"
"layout(location=0) in vec3 vPosition;"
"layout(location=1) in vec4 vColor;"
"layout(location=0) out vec4 outColor;"
"void main() {"
"  outColor = vColor;"
"  gl_Position = vec4(vPosition, 1.0);"
"}";

/// フラグメントシェーダ.
static const char* fsCode =
"#version 410 \n"
"layout(location=0) in vec4 inColor;"
"out vec4 fragColor;"
"void main() {"
"  fragColor = inColor;"
"}";


int main() {
	//シングルトン取得
	GLFWEW::WindowRef window = GLFWEW::Window::Instance();

	if (!window.Init(800,600,"opengl")) {
		std::cerr << "ERORR:GLFWEWの初期化に失敗しました" << std::endl;
		return 1;
	}

	//色々作成
	const GLuint vbo = CreateVBO(sizeof(vertices),vertices);
	const GLuint vao = CreateVAO(vbo);
	const GLuint shaderProgram = CreateShaderProgram(vsCode,fsCode);
	if (!vbo || !vao || !shaderProgram) {
		return 1;
	}

	// メインループ.
	while (!window.ShouldClose()) {
		window.Clear();

		glUseProgram(shaderProgram);
		glBindVertexArray(vao);
		glDrawArrays(GL_LINE_LOOP,0,sizeof(vertices)/sizeof(vertices[0]));

		window.SwapBuffers();
	}
	glDeleteProgram(shaderProgram);
	glDeleteVertexArrays(1,&vao);

	return 0;
}