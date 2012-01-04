/*
 *	HTTPLogger.h
 *	Copyright 2011 Menge. All rights reserved.
 *
 */
#	pragma once
#	include "Interface/LogSystemInterface.h"
#	include "Interface/HTTPSystemInterface.h"
namespace Menge
{
	class HTTPLogger
		: public LoggerInterface
	{
	public:
		enum EVerboseLevel
		{
			VL_SILENT = 0,
			VL_STDERR = 1,
			VL_MAX
		};
		HTTPLogger( HTTPSystemInterface * _interfaceHTTP );
		virtual ~HTTPLogger();
	
	public:
		virtual bool isValid() const;
		void setVerboseLevel( EVerboseLevel );
		EVerboseLevel getVerboseLevel( ) const;
		
	public:
		bool m_isValid;
	
	protected:
		void _debugMessage( const char * _message ) const;
		EVerboseLevel m_verboseLevel;
		HTTPSystemInterface * m_interface;
	};
}
