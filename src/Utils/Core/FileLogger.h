#pragma once

#include "Interface/LoggerInterface.h"
#include "Interface/StreamInterface.h"

#include "Core/ServantBase.h"

namespace Mengine
{
	class FileLogger
		: public ServantBase<LoggerInterface>
	{
	public:
		FileLogger();
		~FileLogger();

	public:
		bool initialize() override;
		void finalize() override;
		
	public:
		void setVerboseLevel( EMessageLevel _level ) override;
        void setVerboseFlag( uint32_t _flag ) override;

    public:
		bool validMessage( EMessageLevel _level, uint32_t _flag ) const override;

	public:
		OutputStreamInterfacePtr getStream() const;

	protected:
		void log( EMessageLevel _level, uint32_t _flag, const char * _data, size_t _count ) override;

    protected:
		void flush() override;

	protected:
		EMessageLevel m_verboseLevel;
        uint32_t m_verboseFlag;

		OutputStreamInterfacePtr m_stream;
	};
}
