
#include "GLFWEW.h"
#include "Texture.h"
#include "FileNameList.h"
#include "Shader.h"
#include "OffscreenBuffer.h"
#include "UniformBuffer.h"
#include "Mesh.h"
#include "Entity.h"
#include <random>

#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#define ARRAY_SIZE(a) (sizeof(a)/sizeof((a)[0]))
struct Vertex
{
	glm::vec3 position; ///<���W
	glm::vec4 color;	///<�F�f�[�^
	glm::vec2 texCoord;	///<�e�N�X�`���f�[�^.
};
/// ���_�f�[�^.
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


{ {-1.0f,-1.0f, 0.5f}, {1.0f, 1.0f, 1.0f, 1.0f}, { 1.0f, 0.0f} },
{ { 1.0f,-1.0f, 0.5f}, {1.0f, 1.0f, 1.0f, 1.0f}, { 0.0f, 0.0f} },
{ { 1.0f, 1.0f, 0.5f}, {1.0f, 1.0f, 1.0f, 1.0f}, { 0.0f, 1.0f} },
{ {-1.0f, 1.0f, 0.5f}, {1.0f, 1.0f, 1.0f, 1.0f}, { 1.0f, 1.0f} },

};

/// �C���f�b�N�X�f�[�^.
const GLuint indices[] = {
  0, 1, 2, 2, 3, 0,
  4,5,6,7,8,9,
  10,11,12,12,13,10,
};

/// ���_�V�F�[�_�̃p�����[�^�^.
struct VertexData
{
	glm::mat4 matMVP;
	glm::mat4 matModel = glm::mat4(1);
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

/// �o�C���f�B���O�|�C���g.
enum BindingPoint
{
	BINDINGPOINT_VERTEXDATA, ///< ���_�V�F�[�_�p�p�����[�^�̃o�C���f�B���O�|�C���g.
	BINDINGPOINT_LIGHTDATA, ///< ���C�e�B���O�p�����[�^�p�̃o�C���f�B���O�|�C���g.
	BINDINGPOINT_VERTEXDATA_TEST, ///< ���_�V�F�[�_�p�p�����[�^�̃o�C���f�B���O�|�C���g.
	BINDINGPOINT_LIGHTDATA_TEST, ///< ���C�e�B���O�p�����[�^�p�̃o�C���f�B���O�|�C���g.
};

/**
* �����`��f�[�^.
*/
struct RenderingPart
{
	GLvoid* offset; ///< �`��J�n�C���f�b�N�X�̃o�C�g�I�t�Z�b�g.
	GLsizei size; ///< �`�悷��C���f�b�N�X��.
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
* Index Buffer Object���쐬����.
*
* @param size �C���f�b�N�X�f�[�^�̃T�C�Y.
* @param data �C���f�b�N�X�f�[�^�ւ̃|�C���^.
*
* @return �쐬����IBO.
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
GLuint CreateVAO(GLuint vbo,GLuint ibo) {
	GLuint vao = 0;
	glGenVertexArrays(1,&vao);//(�쐬��,id�̃|�C���^)
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER,vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,ibo);
	/*SetVertexAttribPointer(n,str,str::meb)
	* glEnableVertexAttribArray(n),
	* glVertexAttribPointer(n,�����o�T�C�Y,GL_FLOAT,GL_FALSE,�\���̃T�C�Y,dataoffset);
	*/
	SetVertexAttribPointer(0, Vertex, position);
	SetVertexAttribPointer(1, Vertex, color);
	SetVertexAttribPointer(2,Vertex,texCoord);
	glBindVertexArray(0);
	glDeleteBuffers(1, &vbo);
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


/**
* Uniform Block Object���쐬����.
*
* @param size Uniform Block�̃T�C�Y.
* @param data Uniform Block�ɓ]������f�[�^�ւ̃|�C���^.
*
* @return �쐬����UBO.
*/
GLuint CreateUBO(GLsizeiptr size, const GLvoid* data = nullptr)
{
	GLuint ubo;
	glGenBuffers(1, &ubo);
	glBindBuffer(GL_UNIFORM_BUFFER, ubo);
	glBufferData(GL_UNIFORM_BUFFER, size, data, GL_STREAM_DRAW);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);
	return ubo;
}



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
	const GLuint ibo = CreateIBO(sizeof(indices),indices);
	const GLuint vao = CreateVAO(vbo,ibo);
	const UniformBufferPtr uboVertex = UniformBuffer::Create(
		sizeof(VertexData), BINDINGPOINT_VERTEXDATA_TEST, "VertexData");
	const UniformBufferPtr uboLight = UniformBuffer::Create(
		sizeof(LightData), BINDINGPOINT_LIGHTDATA_TEST, "LightData");
	const UniformBufferPtr uboVertexScreen = UniformBuffer::Create(
		sizeof(VertexData), BINDINGPOINT_VERTEXDATA, "VertexData");
	const UniformBufferPtr uboLightScreen = UniformBuffer::Create(
		sizeof(LightData), BINDINGPOINT_LIGHTDATA, "LightData");


	const Shader::ProgramPtr progTutorial =
		Shader::Program::Create(FILENAME_VERT_TUTORIAL4,FILENAME_FRAG_TUTORIAL4);
	const Shader::ProgramPtr progScreen=
		Shader::Program::Create(FILENAME_VERT_TUTORIAL3, FILENAME_FRAG_TUTORIAL3);


	if (!vbo || !vao ||!ibo|| !uboVertex || !uboLight || !progTutorial || !progScreen) {

		return 1;
	}


	TexturePtr tex = Texture::LoadFromFile(FILENAME_TGA_BACK);
	TexturePtr tex2 = Texture::LoadFromFile(FILENAME_BMP_GEAR);
	TexturePtr texToroid = Texture::LoadFromFile(FILENAME_BMP_TOROID);
	if (!tex ||!tex2 || !texToroid) {
		std::cout << "�Ă������Ꮈ�s";

		return 1;
	
	}


	Mesh::BufferPtr meshBuffer = Mesh::Buffer::Create(50000, 50000);
	meshBuffer->LoadMeshFromFile(FILENAME_FBX_TOROID);

	Entity::BufferPtr entityBuffer = Entity::Buffer::Create(1024,
		sizeof(VertexData), BINDINGPOINT_VERTEXDATA_TEST, "VertexData");
	if (!entityBuffer) {
		return 1;
		
	}


	progTutorial->UniformBlockBinding(*uboVertex);
	progTutorial->UniformBlockBinding(*uboLight);
	progScreen->UniformBlockBinding(*uboVertexScreen);
	progScreen->UniformBlockBinding(*uboLightScreen);
	GLint maxubosize;
	glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS,&maxubosize);
	std::cout << "�ݒ�ł���UBO�̃o�C���f�B���O�|�C���g�̐���\��:" << (size_t)maxubosize << std::endl;

