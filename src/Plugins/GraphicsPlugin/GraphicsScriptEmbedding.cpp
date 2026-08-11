#include "GraphicsScriptEmbedding.h"

#include "Graphics.h"
#include "GraphicsPath.h"

#include "Environment/Python/PythonIncluder.h"
#include "Environment/Python/PythonScriptWrapper.h"

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
            .def( "GRAPHICS_LINE_CAP_ROUND", EGraphicsLineCap::Round );
        pybind::enum_<EGraphicsLineJoin>( _kernel, "GraphicsLineJoin" )
            .def( "GRAPHICS_LINE_JOIN_MITER", EGraphicsLineJoin::Miter )
            .def( "GRAPHICS_LINE_JOIN_BEVEL", EGraphicsLineJoin::Bevel )
            .def( "GRAPHICS_LINE_JOIN_ROUND", EGraphicsLineJoin::Round );
        pybind::enum_<EGraphicsFillRule>( _kernel, "GraphicsFillRule" )
            .def( "GRAPHICS_FILL_RULE_NON_ZERO", EGraphicsFillRule::NonZero )
            .def( "GRAPHICS_FILL_RULE_EVEN_ODD", EGraphicsFillRule::EvenOdd );
        pybind::enum_<EGraphicsStrokeAlignment>( _kernel, "GraphicsStrokeAlignment" )
            .def( "GRAPHICS_STROKE_CENTER", EGraphicsStrokeAlignment::Center )
            .def( "GRAPHICS_STROKE_INNER", EGraphicsStrokeAlignment::Inner )
            .def( "GRAPHICS_STROKE_OUTER", EGraphicsStrokeAlignment::Outer );
        pybind::enum_<EGraphicsUVMode>( _kernel, "GraphicsUVMode" )
            .def( "GRAPHICS_UV_BOUNDS", EGraphicsUVMode::Bounds )
            .def( "GRAPHICS_UV_PATH", EGraphicsUVMode::Path );
        pybind::enum_<EGraphicsPathMarkerType>( _kernel, "GraphicsPathMarkerType" )
            .def( "GRAPHICS_MARKER_NONE", EGraphicsPathMarkerType::None )
            .def( "GRAPHICS_MARKER_FILLED_ARROW", EGraphicsPathMarkerType::FilledArrow )
            .def( "GRAPHICS_MARKER_OPEN_ARROW", EGraphicsPathMarkerType::OpenArrow )
            .def( "GRAPHICS_MARKER_TRIANGLE", EGraphicsPathMarkerType::Triangle )
            .def( "GRAPHICS_MARKER_CIRCLE", EGraphicsPathMarkerType::Circle )
            .def( "GRAPHICS_MARKER_DIAMOND", EGraphicsPathMarkerType::Diamond );
        pybind::struct_<GraphicsPathPoint>( _kernel, "GraphicsPathPoint" )
            .def_constructor( pybind::init<>() )
            .def_member( "position", &GraphicsPathPoint::position )
            .def_member( "width", &GraphicsPathPoint::width )
            .def_member( "color", &GraphicsPathPoint::color )
            .def_member( "outlineColor", &GraphicsPathPoint::outlineColor );
        pybind::struct_<GraphicsPathStyle>( _kernel, "GraphicsPathStyle" )
            .def_constructor( pybind::init<>() )
            .def_member( "attributes", &GraphicsPathStyle::attributes )
            .def_member( "width", &GraphicsPathStyle::width )
            .def_member( "color", &GraphicsPathStyle::color )
            .def_member( "outlineColor", &GraphicsPathStyle::outlineColor );
        pybind::struct_<GraphicsPathSample>( _kernel, "GraphicsPathSample" )
            .def_constructor( pybind::init<>() )
            .def_member( "position", &GraphicsPathSample::position )
            .def_member( "tangent", &GraphicsPathSample::tangent )
            .def_member( "distance", &GraphicsPathSample::distance )
            .def_member( "ratio", &GraphicsPathSample::ratio )
            .def_member( "contourIndex", &GraphicsPathSample::contourIndex );
        pybind::struct_<GraphicsPathMarker>( _kernel, "GraphicsPathMarker" )
            .def_constructor( pybind::init<>() )
            .def_member( "type", &GraphicsPathMarker::type )
            .def_member( "length", &GraphicsPathMarker::length )
            .def_member( "width", &GraphicsPathMarker::width )
            .def_member( "inset", &GraphicsPathMarker::inset );
        pybind::interface_<GraphicsPath, pybind::bases<Factorable>>( _kernel, "GraphicsPath", false )
            .def( "setStyle", &GraphicsPath::setStyle )
            .def( "clearStyle", &GraphicsPath::clearStyle )
            .def( "clear", &GraphicsPath::clear )
            .def( "moveTo", &GraphicsPath::moveTo )
            .def( "lineTo", &GraphicsPath::lineTo )
            .def( "quadraticCurveTo", &GraphicsPath::quadraticCurveTo )
            .def( "bezierCurveTo", &GraphicsPath::bezierCurveTo )
            .def( "tangentArcTo", &GraphicsPath::tangentArcTo )
            .def( "ellipseArc", &GraphicsPath::ellipseArc )
            .def( "close", &GraphicsPath::close )
            .def( "polyline", &GraphicsPath::polyline )
            .def( "styledPolyline", &GraphicsPath::styledPolyline )
            .def( "catmullRom", &GraphicsPath::catmullRom )
            .def( "styledCatmullRom", &GraphicsPath::styledCatmullRom )
            .def( "getLength", &GraphicsPath::getLength )
            .def( "getBounds", &GraphicsPath::getBounds )
            .def( "sampleNormalized", &GraphicsPath::sampleNormalized )
            .def( "sampleDistance", &GraphicsPath::sampleDistance );

        pybind::interface_<Graphics, pybind::bases<Node>>( _kernel, "Graphics", false )
            .def( "setLineWidth", &Graphics::setLineWidth ).def( "getLineWidth", &Graphics::getLineWidth )
            .def( "setOutlineWidth", &Graphics::setOutlineWidth ).def( "getOutlineWidth", &Graphics::getOutlineWidth )
            .def( "setOutlineColor", &Graphics::setOutlineColor ).def( "getOutlineColor", &Graphics::getOutlineColor )
            .def( "setLineSoft", &Graphics::setLineSoft ).def( "getLineSoft", &Graphics::getLineSoft )
            .def( "setLineColor", &Graphics::setLineColor ).def( "getLineColor", &Graphics::getLineColor )
            .def( "setLineCap", &Graphics::setLineCap ).def( "getLineCap", &Graphics::getLineCap )
            .def( "setLineJoin", &Graphics::setLineJoin ).def( "getLineJoin", &Graphics::getLineJoin )
            .def( "setMiterLimit", &Graphics::setMiterLimit ).def( "getMiterLimit", &Graphics::getMiterLimit )
            .def( "setFillRule", &Graphics::setFillRule ).def( "getFillRule", &Graphics::getFillRule )
            .def( "setLineDash", &Graphics::setLineDash ).def( "clearLineDash", &Graphics::clearLineDash )
            .def( "setPathTrim", &Graphics::setPathTrim ).def( "setPathMarkers", &Graphics::setPathMarkers )
            .def( "setStrokeAlignment", &Graphics::setStrokeAlignment ).def( "getStrokeAlignment", &Graphics::getStrokeAlignment )
            .def( "setUVMode", &Graphics::setUVMode ).def( "getUVMode", &Graphics::getUVMode )
            .def( "setCurveQuality", &Graphics::setCurveQuality ).def( "getCurveQuality", &Graphics::getCurveQuality )
            .def( "setEllipseQuality", &Graphics::setEllipseQuality ).def( "getEllipseQuality", &Graphics::getEllipseQuality )
            .def( "setRectQuality", &Graphics::setRectQuality ).def( "getRectQuality", &Graphics::getRectQuality )
            .def( "pushState", &Graphics::pushState ).def( "popState", &Graphics::popState )
            .def( "beginFill", &Graphics::beginFill ).def( "endFill", &Graphics::endFill )
            .def( "createPath", &Graphics::createPath ).def( "drawPath", &Graphics::drawPath )
            .def( "drawRect", &Graphics::drawRect ).def( "drawRoundedRect", &Graphics::drawRoundedRect )
            .def( "drawRoundedRectVarying", &Graphics::drawRoundedRectVarying )
            .def( "drawRoundedPolygon", &Graphics::drawRoundedPolygon )
            .def( "drawPolyline", &Graphics::drawPolyline ).def( "drawStyledPolyline", &Graphics::drawStyledPolyline )
            .def( "drawRoundedPolyline", &Graphics::drawRoundedPolyline ).def( "drawPolygonOffset", &Graphics::drawPolygonOffset )
            .def( "drawCircle", &Graphics::drawCircle ).def( "drawEllipse", &Graphics::drawEllipse )
            .def( "drawArc", &Graphics::drawArc ).def( "drawEllipseArc", &Graphics::drawEllipseArc )
            .def( "drawRing", &Graphics::drawRing ).def( "drawRoundedRing", &Graphics::drawRoundedRing )
            .def( "drawCapsule", &Graphics::drawCapsule ).def( "drawRegularPolygon", &Graphics::drawRegularPolygon )
            .def( "drawStar", &Graphics::drawStar ).def( "drawSuperellipse", &Graphics::drawSuperellipse )
            .def( "getBounds", &Graphics::getBounds ).def( "clear", &Graphics::clear );

        Helper::registerScriptWrapping<Graphics>( _kernel, MENGINE_DOCUMENT_FACTORABLE );
        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void GraphicsScriptEmbedding::eject( pybind::kernel_interface * _kernel )
    {
        _kernel->remove_scope<Graphics>();
        _kernel->remove_scope<GraphicsPath>();
        _kernel->remove_scope<GraphicsPathMarker>();
        _kernel->remove_scope<GraphicsPathSample>();
        _kernel->remove_scope<GraphicsPathStyle>();
        _kernel->remove_scope<GraphicsPathPoint>();
        Helper::unregisterScriptWrapping<Graphics>();
    }
    //////////////////////////////////////////////////////////////////////////
}
