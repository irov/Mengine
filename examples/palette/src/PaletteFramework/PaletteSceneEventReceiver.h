#pragma once

#include "Kernel/DummySceneEventReceiver.h"

#include "Kernel/Scene.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class PaletteSceneEventReceiver
        : public DummySceneEventReceiver
        , public Factorable
    {
    public:
        PaletteSceneEventReceiver();
        ~PaletteSceneEventReceiver() override;

    public:
        bool onSceneAppMouseLeave( const EntityBehaviorInterfacePtr & _behavior ) override;
        bool onSceneAppMouseEnter( const EntityBehaviorInterfacePtr & _behavior ) override;
        bool onSceneAppFocus( const EntityBehaviorInterfacePtr & _behavior, bool _focus ) override;

    public:
        bool onEntityCreate( const EntityBehaviorInterfacePtr & _behavior, Entity * _entity ) override;
        void onEntityDestroy( const EntityBehaviorInterfacePtr & _behavior ) override;
        bool onEntityPreparation( const EntityBehaviorInterfacePtr & _behavior ) override;
        bool onEntityActivate( const EntityBehaviorInterfacePtr & _behavior ) override;
        void onEntityPreparationDeactivate( const EntityBehaviorInterfacePtr & _behavior ) override;
        void onEntityDeactivate( const EntityBehaviorInterfacePtr & _behavior ) override;
        void onEntityCompile( const EntityBehaviorInterfacePtr & _behavior ) override;
        void onEntityRelease( const EntityBehaviorInterfacePtr & _behavior ) override;

    protected:
        Scene * m_scene;

        uint32_t m_schedulerId;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<PaletteSceneEventReceiver> PaletteSceneEventReceiverPtr;
    //////////////////////////////////////////////////////////////////////////
}