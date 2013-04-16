#	pragma once

#	include "Kernel/AffectorType.h"

#	include "Kernel/Scriptable.h"

#	include "Core/ValueInterpolator.h"
#	include "Core/IntrusiveLinked.h"

namespace Menge
{
    class ServiceProviderInterface;

	class Affector
		: public IntrusiveLinked<Affector>
	{
	public:
		Affector( ServiceProviderInterface * _serviceProvider, PyObject * _cb, EAffectorType _type );
		virtual ~Affector();

	public:
		void setId( size_t _id );
        size_t getId() const;

		void setType( EAffectorType _type );
		EAffectorType getType() const;

		void setCb( PyObject * _cb );
		PyObject * getCb() const;

	public:
		virtual bool affect( float _timing ) = 0;
        virtual void complete() = 0;
		virtual void stop() = 0;

	protected:
		void call( Scriptable * _scriptable, bool _isEnd );

	protected:
        ServiceProviderInterface * m_serviceProvider;
		PyObject * m_cb;

		EAffectorType m_type;
		size_t m_id;
	};

	template<class C, class M>
	class MemeberAffector
		: public Affector
	{
	public:
		MemeberAffector( ServiceProviderInterface * _serviceProvider, PyObject* _cb, EAffectorType _type, C * _self, M _method )
			: Affector(_serviceProvider, _cb, _type)
			, m_self(_self)
			, m_method(_method)
		{
		}

		template<class T>
		void update( T _value )
		{
			(m_self->*m_method)( _value );
		}

        void complete() override
        {
            this->call_self( true );
        }

    protected:
		void call_self( bool _isEnd )
		{
			this->call( m_self, _isEnd );
		}

	protected:
		C * m_self;
		M m_method;
	};

	template<class C, class M, class T, template<class> class Accumulator>
	class MemberAffectorAccumulate
		: public MemeberAffector<C,M>
	{
	public:
		MemberAffectorAccumulate( ServiceProviderInterface * _serviceProvider, PyObject* _cb, EAffectorType _type, C * _self, M _method )
			: MemeberAffector<C,M>(_serviceProvider, _cb, _type, _self, _method)
		{
		}

	protected:
		bool affect( float _timing ) override
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

			this->call_self( false );
		}

	protected:
		Accumulator<T> m_accumulator;
	};

	template<class C, class M, class T, template<class> class Interpolator>
	class MemberAffectorInterpolate
		: public MemeberAffector<C,M>
	{
	public:
		MemberAffectorInterpolate( ServiceProviderInterface * _serviceProvider, PyObject* _cb, EAffectorType _type, C * _self, M _method )
			: MemeberAffector<C,M>(_serviceProvider, _cb, _type, _self, _method)
		{
		}

	protected:
		bool affect( float _timing ) override
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

			this->call_self( false );
		}

	protected:
		Interpolator<T> m_interpolator;
	};

	template<class C, class M, class T>
	class MemberAffectorAccumulateLinear
		: public MemberAffectorAccumulate<C,M,T,ValueAccumulateLinear>
	{
	public:
		template<class ABS>
		MemberAffectorAccumulateLinear<C,M,T>( ServiceProviderInterface * _serviceProvider, PyObject* _cb, EAffectorType _type, C * _self, M _method
			, T _start, T _dir, float _speed, ABS _abs)
			: MemberAffectorAccumulate<C,M,T,ValueAccumulateLinear>(_serviceProvider, _cb, _type, _self, _method)
		{
			MemberAffectorAccumulate<C,M,T,ValueAccumulateLinear>::m_accumulator.start( _start, _dir, _speed, _abs );
		}
	};

	template<class C, class M, class T>
	class MemberAffectorInterpolateLinear
		: public MemberAffectorInterpolate<C,M,T,ValueInterpolatorLinear>
	{
	public:
		template<class ABS>
		MemberAffectorInterpolateLinear<C,M,T>( ServiceProviderInterface * _serviceProvider, PyObject* _cb, EAffectorType _type, C * _self, M _method
										, T _start, T _end, float _time, ABS _abs)
			: MemberAffectorInterpolate<C,M,T,ValueInterpolatorLinear>(_serviceProvider, _cb, _type, _self, _method)
		{
			MemberAffectorInterpolate<C,M,T,ValueInterpolatorLinear>::m_interpolator.start( _start, _end, _time, _abs );
		}
	};

	template<class C, class M, class T>
	class MemberAffectorInterpolateQuadratic
		: public MemberAffectorInterpolate<C,M,T,ValueInterpolatorQuadratic>
	{
	public:
		template< typename ABS >
		MemberAffectorInterpolateQuadratic( ServiceProviderInterface * _serviceProvider, PyObject* _cb, EAffectorType _type
			, C * _self, M _method
			, T _start, T _end, T _v0
			, float _time
			, ABS _abs )
			: MemberAffectorInterpolate<C,M,T,ValueInterpolatorQuadratic>(_serviceProvider, _cb, _type, _self, _method)
		{
			MemberAffectorInterpolate<C,M,T,ValueInterpolatorQuadratic>::m_interpolator.start( _start, _end, _v0, _time, _abs );
		}
	};

	template<class C, class M, class T>
	class MemberAffectorInterpolateQuadraticBezier
		: public MemberAffectorInterpolate<C,M,T,ValueInterpolatorQuadraticBezier>
	{
	public:
		template<class ABS>
		MemberAffectorInterpolateQuadraticBezier<C,M,T>( ServiceProviderInterface * _serviceProvider, PyObject* _cb, EAffectorType _type
			, C * _self, M _method
			, T _start, T _end, T _v0
			, float _time, ABS _abs)
			: MemberAffectorInterpolate<C,M,T,ValueInterpolatorQuadraticBezier>(_serviceProvider, _cb, _type, _self, _method)
		{
			MemberAffectorInterpolate<C,M,T,ValueInterpolatorQuadraticBezier>::m_interpolator.start( _start, _end, _v0, _time, _abs );
		}
	};

	template<class C, class M, class T>
	class MemberAffectorInterpolateCubicBezier
		: public MemberAffectorInterpolate<C,M,T,ValueInterpolatorCubicBezier>
	{
	public:
		template<class ABS>
		MemberAffectorInterpolateCubicBezier<C,M,T>( ServiceProviderInterface * _serviceProvider, PyObject* _cb, EAffectorType _type
			, C * _self, M _method
			, T _start, T _end, T _v0, T _v1
			, float _time, ABS _abs)
			: MemberAffectorInterpolate<C,M,T,ValueInterpolatorCubicBezier>(_serviceProvider, _cb, _type, _self, _method)
		{
			MemberAffectorInterpolate<C,M,T,ValueInterpolatorCubicBezier>::m_interpolator.start( _start, _end, _v0, _v1, _time, _abs );
		}
	};

