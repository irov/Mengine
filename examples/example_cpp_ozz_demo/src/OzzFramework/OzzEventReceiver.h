#pragma once

#include "Kernel/Factory.h"
#include "Kernel/DummySceneEventReceiver.h"

#include "Plugins/OzzAnimationPlugin/OzzAnimationInterface.h"

#include "Interface/TimepipeServiceInterface.h"

#include "Kernel/Scene.h"

#include "GOAP/GOAP.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class OzzEventReceiver
        : public DummySceneEventReceiver
        , public Factorable
    {
    public:
        OzzEventReceiver();
        ~OzzEventReceiver() override;

    public:
        bool onEntityCreate( const EntityBehaviorInterfacePtr & _behavior, Entity * _entity ) override;
        void onEntityDestroy( const EntityBehaviorInterfacePtr & _behavior ) override;
        bool onEntityPreparation( const EntityBehaviorInterfacePtr & _behavior ) override;
        bool onEntityActivate( const EntityBehaviorInterfacePtr & _behavior ) override;
        void onEntityDeactivate( const EntityBehaviorInterfacePtr & _behavior ) override;

    protected:
        SamplerOzzAnimationInterfacePtr createOzzSampler( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, const ResourcePtr & _resourceOzzSkeleton, float _weight );

    protected:
        Scene * m_scene;

        NodePtr m_node;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<OzzEventReceiver> OzzEventReceiverPtr;
    //////////////////////////////////////////////////////////////////////////
}