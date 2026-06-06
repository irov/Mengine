#pragma once

#include "Crimsonland3DDungeonGenerator.h"
#include "Crimsonland3DCombat.h"

#include "Kernel/Factorable.h"
#include "Kernel/DummySceneEventReceiver.h"
#include "Kernel/Scene.h"
#include "Kernel/Resource.h"
#include "Kernel/ResourceImage.h"
#include "Kernel/ResourceImageEmpty.h"
#include "Kernel/RenderCameraProjection.h"
#include "Kernel/Vector.h"
#include "Kernel/SurfaceSolidColor.h"

#include "Engine/ResourceMesh3D.h"

#include "Interface/TimepipeInterface.h"
#include "Interface/FontInterface.h"

#include "Config/UniqueId.h"

#include "math/vec4.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class PostProcessFx3D;
    class DirectionalLight3D;
    class PointLight3D;
    class BlobShadow3D;
    struct InputMouseMoveEvent;
    struct UpdateContext;
    //////////////////////////////////////////////////////////////////////////
    class Crimsonland3DSceneEventReceiver
        : public DummySceneEventReceiver
        , public Factorable
    {
    public:
        Crimsonland3DSceneEventReceiver();
        ~Crimsonland3DSceneEventReceiver() override;

    public:
        bool onEntityCreate( const EntityBehaviorInterfacePtr & _behavior, Entity * _entity ) override;
        void onEntityDestroy( const EntityBehaviorInterfacePtr & _behavior ) override;
        bool onEntityPreparation( const EntityBehaviorInterfacePtr & _behavior ) override;
        bool onEntityActivate( const EntityBehaviorInterfacePtr & _behavior ) override;
        void onEntityDeactivate( const EntityBehaviorInterfacePtr & _behavior ) override;

    protected:
        void updateGameplay_( const UpdateContext * _context );
        void moveHero_( float _dt );
        void frameFollowCamera_( float _dt );
        bool isWalkable_( float _x, float _z ) const;
        void handleOrbitMouseMove_( const InputMouseMoveEvent & _event );
        void addDemoInputHandler_( UniqueId _id );
        void removeDemoInputHandlers_();

        // Combat rendering: pooled colored unit-cubes for enemies/projectiles/xp.
        ResourceMesh3DPtr getCubeResource_( uint32_t _color );
        NodePtr acquireCubeNode_( uint32_t _color );
        ResourceMesh3DPtr getDiamondResource_( uint32_t _color );
        NodePtr acquireDiamondNode_( uint32_t _color );

        // Optional imported GLTF enemy models (replace cube monsters when present).
        // One model per enemy type, looked up by name: 'models/enemy_<name>.gltf'.
        bool ensureEnemyModel_( uint32_t _defIndex, const CombatEnemyDef & _def );
        bool ensureEnemyAtlas_();
        NodePtr acquireEnemyNode_( uint32_t _defIndex );

        void syncCombatRender_();
        void updateWeaponHud_();
        void updateStatsHud_();
        NodePtr acquireHudQuad_( SurfaceSolidColorPtr * const _outSurface );
        void hidePoolFrom_( Vector<NodePtr> & _pool, uint32_t _activeCount );
        void teardownCombatRender_();

        // Level-up overlay + upgrade selection.
        bool setupFont_();
        void showLevelUp_();
        void hideLevelUp_();
        void selectUpgrade_( uint32_t _index );
        void handleLevelUpClick_( const mt::vec2f & _screen );
        void showHeroDeath_();

    protected:
        Scene * m_scene;

        NodePtr m_postFx;
        NodePtr m_background;
        RenderCameraProjectionPtr m_demoCamera;
        Vector<NodePtr> m_demoMeshes;
        NodePtr m_hero;
        NodePtr m_directionalLight;
        NodePtr m_pointLight;
        NodePtr m_blobShadow;

        Vector<ResourcePtr> m_demoResources;
        ResourceImagePtr m_meshTextureResource;
        ResourceImageEmptyPtr m_blobShadowResource;

        Crimsonland3DDungeonBuildResult m_dungeon;
        mt::vec3f m_dungeonSpawn;

        mt::vec3f m_heroPosition;
        float m_heroRadius;
        float m_heroMoveSpeed;

        float m_cameraYaw;
        float m_cameraPitch;
        float m_cameraDistance;
        float m_cameraDistanceMin;
        float m_cameraDistanceMax;
        float m_cameraMouseSensitivity;
        float m_cameraFocusHeight;

        TimepipeInterfacePtr m_demoCameraTimepipe;
        Vector<UniqueId> m_demoInputHandlers;

        // Auto-shooter combat simulation (runs on the XZ plane).
        Crimsonland3DCombat m_combat;
        bool m_combatStarted;
        mt::vec2f m_heroMoveDir;

        // Colored unit-cube cache shared by all combat instances.
        Vector<uint32_t> m_cubeColors;
        Vector<ResourceMesh3DPtr> m_cubeResources;

        // Colored diamond (octahedron) cache used for projectiles.
        Vector<uint32_t> m_diamondColors;
        Vector<ResourceMesh3DPtr> m_diamondResources;

        // Optional imported GLTF enemy models, one per enemy type (by defIndex).
        // State: 0 = not probed, 1 = loaded, 2 = missing (fall back to cube).
        Vector<uint8_t> m_enemyModelState;
        Vector<ResourceMesh3DPtr> m_enemyModelResources;
        // Red-tinted variant of each model, shown while the corpse dies/sinks.
        Vector<ResourceMesh3DPtr> m_enemyModelResourcesRed;

        // Shared atlas texture for the imported monster models (models/Atlas_Monsters.png).
        // State: 0 = not probed, 1 = loaded, 2 = missing.
        uint8_t m_enemyAtlasState;
        ResourceImagePtr m_enemyAtlasResource;

        // Per-category node pools. Enemy slots are keyed by a composite identity
        // (dying flag, model/cube kind, defIndex) so a reused slot fully rebinds
        // its mesh and texture instead of relying on the color alone.
        Vector<NodePtr> m_enemyPool;
        Vector<uint32_t> m_enemyPoolKey;
        Vector<NodePtr> m_projectilePool;
        Vector<uint32_t> m_projectilePoolColor;
        Vector<NodePtr> m_xpPool;
        Vector<uint32_t> m_xpPoolColor;

        float m_xpSpin;

        // Level-up UI.
        bool m_levelUpActive;
        bool m_heroDeathShown;
        float m_savedExposure;
        NodePtr m_overlay;
        Vector<NodePtr> m_levelUpCards;
        // Screen-space rects (x, y, w, h) of the visible cards for mouse picking.
        Vector<mt::vec4f> m_levelUpCardRects;

        // TTF font used for the upgrade card labels.
        FontInterfacePtr m_dungeonFont;

        // Weapon HUD (bottom-left): per-slot icon, reload text, ammo pips.
        Vector<NodePtr> m_hudIconNodes;
        Vector<SurfaceSolidColorPtr> m_hudIconSurfaces;
        Vector<NodePtr> m_hudTextNodes;
        Vector<float> m_hudTextCachedRate;
        Vector<NodePtr> m_hudPipNodes;
        Vector<SurfaceSolidColorPtr> m_hudPipSurfaces;

        // Stats HUD (top-left): HP bar, XP bar, level text.
        NodePtr m_hpBarBack;
        SurfaceSolidColorPtr m_hpBarBackSurface;
        NodePtr m_hpBarFill;
        SurfaceSolidColorPtr m_hpBarFillSurface;
        NodePtr m_xpBarBack;
        SurfaceSolidColorPtr m_xpBarBackSurface;
        NodePtr m_xpBarFill;
        SurfaceSolidColorPtr m_xpBarFillSurface;
        NodePtr m_levelTextNode;
        NodePtr m_hpTextNode;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<Crimsonland3DSceneEventReceiver> Crimsonland3DSceneEventReceiverPtr;
    //////////////////////////////////////////////////////////////////////////
}
