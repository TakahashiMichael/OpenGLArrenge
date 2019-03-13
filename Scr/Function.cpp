// @file Function.cpp


#include "Function.h"
#include <iostream>


namespace  Func{
	

	/*
	* �t�@�C��������g���q(.???)��؂�̂Ă��l��Ԃ�.
	* �����񂩂� '.' �����o����Ȃ��ꍇ�̓t�@�C�����ł͂Ȃ��Ɣ��f���ăG���[�\������.
	*
	* @param filename : �t�@�C���̖��O.
	*
	* @retval  : �g���q��؂�̂Ă�������
	*/
	std::string CutExtension(const std::string& filename) {
		//���H����string�ϐ����`
		std::string newname = filename;

		int extPos = 0;			//�g���q'.'�̈ʒu
		int passPos = 0;		//�t�@�C��'/'������Ȃ��,�ʒu
		bool found = false;		//'.'���������ꍇtrue.
		//�t�@�C�����̖�������'.'��'/'��T������.
		for (int i = filename.size() - 1; i > 0; --i)
		{
			if (!found) {
				if (filename[i] == '.') {
					extPos = i;
					found = true;
				}
			}
			else if(filename[i]=='/'){
				passPos = i + 1;
				break;
			}
		}
		//'.'��������Ȃ������ꍇ�̓G���[�\��
		if (!found) {
			std::cerr << "ERORR in Func::CutExtension: �t�@�C�����ł͂Ȃ�����������o���܂��� @:"<<filename << std::endl;

			return {};
		}

		//�����񂩂�g���q����菜��
		newname.resize(extPos);

		//�X�Ƀt�@�C���p�X�ł���'/'��"../"�ȑO����菜������ '/'�����݂��Ă���Ȃ炻��ȑO����菜��
		if (passPos) {
			newname = newname.substr(passPos,newname.size()-1);
		}

		return newname;
	}


	/*
	* �t�@�C��������g���q�����ʂ�,�񋓌^��Ԃ�
	*
	* @param filename : ���ׂ�t�@�C����
	*
	* @retval :�g���q�ɑΉ�����񋓌^ �Y�����Ȃ���� NONE ��Ԃ�
	*/
	Extension GetExtension(const std::string& filename)
	{

		int extPos = 0;				///< '.'������|�W�V����
		bool found = false;			///< ����������true�ɂ���

		for (int i = filename.size(); i > 0;--i) {
			if (filename[i] == '.') {
				extPos = i;
				found = true;
				break;
			}
		}
		if (!found) {
			std::cerr << "ERORR in Func::GetExtension : �t�@�C��������g���q�����o�ł��܂���ł���" << std::endl;
			return Extension::NONE;
		}
		if (filename.substr(extPos, filename.size() - 1) == ".obj") {
			return Extension::OBJ;
		}
		else {
			std::cerr << "ERORR in Func::GetExtension : �񋓌^�ɓo�^����Ă��Ȃ����ʎq�ł�." << std::endl;
			return Extension::NONE;
		}

	}

}