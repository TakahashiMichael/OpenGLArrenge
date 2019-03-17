/**
* @file GameEngine.cpp
*/
#include "GameEngine.h"
#include "GLFWEW.h"
#include "FileNameList.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

namespace /* unnamed */ {

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


	{ {-1.0f,-1.0f, 0.5f}, {1.0f, 1.0f, 1.0f, 1.0f}, { 0.0f, 0.0f} },
	{ { 1.0f,-1.0f, 0.5f}, {1.0f, 1.0f, 1.0f, 1.0f}, { 1.0f, 0.0f} },
	{ { 1.0f, 1.0f, 0.5f}, {1.0f, 1.0f, 1.0f, 1.0f}, { 1.0f, 1.0f} },
	{ {-1.0f, 1.0f, 0.5f}, {1.0f, 1.0f, 1.0f, 1.0f}, { 0.0f, 1.0f} },

	};

	/// �C���f�b�N�X�f�[�^.
	const GLuint indices[] = {
	  0, 1, 2, 2, 3, 0,
	  4,5,6,7,8,9,
	  10,11,12,12,13,10,
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
	GLuint CreateVBO(GLsizeiptr size, const GLvoid* data) {
		GLuint vbo = 0;
		glGenBuffers(1, &vbo);//(�쐬��,id�̃|�C���^)
		glBindBuffer(GL_ARRAY_BUFFER, vbo);//(���,id)
		glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);//��l�����̓A�N�Z�X�̕p�x
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
	GLuint CreateVAO(GLuint vbo, GLuint ibo) {
		GLuint vao = 0;
		glGenVertexArrays(1, &vao);//(�쐬��,id�̃|�C���^)
		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
		/*SetVertexAttribPointer(n,str,str::meb)
		* glEnableVertexAttribArray(n),
		* glVertexAttribPointer(n,�����o�T�C�Y,GL_FLOAT,GL_FALSE,�\���̃T�C�Y,dataoffset);
		*/
		SetVertexAttribPointer(0, Vertex, position);
		SetVertexAttribPointer(1, Vertex, color);
		SetVertexAttribPointer(2, Vertex, texCoord);
		glBindVertexArray(0);
		glDeleteBuffers(1, &vbo);
		glDeleteBuffers(1, &ibo);
		return vao;

	}

} // unnamed namespace

/**
* �Q�[���G���W���̃C���X�^���X���擾����.
*
* @return �Q�[���G���W���̃C���X�^���X.
*/
GameEngine& GameEngine::Instance()
{
	static GameEngine instance;
	return instance;
}

