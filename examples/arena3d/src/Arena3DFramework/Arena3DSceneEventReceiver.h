#pragma once

#include "Arena3DHud.h"
#include "Arena3DNetworkClient.h"
#include "Arena3DSession.h"

#include "Kernel/Array.h"
#include "Kernel/DummySceneEventReceiver.h"
#include "Kernel/Factorable.h"
#include "Kernel/InputEvent.h"
#include "Kernel/KeyCode.h"
#include "Kernel/Node.h"
#include "Kernel/RenderCameraProjection.h"
#include "Kernel/Resource.h"
#include "Kernel/ResourceImage.h"
#include "Kernel/Scene.h"
#include "Kernel/SurfaceSound.h"
#include "Kernel/UpdateContext.h"
#include "Kernel/Vector.h"
#include "Kernel/VectorResources.h"

#include "Engine/ResourceMesh3D.h"

#include "Interface/TimepipeInterface.h"

#include "Config/UniqueId.h"
#include "Config/StdInt.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class Arena3DSceneEventReceiver
        : public DummySceneEventReceiver
        , public Factorable
    {
    private:
        static constexpr size_t ExplosionTypeCount = 3;
        static constexpr size_t ExplosionCloudsPerType = 8;
        static constexpr size_t ArenaVisualLayerCount = 3;
        static constexpr size_t ArenaChunkCount = 4;
        static constexpr size_t ImpactFlashCount = 64;
        static constexpr size_t DecalCount = 256;
        static constexpr size_t GrenadeImpactCount = 32;
        static constexpr size_t DeathBurstCount = 8;
        static constexpr size_t DeathShardCount = 6;
        typedef Array<NodePtr, DeathShardCount> ArrayDeathShardNode;
        typedef Array<mt::vec3f, DeathShardCount> ArrayDeathShardVector;
        typedef Array<NodePtr, 4> ArrayPointLightNode;
        typedef Array<ResourceMesh3DPtr, ArenaChunkCount> ArrayArenaChunkResource;
        typedef Array<ArrayArenaChunkResource, ArenaVisualLayerCount> ArrayArenaVisualLayerResource;
        typedef Array<ResourceMesh3DPtr, Arena3D::WeaponTypeCount> ArrayWeaponResource;
        typedef Array<ResourceMesh3DPtr, 3> ArrayPickupResource;
        typedef Array<ResourceMesh3DPtr, ExplosionTypeCount> ArrayExplosionResource;
        typedef Array<NodePtr, ArenaChunkCount> ArrayArenaChunkNode;
        typedef Array<ArrayArenaChunkNode, ArenaVisualLayerCount> ArrayArenaVisualLayerNode;
        typedef Array<NodePtr, Arena3D::MaximumPlayers> ArrayPlayerNode;
        typedef Array<NodePtr, Arena3D::MaximumTurrets> ArrayTurretNode;
        typedef Vector<NodePtr> VectorNodePtr;
        typedef Array<VectorNodePtr, Arena3D::WeaponTypeCount> ArrayProjectilePool;
        typedef Array<NodePtr, Arena3D::WeaponTypeCount> ArrayViewModelNode;
        typedef Array<NodePtr, Arena3D::MaximumPickups> ArrayPickupNode;
        typedef Array<NodePtr, 13> ArraySoundEmitterNode;
        typedef Array<SurfaceSoundPtr, 13> ArraySurfaceSound;
        typedef Vector<UniqueId> VectorInputHandler;
        //////////////////////////////////////////////////////////////////////////
        struct ExplosionCloudVisual
        {
            NodePtr node;
            float age = 0.f;
            float duration = 0.f;
            float startScale = 0.f;
            float endScale = 0.f;
            bool active = false;
        };
        //////////////////////////////////////////////////////////////////////////
        struct ImpactFlashVisual
        {
            NodePtr node;
            float age = 0.f;
            float duration = 0.f;
            float startScale = 0.f;
            float endScale = 0.f;
            bool active = false;
        };
        //////////////////////////////////////////////////////////////////////////
        struct DecalVisual
        {
            NodePtr node;
            ResourceMesh3DPtr resource;
            float age = 0.f;
            float duration = 0.f;
            bool active = false;
        };
        //////////////////////////////////////////////////////////////////////////
        struct GrenadeImpactVisual
        {
            Arena3D::ServerEvent event;
            bool active = false;
        };
        //////////////////////////////////////////////////////////////////////////
        struct DeathBurstVisual
        {
            ArrayDeathShardNode nodes;
            ArrayDeathShardVector velocities;
            ArrayDeathShardVector angularVelocities;
            float age = 0.f;
            float duration = 0.f;
            bool active = false;
        };
        //////////////////////////////////////////////////////////////////////////
        typedef Array<ExplosionCloudVisual, ExplosionCloudsPerType> ArrayExplosionCloud;
        typedef Array<ArrayExplosionCloud, ExplosionTypeCount> ArrayExplosionCloudType;
        typedef Array<ImpactFlashVisual, ImpactFlashCount> ArrayImpactFlash;
        typedef Array<DecalVisual, DecalCount> ArrayDecal;
        typedef Array<GrenadeImpactVisual, GrenadeImpactCount> ArrayGrenadeImpact;
        typedef Array<DeathBurstVisual, DeathBurstCount> ArrayDeathBurst;
        //////////////////////////////////////////////////////////////////////////
        enum class ViewModelTransition : uint8_t
        {
            Ready,
            Lowering,
            Raising
        };
        //////////////////////////////////////////////////////////////////////////
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
        void registerInput_();
        void registerButton_( EKeyCode _key, Arena3D::InputButton _button );
        void unregisterInput_();
        void setCursorCaptured_( bool _captured );
        void submit_( Arena3D::CommandType _type, const Arena3D::CommandPayload & _payload = {} );
        void selectWeapon_( Arena3D::WeaponType _weapon );
        void cycleWeapon_( int32_t _direction );
        void onMouseMove_( const InputMouseMoveEvent & _event );
        void initializeNetwork_();

        const Arena3D::Simulation & activeSimulation_() const;
        const Arena3D::VectorServerEvent & activeEvents_() const;
        const Arena3D::PlayerState & localPlayer_( const Arena3D::SimulationState & _state ) const;

        void update_( const UpdateContext * _context );
        void advancePresentationAnimations_( float _seconds );
        void observeWeaponSelection_();
        void syncPresentation_( float _alpha );
        void syncCamera_( const Arena3D::PlayerState & _player, float _alpha );
        void syncPlayers_( float _alpha );
        void syncArenaChunks_();
        void syncTurrets_();
        void syncProjectiles_( float _alpha );
        void syncPickups_();
        void syncViewModels_();
        void syncHud_();
        void processEvents_();
        void spawnExplosionCloud_( const Arena3D::ServerEvent & _event );
        void spawnImpactFlash_( const Arena3D::ServerEvent & _event );
        void spawnDecal_( const Arena3D::ServerEvent & _event );
        void rememberGrenadeImpact_( const Arena3D::ServerEvent & _event );
        void spawnGrenadeExplosionDecal_( const Arena3D::ServerEvent & _event );
        void spawnDeathBurst_( const Arena3D::ServerEvent & _event );
        void updateExplosionClouds_( float _seconds );
        void updateImpactEffects_( float _seconds );
        void updateDeathBursts_( float _seconds );
        void clearImpactEffects_();
        void clearDeathBursts_();
        void playSound_( size_t _index );
        void parkUnused_( VectorNodePtr & _pool, uint32_t _activeCount );
        void logDiagnostics_() const;

    private:
        Scene * m_scene;
        NodePtr m_postProcess;
        NodePtr m_directionalLight;
        ArrayPointLightNode m_pointLights;
        NodePtr m_skyNode;
        NodePtr m_lavaNode;
        RenderCameraProjectionPtr m_worldCamera;
        RenderCameraProjectionPtr m_viewModelCamera;

        ResourceImagePtr m_tileTexture;
        ResourceImagePtr m_stoneTexture;
        ResourceImagePtr m_metalTexture;
        ResourceImagePtr m_hazardTexture;
        ResourceImagePtr m_emissiveTexture;
        ResourceImagePtr m_decalTexture;
        ResourceImagePtr m_whiteTexture;
        VectorResources m_resources;
        ResourceMesh3DPtr m_skyResource;
        ResourceMesh3DPtr m_lavaResource;
        ResourceMesh3DPtr m_playerResource;
        ArrayArenaVisualLayerResource m_arenaChunkResources;
        ResourceMesh3DPtr m_turretBaseResource;
        ArrayWeaponResource m_turretResources;
        ArrayWeaponResource m_projectileResources;
        ArrayWeaponResource m_weaponResources;
        ArrayPickupResource m_pickupResources;
        ArrayExplosionResource m_explosionResources;
        ResourceMesh3DPtr m_impactResource;

        ArrayArenaVisualLayerNode m_arenaChunkNodes;
        ArrayPlayerNode m_playerNodes;
        ArrayTurretNode m_turretBaseNodes;
        ArrayTurretNode m_turretBarrelNodes;
        ArrayProjectilePool m_projectilePools;
        ArrayViewModelNode m_viewModels;
        ArrayPickupNode m_pickupNodes;
        ArrayExplosionCloudType m_explosionClouds;
        ArrayImpactFlash m_impactFlashes;
        ArrayDecal m_decals;
        ArrayGrenadeImpact m_grenadeImpacts;
        ArrayDeathBurst m_deathBursts;
        ArraySoundEmitterNode m_soundEmitters;
        ArraySurfaceSound m_sounds;

        Arena3DHudPtr m_hud;

        Arena3D::LocalSession m_session;
        Arena3D::NetworkClientPtr m_networkClient;
        Arena3D::SimulationState m_previousState;
        Arena3D::SimulationState m_currentState;
        mt::vec3f m_presentedCameraPosition{0.f, 0.f, 0.f};
        mt::vec3f m_presentedCameraDirection{0.f, 0.f, 1.f};
        TimepipeInterfacePtr m_timepipe;
        VectorInputHandler m_inputHandlers;

        double m_accumulatorMs;
        uint64_t m_sourceTimeMs;
        uint64_t m_lastPresentedEventId;
        uint64_t m_presentedMatchId;
        float m_hitFlash;
        float m_warningFlash;
        float m_stepCameraOffset;
        float m_stepCameraTime;
        ViewModelTransition m_viewModelTransition;
        Arena3D::WeaponType m_displayedWeapon;
        Arena3D::WeaponType m_pendingWeapon;
        float m_viewModelTransitionTime;
        uint32_t m_drawCalls;
        uint32_t m_visibleChunks;
        uint32_t m_visiblePlayers;
        uint32_t m_visibleProjectiles;
        uint32_t m_visibleExplosionClouds;
        uint32_t m_visibleImpactFlashes;
        uint32_t m_visibleDecals;
        uint32_t m_visibleDeathShards;
        uint8_t m_lightQuantization;
        uint32_t m_localPlayerId;
        bool m_networkMode;
        bool m_debugHud;
        bool m_collisionDebug;
        bool m_turretDebug;
        bool m_noclip;
        bool m_cursorCaptured;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Arena3DSceneEventReceiver> Arena3DSceneEventReceiverPtr;
    //////////////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////////////
