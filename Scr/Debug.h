//@ file Debug.h

#ifndef DEBUG_H_INCLUDED
#define DEBUG_H_INCLUDED

#include <glm/gtc/matrix_transform.hpp>
#include <string>
/*
* ���܂�ɂ�glm::vec3�^�̈ʒu�v�f�������cout����@�B�����������̂ő΍�̂��ߍ쐬
* 
* �Ȃ�cpp��g���̂����������Ȃ��C������...2/11
*/

namespace Debug {
	
	void print(const glm::vec3&);							//�v�f���ʂɕ\��
	void print(std::string,const glm::vec3&);				//�Ȃɂ��t�������������͂������,
	void print(const glm::mat4x4&);						//�s��̒��g����Â��o���ĕ\��.
}//namespace Debug


#endif // !DEBUG_H_INCLUDED
