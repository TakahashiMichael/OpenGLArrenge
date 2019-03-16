
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
	glm::vec3 position; ///<座標
	glm::vec4 color;	///<色データ
	glm::vec2 texCoord;	///<テクスチャデータ.
};
/// 頂点データ.
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

/// インデックスデータ.
const GLuint indices[] = {
  0, 1, 2, 2, 3, 0,
  4,5,6,7,8,9,
  10,11,12,12,13,10,
};

/// 頂点シェーダのパラメータ型.
struct VertexData
{
	glm::mat4 matMVP;
	glm::mat4 matModel = glm::mat4(1);
	glm::mat3x4 matNormal;
	glm::vec4 color;

};
const int maxLightCount = 4; ///< ライトの数.

/**
* ライトデータ(点光源).
*/
struct PointLight
{
	glm::vec4 position; ///< 座標(ワールド座標系).
	glm::vec4 color; ///< 明るさ.
};

/**
* ライティングパラメータをシェーダに転送するための構造体.
*/
struct LightData
{
	glm::vec4 ambientColor; ///< 環境光.
	PointLight light[maxLightCount]; ///< ライトのリスト.
};

/// バインディングポイント.
enum BindingPoint
{
	BINDINGPOINT_VERTEXDATA, ///< 頂点シェーダ用パラメータのバインディングポイント.
	BINDINGPOINT_LIGHTDATA, ///< ライティングパラメータ用のバインディングポイント.
	BINDINGPOINT_VERTEXDATA_TEST, ///< 頂点シェーダ用パラメータのバインディングポイント.
	BINDINGPOINT_LIGHTDATA_TEST, ///< ライティングパラメータ用のバインディングポイント.
};

/**
* 部分描画データ.
*/
struct RenderingPart
{
	GLvoid* offset; ///< 描画開始インデックスのバイトオフセット.
	GLsizei size; ///< 描画するインデックス数.
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
*
* @param GLsieiptr size		登録する頂点データのサイズ.
* @param const GLvoid* data バイトオフセット.
*
* @retval 作成したVBOのID
*/
GLuint CreateVBO(GLsizeiptr size,const GLvoid* data) {
	GLuint vbo = 0;
	glGenBuffers(1,&vbo);//(作成個数,idのポインタ)
	glBindBuffer(GL_ARRAY_BUFFER,vbo);//(種類,id)
	glBufferData(GL_ARRAY_BUFFER,size,data,GL_STATIC_DRAW);//第四引数はアクセスの頻度
	glBindBuffer(GL_ARRAY_BUFFER, 0);//割り当ての解除.

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



/*
* vertex Array Objectを作成する.
*
* @param vbo VAOに関連つけられるVBO
*
* @retval 作成したVAO
*
*
*/
GLuint CreateVAO(GLuint vbo,GLuint ibo) {
	GLuint vao = 0;
	glGenVertexArrays(1,&vao);//(作成個数,idのポインタ)
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER,vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,ibo);
	/*SetVertexAttribPointer(n,str,str::meb)
	* glEnableVertexAttribArray(n),
	* glVertexAttribPointer(n,メンバサイズ,GL_FLOAT,GL_FALSE,構造体サイズ,dataoffset);
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
* 敵の円盤の状態を更新する.
*/
struct UpdateToroid
{
	explicit UpdateToroid(const Entity::BufferPtr& buffer) : entityBuffer(buffer) {}

	void operator()(Entity::Entity& entity, void* ubo, double delta,
		const glm::mat4& matView, const glm::mat4& matProj)
	{
		// 範囲外に出たら削除する.
		const glm::vec3 pos = entity.Position();
		if (std::abs(pos.x) > 40.0f || std::abs(pos.z) > 40.0f) {
			entityBuffer->RemoveEntity(&entity);
			return;
		}

		// 円盤を回転させる.
		float rot = glm::angle(entity.Rotation());
		rot += glm::radians(15.0f) * static_cast<float>(delta);
		if (rot > glm::pi<float>() * 2.0f) {
			rot -= glm::pi<float>() * 2.0f;
		}
		entity.Rotation(glm::angleAxis(rot, glm::vec3(0, 1, 0)));

		// 頂点シェーダーのパラメータをUBOにコピーする.
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
* ゲームの状態を更新する.
*
* @param entityBuffer 敵エンティティ追加先のエンティティバッファ.
* @param meshBuffer   敵エンティティのメッシュを管理しているメッシュバッファ.
* @param tex          敵エンティティ用のテクスチャ.
* @param prog         敵エンティティ用のシェーダープログラム.
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
* Uniform Block Objectを作成する.
*
* @param size Uniform Blockのサイズ.
* @param data Uniform Blockに転送するデータへのポインタ.
*
* @return 作成したUBO.
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
* シェーダコードをコンパイルする.
*
* @param type シェーダの種類.
* @param string シェーダコードへのポインタ.
*
* @return 作成したシェーダオブジェクト.
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
				std::cerr << "ERROR: シェーダのコンパイルに失敗￥n" << buf.data() << std::endl;
			}
		}
		glDeleteShader(shader);
		return 0;
	}
	return shader;
}


/**
* プログラムオブジェクトを作成する.
*
* @param vsCode 頂点シェーダコードへのポインタ.
* @param fsCode フラグメントシェーダコードへのポインタ.
*
* @return 作成したプログラムオブジェクト.
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
				std::cerr << "ERROR: シェーダのリンクに失敗￥n" << buf.data() << std::endl;
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
		std::cout << "てくすちゃ失敗";

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
	std::cout << "設定できるUBOのバインディングポイントの数を表示:" << (size_t)maxubosize << std::endl;

	const OffscreenBufferPtr offscreen = OffscreenBuffer::Create(window.GetSize().x,window.GetSize().y);


	while (!window.ShouldClose()) {
		glBindFramebuffer(GL_FRAMEBUFFER,offscreen->GetFramebuffer());
		glClearColor(0.1f, 0.3f, 0.5f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glEnable(GL_DEPTH_TEST);

		progScreen->UseProgram();
		// 座標変換行列を作成してシェーダに転送する.
		const glm::vec3 viewPos = glm::vec4(0, 10, -20, 1);
		const glm::mat4x4 matProj =
			glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 500.0f);
		const glm::mat4x4 matView =
			glm::lookAt(viewPos, glm::vec3(0, 0, 20), glm::vec3(0, 1, 0));
		const glm::mat4x4 matMVP = matProj * matView;
		//注意点.UseProgram()で対象プログラムが設定されていないとダメ.
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