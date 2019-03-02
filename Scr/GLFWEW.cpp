// @file GLFWEW.cpp

#include "GLFWEW.h"
#include <iostream>

namespace GLFWEW 
{

	/*
	* シングルトンインスタンスを取得する.
	* @return Window のシングルトンインスタンス.
	*/
	WindowRef Window::Instance()
	{
		static Window instance;
		return instance;
	}

	/*
	* デストラクタ.
	* 終了に必要な処理は一任
	*/
	Window::~Window()
	{
		//glfwが初期化されている場合はterminateを呼び出す
		if (isGLFWInitialized) 
		{
			glfwTerminate();
		}

	}

	/**
	* GLFWからのエラー報告を処理する.
	*
	* @param error エラー番号.
	* @param desc  エラーの内容.
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
	* 初期化
	* エラーコールバックの登録
	* glfwの初期化
	* windowの作成
	* コンテキストの作成.設定
	* glewの初期化
	* 情報の文字列の取得とコンソールに表示
	*
	* @retval true: 初期化成功
	* @retval false: 初期化失敗.
	*/
	bool Window::Init() {
		//二重初期化チェック
		if (isInitialized)
		{
			std::cerr << "ERORR: GLFWEWはすでに初期化されています." << std::endl;
			return false;
		}
		//glfw初期化チェック
		if (!isGLFWInitialized)
		{
			//コールバック登録
			glfwSetErrorCallback(ErrorCallback);
			//glfw初期化
			if (glfwInit() != GL_TRUE) {
				return false;
			}
			isGLFWInitialized = true;
		}

		if (!window) {
			//ウィンドウの作成.
			window = glfwCreateWindow(size.x,size.y,name.c_str(),nullptr,nullptr);
			if (!window) {
				return false;
			}
			//コンテキスト作成.
			glfwMakeContextCurrent(window);
		}

		//glewの初期化.
		if (glewInit() != GLEW_OK) {
			std::cerr << "ERROR: GLEWの初期化に失敗しました" << std::endl;
			return false;
		}

		const GLubyte* renderer = glGetString(GL_RENDERER);
		std::cout << "Renderer: " << renderer << std::endl;
		const GLubyte* version = glGetString(GL_VERSION);
		std::cout << "Version: " << version << std::endl;

		//記録する.
		isInitialized = true;

		return true;
	}

	/*
	* ウィンドウを閉じるかべきか調べる.
	*
	* @retval true 閉じる.
	* @retval false 閉じない.
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
	* フロントバッファとバックバッファを切り替える.
	* ついでにイベント関数もラッピング.
	*/
	void Window::SwapBuffers() const
	{
		//イベント関数
		glfwPollEvents();
		//バッファー切り替え関数.
		glfwSwapBuffers(window);
	}

}	//namespace GLFWEW