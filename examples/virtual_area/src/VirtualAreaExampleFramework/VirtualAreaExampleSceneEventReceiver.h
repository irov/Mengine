#pragma once

#include "Engine/VirtualArea.h"

#include "Frameworks/MosaicFramework/MosaicRender.h"

#include "Kernel/String.h"
#include "Kernel/Deque.h"
#include "Kernel/Vector.h"

#include "Kernel/DummySceneEventReceiver.h"
#include "Kernel/Scene.h"
#include "Kernel/Vector.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class VirtualAreaExampleSceneEventReceiver
        : public DummySceneEventReceiver
        , public Factorable
    {
    public:
        VirtualAreaExampleSceneEventReceiver();
        ~VirtualAreaExampleSceneEventReceiver() override;

    public:
        bool onEntityCreate( const EntityBehaviorInterfacePtr & _behavior, Entity * _entity ) override;
        void onEntityDestroy( const EntityBehaviorInterfacePtr & _behavior ) override;
        bool onEntityPreparation( const EntityBehaviorInterfacePtr & _behavior ) override;
        void onEntityDeactivate( const EntityBehaviorInterfacePtr & _behavior ) override;

    protected:
        NodePtr makeSolidQuad_( const mt::vec2f & _position, const mt::vec2f & _size, float _r, float _g, float _b, float _a );
        NodePtr addContentQuad_( const mt::vec2f & _position, const mt::vec2f & _size, float _r, float _g, float _b, float _a );
        void setupVirtualArea_();
        void rebuildFrame_();
        void rebuildContent_();
        void clearNodes_( Vector<NodePtr> * const _nodes );
        void renderControls_( Mosaic::Context * _ui );

    protected:
        Scene * m_scene;

        VirtualAreaPtr m_virtualArea;
        MosaicRenderPtr m_mosaicRender;

        typedef Deque<String> DequeReadout;
        DequeReadout m_readout;

        Vector<NodePtr> m_contentNodes;
        Vector<NodePtr> m_frameNodes;

        float m_viewportInput[4];
        float m_contentInput[4];
        float m_anchorInput[2];
        float m_positionInput[2];
        float m_percentageInput[2];
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<VirtualAreaExampleSceneEventReceiver> VirtualAreaExampleSceneEventReceiverPtr;
    //////////////////////////////////////////////////////////////////////////
}
