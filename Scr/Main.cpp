
#include <iostream>
#include "GLFWEW.h"

int main() {
	//�V���O���g���擾
	GLFWEW::WindowRef window = GLFWEW::Window::Instance();

	if (!window.Init(800,600,"opengl")) {
		std::cerr << "ERORR:GLFWEW�̏������Ɏ��s���܂���" << std::endl;
		return 1;
	}

	// ���C�����[�v.
	while (!window.ShouldClose()) {
		window.Clear();
		window.SwapBuffers();
	}


	return 0;
}