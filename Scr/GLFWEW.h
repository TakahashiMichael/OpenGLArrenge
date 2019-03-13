// @file GLFWEW.h

#ifndef GLFWEW_H_INCLUDED
#define GLFWEW_H_INCLUDED

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/matrix.hpp>
#include <string>


namespace GLFWEW {

	/*
	* GLFW �� GLEW �̃��b�p�[�N���X.
	*/
	class Window;
	using WindowRef = Window & ;		//�Q�ƌ^
	class Window
	{
	public:
		static WindowRef Instance();		//�V���O���g���̂��߂̊֐�.
		bool Init(float ,float,std::string);
		bool Init();
		bool ShouldClose()const;			//�E�B���h�E����邩�ۂ�
		void Clear();						//��ʂ�����.
		void SwapBuffers()const;			//�o�b�N�o�b�t�@�؂�ւ�.
		float Width()const { return size.x; }
		float Height()const { return size.y; }


	private:
		//�V���O���g���Ȃ̂ŐF�Xprivate��
		Window()=default;
		~Window();
		Window(const WindowRef) = delete;
		WindowRef operator=(const WindowRef) = delete;

		glm::vec4 clearColor=glm::vec4(0.1f,0.3f,0.5f,1.0f);		//��ʂ�h��Ԃ��F.
		glm::vec2 size;				//�E�B���h�E�̃T�C�Y.
		std::string name;			//�E�B���h�E��logo.

		bool isGLFWInitialized = false;		//fw�̃C�j�V�����C�Y���������Ă��邩.
		bool isInitialized = false;			//�N���X�̃C�j�V�����C�Y���������Ă��邩�ǂ���.

		GLFWwindow* window = nullptr;		//�`��Ɏg�p����windowptr
	};

}	//namespace GLFWEW



#endif // !GLFWEW_H_INCLUDED
