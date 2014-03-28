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
		bool initialize( const InputStreamInterfacePtr & _stream ) override
		{
			m_stream = _stream;

			bool successful = this->_initialize();

			return successful;
		}

	public:
		void finalize() override
		{
			this->_finalize();

			m_stream = nullptr;			
		}

	protected:
		virtual bool _initialize()
		{
			return true;
		}

		virtual void _finalize()
		{
			//Empty
		}

	public:
		const InputStreamInterfacePtr & getStream() const override
		{
			return m_stream;				 
		}

	protected:
		ServiceProviderInterface * m_serviceProvider;
		InputStreamInterfacePtr m_stream;
	};    
}
