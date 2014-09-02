#	pragma once

#	include "Kernel/AffectorType.h"

#	include "Kernel/Scriptable.h"

#   include "Factory/Factorable.h"
#   include "Factory/FactoryStore.h"

#	include "Core/ValueInterpolator.h"

#   include "stdex/intrusive_slug_linked.h"

namespace Menge
{
    class ServiceProviderInterface;

	class AffectorCallback
		: public Factorable
	{
	public:
		virtual void onAffectorEnd( size_t _id, bool _isEnd ) = 0;
	};

	class Affector
        : public stdex::intrusive_slug_linked<Affector>
        , public Factorable
	{
	public:
		Affector();
		virtual ~Affector();

    public:
        void initialize( ServiceProviderInterface * _serviceProvider, AffectorCallback * _cb, EAffectorType _type );

	public:
		void setId( size_t _id );
		size_t getId() const;

	public:
		EAffectorType getType() const;
		AffectorCallback * getCb() const;

	public:
		virtual bool affect( float _timing ) = 0;
        virtual void complete() = 0;
		virtual void stop() = 0;

	protected:
		void end_( bool _isEnd );

    protected:
        ServiceProviderInterface * m_serviceProvider;
		AffectorCallback * m_cb;

		EAffectorType m_type;
		size_t m_id;
	};
    //////////////////////////////////////////////////////////////////////////
	template<class C, class M>
	class MemeberAffector
		: public Affector
	{
	public:
        MemeberAffector()
            : m_self(nullptr)
            , m_method(nullptr)
		{
		}

        void initialize( ServiceProviderInterface * _serviceProvider, AffectorCallback * _cb, EAffectorType _type, C * _self, M _method )
        {
            Affector::initialize( _serviceProvider, _cb, _type );

            m_self = _self;
            m_method = _method;
        }

		template<class T>
		void update( T _value )
		{
			(m_self->*m_method)( _value );
		}

        void complete() override
        {
            this->end_( true );
        }

	protected:
		C * m_self;
		M m_method;
	};
    //////////////////////////////////////////////////////////////////////////
	template<class C, class M, class T, template<class> class Accumulator>
	class MemberAffectorAccumulate
		: public MemeberAffector<C,M>
	{
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

			this->end_( false );
		}

