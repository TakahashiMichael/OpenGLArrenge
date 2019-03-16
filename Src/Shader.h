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
	using ProgramPtr = std::shared_ptr<Program>;	///<�v���O�����I�u�W�F�N�g�|�C���^.
	using ProgramRef = Program & ;	///�Q�ƌ^.
	/*
	* �V�F�[�_�v���O����
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

		GLuint program = 0;					///<�v���O�����I�u�W�F�N�g
		GLint samplerLocation = -1;			///<�T���v���[�̈ʒu
		int samplerCount = 0;				///<�T���v���[�̐�
		std::string name;					///<�v���O������.

	};

}//namespace Shader

#endif // !SHADER_H_INCLUDED
