
#include "GLFWEW.h"
#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>

#define ARRAY_SIZE(a) (sizeof(a)/sizeof((a)[0]))
struct Vertex
{
	glm::vec3 position; ///<座標
	glm::vec4 color;	///<色データ
};
/// 頂点データ.
const Vertex vertices[] = {
  { {-0.5f, -0.3f, 0.5f}, {0.0f, 1.0f, 0.0f, 1.0f} },
  { { 0.3f, -0.3f, 0.5f}, {0.0f, 0.0f, 1.0f, 1.0f} },
  { { 0.3f,  0.5f, 0.5f}, {1.0f, 0.0f, 0.0f, 1.0f} },
  { {-0.5f,  0.5f, 0.5f}, {0.0f, 0.0f, 1.0f, 1.0f} },

  { {-0.3f,  0.3f, 0.1f}, {0.0f, 0.0f, 1.0f, 1.0f} },
{ {-0.3f, -0.5f, 0.1f}, {0.0f, 1.0f, 1.0f, 1.0f} },
{ { 0.5f, -0.5f, 0.1f}, {0.0f, 0.0f, 1.0f, 1.0f} },
{ { 0.5f, -0.5f, 0.1f}, {1.0f, 0.0f, 0.0f, 1.0f} },
{ { 0.5f,  0.3f, 0.1f}, {1.0f, 1.0f, 0.0f, 1.0f} },
{ {-0.3f,  0.3f, 0.1f}, {1.0f, 0.0f, 0.0f, 1.0f} },

};

/// インデックスデータ.
const GLuint indices[] = {
  0, 1, 2, 2, 3, 0,
  4,5,6,7,8,9,
};



/*
* VBOを作成する
*
* @param GLsieiptr size		登録する頂点データのサイズ.
* @param const GLvoid* data バイトオフセット.
*
* @retval 作成したVBOのID
*/
GLuint CreateVBO(GLsizeiptr size,const GLvoid* data) {
	GLuint vbo = 0;
	glGenBuffers(1,&vbo);//(作成個数,idのポインタ)
	glBindBuffer(GL_ARRAY_BUFFER,vbo);//(種類,id)
	glBufferData(GL_ARRAY_BUFFER,size,data,GL_STATIC_DRAW);//第四引数はアクセスの頻度
	glBindBuffer(GL_ARRAY_BUFFER, 0);//割り当ての解除.

	return vbo;
}

/**
* Index Buffer Objectを作成する.
*
* @param size インデックスデータのサイズ.
* @param data インデックスデータへのポインタ.
*
* @return 作成したIBO.
*/
GLuint CreateIBO(GLsizeiptr size, const GLvoid* data)
{
	GLuint ibo = 0;
	glGenBuffers(1, &ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	return ibo;
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



/*
* vertex Array Objectを作成する.
*
* @param vbo VAOに関連つけられるVBO
*
* @retval 作成したVAO
*
*
*/
GLuint CreateVAO(GLuint vbo,GLuint ibo) {
	GLuint vao = 0;
	glGenVertexArrays(1,&vao);//(作成個数,idのポインタ)
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER,vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,ibo);
	/*SetVertexAttribPointer(n,str,str::meb)
	* glEnableVertexAttribArray(n),
	* glVertexAttribPointer(n,メンバサイズ,GL_FLOAT,GL_FALSE,構造体サイズ,dataoffset);
	*/
	SetVertexAttribPointer(0, Vertex, position);
	SetVertexAttribPointer(1, Vertex, color);
	glBindVertexArray(0);
	glDeleteBuffers(1, &vbo);
	glDeleteBuffers(1,&ibo);
	return vao;

}

/// 頂点シェーダ.
static const char* vsCode =
"#version 410 \n"
"layout(location=0) in vec3 vPosition;"
"layout(location=1) in vec4 vColor;"
"layout(location=0) out vec4 outColor;"
"uniform mat4x4 matMVP;"
"void main() {"
"  outColor = vColor;"
"  gl_Position = matMVP * vec4(vPosition, 1.0);"
"}";

/// フラグメントシェーダ.
static const char* fsCode =
"#version 410 \n"
"layout(location=0) in vec4 inColor;"
"out vec4 fragColor;"
"void main() {"
"  fragColor = inColor;"
"}";

/**
* シェーダコードをコンパイルする.
*
* @param type シェーダの種類.
* @param string シェーダコードへのポインタ.
*
* @return 作成したシェーダオブジェクト.
*/
GLuint CompileShader(GLenum type, const GLchar* string)
{
	GLuint shader = glCreateShader(type);
	glShaderSource(shader, 1, &string, nullptr);
	glCompileShader(shader);
	GLint compiled = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
	if (!compiled) {
		GLint infoLen = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
		if (infoLen) {
			std::vector<char> buf;
			buf.resize(infoLen);
			if (static_cast<int>(buf.size()) >= infoLen) {
				glGetShaderInfoLog(shader, infoLen, NULL, buf.data());
				std::cerr << "ERROR: シェーダのコンパイルに失敗￥n" << buf.data() << std::endl;
			}
		}
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
	GLuint vs = CompileShader(GL_VERTEX_SHADER, vsCode);
	GLuint fs = CompileShader(GL_FRAGMENT_SHADER, fsCode);
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
	glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
	if (linkStatus != GL_TRUE) {
		GLint infoLen = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLen);
		if (infoLen) {
			std::vector<char> buf;
			buf.resize(infoLen);
			if (static_cast<int>(buf.size()) >= infoLen) {
				glGetProgramInfoLog(program, infoLen, NULL, buf.data());
				std::cerr << "ERROR: シェーダのリンクに失敗￥n" << buf.data() << std::endl;
			}
		}
		glDeleteProgram(program);
		return 0;
	}
	return program;
}


int main()
{
	GLFWEW::Window& window = GLFWEW::Window::Instance();
	if (!window.Init(1280,720,"GameGameGame!!")) {

	}
	const GLuint vbo = CreateVBO(sizeof(vertices), vertices);
	const GLuint ibo = CreateIBO(sizeof(indices),indices);
	const GLuint vao = CreateVAO(vbo,ibo);

	const GLuint shaderProgram = CreateShaderProgram(vsCode, fsCode);
	if (!vbo || !vao ||!ibo|| !shaderProgram) {
		return 1;
	}

	const GLint matMVPLoc = glGetUniformLocation(shaderProgram, "matMVP");


	while (!window.ShouldClose()) {
		glClearColor(0.1f, 0.3f, 0.5f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glEnable(GL_DEPTH_TEST);

		glUseProgram(shaderProgram);
		// 座標変換行列を作成してシェーダに転送する.
		if (matMVPLoc >= 0) {
			const glm::mat4x4 matProj =
				glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
			const glm::mat4x4 matView =
				glm::lookAt(glm::vec3(2, 3, 3), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
			const glm::mat4x4 matMVP = matProj * matView;
			//注意点.UseProgram()で対象プログラムが設定されていないとダメ.
			glUniformMatrix4fv(matMVPLoc, 1, GL_FALSE, &matMVP[0][0]);

		}

		glBindVertexArray(vao);
		glDrawElements(GL_TRIANGLES,ARRAY_SIZE(indices),GL_UNSIGNED_INT,0);


		window.SwapBuffers();
	}

	glDeleteProgram(shaderProgram);
	glDeleteVertexArrays(1, &vao);


	return 0;
}