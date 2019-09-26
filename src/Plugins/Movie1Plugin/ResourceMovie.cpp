#include "ResourceMovie.h"

#include "MovieKeyFrameInterface.h"

#include "Interface/ResourceServiceInterface.h"
#include "Interface/ConverterServiceInterface.h"
#include "Interface/CodecServiceInterface.h"
#include "Interface/PrefetcherServiceInterface.h"
#include "Interface/ConfigServiceInterface.h"
#include "Interface/FileServiceInterface.h"
#include "Interface/VocabularyServiceInterface.h"

#include "Engine/ResourceShape.h"

#include "Kernel/Dataflow.h"
#include "Kernel/Stream.h"
#include "Kernel/Logger.h"
#include "Kernel/Document.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/ConstStringHelper.h"

#include "math/mat4.h"

#include <algorithm>
#include <math.h>

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ResourceMovie::ResourceMovie()
        : m_frameDuration( 0.f )
        , m_duration( 0.f )
        , m_loopSegment( 0.f, -1.f )
        , m_frameCount( 0 )
        , m_size( 0.f, 0.f )
        , m_maxLayerIndex( 0 )
        , m_hasCamera3D( false )
        , m_hasBoundBox( false )
        , m_hasAnchorPoint( false )
        , m_anchorPoint( 0.f, 0.f, 0.f )
        , m_hasOffsetPoint( false )
        , m_offsetPoint( 0.f, 0.f, 0.f )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ResourceMovie::~ResourceMovie()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceMovie::setFrameDuration( float _frameDuration )
    {
        m_frameDuration = _frameDuration;
    }
    //////////////////////////////////////////////////////////////////////////
    float ResourceMovie::getFrameDuration() const
    {
        return m_frameDuration;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceMovie::setDuration( float _duration )
    {
        m_duration = _duration;
    }
    //////////////////////////////////////////////////////////////////////////
    float ResourceMovie::getDuration() const
    {
        return m_duration;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceMovie::setFrameCount( uint32_t _frameCount )
    {
        m_frameCount = _frameCount;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t ResourceMovie::getFrameCount() const
    {
        return m_frameCount;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceMovie::setMaxLayerIndex( uint32_t _maxLayerIndex )
    {
        m_maxLayerIndex = _maxLayerIndex;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t ResourceMovie::getMaxLayerIndex() const
    {
        return m_maxLayerIndex;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceMovie::setSize( const mt::vec2f & _size )
    {
        m_size = _size;
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::vec2f & ResourceMovie::getSize() const
    {
        return m_size;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceMovie::setLoopSegment( const mt::vec2f & _loopSegment )
    {
        m_loopSegment = _loopSegment;
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::vec2f & ResourceMovie::getLoopSegment() const
    {
        return m_loopSegment;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceMovie::setLayers( const VectorMovieLayers & _layers )
    {
        m_layers = _layers;
    }
    //////////////////////////////////////////////////////////////////////////
    const VectorMovieLayers & ResourceMovie::getLayers() const
    {
        return m_layers;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceMovie::hasCamera3D() const
    {
        return m_hasCamera3D;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceMovie::setCamera3D( const MovieLayerCamera3D & _camera3d )
    {
        m_camera3D = _camera3d;

        m_hasCamera3D = true;
    }
    //////////////////////////////////////////////////////////////////////////
    const MovieLayerCamera3D & ResourceMovie::getCamera3D() const
    {
        return m_camera3D;
    }
    //////////////////////////////////////////////////////////////////////////
    ResourceShapePtr ResourceMovie::getSocketResourceShape( const ConstString & _socketName ) const
    {
        for( const MovieLayer & layer : m_layers )
        {
            if( layer.name != _socketName )
            {
                continue;
            }

            ResourceShapePtr resourceShape = RESOURCE_SERVICE()
                ->getResourceReference( layer.source );

            return resourceShape;
        }

        return nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceMovie::hasAnchorPoint() const
    {
        return m_hasAnchorPoint;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceMovie::setAnchorPoint( const mt::vec3f & _anchorPoint )
    {
        m_anchorPoint = _anchorPoint;

        m_hasAnchorPoint = true;
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::vec3f & ResourceMovie::getAnchorPoint() const
    {
        return m_anchorPoint;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceMovie::hasOffsetPoint() const
    {
        return m_hasOffsetPoint;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceMovie::setOffsetPoint( const mt::vec3f & _offsetPoint )
    {
        m_offsetPoint = _offsetPoint;

        m_hasOffsetPoint = true;
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::vec3f & ResourceMovie::getOffsetPoint() const
    {
        return m_offsetPoint;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceMovie::hasBoundBox() const
    {
        return m_hasBoundBox;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceMovie::setBoundBox( const mt::box2f & _boundBox )
    {
        m_boundBox = _boundBox;

        m_hasBoundBox = true;
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::box2f & ResourceMovie::getBoundBox() const
    {
        return m_boundBox;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceMovie::_compile()
    {
        const FilePath & filePath = this->getFilePath();

        if( filePath.empty() == true )
        {
            LOGGER_ERROR( "'%s' group '%s' don`t have Key Frames Pack Path"
                , this->getName().c_str()
                , this->getGroupName().c_str()
            );

            return false;
        }

        ContentInterface * content = this->getContent();

        const FileGroupInterfacePtr & fileGroup = content->getFileGroup();
        const DataflowInterfacePtr & dataflow = content->getDataflow();

        DataInterfacePtr data = Helper::getDataflow( fileGroup, filePath, dataflow, MENGINE_DOCUMENT_FUNCTION );

        MENGINE_ASSERTION_MEMORY_PANIC( data, false, "resource movie '%s' group '%s' can` t get frame pack '%s'"
            , this->getName().c_str()
            , this->getGroupName().c_str()
            , this->getFilePath().c_str()
        );

        m_keyFramePack = data;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceMovie::_release()
    {
        if( m_keyFramePack != nullptr )
        {
            m_keyFramePack->release();
            m_keyFramePack = nullptr;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceMovie::foreachResourceMovie( const LambdaResourceMovie & _lambda )
    {
        if( this->isCompile() == false )
        {
            LOGGER_ERROR( "resource '%s' group '%s' not compile"
                , this->getName().c_str()
                , this->getGroupName().c_str()
            );

            return;
        }

        for( const MovieLayer & layer : m_layers )
        {
            _lambda( m_keyFramePack, layer );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceMovie::hasMovieLayer( const ConstString & _name, const MovieLayer ** _layer ) const
    {
        for( const MovieLayer & layer : m_layers )
        {
            if( layer.name != _name )
            {
                continue;
            }

            *_layer = &layer;

            return true;
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceMovie::hasMovieLayerType( const ConstString & _name, const ConstString & _type, const MovieLayer ** _layer ) const
    {
        for( const MovieLayer & layer : m_layers )
        {
            if( layer.name != _name )
            {
                continue;
            }

            if( layer.type != _type )
            {
                continue;
            }

            *_layer = &layer;

            return true;
        }

        return false;
    }

}