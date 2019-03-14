
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
#include "Function.h"
#include "Mesh.h"
#include "Entity.h"
#include <random>

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
	glm::mat4 matModel;
	glm::mat3x4 matNormal;
	glm::vec4 color;

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

/**
* �G�̉~�Ղ̏�Ԃ��X�V����.
*/
struct UpdateToroid
{
	explicit UpdateToroid(const Entity::BufferPtr& buffer) : entityBuffer(buffer) {}

	void operator()(Entity::Entity& entity, void* ubo, double delta,
		const glm::mat4& matView, const glm::mat4& matProj)
	{
		// �͈͊O�ɏo����폜����.
		const glm::vec3 pos = entity.Position();
		if (std::abs(pos.x) > 40.0f || std::abs(pos.z) > 40.0f) {
			entityBuffer->RemoveEntity(&entity);
			return;
		}

		// �~�Ղ���]������.
		float rot = glm::angle(entity.Rotation());
		rot += glm::radians(15.0f) * static_cast<float>(delta);
		if (rot > glm::pi<float>() * 2.0f) {
			rot -= glm::pi<float>() * 2.0f;
		}
		entity.Rotation(glm::angleAxis(rot, glm::vec3(0, 1, 0)));

		// ���_�V�F�[�_�[�̃p�����[�^��UBO�ɃR�s�[����.
		VertexData data;
		data.matModel = entity.CalcModelMatrix();
		data.matNormal = glm::mat4_cast(entity.Rotation());
		data.matMVP = matProj * matView * data.matModel;
		data.color = entity.Color();
		memcpy(ubo, &data, sizeof(VertexData));
	}

	Entity::BufferPtr entityBuffer;
};


/**
* �Q�[���̏�Ԃ��X�V����.
*
* @param entityBuffer �G�G���e�B�e�B�ǉ���̃G���e�B�e�B�o�b�t�@.
* @param meshBuffer   �G�G���e�B�e�B�̃��b�V�����Ǘ����Ă��郁�b�V���o�b�t�@.
* @param tex          �G�G���e�B�e�B�p�̃e�N�X�`��.
* @param prog         �G�G���e�B�e�B�p�̃V�F�[�_�[�v���O����.
*/
void Update(Entity::BufferPtr entityBuffer, Mesh::BufferPtr meshBuffer,
	TexturePtr tex, Shader::ProgramPtr prog)
{
	static std::mt19937 rand(std::random_device{}());
	static double interval = 0;

	interval -= 1.0 / 60.0;
	if (interval <= 0) {
		const std::uniform_real_distribution<float> posXRange(-15, 15);
		const glm::vec3 pos(posXRange(rand), 0, 40);
		const Mesh::MeshPtr& mesh = meshBuffer->GetMesh("Toroid");
		if (Entity::Entity* p = entityBuffer->AddEntity(pos, mesh, tex, prog,
			UpdateToroid(entityBuffer))) {
			p->Velocity(glm::vec3(pos.x < 0 ? 0.1f : -0.1f, 0, -1.0f));
		}
		const std::uniform_real_distribution<double> intervalRange(3.0, 6.0);
		interval = intervalRange(rand);
	}
}


int main() {


	//�V���O���g���擾
	GLFWEW::WindowRef window = GLFWEW::Window::Instance();

	if (!window.Init(800.0f,600.0f,"opengl")) {
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


	TexturePtr tex = Texture::LoadFromFile(FILENAME_TGA_BUCK);
	TexturePtr texToroid = Texture::LoadFromFile(FILENAME_BMP_TOROID);
	if (!tex || !texToroid) {

		return 1;
		
	}
	Mesh::BufferPtr meshBuffer = Mesh::Buffer::Create(50000,50000);
	meshBuffer->LoadMeshFromFile(FILENAME_FBX_TOROID);

	Entity::BufferPtr entityBuffer = Entity::Buffer::Create(1024,
		sizeof(VertexData), 0, "VertexData");
	if (!entityBuffer) {
		return 1;
	}
	
	//�V�F�[�_��ubo��o�^����.
	progSimple->UniformBlockBinding(*entityBuffer->UniformBuffer());
	progSimple->UniformBlockBinding(*uboVertex);
	progSimple->UniformBlockBinding(*uboLight);
	progColorFilter->UniformBlockBinding(*uboPostEffect);



	const OffscreenBufferPtr offscreen = OffscreenBuffer::Create(window.Width(), window.Height());

	//���ϐ�.
	int count = 0;
	const int countMax = 120;
	bool imageflag = false;

	//���ϐ�.
	// ���C�����[�v.
	while (!window.ShouldClose()) {
		Update(entityBuffer, meshBuffer, texToroid, progSimple);
		glBindFramebuffer(GL_FRAMEBUFFER,offscreen->GetFrameBuffer());
		window.Clear();
		glEnable(GL_DEPTH_TEST);
		progSimple->UseProgram();


		//���W�ϊ��s����쐬���ăV�F�[�_�ɓ]��.
		const glm::vec3 viewPos = glm::vec4(0, 20, -20, 1);
		const glm::mat4x4 matProj =
			glm::perspective(glm::radians(45.0f),800.0f/600.0f,0.1f,1000.0f);
		const glm::mat4x4 matView =
			glm::lookAt(glm::vec3(viewPos),glm::vec3(0),glm::vec3(0,1,0));
		VertexData vertexData;
		vertexData.matMVP = matProj * matView;
		uboVertex->BufferSubdata(&vertexData);

		LightData lightData;
		lightData.ambientColor = glm::vec4(0.05f, 0.1f, 0.2f, 1);
		lightData.light[0].color = glm::vec4(12000, 12000, 12000, 1);
		lightData.light[0].position = glm::vec4(40, 100, 20, 1);

		uboLight->BufferSubdata(&lightData);


		//
		progSimple->BindTexture(GL_TEXTURE0,GL_TEXTURE_2D,tex->Id());
		meshBuffer->BindVAO();
		meshBuffer->GetMesh("Toroid")->Draw(meshBuffer);


		//�I�t�X�N���[���o�b�t�@�֕`�悷��.
		glBindVertexArray(vao);
		glDrawElements(GL_TRIANGLES, renderingParts[0].size,
			GL_UNSIGNED_INT, renderingParts[0].offset);
		progSimple->BindTexture(GL_TEXTURE0,GL_TEXTURE_2D,texToroid->Id());


		//entityBuffer->Update(1.0 / 60.0, matView, matProj);
		//entityBuffer->Draw(meshBuffer);

		glBindVertexArray(vao);

		if (!imageflag) {
			++count;
			if (count >= countMax) {
				SaveImage::saveImage(SaveImage::FILEFORMAT::FILEFORMAT_TGA,window.Width(),window.Height());
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