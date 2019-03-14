
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
	glm::mat4 matModel;
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

/**
* ポストエフェクトデータをシェーダに転送するための構造体.
*/
struct PostEffectData
{
	glm::mat4x4 matColor; ///< 色変換行列.
};


/// バインディングポイント.
enum BindingPoint
{
	BINDINGPOINT_VERTEXDATA, ///< 頂点シェーダ用パラメータのバインディングポイント.
	BINDINGPOINT_LIGHTDATA, ///< ライティングパラメータ用のバインディングポイント.
	BINDINGPOINT_POSTEFFECTDATA, ///< ポストエフェクトパラメータ用のバインディングポイント.
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


int main() {


	//シングルトン取得
	GLFWEW::WindowRef window = GLFWEW::Window::Instance();

	if (!window.Init(800.0f,600.0f,"opengl")) {
		std::cerr << "ERORR:GLFWEWの初期化に失敗しました" << std::endl;
		return 1;
	}

	//色々作成
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
	
	//シェーダにuboを登録する.
	progSimple->UniformBlockBinding(*entityBuffer->UniformBuffer());
	progSimple->UniformBlockBinding(*uboVertex);
	progSimple->UniformBlockBinding(*uboLight);
	progColorFilter->UniformBlockBinding(*uboPostEffect);



	const OffscreenBufferPtr offscreen = OffscreenBuffer::Create(window.Width(), window.Height());

	//仮変数.
	int count = 0;
	const int countMax = 120;
	bool imageflag = false;

	//仮変数.
	// メインループ.
	while (!window.ShouldClose()) {
		Update(entityBuffer, meshBuffer, texToroid, progSimple);
		glBindFramebuffer(GL_FRAMEBUFFER,offscreen->GetFrameBuffer());
		window.Clear();
		glEnable(GL_DEPTH_TEST);
		progSimple->UseProgram();


		//座標変換行列を作成してシェーダに転送.
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


		//オフスクリーンバッファへ描画する.
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

		//オフスクリーンバッファを使用して画面の描画
		glDrawElements(
		GL_TRIANGLES, renderingParts[1].size,
		GL_UNSIGNED_INT, renderingParts[1].offset);

		//バッファーの入れ替え
		window.SwapBuffers();
	}
	//vaoの削除
	glDeleteVertexArrays(1,&vao);

	return 0;
}