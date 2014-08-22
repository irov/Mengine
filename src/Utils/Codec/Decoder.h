#	pragma once

#	include "Interface/ServiceInterface.h"
#	include "Interface/StreamInterface.h"

#	include "Logger/Logger.h"

namespace Menge
{
	template<class TDecoderInterface>
	class Decoder
		: public TDecoderInterface
	{
	public:
		Decoder()
			: m_serviceProvider(nullptr)
			, m_rewindPos(0)
			, m_initialize(false)
		{
		}

	public:
		void setServiceProvider( ServiceProviderInterface * _serviceProvider ) override
		{
			m_serviceProvider = _serviceProvider;
		}

		ServiceProviderInterface * getServiceProvider() const override
		{
			return m_serviceProvider;
		}

	public:
		bool initialize() override
		{
			if( m_initialize == true )
			{
				LOGGER_ERROR(m_serviceProvider)("Decoder::initialize: alredy initialize!"
					);

				return false;
			}

			m_initialize = this->_initialize();

			return m_initialize;
		}

	protected:
		virtual bool _initialize()
		{
			return true;
		}

	public:
		void finalize() override
		{
			if( m_initialize == false )
			{
				LOGGER_ERROR(m_serviceProvider)("Decoder::initialize: alredy finalize!"
					);

				return;
			}

			this->_finalize();

			m_stream = nullptr;
		}
		
	protected:
		virtual void _finalize()
		{
			//Empty
		}

	protected:
		void _destroy() override
		{
			this->finalize();
		}

	public:
		const InputStreamInterfacePtr & getStream() const override
		{
			return m_stream;				 
		}

	public:
		bool prepareData( const InputStreamInterfacePtr & _stream ) override
		{
			m_stream = _stream;

			bool successful = this->_prepareData();

			m_rewindPos = m_stream->tell();

			return successful;
		}

	public:
		void rewind() override
		{			
			this->_rewind();
		}

		virtual void _rewind()
		{
			m_stream->seek( m_rewindPos );
		}

	protected:
		virtual bool _prepareData()
		{
			return true;
		}

	protected:
		ServiceProviderInterface * m_serviceProvider;
		InputStreamInterfacePtr m_stream;

		size_t m_rewindPos;

		bool m_initialize;
	};    
}
