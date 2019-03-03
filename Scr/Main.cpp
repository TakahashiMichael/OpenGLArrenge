
#include <iostream>
#include "GLFWEW.h"
#include <glm/gtc/matrix_transform.hpp>
#include "Geometry.h"
#include "Texture.h"
#include "Debug.h"
#include "LoadFileName.h"
#include <vector>
#include "Shader.h"
#include "OffscreenBuffer.h"


const Vertex vertices[] = {
{ {-0.5f, -0.3f, 0.5f}, {0.0f, 0.0f, 1.0f, 1.0f}, { 0.0f, 0.0f} },
{ { 0.3f, -0.3f, 0.5f}, {0.0f, 1.0f, 0.0f, 1.0f}, { 1.0f, 0.0f} },
{ { 0.3f,  0.5f, 0.5f}, {0.0f, 0.0f, 1.0f, 1.0f}, { 1.0f, 1.0f} },
{ {-0.5f,  0.5f, 0.5f}, {1.0f, 0.0f, 0.0f, 1.0f}, { 0.0f, 1.0f} },

{ {-0.3f,  0.3f, 0.1f}, {0.0f, 0.0f, 1.0f, 1.0f}, { 0.0f, 1.0f} },
{ {-0.3f, -0.5f, 0.1f}, {0.0f, 1.0f, 1.0f, 1.0f}, { 0.0f, 0.0f} },
{ { 0.5f, -0.5f, 0.1f}, {0.0f, 0.0f, 1.0f, 1.0f}, { 1.0f, 0.0f} },
{ { 0.5f, -0.5f, 0.1f}, {1.0f, 0.0f, 0.0f, 1.0f}, { 1.0f, 0.0f} },
{ { 0.5f,  0.3f, 0.1f}, {1.0f, 1.0f, 0.0f, 1.0f}, { 1.0f, 1.0f} },
{ {-0.3f,  0.3f, 0.1f}, {1.0f, 0.0f, 0.0f, 1.0f}, { 0.0f, 1.0f} },

{ {-1.0f,-1.0f, 0.5f}, {1.0f, 1.0f, 1.0f, 1.0f}, { 0.0f, 1.0f} },
{ { 1.0f,-1.0f, 0.5f}, {1.0f, 1.0f, 1.0f, 1.0f}, { 1.0f, 1.0f} },
{ { 1.0f, 1.0f, 0.5f}, {1.0f, 1.0f, 1.0f, 1.0f}, { 1.0f, 0.0f} },
{ {-1.0f, 1.0f, 0.5f}, {1.0f, 1.0f, 1.0f, 1.0f}, { 0.0f, 0.0f} },

};

//インデックスデータ.
const GLuint indices[] =
{
	0,1,2,
	2,3,0,
	4,5,6,
	7,8,9,
	10,11,12,
	12,13,10,
};

/// 頂点シェーダのパラメータ型.
struct VertexData
{
	glm::mat4 matMVP;
	glm::vec4 lightPosition;
	glm::vec4 lightColor;
	glm::vec4 ambientColor;
};


//部分描画データ.
struct RenderingPart
{
	GLvoid* offset;	///<びょぐあ開始インデックスのバイトオフセット.
	GLsizei size;	///<描画するインデックス数.
};

/**
* RenderingPartを作成する.
*
* @param offset 描画開始インデックスのオフセット(インデックス単位).
* @param size 描画するインデックス数.
*
* @return 作成した部分描画オブジェクト.
*/
constexpr RenderingPart MakeRenderingPart(GLsizei offset, GLsizei size) {
	return { reinterpret_cast<GLvoid*>(offset * sizeof(GLuint)), size };
}

/**
* 部分描画データリスト.
*/
static const RenderingPart renderingParts[] = {
  MakeRenderingPart(0, 12),
  MakeRenderingPart(12, 6),
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
* Index Buffer Objectを作成する.
*
* @param size インデックスデータのサイズ.
* @param data インデックスデータへのポインタ.
*
* @return 作成したIBO.
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
GLuint CreateVAO(GLuint vbo ,GLuint ibo)
{
	GLuint vao = 0;
	glGenVertexArrays(1,&vao);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER,vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,ibo);
	SetVertexAttribPointer(0, Vertex, position);
	SetVertexAttribPointer(1, Vertex, color);
	SetVertexAttribPointer(2,Vertex,texCoord);
	glBindVertexArray(0);
	glDeleteBuffers(1,&vbo);
	glDeleteBuffers(1,&ibo);
	return vao;
}

