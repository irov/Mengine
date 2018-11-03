#include "ResourceMovie.h"

#include "Interface/ResourceServiceInterface.h"
#include "Interface/ConverterServiceInterface.h"
#include "Interface/CodecServiceInterface.h"
#include "Interface/StringizeInterface.h"
#include "Interface/PrefetcherInterface.h"
#include "Interface/ConfigServiceInterface.h"
#include "Interface/FileSystemInterface.h"
#include "Interface/DataServiceInterface.h"
#include "Interface/MovieKeyFrameInterface.h"

#include "Engine/ResourceShape.h"

#include "Kernel/Stream.h"
#include "Kernel/Logger.h"

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
    void ResourceMovie::setDataflowType( const ConstString & _dataflowType )
    {
        m_dataflowType = _dataflowType;
    }
    //////////////////////////////////////////////////////////////////////////
    const ConstString & ResourceMovie::getDataflowType() const
    {
        return m_dataflowType;
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
    void ResourceMovie::setOffsetPoint( const mt::vec3f &  _offsetPoint )
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
    bool ResourceMovie::_convert()
    {
        //bool result = this->convertDefault_( m_converter, m_path, m_path, m_codecType );

        if( m_filePath.empty() == true )
        {
            return false;
        }

        if( m_converterType.empty() == false )
        {
            //FIX THIS

            PathString xml_path;

            xml_path += m_filePath;
            xml_path.replace_last( "xml" );

            FilePath c_xml_path = Helper::stringizeFilePath( xml_path );

            if( CONVERTER_SERVICE()
                ->convert( m_converterType, m_category, c_xml_path, m_filePath ) == false )
            {
                LOGGER_ERROR( "ResourceMovie::_convert: '%s' group '%s' can't convert '%s':'%s'"
                    , this->getName().c_str()
                    , this->getGroupName().c_str()
                    , c_xml_path.c_str()
                    , m_converterType.c_str()
                );

                return false;
            }
        }

        if( m_dataflowType.empty() == true )
        {
            m_dataflowType = CODEC_SERVICE()
                ->findCodecType( m_filePath );

            if( m_dataflowType.empty() == true )
            {
                LOGGER_ERROR( "ResourceMovie::_convert: '%s' group '%s' you must determine codec for file '%s'"
                    , this->getName().c_str()
                    , this->getGroupName().c_str()
                    , m_filePath.c_str()
                );

                return false;
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceMovie::_compile()
    {
        if( Resource::_compile() == false )
        {
            return false;
        }

        if( m_filePath.empty() == true )
        {
            LOGGER_ERROR( "ResourceMovie::_compile: '%s' group '%s' don`t have Key Frames Pack Path"
                , this->getName().c_str()
                , this->getGroupName().c_str()
            );

            return false;
        }

        const FileGroupInterfacePtr & fileGroup = this->getFileGroup();

        DataInterfacePtr data = this->compileData_( fileGroup, m_filePath );

        if( data == nullptr )
        {
            LOGGER_ERROR( "ResourceMovie::_compile: '%s' group '%s' can` t get frame pack '%s'"
                , this->getName().c_str()
                , this->getGroupName().c_str()
                , m_filePath.c_str()
            );

            return false;
        }

        m_keyFramePack = data;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    DataInterfacePtr ResourceMovie::compileData_( const FileGroupInterfacePtr & _category, const FilePath & _path )
    {
        DataInterfacePtr prefetch_data;
        if( PREFETCHER_SERVICE()
            ->getData( _category, _path, prefetch_data ) == true )
        {
            return prefetch_data;
        }

        const FileGroupInterfacePtr & fileGroup = this->getFileGroup();

        InputStreamInterfacePtr stream = FILE_SERVICE()
            ->openInputFile( fileGroup, _path, false );

        if( stream == nullptr )
        {
            LOGGER_ERROR( "ResourceMovie::compileData_: '%s' group '%s' don`t open Frames Pack '%s'"
                , this->getName().c_str()
                , this->getGroupName().c_str()
                , _path.c_str()
            );

            return nullptr;
        }

        const DataflowInterfacePtr & dataflow = DATA_SERVICE()
            ->getDataflow( m_dataflowType );

        if( dataflow == nullptr )
        {
            LOGGER_ERROR( "ResourceMovie::compileData_: '%s' group '%s' can` t find dataflow type '%s'"
                , this->getName().c_str()
                , this->getGroupName().c_str()
                , m_dataflowType.c_str()
            );

            return nullptr;
        }

        DataInterfacePtr data = DATA_SERVICE()
            ->dataflow( dataflow, stream );

        if( data == nullptr )
        {
            LOGGER_ERROR( "ResourceMovie::compileData_: '%s' group '%s' can` t dataflow '%s' from '%s'"
                , this->getName().c_str()
                , this->getGroupName().c_str()
                , m_dataflowType.c_str()
                , _path.c_str()
            );

            return nullptr;
        }

        return data;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceMovie::_release()
    {
        m_keyFramePack = nullptr;

        Resource::_release();
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceMovie::visitResourceMovie( VisitorResourceMovie * _visitor )
    {
        if( this->isCompile() == false )
        {
            LOGGER_ERROR( "ResourceMovie::visitResourceMovie '%s' group '%s' not compile"
                , this->getName().c_str()
                , this->getGroupName().c_str()
            );
        }

        for( const MovieLayer & layer : m_layers )
        {
            _visitor->visitLayer( m_keyFramePack, layer );
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