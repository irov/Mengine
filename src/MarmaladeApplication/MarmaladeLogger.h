
#include "Interface/LogSystemInterface.h"

namespace Menge
{

	class MarmaladeLogger 
		: public LoggerInterface
	{
	public:
		MarmaladeLogger();
	public:
		void setVerboseLevel( EMessageLevel _level );
		bool validVerboseLevel( EMessageLevel _level ) const;
	public:
		void log( const char * _data, int _count, EMessageLevel _level );
		void flush( void );
	};

}