
#include <iostream>
#include "GLFWEW.h"
#include <glm/gtc/matrix_transform.hpp>
#include "Geometry.h"
#include "Debug.h"
#include <vector>

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

//�C���f�b�N�X�f�[�^.
const GLuint indices[] =
{
	0,1,2,
	2,3,0,
	4,5,6,
	7,8,9,
};

/*
* VBO���쐬����
* @param size ���_�f�[�^�̃T�C�Y.
* @param data ���_�f�[�^�ւ̃|�C���^.
*
* @retval �쐬����VBO
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
* Index Buffer Object���쐬����.
*
* @param size �C���f�b�N�X�f�[�^�̃T�C�Y.
* @param data �C���f�b�N�X�f�[�^�ւ̃|�C���^.
*
* @return �쐬����IBO.
*/
GLuint CreateIBO(GLsizeiptr size, const GLvoid* data) {
	GLuint ibo = 0;
	glGenBuffers(1,&ibo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,size,data,GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);
	return ibo;
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


/**
* Vertex Array Object���쐬����.
*
* @param vbo VAO�Ɋ֘A�t������VBO.
*
* @return �쐬����VAO.
*/
GLuint CreateVAO(GLuint vbo ,GLuint ibo)
{
	GLuint vao = 0;
	glGenVertexArrays(1,&vao);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER,vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,ibo);
	SetVertexAttribPointer(0, Vertex, position);
	SetVertexAttribPointer(1, Vertex, color);
	glBindVertexArray(0);
	glDeleteBuffers(1,&vbo);
	glDeleteBuffers(1,&ibo);
	return vao;
}

/**
* �V�F�[�_�R�[�h���R���p�C������.
*
* @param type �V�F�[�_�̎��.
* @param string �V�F�[�_�R�[�h�ւ̃|�C���^.
*
* @return �쐬�����V�F�[�_�I�u�W�F�N�g.
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
				std::cerr << "ERROR: �V�F�[�_�[�̃R���p�C���Ɏ��s\n" << buf.data() << std::endl;
			}
		}
		//���s�����ꍇ.�V�F�[�_���폜���ďI��.
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
				std::cerr << "ERROR:�V�F�[�_�̃����N�Ɏ��s\n" << buf.data() << std::endl;
			}
		}
		glDeleteProgram(program);
		return 0;
	}
	return program;
}

/// ���_�V�F�[�_.
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

/// �t���O�����g�V�F�[�_.
static const char* fsCode =
"#version 410 \n"
"layout(location=0) in vec4 inColor;"
"out vec4 fragColor;"
"void main() {"
"  fragColor = inColor;"
"}";




int main() {
	//�V���O���g���擾
	GLFWEW::WindowRef window = GLFWEW::Window::Instance();

	if (!window.Init(800,600,"opengl")) {
		std::cerr << "ERORR:GLFWEW�̏������Ɏ��s���܂���" << std::endl;
		return 1;
	}

	//�F�X�쐬
	const GLuint vbo = CreateVBO(sizeof(vertices),vertices);
	const GLuint ibo = CreateIBO(sizeof(indices),indices);
	const GLuint vao = CreateVAO(vbo,ibo);
	const GLuint shaderProgram = CreateShaderProgram(vsCode,fsCode);
	if (!vbo || !ibo || !vao || !shaderProgram) {
		return 1;
	}
	//�V�F�[�_����uniform�ϐ��̈ʒu���擾����.
	const GLint matMVPLoc = glGetUniformLocation(shaderProgram,"matMVP");
	if (!(matMVPLoc >= 0)) {
		std::cerr << "ERORR:uniform�̎擾�Ɏ��s���܂���.matMVP" << std::endl;
		glDeleteProgram(shaderProgram);
		glDeleteVertexArrays(1, &vao);
		return 1;
	}

	//�\���e�X�g.
	glm::mat4x4 a = glm::translate(glm::mat4x4(1),glm::vec3(3,5,7));
	Debug::print(a);
	glm::mat4x4 b = glm::rotate(glm::mat4x4(1), glm::radians(45.0f), glm::vec3(0, 1, 0));
	Debug::print(b);
	glm::mat4x4 s = glm::scale(glm::mat4x4(1), glm::vec3(2, 3, 5));
	Debug::print(s);

	//���ϐ�.
	float persAngle = 1.0f;
	bool flag = false;
	// ���C�����[�v.
	while (!window.ShouldClose()) {
		window.Clear();
		glEnable(GL_DEPTH_TEST);
		glUseProgram(shaderProgram);

		if (persAngle<=1.0f) {
			flag = true;
		}
		else if (persAngle > 80.0f) {
			flag = false;
		}
		if (flag) {
			persAngle += 0.1f;
		}
		else {
			persAngle -= 0.1f;

		}

		//���W�ϊ��s����쐬���ăV�F�[�_�ɓ]��.
		const glm::mat4x4 matProj =
			glm::perspective(glm::radians(persAngle),800.0f/600.0f,0.1f,100.0f);
		const glm::mat4x4 matView =
			glm::lookAt(glm::vec3(2,3,3),glm::vec3(0),glm::vec3(0,1,0));
		const glm::mat4x4 matMVP = matProj * matView;
		glUniformMatrix4fv(matMVPLoc,1,GL_FALSE,&matMVP[0][0]);

		glBindVertexArray(vao);
		glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(indices[0]),
			GL_UNSIGNED_INT, reinterpret_cast<const GLvoid*>(0));

		window.SwapBuffers();
	}
	glDeleteProgram(shaderProgram);
	glDeleteVertexArrays(1,&vao);

	return 0;
}