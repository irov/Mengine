#	pragma once

//	Target Render System //
#	define RS_D3D7	(1)
#	define RS_D3D9	(2)
#	define RS_OGL	(3)

#	define	RENDER_SYSTEM	RS_D3D9


// Save MiniDump on unhandled exception	//
#	define SAVE_DUMP	(1)

// Build-in particle system
#	define MENGE_PARTICLES	(1)

//	Target Platform
#	define MENGE_PLATFORM_WINDOWS		(1)
#	define MENGE_PLATFORM_APPLE			(2)
#	define MENGE_PLATFORM_LINUX			(3)

#	define MENGE_PLATFORM	MENGE_PLATFORM_WINDOWS

//	Endian type
#	define MENGE_ENDIAN_LITTLE	(1)
#	define MENGE_ENDIAN_BIG		(2)

#	define MENGE_ENDIAN		MENGE_ENDIAN_LITTLE


// Static Library
#	define MENGE_STATIC_LIB

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
//#	define MENGE_MASTER_RELEASE