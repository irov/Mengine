#	pragma once

#	include "Config/Typedef.h"

#	include "Interface/ServiceInterface.h"
#	include "Interface/NotificatorInterface.h"

#	include "Core/MemoryAllocator.h"

#	include "stdex/mpl.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	class ObserverInterface
		: public MemoryAllocator
	{
	public:
		ObserverInterface()
		{
		};
		virtual ~ObserverInterface()
		{
		};

	public:
		virtual void destroy() = 0;
	};
	//////////////////////////////////////////////////////////////////////////
	template<class M>
	class GeneratorObserverMethod
		: public ObserverInterface
	{
	};
	//////////////////////////////////////////////////////////////////////////
	class Observer0
		: public ObserverInterface
	{
	public:
		virtual void notify() = 0;
	};
	//////////////////////////////////////////////////////////////////////////
	template<class C, class M>
	class ObserverMethod0
		: public Observer0
	{
	public:
		ObserverMethod0( C * _self, M _method )
			: m_self( _self )
			, m_method( _method )
		{
		}

	protected:
		void notify() override
		{
			(m_self->*m_method)();
		}

	protected:
		C * m_self;
		M m_method;
	};
	//////////////////////////////////////////////////////////////////////////
	template<class C>
	class GeneratorObserverMethod<void (C::*)()>
		: public ObserverMethod0 < C, void (C::*)() >
	{
	public:
		GeneratorObserverMethod( C * _self, void (C::*_method)() )
			: ObserverMethod0<C, void (C::*)()>( _self, _method )
		{
		}

	protected:
		void destroy() override
		{
			delete this;
		}
	};
	//////////////////////////////////////////////////////////////////////////
	template<class P0>
	class Observer1
		: public ObserverInterface
	{
	public:
		virtual void notify( P0 _p0 ) = 0;
	};
	//////////////////////////////////////////////////////////////////////////
	template<class C, class M, class P0>
	class ObserverMethod1
		: public Observer1 < P0 >
	{
	public:
		ObserverMethod1( C * _self, M _method )
			: m_self( _self )
			, m_method( _method )
		{
		}

	protected:
		void notify( P0 _p0 ) override
		{
			(m_self->*m_method)(_p0);
		}

	protected:
		C * m_self;
		M m_method;
	};
	//////////////////////////////////////////////////////////////////////////
	template<class C, class P0>
	class GeneratorObserverMethod<void (C::*)(P0)>
		: public ObserverMethod1 < C, void (C::*)(P0), typename stdex::mpl::ref_attribute<P0>::type >
	{
	public:
		GeneratorObserverMethod( C * _self, void (C::*_method)(P0) )
			: ObserverMethod1<C, void (C::*)(P0), typename stdex::mpl::ref_attribute<P0>::type>( _self, _method )
		{
		}

	protected:
		void destroy() override
		{
			delete this;
		}
	};
	//////////////////////////////////////////////////////////////////////////
	template<class P0, class P1>
	class Observer2
		: public ObserverInterface
	{
	public:
		virtual void notify( P0 _p0, P1 _p1 ) = 0;
	};
	//////////////////////////////////////////////////////////////////////////
	template<class C, class M, class P0, class P1>
	class ObserverMethod2
		: public Observer2 < P0, P1 >
	{
	public:
		ObserverMethod2( C * _self, M _method )
			: m_self( _self )
			, m_method( _method )
		{
		}

	protected:
		void notify( P0 _p0, P1 _p1 ) override
		{
			(m_self->*m_method)(_p0, _p1);
		}

	protected:
		C * m_self;
		M m_method;
	};
	//////////////////////////////////////////////////////////////////////////
	template<class C, class P0, class P1>
	class GeneratorObserverMethod<void (C::*)(P0, P1)>
		: public ObserverMethod2 < C, void (C::*)(P0, P1), typename stdex::mpl::ref_attribute<P0>::type, typename stdex::mpl::ref_attribute<P1>::type>
	{
	public:
		GeneratorObserverMethod( C * _self, void (C::*_method)(P0, P1) )
			: ObserverMethod2<C, void (C::*)(P0, P1), typename stdex::mpl::ref_attribute<P0>::type, typename stdex::mpl::ref_attribute<P1>::type>( _self, _method )
		{
		}

	protected:
		void destroy() override
		{
			delete this;
		}
	};
	//////////////////////////////////////////////////////////////////////////
	class VisitorObserver
	{
	public:
		virtual bool visit( ObserverInterface * _observer ) = 0;
	};
	//////////////////////////////////////////////////////////////////////////
	class VisitorObserverCall0
		: public VisitorObserver
	{
	public:
		bool visit( ObserverInterface * _observer ) override
		{
			Observer0 * observer = dynamic_cast<Observer0 *>(_observer);

			if( observer == nullptr )
			{
				return false;
			}

			observer->notify();

			return true;
		}

	private:
		void operator = (const VisitorObserverCall0 &);
	};
	//////////////////////////////////////////////////////////////////////////
	template<class P0>
	class VisitorObserverCall1
		: public VisitorObserver
	{
	public:
		typedef typename stdex::mpl::ref_attribute<P0>::type A0;

	public:
		VisitorObserverCall1( P0 _p0 )
			: m_p0( _p0 )
		{
		}

	public:
		bool visit( ObserverInterface * _observer ) override
		{
			Observer1<A0> * observer = dynamic_cast<Observer1<A0> *>(_observer);

			if( observer == nullptr )
			{
				return false;
			}

			observer->notify( m_p0 );

			return true;
		}

	protected:
		P0 m_p0;

	private:
		void operator = (const VisitorObserverCall1 &);
	};
	//////////////////////////////////////////////////////////////////////////
	template<class P0, class P1>
	class VisitorObserverCall2
		: public VisitorObserver
	{
	public:
		typedef typename stdex::mpl::ref_attribute<P0>::type A0;
		typedef typename stdex::mpl::ref_attribute<P1>::type A1;

	public:
		VisitorObserverCall2( P0 _p0, P1 _p1 )
			: m_p0( _p0 )
			, m_p1( _p1 )
		{
		}

	public:
		bool visit( ObserverInterface * _observer ) override
		{
			Observer2<A0, A1> * observer = dynamic_cast<Observer2<A0, A1> *>(_observer);

			if( observer == nullptr )
			{
				return false;
			}

			observer->notify( m_p0, m_p1 );

			return true;
		}

	protected:
		P0 m_p0;
		P1 m_p1;

	private:
		void operator = (const VisitorObserverCall2 &);
	};
	//////////////////////////////////////////////////////////////////////////
	class NotificationServiceInterface
		: public ServiceInterface
	{
		SERVICE_DECLARE( "NotificationService" )

	public:
		virtual void addObserver( uint32_t _id, ObserverInterface * _observer ) = 0;
		virtual void removeObserver( uint32_t _id, ObserverInterface * _observer ) = 0;

	protected:
		virtual void visitObservers( uint32_t _id, VisitorObserver * _visitor ) = 0;

	public:
		template<class C, class M>
		inline ObserverInterface * addObserverMethod( uint32_t _id, C * _self, M _method )
		{
			ObserverInterface * observer =
				new GeneratorObserverMethod<M>( _self, _method );

			this->addObserver( _id, observer );

			return observer;
		}

	public:
		inline void notify( uint32_t _id )
		{
			VisitorObserverCall0 caller;

			this->visitObservers( _id, &caller );
		}

		template<class P0>
		inline void notify( uint32_t _id, const P0 & _p0 )
		{
			VisitorObserverCall1<P0> caller( _p0 );

			this->visitObservers( _id, &caller );
		}

		template<class P0, class P1>
		inline void notify( uint32_t _id, const P0 & _p0, const P1 & _p1 )
		{
			VisitorObserverCall2<P0, P1> caller( _p0, _p1 );

			this->visitObservers( _id, &caller );
		}
	};
	//////////////////////////////////////////////////////////////////////////
#   define NOTIFICATION_SERVICE( serviceProvider )\
    SERVICE_GET(serviceProvider, Menge::NotificationServiceInterface)
}