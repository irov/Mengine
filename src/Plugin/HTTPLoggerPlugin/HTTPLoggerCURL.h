/*
 *	HTTPLoggerCURL.h
 *	Copyright 2011 Menge. All rights reserved.
 *
 */
#	pragma once
#	include "HTTPLogger.h"
namespace Menge
{
	
    class HTTPLoggerCURL
		: public HTTPLogger
        , public HTTPResponseReceiver
	{
	public:
		HTTPLoggerCURL( HTTPSystemInterface * _interfaceHTTP );
		virtual ~HTTPLoggerCURL();

	public:
		void log( EMessageLevel _level, size_t _flag, const char * _data, size_t _count ) override;

    public:
		void flush() override;
        void receive( const HTTPResponse & _response ) override;

	private:
		EMessageLevel m_verboseLevel;

		HTTPStringType m_buffer;
	};
}
