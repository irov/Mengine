#	pragma once

#	define MENGE_STATIC

#	ifndef MENGE_STATIC
#		ifdef MENGE_EXPORT
#			define MENGE_API __declspec(dllexport)
#		else
#			define MENGE_API __declspec(dllimport)
#		endif
#	else
#		define MENGE_API
#	endif

#	pragma warning ( disable : 4251 ) // needs to have dll-interface to be used by clients of class