/**
* �Q�[���G���W��������������.
*
* @param w     �E�B���h�E�̕`��͈͂̕�(�s�N�Z��).
* @param h     �E�B���h�E�̕`��͈͂̍���(�s�N�Z��).
* @param title �E�B���h�E�^�C�g��(UTF-8��0�I�[������).
*
* @retval true  ����������.
* @retval false ���������s.
*
* Run�֐����Ăяo���O�ɁA��x�����Ăяo���Ă����K�v������.
* ��x�������ɐ�������ƁA�Ȍ�̌Ăяo���ł͂Ȃɂ�������true��Ԃ�.
*/
bool GameEngine::Init(int w,int h,const char* title)
{
	if (isInitialized) {
		return true;
	}
	if (!GLFWEW::Window::Instance().Init(w, h, title)) {
		return false;
	}

	//�C���ӏ�
	width = w;
	height = h;
	vbo = CreateVBO(sizeof(vertices), vertices);
	ibo = CreateIBO(sizeof(indices), indices);
	vao = CreateVAO(vbo, ibo);
	uboVertex = UniformBuffer::Create(
		sizeof(InterfaceBlock::VertexData), InterfaceBlock::BINDINGPOINT_VERTEXDATA_TEST, "VertexData");
	uboLight = UniformBuffer::Create(
		sizeof(InterfaceBlock::LightData), InterfaceBlock::BINDINGPOINT_LIGHTDATA_TEST, "LightData");
	uboVertexScreen = UniformBuffer::Create(
		sizeof(InterfaceBlock::VertexData), InterfaceBlock::BINDINGPOINT_VERTEXDATA, "VertexData");
	uboLightScreen = UniformBuffer::Create(
		sizeof(InterfaceBlock::LightData), InterfaceBlock::BINDINGPOINT_LIGHTDATA, "LightData");


	progTutorial =
		Shader::Program::Create(FILENAME_VERT_TUTORIAL4, FILENAME_FRAG_TUTORIAL4);
	progScreen =
		Shader::Program::Create(FILENAME_VERT_TUTORIAL3, FILENAME_FRAG_TUTORIAL3);


	if (!vbo || !vao || !ibo || !uboVertex || !uboLight || !progTutorial || !progScreen) {
		std::cerr << "ERROR: GameEngine�̏������Ɏ��s" << std::endl;
		return false;
	}


	//TexturePtr tex = Texture::LoadFromFile(FILENAME_TGA_BACK);
	//TexturePtr tex2 = Texture::LoadFromFile(FILENAME_BMP_GEAR);
	//TexturePtr texToroid = Texture::LoadFromFile(FILENAME_BMP_TOROID);
	//if (!tex || !tex2 || !texToroid) {
	//	std::cout << "�Ă������Ꮈ�s";

	//	return 1;

	//}


	meshBuffer = Mesh::Buffer::Create(10*1024,30*1024);
	meshBuffer->LoadMeshFromFile(FILENAME_FBX_TOROID);
	if (!meshBuffer) {
		std::cerr << "ERROR: GameEngine�̏������Ɏ��s" << std::endl;
		return false;
	}



	 entityBuffer = Entity::Buffer::Create(1024,
		sizeof(InterfaceBlock::VertexData), InterfaceBlock::BINDINGPOINT_VERTEXDATA_TEST, "VertexData");
	 if (!entityBuffer) {
		 std::cerr << "ERROR: GameEngine�̏������Ɏ��s" << std::endl;
		 return false;
	 }


	progTutorial->UniformBlockBinding(*entityBuffer->UniformBuffer());
	progTutorial->UniformBlockBinding(*uboLight);
	progScreen->UniformBlockBinding(*uboVertexScreen);
	progScreen->UniformBlockBinding(*uboLightScreen);
	GLint maxubosize;
	glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, &maxubosize);
	std::cout << "�ݒ�ł���UBO�̃o�C���f�B���O�|�C���g�̐���\��:" << (size_t)maxubosize << std::endl;

	offscreen = OffscreenBuffer::Create(width, height);
	rand.seed(std::random_device()());

	isInitialized = true;
	return true;
}

/*
* �Q�[�������s����.
*/
void GameEngine::Run()
{
	GLFWEW::Window& window = GLFWEW::Window::Instance();
	const double delta = 1.0 / 60.0;
	while (!window.ShouldClose()) {
		Update(delta);
		Render();
		window.SwapBuffers();
	}
}

/*
* ��ԍX�V�֐���ݒ肷��.
*
* @param func �ݒ肷��X�V�֐�.
*
*/
void GameEngine::UpdateFunc(const UpdateFuncType& func)
{
	updateFunc = func;
}

/**
* �e�N�X�`����ǂݍ���.
*
* @param filename �e�N�X�`���t�@�C����.
*
* @retval true  �ǂݍ��ݐ���.
* @retval false �ǂݍ��ݎ��s.
*/
bool GameEngine::LoadTextureFromFile(const char* filename)
{
	//���ɓǂݍ���ł���Ȃ�Ȃɂ����Ȃ�
	if (GetTexture(filename)) {
		return true;
	}
	TexturePtr texture = Texture::LoadFromFile(filename);
	if (!texture) {
		return false;
	}
	textureBuffer.insert(std::make_pair(std::string(filename),texture));
	return true;
}
/**
* ���b�V����ǂݍ���.
*
* @param filename ���b�V���t�@�C����.
*
* @retval true  �ǂݍ��ݐ���.
* @retval false �ǂݍ��ݎ��s.
*/
bool GameEngine::LoadMeshFromFile(const char* filename)
{
	return meshBuffer->LoadMeshFromFile(filename);
}

/**
* �G���e�B�e�B��ǉ�����.
*
* @param pos      �G���e�B�e�B�̍��W.
* @param meshName �G���e�B�e�B�̕\���Ɏg�p���郁�b�V����.
* @param texName  �G���e�B�e�B�̕\���Ɏg���e�N�X�`���t�@�C����.
* @param func     �G���e�B�e�B�̏�Ԃ��X�V����֐�(�܂��͊֐��I�u�W�F�N�g).
*
* @return �ǉ������G���e�B�e�B�ւ̃|�C���^.
*         ����ȏ�G���e�B�e�B��ǉ��ł��Ȃ��ꍇ��nullptr���Ԃ����.
*         ��]��g�嗦�͂��̃|�C���^�o�R�Őݒ肷��.
*         �Ȃ��A���̃|�C���^���A�v���P�[�V�������ŕێ�����K�v�͂Ȃ�.
*/
Entity::Entity* GameEngine::AddEntity(const glm::vec3& pos ,const char* meshName,
	const char* texName,Entity::Entity::UpdateFuncType func)
{
	const Mesh::MeshPtr& mesh = meshBuffer->GetMesh(meshName);
	const TexturePtr& tex = textureBuffer.find(texName)->second;
	return entityBuffer->AddEntity(pos,mesh,tex,progTutorial,func);
}

