#include "GraphicsScriptEmbedding.h"

#include "Interface/StringizeServiceInterface.h"
#include "Interface/PluginServiceInterface.h"

#include "Environment/Python/PythonIncluder.h"
#include "Environment/Python/PythonScriptWrapper.h"
#include "Environment/Python/PythonAnimatableEventReceiver.h"
#include "Environment/Python/PythonDocument.h"

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
        pybind::enum_<EGraphicsLineCap>( _kernel, "GraphicsLineCap" )
            .def( "GRAPHICS_LINE_CAP_BUTT", EGraphicsLineCap::Butt )
            .def( "GRAPHICS_LINE_CAP_SQUARE", EGraphicsLineCap::Square )
            .def( "GRAPHICS_LINE_CAP_ROUND", EGraphicsLineCap::Round )
            ;

        pybind::enum_<EGraphicsLineJoin>( _kernel, "GraphicsLineJoin" )
            .def( "GRAPHICS_LINE_JOIN_MITER", EGraphicsLineJoin::Miter )
            .def( "GRAPHICS_LINE_JOIN_BEVEL", EGraphicsLineJoin::Bevel )
            .def( "GRAPHICS_LINE_JOIN_ROUND", EGraphicsLineJoin::Round )
            ;

        pybind::enum_<EGraphicsFillRule>( _kernel, "GraphicsFillRule" )
            .def( "GRAPHICS_FILL_RULE_NON_ZERO", EGraphicsFillRule::NonZero )
            .def( "GRAPHICS_FILL_RULE_EVEN_ODD", EGraphicsFillRule::EvenOdd )
            ;

        pybind::interface_<Graphics, pybind::bases<Node>>( _kernel, "Graphics", false )
            .def( "setLineWidth", &Graphics::setLineWidth )
            .def( "getLineWidth", &Graphics::getLineWidth )
            .def( "setLineSoft", &Graphics::setLineSoft )
            .def( "getLineSoft", &Graphics::getLineSoft )
            .def( "setLineColor", &Graphics::setLineColor )
            .def( "getLineColor", &Graphics::getLineColor )
            .def( "setLineCap", &Graphics::setLineCap )
            .def( "getLineCap", &Graphics::getLineCap )
            .def( "setLineJoin", &Graphics::setLineJoin )
            .def( "getLineJoin", &Graphics::getLineJoin )
            .def( "setMiterLimit", &Graphics::setMiterLimit )
            .def( "getMiterLimit", &Graphics::getMiterLimit )
            .def( "setFillRule", &Graphics::setFillRule )
            .def( "getFillRule", &Graphics::getFillRule )
            .def( "setCurveQuality", &Graphics::setCurveQuality )
            .def( "getCurveQuality", &Graphics::getCurveQuality )
            .def( "setEllipseQuality", &Graphics::setEllipseQuality )
            .def( "getEllipseQuality", &Graphics::getEllipseQuality )
            .def( "beginFill", &Graphics::beginFill )
            .def( "endFill", &Graphics::endFill )
            .def( "beginCompoundFill", &Graphics::beginCompoundFill )
            .def( "endCompoundFill", &Graphics::endCompoundFill )
            .def( "pointMoveTo", &Graphics::pointMoveTo )
            .def( "pointLineTo", &Graphics::pointLineTo )
            .def( "pointQuadraticCurveTo", &Graphics::pointQuadraticCurveTo )
            .def( "pointBezierCurveTo", &Graphics::pointBezierCurveTo )
            .def( "pointClose", &Graphics::pointClose )
            .def( "pointArcTo", &Graphics::pointArcTo )
            .def( "drawRect", &Graphics::drawRect )
            .def( "drawRoundedRect", &Graphics::drawRoundedRect )
            .def( "drawRoundedPolygon", &Graphics::drawRoundedPolygon )
            .def( "drawPolyline", &Graphics::drawPolyline )
            .def( "drawCircle", &Graphics::drawCircle )
            .def( "drawEllipse", &Graphics::drawEllipse )
            .def( "drawArc", &Graphics::drawArc )
            .def( "drawEllipseArc", &Graphics::drawEllipseArc )
            .def( "drawSector", &Graphics::drawSector )
            .def( "drawRing", &Graphics::drawRing )
            .def( "getBounds", &Graphics::getBounds )
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
    //////////////////////////////////////////////////////////////////////////
}
