// @file Function.cpp


#include "Function.h"
#include <iostream>


namespace  Func{
	

	/*
	* ファイル名から拡張子(.???)を切り捨てた値を返す.
	* 文字列から '.' が検出されない場合はファイル名ではないと判断してエラー表示する.
	*
	* @param filename : ファイルの名前.
	*
	* @retval  : 拡張子を切り捨てた文字列
	*/
	std::string CutExtension(const std::string& filename) {
		//加工するstring変数を定義
		std::string newname = filename;

		int extPos = 0;			//拡張子'.'の位置
		int passPos = 0;		//ファイル'/'があるならば,位置
		bool found = false;		//'.'を見つけた場合true.
		//ファイル名の末尾から'.'と'/'を探索する.
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
		//'.'が見つからなかった場合はエラー表示
		if (!found) {
			std::cerr << "ERORR in Func::CutExtension: ファイル名ではない文字列を検出しました @:"<<filename << std::endl;

			return {};
		}

		//文字列から拡張子を取り除く
		newname.resize(extPos);

		//更にファイルパスである'/'又"../"以前を取り除くため '/'が存在しているならそれ以前を取り除く
		if (passPos) {
			newname = newname.substr(passPos,newname.size()-1);
		}

		return newname;
	}


	/*
	* ファイル名から拡張子を識別し,列挙型を返す
	*
	* @param filename : 調べるファイル名
	*
	* @retval :拡張子に対応する列挙型 該当しなければ NONE を返す
	*/
	Extension GetExtension(const std::string& filename)
	{

		int extPos = 0;				///< '.'があるポジション
		bool found = false;			///< 見つかったらtrueにする

		for (int i = filename.size(); i > 0;--i) {
			if (filename[i] == '.') {
				extPos = i;
				found = true;
				break;
			}
		}
		if (!found) {
			std::cerr << "ERORR in Func::GetExtension : ファイル名から拡張子を検出できませんでした" << std::endl;
			return Extension::NONE;
		}
		if (filename.substr(extPos, filename.size() - 1) == ".obj") {
			return Extension::OBJ;
		}
		else {
			std::cerr << "ERORR in Func::GetExtension : 列挙型に登録されていない識別子です." << std::endl;
			return Extension::NONE;
		}

	}

}