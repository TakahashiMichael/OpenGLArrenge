
#include "GLFWEW.h"
#include <iostream>
#include <glm/glm.hpp>
#include <vector>
struct Vertex
{
	glm::vec3 position; ///<���W
	glm::vec4 color;	///<�F�f�[�^
};
/// ���_�f�[�^.
const Vertex vertices[] = {
	{ {-0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f, 1.0f} },
	{ { 0.5f, -0.5f, 0.5f}, {0.0f, 1.0f, 0.0f, 1.0f} },
	{ { 0.0f,  0.5f, 0.5f}, {1.0f, 0.0f, 0.0f, 1.0f} },
};

/*
* VBO���쐬����
*
* @param GLsieiptr size		�o�^���钸�_�f�[�^�̃T�C�Y.
* @param const GLvoid* data �o�C�g�I�t�Z�b�g.
*
* @retval �쐬����VBO��ID
*/
GLuint CreateVBO(GLsizeiptr size,const GLvoid* data) {
	GLuint vbo = 0;
	glGenBuffers(1,&vbo);//(�쐬��,id�̃|�C���^)
	glBindBuffer(GL_ARRAY_BUFFER,vbo);//(���,id)
	glBufferData(GL_ARRAY_BUFFER,size,data,GL_STATIC_DRAW);//��l�����̓A�N�Z�X�̕p�x
	glBindBuffer(GL_ARRAY_BUFFER, 0);//���蓖�Ẳ���.

	return vbo;
}

/**
* ���_�A�g���r���[�g��ݒ肷��.
*
* @param index ���_�A�g���r���[�g�̃C���f�b�N�X.
* @param cls   ���_�f�[�^�^��.
* @param mbr   ���_�A�g���r���[�g�ɐݒ肷��cls�̃����o�ϐ���.
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
* vertex Array Object���쐬����.
*
* @param vbo VAO�Ɋ֘A������VBO
*
* @retval �쐬����VAO
*
*
*/
GLuint CreateVAO(GLuint vbo) {
	GLuint vao = 0;
	glGenVertexArrays(1,&vao);//(�쐬��,id�̃|�C���^)
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER,vbo);
	/*SetVertexAttribPointer(n,str,str::meb)
	* glEnableVertexAttribArray(n),
	* glVertexAttribPointer(n,�����o�T�C�Y,GL_FLOAT,GL_FALSE,�\���̃T�C�Y,dataoffset);
	*/
	SetVertexAttribPointer(0, Vertex, position);
	SetVertexAttribPointer(1, Vertex, color);
	glBindVertexArray(0);
	glDeleteBuffers(1, &vbo);
	return vao;

}

/// ���_�V�F�[�_.
static const char* vsCode =
"#version 410 \n"
"layout(location=0) in vec3 vPosition;"
"layout(location=1) in vec4 vColor;"
"layout(location=0) out vec4 outColor;"
"void main() {"
"  outColor = vColor;"
"  gl_Position = vec4(vPosition, 1.0);"
"}";

/// �t���O�����g�V�F�[�_.
static const char* fsCode =
"#version 410 \n"
"layout(location=0) in vec4 inColor;"
"out vec4 fragColor;"
"void main() {"
"  fragColor = inColor;"
"}";

/**
* �V�F�[�_�R�[�h���R���p�C������.
*
* @param type �V�F�[�_�̎��.
* @param string �V�F�[�_�R�[�h�ւ̃|�C���^.
*
* @return �쐬�����V�F�[�_�I�u�W�F�N�g.
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
				std::cerr << "ERROR: �V�F�[�_�̃R���p�C���Ɏ��s��n" << buf.data() << std::endl;
			}
		}
		glDeleteShader(shader);
		return 0;
	}
	return shader;
}


/**
* �v���O�����I�u�W�F�N�g���쐬����.
*
* @param vsCode ���_�V�F�[�_�R�[�h�ւ̃|�C���^.
* @param fsCode �t���O�����g�V�F�[�_�R�[�h�ւ̃|�C���^.
*
* @return �쐬�����v���O�����I�u�W�F�N�g.
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
				std::cerr << "ERROR: �V�F�[�_�̃����N�Ɏ��s��n" << buf.data() << std::endl;
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
	const GLuint vao = CreateVAO(vbo);
	const GLuint shaderProgram = CreateShaderProgram(vsCode, fsCode);
	if (!vbo || !vao || !shaderProgram) {
		return 1;
	}


	while (!window.ShouldClose()) {
		glClearColor(0.1f, 0.3f, 0.5f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(shaderProgram);
		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, sizeof(vertices) / sizeof(vertices[0]));


		window.SwapBuffers();
	}

	glDeleteProgram(shaderProgram);
	glDeleteVertexArrays(1, &vao);


	return 0;
}