#pragma once

#include "Interface/ScriptModuleInterface.h"
#include "Interface/RenderTargetInterface.h"
#include "Interface/RenderTextureInterface.h"
#include "Interface/RenderPipelineInterface.h"

#include "Plugins/ImGUIPlugin/ImGUIInterface.h"

#include "LayoutEditorLogger.h"

#include "Kernel/ModuleBase.h"
#include "Kernel/DummySceneEventReceiver.h"
#include "Kernel/Scene.h"
#include "Kernel/Histogram.h"
#include "Kernel/String.h"
#include "Kernel/Vector.h"
#include "Kernel/Map.h"
#include "Kernel/ConstString.h"
#include "Kernel/Node.h"
#include "Kernel/Color.h"

#include "Interface/TransformationInterface.h"

#include "Environment/Python/PythonIncluder.h"

#include "Config/StdIntTypes.h"

namespace Mengine
{
    class LayoutEditorModule
        : public ModuleBase
        , public RenderDrawPrimitiveInterface
    {
        DECLARE_FACTORABLE( LayoutEditorModule );

    public:
        LayoutEditorModule();
        ~LayoutEditorModule() override;

    public:
        bool _initializeModule() override;
        void _finalizeModule() override;

    protected:
        bool _handleKeyEvent( const InputKeyEvent & _event ) override;

    protected:
        void _beginUpdate( bool _focus ) override;

    protected:
        void _render( const RenderPipelineInterfacePtr & _renderPipeline, const RenderContext * _context ) override;

    protected:
        void onRenderDrawPrimitives( const RenderPrimitive * _primitives, uint32_t _count ) const override;

    protected:
        void notifyBootstrapperInitializeGame_();

    protected:
        void renderImGUI();
        void renderImGUIHeader();
        void renderImGUILayouts();
        void renderImGUIObject( const pybind::object & _obj );
        void renderImGUIObjectsChildren( const pybind::list & _children );
        void renderImGUIGroups( const pybind::list & _groups );
        void renderImGUIObjectsTree();
        void renderImGUISelectedObjectAttributes();
        void renderImGUINodeInspector();
        void renderImGUIConsole();

    protected:
        bool createViewportRenderTarget_( uint32_t _width, uint32_t _height );
        void destroyViewportRenderTarget_();
        void renderSceneToViewport_();
        void renderViewportInImGui_();

    protected:
        NodePtr getObjectEntityNode_( const pybind::object & _obj ) const;
        float getWidgetStep_( const pybind::object & _widget, float _default ) const;
        bool drawRotationAxisControl_( const Char * _label, float & _value );
        bool drawVec3Attribute_( mt::vec3f & _value, float _step, bool _clamp, float _minValue, float _maxValue );
        bool drawOrientationAttribute_( mt::vec3f & _value );
        bool drawRGBAttribute_( mt::vec3f & _value );

    protected:
        Resolution m_fromResolution;
        Resolution m_viewportDeviceResolution;

        Scene * m_scene;

        ImGUIRenderProviderInterfacePtr m_imguiRenderProvider;

        ScriptModuleInterfacePtr m_module;

        pybind::object m_selectObject;
        NodePtr m_selectNode;

        bool m_show;

        bool m_showObjectsWindow;
        bool m_showViewportWindow;
        bool m_showAttributesWindow;
        bool m_showNodeWindow;
        bool m_showConsoleWindow;
        bool m_showViewportDeviceFrame;
        Color m_viewportDeviceFrameColor;
        float m_viewportDeviceFrameWidth;
        float m_viewportDeviceCornerRadius;
        float m_viewportDeviceFitScale;

        // Logger
        LayoutEditorLoggerPtr m_logger;
        
        // Console filters
        Char m_consoleFilterBuffer[256];
        bool m_consoleFilterLevels[LOGGER_LEVEL_COUNT];
        Map<String, bool> m_consoleFilterCategories;
        String m_consoleExclusiveCategory;
        bool m_consoleAutoScroll;
        bool m_consoleClearOnPlay;

        // Viewport rendering
        RenderTargetInterfacePtr m_viewportRenderTarget;
        RenderTextureInterfacePtr m_viewportRenderTexture;
        RenderPipelineInterfacePtr m_viewportRenderPipeline;
        uint32_t m_viewportWidth;
        uint32_t m_viewportHeight;
    };
}