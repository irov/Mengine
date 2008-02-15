#	pragma once

#	ifdef MENGE_EXPORT
#	define MENGE_API __declspec(dllexport)
#	else
#	define MENGE_API __declspec(dllimport)
#	endif

#	pragma warning ( disable : 4251 ) // needs to have dll-interface to be used by clients of class
