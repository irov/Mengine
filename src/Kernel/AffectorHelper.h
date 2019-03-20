#pragma once

#include "Config/Lambda.h"

#include "Kernel/Affector.h"
#include "Kernel/AffectorCallbackInterface.h"
#include "Kernel/Factory.h"
#include "Kernel/FactoryPool.h"
#include "Kernel/ValueInterpolator.h"
#include "Kernel/Document.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class BaseAffector
        : public Affector
    {
    public:
        BaseAffector();
        ~BaseAffector() override;

    public:
        void setCallback( const AffectorCallbackInterfacePtr & _cb );
        const AffectorCallbackInterfacePtr & getCallback() const;

    protected:
        void _complete( bool _isEnd ) override;

    protected:
        AffectorCallbackInterfacePtr m_cb;
    };
    //////////////////////////////////////////////////////////////////////////
    template<class T>
    class LambdaAffector
        : public BaseAffector
    {
    public:
        typedef Lambda<void( const T & )> LambdaSettuper;

    public:
        LambdaAffector()
        {
        }

        ~LambdaAffector() override
        {
        }

    public:
        void setSettuper( const LambdaSettuper & _lambda )
        {
            m_settuper = _lambda;
        }

        const LambdaSettuper & getSettuper() const
        {
            return m_settuper;
        }

        void callSettuper( const T & _value )
        {
            m_settuper( _value );
        }

    protected:
        LambdaSettuper m_settuper;
    };
    //////////////////////////////////////////////////////////////////////////
    template<class T, class Interpolator>
    class LambdaAffectorInterpolate
        : public LambdaAffector<T>
    {
    protected:
        Interpolator & getInterpolator()
        {
            return m_interpolator;
        }

    protected:
        bool _affect( const UpdateContext * _context, float * _used ) override
        {
            T value;
            bool finish = m_interpolator.update( _context, &value, _used );

            this->callSettuper( value );

            return finish;
        }

        void _stop() override
        {
            m_interpolator.stop();
        }

    protected:
        Interpolator m_interpolator;
    };
    //////////////////////////////////////////////////////////////////////////
    template<class T>
    class LambdaAffectorAccumulateLinear
        : public LambdaAffectorInterpolate<T, ValueAccumulateLinear<T> >
    {
    public:
        typedef typename LambdaAffector<T>::LambdaSettuper LambdaSettuper;

    public:
        bool initialize( const LambdaSettuper & _lambda, const T & _start, const T & _dir, float _speed )
        {
            this->setSettuper( _lambda );

            ValueAccumulateLinear<T> & interpolator = this->getInterpolator();

            if( interpolator.start( _start, _dir, _speed ) == false )
            {
                return false;
            }

            return true;
        }
    };
    //////////////////////////////////////////////////////////////////////////
    template<class T>
    class LambdaAffectorInterpolateLinear
        : public LambdaAffectorInterpolate<T, ValueInterpolatorLinear<T> >
    {
    public:
        typedef typename LambdaAffector<T>::LambdaSettuper LambdaSettuper;

    public:
        bool initialize( const LambdaSettuper & _lambda, const T & _start, const T & _end, float _time )
        {
            this->setSettuper( _lambda );

            ValueInterpolatorLinear<T> & interpolator = this->getInterpolator();

            bool successful = interpolator.start( _start, _end, _time );

            return successful;
        }
    };
    //////////////////////////////////////////////////////////////////////////
    template<class T>
    class LambdaAffectorInterpolateQuadratic
        : public LambdaAffectorInterpolate<T, ValueInterpolatorQuadratic<T> >
    {
    public:
        typedef typename LambdaAffector<T>::LambdaSettuper LambdaSettuper;

    public:
        bool initialize( const LambdaSettuper & _lambda, const T & _start, const T & _end, const T & _v0, float _time )
        {
            this->setSettuper( _lambda );

            ValueInterpolatorQuadratic<T> & interpolator = this->getInterpolator();

            bool successful = interpolator.start( _start, _end, _v0, _time );

            return successful;
        }
    };
    //////////////////////////////////////////////////////////////////////////
    template<class T, uint32_t N>
    class LambdaAffectorInterpolateBezier
        : public LambdaAffectorInterpolate<T, ValueInterpolatorBezier<T, N> >
    {
    public:
        typedef Lambda<void( const T & )> LambdaSettuper;
        typedef Lambda<T()> LambdaGetter;
        typedef Lambda<void( T * )> LambdaPoints;

    public:
        bool initialize( const LambdaSettuper & _settuper, const LambdaGetter & _getterFrom, const LambdaGetter & _getterTo, const LambdaPoints & _getterPoints, float _time )
        {
            m_settuper = _settuper;
            m_getterFrom = _getterFrom;
            m_getterTo = _getterTo;
            m_getterPoints = _getterPoints;

            T from = _getterFrom();
            T to = _getterTo();

            T v[N];
            _getterPoints( v );

            ValueInterpolatorBezier<T, N> & interpolator = this->getInterpolator();

            bool successful = interpolator.start( from, to, v, _time );

            return successful;
        }

    protected:
        bool _affect( const UpdateContext * _context, float * _used ) override
        {
            ValueInterpolatorBezier<T, N> & interpolator = this->getInterpolator();

            T value1 = m_getterFrom();
            interpolator.setValue1( value1 );

            T value2 = m_getterTo();
            interpolator.setValue2( value2 );

            T v[N];
            m_getterPoints( v );
            interpolator.setV( v );

            T value;
            bool finish = interpolator.update( _context, &value, _used );

            m_settuper( value );

            return finish;
        }

        void _stop() override
        {
            ValueInterpolatorBezier<T, N> & interpolator = this->getInterpolator();

            interpolator.stop();
        }

    protected:
        LambdaSettuper m_settuper;
        LambdaGetter m_getterFrom;
        LambdaGetter m_getterTo;
        LambdaPoints m_getterPoints;
    };
    //////////////////////////////////////////////////////////////////////////
    namespace NodeAffectorCreator
    {
        template<class T>
        class NodeAffectorCreatorAccumulateLinear
        {
        public:
            typedef LambdaAffectorAccumulateLinear<T> AffectorType;
            typedef typename AffectorType::LambdaSettuper LambdaType;
            typedef IntrusivePtr<AffectorType> AffectorTypePtr;

        public:
            NodeAffectorCreatorAccumulateLinear()
                : m_factory( new FactoryPool<AffectorType, 16>() )
            {
            }

            virtual ~NodeAffectorCreatorAccumulateLinear()
            {
            }

        public:
            AffectorPtr create( EAffectorType _type, const AffectorCallbackInterfacePtr & _cb
                , const LambdaType & _lambda
                , const T & _pos, const T & _dir, float _speed )
            {
                AffectorTypePtr affector = m_factory->createObject( MENGINE_DOCUMENT_FUNCTION );

                affector->setAffectorType( _type );

                affector->setCallback( _cb );

                if( affector->initialize( _lambda, _pos, _dir, _speed ) == false )
                {
                    return nullptr;
                }

                affector->callSettuper( _pos );

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
            typedef LambdaAffectorInterpolateLinear<T> AffectorType;
            typedef typename AffectorType::LambdaSettuper LambdaSettuper;
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
            AffectorPtr create( EAffectorType _type, const AffectorCallbackInterfacePtr & _cb
                , const LambdaSettuper & _lambda
                , const T & _start, const T & _end, float _time )
            {
                AffectorTypePtr affector = m_factory->createObject( MENGINE_DOCUMENT_FUNCTION );

                affector->setAffectorType( _type );

                affector->setCallback( _cb );

                if( affector->initialize( _lambda, _start, _end, _time ) == false )
                {
                    return nullptr;
                }

                affector->callSettuper( _start );

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
            typedef LambdaAffectorInterpolateQuadratic<T> AffectorType;
            typedef typename AffectorType::LambdaSettuper LambdaSettuper;
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
            AffectorPtr create( EAffectorType _type, const AffectorCallbackInterfacePtr & _cb
                , const LambdaSettuper & _lambda
                , const T & _start, const T & _end, const T & _v0, float _time )
            {
                AffectorTypePtr affector = m_factory->createObject( MENGINE_DOCUMENT_FUNCTION );

                affector->setAffectorType( _type );

                affector->setCallback( _cb );

                if( affector->initialize( _lambda, _start, _end, _v0, _time ) == false )
                {
                    return nullptr;
                }

                affector->callSettuper( _start );

                return affector;
            }

        protected:
            FactoryPtr m_factory;
        };
        //////////////////////////////////////////////////////////////////////////
        template<class T, uint32_t N>
        class NodeAffectorCreatorInterpolateBezier
        {
        public:
            typedef LambdaAffectorInterpolateBezier<T, N> AffectorType;
            typedef typename AffectorType::LambdaSettuper LambdaSettuper;
            typedef typename AffectorType::LambdaGetter LambdaGetter;
            typedef typename AffectorType::LambdaPoints LambdaPoints;

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
            AffectorPtr create( EAffectorType _type, const AffectorCallbackInterfacePtr & _cb
                , const LambdaSettuper & _settuper, const LambdaGetter & _getterFrom, const LambdaGetter & _getterTo, const LambdaPoints & _points
                , float _time )
            {
                AffectorTypePtr affector = m_factory->createObject( MENGINE_DOCUMENT_FUNCTION );

                affector->setAffectorType( _type );

                affector->setCallback( _cb );

                if( affector->initialize( _settuper, _getterFrom, _getterTo, _points, _time ) == false )
                {
                    return nullptr;
                }

                T from = _getterFrom();
                _settuper( from );

                return affector;
            }

        protected:
            FactoryPtr m_factory;
        };
        //////////////////////////////////////////////////////////////////////////
    }
}
