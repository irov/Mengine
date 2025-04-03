#include "ResourceScriptEmbedding.h"

#include "Interface/ApplicationInterface.h"
#include "Interface/TimelineServiceInterface.h"
#include "Interface/RenderSystemInterface.h"
#include "Interface/InputServiceInterface.h"
#include "Interface/PrototypeServiceInterface.h"
#include "Interface/MemoryInterface.h"
#include "Interface/PackageInterface.h"
#include "Interface/SceneServiceInterface.h"
#include "Interface/ScriptServiceInterface.h"
#include "Interface/SchedulerInterface.h"
#include "Interface/FontInterface.h"
#include "Interface/AccountInterface.h"
#include "Interface/ResourceServiceInterface.h"
#include "Interface/TextServiceInterface.h"
#include "Interface/FontServiceInterface.h"
#include "Interface/ApplicationInterface.h"

#include "Environment/Python/PythonAnimatableEventReceiver.h"
#include "Environment/Python/PythonEventReceiver.h"
#include "Environment/Python/PythonDocumentTraceback.h"
#include "Environment/Python/PythonScriptWrapper.h"

#include "Plugins/MoviePlugin/ResourceMovie2.h"

#include "ScriptHolder.h"
#include "ScriptableAffectorCallback.h"
#include "PythonEntityBehavior.h"
#include "PythonHotSpotEventReceiver.h"
#include "PythonScheduleTiming.h"
#include "PythonSchedulePipe.h"
#include "PythonScheduleEvent.h"
#include "DelaySchedulePipe.h"

#include "Engine/ResourceFile.h"
#include "Engine/ResourceMusic.h"
#include "Engine/ResourceTestPick.h"
#include "Engine/ResourceHIT.h"
#include "Engine/ResourceShape.h"
#include "Engine/ResourceCursorICO.h"
#include "Engine/ResourceCursorSystem.h"
#include "Engine/ResourceWindow.h"

#include "Kernel/Eventable.h"
#include "Kernel/ThreadTask.h"
#include "Kernel/NodeRenderHierarchy.h"
#include "Kernel/MatrixProxy.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/UnicodeHelper.h"
#include "Kernel/ResourceSound.h"
#include "Kernel/RenderCameraProjection.h"
#include "Kernel/RenderContextHelper.h"
#include "Kernel/ResourceImage.h"
#include "Kernel/ResourceImageData.h"
#include "Kernel/ResourceImageEmpty.h"
#include "Kernel/ResourceImageSequence.h"
#include "Kernel/ResourceImageSolid.h"
#include "Kernel/ResourceImageDefault.h"
#include "Kernel/ResourceImageSubstractRGBAndAlpha.h"
#include "Kernel/ResourceImageSubstract.h"
#include "Kernel/Shape.h"
#include "Kernel/Layer.h"
#include "Kernel/HotSpot.h"
#include "Kernel/Entity.h"
#include "Kernel/Logger.h"
#include "Kernel/Identity.h"
#include "Kernel/Affector.h"
#include "Kernel/AffectorHelper.h"
#include "Kernel/ThreadTask.h"
#include "Kernel/Polygon.h"
#include "Kernel/MemoryStreamHelper.h"
#include "Kernel/ValueFollower.h"
#include "Kernel/Rect.h"
#include "Kernel/Polygon.h"
#include "Kernel/ValueFollower.h"
#include "Kernel/ValueInterpolatorParabolic.h"
#include "Kernel/FactoryPool.h"
#include "Kernel/AssertionFactory.h"