/**
* Uniform Block Objectを作成する.
*
* @param size Uniform Blockのサイズ.
* @param data Uniform Blockに転送するデータへのポインタ.
*
* @return 作成したUBO.
*/
GLuint CreateUBO(GLsizeiptr size,const GLvoid* data =nullptr )
{
	GLuint ubo;
	glGenBuffers(1,&ubo);
	glBindBuffer(GL_UNIFORM_BUFFER,ubo);
	glBufferData(GL_UNIFORM_BUFFER,size,data,GL_STREAM_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER,0);
	return ubo;
}




int main() {
	//シングルトン取得
	GLFWEW::WindowRef window = GLFWEW::Window::Instance();

	if (!window.Init(800,600,"opengl")) {
		std::cerr << "ERORR:GLFWEWの初期化に失敗しました" << std::endl;
		return 1;
	}

	//色々作成
	const GLuint vbo = CreateVBO(sizeof(vertices),vertices);
	const GLuint ibo = CreateIBO(sizeof(indices),indices);
	const GLuint vao = CreateVAO(vbo,ibo);
	const GLuint ubo = CreateUBO(sizeof(VertexData));
	const GLuint shaderProgram = Shader::CreateProgramFromFile(FILENAME_SHADER_VERT_SIMPLE,FILENAME_SHADER_FRAG_SIMPLE);
	if (!vbo || !ibo || !vao || !ubo || !shaderProgram) {
		return 1;
	}
	//locの取得のところがこれ.
	const GLuint uboIndex = glGetUniformBlockIndex(shaderProgram, "VertexData");
	if (uboIndex == GL_INVALID_INDEX) {
		return 1;
		
	}
	glUniformBlockBinding(shaderProgram, uboIndex, 0);


	TexturePtr tex = Texture::LoadFromFile(FILENAME_BMP_GEAR);
	if (!tex) {
		return 1;
		
	}
	//テクスチャのサンプラーの位置を取得する.
	const GLint colorSamplerLoc = glGetUniformLocation(shaderProgram,"texColor");





	const OffscreenBufferPtr offscreen = OffscreenBuffer::Create(800,600);


	//仮変数.
	float persAngle = 1.0f;
	bool flag = false;
	// メインループ.
	while (!window.ShouldClose()) {
		glBindFramebuffer(GL_FRAMEBUFFER,offscreen->GetFrameBuffer());
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

		//座標変換行列を作成してシェーダに転送.
		const glm::mat4x4 matProj =
			glm::perspective(glm::radians(persAngle),800.0f/600.0f,0.1f,100.0f);
		const glm::mat4x4 matView =
			glm::lookAt(glm::vec3(2,3,3),glm::vec3(0),glm::vec3(0,1,0));
		VertexData vertexData;
		vertexData.matMVP = matProj * matView;
		vertexData.lightPosition = glm::vec4(1, 1, 1, 1);
		vertexData.lightColor = glm::vec4(2, 2, 2, 1);
		vertexData.ambientColor = glm::vec4(0.05f, 0.1f, 0.2f, 1);
		glBindBufferBase(GL_UNIFORM_BUFFER, 0, ubo);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(VertexData), &vertexData);


		//サンプラーにテクスチャデータを転送.
		if (colorSamplerLoc >= 0) {
			glUniform1i(colorSamplerLoc, 0);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, tex->Id());
			//補足,使わないテクスチャはいちいち割り当てを解除した方がいいそう
		}


		glBindVertexArray(vao);
		glDrawElements(GL_TRIANGLES, renderingParts[0].size,
			GL_UNSIGNED_INT, renderingParts[0].offset);


		//デフォルトのフレームバッファーを設定する/
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor(0.5f, 0.3f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		if (colorSamplerLoc >= 0) {
			glBindTexture(GL_TEXTURE_2D, offscreen->GetTexture());
			
		}

		vertexData = {};
		vertexData.matMVP = glm::mat4x4(1);
		vertexData.ambientColor = glm::vec4(1, 1, 1, 1);
		glBufferSubData(GL_UNIFORM_BUFFER,0,sizeof(VertexData),&vertexData);


		glDrawElements(
		GL_TRIANGLES, renderingParts[1].size,
		GL_UNSIGNED_INT, renderingParts[1].offset);


		window.SwapBuffers();
	}
	glDeleteBuffers(1,&ubo);
	glDeleteProgram(shaderProgram);
	glDeleteVertexArrays(1,&vao);

	return 0;
}