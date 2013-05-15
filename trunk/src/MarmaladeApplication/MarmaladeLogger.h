
#include "Interface/LogSystemInterface.h"

namespace Menge
{
	class MarmaladeLogger 
		: public LoggerInterface
	{
	public:
		MarmaladeLogger();

	public:
		void setVerboseLevel( EMessageLevel _level ) override;
        void setVerboseFlag( size_t _flag ) override;

		bool validMessage( EMessageLevel _level, size_t _flag ) const override;

	public:
		void log( EMessageLevel _level, size_t _flag, const char * _data, size_t _count ) override;
		void flush() override;
	};
}