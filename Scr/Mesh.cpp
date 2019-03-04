/**
* @file Mesh.cpp
*/
#include "Mesh.h"
#include <fbxsdk.h>
#include <iostream>

/**
* ���f���f�[�^�Ǘ��̂��߂̖��O���.
*/
namespace Mesh {
	//���_�f�[�^���i�[����\����
	struct Vertex
	{
		glm::vec3 position;		///<���W.
		glm::vec4 color;		///<�F.
		glm::vec2 texCoord;		///<�e�N�X�`�����W.
		glm::vec3 normal;		///<�@��
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
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
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
		glGenBuffers(1, &ibo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
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
	GLuint CreateVAO(GLuint vbo, GLuint ibo)
	{
		GLuint vao = 0;
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		SetVertexAttribPointer(0, Vertex, position);
		SetVertexAttribPointer(1, Vertex, color);
		SetVertexAttribPointer(2, Vertex, texCoord);
		SetVertexAttribPointer(3, Vertex, normal);
		glBindVertexArray(0);

		return vao;
	}


	//<--- �����ɃR�[�h��ǉ����Ă����܂� --->

}
