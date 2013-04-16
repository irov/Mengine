#	pragma once

#	include "Config/Typedef.h"

namespace Menge
{
	enum EAffectorType
	{
		ETA_POSITION,
		ETA_ANGLE,
		ETA_SCALE,
		ETA_COLOR,
		ETA_VISIBILITY,
        ETA_SCRIPT,
		ETA_END,
		__ETA_LAST__
	};

	const char * affectorToString( size_t _event );
}