#include "math/angle.h"
#include "math/vec4.h"
#include "math/mat3.h"
#include "math/mat4.h"
#include "math/quat.h"
#include "math/utils.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ResourceScriptEmbedding::ResourceScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ResourceScriptEmbedding::~ResourceScriptEmbedding()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceScriptEmbedding::embed( pybind::kernel_interface * _kernel )
    {
        pybind::interface_<ResourceImage, pybind::bases<Resource>>( _kernel, "ResourceImage", false )
            .def( "setMaxSize", &ResourceImage::setMaxSize )
            .def( "getMaxSize", &ResourceImage::getMaxSize )
            .def( "setSize", &ResourceImage::setSize )
            .def( "getSize", &ResourceImage::getSize )
            .def( "setOffset", &ResourceImage::setOffset )
            .def( "getOffset", &ResourceImage::getOffset )
            .def( "setUV", &ResourceImage::setUV )
            .def( "getUV", &ResourceImage::getUV )
            .def( "setUVTexture", &ResourceImage::setUVTexture )
            .def( "getUVTexture", &ResourceImage::getUVTexture )
            .def( "setAlpha", &ResourceImage::setAlpha )
            .def( "isAlpha", &ResourceImage::hasAlpha )
            .def( "setPremultiply", &ResourceImage::setPremultiply )
            .def( "isPremultiply", &ResourceImage::isPremultiply )
            .def( "setPow2", &ResourceImage::setPow2 )
            .def( "isPow2", &ResourceImage::isPow2 )
            .def( "setColor", &ResourceImage::setColor )
            .def( "getColor", &ResourceImage::getColor )
            ;

        pybind::interface_<ResourceImageData, pybind::bases<Resource>>( _kernel, "ResourceImageData", false )
            .def( "setImageMaxSize", &ResourceImageData::setImageMaxSize )
            .def( "getImageMaxSize", &ResourceImageData::getImageMaxSize )
            .def( "setImageWidth", &ResourceImageData::setImageWidth )
            .def( "getImageWidth", &ResourceImageData::getImageWidth )
            .def( "setImageHeight", &ResourceImageData::setImageHeight )
            .def( "getImageHeight", &ResourceImageData::getImageHeight )
            ;

        pybind::interface_<ResourceImageDefault, pybind::bases<ResourceImage>>( _kernel, "ResourceImageDefault", false )
            .def( "correctUVTexture", &ResourceImageDefault::correctUVTexture )
            ;

        pybind::interface_<ResourceImageSubstractRGBAndAlpha, pybind::bases<ResourceImage>>( _kernel, "ResourceImageSubstractRGBAndAlpha", false )
            ;

        pybind::interface_<ResourceImageSubstract, pybind::bases<ResourceImage>>( _kernel, "ResourceImageSubstract", false )
            ;

        pybind::interface_<ResourceImageSolid, pybind::bases<ResourceImage>>( _kernel, "ResourceImageSolid", false )
            ;

        pybind::interface_<ResourceMusic, pybind::bases<Resource>>( _kernel, "ResourceMusic", false )
            ;

        pybind::interface_<ResourceImageSequence, pybind::bases<Resource>>( _kernel, "ResourceImageSequence", false )
            ;

        pybind::interface_<ResourceSound, pybind::bases<Resource>>( _kernel, "ResourceSound", false )
            ;

        pybind::interface_<ResourceFile, pybind::bases<Resource>>( _kernel, "ResourceFile", false )
            ;

        pybind::interface_<ResourceShape, pybind::bases<Resource>>( _kernel, "ResourceShape", false )
            .def( "setPolygon", &ResourceShape::setPolygon )
            .def( "getPolygon", &ResourceShape::getPolygon )
            ;

        pybind::interface_<ResourceCursor, pybind::bases<Resource>>( _kernel, "ResourceCursor", false )
            ;

        pybind::interface_<ResourceCursorICO, pybind::bases<ResourceCursor>>( _kernel, "ResourceCursorICO", false )
            ;

        pybind::interface_<ResourceCursorSystem, pybind::bases<ResourceCursor>>( _kernel, "ResourceCursorSystem", false )
            ;

        pybind::interface_<ResourceWindow, pybind::bases<Resource>>( _kernel, "ResourceWindow", false )
            ;

        pybind::interface_<ResourceTestPick, pybind::bases<Resource>>( _kernel, "ResourceTestPick", false )
            .def_deprecated( "getWidth", &ResourceTestPick::getImageWidth, "use getImageWidth" )
            .def_deprecated( "getHeight", &ResourceTestPick::getImageHeight, "use getImageHeight" )
            .def( "getImageWidth", &ResourceTestPick::getImageWidth )
            .def( "getImageHeight", &ResourceTestPick::getImageHeight )
            .def( "getImageWidthF", &ResourceTestPick::getImageWidthF )
            .def( "getImageHeightF", &ResourceTestPick::getImageHeightF )
            ;

        pybind::interface_<ResourceHIT, pybind::bases<ResourceTestPick>>( _kernel, "ResourceHIT", false )
            ;

        Helper::registerScriptWrappingEx<Resource>( _kernel, Resource::getFactorableType(), MENGINE_DOCUMENT_FACTORABLE );

#define SCRIPT_CLASS_WRAPPING( Class )\
    Helper::registerScriptWrapping<Class>(_kernel, MENGINE_DOCUMENT_FACTORABLE)

        SCRIPT_CLASS_WRAPPING( ResourceImage );        
        SCRIPT_CLASS_WRAPPING( ResourceImageData );
        SCRIPT_CLASS_WRAPPING( ResourceImageDefault );
        SCRIPT_CLASS_WRAPPING( ResourceImageEmpty );
        SCRIPT_CLASS_WRAPPING( ResourceMusic );
        SCRIPT_CLASS_WRAPPING( ResourceImageSequence );
        SCRIPT_CLASS_WRAPPING( ResourceSound );
        SCRIPT_CLASS_WRAPPING( ResourceFile );

        SCRIPT_CLASS_WRAPPING( ResourceImageSolid );
        SCRIPT_CLASS_WRAPPING( ResourceShape );
        SCRIPT_CLASS_WRAPPING( ResourceCursorICO );
        SCRIPT_CLASS_WRAPPING( ResourceCursorSystem );
        SCRIPT_CLASS_WRAPPING( ResourceWindow );
        SCRIPT_CLASS_WRAPPING( ResourceImageSubstractRGBAndAlpha );
        SCRIPT_CLASS_WRAPPING( ResourceImageSubstract );

        SCRIPT_CLASS_WRAPPING( ResourceTestPick );
        SCRIPT_CLASS_WRAPPING( ResourceHIT );
#undef SCRIPT_CLASS_WRAPPING

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceScriptEmbedding::eject( pybind::kernel_interface * _kernel )
    {
        MENGINE_UNUSED( _kernel );

        Helper::unregisterScriptWrappingEx( Node::getFactorableType() );
        Helper::unregisterScriptWrappingEx( Resource::getFactorableType() );
        Helper::unregisterScriptWrappingEx( Layer::getFactorableType() );
        Helper::unregisterScriptWrappingEx( HotSpot::getFactorableType() );
        Helper::unregisterScriptWrappingEx( Surface::getFactorableType() );

#define UNSCRIPT_CLASS_WRAPPING( Class )\
    Helper::unregisterScriptWrapping<Class>()

        UNSCRIPT_CLASS_WRAPPING( ResourceImage );
        UNSCRIPT_CLASS_WRAPPING( ResourceImageEmpty );
        UNSCRIPT_CLASS_WRAPPING( ResourceImageData );
        UNSCRIPT_CLASS_WRAPPING( ResourceImageDefault );
        UNSCRIPT_CLASS_WRAPPING( ResourceMusic );
        UNSCRIPT_CLASS_WRAPPING( ResourceImageSequence );
        UNSCRIPT_CLASS_WRAPPING( ResourceSound );
        UNSCRIPT_CLASS_WRAPPING( ResourceFile );

        UNSCRIPT_CLASS_WRAPPING( ResourceImageSolid );
        UNSCRIPT_CLASS_WRAPPING( ResourceShape );
        UNSCRIPT_CLASS_WRAPPING( ResourceCursorICO );
        UNSCRIPT_CLASS_WRAPPING( ResourceCursorSystem );
        UNSCRIPT_CLASS_WRAPPING( ResourceWindow );
        UNSCRIPT_CLASS_WRAPPING( ResourceImageSubstractRGBAndAlpha );
        UNSCRIPT_CLASS_WRAPPING( ResourceImageSubstract );

        UNSCRIPT_CLASS_WRAPPING( ResourceTestPick );
        UNSCRIPT_CLASS_WRAPPING( ResourceHIT );

#undef UNSCRIPT_CLASS_WRAPPING
    }
    //////////////////////////////////////////////////////////////////////////
}
