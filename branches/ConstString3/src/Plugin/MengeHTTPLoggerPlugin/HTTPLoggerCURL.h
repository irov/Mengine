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
		void log( const void* _data, int _count, EMessageLevel _level ) override;
		void flush() override;
        void receive( const HTTPResponse & _response ) override;

	private:
		HTTPStringType m_buffer;

	};
}