	const OffscreenBufferPtr offscreen = OffscreenBuffer::Create(window.GetSize().x,window.GetSize().y);


	while (!window.ShouldClose()) {
		glBindFramebuffer(GL_FRAMEBUFFER,offscreen->GetFramebuffer());
		glClearColor(0.1f, 0.3f, 0.5f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glEnable(GL_DEPTH_TEST);

		progScreen->UseProgram();
		// ���W�ϊ��s����쐬���ăV�F�[�_�ɓ]������.
		const glm::vec3 viewPos = glm::vec4(0, 10, -20, 1);
		const glm::mat4x4 matProj =
			glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 500.0f);
		const glm::mat4x4 matView =
			glm::lookAt(viewPos, glm::vec3(0, 0, 20), glm::vec3(0, 1, 0));
		const glm::mat4x4 matMVP = matProj * matView;
		//���ӓ_.UseProgram()�őΏۃv���O�������ݒ肳��Ă��Ȃ��ƃ_��.
		VertexData vertexData;
		vertexData.matMVP = matProj * matView;
		vertexData.color = glm::vec4(1, 1, 1, 1);
		uboVertexScreen->BufferSubData(&vertexData);

		LightData lightData;
		lightData.ambientColor = glm::vec4(0.05f, 0.1f, 0.2f, 1);
		lightData.light[0].position = glm::vec4(1, 1, 1, 1);
		lightData.light[0].color = glm::vec4(1, 1, 1, 1);
		lightData.light[1].position = glm::vec4(-0.2f, 0, 0.6f, 1);
		lightData.light[1].color = glm::vec4(0.125f, 0.125f, 0.05f, 1);
		uboLightScreen->BufferSubData(&lightData);


		progScreen->BindTexture(GL_TEXTURE0,GL_TEXTURE_2D,tex->Id());


		glBindVertexArray(vao);
		glDrawElements(GL_TRIANGLES,renderingParts[0].size,GL_UNSIGNED_INT,renderingParts[0].offset);
		progScreen->BindTexture(GL_TEXTURE0, GL_TEXTURE_2D, texToroid->Id());
		meshBuffer->BindVAO();
		meshBuffer->GetMesh("Toroid")->Draw(meshBuffer);

		Update(entityBuffer, meshBuffer, texToroid, progTutorial);

		entityBuffer->Update(1.0 / 60.0, matView, matProj);

		progTutorial->UseProgram();
		lightData.ambientColor = glm::vec4(0.05f, 0.1f, 0.2f, 1);
		lightData.light[0].position = glm::vec4(1, 1, 1, 1);
		lightData.light[0].color = glm::vec4(1, 1, 1, 1);
		lightData.light[1].position = glm::vec4(-0.2f, 0, 0.6f, 1);
		lightData.light[1].color = glm::vec4(0.125f, 0.125f, 0.05f, 1);
		uboLight->BufferSubData(&lightData);


		entityBuffer->Draw(meshBuffer);

		glBindVertexArray(vao);



		glBindFramebuffer(GL_FRAMEBUFFER,0);
		glClearColor(0.1f, 0.3f, 0.5f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		progScreen->UseProgram();
		progScreen->BindTexture(GL_TEXTURE0,GL_TEXTURE_2D,offscreen->GetTexture());
			
		vertexData = {};
		vertexData.matMVP = glm::mat4(1);

		uboVertexScreen->BufferSubData(&vertexData);

		lightData = {};
		lightData.ambientColor = glm::vec4(1);
		uboLightScreen->BufferSubData(&lightData);


		glDrawElements(
			GL_TRIANGLES,renderingParts[1].size,
			GL_UNSIGNED_INT,renderingParts[1].offset);

		window.SwapBuffers();
	}
	glDeleteVertexArrays(1, &vao);


	return 0;
}