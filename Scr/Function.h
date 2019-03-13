// @Function.h

#ifndef FUNCTION_H_INCLUDED
#define FUNCTION_H_INCLUDED

#include <string>

namespace Func {
	
	std::string CutExtension(const std::string&);
	enum class Extension {
		OBJ,
		NONE,
	};
	Extension GetExtension(const std::string&);
}


#endif // !FUNCTION_H_INCLUDED
