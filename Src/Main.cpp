#include "GameEngine.h"
#include "FileNameList.h"
#include "GLFWEW.h"
#include <random>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

#define ARRAY_SIZE(a) (sizeof(a)/sizeof((a)[0]))
/**
* 敵の円盤の状態を更新する.
*/
struct UpdateToroid
{
	void operator()(Entity::Entity& entity, void* ubo, double delta,
		const glm::mat4& matView, const glm::mat4& matProj)
	{
		// 範囲外に出たら削除する.
		const glm::vec3 pos = entity.Position();
		if(std::abs(pos.x) > 40.0f || std::abs(pos.z) > 40.0f) {
			GameEngine::Instance().RemoveEntity(&entity);
			return;
		}
		// 円盤を回転させる.
		float rot = glm::angle(entity.Rotation());
		rot += glm::radians(60.0f) * static_cast<float>(delta);
		if (rot > glm::pi<float>() * 2.0f) {
			rot -= glm::pi<float>() * 2.0f;
		}
		entity.Rotation(glm::angleAxis(rot, glm::vec3(0, 1, 0)));

		// 頂点シェーダーのパラメータをUBOにコピーする.
		InterfaceBlock::VertexData data;
		data.matModel = entity.CalcModelMatrix();
		data.matNormal = glm::mat4_cast(entity.Rotation());
		data.matMVP = matProj * matView * data.matModel;
		data.color = entity.Color();
		memcpy(ubo, &data, sizeof(InterfaceBlock::VertexData));
	}
};

/**
* 自機の更新.
*/
struct UpdatePlayer
{
	void operator()(Entity::Entity& entity, void* ubo, double delta,
		const glm::mat4& matView, const glm::mat4& matProj)
	{
		GameEngine& game = GameEngine::Instance();
		glm::vec3 vec=glm::vec3(0);
		float rotZ = 0;
		if (game.GetComand(GLFWEW::Window::COMMANDLIST_LEFT_PRESS)) {
			vec.x = 1;
			rotZ = -glm::radians(30.0f);
		}
		else if (game.GetComand(GLFWEW::Window::COMMANDLIST_RIGHT_PRESS)) {
			vec.x = -1;
			rotZ = glm::radians(30.0f);
		}
		if (game.GetComand(GLFWEW::Window::COMMANDLIST_UP_PRESS)) {
			vec.z = 1;
		}
		else if (game.GetComand(GLFWEW::Window::COMMANDLIST_DOWN_PRESS) ){
			vec.z = -1;
		}
		if (vec.x || vec.z) {
			vec = glm::normalize(vec) * 15.0f;
		}
		entity.Velocity(vec);
		entity.Rotation(glm::quat(glm::vec3(0, 0, rotZ)));
		glm::vec3 pos = entity.Position();
		pos = glm::min(glm::vec3(11, 100, 20), glm::max(pos, glm::vec3(-11, -100, 1)));
		entity.Position(pos);



		InterfaceBlock::VertexData data;
		data.matModel = entity.CalcModelMatrix();
		data.matNormal = glm::mat4_cast(entity.Rotation());
		data.matMVP = matProj * matView * data.matModel;
		data.color = entity.Color();
		memcpy(ubo, &data, sizeof(InterfaceBlock::VertexData));



		glm::vec3 Ppos = entity.Position();
		game.Camera({ Ppos+glm::vec3{0, 20, -8, }, Ppos, {0, 0, 1} });

		std::cout << "Debug:プレイヤーのpositionを表示する x:" << Ppos.x <<" y:" <<Ppos.y<<" z:"<<Ppos.z<< std::endl;
	}
};


