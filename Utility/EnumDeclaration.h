#	pragma once

#	include "boost/preprocessor/seq.hpp"
#	include "boost/preprocessor/stringize.hpp"

#	include "utility/preprocessor/seq.h"

#	define FUNCTION_ENUM_SWITCH_CASE(r, data, i, elem)\
	case elem: return BOOST_PP_STRINGIZE(elem);

#	define ENUM_DECLARATION( ENUM, PARAM )\
	enum ENUM\
	{\
	IROV_PP_SEQ_ENUM(PARAM)\
	};\
	namespace EnumWrapper\
	{\
	const char * stringize( ENUM elem)\
		{\
		switch(elem)\
			{\
			BOOST_PP_SEQ_FOR_EACH_I(FUNCTION_ENUM_SWITCH_CASE, _, PARAM)\
			default: return "<invalid>";\
			}\
		}\
	}

#	define ENUM_STRINGIZE EnumWrapper::stringize

