/**
* @file Mesh.cpp
*/
#include "Mesh.h"
#include <fbxsdk.h>
#include <iostream>

/**
* モデルデータ管理のための名前空間.
*/
namespace Mesh {
	//頂点データを格納する構造体
	struct Vertex
	{
		glm::vec3 position;		///<座標.
		glm::vec4 color;		///<色.
		glm::vec2 texCoord;		///<テクスチャ座標.
		glm::vec3 normal;		///<法線
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
		glGenBuffers(1, &vbo);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
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
	GLuint CreateIBO(GLsizeiptr size, const GLvoid* data) {
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


	/**
	* Vertex Array Objectを作成する.
	*
	* @param vbo VAOに関連付けられるVBO.
	*
	* @return 作成したVAO.
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


	//<--- ここにコードを追加していきます --->

}
