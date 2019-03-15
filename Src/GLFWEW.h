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
	* GLFWとGLEWのラッパークラス.
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
		void Exit() { this->exit = true; }			//ゲームを終了させる.

		enum COMMANDLIST {
			COMMANDLIST_DECISION,		//決定,
			COMMANDLIST_CANCEL,			//キャンセル
			COMMANDLIST_UP_1ST,			//上押した瞬間
			COMMANDLIST_DOWN_1ST,		//下押した瞬間
			COMMANDLIST_RIGHT_1ST,		//右押した瞬間
			COMMANDLIST_LEFT_1ST,		//左押した瞬間
			COMMANDLIST_UP_PRESS,		//上押した瞬間
			COMMANDLIST_DOWN_PRESS,		//下押した瞬間
			COMMANDLIST_RIGHT_PRESS,	//右押した瞬間
			COMMANDLIST_LEFT_PRESS,		//左押した瞬間
			COMMANDLIST_BRAKE,			//プレイ中のブレーキ
			COMMANDLIST_PAUSE,			//一時停止,
			COMMANDLIST_SNAP,			//スナップ
			COMMANDLISE_MAX,
		};
		bool GetCommand(COMMANDLIST);//対象のボタン操作が行われているかどうか,
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
		GamePad gamepad;	//パッド入力
		//ゲームを終了させる変数.
		bool exit = false;
	};

} // namespace GLFWEW

#endif // GLFWEW_H_INCLUDED
