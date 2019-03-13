// @file GLFWEW.h

#ifndef GLFWEW_H_INCLUDED
#define GLFWEW_H_INCLUDED

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/matrix.hpp>
#include <string>


namespace GLFWEW {

	/*
	* GLFW と GLEW のラッパークラス.
	*/
	class Window;
	using WindowRef = Window & ;		//参照型
	class Window
	{
	public:
		static WindowRef Instance();		//シングルトンのための関数.
		bool Init(float ,float,std::string);
		bool Init();
		bool ShouldClose()const;			//ウィンドウを閉じるか否か
		void Clear();						//画面を消す.
		void SwapBuffers()const;			//バックバッファ切り替え.
		float Width()const { return size.x; }
		float Height()const { return size.y; }


	private:
		//シングルトンなので色々privateに
		Window()=default;
		~Window();
		Window(const WindowRef) = delete;
		WindowRef operator=(const WindowRef) = delete;

		glm::vec4 clearColor=glm::vec4(0.1f,0.3f,0.5f,1.0f);		//画面を塗りつぶす色.
		glm::vec2 size;				//ウィンドウのサイズ.
		std::string name;			//ウィンドウのlogo.

		bool isGLFWInitialized = false;		//fwのイニシャライズが完了しているか.
		bool isInitialized = false;			//クラスのイニシャライズが完了しているかどうか.

		GLFWwindow* window = nullptr;		//描画に使用するwindowptr
	};

}	//namespace GLFWEW



#endif // !GLFWEW_H_INCLUDED
