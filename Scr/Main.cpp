
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
#include "UniformBuffer.h"
#include "SaveImage.h"

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

//�C���f�b�N�X�f�[�^.
const GLuint indices[] =
{
	0,1,2,
	2,3,0,
	4,5,6,
	7,8,9,
	10,11,12,
	12,13,10,
};

/// ���_�V�F�[�_�̃p�����[�^�^.
struct VertexData
{
	glm::mat4 matMVP;
};
const int maxLightCount = 4; ///< ���C�g�̐�.

/**
* ���C�g�f�[�^(�_����).
*/
struct PointLight
{
	glm::vec4 position; ///< ���W(���[���h���W�n).
	glm::vec4 color; ///< ���邳.
};

/**
* ���C�e�B���O�p�����[�^���V�F�[�_�ɓ]�����邽�߂̍\����.
*/
struct LightData
{
	glm::vec4 ambientColor; ///< ����.
	PointLight light[maxLightCount]; ///< ���C�g�̃��X�g.
};

/**
* �|�X�g�G�t�F�N�g�f�[�^���V�F�[�_�ɓ]�����邽�߂̍\����.
*/
struct PostEffectData
{
	glm::mat4x4 matColor; ///< �F�ϊ��s��.
};


/// �o�C���f�B���O�|�C���g.
enum BindingPoint
{
	BINDINGPOINT_VERTEXDATA, ///< ���_�V�F�[�_�p�p�����[�^�̃o�C���f�B���O�|�C���g.
	BINDINGPOINT_LIGHTDATA, ///< ���C�e�B���O�p�����[�^�p�̃o�C���f�B���O�|�C���g.
	BINDINGPOINT_POSTEFFECTDATA, ///< �|�X�g�G�t�F�N�g�p�����[�^�p�̃o�C���f�B���O�|�C���g.
};



//�����`��f�[�^.
struct RenderingPart
{
	GLvoid* offset;	///<�т傮���J�n�C���f�b�N�X�̃o�C�g�I�t�Z�b�g.
	GLsizei size;	///<�`�悷��C���f�b�N�X��.
};

/**
* RenderingPart���쐬����.
*
* @param offset �`��J�n�C���f�b�N�X�̃I�t�Z�b�g(�C���f�b�N�X�P��).
* @param size �`�悷��C���f�b�N�X��.
*
* @return �쐬���������`��I�u�W�F�N�g.
*/
constexpr RenderingPart MakeRenderingPart(GLsizei offset, GLsizei size) {
	return { reinterpret_cast<GLvoid*>(offset * sizeof(GLuint)), size };
}

