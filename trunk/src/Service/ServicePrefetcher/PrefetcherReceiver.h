#	include "Interface/PrefetcherInterface.h"

namespace Menge
{
	template<class T>
	class PrefetcherReceiver
		: public PrefetcherServiceInterface
	{
	public:
		PrefetcherReceiver()
			: m_refcount(0)
		{
		}

		~PrefetcherReceiver()
		{
		}

	public:
		void setReceiverData( const T & _receiverData )
		{

		}

		const T & getReceiverData() const
		{
			return m_receiverData;
		}

	public:
		void incref()
		{
			++m_refcount;
		}

		bool decref()
		{
			if( --m_refcount == 0 )
			{
				m_receiverData = nullptr;

				return true;
			}

			return false;
		}

	public:
		void onReceiverData( const T & _data )
		{
			m_receiverData = _data;
		}

	protected:
		size_t m_refcount;

		T m_receiverData;
	};
}