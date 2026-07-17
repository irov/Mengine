#pragma once

#include "Arena3DSimulation.h"

#include "Kernel/Array.h"
#include "Kernel/Factorable.h"
#include "Kernel/Node.h"
#include "Kernel/RenderCameraProjection.h"
#include "Kernel/Scene.h"
#include "Kernel/SurfaceSolidColor.h"

#include "Config/StdDef.h"
#include "Config/StdInt.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class Arena3DHud
        : public Factorable
    {
        DECLARE_FACTORABLE( Arena3DHud );

    private:
        static constexpr size_t WeaponSlotCount = Arena3D::WeaponTypeCount;
        static constexpr size_t WeaponIconPartCount = 3;
        static constexpr size_t DigitCount = 3;
        static constexpr size_t DigitSegmentCount = 7;
        static constexpr size_t DamageNumberCount = 12;
        typedef Array<NodePtr, DigitSegmentCount> ArrayDigitSegment;
        typedef Array<NodePtr, WeaponIconPartCount> ArrayWeaponIconPart;
        //////////////////////////////////////////////////////////////////////////
        struct DigitVisual
        {
            ArrayDigitSegment segments;
        };
        //////////////////////////////////////////////////////////////////////////
        typedef Array<DigitVisual, DigitCount> ArrayDigitVisual;
        //////////////////////////////////////////////////////////////////////////
        struct WeaponVisual
        {
            NodePtr background;
            SurfaceSolidColorPtr backgroundSurface;
            ArrayWeaponIconPart iconParts;
            ArrayDigitVisual digits;
        };
        //////////////////////////////////////////////////////////////////////////
        struct DamageNumberVisual
        {
            ArrayDigitVisual digits;
            kf_vec3_t position;
            uint32_t targetId = 0;
            uint32_t amount = 0;
            float age = 0.f;
            float duration = 0.f;
            bool active = false;
        };
        //////////////////////////////////////////////////////////////////////////
        typedef Array<WeaponVisual, WeaponSlotCount> ArrayWeaponVisual;
        typedef Array<DamageNumberVisual, DamageNumberCount> ArrayDamageNumberVisual;
        //////////////////////////////////////////////////////////////////////////

    public:
        Arena3DHud();
        ~Arena3DHud() override;

        void initialize( Scene * _scene, float _width, float _height );
        void finalize();

        void update( float _seconds, const RenderCameraProjectionPtr & _worldCamera, const mt::vec3f & _cameraPosition, const mt::vec3f & _cameraDirection );
        void sync( const Arena3D::PlayerState & _player, float _warningFlash );
        void showDamageDirection( float _relativeRadians );
        void spawnDamageNumber( const Arena3D::ServerEvent & _event, const Arena3D::PlayerState & _target );
        void clearDamageNumbers();

        uint32_t getDrawCallCount() const;

    private:
        NodePtr createQuad_( const mt::vec2f & _size, const Color & _color, SurfaceSolidColorPtr * _surface );
        void createDigit_( DigitVisual * _digit, const Color & _color );
        void setDigit_( DigitVisual * _digit, int32_t _value, const mt::vec2f & _position, float _scale, float _alpha );
        void setNumber_( ArrayDigitVisual * _digits, uint32_t _value, bool _leadingZeros, const mt::vec2f & _position, float _scale, float _alpha );
        void updateDamageNumbers_( float _seconds, const RenderCameraProjectionPtr & _worldCamera, const mt::vec3f & _cameraPosition, const mt::vec3f & _cameraDirection );

    private:
        Scene * m_scene;
        ArrayWeaponVisual m_weapons;
        ArrayDamageNumberVisual m_damageNumbers;

        NodePtr m_crosshairHorizontal;
        NodePtr m_crosshairVertical;
        NodePtr m_healthBack;
        NodePtr m_healthFill;
        NodePtr m_armorBack;
        NodePtr m_armorFill;
        NodePtr m_speedBack;
        NodePtr m_speedFill;
        NodePtr m_damageIndicator;
        SurfaceSolidColorPtr m_crosshairHorizontalSurface;
        SurfaceSolidColorPtr m_crosshairVerticalSurface;
        SurfaceSolidColorPtr m_healthFillSurface;
        SurfaceSolidColorPtr m_armorFillSurface;
        SurfaceSolidColorPtr m_speedFillSurface;
        SurfaceSolidColorPtr m_damageIndicatorSurface;

        float m_damageIndicatorTime;
        uint32_t m_visibleDamageNumbers;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Arena3DHud> Arena3DHudPtr;
    //////////////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////////////
