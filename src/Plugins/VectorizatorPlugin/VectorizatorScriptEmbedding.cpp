#include "VectorizatorScriptEmbedding.h"

#include "Interface/VocabularyServiceInterface.h"
#include "Interface/StringizeServiceInterface.h"

#include "Environment/Python/PythonScriptWrapper.h"
#include "Environment/Python/PythonAnimatableEventReceiver.h"

#include "Frameworks/PythonFramework/DocumentTraceback.h"

#include "Vectorizator.h"

#include "Kernel/Logger.h"
#include "Kernel/Error.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"

#include "pybind/pybind.hpp"

namespace Mengine
{    
    //////////////////////////////////////////////////////////////////////////
    VectorizatorScriptEmbedding::VectorizatorScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    VectorizatorScriptEmbedding::~VectorizatorScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool VectorizatorScriptEmbedding::embedding( pybind::kernel_interface * _kernel )
    {
        pybind::interface_<Vectorizator, pybind::bases<Node, Eventable> >( _kernel, "Vectorizator", false )
            .def( "setLineWidth", &Vectorizator::setLineWidth )
            .def( "getLineWidth", &Vectorizator::getLineWidth )
            .def( "setLineSoft", &Vectorizator::setLineSoft )
            .def( "setLineSoft", &Vectorizator::setLineSoft )
            .def( "setLineColor", &Vectorizator::setLineColor )
            .def( "getLineColor", &Vectorizator::getLineColor )
            .def( "setCurveQuality", &Vectorizator::setCurveQuality )
            .def( "getCurveQuality", &Vectorizator::getCurveQuality )
            .def( "setEllipseQuality", &Vectorizator::setEllipseQuality )
            .def( "getEllipseQuality", &Vectorizator::getEllipseQuality )
            .def( "beginFill", &Vectorizator::beginFill )
            .def( "endFill", &Vectorizator::endFill )
            .def( "moveTo", &Vectorizator::moveTo )
            .def( "lineTo", &Vectorizator::lineTo )
            .def( "quadraticCurveTo", &Vectorizator::quadraticCurveTo )
            .def( "bezierCurveTo", &Vectorizator::bezierCurveTo )
            .def( "drawRect", &Vectorizator::drawRect )
            .def( "drawRoundedRect", &Vectorizator::drawRoundedRect )
            .def( "drawCircle", &Vectorizator::drawCircle )
            .def( "drawEllipse", &Vectorizator::drawEllipse )
            .def( "clear", &Vectorizator::clear )
            ;

        VOCABULARY_SET( ScriptWrapperInterface, STRINGIZE_STRING_LOCAL( "ClassWrapping" ), STRINGIZE_STRING_LOCAL( "Vectorizator" ), Helper::makeFactorableUnique<PythonScriptWrapper<Vectorizator> >( MENGINE_DOCUMENT_FACTORABLE, _kernel ) );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void VectorizatorScriptEmbedding::ejecting( pybind::kernel_interface * _kernel )
    {
        _kernel->remove_scope<Vectorizator>();

        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "ClassWrapping" ), STRINGIZE_STRING_LOCAL( "Vectorizator" ) );        
    }
}