//MemberAffectorInterpolateCubic

	namespace NodeAffectorCreator
	{
		template<class C, class M, class T, class ABS>
		Affector * newNodeAffectorAccumulateLinear( ServiceProviderInterface * _serviceProvider, PyObject * _cb, EAffectorType _type
			, C * _self, M _method
			, T _pos, T _dir, float _speed, ABS _abs )
		{
            typedef MemberAffectorAccumulateLinear<C, M, T> AffectorType;

            //Factory * AFFECTOR_SERVICE(_serviceProvider)
            //    ->getFactory();

            //Factorable * factorable = Factory->createObject();

			return new AffectorType(_serviceProvider, _cb, _type
				, _self, _method
				, _pos, _dir, _speed
				, _abs );
		}

		template<class C, class M, class T, class ABS>
		Affector * newNodeAffectorInterpolateLinear( ServiceProviderInterface * _serviceProvider, PyObject* _cb, EAffectorType _type
			, C * _self, M _method
			, T _start, T _end, float _time, ABS _abs )
		{
            typedef MemberAffectorInterpolateLinear<C, M, T> AffectorType;

            return new AffectorType(_serviceProvider, _cb, _type
				, _self, _method
				, _start, _end, _time
				, _abs );
		}

		template<class C, class M, class T, class ABS>
		Affector * newNodeAffectorInterpolateQuadratic( ServiceProviderInterface * _serviceProvider, PyObject* _cb, EAffectorType _type
			, C * _self, M _method
			, T _start, T _end, T _v0, float _time, ABS _abs )
		{
            typedef MemberAffectorInterpolateQuadratic<C, M, T> AffectorType;

			return new AffectorType(_serviceProvider, _cb, _type
				, _self, _method
				, _start, _end, _v0, _time
				, _abs );
		}

		template<class C, class M, class T, class ABS>
		Affector * newNodeAffectorInterpolateQuadraticBezier( ServiceProviderInterface * _serviceProvider, PyObject* _cb, EAffectorType _type
			, C * _self, M _method
			, T _start, T _end, T _v0, float _time, ABS _abs )
		{
            typedef MemberAffectorInterpolateQuadraticBezier<C, M, T> AffectorType;

			return new AffectorType(_serviceProvider, _cb, _type
				, _self, _method
				, _start, _end, _v0, _time
				, _abs );
		}

		template<class C, class M, class T, class ABS>
		Affector * newNodeAffectorInterpolateCubicBezier( ServiceProviderInterface * _serviceProvider, PyObject* _cb, EAffectorType _type
			, C * _self, M _method
			, T _start, T _end, T _v0, T _v1, float _time, ABS _abs )
		{
            typedef MemberAffectorInterpolateCubicBezier<C, M, T> AffectorType;

			return new AffectorType(_serviceProvider, _cb, _type
				, _self, _method
				, _start, _end, _v0, _v1, _time
				, _abs );
		}

	}
}	// namespace Menge
