
#include <iostream>
#include "GLFWEW.h"

int main() {
	//シングルトン取得
	GLFWEW::WindowRef window = GLFWEW::Window::Instance();

	if (!window.Init(800,600,"opengl")) {
		std::cerr << "ERORR:GLFWEWの初期化に失敗しました" << std::endl;
		return 1;
	}

	// メインループ.
	while (!window.ShouldClose()) {
		window.Clear();
		window.SwapBuffers();
	}


	return 0;
}