// @file Shader.h

#ifndef SHADER_H_INCLUDED
#define SHADER_H_INCLUDED
#include <GL/glew.h>
#include <string>
#include <memory>

class UniformBuffer;

namespace Shader {
	GLuint CreateProgramFromFile(const char* vsFilename,const char* fsFilename);

	class Program;
	using ProgramPtr = std::shared_ptr<Program>;	///<プログラムオブジェクトポインタ.
	using ProgramRef = Program & ;	///参照型.
	/*
	* シェーダプログラム
	*/
	class Program
	{
	public:
		static ProgramPtr Create(const char* ,const char*);

		bool UniformBlockBinding(const char* ,GLuint);
		bool UniformBlockBinding(const UniformBuffer&);
		void UseProgram();
		void BindTexture(GLenum unit,GLenum type,GLuint texture);
		void DebugPring()const;
	private:
		Program() = default;
		~Program();
		Program(const ProgramRef) = delete;
		ProgramRef operator=(const ProgramRef) = delete;

		GLuint program = 0;					///<プログラムオブジェクト
		GLint samplerLocation = -1;			///<サンプラーの位置
		int samplerCount = 0;				///<サンプラーの数
		std::string name;					///<プログラム名.

	};

}//namespace Shader

#endif // !SHADER_H_INCLUDED
