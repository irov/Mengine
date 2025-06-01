#pragma once

#include "Interface/FactoryInterface.h"

#include "Kernel/Affector.h"
#include "Kernel/LambdaAffectorInterpolate.h"
#include "Kernel/LambdaAffectorAccumulateLinear.h"
#include "Kernel/AffectorCallbackInterface.h"
#include "Kernel/AssertionFactory.h"
#include "Kernel/FactoryPool.h"
#include "Kernel/ValueInterpolatorLinear.h"
#include "Kernel/ValueInterpolatorQuadratic.h"
#include "Kernel/ValueInterpolatorBezier.h"
#include "Kernel/DocumentHelper.h"

#include "Config/Lambda.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    template<class T>
    class LambdaAffectorInterpolateLinear
        : public LambdaAffectorInterpolate<T, ValueInterpolatorLinear<T> >
    {
        DECLARE_FACTORABLE( LambdaAffectorInterpolateLinear );

    public:
        typedef typename LambdaAffector<T>::LambdaSettuper LambdaSettuper;

    public:
        LambdaAffectorInterpolateLinear()
        {
        }

        ~LambdaAffectorInterpolateLinear() override
        {
        }

    public:
        void initialize( const LambdaSettuper & _lambda, const T & _start, const T & _end, float _time )
        {
            this->setSettuper( _lambda );

            ValueInterpolatorLinear<T> & interpolator = this->getInterpolator();

            interpolator.start( _start, _end, _time );
        }
    };
    //////////////////////////////////////////////////////////////////////////
    template<class T>
    class LambdaAffectorInterpolateQuadratic
        : public LambdaAffectorInterpolate<T, ValueInterpolatorQuadratic<T> >
    {
        DECLARE_FACTORABLE( LambdaAffectorInterpolateQuadratic );

    public:
        typedef typename LambdaAffector<T>::LambdaSettuper LambdaSettuper;

    public:
        LambdaAffectorInterpolateQuadratic()
        {
        }

        ~LambdaAffectorInterpolateQuadratic() override
        {
        }

    public:
        void initialize( const LambdaSettuper & _lambda, const T & _start, const T & _end, const T & _v0, float _time )
        {
            this->setSettuper( _lambda );

            ValueInterpolatorQuadratic<T> & interpolator = this->getInterpolator();

            interpolator.start( _start, _end, _v0, _time );
        }
    };
    //////////////////////////////////////////////////////////////////////////
    template<class T, uint32_t N>
    class LambdaAffectorInterpolateBezier
        : public LambdaAffectorInterpolate<T, ValueInterpolatorBezier<T, N> >
    {
        DECLARE_FACTORABLE( LambdaAffectorInterpolateBezier );

    public:
        typedef Lambda<void( const T & )> LambdaSettuper;
        typedef Lambda<T()> LambdaGetter;
        typedef Lambda<void( T * )> LambdaPoints;

    public:
        LambdaAffectorInterpolateBezier()
        {
        }

        ~LambdaAffectorInterpolateBezier() override
        {
        }


    public:
        void initialize( const LambdaSettuper & _settuper, const LambdaGetter & _getterFrom, const LambdaGetter & _getterTo, const LambdaPoints & _getterPoints, float _time )
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

            interpolator.start( from, to, v, _time );
        }

    protected:
        bool _affect( const UpdateContext * _context, float * const _used ) override
        {
            ValueInterpolatorBezier<T, N> & interpolator = this->getInterpolator();

            T value1 = m_getterFrom();
            interpolator.setValue1( value1 );

            T value2 = m_getterTo();
            interpolator.setValue2( value2 );

            T v[N];
            m_getterPoints( v );
            interpolator.setV( v );

            const EasingInterfacePtr & easing = this->getEasing();

            T value;
            bool finish = interpolator.update( easing, _context, &value, _used );

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
            : public Factorable
        {
        public:
            typedef LambdaAffectorAccumulateLinear<T> AffectorType;
            typedef typename AffectorType::LambdaSettuper LambdaType;
            typedef IntrusivePtr<AffectorType> AffectorTypePtr;

        public:
            NodeAffectorCreatorAccumulateLinear()
            {
            }

            ~NodeAffectorCreatorAccumulateLinear() override
            {
            }

        public:
            bool initialize()
            {
                m_factory = Helper::makeFactoryPool<AffectorType, 16>( MENGINE_DOCUMENT_FACTORABLE );

                return true;
            }

            void finalize()
            {
                MENGINE_ASSERTION_FACTORY_EMPTY( m_factory );
                m_factory = nullptr;
            }

        public:
            AffectorPtr create( EAffectorType _type
                , const EasingInterfacePtr & _easing
                , const AffectorCallbackInterfacePtr & _cb
                , const LambdaType & _lambda
                , const T & _value, const T & _dir, float _speed
                , const DocumentInterfacePtr & _doc )
            {
                AffectorTypePtr affector = m_factory->createObject( _doc );

                affector->setAffectorType( _type );
                affector->setEasing( _easing );
                affector->setCallback( _cb );

                affector->initialize( _lambda, _value, _dir, _speed );

                affector->callSettuper( _value );

                return affector;
            }

        protected:
            FactoryInterfacePtr m_factory;
        };
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        class NodeAffectorCreatorInterpolateLinear
            : public Factorable
        {
            DECLARE_FACTORABLE( NodeAffectorCreatorInterpolateLinear );

        public:
            typedef LambdaAffectorInterpolateLinear<T> AffectorType;
            typedef typename AffectorType::LambdaSettuper LambdaSettuper;
            typedef IntrusivePtr<AffectorType> AffectorTypePtr;

        public:
            NodeAffectorCreatorInterpolateLinear()
            {
            }

            ~NodeAffectorCreatorInterpolateLinear() override
            {
            }

        public:
            bool initialize()
            {
                m_factory = Helper::makeFactoryPool<AffectorType, 16>( MENGINE_DOCUMENT_FACTORABLE );

                return true;
            }

            void finalize()
            {
                MENGINE_ASSERTION_FACTORY_EMPTY( m_factory );
                m_factory = nullptr;
            }

        public:
            AffectorPtr create( EAffectorType _type
                , const EasingInterfacePtr & _easing
                , const AffectorCallbackInterfacePtr & _cb
                , const LambdaSettuper & _lambda
                , const T & _start, const T & _end, float _time
                , const DocumentInterfacePtr & _doc )
            {
                AffectorTypePtr affector = m_factory->createObject( _doc );

                affector->setAffectorType( _type );
                affector->setEasing( _easing );
                affector->setCallback( _cb );

                affector->initialize( _lambda, _start, _end, _time );

                affector->callSettuper( _start );

                return affector;
            }

        protected:
            FactoryInterfacePtr m_factory;
        };
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        class NodeAffectorCreatorInterpolateQuadratic
            : public Factorable
        {
            DECLARE_FACTORABLE( NodeAffectorCreatorInterpolateQuadratic );

        public:
            typedef LambdaAffectorInterpolateQuadratic<T> AffectorType;
            typedef typename AffectorType::LambdaSettuper LambdaSettuper;
            typedef IntrusivePtr<AffectorType> AffectorTypePtr;

        public:
            NodeAffectorCreatorInterpolateQuadratic()
            {
            }

            ~NodeAffectorCreatorInterpolateQuadratic() override
            {
            }

        public:
            bool initialize()
            {
                m_factory = Helper::makeFactoryPool<AffectorType, 16>( MENGINE_DOCUMENT_FACTORABLE );

                return true;
            }

            void finalize()
            {
                MENGINE_ASSERTION_FACTORY_EMPTY( m_factory );

                m_factory = nullptr;
            }

        public:
            AffectorPtr create( EAffectorType _type
                , const EasingInterfacePtr & _easing
                , const AffectorCallbackInterfacePtr & _cb
                , const LambdaSettuper & _lambda
                , const T & _start, const T & _end, const T & _v0, float _time
                , const DocumentInterfacePtr & _doc )
            {
                AffectorTypePtr affector = m_factory->createObject( _doc );

                affector->setAffectorType( _type );
                affector->setEasing( _easing );
                affector->setCallback( _cb );

                affector->initialize( _lambda, _start, _end, _v0, _time );

                affector->callSettuper( _start );

                return affector;
            }

        protected:
            FactoryInterfacePtr m_factory;
        };
        //////////////////////////////////////////////////////////////////////////
        template<class T, uint32_t N>
        class NodeAffectorCreatorInterpolateBezier
            : public Factorable
        {
            DECLARE_FACTORABLE( NodeAffectorCreatorInterpolateBezier );

        public:
            typedef LambdaAffectorInterpolateBezier<T, N> AffectorType;
            typedef typename AffectorType::LambdaSettuper LambdaSettuper;
            typedef typename AffectorType::LambdaGetter LambdaGetter;
            typedef typename AffectorType::LambdaPoints LambdaPoints;

            typedef IntrusivePtr<AffectorType> AffectorTypePtr;

        public:
            NodeAffectorCreatorInterpolateBezier()
            {
            }

            ~NodeAffectorCreatorInterpolateBezier() override
            {
            }

        public:
            bool initialize()
            {
                m_factory = Helper::makeFactoryPool<AffectorType, 16>( MENGINE_DOCUMENT_FACTORABLE );

                return true;
            }

            void finalize()
            {
                MENGINE_ASSERTION_FACTORY_EMPTY( m_factory );
                m_factory = nullptr;
            }

        public:
            AffectorPtr create( EAffectorType _type
                , const EasingInterfacePtr & _easing
                , const AffectorCallbackInterfacePtr & _cb
                , const LambdaSettuper & _settuper, const LambdaGetter & _getterFrom, const LambdaGetter & _getterTo, const LambdaPoints & _points
                , float _time
                , const DocumentInterfacePtr & _doc )
            {
                AffectorTypePtr affector = m_factory->createObject( _doc );

                affector->setAffectorType( _type );
                affector->setEasing( _easing );
                affector->setCallback( _cb );

                affector->initialize( _settuper, _getterFrom, _getterTo, _points, _time );

                T from = _getterFrom();
                _settuper( from );

                return affector;
            }

        protected:
            FactoryInterfacePtr m_factory;
        };
        //////////////////////////////////////////////////////////////////////////
    }
}
