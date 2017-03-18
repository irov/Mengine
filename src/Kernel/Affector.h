#	pragma once

#	include "Kernel/AffectorType.h"

#	include "Kernel/Scriptable.h"

#   include "Factory/Factorable.h"
#   include "Factory/FactoryPool.h"

#	include "Core/ValueInterpolator.h"

#   include "stdex/intrusive_slug_linked.h"

namespace Menge
{
	//////////////////////////////////////////////////////////////////////////
	class ServiceProviderInterface;
	//////////////////////////////////////////////////////////////////////////
	class AffectorCallback
		: public FactorablePtr
	{
	public:
		virtual void onAffectorEnd( AFFECTOR_ID _id, bool _isEnd ) = 0;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef stdex::intrusive_ptr<AffectorCallback> AffectorCallbackPtr;
	//////////////////////////////////////////////////////////////////////////
	class Affector
		: public stdex::intrusive_slug_linked<Affector>
		, public Factorable
	{
	public:
		Affector();
		virtual ~Affector();

	public:
		void setServiceProvider( ServiceProviderInterface * _serviceProvider );
		ServiceProviderInterface * getServiceProvider() const;

	public:
		void setAffectorType( EAffectorType _type );
		EAffectorType getAffectorType() const;

	public:
		void setId( AFFECTOR_ID _id );
		AFFECTOR_ID getId() const;

	public:
		void setFreeze( bool _value );
		bool getFreeze() const;

	protected:
		virtual void _setFreeze( bool _value );

	public:
		void setSpeedFactor( float _speed );
		float getSpeedFactor() const;

	public:
		bool prepare();

	protected:
		virtual bool _prepare();

	public:
		virtual bool affect( float _timing );

	protected:
		virtual bool _affect( float _timing ) = 0;

	public:
		virtual void complete() = 0;
		virtual void stop() = 0;

	protected:
		ServiceProviderInterface * m_serviceProvider;

		EAffectorType m_type;
		AFFECTOR_ID m_id;

		float m_speedFactor;

		bool m_freeze;
	};
	//////////////////////////////////////////////////////////////////////////
	class CallbackAffector
		: public Affector
	{
	public:
		CallbackAffector();
		~CallbackAffector();

	public:
		void setCallback( const AffectorCallbackPtr & _cb );
		const AffectorCallbackPtr & getCallback() const;

	protected:
		void complete() override;

	protected:
		void end_( bool _isEnd );

	protected:
		AffectorCallbackPtr m_cb;
	};
	//////////////////////////////////////////////////////////////////////////
	template<class C, class M, class MA>
	class MemeberAffector
		: public CallbackAffector
	{
	public:
		MemeberAffector()
			: m_self( nullptr )
			, m_method( nullptr )
		{
		}

		void setup( C * _self, M _method, const MA & _argument )
		{
			m_self = _self;
			m_method = _method;
			m_argument = _argument;
		}

		template<class T>
		void update( const T & _value )
		{
			(m_self->*m_method)(m_argument, _value);
		}

	protected:
		C * m_self;
		M m_method;
		MA m_argument;
	};
	//////////////////////////////////////////////////////////////////////////
	template<class C, class M>
	class MemeberAffector<C, M, void>
		: public CallbackAffector
	{
	public:
		MemeberAffector()
			: m_self( nullptr )
			, m_method( nullptr )
		{
		}

		void setup( C * _self, M _method )
		{
			m_self = _self;
			m_method = _method;
		}

		template<class T>
		void update( const T & _value )
		{
			(m_self->*m_method)(_value);
		}

	protected:
		C * m_self;
		M m_method;
	};
	//////////////////////////////////////////////////////////////////////////
	template<class C, class M, class MA, class T, template<class> class Accumulator>
	class MemberAffectorAccumulate
		: public MemeberAffector<C, M, MA>
	{
	protected:
		bool _affect( float _timing ) override
		{
			T value;
			bool finish = m_accumulator.update( _timing, &value );

			this->update( value );

			if( finish == false )
			{
				return false;
			}

			return true;
		}

		void stop() override
		{
			m_accumulator.stop();

			this->end_( false );
		}

	protected:
		Accumulator<T> m_accumulator;
	};
	//////////////////////////////////////////////////////////////////////////
	template<class C, class M, class MA, class T, class Interpolator>
	class MemberAffectorInterpolate
		: public MemeberAffector<C, M, MA>
	{
	protected:
		bool _affect( float _timing ) override
		{
			T value;
			bool finish = m_interpolator.update( _timing, &value );

			this->update( value );

			if( finish == false )
			{
				return false;
			}

			return true;
		}

		void stop() override
		{
			m_interpolator.stop();

			this->end_( false );
		}

	protected:
		Interpolator m_interpolator;
	};
	//////////////////////////////////////////////////////////////////////////
	template<class C, class M, class T, class MA>
	class MemberAffectorAccumulateLinear
		: public MemberAffectorAccumulate<C, M, MA, T, ValueAccumulateLinear>
	{
	public:
		bool initialize( C * _self, M _method, const MA & _argument, const T & _start, const T & _dir, float _speed )
		{
			MemberAffectorAccumulate<C, M, MA, T, ValueAccumulateLinear>::setup( _self, _method, _argument );

			if( MemberAffectorAccumulate<C, M, MA, T, ValueAccumulateLinear>::m_accumulator.start( _start, _dir, _speed ) == false )
			{
				return false;
			}

			return true;
		}
	};
	//////////////////////////////////////////////////////////////////////////
	template<class C, class M, class T>
	class MemberAffectorAccumulateLinear<C, M, T, void>
		: public MemberAffectorAccumulate<C, M, void, T, ValueAccumulateLinear>
	{
	public:
		bool initialize( C * _self, M _method, const T & _start, const T & _dir, float _speed )
		{
			MemberAffectorAccumulate<C, M, void, T, ValueAccumulateLinear>::setup( _self, _method );

			if( MemberAffectorAccumulate<C, M, void, T, ValueAccumulateLinear>::m_accumulator.start( _start, _dir, _speed ) == false )
			{
				return false;
			}

			return true;
		}
	};
	//////////////////////////////////////////////////////////////////////////
	template<class C, class M, class T, class MA>
	class MemberAffectorInterpolateLinear
		: public MemberAffectorInterpolate<C, M, MA, T, ValueInterpolatorLinear<T> >
	{
	public:
		bool initialize( C * _self, M _method, const MA & _argument, const T & _start, const T & _end, float _time )
		{
			MemberAffectorInterpolate<C, M, MA, T, ValueInterpolatorLinear<T> >::setup( _self, _method, _argument );

			if( MemberAffectorInterpolate<C, M, MA, T, ValueInterpolatorLinear<T> >::m_interpolator.start( _start, _end, _time ) == false )
			{
				return false;
			}

			return true;
		}
	};
	//////////////////////////////////////////////////////////////////////////
	template<class C, class M, class T>
	class MemberAffectorInterpolateLinear<C, M, T, void>
		: public MemberAffectorInterpolate<C, M, void, T, ValueInterpolatorLinear<T> >
	{
	public:
		bool initialize( C * _self, M _method, const T & _start, const T & _end, float _time )
		{
			MemberAffectorInterpolate<C, M, void, T, ValueInterpolatorLinear<T> >::setup( _self, _method );

			if( MemberAffectorInterpolate<C, M, void, T, ValueInterpolatorLinear<T> >::m_interpolator.start( _start, _end, _time ) == false )
			{
				return false;
			}

			return true;
		}
	};
	//////////////////////////////////////////////////////////////////////////
	template<class C, class M, class T>
	class MemberAffectorInterpolateQuadratic
		: public MemberAffectorInterpolate<C, M, void, T, ValueInterpolatorQuadratic<T> >
	{
	public:
		bool initialize( C * _self, M _method, const T & _start, const T & _end, const T & _v0, float _time )
		{
			MemberAffectorInterpolate<C, M, void, T, ValueInterpolatorQuadratic<T> >::setup( _self, _method );

			if( MemberAffectorInterpolate<C, M, void, T, ValueInterpolatorQuadratic<T> >::m_interpolator.start( _start, _end, _v0, _time ) == false )
			{
				return false;
			}

			return true;
		}
	};
	//////////////////////////////////////////////////////////////////////////
	template<class C, class M, class T, size_t N>
	class MemberAffectorInterpolateBezier
		: public MemberAffectorInterpolate<C, M, void, T, ValueInterpolatorBezier<T, N> >
	{
	public:
		bool initialize( C * _self, M _method, const T & _start, const T & _end, const T * _v, float _time )
		{
			MemberAffectorInterpolate<C, M, void, T, ValueInterpolatorBezier<T, N> >::setup( _self, _method );

			if( MemberAffectorInterpolate<C, M, void, T, ValueInterpolatorBezier<T, N> >::m_interpolator.start( _start, _end, _v, _time ) == false )
			{
				return false;
			}

			return true;
		}
	};
	//////////////////////////////////////////////////////////////////////////
	namespace NodeAffectorCreator
	{
		template<class C, class M, class T>
		class NodeAffectorCreatorAccumulateLinear
		{
		public:
			typedef MemberAffectorAccumulateLinear<C, M, T, void> AffectorType;

        public:
            NodeAffectorCreatorAccumulateLinear( ServiceProviderInterface * _serviceProvider )
            {
                m_factory = new FactoryPool<AffectorType, 4>( _serviceProvider );
            }

		public:
			Affector * create( EAffectorType _type, const AffectorCallbackPtr & _cb
				, C * _self, M _method
				, const T & _pos, const T & _dir, float _speed )
			{
				AffectorType * affector = m_factory->createObject();

				affector->setServiceProvider( m_serviceProvider );
				affector->setAffectorType( _type );

				affector->setCallback( _cb );

				if( affector->initialize( _self, _method, _pos, _dir, _speed ) == false )
				{
					return nullptr;
				}

				affector->update( _pos );

				return affector;
			}

        protected:
            ServiceProviderInterface * m_serviceProvider;

			FactoryPtr m_factory;
		};
		//////////////////////////////////////////////////////////////////////////
		template<class C, class M, class T, class MA = void>
		class NodeAffectorCreatorInterpolateLinear
		{
		public:
			typedef MemberAffectorInterpolateLinear<C, M, T, MA> AffectorType;

        public:
            NodeAffectorCreatorInterpolateLinear( ServiceProviderInterface * _serviceProvider )
                : m_serviceProvider( _serviceProvider )
            {
                m_factory = new FactoryPool<AffectorType, 4>( m_serviceProvider );
            }

		public:
			Affector * create( EAffectorType _type, const AffectorCallbackPtr & _cb
				, C * _self, M _method, const MA & _argument
				, const T & _start, const T & _end, float _time )
			{
				AffectorType * affector = m_factory->createObject();

				affector->setServiceProvider( m_serviceProvider );
				affector->setAffectorType( _type );

				affector->setCallback( _cb );

				if( affector->initialize( _self, _method, _argument, _start, _end, _time ) == false )
				{
					return nullptr;
				}

				affector->update( _start );

				return affector;
			}

		protected:
            ServiceProviderInterface * m_serviceProvider;

            FactoryPtr m_factory;
		};
		//////////////////////////////////////////////////////////////////////////
		template<class C, class M, class T>
		class NodeAffectorCreatorInterpolateLinear<C, M, T, void>
		{
		public:
			typedef MemberAffectorInterpolateLinear<C, M, T, void> AffectorType;

        public:
            NodeAffectorCreatorInterpolateLinear( ServiceProviderInterface * _serviceProvider )
                : m_serviceProvider( _serviceProvider )
            {
                m_factory = new FactoryPool<AffectorType, 4>( m_serviceProvider );
            }                       
            
		public:
			Affector * create( EAffectorType _type, const AffectorCallbackPtr & _cb
				, C * _self, M _method
				, const T & _start, const T & _end, float _time )
			{
				AffectorType * affector = m_factory->createObject();

				affector->setServiceProvider( m_serviceProvider );
				affector->setAffectorType( _type );

				affector->setCallback( _cb );

				if( affector->initialize( _self, _method, _start, _end, _time ) == false )
				{
					return nullptr;
				}

				affector->update( _start );

				return affector;
			}

		protected:
            ServiceProviderInterface * m_serviceProvider;

			FactoryPtr m_factory;
		};
		//////////////////////////////////////////////////////////////////////////
		template<class C, class M, class T>
		class NodeAffectorCreatorInterpolateQuadratic
		{
		public:
			typedef MemberAffectorInterpolateQuadratic<C, M, T> AffectorType;

        public:
            NodeAffectorCreatorInterpolateQuadratic( ServiceProviderInterface * _serviceProvider )
                : m_serviceProvider( _serviceProvider )
            {
                m_factory = new FactoryPool<AffectorType, 4>( m_serviceProvider );
            }

		public:
			Affector * create( EAffectorType _type, const AffectorCallbackPtr & _cb
				, C * _self, M _method
				, const T & _start, const T & _end, const T & _v0, float _time )
			{
				AffectorType * affector = m_factory->createObject();

				affector->setServiceProvider( m_serviceProvider );
				affector->setAffectorType( _type );

				affector->setCallback( _cb );

				if( affector->initialize( _self, _method, _start, _end, _v0, _time ) == false )
				{
					return nullptr;
				}

				affector->update( _start );

				return affector;
			}

		protected:
            ServiceProviderInterface * m_serviceProvider;

			FactoryPtr m_factory;
		};
		//////////////////////////////////////////////////////////////////////////
		template<class C, class M, class T, size_t N>
		class NodeAffectorCreatorInterpolateBezier
		{
		public:
			typedef MemberAffectorInterpolateBezier<C, M, T, N> AffectorType;

        public:
            NodeAffectorCreatorInterpolateBezier( ServiceProviderInterface * _serviceProvider )
                : m_serviceProvider( _serviceProvider )
            {
                m_factory = new FactoryPool<AffectorType, 4>( m_serviceProvider );
            }

		public:
			Affector * create( EAffectorType _type, const AffectorCallbackPtr & _cb
				, C * _self, M _method
				, const T & _start, const T & _end, const T * _v, float _time )
			{
				AffectorType * affector = m_factory->createObject();

				affector->setServiceProvider( m_serviceProvider );
				affector->setAffectorType( _type );

				affector->setCallback( _cb );

				if( affector->initialize( _self, _method, _start, _end, _v, _time ) == false )
				{
					return nullptr;
				}

				affector->update( _start );

				return affector;
			}

		protected:
            ServiceProviderInterface * m_serviceProvider;

            FactoryPtr m_factory;
		};
		//////////////////////////////////////////////////////////////////////////
	}
}	// namespace Menge
