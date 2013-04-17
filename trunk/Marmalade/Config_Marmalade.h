#	pragma once

// Build-in particle system
#	define MENGE_PARTICLES	(1)

//	Endian type
#	define MENGE_ENDIAN_LITTLE	(1)
#	define MENGE_ENDIAN_BIG		(2)

#	define MENGE_ENDIAN		MENGE_ENDIAN_LITTLE

// Static Library
#	define MENGE_STATIC_LIB

//	exclude all dev stuff

#	define MENGE_DEBUG_HOTSPOTS		0x01
#	define MENGE_DEBUG_PHYSICS		0x02
#	define MENGE_DEBUG_NODES		0x04
#	define MENGE_DEBUG_SHADOWS		0x08
#	define MENGE_DEBUG_TILEPOLYGON	0x10

#	define MENGE_DEFAULT_BASE_DIR "..\\"
#	define MENGE_FOLDER_DELIM '\\'
#	define MENGE_FOLDER_RESOURCE_DELIM '/'
