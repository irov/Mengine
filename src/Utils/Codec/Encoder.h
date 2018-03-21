#pragma once

#   include "Core/ServantBase.h"

namespace Mengine
{
	template<class TEncoderInterface>
	class Encoder
		: public ServantBase<TEncoderInterface>
	{
	public:
		Encoder()
			: m_initialize(false)
		{
		}

		~Encoder()
		{
			this->finalize();
		}

	public:
		bool initialize( const OutputStreamInterfacePtr & _stream ) override
		{
			if( m_initialize == true )
			{
				return false;
			}

			m_stream = _stream;

			m_initialize = this->_initialize();

			return m_initialize;
		}

		void finalize() override
		{
			if( m_initialize == false )
			{
				return;
			}

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
		OutputStreamInterfacePtr getStream() const override
		{
			return m_stream;
		}

	protected:
		OutputStreamInterfacePtr m_stream;

		bool m_initialize;
	};
}
