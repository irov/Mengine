#include "GraphicsScriptEmbedding.h"

#include "Interface/VocabularyServiceInterface.h"
#include "Interface/StringizeServiceInterface.h"

#include "Environment/Python/PythonScriptWrapper.h"
#include "Environment/Python/PythonAnimatableEventReceiver.h"

#include "Frameworks/PythonFramework/DocumentTraceback.h"

#include "Graphics.h"

#include "Kernel/Logger.h"
#include "Kernel/Error.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"

#include "pybind/pybind.hpp"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    GraphicsScriptEmbedding::GraphicsScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    GraphicsScriptEmbedding::~GraphicsScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool GraphicsScriptEmbedding::embedding( pybind::kernel_interface * _kernel )
    {
        pybind::interface_<Graphics, pybind::bases<Node, Eventable>>( _kernel, "Graphics", false )
            .def( "setLineWidth", &Graphics::setLineWidth )
            .def( "getLineWidth", &Graphics::getLineWidth )
            .def( "setLineSoft", &Graphics::setLineSoft )
            .def( "setLineSoft", &Graphics::setLineSoft )
            .def( "setLineColor", &Graphics::setLineColor )
            .def( "getLineColor", &Graphics::getLineColor )
            .def( "setCurveQuality", &Graphics::setCurveQuality )
            .def( "getCurveQuality", &Graphics::getCurveQuality )
            .def( "setEllipseQuality", &Graphics::setEllipseQuality )
            .def( "getEllipseQuality", &Graphics::getEllipseQuality )
            .def( "beginFill", &Graphics::beginFill )
            .def( "endFill", &Graphics::endFill )
            .def( "moveTo", &Graphics::moveTo )
            .def( "lineTo", &Graphics::lineTo )
            .def( "quadraticCurveTo", &Graphics::quadraticCurveTo )
            .def( "bezierCurveTo", &Graphics::bezierCurveTo )
            .def( "drawRect", &Graphics::drawRect )
            .def( "drawRoundedRect", &Graphics::drawRoundedRect )
            .def( "drawCircle", &Graphics::drawCircle )
            .def( "drawEllipse", &Graphics::drawEllipse )
            .def( "clear", &Graphics::clear )
            ;

        VOCABULARY_SET( ScriptWrapperInterface, STRINGIZE_STRING_LOCAL( "ClassWrapping" ), STRINGIZE_STRING_LOCAL( "Graphics" ), Helper::makeFactorableUnique<PythonScriptWrapper<Graphics>>( MENGINE_DOCUMENT_FACTORABLE, _kernel ), MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void GraphicsScriptEmbedding::ejecting( pybind::kernel_interface * _kernel )
    {
        _kernel->remove_scope<Graphics>();

        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "ClassWrapping" ), STRINGIZE_STRING_LOCAL( "Graphics" ) );
    }
}
