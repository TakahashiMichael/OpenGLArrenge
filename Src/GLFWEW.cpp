/**
* @file GLFWE.cpp
*/
#include "GLFWEW.h"
#include <iostream>

/// GLFWとGLEWをラップするための名前空間.
namespace GLFWEW {

void APIENTRY OutputGLDebugMessage(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, GLvoid *userParam)
{
  std::cerr << message << "\n";
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

/**
* シングルトンインスタンスを取得する.
*
* @return Windowのシングルトンインスタンス.
*/
Window& Window::Instance()
{
	static Window instance;
	return instance;
}

/**
* コンストラクタ.
*/
Window::Window()
{
}

/**
* デストラクタ.
*/
Window::~Window()
{
	if (isGLFWInitialized) {
		glfwTerminate();
	}
}

/**
* GLFW/GLEWの初期化.
*
* @param w ウィンドウの描画範囲の幅(ピクセル).
* @param h ウィンドウの描画範囲の高さ(ピクセル).
* @param title ウィンドウタイトル(UTF-8の0終端文字列).
*
* @retval true 初期化成功.
* @retval false 初期化失敗.
*/
bool Window::Init(int w, int h, const char* title)
{
	if (isInitialized) {
		std::cerr << "ERROR: GLFWEWは既に初期化されています." << std::endl;
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
		//glfwGetPrimaryMonitor();フルスクリーンポインタ取得関数
		window = glfwCreateWindow(w, h, title, nullptr, nullptr);


		if (!window) {
			return false;
		}
		glfwMakeContextCurrent(window);
    }

	if (glewInit() != GLEW_OK) {
		std::cerr << "ERROR: GLEWの初期化に失敗しました." << std::endl;
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
* ウィンドウを閉じるべきか調べる.
*
* @retval true 閉じる.
* @retval false 閉じない.
*/
bool Window::ShouldClose() const
{
	return (glfwWindowShouldClose(window) != 0)||(this->exit);
}

/**
* フロントバッファとバックバッファを切り替える.
*/
void Window::SwapBuffers() const
{
	glfwPollEvents();
	glfwSwapBuffers(window);
}

/**
* キーが押されているか調べる.
*
* @param key 調べるキーのID(GLFW_KEY_Aなど).
*
* @retval true  キーが押されている.
* @retval false キーが押されていない.
*/
bool Window::IsKeyPressed(int key) const
{
  if (key < 0 || key >= GLFW_KEY_LAST + 1) {
    return false;
  }
  return keyState[key] != KeyState::release;
}

/**
* キーが押された瞬間か調べる.
*
* @param key 調べるキーのID(GLFW_KEY_Aなど).
*
* @retval true  キーが押された瞬間.
* @retval false キーが押された瞬間ではない.
*/
bool Window::IsKeyDown(int key) const
{
  if (key < 0 || key >= GLFW_KEY_LAST + 1) {
    return false;
  }
  return keyState[key] == KeyState::press1st;
}

/*
* ゲームパッドの状態を取得する.
*
* @return ゲームパッドの状態.
*/
const GamePad& Window::GetGamePad()const
{
	return gamepad;
}


/**
* タイマーを初期化する.
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
* タイマーを更新する.
*
* @sa InitTimer, GetDeltaTime
*/
void Window::UpdateTimer()
{
  // 経過時間を計測.
  const double currentTime = glfwGetTime();
  
  deltaTime = currentTime - previousTime;
  while (deltaTime * 60 < 1.0f)
  {
	  const double currentTime = glfwGetTime();

	  deltaTime = currentTime - previousTime;

  }
  previousTime = currentTime;

  // 経過時間が長くなりすぎないように調整.
  const float upperLimit = 0.25f; // 経過時間として許容される上限.
  if (deltaTime > upperLimit) {
    deltaTime = 0.1f;
  }
  std::cout << "DeltaTimeを表示する:" << deltaTime*60 << std::endl;
}

/**
* 経過時間を取得する.
*
* @return 直前の2回のUpdateTimer()呼び出しの間に経過した時間.
*
* @sa InitTimer, UpdateTimer
*/
double Window::DeltaTime() const
{
  return deltaTime;
}

/**
* 状態を更新する.
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
* あらかじめ設定しておいたコマンドが入力されているかどうか,複数登録可
*
* 可読性悪くて改善したい2/10
*/
bool Window::GetCommand(COMMANDLIST command)
{
	//決定キー
	if (command==COMMANDLIST_DECISION) {
		//決定キーの処理,
		if (IsKeyDown(GLFW_KEY_A) || gamepad.buttonDown & GamePad::A) {
			
			return true;
		}
		else {
			return false;
		}
	}
	//キャンセルキー
	else if (command == COMMANDLIST_CANCEL) {
		//キャンセルキーの処理,
		if (IsKeyDown(GLFW_KEY_ESCAPE) || gamepad.buttonDown & GamePad::B) {

			return true;
		}
		else {
			return false;
		}

	}
	//上キー押した瞬間
	else if (command == COMMANDLIST_UP_1ST) {
		if (IsKeyDown(GLFW_KEY_UP) || gamepad.buttonDown & GamePad::DPAD_UP) {
			return true;
		}
		else {
			return false;
		}

	}
	//下キー押した瞬間
	else if (command == COMMANDLIST_DOWN_1ST) {
		if (IsKeyDown(GLFW_KEY_DOWN) || gamepad.buttonDown & GamePad::DPAD_DOWN) {
			return true;
		}
		else {
			return false;
		}

	}
	//右入力押した瞬間
	else if (command == COMMANDLIST_RIGHT_1ST) {
		if (IsKeyDown(GLFW_KEY_RIGHT) || gamepad.buttonDown & GamePad::DPAD_RIGHT) {
			return true;
		}
		else {
			return false;
		}

	}
	//左入力押した瞬間
	else if (command == COMMANDLIST_LEFT_1ST) {
		if (IsKeyDown(GLFW_KEY_LEFT) || gamepad.buttonDown & GamePad::DPAD_LEFT) {
			return true;
		}
		else {
			return false;
		}

	}
	//上キー押している間
	else if (command == COMMANDLIST_UP_PRESS) {
		if (IsKeyPressed(GLFW_KEY_UP) || gamepad.buttons & GamePad::DPAD_UP) {
			return true;
		}
		else {
			return false;
		}

	}
	//下キー押している間
	else if (command == COMMANDLIST_DOWN_PRESS) {
		if (IsKeyPressed(GLFW_KEY_DOWN) || gamepad.buttons & GamePad::DPAD_DOWN) {
			return true;
		}
		else {
			return false;
		}

	}
	//右入力押し
	else if (command == COMMANDLIST_RIGHT_PRESS) {
		if (IsKeyPressed(GLFW_KEY_RIGHT) || gamepad.buttons & GamePad::DPAD_RIGHT) {
			return true;
		}
		else {
			return false;
		}

	}
	//左入力押している間
	else if (command == COMMANDLIST_LEFT_PRESS) {
		if (IsKeyPressed(GLFW_KEY_LEFT) || gamepad.buttons & GamePad::DPAD_LEFT) {
			return true;
		}
		else {
			return false;
		}

	}
	//プレイ中のブレーキ処理.
	else if (command == COMMANDLIST_BRAKE) {
		if (IsKeyPressed(GLFW_KEY_SPACE) || gamepad.buttons & GamePad::A) {
			return true;
		}
		else {
			return false;
		}
	}
	//一時停止処理
	else if (command == COMMANDLIST_PAUSE) {
	if (IsKeyDown(GLFW_KEY_ESCAPE) || gamepad.buttonDown & GamePad::ESC) {
		return true;
	}
	else {
		return false;
	}
	}
	//写真処理.
	else if (command == COMMANDLIST_SNAP) {
	if (IsKeyDown(GLFW_KEY_S)) {
		return true;
	}
	else {
		return false;
	}
	}
	//記入漏れがあった時の保険でコンソールに表示,
	std::cout << "GLFWEW.cpp/GetCommand()関数の記入漏れ;PleaseChack" << std::endl;
	return false;
	}

	/*
	* ジョイスティックのアナログ入力装置ID
	*
	* @note XBOX360コントローラー基準.
	*/
	enum GLFWAXESID {
		GLFWAXESID_LeftX,	///<左スティックのX軸,
		GLFWAXESID_LeftY,	///<左スティックのY軸
		GLFWAXESID_BackX,	///<アナログトリガー,
		GLFWAXESID_RightX,	///<右スティックのY軸
		GLFWAXESID_RightY,	///<右スティックのX軸
	};
	/*
	* ジョイスティックのデジタル入力装置ID
	*
	* @note XBOX360コントローラー基準.
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
		GLFWBUTTONID_LThumb,///< 左スティックbutton
		GLFWBUTTONID_RThumb,///< 右スティックbutton
		GLFWBUTTONID_Up,	///< 上キー
		GLFWBUTTONID_Right,	///< 右キー
		GLFWBUTTONID_Down,	///< 下キー
		GLFWBUTTONID_Left,	///< 左キー
	};

	/**
	* ゲームパッドの状態を更新する.
	*/
	void Window::UpdateGamePad()
	{
		const uint32_t prevButtons = gamepad.buttons; //前フレームのボタン状態を保存.

		// ジョイスティック入力を取得し、取得した要素数が足りていればそれを使う.
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
