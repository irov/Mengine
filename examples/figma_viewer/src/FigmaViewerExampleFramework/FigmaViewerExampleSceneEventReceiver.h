#pragma once

#include "Frameworks/ImGUIFramework/ImGUIRender.h"

#include "Plugins/FigmaPlugin/FigmaInterface.h"
#include "Plugins/FigmaPlugin/ResourceFigma.h"

#include "Interface/FileGroupInterface.h"

#include "Kernel/DummySceneEventReceiver.h"
#include "Kernel/FilePath.h"
#include "Kernel/Scene.h"
#include "Kernel/String.h"

#include "Config/Path.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class FigmaViewerExampleSceneEventReceiver
        : public DummySceneEventReceiver
        , public Factorable
    {
    public:
        FigmaViewerExampleSceneEventReceiver();
        ~FigmaViewerExampleSceneEventReceiver() override;

    public:
        bool onEntityCreate( const EntityBehaviorInterfacePtr & _behavior, Entity * _entity ) override;
        void onEntityDestroy( const EntityBehaviorInterfacePtr & _behavior ) override;
        bool onEntityPreparation( const EntityBehaviorInterfacePtr & _behavior ) override;
        void onEntityDeactivate( const EntityBehaviorInterfacePtr & _behavior ) override;

    protected:
        void renderControls_( const ImGUIRenderProviderInterfacePtr & _provider );
        void syncPathInputs_();
        void loadSettings_();
        void saveSettings_() const;
        void applySelection_();
        void clearFigma_();
        void clearMountedFileGroup_();
        bool resolveInputPaths_( FileGroupInterfacePtr * const _fileGroup, FileGroupInterfacePtr * const _mountedFileGroup, FilePath * const _figPath );
        ResourceFigmaPtr createFigmaResource_( const FileGroupInterfacePtr & _fileGroup, const FilePath & _figPath );
        void resetEntryParams_();

    protected:
        Scene * m_scene;

        ImGUIRenderPtr m_imguiRender;
        NodePtr m_figmaNode;
        ResourceFigmaPtr m_figmaResource;
        UnknownFigmaInterface * m_figmaUnknown;
        FileGroupInterfacePtr m_mountedFileGroup;

        Char m_figPathInput[MENGINE_MAX_PATH];
        Char m_fontsPathInput[MENGINE_MAX_PATH];
        Char m_startFrameIdInput[256];

        float m_viewportInput[2];
        float m_positionInput[2];
        float m_scaleInput;
        bool m_autoViewportInput;
        int32_t m_fileIndex;
        int32_t m_entryIndex;
        String m_status;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<FigmaViewerExampleSceneEventReceiver> FigmaViewerExampleSceneEventReceiverPtr;
    //////////////////////////////////////////////////////////////////////////
}
