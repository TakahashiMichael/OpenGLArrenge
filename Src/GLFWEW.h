/**
* @file GLFWEW.h
*/
#ifndef GLFWEW_H_INCLUDED
#define GLFWEW_H_INCLUDED
#include "GamePad.h"
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/vec2.hpp>

namespace GLFWEW {

	/**
	* GLFW��GLEW�̃��b�p�[�N���X.
	*/
	class Window
	{
	public:
		static Window& Instance();
		bool Init(int w, int h, const char* title);
		bool ShouldClose() const;
		void SwapBuffers() const;
		void Update();

		void InitTimer();
		void UpdateTimer();
		double DeltaTime() const;
		const glm::vec2& GetSize()const { return size; }

        bool IsKeyDown(int key) const;
        bool IsKeyPressed(int key) const;
		const GamePad& GetGamePad()const;
        glm::vec2 GetMousePosition() const;
        int GetMouseButton(int button) const;
		void Exit() { this->exit = true; }			//�Q�[�����I��������.

		enum COMMANDLIST {
			COMMANDLIST_DECISION,		//����,
			COMMANDLIST_CANCEL,			//�L�����Z��
			COMMANDLIST_UP_1ST,			//�㉟�����u��
			COMMANDLIST_DOWN_1ST,		//���������u��
			COMMANDLIST_RIGHT_1ST,		//�E�������u��
			COMMANDLIST_LEFT_1ST,		//���������u��
			COMMANDLIST_UP_PRESS,		//�㉟�����u��
			COMMANDLIST_DOWN_PRESS,		//���������u��
			COMMANDLIST_RIGHT_PRESS,	//�E�������u��
			COMMANDLIST_LEFT_PRESS,		//���������u��
			COMMANDLIST_BRAKE,			//�v���C���̃u���[�L
			COMMANDLIST_PAUSE,			//�ꎞ��~,
			COMMANDLIST_SNAP,			//�X�i�b�v
			COMMANDLISE_MAX,
		};
		bool GetCommand(COMMANDLIST);//�Ώۂ̃{�^�����삪�s���Ă��邩�ǂ���,
	private:
		Window();
		~Window();
		Window(const Window&) = delete;
		Window& operator=(const Window&) = delete;

		void UpdateGamePad();

		bool isGLFWInitialized = false;
		bool isInitialized = false;
		GLFWwindow* window = nullptr;
        double previousTime = 0;
        double deltaTime = 0;
		glm::vec2 size;

        enum class KeyState : char {
          release,
          press1st,
          press,
        };

        KeyState keyState[GLFW_KEY_LAST + 1];
		GamePad gamepad;	//�p�b�h����
		//�Q�[�����I��������ϐ�.
		bool exit = false;
	};

} // namespace GLFWEW

#endif // GLFWEW_H_INCLUDED
