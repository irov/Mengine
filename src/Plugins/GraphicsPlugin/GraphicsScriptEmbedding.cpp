#include "GraphicsScriptEmbedding.h"

#include "Interface/VocabularyServiceInterface.h"
#include "Interface/StringizeServiceInterface.h"

#include "Environment/Python/PythonIncluder.h"
#include "Environment/Python/PythonScriptWrapper.h"
#include "Environment/Python/PythonAnimatableEventReceiver.h"
#include "Environment/Python/PythonDocumentTraceback.h"

#include "Graphics.h"

#include "Kernel/Logger.h"
#include "Kernel/Error.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"

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
    bool GraphicsScriptEmbedding::embed( pybind::kernel_interface * _kernel )
    {
        SCRIPT_SERVICE()
            ->setAvailablePlugin( "Graphics", true );

        pybind::interface_<Graphics, pybind::bases<Node>>( _kernel, "Graphics", false )
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

        Helper::registerScriptWrapping<Graphics>( _kernel, MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void GraphicsScriptEmbedding::eject( pybind::kernel_interface * _kernel )
    {
        _kernel->remove_scope<Graphics>();

        Helper::unregisterScriptWrapping<Graphics>();
    }
}