/**
* �����`��f�[�^���X�g.
*/
static const RenderingPart renderingParts[] = {
  MakeRenderingPart(0, 12),
  MakeRenderingPart(12, 6),
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
	SetVertexAttribPointer(2,Vertex,texCoord);
	glBindVertexArray(0);
	glDeleteBuffers(1,&vbo);
	glDeleteBuffers(1,&ibo);
	return vao;
}





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
	const UniformBufferPtr uboVertex = UniformBuffer::Create(
		sizeof(VertexData), BINDINGPOINT_VERTEXDATA, "VertexData");
	const UniformBufferPtr uboLight = UniformBuffer::Create(
		sizeof(LightData),BINDINGPOINT_LIGHTDATA,"LightData");
	const UniformBufferPtr uboPostEffect = UniformBuffer::Create(
		sizeof(PostEffectData),BINDINGPOINT_POSTEFFECTDATA,"PostEffectData");
	const Shader::ProgramPtr progSimple =
		Shader::Program::Create(FILENAME_SHADER_VERT_SIMPLE,FILENAME_SHADER_FRAG_SIMPLE);
	const Shader::ProgramPtr progColorFilter =
		Shader::Program::Create(FILENAME_SHADER_VERT_COLORFILTER,FILENAME_SHADER_FRAG_COLORFILTER);
	const Shader::ProgramPtr progPosterFilter = 
		Shader::Program::Create(FILENAME_SHADER_VERT_POSTER,FILENAME_SHADER_FRAG_POSTER);
	if (!vbo || !ibo || !vao || !uboVertex || !uboLight || !uboPostEffect || !progSimple || !progColorFilter || !progPosterFilter) {
		return 1;
	}
	progSimple->UniformBlockBinding(*uboVertex);
	progSimple->UniformBlockBinding(*uboLight);
	progColorFilter->UniformBlockBinding(*uboPostEffect);


	TexturePtr tex = Texture::LoadFromFile(FILENAME_TGA_BUCK);
	if (!tex) {

		return 1;
		
	}





	const OffscreenBufferPtr offscreen = OffscreenBuffer::Create(800,600);

	//���ϐ�.
	int count = 0;
	const int countMax = 120;
	bool imageflag = false;

	//���ϐ�.
	float persAngle = 1.0f;
	bool flag = false;
	// ���C�����[�v.
	while (!window.ShouldClose()) {
		glBindFramebuffer(GL_FRAMEBUFFER,offscreen->GetFrameBuffer());
		window.Clear();
		glEnable(GL_DEPTH_TEST);
		progSimple->UseProgram();

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
		VertexData vertexData;
		vertexData.matMVP = matProj * matView;
		uboVertex->BufferSubdata(&vertexData);

		LightData lightData;
		lightData.ambientColor = glm::vec4(0.05f, 0.1f, 0.2f, 1);
		lightData.light[0].position = glm::vec4(1, 1, 1, 1);
		lightData.light[0].color = glm::vec4(2, 2, 2, 1);
		lightData.light[1].position = glm::vec4(-0.2f, 0, 0.6f, 1);
		lightData.light[1].color = glm::vec4(0.125f, 0.125f, 0.05f, 1);
		uboLight->BufferSubdata(&lightData);


		//
		progSimple->BindTexture(GL_TEXTURE0,GL_TEXTURE_2D,tex->Id());


		//�I�t�X�N���[���o�b�t�@�֕`�悷��.
		glBindVertexArray(vao);
		glDrawElements(GL_TRIANGLES, renderingParts[0].size,
			GL_UNSIGNED_INT, renderingParts[0].offset);

		if (!imageflag) {
			++count;
			if (count >= countMax) {
				SaveImage::saveImage(SaveImage::FILEFORMAT::FILEFORMAT_TGA,800,600);
				imageflag = true;
			}
		}


		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor(0.5f, 0.3f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//progSimple->BindTexture(GL_TEXTURE0,GL_TEXTURE_2D,offscreen->GetTexture());

		//vertexData = {};
		//vertexData.matMVP = glm::mat4x4(1);
		//uboVertex->BufferSubdata(&vertexData);

		//lightData = {};
		//lightData.ambientColor = glm::vec4(1);
		//uboLight->BufferSubdata(&lightData);

		progColorFilter->UseProgram();
		progColorFilter->BindTexture(GL_TEXTURE0,GL_TEXTURE_2D,offscreen->GetTexture());

		PostEffectData postEffect;
		//postEffect.matColor[0] = glm::vec4(0.393f, 0.349f, 0.272f, 0);
		//postEffect.matColor[1] = glm::vec4(0.769f, 0.686f, 0.534f, 0);
		//postEffect.matColor[2] = glm::vec4(0.189f, 0.168f, 0.131f, 0);
		//postEffect.matColor[3] = glm::vec4(0, 0, 0, 1);
		//postEffect.matColor[0] = glm::vec4(-1, 0, 0, 0);
		//postEffect.matColor[1] = glm::vec4( 0,-1, 0, 0);
		//postEffect.matColor[2] = glm::vec4( 0, 0,-1, 0);
		//postEffect.matColor[3] = glm::vec4( 1, 1, 1, 1);
		postEffect.matColor = glm::mat4x4(1);
		uboPostEffect->BufferSubdata(&postEffect);

		//progPosterFilter->UseProgram();
		//progPosterFilter->BindTexture(GL_TEXTURE0,GL_TEXTURE_2D,offscreen->GetTexture());

		//�I�t�X�N���[���o�b�t�@���g�p���ĉ�ʂ̕`��
		glDrawElements(
		GL_TRIANGLES, renderingParts[1].size,
		GL_UNSIGNED_INT, renderingParts[1].offset);

		//�o�b�t�@�[�̓���ւ�
		window.SwapBuffers();
	}
	//vao�̍폜
	glDeleteVertexArrays(1,&vao);

	return 0;
}