/**
* ゲーム状態の更新.
*/
class Update
{
public:
	void operator()(double delta)
	{
		GameEngine& game = GameEngine::Instance();

		if (!isInitialized) {
			isInitialized = true;
			game.Camera({ {0, 20, -8, }, {0, 0, 12}, {0, 0, 1} });
			game.AmbientLight(glm::vec4(0.05f, 0.1f, 0.2f, 1));
			game.Light(0, { {40, 100, 10, 1}, {12000, 12000, 12000, 1} });
			pPlayer = game.AddEntity(glm::vec3(0, 0, 2),
				"Aircraft", FILENAME_BMP_PLAYER, UpdatePlayer());
			
	}

	std::uniform_int_distribution<> posXRange(-15, 15);
	std::uniform_int_distribution<> posZRange(38, 40);
	interval -= delta;
	if (interval <= 0) {
		std::uniform_int_distribution<> rndAddingCount(1, 5);
		for (int i = rndAddingCount(game.Rand()); i > 0; --i) {
			const glm::vec3 pos(posXRange(game.Rand()), 0, posZRange(game.Rand()));
			if (Entity::Entity* p = game.AddEntity(
				pos, "Toroid", FILENAME_BMP_TOROID, UpdateToroid())) {
				p->Velocity({ pos.x < 0 ? 3.0f : -3.0f, 0, -12.0f });
			}
		}
		std::normal_distribution<> intervalRange(2.0, 0.5);
		interval += glm::clamp(intervalRange(game.Rand()), 0.5, 3.0);
	}
		}

	private:
		bool isInitialized = false;
		double interval = 0;
		Entity::Entity* pPlayer = nullptr;
	};


//
///**
//* Uniform Block Objectを作成する.
//*
//* @param size Uniform Blockのサイズ.
//* @param data Uniform Blockに転送するデータへのポインタ.
//*
//* @return 作成したUBO.
//*/
//GLuint CreateUBO(GLsizeiptr size, const GLvoid* data = nullptr)
//{
//	GLuint ubo;
//	glGenBuffers(1, &ubo);
//	glBindBuffer(GL_UNIFORM_BUFFER, ubo);
//	glBufferData(GL_UNIFORM_BUFFER, size, data, GL_STREAM_DRAW);
//	glBindBuffer(GL_UNIFORM_BUFFER, 0);
//	return ubo;
//}
//
//
//
///**
//* シェーダコードをコンパイルする.
//*
//* @param type シェーダの種類.
//* @param string シェーダコードへのポインタ.
//*
//* @return 作成したシェーダオブジェクト.
//*/
//GLuint CompileShader(GLenum type, const GLchar* string)
//{
//	GLuint shader = glCreateShader(type);
//	glShaderSource(shader, 1, &string, nullptr);
//	glCompileShader(shader);
//	GLint compiled = 0;
//	glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
//	if (!compiled) {
//		GLint infoLen = 0;
//		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
//		if (infoLen) {
//			std::vector<char> buf;
//			buf.resize(infoLen);
//			if (static_cast<int>(buf.size()) >= infoLen) {
//				glGetShaderInfoLog(shader, infoLen, NULL, buf.data());
//				std::cerr << "ERROR: シェーダのコンパイルに失敗￥n" << buf.data() << std::endl;
//			}
//		}
//		glDeleteShader(shader);
//		return 0;
//	}
//	return shader;
//}
//
//
///**
//* プログラムオブジェクトを作成する.
//*
//* @param vsCode 頂点シェーダコードへのポインタ.
//* @param fsCode フラグメントシェーダコードへのポインタ.
//*
//* @return 作成したプログラムオブジェクト.
//*/
//GLuint CreateShaderProgram(const GLchar* vsCode, const GLchar* fsCode)
//{
//	GLuint vs = CompileShader(GL_VERTEX_SHADER, vsCode);
//	GLuint fs = CompileShader(GL_FRAGMENT_SHADER, fsCode);
//	if (!vs || !fs) {
//		return 0;
//	}
//	GLuint program = glCreateProgram();
//	glAttachShader(program, fs);
//	glDeleteShader(fs);
//	glAttachShader(program, vs);
//	glDeleteShader(vs);
//	glLinkProgram(program);
//	GLint linkStatus = GL_FALSE;
//	glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
//	if (linkStatus != GL_TRUE) {
//		GLint infoLen = 0;
//		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLen);
//		if (infoLen) {
//			std::vector<char> buf;
//			buf.resize(infoLen);
//			if (static_cast<int>(buf.size()) >= infoLen) {
//				glGetProgramInfoLog(program, infoLen, NULL, buf.data());
//				std::cerr << "ERROR: シェーダのリンクに失敗￥n" << buf.data() << std::endl;
//			}
//		}
//		glDeleteProgram(program);
//		return 0;
//	}
//	return program;
//}