	protected:
		Accumulator<T> m_accumulator;
	};
    //////////////////////////////////////////////////////////////////////////
	template<class C, class M, class T, template<class> class Interpolator>
	class MemberAffectorInterpolate
		: public MemeberAffector<C,M>
	{
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

			this->end_( false );
		}

	protected:
		Interpolator<T> m_interpolator;
	};
    //////////////////////////////////////////////////////////////////////////
	template<class C, class M, class T>
	class MemberAffectorAccumulateLinear
		: public MemberAffectorAccumulate<C,M,T,ValueAccumulateLinear>
	{
	public:
		template<class ABS>
		void initialize( ServiceProviderInterface * _serviceProvider, AffectorCallback * _cb, EAffectorType _type, C * _self, M _method
			, const T & _start, const T & _dir, float _speed, ABS _abs)
		{
            MemberAffectorAccumulate<C,M,T,ValueAccumulateLinear>::initialize( _serviceProvider, _cb, _type, _self, _method );
			MemberAffectorAccumulate<C,M,T,ValueAccumulateLinear>::m_accumulator.start( _start, _dir, _speed, _abs );
		}        
	};

	template<class C, class M, class T>
	class MemberAffectorInterpolateLinear
		: public MemberAffectorInterpolate<C,M,T,ValueInterpolatorLinear>
	{
	public:
		template<class ABS>
		void initialize( ServiceProviderInterface * _serviceProvider, AffectorCallback * _cb, EAffectorType _type
            , C * _self, M _method
            , const T & _start, const T & _end, float _time, ABS _abs)
		{
            MemberAffectorInterpolate<C,M,T,ValueInterpolatorLinear>::initialize( _serviceProvider, _cb, _type, _self, _method );
			MemberAffectorInterpolate<C,M,T,ValueInterpolatorLinear>::m_interpolator.start( _start, _end, _time, _abs );
		}
	};
    //////////////////////////////////////////////////////////////////////////
	template<class C, class M, class T>
	class MemberAffectorInterpolateQuadratic
		: public MemberAffectorInterpolate<C,M,T,ValueInterpolatorQuadratic>
	{
	public:
		template< typename ABS >
		void initialize( ServiceProviderInterface * _serviceProvider, AffectorCallback * _cb, EAffectorType _type
			, C * _self, M _method
			, const T & _start, const T & _end, const T & _v0, float _time, ABS _abs )
		{
            MemberAffectorInterpolate<C,M,T,ValueInterpolatorQuadratic>::initialize( _serviceProvider, _cb, _type, _self, _method );
			MemberAffectorInterpolate<C,M,T,ValueInterpolatorQuadratic>::m_interpolator.start( _start, _end, _v0, _time, _abs );
		}
	};
    //////////////////////////////////////////////////////////////////////////
	template<class C, class M, class T>
	class MemberAffectorInterpolateQuadraticBezier
		: public MemberAffectorInterpolate<C,M,T,ValueInterpolatorQuadraticBezier>
	{
	public:
		template<class ABS>
		void initialize( ServiceProviderInterface * _serviceProvider, AffectorCallback * _cb, EAffectorType _type
			, C * _self, M _method
			, const T & _start, const T & _end, const T & _v0
			, float _time, ABS _abs)
		{
            MemberAffectorInterpolate<C,M,T,ValueInterpolatorQuadraticBezier>::initialize( _serviceProvider, _cb, _type, _self, _method );
			MemberAffectorInterpolate<C,M,T,ValueInterpolatorQuadraticBezier>::m_interpolator.start( _start, _end, _v0, _time, _abs );
		}
	};
    //////////////////////////////////////////////////////////////////////////
	template<class C, class M, class T>
	class MemberAffectorInterpolateCubicBezier
		: public MemberAffectorInterpolate<C,M,T,ValueInterpolatorCubicBezier>
	{
	public:
		template<class ABS>
		void initialize( ServiceProviderInterface * _serviceProvider, AffectorCallback * _cb, EAffectorType _type
			, C * _self, M _method
			, const T & _start, const T & _end, const T & _v0, const T & _v1
			, float _time, ABS _abs)
		{
            MemberAffectorInterpolate<C,M,T,ValueInterpolatorCubicBezier>::initialize( _serviceProvider, _cb, _type, _self, _method );
			MemberAffectorInterpolate<C,M,T,ValueInterpolatorCubicBezier>::m_interpolator.start( _start, _end, _v0, _v1, _time, _abs );
		}
	};
    //////////////////////////////////////////////////////////////////////////
	namespace NodeAffectorCreator
	{
        template<class C, class M, class T>
        class NodeAffectorCreatorAccumulateLinear
        {
        public:
            typedef MemberAffectorAccumulateLinear<C, M, T> AffectorType;

        public:
            template<class ABS>
            Affector * create( ServiceProviderInterface * _serviceProvider, AffectorCallback * _cb, EAffectorType _type
                , C * _self, M _method
                , const T & _pos, const T & _dir, float _speed, ABS _abs )
            {
                AffectorType * affector = m_factory.createObjectT();

                affector->initialize(_serviceProvider, _cb, _type
                    , _self, _method
                    , _pos, _dir, _speed
                    , _abs );

                return affector;
            }

            typedef FactoryPoolStore<AffectorType, 4> TFactoryAffector;
            TFactoryAffector m_factory;
        };
        //////////////////////////////////////////////////////////////////////////
		template<class C, class M, class T>
        class NodeAffectorCreatorInterpolateLinear
        {
        public:
            typedef MemberAffectorInterpolateLinear<C, M, T> AffectorType;

        public:
            template<class ABS>
            Affector * create( ServiceProviderInterface * _serviceProvider, AffectorCallback * _cb, EAffectorType _type
                , C * _self, M _method
                , const T & _start, const T & _end, float _time, ABS _abs )
            {
                AffectorType * affector = m_factory.createObjectT();

                affector->initialize(_serviceProvider, _cb, _type
                    , _self, _method
                    , _start, _end, _time
                    , _abs );

                return affector;
            }

        protected:
            typedef FactoryPoolStore<AffectorType, 4> TFactoryAffector;
            TFactoryAffector m_factory;
        };
        //////////////////////////////////////////////////////////////////////////
		template<class C, class M, class T>
        class NodeAffectorCreatorInterpolateQuadratic
        {
        public:
            typedef MemberAffectorInterpolateQuadratic<C, M, T> AffectorType;

        public:
            template<class ABS>
		    Affector * create( ServiceProviderInterface * _serviceProvider, AffectorCallback * _cb, EAffectorType _type
			    , C * _self, M _method
			    , const T & _start, const T & _end, const T & _v0, float _time, ABS _abs )
		    {
                AffectorType * affector = m_factory.createObjectT();

                affector->initialize(_serviceProvider, _cb, _type
                    , _self, _method
                    , _start, _end, _v0, _time
                    , _abs );

                return affector;
		    }

        protected:
            typedef FactoryPoolStore<AffectorType, 4> TFactoryAffector;
            TFactoryAffector m_factory;
        };
        //////////////////////////////////////////////////////////////////////////
		template<class C, class M, class T>
        class NodeAffectorCreatorInterpolateQuadraticBezier
        {
        public:
            typedef MemberAffectorInterpolateQuadraticBezier<C, M, T> AffectorType;

        public:
            template<class ABS>
		    Affector * create( ServiceProviderInterface * _serviceProvider, AffectorCallback * _cb, EAffectorType _type
			    , C * _self, M _method
			    , const T & _start, const T & _end, const T & _v0, float _time, ABS _abs )
		    {
                AffectorType * affector = m_factory.createObjectT();

                affector->initialize(_serviceProvider, _cb, _type
                    , _self, _method
                    , _start, _end, _v0, _time
                    , _abs );

                return affector;
		    }

        protected:
            typedef FactoryPoolStore<AffectorType, 4> TFactoryAffector;
            TFactoryAffector m_factory;
        };
        //////////////////////////////////////////////////////////////////////////
		template<class C, class M, class T>
        class NodeAffectorCreatorInterpolateCubicBezier
        {
        public:
            typedef MemberAffectorInterpolateCubicBezier<C, M, T> AffectorType;

        public:
            template<class ABS>
            Affector * create( ServiceProviderInterface * _serviceProvider, AffectorCallback * _cb, EAffectorType _type
                , C * _self, M _method
                , const T & _start, const T & _end, const T & _v0, const T & _v1, float _time, ABS _abs )
            {
                AffectorType * affector = m_factory.createObjectT();

                affector->initialize(_serviceProvider, _cb, _type
                    , _self, _method
                    , _start, _end, _v0, _v1, _time
                    , _abs );

                return affector;
            }

        protected:
            typedef FactoryPoolStore<AffectorType, 4> TFactoryAffector;
            TFactoryAffector m_factory;
        };
        //////////////////////////////////////////////////////////////////////////
	}
}	// namespace Menge
