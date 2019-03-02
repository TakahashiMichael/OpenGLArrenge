// @file GLFWEW.cpp

#include "GLFWEW.h"
#include <iostream>

namespace GLFWEW 
{

	/*
	* �V���O���g���C���X�^���X���擾����.
	* @return Window �̃V���O���g���C���X�^���X.
	*/
	WindowRef Window::Instance()
	{
		static Window instance;
		return instance;
	}

	/*
	* �f�X�g���N�^.
	* �I���ɕK�v�ȏ����͈�C
	*/
	Window::~Window()
	{
		//glfw������������Ă���ꍇ��terminate���Ăяo��
		if (isGLFWInitialized) 
		{
			glfwTerminate();
		}

	}

	/**
	* GLFW����̃G���[�񍐂���������.
	*
	* @param error �G���[�ԍ�.
	* @param desc  �G���[�̓��e.
	*/
	void ErrorCallback(int error, const char* desc)
	{
		std::cerr << "ERROR: " << desc << std::endl;
	}


	bool Window::Init(float w, float h, std::string s) {
		size = glm::vec2(w,h);
		name = s;
		return Init();
	}

	/*
	* ������
	* �G���[�R�[���o�b�N�̓o�^
	* glfw�̏�����
	* window�̍쐬
	* �R���e�L�X�g�̍쐬.�ݒ�
	* glew�̏�����
	* ���̕�����̎擾�ƃR���\�[���ɕ\��
	*
	* @retval true: ����������
	* @retval false: ���������s.
	*/
	bool Window::Init() {
		//��d�������`�F�b�N
		if (isInitialized)
		{
			std::cerr << "ERORR: GLFWEW�͂��łɏ���������Ă��܂�." << std::endl;
			return false;
		}
		//glfw�������`�F�b�N
		if (!isGLFWInitialized)
		{
			//�R�[���o�b�N�o�^
			glfwSetErrorCallback(ErrorCallback);
			//glfw������
			if (glfwInit() != GL_TRUE) {
				return false;
			}
			isGLFWInitialized = true;
		}

		if (!window) {
			//�E�B���h�E�̍쐬.
			window = glfwCreateWindow(size.x,size.y,name.c_str(),nullptr,nullptr);
			if (!window) {
				return false;
			}
			//�R���e�L�X�g�쐬.
			glfwMakeContextCurrent(window);
		}

		//glew�̏�����.
		if (glewInit() != GLEW_OK) {
			std::cerr << "ERROR: GLEW�̏������Ɏ��s���܂���" << std::endl;
			return false;
		}

		const GLubyte* renderer = glGetString(GL_RENDERER);
		std::cout << "Renderer: " << renderer << std::endl;
		const GLubyte* version = glGetString(GL_VERSION);
		std::cout << "Version: " << version << std::endl;

		//�L�^����.
		isInitialized = true;

		return true;
	}

	/*
	* �E�B���h�E����邩�ׂ������ׂ�.
	*
	* @retval true ����.
	* @retval false ���Ȃ�.
	*/
	bool Window::ShouldClose() const
	{
		return glfwWindowShouldClose(window) != 0;
	}

	/*
	*
	*
	*
	*
	*/
	void Window::Clear() 
	{
		glClearColor(clearColor.r, clearColor.g, clearColor.b, clearColor.a);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	/*
	* �t�����g�o�b�t�@�ƃo�b�N�o�b�t�@��؂�ւ���.
	* ���łɃC�x���g�֐������b�s���O.
	*/
	void Window::SwapBuffers() const
	{
		//�C�x���g�֐�
		glfwPollEvents();
		//�o�b�t�@�[�؂�ւ��֐�.
		glfwSwapBuffers(window);
	}

}	//namespace GLFWEW