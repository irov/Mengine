#	pragma once

#	include "Config/Typedef.h"

#	include "Interface/ServiceInterface.h"

namespace Menge
{
	class Observer
	{
	public:
		virtual ~Observer(){};
	};

	template<class M>
	class GeneratorObserverMethod
        : public Observer
	{
	};

	class Observer0
		: public Observer
	{
	public:
		virtual void notify() = 0;
	};

	template<class C, class M>
	class ObserverMethod0
		: public Observer0
	{
	public:
		ObserverMethod0( C * _self, M _method )
			: m_self(_self)
			, m_method(_method)
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

	template<class C>
	class GeneratorObserverMethod<void (C::*)()>
		: public ObserverMethod0<C, void (C::*)()>
	{
	public:
		GeneratorObserverMethod( C * _self, void (C::*_method)() )
			: ObserverMethod0<C, void (C::*)()>(_self, _method)
		{
		}
	};

	template<class P0>
	class Observer1
		: public Observer
	{
	public:
		virtual void notify( P0 _p0 ) = 0;
	};

	template<class C, class M, class P0>
	class ObserverMethod1
		: public Observer1<P0>
	{
	public:
		ObserverMethod1( C * _self, M _method )
			: m_self(_self)
			, m_method(_method)
		{
		}

	protected:
		void notify( P0 _p0 ) override
		{
			(m_self->*m_method)( _p0 );
		}

	protected:
		C * m_self;
		M m_method;
	};
	
	template<class C, class P0>
	class GeneratorObserverMethod<void (C::*)(P0)>
		: public ObserverMethod1<C, void (C::*)(P0), P0>
	{
	public:
		GeneratorObserverMethod( C * _self, void (C::*_method)(P0) )
			: ObserverMethod1<C, void (C::*)(P0), P0>(_self, _method)
		{
		}
	};

	template<class P0, class P1>
	class Observer2
		: public Observer
	{
	public:
		virtual void notify( P0 _p0, P1 _p1 ) = 0;
	};

	template<class C, class M, class P0, class P1>
	class ObserverMethod2
		: public Observer2<P0, P1>
	{
	public:
		ObserverMethod2( C * _self, M _method )
			: m_self(_self)
			, m_method(_method)
		{
		}

	protected:
		void notify( P0 _p0, P1 _p1 ) override
		{
			(m_self->*m_method)( _p0, _p1 );
		}

	protected:
		C * m_self;
		M m_method;
	};

	template<class C, class P0, class P1>
	class GeneratorObserverMethod<void (C::*)(P0, P1)>
		: public ObserverMethod2<C, void (C::*)(P0, P1), P0, P1>
	{
	public:
		GeneratorObserverMethod( C * _self, void (C::*_method)(P0, P1) )
			: ObserverMethod2<C, void (C::*)(P0, P1), P0, P1>(_self, _method)
		{
		}
	};
	
	class VisitorObserver
	{
	public:
		virtual bool visit( Observer * _observer ) = 0;
	};

	class VisitorObserverCall0
		: public VisitorObserver
	{
	public:
		bool visit( Observer * _observer ) override
		{
			Observer0 * observer = dynamic_cast<Observer0 *>(_observer);

			if( observer == 0 )
			{
				return false;
			}

			observer->notify();

			return true;
		}
	};

	template<class P0>
	class VisitorObserverCall1
		: public VisitorObserver
	{
	public:
		VisitorObserverCall1( P0 _p0 )
			: m_p0(_p0)
		{
		}

	public:
		bool visit( Observer * _observer ) override
		{
			Observer1<P0> * observer = dynamic_cast<Observer1<P0> *>(_observer);

			if( observer == 0 )
			{
				return false;
			}

			observer->notify( m_p0 );

			return true;
		}

	protected:
		P0 m_p0;
	};

	template<class P0, class P1>
	class VisitorObserverCall2
		: public VisitorObserver
	{
	public:
		VisitorObserverCall2( P0 _p0, P1 _p1 )
			: m_p0(_p0)
			, m_p1(_p1)
		{
		}

	public:
		bool visit( Observer * _observer ) override
		{
			Observer2<P0, P1> * observer = dynamic_cast<Observer2<P0,P1> *>(_observer);

			if( observer == 0 )
			{
				return false;
			}

			observer->notify( m_p0, m_p1 );

			return true;
		}

	protected:
		P0 m_p0;
		P1 m_p1;
	};
	
	class NotificationServiceInterface
		: public ServiceInterface
	{
        SERVICE_DECLARE("NotificationService")

	public:
		virtual void addObserver( const String & _id, Observer * _observer ) = 0;
		virtual void removeObserver( const String & _id, Observer * _observer ) = 0;

	protected:		
		virtual void visitObservers( const String & _id, VisitorObserver * _visitor ) = 0;
		
	public:
		template<class C, class M>
		inline Observer * addObserverMethod( const String & _id, C * _self, M _method )
		{
			Observer * observer = 
				new GeneratorObserverMethod<M>(_self, _method);

			this->addObserver(_id, observer);

			return observer;
		}

	public:
		inline void notify( const String & _id )
		{
			VisitorObserverCall0 caller;

			this->visitObservers( _id, &caller );
		}
		
		template<class P0>
		inline void notify( const String & _id, P0 _p0 )
		{
			VisitorObserverCall1<P0> caller(_p0);

			this->visitObservers( _id, &caller );
		}

		template<class P0, class P1>
		inline void notify( const String & _id, P0 _p0, P1 _p1 )
		{
			VisitorObserverCall2<P0, P1> caller(_p0, _p1);

			this->visitObservers( _id, &caller );
		}	
	};	

#   define NOTIFICATION_SERVICE( serviceProvider )\
    (Menge::getService<Menge::NotificationServiceInterface>(serviceProvider))
}