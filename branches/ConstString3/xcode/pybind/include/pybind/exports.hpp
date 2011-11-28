#	pragma once

#	include "config/config.hpp"

#	ifndef PYBIND_STATIC
	#	ifdef PYBIND_EXPORTS
		#	define PYBIND_API __declspec(dllexport)
	#	else
		#	define PYBIND_API __declspec(dllimport)
	#	endif
#	else
	#	define PYBIND_API
#	endif


#	pragma warning ( disable : 4251 ) // needs to have dll-interface to be used by clients of class

