#	include "Kernel/AffectorType.h"

namespace Menge
{
	static const char* s_affectorToString[] = 
	{
		"ETA_POSITION",
		"ETA_ANGLE",
		"ETA_SCALE",
		"ETA_COLOR",
		"ETA_VISIBILITY",
		"ETA_END",
		"__ETA_LAST__"
	};

	const char * affectorToString( size_t _event )
	{
		return s_affectorToString[_event];
	}
}