/**
*�@�G���e�B�e�B���폜����.
*
* @param �폜����G���e�B�e�B�̃|�C���^.
*/
void GameEngine::RemoveEntity(Entity::Entity* e)
{
	entityBuffer->RemoveEntity(e);
}
/**
* ���C�g��ݒ肷��.
*
* @param indes  �ݒ肷�郉�C�g�̃C���f�b�N�X.
* @param light  ���C�g�f�[�^.
*/
void GameEngine::Light(int index, const InterfaceBlock::PointLight& light)
{
	if (index < 0 || index >= InterfaceBlock::maxLightCount) {
		std::cerr << "WARNING: '" << index << "'�͕s���ȃ��C�g�C���f�b�N�X�ł�" << std::endl;
		return;
	}
	lightData.light[index] = light;
}

/**
* ���C�g���擾����.
*
* @param index �擾���郉�C�g�̃C���f�b�N�X.
*
* @return ���C�g�f�[�^.
*/
const InterfaceBlock::PointLight& GameEngine::Light(int index) const
{
	if (index < 0 || index >= InterfaceBlock::maxLightCount) {
		std::cerr << "WARNING: '" << index << "'�͕s���ȃ��C�g�C���f�b�N�X�ł�" << std::endl;
		static const InterfaceBlock::PointLight dummy;
		return dummy;
	}
	return lightData.light[index];
}

/**
* ������ݒ肷��.
*
* @param color �����̖��邳.
*/
void GameEngine::AmbientLight(const glm::vec4& color)
{
	lightData.ambientColor = color;
}

/**
* �������擾����.
*
* @return �����̖��邳.
*/
const glm::vec4& GameEngine::AmbientLight() const
{
	return lightData.ambientColor;
}

/**
* ���_�̈ʒu�Ǝp����ݒ肷��.
*
* @param cam �ݒ肷��J�����f�[�^.
*/
void GameEngine::Camera(const CameraData& cam)
{
	camera = cam;
}

/**
* ���_�̈ʒu�Ǝp�����擾����.
*
* @return �J�����f�[�^.
*/
const GameEngine::CameraData& GameEngine::Camera() const
{
	return camera;
}

/**
* �����I�u�W�F�N�g���擾����.
*
* @return �����I�u�W�F�N�g.
*/
std::mt19937& GameEngine::Rand()
{
	return rand;
}


/**
* �e�N�X�`�����擾����.
*
* @param filename �e�N�X�`���t�@�C����.
*
* @return filename�ɑΉ�����e�N�X�`���ւ̃|�C���^.
*         �Ή�����e�N�X�`����������Ȃ��ꍇ��nullptr��Ԃ�.
*/
const TexturePtr& GameEngine::GetTexture(const char* filename) const
{
	const auto itr = textureBuffer.find(filename);
	if (itr != textureBuffer.end()) {
		return itr->second;
	}
	static const TexturePtr dummy;
	return dummy;
}

/*
* �R�}���h���͂��擾����/
*/
bool GameEngine::GetComand(GLFWEW::Window::COMMANDLIST c)const {
	GLFWEW::Window& window = GLFWEW::Window::Instance();
	return window.GetCommand(c);
}


/*
* ��ԍX�V�֐����擾����.
*
* @return �ݒ肳��Ă���X�V�֐�.
*/
const GameEngine::UpdateFuncType& GameEngine::UpdateFunc()const
{
	return updateFunc;
}


/*
* �f�X�g���N�^.
*
*/
GameEngine::~GameEngine()
{
	updateFunc = nullptr;
	if (vao) {
		glDeleteVertexArrays(1, &vao);
	}
	if (ibo) {
		glDeleteBuffers(1, &ibo);
	}
	if (vbo) {
		glDeleteBuffers(1, &vbo);
	}
}

/*
* �Q�[���G���W�����X�V����
*
* @param delta �o�ߎ���deltatime
*/
void GameEngine::Update(double delta)
{
	GLFWEW::Window::Instance().Update();
	//���������͒��g�����邩�ǂ���.�����L�͕��ʂɎ��s/
	if (updateFunc) {
		updateFunc(delta);
	}

	const glm::mat4x4 matProj =
		glm::perspective(glm::radians(45.0f),static_cast<float>(width) /static_cast<float>(height), 0.1f, 500.0f);
	const glm::mat4x4 matView =
		glm::lookAt(camera.position, camera.target, camera.up);
	entityBuffer->Update(delta, matView,matProj);
}

