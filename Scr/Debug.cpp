#include "Debug.h"
#include <iostream>

namespace Debug {

	/* 
	* 要素を個別に表示
	*/
	void print(const glm::vec3& vec) {
		std::cout << "Debug::print. glm::vec3型の要素を表示する" << std::endl;
		std::cout << "vec3のx要素:" << vec.x << std::endl;
		std::cout << "vec3のy要素:" << vec.y << std::endl;
		std::cout << "vec3のz要素:" << vec.z << std::endl;
	}
	/*
	* 上になにか付け加えたい文章があれば,
	*/
	void print(std::string string,const glm::vec3& vec) {
		std::cout << string << std::endl;
		print(vec);
	}

	//mat4x4ver
	void print(const glm::mat4x4& mat4) {
		for (int j = 0; j < 4; ++j) {
			//x表示
			switch (j)
			{
			case 0:
				std::cout << "{";
				for (int i = 0; i < 4; ++i) {
					std::cout << mat4[i].x;
					if (i == 3) {
						std::cout << "}" << std::endl;
					}
					else {
						std::cout << " ";
					}
				}
				break;
			case 1:
				std::cout << "{";
				for (int i = 0; i < 4; ++i) {
					std::cout << mat4[i].y;
					if (i == 3) {
						std::cout << "}" << std::endl;
					}
					else {
						std::cout << " ";
					}
				}
				break;
			case 2:
				std::cout << "{";
				for (int i = 0; i < 4; ++i) {
					std::cout << mat4[i].z;
					if (i == 3) {
						std::cout << "}" << std::endl;
					}
					else {
						std::cout << " ";
					}
				}
				break;
			case 3:
				std::cout << "{";
				for (int i = 0; i < 4; ++i) {
					std::cout << mat4[i].w;

					if (i == 3) {
						std::cout << "}" << std::endl;
					}
					else {
						std::cout << " ";
					}
				}
				break;
			default:
				break;
			}

		}
		
	}

}