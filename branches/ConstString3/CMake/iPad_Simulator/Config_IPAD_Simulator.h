#	pragma once

// Build-in particle system
#	define MENGE_PARTICLES	(1)

//	Endian type
#	define MENGE_ENDIAN_LITTLE	(1)
#	define MENGE_ENDIAN_BIG		(2)

#	define MENGE_ENDIAN		MENGE_ENDIAN_LITTLE

// Static Library
#	define MENGE_STATIC_LIB

// Unicode
//#	define MENGE_UNICODE

// Compiler
#	define MENGE_COMPILER_MSVC	(1)

#if defined( _MSC_VER )
#   define MENGE_COMPILER MENGE_COMPILER_MSVC
#   define MENGE_COMP_VER _MSC_VER
#endif

#if MENGE_COMPILER == MENGE_COMPILER_MSVC
#   if MENGE_COMP_VER >= 1200
#       define FORCEINLINE __forceinline
#   endif
#elif defined(__MINGW32__)
#   if !defined(FORCEINLINE)
#       define FORCEINLINE __inline
#   endif
#else
#   define FORCEINLINE __inline
#endif

//	exclude all dev stuff

#	define MENGE_DEBUG_HOTSPOTS		0x01
#	define MENGE_DEBUG_PHYSICS		0x02
#	define MENGE_DEBUG_NODES		0x04
#	define MENGE_DEBUG_SHADOWS		0x08
#	define MENGE_DEBUG_TILEPOLYGON	0x10


#	define MENGE_DEFAULT_BASE_DIR L"data/"
#	define MENGE_FOLDER_DELIM L'/'
#	define MENGE_FOLDER_RESOURCE_DELIM L'/'