/**
* @file InterfaceBlock.h
*/
#ifndef INTERFACEBLOCK_H_INCLUDED
#define INTERFACEBLOCK_H_INCLUDED
#include <glm/glm.hpp>

namespace InterfaceBlock {


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

}

#endif
