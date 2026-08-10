#include "Box2DScriptEmbedding.h"

#include "Box2DInterface.h"

#include "Environment/Python/PythonCallbackProvider.h"
#include "Environment/Python/PythonDocument.h"

#include "Kernel/Factorable.h"
#include "Kernel/FactorableUnique.h"

#include "pybind/list.hpp"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        class PythonBox2DContactListener
            : public Box2DContactListenerInterface
            , public PythonCallbackProvider
        {
        public:
            PythonBox2DContactListener( const pybind::object & _callback, const pybind::args & _args )
                : PythonCallbackProvider( _callback, _args )
            {
            }

            ~PythonBox2DContactListener() override
            {
            }

        protected:
            void onBox2DContactEvent( EBox2DContactEventType _type
                , const Box2DBodyInterfacePtr & _bodyA
                , const Box2DBodyInterfacePtr & _bodyB
                , const mt::vec2f & _point
                , const mt::vec2f & _normal
                , float _value ) override
            {
                this->call_cb( (uint32_t)_type, _bodyA, _bodyB, _point, _normal, _value );
            }
        };
        //////////////////////////////////////////////////////////////////////////
        typedef IntrusivePtr<PythonBox2DContactListener, Box2DContactListenerInterface> PythonBox2DContactListenerPtr;
        //////////////////////////////////////////////////////////////////////////
        class PythonBox2DRayCast
            : public Box2DRayCastInterface
            , public PythonCallbackProvider
            , public Factorable
        {
        public:
            PythonBox2DRayCast( const pybind::object & _callback, const pybind::args & _args )
                : PythonCallbackProvider( _callback, _args )
            {
            }

            ~PythonBox2DRayCast() override
            {
            }

        protected:
            float onBox2DRayCast( uint32_t _index, const Box2DBodyInterface * _body, const mt::vec2f & _point, const mt::vec2f & _normal, float _fraction ) override
            {
                Box2DBodyInterfacePtr body = Box2DBodyInterfacePtr::from( const_cast<Box2DBodyInterface *>(_body) );
                pybind::object result = this->call_cb( _index, body, _point, _normal, _fraction );

                if( result.is_none() == true )
                {
                    return _fraction;
                }

                return result.extract();
            }
        };
        //////////////////////////////////////////////////////////////////////////
        typedef IntrusivePtr<PythonBox2DRayCast, Box2DRayCastInterface> PythonBox2DRayCastPtr;
        //////////////////////////////////////////////////////////////////////////
        static Box2DWorldInterfacePtr createBox2DWorld( const mt::vec2f & _gravity, float _scaler )
        {
            return BOX2D_SERVICE()->createWorld( _gravity, _scaler, MENGINE_DOCUMENT_PYTHON );
        }
        //////////////////////////////////////////////////////////////////////////
        static void destroyBox2DWorld( const Box2DWorldInterfacePtr & _world )
        {
            if( _world == nullptr || _world->isValid() == false )
            {
                return;
            }

            BOX2D_SERVICE()->destroyWorld( _world );
        }
        //////////////////////////////////////////////////////////////////////////
        static bool isSameBox2DBody( const Box2DBodyInterfacePtr & _left, const Box2DBodyInterfacePtr & _right )
        {
            return _left.get() == _right.get();
        }
        //////////////////////////////////////////////////////////////////////////
        static Box2DBodyInterfacePtr createBox2DBody( const Box2DWorldInterfacePtr & _world
            , uint32_t _type
            , const mt::vec2f & _position
            , float _angle
            , float _linearDamping
            , float _angularDamping
            , bool _allowSleep
            , bool _isBullet
            , bool _fixedRotation
            , float _gravityScale )
        {
            if( _world == nullptr || _world->isValid() == false || _type > EBOX2D_BODY_DYNAMIC )
            {
                return nullptr;
            }

            return _world->createBodyType( (EBox2DBodyType)_type, _position, _angle, _linearDamping, _angularDamping, _allowSleep, _isBullet, _fixedRotation, _gravityScale, MENGINE_DOCUMENT_PYTHON );
        }
        //////////////////////////////////////////////////////////////////////////
        static Box2DJointInterfacePtr createBox2DDistanceJoint( const Box2DWorldInterfacePtr & _world, const Box2DBodyInterfacePtr & _bodyA, const Box2DBodyInterfacePtr & _bodyB, const mt::vec2f & _offsetA, const mt::vec2f & _offsetB, bool _collide )
        {
            return _world->createDistanceJoint( _bodyA, _bodyB, _offsetA, _offsetB, _collide, MENGINE_DOCUMENT_PYTHON );
        }
        //////////////////////////////////////////////////////////////////////////
        static Box2DJointInterfacePtr createBox2DHingeJoint( const Box2DWorldInterfacePtr & _world, const Box2DBodyInterfacePtr & _bodyA, const Box2DBodyInterfacePtr & _bodyB, const mt::vec2f & _offsetA, const mt::vec2f & _limits, bool _collide )
        {
            return _world->createHingeJoint( _bodyA, _bodyB, _offsetA, _limits, _collide, MENGINE_DOCUMENT_PYTHON );
        }
        //////////////////////////////////////////////////////////////////////////
        static Box2DJointInterfacePtr createBox2DPrismaticJoint( const Box2DWorldInterfacePtr & _world, const Box2DBodyInterfacePtr & _bodyA, const Box2DBodyInterfacePtr & _bodyB, const mt::vec2f & _axis, bool _collide, bool _enableLimit, const mt::vec2f & _translation, bool _enableMotor, float _maxMotorForce, float _motorSpeed )
        {
            return _world->createPrismaticJoint( _bodyA, _bodyB, _axis, _collide, _enableLimit, _translation, _enableMotor, _maxMotorForce, _motorSpeed, MENGINE_DOCUMENT_PYTHON );
        }
        //////////////////////////////////////////////////////////////////////////
        static Box2DJointInterfacePtr createBox2DWeldJoint( const Box2DWorldInterfacePtr & _world, const Box2DBodyInterfacePtr & _bodyA, const Box2DBodyInterfacePtr & _bodyB, const mt::vec2f & _offsetA, const mt::vec2f & _offsetB, bool _collide )
        {
            return _world->createWeldJoint( _bodyA, _bodyB, _offsetA, _offsetB, _collide, MENGINE_DOCUMENT_PYTHON );
        }
        //////////////////////////////////////////////////////////////////////////
        static Box2DJointInterfacePtr createBox2DWheelJoint( const Box2DWorldInterfacePtr & _world, const Box2DBodyInterfacePtr & _bodyA, const Box2DBodyInterfacePtr & _bodyB, const mt::vec2f & _offset, const mt::vec2f & _axis, bool _collide, bool _enableLimit, float _lowerTranslation, float _upperTranslation, float _maxMotorTorque, bool _enableMotor, float _motorSpeed, float _stiffness, float _damping )
        {
            return _world->createWheelJoint( _bodyA, _bodyB, _offset, _axis, _collide, _enableLimit, _lowerTranslation, _upperTranslation, _maxMotorTorque, _enableMotor, _motorSpeed, _stiffness, _damping, MENGINE_DOCUMENT_PYTHON );
        }
        //////////////////////////////////////////////////////////////////////////
        static Box2DJointInterfacePtr createBox2DRevoluteJoint( const Box2DWorldInterfacePtr & _world, const Box2DBodyInterfacePtr & _bodyA, const Box2DBodyInterfacePtr & _bodyB, const mt::vec2f & _anchor, bool _enableLimit, float _lowerAngle, float _upperAngle, bool _enableMotor, float _motorSpeed, float _maxMotorTorque )
        {
            return _world->createRevoluteJoint( _bodyA, _bodyB, _anchor, _enableLimit, _lowerAngle, _upperAngle, _enableMotor, _motorSpeed, _maxMotorTorque, MENGINE_DOCUMENT_PYTHON );
        }
        //////////////////////////////////////////////////////////////////////////
        static Box2DJointInterfacePtr createBox2DMouseJoint( const Box2DWorldInterfacePtr & _world, const Box2DBodyInterfacePtr & _groundBody, const Box2DBodyInterfacePtr & _body, const mt::vec2f & _target, float _hertz, float _dampingRatio, float _maxForce )
        {
            return _world->createMouseJoint( _groundBody, _body, _target, _hertz, _dampingRatio, _maxForce, MENGINE_DOCUMENT_PYTHON );
        }
        //////////////////////////////////////////////////////////////////////////
        static pybind::list overlapBox2DCircle( pybind::kernel_interface * _kernel, const Box2DWorldInterfacePtr & _world, const mt::vec2f & _position, float _radius, uint32_t _categoryBits, uint32_t _maskBits )
        {
            Box2DBodyInterface * bodies[64] = {};
            uint32_t count = _world->overlapCircle( _position, _radius, _categoryBits, _maskBits, bodies, 64 );
            pybind::list result( _kernel );

            for( uint32_t index = 0; index != count; ++index )
            {
                result.append( Box2DBodyInterfacePtr::from( bodies[index] ) );
            }

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
        static Box2DContactListenerInterfacePtr setBox2DContactCallback( const Box2DWorldInterfacePtr & _world, const pybind::object & _callback, const pybind::args & _args )
        {
            PythonBox2DContactListenerPtr listener = Helper::makeFactorableUnique<PythonBox2DContactListener>( MENGINE_DOCUMENT_PYTHON, _callback, _args );
            _world->setContactListener( listener );
            return listener;
        }
        //////////////////////////////////////////////////////////////////////////
        static void clearBox2DContactCallback( const Box2DWorldInterfacePtr & _world )
        {
            _world->setContactListener( nullptr );
        }
        //////////////////////////////////////////////////////////////////////////
        static void rayCastBox2D( const Box2DWorldInterfacePtr & _world, const mt::vec2f & _pointA, const mt::vec2f & _pointB, const pybind::object & _callback, const pybind::args & _args )
        {
            PythonBox2DRayCastPtr response = Helper::makeFactorableUnique<PythonBox2DRayCast>( MENGINE_DOCUMENT_PYTHON, _callback, _args );
            _world->rayCast( _pointA, _pointB, response );
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    Box2DScriptEmbedding::Box2DScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Box2DScriptEmbedding::~Box2DScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool Box2DScriptEmbedding::embed( pybind::kernel_interface * _kernel )
    {
        pybind::interface_<Box2DBodyInterface, pybind::bases<Mixin>>( _kernel, "Box2DBody" )
            .def( "isValid", &Box2DBodyInterface::isValid )
            .def( "setNode", &Box2DBodyInterface::setNode )
            .def( "getNode", &Box2DBodyInterface::getNode )
            .def( "addShapeConvex", &Box2DBodyInterface::addShapeConvex )
            .def( "addShapeCircle", &Box2DBodyInterface::addShapeCircle )
            .def( "addShapeBox", &Box2DBodyInterface::addShapeBox )
            .def( "clearShapes", &Box2DBodyInterface::clearShapes )
            .def( "getPosition", &Box2DBodyInterface::getBodyPosition )
            .def( "getAngle", &Box2DBodyInterface::getBodyAngle )
            .def( "getWorldVector", &Box2DBodyInterface::getBodyWorldVector )
            .def( "getMass", &Box2DBodyInterface::getBodyMass )
            .def( "getInertia", &Box2DBodyInterface::getBodyInertiaTensor )
            .def( "setLinearVelocity", &Box2DBodyInterface::setBodyLinearVelocity )
            .def( "getLinearVelocity", &Box2DBodyInterface::getBodyLinearVelocity )
            .def( "setAngularVelocity", &Box2DBodyInterface::setBodyAngularVelocity )
            .def( "getAngularVelocity", &Box2DBodyInterface::getBodyAngularVelocity )
            .def( "applyForce", &Box2DBodyInterface::applyForce )
            .def( "applyImpulse", &Box2DBodyInterface::applyImpulse )
            .def( "applyAngularImpulse", &Box2DBodyInterface::applyAngularImpulse )
            .def( "applyTorque", &Box2DBodyInterface::applyTorque )
            .def( "isFrozen", &Box2DBodyInterface::isFrozen )
            .def( "isSleeping", &Box2DBodyInterface::isSleeping )
            .def( "isStatic", &Box2DBodyInterface::isStatic )
            .def( "isKinematic", &Box2DBodyInterface::isKinematic )
            .def( "isDynamic", &Box2DBodyInterface::isDynamic )
            .def( "setTransform", &Box2DBodyInterface::setTransform )
            .def( "setLinearDamping", &Box2DBodyInterface::setBodyLinearDamping )
            .def( "getLinearDamping", &Box2DBodyInterface::getBodyLinearDamping )
            .def( "setAngularDamping", &Box2DBodyInterface::setBodyAngularDamping )
            .def( "getAngularDamping", &Box2DBodyInterface::getBodyAngularDamping )
            .def( "setGravityScale", &Box2DBodyInterface::setBodyGravityScale )
            .def( "getGravityScale", &Box2DBodyInterface::getBodyGravityScale )
            .def( "setFixedRotation", &Box2DBodyInterface::setBodyFixedRotation )
            .def( "isFixedRotation", &Box2DBodyInterface::isBodyFixedRotation )
            .def( "setBullet", &Box2DBodyInterface::setBodyBulletMode )
            .def( "isBullet", &Box2DBodyInterface::isBodyBulletMode )
            .def( "sleep", &Box2DBodyInterface::sleep )
            .def( "wakeUp", &Box2DBodyInterface::wakeUp )
            .def( "setFilterData", &Box2DBodyInterface::setFilterData )
            ;

        pybind::interface_<Box2DJointInterface, pybind::bases<Mixin>>( _kernel, "Box2DJoint" )
            .def( "isValid", &Box2DJointInterface::isValid )
            .def( "setMouseTarget", &Box2DJointInterface::setMouseTarget )
            .def( "getMouseTarget", &Box2DJointInterface::getMouseTarget )
            ;

        pybind::interface_<Box2DWorldInterface, pybind::bases<Mixin>>( _kernel, "Box2DWorld" )
            .def( "isValid", &Box2DWorldInterface::isValid )
            .def( "setTimeStep", &Box2DWorldInterface::setTimeStep )
            .def( "setGravity", &Box2DWorldInterface::setGravity )
            .def( "getGravity", &Box2DWorldInterface::getGravity )
            ;

        pybind::interface_<Box2DContactListenerInterface, pybind::bases<Mixin>>( _kernel, "Box2DContactListener" );

        pybind::def_const<uint32_t>( _kernel, "BOX2D_BODY_STATIC", EBOX2D_BODY_STATIC );
        pybind::def_const<uint32_t>( _kernel, "BOX2D_BODY_KINEMATIC", EBOX2D_BODY_KINEMATIC );
        pybind::def_const<uint32_t>( _kernel, "BOX2D_BODY_DYNAMIC", EBOX2D_BODY_DYNAMIC );
        pybind::def_const<uint32_t>( _kernel, "BOX2D_CONTACT_BEGIN", EBOX2D_CONTACT_BEGIN );
        pybind::def_const<uint32_t>( _kernel, "BOX2D_CONTACT_END", EBOX2D_CONTACT_END );
        pybind::def_const<uint32_t>( _kernel, "BOX2D_SENSOR_BEGIN", EBOX2D_SENSOR_BEGIN );
        pybind::def_const<uint32_t>( _kernel, "BOX2D_SENSOR_END", EBOX2D_SENSOR_END );
        pybind::def_const<uint32_t>( _kernel, "BOX2D_CONTACT_HIT", EBOX2D_CONTACT_HIT );

        pybind::def_function( _kernel, "createBox2DWorld", &Detail::createBox2DWorld );
        pybind::def_function( _kernel, "destroyBox2DWorld", &Detail::destroyBox2DWorld );
        pybind::def_function( _kernel, "isSameBox2DBody", &Detail::isSameBox2DBody );
        pybind::def_function( _kernel, "createBox2DBody", &Detail::createBox2DBody );
        pybind::def_function( _kernel, "createBox2DDistanceJoint", &Detail::createBox2DDistanceJoint );
        pybind::def_function( _kernel, "createBox2DHingeJoint", &Detail::createBox2DHingeJoint );
        pybind::def_function( _kernel, "createBox2DPrismaticJoint", &Detail::createBox2DPrismaticJoint );
        pybind::def_function( _kernel, "createBox2DWeldJoint", &Detail::createBox2DWeldJoint );
        pybind::def_function( _kernel, "createBox2DWheelJoint", &Detail::createBox2DWheelJoint );
        pybind::def_function( _kernel, "createBox2DRevoluteJoint", &Detail::createBox2DRevoluteJoint );
        pybind::def_function( _kernel, "createBox2DMouseJoint", &Detail::createBox2DMouseJoint );
        pybind::def_function_kernel( _kernel, "overlapBox2DCircle", &Detail::overlapBox2DCircle );
        pybind::def_function_args( _kernel, "setBox2DContactCallback", &Detail::setBox2DContactCallback );
        pybind::def_function( _kernel, "clearBox2DContactCallback", &Detail::clearBox2DContactCallback );
        pybind::def_function_args( _kernel, "rayCastBox2D", &Detail::rayCastBox2D );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Box2DScriptEmbedding::eject( pybind::kernel_interface * _kernel )
    {
        _kernel->remove_scope<Box2DContactListenerInterface>();
        _kernel->remove_scope<Box2DWorldInterface>();
        _kernel->remove_scope<Box2DJointInterface>();
        _kernel->remove_scope<Box2DBodyInterface>();
    }
    //////////////////////////////////////////////////////////////////////////
}
