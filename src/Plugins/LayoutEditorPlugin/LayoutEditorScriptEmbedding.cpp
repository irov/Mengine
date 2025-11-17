#include "LayoutEditorScriptEmbedding.h"

#include "Environment/Python/PythonIncluder.h"

#include "LayoutEditorWidgetType.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    LayoutEditorScriptEmbedding::LayoutEditorScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    LayoutEditorScriptEmbedding::~LayoutEditorScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool LayoutEditorScriptEmbedding::embed( pybind::kernel_interface * _kernel )
    {
        pybind::enum_<ELayoutEditorWidgetType>( _kernel, "ELayoutEditorWidgetType" )
            .def( "LEWT_NONE", ELayoutEditorWidgetType::LEWT_NONE )
            .def( "LEWT_CHECKBOX", ELayoutEditorWidgetType::LEWT_CHECKBOX )
            .def( "LEWT_POSITION", ELayoutEditorWidgetType::LEWT_POSITION )
            .def( "LEWT_SCALE", ELayoutEditorWidgetType::LEWT_SCALE )
            .def( "LEWT_ORIENTATION", ELayoutEditorWidgetType::LEWT_ORIENTATION )
            .def( "LEWT_ALPHA", ELayoutEditorWidgetType::LEWT_ALPHA )
            .def( "LEWT_RGB", ELayoutEditorWidgetType::LEWT_RGB )
            ;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void LayoutEditorScriptEmbedding::eject( pybind::kernel_interface * _kernel )
    {
        MENGINE_UNUSED( _kernel );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
}