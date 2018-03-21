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
        void setVerboseFlag( size_t _flag ) override;

    public:
        bool validMessage( EMessageLevel _level, size_t _flag ) const override;

    protected:		
		HTTPSystemInterface * m_interface;

        EMessageLevel m_verboseLevel;
        size_t m_verboseFlag;
	};
}
