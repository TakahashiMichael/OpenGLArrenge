/**
* @file GLFWE.cpp
*/
#include "GLFWEW.h"
#include <iostream>

/// GLFW��GLEW�����b�v���邽�߂̖��O���.
namespace GLFWEW {

void APIENTRY OutputGLDebugMessage(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, GLvoid *userParam)
{
  std::cerr << message << "\n";
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

/**
* �V���O���g���C���X�^���X���擾����.
*
* @return Window�̃V���O���g���C���X�^���X.
*/
Window& Window::Instance()
{
	static Window instance;
	return instance;
}

/**
* �R���X�g���N�^.
*/
Window::Window()
{
}

/**
* �f�X�g���N�^.
*/
Window::~Window()
{
	if (isGLFWInitialized) {
		glfwTerminate();
	}
}

/**
* GLFW/GLEW�̏�����.
*
* @param w �E�B���h�E�̕`��͈͂̕�(�s�N�Z��).
* @param h �E�B���h�E�̕`��͈͂̍���(�s�N�Z��).
* @param title �E�B���h�E�^�C�g��(UTF-8��0�I�[������).
*
* @retval true ����������.
* @retval false ���������s.
*/
bool Window::Init(int w, int h, const char* title)
{
	if (isInitialized) {
		std::cerr << "ERROR: GLFWEW�͊��ɏ���������Ă��܂�." << std::endl;
		return false;
	}
	if (!isGLFWInitialized) {
		glfwSetErrorCallback(ErrorCallback);
		if (glfwInit() != GL_TRUE) {
			return false;
		}
        isGLFWInitialized = true;
	}

	if (!window) {
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
		//glfwGetPrimaryMonitor();�t���X�N���[���|�C���^�擾�֐�
		window = glfwCreateWindow(w, h, title, nullptr, nullptr);


		if (!window) {
			return false;
		}
		glfwMakeContextCurrent(window);
    }

	if (glewInit() != GLEW_OK) {
		std::cerr << "ERROR: GLEW�̏������Ɏ��s���܂���." << std::endl;
		return false;
	}

    glDebugMessageCallback(OutputGLDebugMessage, nullptr);
	size = glm::vec2(w,h);

	const GLubyte* renderer = glGetString(GL_RENDERER);
	std::cout << "Renderer: " << renderer << std::endl;
	const GLubyte* version = glGetString(GL_VERSION);
	std::cout << "Version: " << version << std::endl;

    isInitialized = true;
	return true;
}

/**
* �E�B���h�E�����ׂ������ׂ�.
*
* @retval true ����.
* @retval false ���Ȃ�.
*/
bool Window::ShouldClose() const
{
	return (glfwWindowShouldClose(window) != 0)||(this->exit);
}

/**
* �t�����g�o�b�t�@�ƃo�b�N�o�b�t�@��؂�ւ���.
*/
void Window::SwapBuffers() const
{
	glfwPollEvents();
	glfwSwapBuffers(window);
}

/**
* �L�[��������Ă��邩���ׂ�.
*
* @param key ���ׂ�L�[��ID(GLFW_KEY_A�Ȃ�).
*
* @retval true  �L�[��������Ă���.
* @retval false �L�[��������Ă��Ȃ�.
*/
bool Window::IsKeyPressed(int key) const
{
  if (key < 0 || key >= GLFW_KEY_LAST + 1) {
    return false;
  }
  return keyState[key] != KeyState::release;
}

/**
* �L�[�������ꂽ�u�Ԃ����ׂ�.
*
* @param key ���ׂ�L�[��ID(GLFW_KEY_A�Ȃ�).
*
* @retval true  �L�[�������ꂽ�u��.
* @retval false �L�[�������ꂽ�u�Ԃł͂Ȃ�.
*/
bool Window::IsKeyDown(int key) const
{
  if (key < 0 || key >= GLFW_KEY_LAST + 1) {
    return false;
  }
  return keyState[key] == KeyState::press1st;
}

/*
* �Q�[���p�b�h�̏�Ԃ��擾����.
*
* @return �Q�[���p�b�h�̏��.
*/
const GamePad& Window::GetGamePad()const
{
	return gamepad;
}


/**
* �^�C�}�[������������.
*
* @sa UpdateTimer, GetDeltaTime
*/
void Window::InitTimer()
{
  glfwSetTime(0.0);
  previousTime = 0.0;
  deltaTime = 0.0;
}

/**
* �^�C�}�[���X�V����.
*
* @sa InitTimer, GetDeltaTime
*/
void Window::UpdateTimer()
{
  // �o�ߎ��Ԃ��v��.
  const double currentTime = glfwGetTime();
  
  deltaTime = currentTime - previousTime;
  while (deltaTime * 60 < 1.0f)
  {
	  const double currentTime = glfwGetTime();

	  deltaTime = currentTime - previousTime;

  }
  previousTime = currentTime;

  // �o�ߎ��Ԃ������Ȃ肷���Ȃ��悤�ɒ���.
  const float upperLimit = 0.25f; // �o�ߎ��ԂƂ��ċ��e�������.
  if (deltaTime > upperLimit) {
    deltaTime = 0.1f;
  }
  std::cout << "DeltaTime��\������:" << deltaTime*60 << std::endl;
}

/**
* �o�ߎ��Ԃ��擾����.
*
* @return ���O��2���UpdateTimer()�Ăяo���̊ԂɌo�߂�������.
*
* @sa InitTimer, UpdateTimer
*/
double Window::DeltaTime() const
{
  return deltaTime;
}

/**
* ��Ԃ��X�V����.
*/
void Window::Update()
{
	UpdateGamePad();
  UpdateTimer();
  for (size_t i = 0; i < GLFW_KEY_LAST + 1; ++i) {
    const bool pressed = glfwGetKey(window, i) == GLFW_PRESS;
    if (pressed) {
      if (keyState[i] == KeyState::release) {
        keyState[i] = KeyState::press1st;
      } else if (keyState[i] == KeyState::press1st) {
        keyState[i] = KeyState::press;
      }
    } else if (keyState[i] != KeyState::release) {
      keyState[i] = KeyState::release;
    }
  }
}

/**
*
*/
glm::vec2 Window::GetMousePosition() const
{
  double x, y;
  glfwGetCursorPos(window, &x, &y);
  return glm::vec2(x, y);
}

/**
*
*/
int Window::GetMouseButton(int button) const
{
  return glfwGetMouseButton(window, button);
}


/*
* ���炩���ߐݒ肵�Ă������R�}���h�����͂���Ă��邩�ǂ���,�����o�^��
*
* �ǐ������ĉ��P������2/10
*/
bool Window::GetCommand(COMMANDLIST command)
{
	//����L�[
	if (command==COMMANDLIST_DECISION) {
		//����L�[�̏���,
		if (IsKeyDown(GLFW_KEY_A) || gamepad.buttonDown & GamePad::A) {
			
			return true;
		}
		else {
			return false;
		}
	}
	//�L�����Z���L�[
	else if (command == COMMANDLIST_CANCEL) {
		//�L�����Z���L�[�̏���,
		if (IsKeyDown(GLFW_KEY_ESCAPE) || gamepad.buttonDown & GamePad::B) {

			return true;
		}
		else {
			return false;
		}

	}
	//��L�[�������u��
	else if (command == COMMANDLIST_UP_1ST) {
		if (IsKeyDown(GLFW_KEY_UP) || gamepad.buttonDown & GamePad::DPAD_UP) {
			return true;
		}
		else {
			return false;
		}

	}
	//���L�[�������u��
	else if (command == COMMANDLIST_DOWN_1ST) {
		if (IsKeyDown(GLFW_KEY_DOWN) || gamepad.buttonDown & GamePad::DPAD_DOWN) {
			return true;
		}
		else {
			return false;
		}

	}
	//�E���͉������u��
	else if (command == COMMANDLIST_RIGHT_1ST) {
		if (IsKeyDown(GLFW_KEY_RIGHT) || gamepad.buttonDown & GamePad::DPAD_RIGHT) {
			return true;
		}
		else {
			return false;
		}

	}
	//�����͉������u��
	else if (command == COMMANDLIST_LEFT_1ST) {
		if (IsKeyDown(GLFW_KEY_LEFT) || gamepad.buttonDown & GamePad::DPAD_LEFT) {
			return true;
		}
		else {
			return false;
		}

	}
	//��L�[�����Ă����
	else if (command == COMMANDLIST_UP_PRESS) {
		if (IsKeyPressed(GLFW_KEY_UP) || gamepad.buttons & GamePad::DPAD_UP) {
			return true;
		}
		else {
			return false;
		}

	}
	//���L�[�����Ă����
	else if (command == COMMANDLIST_DOWN_PRESS) {
		if (IsKeyPressed(GLFW_KEY_DOWN) || gamepad.buttons & GamePad::DPAD_DOWN) {
			return true;
		}
		else {
			return false;
		}

	}
	//�E���͉���
	else if (command == COMMANDLIST_RIGHT_PRESS) {
		if (IsKeyPressed(GLFW_KEY_RIGHT) || gamepad.buttons & GamePad::DPAD_RIGHT) {
			return true;
		}
		else {
			return false;
		}

	}
	//�����͉����Ă����
	else if (command == COMMANDLIST_LEFT_PRESS) {
		if (IsKeyPressed(GLFW_KEY_LEFT) || gamepad.buttons & GamePad::DPAD_LEFT) {
			return true;
		}
		else {
			return false;
		}

	}
	//�v���C���̃u���[�L����.
	else if (command == COMMANDLIST_BRAKE) {
		if (IsKeyPressed(GLFW_KEY_SPACE) || gamepad.buttons & GamePad::A) {
			return true;
		}
		else {
			return false;
		}
	}
	//�ꎞ��~����
	else if (command == COMMANDLIST_PAUSE) {
	if (IsKeyDown(GLFW_KEY_ESCAPE) || gamepad.buttonDown & GamePad::ESC) {
		return true;
	}
	else {
		return false;
	}
	}
	//�ʐ^����.
	else if (command == COMMANDLIST_SNAP) {
	if (IsKeyDown(GLFW_KEY_S)) {
		return true;
	}
	else {
		return false;
	}
	}
	//�L���R�ꂪ���������̕ی��ŃR���\�[���ɕ\��,
	std::cout << "GLFWEW.cpp/GetCommand()�֐��̋L���R��;PleaseChack" << std::endl;
	return false;
	}

	/*
	* �W���C�X�e�B�b�N�̃A�i���O���͑��uID
	*
	* @note XBOX360�R���g���[���[�.
	*/
	enum GLFWAXESID {
		GLFWAXESID_LeftX,	///<���X�e�B�b�N��X��,
		GLFWAXESID_LeftY,	///<���X�e�B�b�N��Y��
		GLFWAXESID_BackX,	///<�A�i���O�g���K�[,
		GLFWAXESID_RightX,	///<�E�X�e�B�b�N��Y��
		GLFWAXESID_RightY,	///<�E�X�e�B�b�N��X��
	};
	/*
	* �W���C�X�e�B�b�N�̃f�W�^�����͑��uID
	*
	* @note XBOX360�R���g���[���[�.
	*/
	enum GLFWBUTTONID {
		GLFWBUTTONID_A,		///< A
		GLFWBUTTONID_B,		///< B
		GLFWBUTTONID_X,		///< X
		GLFWBUTTONID_Y,		///< Y
		GLFWBUTTONID_L,		///< L
		GLFWBUTTONID_R,		///< R
		GLFWBUTTONID_Back,	///< Back
		GLFWBUTTONID_Start,	///< Start
		GLFWBUTTONID_LThumb,///< ���X�e�B�b�Nbutton
		GLFWBUTTONID_RThumb,///< �E�X�e�B�b�Nbutton
		GLFWBUTTONID_Up,	///< ��L�[
		GLFWBUTTONID_Right,	///< �E�L�[
		GLFWBUTTONID_Down,	///< ���L�[
		GLFWBUTTONID_Left,	///< ���L�[
	};

	/**
	* �Q�[���p�b�h�̏�Ԃ��X�V����.
	*/
	void Window::UpdateGamePad()
	{
		const uint32_t prevButtons = gamepad.buttons; //�O�t���[���̃{�^����Ԃ�ۑ�.

		// �W���C�X�e�B�b�N���͂��擾���A�擾�����v�f��������Ă���΂�����g��.
		int axesCount, buttonCount;
		const float* axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &axesCount);
		const uint8_t* buttons = glfwGetJoystickButtons(GLFW_JOYSTICK_1, &buttonCount);
		if (axes && buttons && axesCount >= 2 && buttonCount >= 8) {
			gamepad.buttons &= ~(GamePad::DPAD_UP | GamePad::DPAD_DOWN
				| GamePad::DPAD_LEFT | GamePad::DPAD_RIGHT);
			static const float threshould = 0.3f;
			if (axes[GLFWAXESID_LeftY] >= threshould) {
				gamepad.buttons |= GamePad::DPAD_DOWN;
			}
			else if (axes[GLFWAXESID_LeftY] <= -threshould) {
				gamepad.buttons |= GamePad::DPAD_UP;
			}
			if (axes[GLFWAXESID_LeftX] >= threshould) {
				gamepad.buttons |= GamePad::DPAD_RIGHT;
			}
			else if (axes[GLFWAXESID_LeftX] <= -threshould) {
				gamepad.buttons |= GamePad::DPAD_LEFT;
			}
			static const struct {
				int glfwCode;
				uint32_t gamepadCode;
			} keyMap[] = {
			  { GLFWBUTTONID_A, GamePad::A },
			  { GLFWBUTTONID_B, GamePad::B },
			  { GLFWBUTTONID_X, GamePad::X },
			  { GLFWBUTTONID_Y, GamePad::Y },
			  { GLFWBUTTONID_L, GamePad::L },
			  { GLFWBUTTONID_R, GamePad::R },
			  { GLFWBUTTONID_Start, GamePad::START },
			};
			for (const auto& e : keyMap) {
				if (buttons[e.glfwCode] == GLFW_PRESS) {
					gamepad.buttons |= e.gamepadCode;
				}
				else if (buttons[e.glfwCode] == GLFW_RELEASE) {
					gamepad.buttons &= ~e.gamepadCode;
				}
			}
		}
		else {
			static const struct {
				int glfwCode;
				uint32_t gamepadCode;
			} keyMap[] = {
			  { GLFW_KEY_UP, GamePad::DPAD_UP },
			  { GLFW_KEY_DOWN, GamePad::DPAD_DOWN },
			  { GLFW_KEY_LEFT, GamePad::DPAD_LEFT },
			  { GLFW_KEY_RIGHT, GamePad::DPAD_RIGHT },
			  { GLFW_KEY_ENTER, GamePad::START },
			  { GLFW_KEY_A, GamePad::A },
			  { GLFW_KEY_S, GamePad::B },
			  { GLFW_KEY_Z, GamePad::X },
			  { GLFW_KEY_X, GamePad::Y },
			  { GLFW_KEY_LEFT_CONTROL, GamePad::L },
			  { GLFW_KEY_LEFT_SHIFT, GamePad::R },
			  { GLFW_KEY_ESCAPE, GamePad::ESC },
			};
			for (const auto& e : keyMap) {
				const int key = glfwGetKey(window, e.glfwCode);
				if (key == GLFW_PRESS) {
					gamepad.buttons |= e.gamepadCode;
				}
				else if (key == GLFW_RELEASE) {
					gamepad.buttons &= ~e.gamepadCode;
				}
			}
		}
		gamepad.buttonDown = gamepad.buttons & ~prevButtons;
	}


} // namespace GLFWEW
