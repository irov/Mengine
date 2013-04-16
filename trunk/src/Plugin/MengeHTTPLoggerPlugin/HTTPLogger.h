#	pragma once

#	include "Interface/LogSystemInterface.h"
#	include "Interface/HTTPSystemInterface.h"

namespace Menge
{
	class HTTPLogger
		: public LoggerInterface
	{
    public:
		HTTPLogger( HTTPSystemInterface * _interfaceHTTP );
		virtual ~HTTPLogger();
	
    public:
		void setVerboseLevel( EMessageLevel _level ) override;
        bool validVerboseLevel( EMessageLevel _level ) const override;

    protected:		
		HTTPSystemInterface * m_interface;
        EMessageLevel m_verboseLevel;
	};
}
