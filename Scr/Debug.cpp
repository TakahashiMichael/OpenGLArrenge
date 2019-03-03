#include "Debug.h"
#include <iostream>

namespace Debug {

	/* 
	* —v‘f‚ðŒÂ•Ê‚É•\Ž¦
	*/
	void print(const glm::vec3& vec) {
		std::cout << "Debug::print. glm::vec3Œ^‚Ì—v‘f‚ð•\Ž¦‚·‚é" << std::endl;
		std::cout << "vec3‚Ìx—v‘f:" << vec.x << std::endl;
		std::cout << "vec3‚Ìy—v‘f:" << vec.y << std::endl;
		std::cout << "vec3‚Ìz—v‘f:" << vec.z << std::endl;
	}
	/*
	* ã‚É‚È‚É‚©•t‚¯‰Á‚¦‚½‚¢•¶Í‚ª‚ ‚ê‚Î,
	*/
	void print(std::string string,const glm::vec3& vec) {
		std::cout << string << std::endl;
		print(vec);
	}

	//mat4x4ver
	void print(const glm::mat4x4& mat4) {
		for (int j = 0; j < 4; ++j) {
			//x•\Ž¦
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