int main()
{
	GameEngine& game = GameEngine::Instance();
	if (!game.Init(800, 600, "OpenGL Tutorial")) {
		return 1;
	}
	game.LoadTextureFromFile(FILENAME_BMP_TOROID);
	game.LoadTextureFromFile(FILENAME_BMP_PLAYER);
	game.LoadMeshFromFile(FILENAME_FBX_TOROID);
	game.LoadMeshFromFile(FILENAME_FBX_PLAYER);
	game.UpdateFunc(Update());
	game.Run();
	return 0;
}



//int main()
//{
//	GLFWEW::Window& window = GLFWEW::Window::Instance();
//	if (!window.Init(1280,720,"GameGameGame!!")) {
//
//	}
//	const GLuint vbo = CreateVBO(sizeof(vertices), vertices);
//	const GLuint ibo = CreateIBO(sizeof(indices),indices);
//	const GLuint vao = CreateVAO(vbo,ibo);
//	const UniformBufferPtr uboVertex = UniformBuffer::Create(
//		sizeof(VertexData), BINDINGPOINT_VERTEXDATA_TEST, "VertexData");
//	const UniformBufferPtr uboLight = UniformBuffer::Create(
//		sizeof(LightData), BINDINGPOINT_LIGHTDATA_TEST, "LightData");
//	const UniformBufferPtr uboVertexScreen = UniformBuffer::Create(
//		sizeof(VertexData), BINDINGPOINT_VERTEXDATA, "VertexData");
//	const UniformBufferPtr uboLightScreen = UniformBuffer::Create(
//		sizeof(LightData), BINDINGPOINT_LIGHTDATA, "LightData");
//
//
//	const Shader::ProgramPtr progTutorial =
//		Shader::Program::Create(FILENAME_VERT_TUTORIAL4,FILENAME_FRAG_TUTORIAL4);
//	const Shader::ProgramPtr progScreen=
//		Shader::Program::Create(FILENAME_VERT_TUTORIAL3, FILENAME_FRAG_TUTORIAL3);
//
//
//	if (!vbo || !vao ||!ibo|| !uboVertex || !uboLight || !progTutorial || !progScreen) {
//
//		return 1;
//	}
//
//
//	TexturePtr tex = Texture::LoadFromFile(FILENAME_TGA_BACK);
//	TexturePtr tex2 = Texture::LoadFromFile(FILENAME_BMP_GEAR);
//	TexturePtr texToroid = Texture::LoadFromFile(FILENAME_BMP_TOROID);
//	if (!tex ||!tex2 || !texToroid) {
//		std::cout << "てくすちゃ失敗";
//
//		return 1;
//	
//	}
//
//
//	Mesh::BufferPtr meshBuffer = Mesh::Buffer::Create(50000, 50000);
//	meshBuffer->LoadMeshFromFile(FILENAME_FBX_TOROID);
//
//	Entity::BufferPtr entityBuffer = Entity::Buffer::Create(1024,
//		sizeof(VertexData), BINDINGPOINT_VERTEXDATA_TEST, "VertexData");
//	if (!entityBuffer) {
//		return 1;
//		
//	}
//
//
//	progTutorial->UniformBlockBinding(*uboVertex);
//	progTutorial->UniformBlockBinding(*uboLight);
//	progScreen->UniformBlockBinding(*uboVertexScreen);
//	progScreen->UniformBlockBinding(*uboLightScreen);
//	GLint maxubosize;
//	glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS,&maxubosize);
//	std::cout << "設定できるUBOのバインディングポイントの数を表示:" << (size_t)maxubosize << std::endl;
//
//	const OffscreenBufferPtr offscreen = OffscreenBuffer::Create(window.GetSize().x,window.GetSize().y);
//
//
//	while (!window.ShouldClose()) {
//		glBindFramebuffer(GL_FRAMEBUFFER,offscreen->GetFramebuffer());
//		glClearColor(0.1f, 0.3f, 0.5f, 1.0f);
//		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//		
//		glEnable(GL_DEPTH_TEST);
//
//		progScreen->UseProgram();
//		// 座標変換行列を作成してシェーダに転送する.
//		const glm::vec3 viewPos = glm::vec4(0, 10, -20, 1);
//		const glm::mat4x4 matProj =
//			glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 500.0f);
//		const glm::mat4x4 matView =
//			glm::lookAt(viewPos, glm::vec3(0, 0, 20), glm::vec3(0, 1, 0));
//		const glm::mat4x4 matMVP = matProj * matView;
//		//注意点.UseProgram()で対象プログラムが設定されていないとダメ.
//		VertexData vertexData;
//		vertexData.matMVP = matProj * matView;
//		vertexData.color = glm::vec4(1, 1, 1, 1);
//		uboVertexScreen->BufferSubData(&vertexData);
//
//		LightData lightData;
//		lightData.ambientColor = glm::vec4(0.05f, 0.1f, 0.2f, 1);
//		lightData.light[0].position = glm::vec4(1, 1, 1, 1);
//		lightData.light[0].color = glm::vec4(1, 1, 1, 1);
//		lightData.light[1].position = glm::vec4(-0.2f, 0, 0.6f, 1);
//		lightData.light[1].color = glm::vec4(0.125f, 0.125f, 0.05f, 1);
//		uboLightScreen->BufferSubData(&lightData);
//
//
//		progScreen->BindTexture(GL_TEXTURE0,GL_TEXTURE_2D,tex->Id());
//
//
//		glBindVertexArray(vao);
//		glDrawElements(GL_TRIANGLES,renderingParts[0].size,GL_UNSIGNED_INT,renderingParts[0].offset);
//		progScreen->BindTexture(GL_TEXTURE0, GL_TEXTURE_2D, texToroid->Id());
//		meshBuffer->BindVAO();
//		meshBuffer->GetMesh("Toroid")->Draw(meshBuffer);
//
//		Update(entityBuffer, meshBuffer, texToroid, progTutorial);
//
//		entityBuffer->Update(1.0 / 60.0, matView, matProj);
//
//		progTutorial->UseProgram();
//		lightData.ambientColor = glm::vec4(0.05f, 0.1f, 0.2f, 1);
//		lightData.light[0].position = glm::vec4(1, 1, 1, 1);
//		lightData.light[0].color = glm::vec4(1, 1, 1, 1);
//		lightData.light[1].position = glm::vec4(-0.2f, 0, 0.6f, 1);
//		lightData.light[1].color = glm::vec4(0.125f, 0.125f, 0.05f, 1);
//		uboLight->BufferSubData(&lightData);
//
//
//		entityBuffer->Draw(meshBuffer);
//
//		glBindVertexArray(vao);
//
//
//
//		glBindFramebuffer(GL_FRAMEBUFFER,0);
//		glClearColor(0.1f, 0.3f, 0.5f, 1.0f);
//		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//		progScreen->UseProgram();
//		progScreen->BindTexture(GL_TEXTURE0,GL_TEXTURE_2D,offscreen->GetTexture());
//			
//		vertexData = {};
//		vertexData.matMVP = glm::mat4(1);
//
//		uboVertexScreen->BufferSubData(&vertexData);
//
//		lightData = {};
//		lightData.ambientColor = glm::vec4(1);
//		uboLightScreen->BufferSubData(&lightData);
//
//
//		glDrawElements(
//			GL_TRIANGLES,renderingParts[1].size,
//			GL_UNSIGNED_INT,renderingParts[1].offset);
//
//		window.SwapBuffers();
//	}
//	glDeleteVertexArrays(1, &vao);
//
//
//	return 0;
//}