/*
* �Q�[���̏�Ԃ�`�悷��.
*/
void GameEngine::Render() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, offscreen->GetFramebuffer());
	glEnable(GL_DEPTH_TEST);//�[�x
	glEnable(GL_CULL_FACE);//�J�����O
	glEnable(GL_BLEND);//�A���t�@�u�����f�B���O,
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);//�������@.
	glViewport(0, 0, width, height);		//��
	glScissor(0, 0, width, height);			//��

	glClearColor(0.1f, 0.3f, 0.5f, 1.0f);
	glClearDepth(1);					//��
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	uboLight->BufferSubData(&lightData);
	entityBuffer->Draw(meshBuffer);







	//progScreen->UseProgram();
	//// ���W�ϊ��s����쐬���ăV�F�[�_�ɓ]������.
	//const glm::vec3 viewPos = glm::vec4(0, 10, -20, 1);
	//const glm::mat4x4 matProj =
	//	glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 500.0f);
	//const glm::mat4x4 matView =
	//	glm::lookAt(viewPos, glm::vec3(0, 0, 20), glm::vec3(0, 1, 0));
	//const glm::mat4x4 matMVP = matProj * matView;
	////���ӓ_.UseProgram()�őΏۃv���O�������ݒ肳��Ă��Ȃ��ƃ_��.
	//VertexData vertexData;
	//vertexData.matMVP = matProj * matView;
	//vertexData.color = glm::vec4(1, 1, 1, 1);
	//uboVertexScreen->BufferSubData(&vertexData);

	//LightData lightData;
	//lightData.ambientColor = glm::vec4(0.05f, 0.1f, 0.2f, 1);
	//lightData.light[0].position = glm::vec4(1, 1, 1, 1);
	//lightData.light[0].color = glm::vec4(1, 1, 1, 1);
	//lightData.light[1].position = glm::vec4(-0.2f, 0, 0.6f, 1);
	//lightData.light[1].color = glm::vec4(0.125f, 0.125f, 0.05f, 1);
	//uboLightScreen->BufferSubData(&lightData);


	//progScreen->BindTexture(GL_TEXTURE0, GL_TEXTURE_2D, tex->Id());


	//glBindVertexArray(vao);
	//glDrawElements(GL_TRIANGLES, renderingParts[0].size, GL_UNSIGNED_INT, renderingParts[0].offset);
	//progScreen->BindTexture(GL_TEXTURE0, GL_TEXTURE_2D, texToroid->Id());
	//meshBuffer->BindVAO();
	//meshBuffer->GetMesh("Toroid")->Draw(meshBuffer);

	//Update(entityBuffer, meshBuffer, texToroid, progTutorial);

	//entityBuffer->Update(1.0 / 60.0, matView, matProj);

	//progTutorial->UseProgram();
	//lightData.ambientColor = glm::vec4(0.05f, 0.1f, 0.2f, 1);
	//lightData.light[0].position = glm::vec4(1, 1, 1, 1);
	//lightData.light[0].color = glm::vec4(1, 1, 1, 1);
	//lightData.light[1].position = glm::vec4(-0.2f, 0, 0.6f, 1);
	//lightData.light[1].color = glm::vec4(0.125f, 0.125f, 0.05f, 1);
	//uboLight->BufferSubData(&lightData);


	//entityBuffer->Draw(meshBuffer);

	//glBindVertexArray(vao);


	/*****************************************************/

	/*
	* �I�t�X�N���[���o�b�t�@����ʃT�C�Y�̃|���S���ɒ���t���ďo�͂���.
	*
	*/
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glDisable(GL_BLEND);

	glBindVertexArray(vao);

	progScreen->UseProgram();

	InterfaceBlock::VertexData vd;
	vd = {};
	vd.matMVP = glm::mat4(1);

	uboVertexScreen->BufferSubData(&vd);

	InterfaceBlock::LightData ld;
	ld = {};
	ld.ambientColor = glm::vec4(1);
	uboLightScreen->BufferSubData(&ld);

	progScreen->BindTexture(GL_TEXTURE0, GL_TEXTURE_2D, offscreen->GetTexture());



	glDrawElements(
		GL_TRIANGLES, renderingParts[1].size,
		GL_UNSIGNED_INT, renderingParts[1].offset);
}