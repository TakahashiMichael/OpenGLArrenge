/**
* @file InterfaceBlock.h
*/
#ifndef INTERFACEBLOCK_H_INCLUDED
#define INTERFACEBLOCK_H_INCLUDED
#include <glm/glm.hpp>

namespace InterfaceBlock {


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

}

#endif
