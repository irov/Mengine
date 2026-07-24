#include "Arena3DHud.h"
#include "Arena3DHelper.h"

#include "Interface/ApplicationInterface.h"
#include "Interface/PrototypeServiceInterface.h"

#include "Kernel/ConstStringHelper.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/Scene.h"
#include "Kernel/ShapeQuadFixed.h"

#include "Config/StdAlgorithm.h"
#include "Config/StdMath.h"

#include "math/mat4.h"

//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static constexpr size_t weaponIndex( Arena3D::WeaponType _weapon )
        {
            return static_cast<size_t>(_weapon);
        }
        //////////////////////////////////////////////////////////////////////////
        static Color weaponHudColor( Arena3D::WeaponType _weapon, float _alpha )
        {
            static const mt::vec3f colors[] = {
                {1.f, 0.62f, 0.18f}, {1.f, 0.22f, 0.04f}, {0.25f, 0.75f, 1.f},
                {0.05f, 1.f, 0.72f}, {1.f, 0.55f, 0.10f}, {1.f, 0.78f, 0.28f}};
            const mt::vec3f & color = colors[weaponIndex( _weapon )];
            return Color( color.x, color.y, color.z, _alpha );
        }
        //////////////////////////////////////////////////////////////////////////
        static uint8_t digitMask( uint32_t _digit )
        {
            static const uint8_t masks[] = {
                0x3Fu, 0x06u, 0x5Bu, 0x4Fu, 0x66u,
                0x6Du, 0x7Du, 0x07u, 0x7Fu, 0x6Fu};
            return _digit < 10u ? masks[_digit] : 0u;
        }
        //////////////////////////////////////////////////////////////////////////
        static float dot3( const mt::vec3f & _left, const mt::vec3f & _right )
        {
            return _left.x * _right.x + _left.y * _right.y + _left.z * _right.z;
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    Arena3DHud::Arena3DHud()
        : m_scene( nullptr )
        , m_damageIndicatorTime( 0.f )
        , m_visibleDamageNumbers( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Arena3DHud::~Arena3DHud()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void Arena3DHud::initialize( Scene * _scene, float _width, float _height )
    {
        m_scene = _scene;

        const mt::vec2f center( _width * 0.5f, _height * 0.5f );
        m_crosshairHorizontal = this->createQuad_( {18.f, 2.f}, {0.8f, 1.f, 0.95f, 0.9f}, &m_crosshairHorizontalSurface );
        m_crosshairVertical = this->createQuad_( {2.f, 18.f}, {0.8f, 1.f, 0.95f, 0.9f}, &m_crosshairVerticalSurface );
        m_crosshairHorizontal->getTransformation()->setLocalPosition( {center.x - 9.f, center.y - 1.f, 0.f} );
        m_crosshairVertical->getTransformation()->setLocalPosition( {center.x - 1.f, center.y - 9.f, 0.f} );

        SurfaceSolidColorPtr healthBackSurface;
        m_healthBack = this->createQuad_( {244.f, 18.f}, {0.02f, 0.03f, 0.04f, 0.75f}, &healthBackSurface );
        m_healthFill = this->createQuad_( {240.f, 14.f}, {0.2f, 0.9f, 0.45f, 0.95f}, &m_healthFillSurface );
        m_healthBack->getTransformation()->setLocalPosition( {24.f, _height - 42.f, 0.f} );
        m_healthFill->getTransformation()->setLocalPosition( {26.f, _height - 40.f, 0.f} );

        SurfaceSolidColorPtr armorBackSurface;
        m_armorBack = this->createQuad_( {164.f, 12.f}, {0.02f, 0.03f, 0.04f, 0.75f}, &armorBackSurface );
        m_armorFill = this->createQuad_( {160.f, 8.f}, {0.15f, 0.55f, 1.f, 0.95f}, &m_armorFillSurface );
        m_armorBack->getTransformation()->setLocalPosition( {24.f, _height - 62.f, 0.f} );
        m_armorFill->getTransformation()->setLocalPosition( {26.f, _height - 60.f, 0.f} );

        SurfaceSolidColorPtr speedBackSurface;
        m_speedBack = this->createQuad_( {204.f, 10.f}, {0.02f, 0.03f, 0.04f, 0.75f}, &speedBackSurface );
        m_speedFill = this->createQuad_( {200.f, 6.f}, {0.12f, 0.9f, 0.85f, 0.9f}, &m_speedFillSurface );
        m_speedBack->getTransformation()->setLocalPosition( {_width - 228.f, _height - 42.f, 0.f} );
        m_speedFill->getTransformation()->setLocalPosition( {_width - 226.f, _height - 40.f, 0.f} );

        m_damageIndicator = this->createQuad_( {7.f, 30.f}, {1.f, 0.12f, 0.04f, 0.f}, &m_damageIndicatorSurface );
        m_damageIndicator->getTransformation()->setLocalPosition( {center.x, center.y - 120.f, 0.f} );

        static const mt::vec2f weaponIconSizes[WeaponSlotCount][WeaponIconPartCount] = {
            {{20.f, 4.f}, {4.f, 12.f}, {10.f, 3.f}},
            {{20.f, 7.f}, {7.f, 11.f}, {8.f, 4.f}},
            {{25.f, 3.f}, {5.f, 11.f}, {8.f, 5.f}},
            {{16.f, 10.f}, {12.f, 4.f}, {4.f, 12.f}},
            {{12.f, 12.f}, {6.f, 5.f}, {4.f, 10.f}},
            {{25.f, 3.f}, {25.f, 3.f}, {5.f, 12.f}}};
        for( size_t weaponIndex = 0; weaponIndex != WeaponSlotCount; ++weaponIndex )
        {
            WeaponVisual & weaponHud = m_weapons[weaponIndex];
            weaponHud.background = this->createQuad_( {58.f, 50.f}, {0.02f, 0.03f, 0.04f, 0.78f}, &weaponHud.backgroundSurface );
            const Arena3D::WeaponType weapon = static_cast<Arena3D::WeaponType>(weaponIndex);
            for( size_t part = 0; part != WeaponIconPartCount; ++part )
            {
                SurfaceSolidColorPtr surface;
                weaponHud.iconParts[part] = this->createQuad_( weaponIconSizes[weaponIndex][part], Detail::weaponHudColor( weapon, 0.95f ), &surface );
            }
            for( DigitVisual & digit : weaponHud.digits )
            {
                this->createDigit_( &digit, Detail::weaponHudColor( weapon, 0.95f ) );
            }
        }

        for( DamageNumberVisual & damageNumber : m_damageNumbers )
        {
            for( DigitVisual & digit : damageNumber.digits )
            {
                this->createDigit_( &digit, {1.f, 0.04f, 0.02f, 1.f} );
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Arena3DHud::finalize()
    {
        for( WeaponVisual & weapon : m_weapons )
        {
            if( weapon.background != nullptr )
            {
                weapon.background->removeFromParent();
            }
            for( NodePtr & node : weapon.iconParts )
            {
                if( node != nullptr )
                {
                    node->removeFromParent();
                }
            }
            for( DigitVisual & digit : weapon.digits )
            {
                for( NodePtr & node : digit.segments )
                {
                    if( node != nullptr )
                    {
                        node->removeFromParent();
                    }
                }
            }
            weapon = WeaponVisual{};
        }

        for( DamageNumberVisual & damageNumber : m_damageNumbers )
        {
            for( DigitVisual & digit : damageNumber.digits )
            {
                for( NodePtr & node : digit.segments )
                {
                    if( node != nullptr )
                    {
                        node->removeFromParent();
                    }
                }
            }
            damageNumber = DamageNumberVisual{};
        }

        const NodePtr nodes[] = {m_crosshairHorizontal, m_crosshairVertical, m_healthFill, m_healthBack,
            m_armorFill, m_armorBack, m_speedFill, m_speedBack, m_damageIndicator};
        for( const NodePtr & node : nodes )
        {
            if( node != nullptr )
            {
                node->removeFromParent();
            }
        }

        m_crosshairHorizontal = nullptr;
        m_crosshairVertical = nullptr;
        m_healthFill = nullptr;
        m_healthBack = nullptr;
        m_armorFill = nullptr;
        m_armorBack = nullptr;
        m_speedFill = nullptr;
        m_speedBack = nullptr;
        m_damageIndicator = nullptr;
        m_crosshairHorizontalSurface = nullptr;
        m_crosshairVerticalSurface = nullptr;
        m_healthFillSurface = nullptr;
        m_armorFillSurface = nullptr;
        m_speedFillSurface = nullptr;
        m_damageIndicatorSurface = nullptr;
        m_damageIndicatorTime = 0.f;
        m_visibleDamageNumbers = 0;
        m_scene = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void Arena3DHud::update( float _seconds, const RenderCameraProjectionPtr & _worldCamera, const mt::vec3f & _cameraPosition, const mt::vec3f & _cameraDirection )
    {
        m_damageIndicatorTime = Mengine::StdAlgorithm::max( 0.f, m_damageIndicatorTime - _seconds * 1.5f );
        this->updateDamageNumbers_( _seconds, _worldCamera, _cameraPosition, _cameraDirection );
    }
    //////////////////////////////////////////////////////////////////////////
    void Arena3DHud::sync( const Arena3D::PlayerState & _player, float _warningFlash )
    {
        const float health = Mengine::StdAlgorithm::max( 0.f, Mengine::StdAlgorithm::min( 1.f, static_cast<float>(_player.health) / 100.f ) );
        const float armor = Mengine::StdAlgorithm::max( 0.f, Mengine::StdAlgorithm::min( 1.f, static_cast<float>(_player.armor) / 100.f ) );
        const kf_fixed_t horizontalSpeed = kf_vec3_length( {_player.velocity.x, 0, _player.velocity.z} );
        const float speed = Mengine::StdAlgorithm::max( 0.f, Mengine::StdAlgorithm::min( 1.f, kf_fixed_to_float( horizontalSpeed ) / 12.f ) );
        m_healthFillSurface->setSolidSize( {240.f * health, 14.f} );
        m_armorFillSurface->setSolidSize( {160.f * armor, 8.f} );
        m_speedFillSurface->setSolidSize( {200.f * speed, 6.f} );
        m_damageIndicatorSurface->setSolidColor( {1.f, 0.12f, 0.04f, m_damageIndicatorTime} );
        const Color crosshair = _warningFlash > 0.f ? Color( 1.f, 0.15f, 0.75f, 1.f ) :
            (_player.fireHeld ? Color( 1.f, 0.32f, 0.14f, 1.f ) : Color( 0.8f, 1.f, 0.95f, 0.9f ));
        m_crosshairHorizontalSurface->setSolidColor( crosshair );
        m_crosshairVerticalSurface->setSolidColor( crosshair );

        static const mt::vec2f iconOffsets[WeaponSlotCount][WeaponIconPartCount] = {
            {{16.f, 10.f}, {30.f, 13.f}, {7.f, 11.f}},
            {{14.f, 10.f}, {29.f, 14.f}, {7.f, 11.f}},
            {{10.f, 10.f}, {31.f, 12.f}, {6.f, 9.f}},
            {{17.f, 7.f}, {7.f, 10.f}, {31.f, 12.f}},
            {{18.f, 7.f}, {21.f, 3.f}, {31.f, 12.f}},
            {{9.f, 8.f}, {9.f, 13.f}, {31.f, 12.f}}};
        const Resolution & resolution = APPLICATION_SERVICE()->getContentResolution();
        constexpr float slotStride = 62.f;
        const float firstX = resolution.getWidthF() * 0.5f - slotStride * static_cast<float>(WeaponSlotCount) * 0.5f;
        const float top = resolution.getHeightF() - 78.f;
        for( size_t weaponIndex = 0; weaponIndex != WeaponSlotCount; ++weaponIndex )
        {
            WeaponVisual & weaponHud = m_weapons[weaponIndex];
            const Arena3D::WeaponType weapon = static_cast<Arena3D::WeaponType>(weaponIndex);
            const bool selected = _player.selectedWeapon == weapon;
            const bool empty = _player.ammo[weaponIndex] == 0;
            const float x = firstX + static_cast<float>(weaponIndex) * slotStride + 2.f;
            weaponHud.background->getTransformation()->setLocalPosition( {x, top, 0.f} );
            weaponHud.backgroundSurface->setSolidColor( selected == true ? Detail::weaponHudColor( weapon, 0.36f ) : Color( 0.02f, 0.03f, 0.04f, 0.78f ) );
            const float iconAlpha = empty == true ? 0.22f : (selected == true ? 1.f : 0.62f);
            for( size_t part = 0; part != WeaponIconPartCount; ++part )
            {
                weaponHud.iconParts[part]->getTransformation()->setLocalPosition( {x + iconOffsets[weaponIndex][part].x, top + iconOffsets[weaponIndex][part].y, 0.f} );
                weaponHud.iconParts[part]->getRender()->setLocalAlpha( iconAlpha );
            }
            this->setNumber_( &weaponHud.digits, _player.ammo[weaponIndex], false, {x + 18.f, top + 31.f}, 0.55f, empty == true ? 0.25f : (selected == true ? 1.f : 0.72f) );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Arena3DHud::showDamageDirection( float _relativeRadians )
    {
        m_damageIndicatorTime = 1.f;
        const Resolution & resolution = APPLICATION_SERVICE()->getContentResolution();
        const float centerX = resolution.getWidthF() * 0.5f;
        const float centerY = resolution.getHeightF() * 0.5f;
        m_damageIndicator->getTransformation()->setLocalPosition( {centerX + StdMath::sinf( _relativeRadians ) * 120.f,
            centerY - StdMath::cosf( _relativeRadians ) * 120.f, 0.f} );
        m_damageIndicator->getTransformation()->setLocalOrientationZ( -_relativeRadians );
    }
    //////////////////////////////////////////////////////////////////////////
    void Arena3DHud::spawnDamageNumber( const Arena3D::ServerEvent & _event, const Arena3D::PlayerState & _target )
    {
        if( _event.amount <= 0 )
        {
            return;
        }

        for( DamageNumberVisual & damageNumber : m_damageNumbers )
        {
            if( damageNumber.active == false || damageNumber.targetId != _event.targetId || damageNumber.age > 0.16f )
            {
                continue;
            }
            damageNumber.amount = Mengine::StdAlgorithm::min<uint32_t>( 999u, damageNumber.amount + static_cast<uint32_t>(_event.amount) );
            damageNumber.position = kf_vec3_add( _target.position, {0, kf_fixed_from_float( 2.2f ), 0} );
            damageNumber.age = 0.f;
            return;
        }

        DamageNumberVisual * selected = nullptr;
        for( DamageNumberVisual & damageNumber : m_damageNumbers )
        {
            if( damageNumber.active == false )
            {
                selected = &damageNumber;
                break;
            }
            if( selected == nullptr || damageNumber.age > selected->age )
            {
                selected = &damageNumber;
            }
        }
        if( selected == nullptr )
        {
            return;
        }

        selected->position = kf_vec3_add( _target.position, {0, kf_fixed_from_float( 2.2f ), 0} );
        selected->targetId = _event.targetId;
        selected->amount = static_cast<uint32_t>(_event.amount);
        selected->age = 0.f;
        selected->duration = 0.9f;
        selected->active = true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Arena3DHud::clearDamageNumbers()
    {
        for( DamageNumberVisual & damageNumber : m_damageNumbers )
        {
            damageNumber.active = false;
            this->setNumber_( &damageNumber.digits, damageNumber.amount, false, {0.f, 0.f}, 1.f, 0.f );
        }
        m_visibleDamageNumbers = 0;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t Arena3DHud::getDrawCallCount() const
    {
        return m_visibleDamageNumbers * static_cast<uint32_t>(DigitCount * DigitSegmentCount) + 2u;
    }
    //////////////////////////////////////////////////////////////////////////
    NodePtr Arena3DHud::createQuad_( const mt::vec2f & _size, const Color & _color, SurfaceSolidColorPtr * _surface )
    {
        SurfaceSolidColorPtr surface = PROTOTYPE_SERVICE()->generatePrototype(
            STRINGIZE_STRING_LOCAL( "Surface" ), STRINGIZE_STRING_LOCAL( "SurfaceSolidColor" ), MENGINE_DOCUMENT_FUNCTION );
        surface->setSolidColor( _color );
        surface->setSolidSize( _size );
        ShapeQuadFixedPtr quad = PROTOTYPE_SERVICE()->generatePrototype(
            STRINGIZE_STRING_LOCAL( "Node" ), STRINGIZE_STRING_LOCAL( "ShapeQuadFixed" ), MENGINE_DOCUMENT_FUNCTION );
        quad->setSurface( surface );
        m_scene->addChild( quad );
        *_surface = surface;
        return quad;
    }
    //////////////////////////////////////////////////////////////////////////
    void Arena3DHud::createDigit_( DigitVisual * _digit, const Color & _color )
    {
        for( size_t segment = 0; segment != DigitSegmentCount; ++segment )
        {
            const bool horizontal = segment == 0 || segment == 3 || segment == 6;
            SurfaceSolidColorPtr surface;
            _digit->segments[segment] = this->createQuad_( horizontal == true ? mt::vec2f( 8.f, 2.f ) : mt::vec2f( 2.f, 8.f ), _color, &surface );
            _digit->segments[segment]->getRender()->setLocalAlpha( 0.f );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Arena3DHud::setDigit_( DigitVisual * _digit, int32_t _value, const mt::vec2f & _position, float _scale, float _alpha )
    {
        static const mt::vec2f offsets[] = {
            {2.f, 0.f}, {10.f, 2.f}, {10.f, 10.f}, {2.f, 18.f},
            {0.f, 10.f}, {0.f, 2.f}, {2.f, 9.f}};
        const uint8_t mask = _value >= 0 ? Detail::digitMask( static_cast<uint32_t>(_value) ) : 0u;
        for( size_t segment = 0; segment != DigitSegmentCount; ++segment )
        {
            NodePtr & node = _digit->segments[segment];
            node->getTransformation()->setLocalPosition( {_position.x + offsets[segment].x * _scale, _position.y + offsets[segment].y * _scale, 0.f} );
            node->getTransformation()->setLocalScale( {_scale, _scale, 1.f} );
            node->getRender()->setLocalAlpha( (mask & (1u << segment)) != 0u ? _alpha : 0.f );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Arena3DHud::setNumber_( Array<DigitVisual, DigitCount> * _digits, uint32_t _value, bool _leadingZeros, const mt::vec2f & _position, float _scale, float _alpha )
    {
        _value = Mengine::StdAlgorithm::min<uint32_t>( _value, 999u );
        const uint32_t divisors[] = {100u, 10u, 1u};
        for( size_t index = 0; index != DigitCount; ++index )
        {
            const uint32_t value = (_value / divisors[index]) % 10u;
            const bool hidden = _leadingZeros == false && ((index == 0 && _value < 100u) || (index == 1 && _value < 10u));
            this->setDigit_( &(*_digits)[index], hidden == true ? -1 : static_cast<int32_t>(value),
                {_position.x + static_cast<float>(index) * 14.f * _scale, _position.y}, _scale, _alpha );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void Arena3DHud::updateDamageNumbers_( float _seconds, const RenderCameraProjectionPtr & _worldCamera, const mt::vec3f & _cameraPosition, const mt::vec3f & _cameraDirection )
    {
        m_visibleDamageNumbers = 0;
        const Resolution & resolution = APPLICATION_SERVICE()->getContentResolution();
        for( DamageNumberVisual & damageNumber : m_damageNumbers )
        {
            if( damageNumber.active == false )
            {
                continue;
            }
            damageNumber.age += _seconds;
            if( damageNumber.age >= damageNumber.duration )
            {
                damageNumber.active = false;
                this->setNumber_( &damageNumber.digits, damageNumber.amount, false, {0.f, 0.f}, 1.f, 0.f );
                continue;
            }

            mt::vec3f worldPosition = Helper::kinefixVec3ToVec3f( damageNumber.position );
            worldPosition.y += damageNumber.age * 0.9f;
            const mt::vec3f cameraDelta = worldPosition - _cameraPosition;
            if( Detail::dot3( cameraDelta, _cameraDirection ) <= 0.05f )
            {
                this->setNumber_( &damageNumber.digits, damageNumber.amount, false, {0.f, 0.f}, 1.f, 0.f );
                continue;
            }

            mt::mat4f worldMatrix;
            mt::make_translation_m4_v3( &worldMatrix, worldPosition );
            mt::vec2f screenPosition;
            const RenderCameraInterface * worldCamera = _worldCamera.get();
            worldCamera->fromWorldToScreenPosition( worldMatrix, &screenPosition );
            if( screenPosition.x < -0.1f || screenPosition.x > 1.1f || screenPosition.y < -0.1f || screenPosition.y > 1.1f )
            {
                this->setNumber_( &damageNumber.digits, damageNumber.amount, false, {0.f, 0.f}, 1.f, 0.f );
                continue;
            }

            const float linear = damageNumber.age / damageNumber.duration;
            const float alpha = Mengine::StdAlgorithm::min( 1.f, (1.f - linear) * 3.5f );
            const float scale = 0.9f + linear * 0.18f;
            const mt::vec2f position( screenPosition.x * resolution.getWidthF() - 18.f * scale, screenPosition.y * resolution.getHeightF() - 10.f * scale );
            this->setNumber_( &damageNumber.digits, damageNumber.amount, false, position, scale, alpha );
            ++m_visibleDamageNumbers;
        }
    }
    //////////////////////////////////////////////////////////////////////////
}
