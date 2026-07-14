#pragma once

#include "Arena3DSession.h"

#include "Kernel/DummySceneEventReceiver.h"
#include "Kernel/Factorable.h"
#include "Kernel/KeyCode.h"
#include "Kernel/Node.h"
#include "Kernel/RenderCameraProjection.h"
#include "Kernel/Resource.h"
#include "Kernel/ResourceImage.h"
#include "Kernel/SurfaceSolidColor.h"
#include "Kernel/SurfaceSound.h"
#include "Kernel/Vector.h"

#include "Engine/ResourceMesh3D.h"

#include "Interface/TimepipeInterface.h"

#include "Config/UniqueId.h"

#include <array>
#include <cstdint>

namespace Mengine
{
    class Scene;
    struct InputMouseMoveEvent;
    struct UpdateContext;

    class Arena3DSceneEventReceiver
        : public DummySceneEventReceiver
        , public Factorable
    {
    public:
        Arena3DSceneEventReceiver();

        bool onEntityCreate( const EntityBehaviorInterfacePtr & _behavior, Entity * _entity ) override;
        void onEntityDestroy( const EntityBehaviorInterfacePtr & _behavior ) override;
        bool onEntityPreparation( const EntityBehaviorInterfacePtr & _behavior ) override;
        bool onEntityActivate( const EntityBehaviorInterfacePtr & _behavior ) override;
        void onEntityDeactivate( const EntityBehaviorInterfacePtr & _behavior ) override;

    private:
        ResourceMesh3DPtr importMesh_( const Char * _path, const ConstString & _name );
        ResourceImagePtr importTexture_( const Char * _path, const ConstString & _name );
        SurfaceSoundPtr importSound_( const Char * _path, const ConstString & _name, NodePtr * _emitter );
        NodePtr createMeshNode_( const ConstString & _name, const ResourceMesh3DPtr & _mesh,
            const ResourceImagePtr & _image, const ConstString & _material, const RenderCameraProjectionPtr & _camera, Node * _parent );
        NodePtr createHudQuad_( const mt::vec2f & _size, const Color & _color, SurfaceSolidColorPtr * _surface );

        void registerInput_();
        void registerButton_( EKeyCode _key, Arena3D::InputButton _button );
        void unregisterInput_();
        void submit_( Arena3D::CommandType _type, const Arena3D::CommandPayload & _payload = {} );
        void selectWeapon_( Arena3D::WeaponType _weapon );
        void onMouseMove_( const InputMouseMoveEvent & _event );

        void update_( const UpdateContext * _context );
        void syncPresentation_( float _alpha );
        void syncCamera_( const Arena3D::PlayerState & _player, float _alpha );
        void syncArenaChunks_();
        void syncTurrets_();
        void syncProjectiles_( float _alpha );
        void syncPickups_();
        void syncViewModels_();
        void syncHud_();
        void processEvents_();
        void playSound_( size_t _index );
        void parkUnused_( Vector<NodePtr> & _pool, uint32_t _activeCount );
        void logDiagnostics_() const;

    private:
        Scene * m_scene;
        NodePtr m_postProcess;
        NodePtr m_directionalLight;
        RenderCameraProjectionPtr m_worldCamera;
        RenderCameraProjectionPtr m_viewCamera;

        ResourceImagePtr m_tileTexture;
        ResourceImagePtr m_emissiveTexture;
        Vector<ResourcePtr> m_resources;
        std::array<ResourceMesh3DPtr, 4> m_arenaChunkResources;
        ResourceMesh3DPtr m_turretBaseResource;
        std::array<ResourceMesh3DPtr, static_cast<size_t>(Arena3D::WeaponType::Count)> m_turretResources;
        std::array<ResourceMesh3DPtr, static_cast<size_t>(Arena3D::WeaponType::Count)> m_projectileResources;
        std::array<ResourceMesh3DPtr, static_cast<size_t>(Arena3D::WeaponType::Count)> m_weaponResources;
        std::array<ResourceMesh3DPtr, 3> m_pickupResources;

        std::array<NodePtr, 4> m_arenaChunkNodes;
        std::array<NodePtr, Arena3D::MaximumTurrets> m_turretBaseNodes;
        std::array<NodePtr, Arena3D::MaximumTurrets> m_turretBarrelNodes;
        std::array<Vector<NodePtr>, static_cast<size_t>(Arena3D::WeaponType::Count)> m_projectilePools;
        std::array<NodePtr, static_cast<size_t>(Arena3D::WeaponType::Count)> m_viewModels;
        std::array<NodePtr, Arena3D::MaximumPickups> m_pickupNodes;
        std::array<NodePtr, 13> m_soundEmitters;
        std::array<SurfaceSoundPtr, 13> m_sounds;

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

        Arena3D::LocalSession m_session;
        Arena3D::SimulationState m_previousState;
        Arena3D::SimulationState m_currentState;
        TimepipeInterfacePtr m_timepipe;
        Vector<UniqueId> m_inputHandlers;

        double m_accumulatorMs;
        uint64_t m_sourceTimeMs;
        uint64_t m_lastPresentedEventId;
        uint64_t m_presentedMatchId;
        float m_hitFlash;
        float m_damageIndicatorTime;
        float m_warningFlash;
        uint32_t m_drawCalls;
        uint32_t m_visibleChunks;
        uint32_t m_visibleProjectiles;
        uint8_t m_lightQuantization;
        bool m_debugHud;
        bool m_collisionDebug;
        bool m_turretDebug;
        bool m_noclip;
        bool m_cursorCaptured;
    };

    using Arena3DSceneEventReceiverPtr = IntrusivePtr<Arena3DSceneEventReceiver>;
}
