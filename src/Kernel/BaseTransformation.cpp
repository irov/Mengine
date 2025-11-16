#include "Kernel/BaseTransformation.h"

#include "Kernel/Assertion.h"

#include "Config/StdAlgorithm.h"

#include "math/angle.h"
#include "math/quat.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    BaseTransformation::BaseTransformation()
        : m_relationTransformation( nullptr )
        , m_position( 0.f, 0.f, 0.f )
        , m_origin( 0.f, 0.f, 0.f )
        , m_skew( 0.f, 0.f )
        , m_scale( 1.f, 1.f, 1.f )
        , m_orientation( 0.f, 0.f, 0.f )
        , m_transformationFlag( TRANSFORMATION_INVALIDATE_IDENTITY )
        , m_identityWorldMatrix( true )
        , m_invalidateLocalMatrix( false )
        , m_invalidateWorldMatrix( false )
    {
        mt::ident_m4( &m_localMatrix );
        mt::ident_m4( &m_worldMatrix );
    }
    //////////////////////////////////////////////////////////////////////////
    BaseTransformation::~BaseTransformation()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseTransformation::setRelationTransformation( TransformationInterface * _relationTransformation )
    {
        MENGINE_ASSERTION_FATAL( _relationTransformation != nullptr, "set nullptr relation transformation" );
        MENGINE_ASSERTION_FATAL( _relationTransformation != this, "set this relation transformation" );

        if( m_relationTransformation != nullptr )
        {
            m_relationTransformation->removeRelationTransformationChild_( this );
        }

        m_relationTransformation = static_cast<BaseTransformation *>(_relationTransformation);

        m_relationTransformation->addRelationTransformationChild_( this );

        if( m_relationTransformation->isIdentityWorldMatrix() == true && this->isIdentityWorldMatrix() == true )
        {
            return;
        }

        this->invalidateWorldMatrix();
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseTransformation::removeRelationTransformation()
    {
        if( m_relationTransformation == nullptr )
        {
            return;
        }

        bool identityPrevTransformation = m_relationTransformation->isIdentityWorldMatrix();

        m_relationTransformation->removeRelationTransformationChild_( this );
        m_relationTransformation = nullptr;

        if( identityPrevTransformation == true )
        {
            return;
        }

        this->invalidateWorldMatrix();
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseTransformation::addRelationTransformationChild_( BaseTransformation * _childTransformation )
    {
        m_relationChildren.push_back( _childTransformation );
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseTransformation::removeRelationTransformationChild_( BaseTransformation * _childTransformation )
    {
        VectorTransformation::iterator it_erase = StdAlgorithm::find( m_relationChildren.begin(), m_relationChildren.end(), _childTransformation );

        MENGINE_ASSERTION_FATAL( it_erase != m_relationChildren.end(), "remove relation transformation not found" );

        *it_erase = m_relationChildren.back();
        m_relationChildren.pop_back();
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseTransformation::invalidateWorldMatrix() const
    {
        if( m_invalidateWorldMatrix == true )
        {
            return;
        }

        m_invalidateWorldMatrix = true;

        for( BaseTransformation * child : m_relationChildren )
        {
            child->invalidateWorldMatrix();
        }

        this->_invalidateWorldMatrix();
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseTransformation::_invalidateWorldMatrix() const
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseTransformation::invalidateLocalMatrix() const
    {
        m_invalidateLocalMatrix = true;

        this->invalidateWorldMatrix();
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseTransformation::setLocalPosition( const mt::vec3f & _position )
    {
        if( mt::cmp_v3_v3( m_position, _position ) == true )
        {
            return;
        }

        m_position = _position;

        if( mt::is_ident_v3( m_position ) == true )
        {
            m_transformationFlag &= ~TRANSFORMATION_INVALIDATE_POSITION;
        }
        else
        {
            m_transformationFlag |= TRANSFORMATION_INVALIDATE_POSITION;
        }

        this->invalidateLocalMatrix();
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseTransformation::setLocalPositionX( float _x )
    {
        if( mt::equal_f_f( m_position.x, _x ) == true )
        {
            return;
        }

        m_position.x = _x;

        if( mt::is_ident_v3( m_position ) == true )
        {
            m_transformationFlag &= ~TRANSFORMATION_INVALIDATE_POSITION;
        }
        else
        {
            m_transformationFlag |= TRANSFORMATION_INVALIDATE_POSITION;
        }

        this->invalidateLocalMatrix();
    }
    //////////////////////////////////////////////////////////////////////////
    float BaseTransformation::getLocalPositionX() const
    {
        return m_position.x;
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseTransformation::setLocalPositionY( float _y )
    {
        if( mt::equal_f_f( m_position.y, _y ) == true )
        {
            return;
        }

        m_position.y = _y;

        if( mt::is_ident_v3( m_position ) == true )
        {
            m_transformationFlag &= ~TRANSFORMATION_INVALIDATE_POSITION;
        }
        else
        {
            m_transformationFlag |= TRANSFORMATION_INVALIDATE_POSITION;
        }

        this->invalidateLocalMatrix();
    }
    //////////////////////////////////////////////////////////////////////////
    float BaseTransformation::getLocalPositionY() const
    {
        return m_position.y;
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseTransformation::setLocalPositionZ( float _z )
    {
        if( mt::equal_f_f( m_position.z, _z ) == true )
        {
            return;
        }

        m_position.z = _z;

        if( mt::is_ident_v3( m_position ) == true )
        {
            m_transformationFlag &= ~TRANSFORMATION_INVALIDATE_POSITION;
        }
        else
        {
            m_transformationFlag |= TRANSFORMATION_INVALIDATE_POSITION;
        }

        this->invalidateLocalMatrix();
    }
    //////////////////////////////////////////////////////////////////////////
    float BaseTransformation::getLocalPositionZ() const
    {
        return m_position.z;
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseTransformation::moveLocalPosition( const mt::vec3f & _delta )
    {
        const mt::vec3f & position = this->getLocalPosition();

        mt::vec3f new_position = position + _delta;

        this->setLocalPosition( new_position );
    }
    //////////////////////////////////////////////////////////////////////////
    bool BaseTransformation::isIdentityPosition() const
    {
        return (m_transformationFlag & TRANSFORMATION_INVALIDATE_POSITION) == TRANSFORMATION_INVALIDATE_POSITION;
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseTransformation::setLocalOrientationX( float _angle )
    {
        if( mt::equal_f_f( m_orientation.x, _angle ) == true )
        {
            return;
        }

        m_orientation.x = _angle;

        if( mt::equal_f_z( m_orientation.x ) == true )
        {
            m_transformationFlag &= ~TRANSFORMATION_INVALIDATE_ORIENTATION_X;
        }
        else
        {
            m_transformationFlag |= TRANSFORMATION_INVALIDATE_ORIENTATION_X;
        }

        this->invalidateLocalMatrix();
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseTransformation::setLocalOrientationY( float _angle )
    {
        if( mt::equal_f_f( m_orientation.y, _angle ) == true )
        {
            return;
        }

        m_orientation.y = _angle;

        if( mt::equal_f_z( m_orientation.y ) == true )
        {
            m_transformationFlag &= ~TRANSFORMATION_INVALIDATE_ORIENTATION_Y;
        }
        else
        {
            m_transformationFlag |= TRANSFORMATION_INVALIDATE_ORIENTATION_Y;
        }

        this->invalidateLocalMatrix();
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseTransformation::setLocalOrientationZ( float _angle )
    {
        if( mt::equal_f_f( m_orientation.z, _angle ) == true )
        {
            return;
        }

        m_orientation.z = _angle;

        if( mt::equal_f_z( m_orientation.z ) == true )
        {
            m_transformationFlag &= ~TRANSFORMATION_INVALIDATE_ORIENTATION_Z;
        }
        else
        {
            m_transformationFlag |= TRANSFORMATION_INVALIDATE_ORIENTATION_Z;
        }

        this->invalidateLocalMatrix();
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseTransformation::setLocalOrientation( const mt::vec3f & _orientation )
    {
        if( mt::cmp_v3_v3( m_orientation, _orientation ) == true )
        {
            return;
        }

        m_orientation = _orientation;

        if( mt::equal_f_z( m_orientation.x ) == true )
        {
            m_transformationFlag &= ~TRANSFORMATION_INVALIDATE_ORIENTATION_X;
        }
        else
        {
            m_transformationFlag |= TRANSFORMATION_INVALIDATE_ORIENTATION_X;
        }

        if( mt::equal_f_z( m_orientation.y ) == true )
        {
            m_transformationFlag &= ~TRANSFORMATION_INVALIDATE_ORIENTATION_Y;
        }
        else
        {
            m_transformationFlag |= TRANSFORMATION_INVALIDATE_ORIENTATION_Y;
        }

        if( mt::equal_f_z( m_orientation.z ) == true )
        {
            m_transformationFlag &= ~TRANSFORMATION_INVALIDATE_ORIENTATION_Z;
        }
        else
        {
            m_transformationFlag |= TRANSFORMATION_INVALIDATE_ORIENTATION_Z;
        }

        this->invalidateLocalMatrix();
    }
    //////////////////////////////////////////////////////////////////////////
    bool BaseTransformation::isIdentityOrientation() const
    {
        return (m_transformationFlag & TRANSFORMATION_INVALIDATE_ORIENTATION) == TRANSFORMATION_INVALIDATE_ORIENTATION;
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseTransformation::setTransformationData( uint8_t _transformationFlag, const mt::vec3f & _position, const mt::vec3f & _origin, const mt::vec3f & _scale, const mt::vec2f & _skew, const mt::vec3f & _orientation )
    {
        m_position = _position;
        m_origin = _origin;
        m_scale = _scale;
        m_skew = _skew;
        m_orientation = _orientation;

        if( _transformationFlag == TRANSFORMATION_INVALIDATE_UNKNOWN )
        {
            m_transformationFlag = TRANSFORMATION_INVALIDATE_IDENTITY;

            if( mt::is_ident_v3( m_position ) == false )
            {
                m_transformationFlag |= TRANSFORMATION_INVALIDATE_POSITION;
            }

            if( mt::is_ident_v3( m_origin ) == false )
            {
                m_transformationFlag |= TRANSFORMATION_INVALIDATE_ORIGIN;
            }

            if( mt::is_one_v3( m_scale ) == false )
            {
                m_transformationFlag |= TRANSFORMATION_INVALIDATE_SCALE;
            }

            if( mt::is_ident_v2( m_skew ) == false )
            {
                m_transformationFlag |= TRANSFORMATION_INVALIDATE_SKEW;
            }

            if( mt::equal_f_z( m_orientation.x ) == false )
            {
                m_transformationFlag |= TRANSFORMATION_INVALIDATE_ORIENTATION_X;
            }

            if( mt::equal_f_z( m_orientation.y ) == false )
            {
                m_transformationFlag |= TRANSFORMATION_INVALIDATE_ORIENTATION_Y;
            }

            if( mt::equal_f_z( m_orientation.z ) == false )
            {
                m_transformationFlag |= TRANSFORMATION_INVALIDATE_ORIENTATION_Z;
            }
        }
        else
        {
            m_transformationFlag = _transformationFlag;
        }

        this->invalidateLocalMatrix();
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseTransformation::getTransformationData( uint8_t * const _transformationFlag, mt::vec3f * const _position, mt::vec3f * const _origin, mt::vec3f * const _scale, mt::vec2f * const _skew, mt::vec3f * const _orientation ) const
    {
        *_transformationFlag = m_transformationFlag;
        *_position = m_position;
        *_origin = m_origin;
        *_scale = m_scale;
        *_skew = m_skew;
        *_orientation = m_orientation;
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseTransformation::calcWorldMatrix( mt::mat4f * _wm, uint8_t _transformationFlag, const mt::vec3f & _position, const mt::vec3f & _origin, const mt::vec3f & _scale, const mt::vec2f & _skew, const mt::vec3f & _orientation ) const
    {
        mt::mat4f localMatrix;
        BaseTransformation::makeLocalMatrix( &localMatrix, _transformationFlag, _position, _origin, _scale, _skew, _orientation );

        if( m_relationTransformation == nullptr )
        {
            *_wm = localMatrix;
        }
        else
        {
            bool identityWorldMatrix = m_relationTransformation->isIdentityWorldMatrix();

            if( identityWorldMatrix == false )
            {
                const mt::mat4f & relationMatrix = m_relationTransformation->getWorldMatrix();

                if( _transformationFlag != 0 )
                {
                    mt::mul_m4_m4_r( _wm, localMatrix, relationMatrix );
                }
                else
                {
                    *_wm = relationMatrix;
                }
            }
            else
            {
                if( _transformationFlag != 0 )
                {
                    *_wm = localMatrix;
                }
                else
                {
                    mt::ident_m4( _wm );
                }
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseTransformation::resetTransformation()
    {
        m_position = mt::vec3f( 0.f, 0.f, 0.f );
        m_origin = mt::vec3f( 0.f, 0.f, 0.f );
        m_skew = mt::vec2f( 0.f, 0.f );
        m_scale = mt::vec3f( 1.f, 1.f, 1.f );
        m_orientation = mt::vec3f( 0.f, 0.f, 0.f );

        m_transformationFlag = TRANSFORMATION_INVALIDATE_IDENTITY;

        this->invalidateLocalMatrix();
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseTransformation::translate( const mt::vec3f & _delta )
    {
        mt::vec3f new_position = m_position + _delta;

        this->setLocalPosition( new_position );
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseTransformation::rotate( const mt::vec3f & _euler )
    {
        mt::vec3f new_euler = m_orientation + _euler;

        this->setLocalOrientation( new_euler );
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseTransformation::coordinate( const mt::vec3f & _delta )
    {
        mt::vec3f new_position = m_position + _delta;
        mt::vec3f new_origin = m_origin + _delta;

        this->setLocalPosition( new_position );
        this->setLocalOrigin( new_origin );
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseTransformation::setDirection( const mt::vec3f & _direction, const mt::vec3f & _up )
    {
        mt::mat4f mr;
        mt::make_rotate_m4_direction( &mr, _direction, _up );

        mt::vec3f orientation;
        mt::make_euler_angles( &orientation, mr );

        this->setLocalOrientation( orientation );
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseTransformation::setBillboard( const mt::vec3f & _direction, const mt::vec3f & _normal )
    {
        mt::mat4f mr;
        mt::make_rotate_m4_fixed_up( &mr, _direction, _normal );

        mt::vec3f orientation;
        mt::make_euler_angles( &orientation, mr );

        this->setLocalOrientation( orientation );
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseTransformation::billboardAt( const mt::vec3f & _position, const mt::vec3f & _at, const mt::vec3f & _normal )
    {
        mt::vec3f dir = _at - _position;

        mt::mat4f mr;
        mt::make_rotate_m4_fixed_up( &mr, dir, _normal );

        mt::vec3f orientation;
        mt::make_euler_angles( &orientation, mr );

        this->setLocalOrientation( orientation );

        this->setLocalPosition( _position );
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseTransformation::setAxes( const mt::vec3f & _direction, const mt::vec3f & _left, const mt::vec3f & _up )
    {
        mt::mat4f mr;
        mt::make_rotate_m4_axes( &mr, _direction, _left, _up );

        mt::vec3f orientation;
        mt::make_euler_angles( &orientation, mr );

        this->setLocalOrientation( orientation );
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseTransformation::lookAt( const mt::vec3f & _position, const mt::vec3f & _at, const mt::vec3f & _up )
    {
        mt::vec3f dir = _at - _position;

        mt::mat4f mr;
        mt::make_rotate_m4_direction( &mr, dir, _up );

        mt::vec3f orientation;
        mt::make_euler_angles( &orientation, mr );

        this->setLocalOrientation( orientation );

        this->setLocalPosition( _position );
    }
    //////////////////////////////////////////////////////////////////////////
    mt::vec3f BaseTransformation::getAxisDirection() const
    {
        mt::mat4f mat_rot;
        mt::make_rotate_m4_euler( &mat_rot, m_orientation.x, m_orientation.y, m_orientation.z );

        mt::vec3f axis = mat_rot.v0.to_vec3f();

        return axis;
    }
    //////////////////////////////////////////////////////////////////////////
    mt::vec3f BaseTransformation::getAxisLeft() const
    {
        mt::mat4f mat_rot;
        mt::make_rotate_m4_euler( &mat_rot, m_orientation.x, m_orientation.y, m_orientation.z );

        mt::vec3f axis = mat_rot.v1.to_vec3f();

        return axis;
    }
    //////////////////////////////////////////////////////////////////////////
    mt::vec3f BaseTransformation::getAxisUp() const
    {
        mt::mat4f mat_rot;
        mt::make_rotate_m4_euler( &mat_rot, m_orientation.x, m_orientation.y, m_orientation.z );

        mt::vec3f axis = mat_rot.v2.to_vec3f();

        return axis;
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseTransformation::updateLocalMatrix() const
    {
        m_invalidateLocalMatrix = false;

        BaseTransformation::makeLocalMatrix( &m_localMatrix, m_transformationFlag, m_position, m_origin, m_scale, m_skew, m_orientation );
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseTransformation::makeLocalMatrix( mt::mat4f * const _lm, uint8_t _transformationFlag, const mt::vec3f & _position, const mt::vec3f & _origin, const mt::vec3f & _scale, const mt::vec2f & _skew, const mt::vec3f & _orientation )
    {
        switch( _transformationFlag )
        {
        case TRANSFORMATION_INVALIDATE_IDENTITY:
            {
                mt::ident_m4( _lm );

                return;
            }break;
        case TRANSFORMATION_INVALIDATE_POSITION:
            {
                mt::make_translation_m4( _lm, _position.x, _position.y, _position.z );

                return;
            }break;
        case TRANSFORMATION_INVALIDATE_ORIGIN:
            {
                mt::make_translation_m4( _lm, -_origin.x, -_origin.y, -_origin.z );

                return;
            }break;
        case TRANSFORMATION_INVALIDATE_SCALE:
            {
                mt::make_scale_m4( _lm, _scale.x, _scale.y, _scale.z );

                return;
            }break;
        case TRANSFORMATION_INVALIDATE_SKEW:
            {
                mt::make_skew_m4( _lm, _skew.x, _skew.y );

                return;
            }break;
        case TRANSFORMATION_INVALIDATE_ORIENTATION_X:
            {
                mt::make_rotate_z_axis_m4( _lm, _orientation.x );

                return;
            }break;
        case TRANSFORMATION_INVALIDATE_POSITION | TRANSFORMATION_INVALIDATE_ORIGIN:
            {
                mt::make_translation_m4( _lm, _position.x - _origin.x, _position.y - _origin.y, _position.z - _origin.z );

                return;
            }break;
        default:
            {
            }break;
        }

        mt::mat4f mat_base;

        mat_base.v0.x = _scale.x;
        mat_base.v0.y = _skew.y;
        mat_base.v0.z = 0.f;
        mat_base.v0.w = 0.f;
        mat_base.v1.x = _skew.x;
        mat_base.v1.y = _scale.y;
        mat_base.v1.z = 0.f;
        mat_base.v1.w = 0.f;
        mat_base.v2.x = 0.f;
        mat_base.v2.y = 0.f;
        mat_base.v2.z = _scale.z;
        mat_base.v2.w = 0.f;

        if( _transformationFlag & TRANSFORMATION_INVALIDATE_ORIGIN )
        {
            mat_base.v3.x = -_origin.x * _scale.x;
            mat_base.v3.y = -_origin.y * _scale.y;
            mat_base.v3.z = -_origin.z * _scale.z;
        }
        else
        {
            mat_base.v3.x = 0.f;
            mat_base.v3.y = 0.f;
            mat_base.v3.z = 0.f;
        }

        mat_base.v3.w = 1.f;

        if( _transformationFlag & TRANSFORMATION_INVALIDATE_ORIENTATION_YZ )
        {
            mt::mat4f mat_rot;
            mt::make_rotate_m4_euler( &mat_rot, _orientation.x, _orientation.y, _orientation.z );

            mt::mul_m4_m4_r( _lm, mat_base, mat_rot );
        }
        else if( _transformationFlag & TRANSFORMATION_INVALIDATE_ORIENTATION_X )
        {
            mt::mat4f mat_rot;
            mt::make_rotate_z_axis_m4( &mat_rot, _orientation.x );

            mt::mul_m4_m4_r( _lm, mat_base, mat_rot );
        }
        else
        {
            *_lm = mat_base;
        }

        if( _transformationFlag & TRANSFORMATION_INVALIDATE_POSITION )
        {
            _lm->v3.x += _position.x;
            _lm->v3.y += _position.y;
            _lm->v3.z += _position.z;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseTransformation::updateWorldMatrix() const
    {
        m_invalidateWorldMatrix = false;

        bool identityLocalMatrix = this->isIdentityLocalMatrix();

        if( m_relationTransformation == nullptr )
        {
            const mt::mat4f & localMatrix = this->getLocalMatrix();

            m_worldMatrix = localMatrix;

            m_identityWorldMatrix = identityLocalMatrix;
        }
        else
        {
            bool identityRelationWorldMatrix = m_relationTransformation->isIdentityWorldMatrix();

            if( identityRelationWorldMatrix == false )
            {
                const mt::mat4f & relationMatrix = m_relationTransformation->getWorldMatrix();

                if( identityLocalMatrix == false )
                {
                    const mt::mat4f & localMatrix = this->getLocalMatrix();

                    mt::mul_m4_m4_r( &m_worldMatrix, localMatrix, relationMatrix );
                }
                else
                {
                    m_worldMatrix = relationMatrix;
                }

                m_identityWorldMatrix = false;
            }
            else
            {
                if( identityLocalMatrix == false )
                {
                    const mt::mat4f & localMatrix = this->getLocalMatrix();

                    m_worldMatrix = localMatrix;

                    m_identityWorldMatrix = false;
                }
                else
                {
                    mt::ident_m4( &m_worldMatrix );

                    m_identityWorldMatrix = true;
                }
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseTransformation::setWorldPosition( const mt::vec3f & _worldPosition )
    {
        if( m_relationTransformation != nullptr )
        {
            const mt::mat4f & rwm = m_relationTransformation->getWorldMatrix();

            mt::mat4f rwm_inv;
            mt::inv_m4_m4( &rwm_inv, rwm );

            mt::vec3f new_lp;
            mt::mul_v3_v3_m4( &new_lp, _worldPosition, rwm_inv );

            const mt::mat4f & lm = this->getLocalMatrix();

            mt::mat4f lm_inv;
            mt::inv_m4_m4( &lm_inv, lm );

            mt::vec3f pp;
            mt::mul_v3_v3_m4( &pp, new_lp, lm_inv );

            mt::vec3f pp2;
            mt::mul_v3_v3_m4_r( &pp2, pp, lm );

            this->translate( pp2 );
        }
        else
        {
            const mt::mat4f & lm = this->getLocalMatrix();

            mt::mat4f lm_inv;
            mt::inv_m4_m4( &lm_inv, lm );

            mt::vec3f pp;
            mt::mul_v3_v3_m4( &pp, _worldPosition, lm_inv );

            mt::vec3f pp2;
            mt::mul_v3_v3_m4_r( &pp2, pp, lm );

            this->translate( pp2 );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    mt::vec3f BaseTransformation::getWorldPosition() const
    {
        const mt::mat4f & wm = this->getWorldMatrix();

        mt::vec3f wp;
        wp.x = wm.v3.x;
        wp.y = wm.v3.y;
        wp.z = wm.v3.z;

        return wp;
    }
    //////////////////////////////////////////////////////////////////////////
    mt::vec3f BaseTransformation::getWorldScale() const
    {
        const mt::mat4f & wm = this->getWorldMatrix();

        mt::vec3f ws;
        ws.x = wm.v0.x;
        ws.y = wm.v1.y;
        ws.z = wm.v2.z;

        return ws;
    }
    //////////////////////////////////////////////////////////////////////////
    mt::vec3f BaseTransformation::getWorldOrientation() const
    {
        const mt::mat4f & wm = this->getWorldMatrix();

        mt::quatf q;
        mt::q_from_rot_m4( &q, wm );

        mt::vec3f wo;
        mt::quat_to_euler( q, &wo );

        return wo;
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseTransformation::setLocalOrigin( const mt::vec3f & _origin )
    {
        if( mt::cmp_v3_v3( m_origin, _origin ) == true )
        {
            return;
        }

        m_origin = _origin;

        if( mt::is_ident_v3( m_origin ) == true )
        {
            m_transformationFlag &= ~TRANSFORMATION_INVALIDATE_ORIGIN;
        }
        else
        {
            m_transformationFlag |= TRANSFORMATION_INVALIDATE_ORIGIN;
        }

        this->invalidateLocalMatrix();
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseTransformation::setLocalScale( const mt::vec3f & _scale )
    {
        if( mt::cmp_v3_v3( m_scale, _scale ) == true )
        {
            return;
        }

        m_scale = _scale;

        if( mt::is_one_v3( m_scale ) == true )
        {
            m_transformationFlag &= ~TRANSFORMATION_INVALIDATE_SCALE;
        }
        else
        {
            m_transformationFlag |= TRANSFORMATION_INVALIDATE_SCALE;
        }

        this->invalidateLocalMatrix();
    }
    //////////////////////////////////////////////////////////////////////////
    void BaseTransformation::setLocalSkew( const mt::vec2f & _skew )
    {
        if( mt::cmp_v2_v2( m_skew, _skew ) == true )
        {
            return;
        }

        m_skew = _skew;

        if( mt::is_ident_v2( m_skew ) == true )
        {
            m_transformationFlag &= ~TRANSFORMATION_INVALIDATE_SKEW;
        }
        else
        {
            m_transformationFlag |= TRANSFORMATION_INVALIDATE_SKEW;
        }

        this->invalidateLocalMatrix();
    }
    //////////////////////////////////////////////////////////////////////////
}
