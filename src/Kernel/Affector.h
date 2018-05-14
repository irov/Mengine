#pragma once

#include "Config/Lambda.h"

#include "Kernel/AffectorType.h"

#include "Kernel/Scriptable.h"

#include "Factory/Factorable.h"
#include "Factory/FactoryPool.h"

#include "Core/ValueInterpolator.h"

#include "stdex/intrusive_slug_linked_ptr.h"

namespace Mengine
{
	//////////////////////////////////////////////////////////////////////////
	class AffectorCallback
		: public Factorable
	{
	public:
		virtual void onAffectorEnd( AFFECTOR_ID _id, bool _isEnd ) = 0;
	};
	//////////////////////////////////////////////////////////////////////////
	typedef IntrusivePtr<AffectorCallback> AffectorCallbackPtr;
	//////////////////////////////////////////////////////////////////////////
	class Affector
		: public stdex::intrusive_slug_linked_ptr<Affector>
		, public Factorable
	{
	public:
		Affector();
		~Affector() override;

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
		EAffectorType m_type;
		AFFECTOR_ID m_id;

		float m_speedFactor;

		bool m_freeze;
	};
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Affector> AffectorPtr;
	//////////////////////////////////////////////////////////////////////////
	class CallbackAffector
		: public Affector
	{
	public:
		CallbackAffector();
		~CallbackAffector() override;

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
	template<class L>
	class LambdaAffector
		: public CallbackAffector
	{
	public:
		LambdaAffector()
		{
		}

		void setup( const L & _lambda )
		{
			m_lambda = _lambda;
		}

		template<class T>
		void update( const T & _value )
		{
            m_lambda( _value );
		}

	protected:
		L m_lambda;
	};
	//////////////////////////////////////////////////////////////////////////
	template<class L, class T, template<class> class Accumulator>
	class LambdaAffectorAccumulate
		: public LambdaAffector<L>
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
	template<class L, class T, class Interpolator>
	class LambdaAffectorInterpolate
		: public LambdaAffector<L>
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
	template<class L, class T>
	class LambdaAffectorAccumulateLinear
		: public LambdaAffectorAccumulate<L, T, ValueAccumulateLinear>
	{
	public:
		bool initialize( const L & _lambda, const T & _start, const T & _dir, float _speed )
		{
            LambdaAffectorAccumulate<L, T, ValueAccumulateLinear>::setup( _lambda );

			if( LambdaAffectorAccumulate<L, T, ValueAccumulateLinear>::m_accumulator.start( _start, _dir, _speed ) == false )
			{
				return false;
			}

			return true;
		}
	};
	//////////////////////////////////////////////////////////////////////////
	template<class L, class T>
	class LambdaAffectorInterpolateLinear
		: public LambdaAffectorInterpolate<L, T, ValueInterpolatorLinear<T> >
	{
	public:
		bool initialize( const L & _lambda, const T & _start, const T & _end, float _time )
		{
            LambdaAffectorInterpolate<L, T, ValueInterpolatorLinear<T> >::setup( _lambda );

            bool successful = LambdaAffectorInterpolate<L, T, ValueInterpolatorLinear<T> >::m_interpolator.start( _start, _end, _time );

			return successful;
		}
	};
	//////////////////////////////////////////////////////////////////////////
	template<class L, class T>
	class LambdaAffectorInterpolateQuadratic
		: public LambdaAffectorInterpolate<L, T, ValueInterpolatorQuadratic<T> >
	{
	public:
		bool initialize( const L & _lambda, const T & _start, const T & _end, const T & _v0, float _time )
		{
            LambdaAffectorInterpolate<L, T, ValueInterpolatorQuadratic<T> >::setup( _lambda );

            bool successful = LambdaAffectorInterpolate<L, T, ValueInterpolatorQuadratic<T> >::m_interpolator.start( _start, _end, _v0, _time );

			return successful;
		}
	};
	//////////////////////////////////////////////////////////////////////////
	template<class L, class T, size_t N>
	class LambdaAffectorInterpolateBezier
		: public LambdaAffectorInterpolate<L, T, ValueInterpolatorBezier<T, N> >
	{
	public:
		bool initialize( const L & _lambda, const T & _start, const T & _end, const T * _v, float _time )
		{
            LambdaAffectorInterpolate<L, T, ValueInterpolatorBezier<T, N> >::setup( _lambda );

            bool successful = LambdaAffectorInterpolate<L, T, ValueInterpolatorBezier<T, N> >::m_interpolator.start( _start, _end, _v, _time );

            return successful;
		}
	};
	//////////////////////////////////////////////////////////////////////////
	namespace NodeAffectorCreator
	{
		template<class T>
		class NodeAffectorCreatorAccumulateLinear
		{
		public:
            typedef Lambda<void( T )> LambdaType;
			typedef LambdaAffectorAccumulateLinear<LambdaType, T> AffectorType;
            typedef IntrusivePtr<AffectorType> AffectorTypePtr;

        public:
            NodeAffectorCreatorAccumulateLinear()
            {
                m_factory = new FactoryPool<AffectorType, 16>();
            }

            virtual ~NodeAffectorCreatorAccumulateLinear()
            {
            }

		public:
            AffectorPtr create( EAffectorType _type, const AffectorCallbackPtr & _cb
                , const LambdaType & _lambda
				, const T & _pos, const T & _dir, float _speed )
			{
				AffectorTypePtr affector = m_factory->createObject();

				affector->setAffectorType( _type );

				affector->setCallback( _cb );

				if( affector->initialize( _lambda, _pos, _dir, _speed ) == false )
				{
					return nullptr;
				}

				affector->update( _pos );

				return affector;
			}

        protected:
			FactoryPtr m_factory;
		};
		//////////////////////////////////////////////////////////////////////////
		template<class T>
		class NodeAffectorCreatorInterpolateLinear
		{
		public:
            typedef Lambda<void( T )> LambdaType;
            typedef LambdaAffectorInterpolateLinear<LambdaType, T> AffectorType;
            typedef IntrusivePtr<AffectorType> AffectorTypePtr;

        public:
            NodeAffectorCreatorInterpolateLinear()
            {
                m_factory = new FactoryPool<AffectorType, 16>();
            }

            virtual ~NodeAffectorCreatorInterpolateLinear()
            {
            }

		public:
			AffectorPtr create( EAffectorType _type, const AffectorCallbackPtr & _cb
				, const LambdaType & _lambda
				, const T & _start, const T & _end, float _time )
			{
				AffectorTypePtr affector = m_factory->createObject();

				affector->setAffectorType( _type );

				affector->setCallback( _cb );

                if( affector->initialize( _lambda, _start, _end, _time ) == false )
				{
					return nullptr;
				}

				affector->update( _start );

				return affector;
			}

		protected:
            FactoryPtr m_factory;
		};
		//////////////////////////////////////////////////////////////////////////
		template<class T>
		class NodeAffectorCreatorInterpolateQuadratic
		{
		public:
            typedef Lambda<void( T )> LambdaType;
			typedef LambdaAffectorInterpolateQuadratic<LambdaType, T> AffectorType;
            typedef IntrusivePtr<AffectorType> AffectorTypePtr;

        public:
            NodeAffectorCreatorInterpolateQuadratic()
            {
                m_factory = new FactoryPool<AffectorType, 16>();
            }

            virtual ~NodeAffectorCreatorInterpolateQuadratic()
            {
            }

		public:
			AffectorPtr create( EAffectorType _type, const AffectorCallbackPtr & _cb
				, const LambdaType & _lambda
				, const T & _start, const T & _end, const T & _v0, float _time )
			{
                AffectorTypePtr affector = m_factory->createObject();

				affector->setAffectorType( _type );

				affector->setCallback( _cb );

				if( affector->initialize( _lambda, _start, _end, _v0, _time ) == false )
				{
					return nullptr;
				}

				affector->update( _start );

				return affector;
			}

		protected:
			FactoryPtr m_factory;
		};
		//////////////////////////////////////////////////////////////////////////
        template<class T, size_t N>
		class NodeAffectorCreatorInterpolateBezier
		{
		public:
            typedef Lambda<void( T )> LambdaType;
            typedef LambdaAffectorInterpolateBezier<LambdaType, T, N> AffectorType;
            typedef IntrusivePtr<AffectorType> AffectorTypePtr;

        public:
            NodeAffectorCreatorInterpolateBezier()
            {
                m_factory = new FactoryPool<AffectorType, 16>();
            }

            virtual ~NodeAffectorCreatorInterpolateBezier()
            {
            }

		public:
			AffectorPtr create( EAffectorType _type, const AffectorCallbackPtr & _cb
				, const LambdaType & _lambda
				, const T & _start, const T & _end, const T * _v, float _time )
			{
                AffectorTypePtr affector = m_factory->createObject();

				affector->setAffectorType( _type );

				affector->setCallback( _cb );

                if( affector->initialize( _lambda, _start, _end, _v, _time ) == false )
				{
					return nullptr;
				}

				affector->update( _start );

				return affector;
			}

		protected:
            FactoryPtr m_factory;
		};
		//////////////////////////////////////////////////////////////////////////
	}
}	
