#	pragma once

namespace Menge
{
	template<class TDecoderInterface>
	class Decoder
		: public TDecoderInterface
	{
	public:
		Decoder()
			: m_serviceProvider(nullptr)
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
			bool successful = this->_initialize();

			return successful;
		}

	protected:
		virtual bool _initialize()
		{
			return true;
		}

	public:
		void finalize() override
		{
			this->_finalize();

			m_stream = nullptr;
		}
		
	protected:
		virtual void _finalize()
		{
			//Empty
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

			return successful;
		}

	protected:
		virtual bool _prepareData()
		{
			return true;
		}

	protected:
		ServiceProviderInterface * m_serviceProvider;
		InputStreamInterfacePtr m_stream;
	};    
}
