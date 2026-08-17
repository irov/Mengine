#include "ResourceTiledMap.h"

#include "Interface/MetabufLoaderServiceInterface.h"

#include "Kernel/ConstStringHelper.h"
#include "Kernel/FilePathHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/Stringalized.h"

#include "Metacode/Metacode.h"

namespace Mengine
{
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        static bool makeTiledMapProperties( const Metabuf::Vector<T> & _metadata, TiledMapProperties * const _properties )
        {
            for( const T & metadata : _metadata )
            {
                ConstString name = Helper::stringizeString( metadata.get_Name() );
                const String & value = metadata.get_Value();
                ParamVariant property;

                switch( metadata.get_Type() )
                {
                case ETMPT_STRING:
                case ETMPT_COLOR:
                    property.emplace<ParamString>( value );
                    break;
                case ETMPT_BOOL:
                    if( value == "true" )
                    {
                        property.emplace<ParamBool>( true );
                    }
                    else if( value == "false" )
                    {
                        property.emplace<ParamBool>( false );
                    }
                    else
                    {
                        return false;
                    }
                    break;
                case ETMPT_INTEGER:
                case ETMPT_OBJECT:
                    {
                        ParamInteger integerValue;

                        if( Helper::stringalized( value.c_str(), &integerValue ) == false )
                        {
                            return false;
                        }

                        property.emplace<ParamInteger>( integerValue );
                    }
                    break;
                case ETMPT_FLOAT:
                    {
                        ParamDouble doubleValue;

                        if( Helper::stringalized( value.c_str(), &doubleValue ) == false )
                        {
                            return false;
                        }

                        property.emplace<ParamDouble>( doubleValue );
                    }
                    break;
                case ETMPT_FILE:
                    property.emplace<ParamFilePath>( Helper::stringizeFilePath( value ) );
                    break;
                default:
                    return false;
                }

                if( _properties->emplace( name, std::move( property ) ).second == false )
                {
                    return false;
                }
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    ResourceTiledMap::ResourceTiledMap()
        : m_orientation( ETMO_ORTHOGONAL )
        , m_width( 0 )
        , m_height( 0 )
        , m_tileWidth( 0 )
        , m_tileHeight( 0 )
        , m_hexSideLength( 0 )
        , m_staggerAxis( 0 )
        , m_staggerIndex( 0 )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ResourceTiledMap::~ResourceTiledMap()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ETiledMapOrientation ResourceTiledMap::getOrientation() const
    {
        return m_orientation;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t ResourceTiledMap::getColumnCount() const
    {
        return m_width;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t ResourceTiledMap::getRowCount() const
    {
        return m_height;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t ResourceTiledMap::getTileWidth() const
    {
        return m_tileWidth;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t ResourceTiledMap::getTileHeight() const
    {
        return m_tileHeight;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t ResourceTiledMap::getHexSideLength() const
    {
        return m_hexSideLength;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t ResourceTiledMap::getStaggerAxis() const
    {
        return m_staggerAxis;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t ResourceTiledMap::getStaggerIndex() const
    {
        return m_staggerIndex;
    }
    //////////////////////////////////////////////////////////////////////////
    uint32_t ResourceTiledMap::getTileCount() const
    {
        uint32_t count = 0;

        for( const TiledMapTileLayer & layer : m_tileLayers )
        {
            count += (uint32_t)layer.tiles.size();
        }

        return count;
    }
    //////////////////////////////////////////////////////////////////////////
    const VectorTiledMapTilesets & ResourceTiledMap::getTilesets() const
    {
        return m_tilesets;
    }
    //////////////////////////////////////////////////////////////////////////
    const VectorTiledMapTileLayers & ResourceTiledMap::getTileLayers() const
    {
        return m_tileLayers;
    }
    //////////////////////////////////////////////////////////////////////////
    const VectorTiledMapLayers & ResourceTiledMap::getObjectLayers() const
    {
        return m_objectLayers;
    }
    //////////////////////////////////////////////////////////////////////////
    const TiledMapProperties & ResourceTiledMap::getMapProperties() const
    {
        return m_mapProperties;
    }
    //////////////////////////////////////////////////////////////////////////
    const TiledMapObject * ResourceTiledMap::getObject( uint32_t _layerIndex, uint32_t _objectIndex ) const
    {
        if( _layerIndex >= m_objectLayers.size() )
        {
            return nullptr;
        }

        const TiledMapLayer & layer = m_objectLayers[_layerIndex];

        if( _objectIndex >= layer.objects.size() )
        {
            return nullptr;
        }

        return &layer.objects[_objectIndex];
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceTiledMap::_compile()
    {
        if( SERVICE_IS_INITIALIZE( MetabufLoaderServiceInterface ) == false )
        {
            LOGGER_ERROR( "resource tiled map '%s' requires MetabufLoaderService"
                , this->getName().c_str()
            );

            return false;
        }

        Metacode::Meta_TiledMap::Meta_TiledMap metadata;

        if( METABUF_LOADER_SERVICE()->load( this->getContent(), &metadata, nullptr, MENGINE_DOCUMENT_FACTORABLE ) == false )
        {
            LOGGER_ERROR( "resource tiled map '%s' can't load Metabuf"
                , this->getName().c_str()
            );

            return false;
        }

        const uint32_t orientation = metadata.get_Orientation();

        if( orientation > ETMO_HEXAGONAL )
        {
            LOGGER_ERROR( "resource tiled map '%s' has invalid orientation %u"
                , this->getName().c_str()
                , orientation
            );

            return false;
        }

        m_orientation = (ETiledMapOrientation)orientation;
        m_staggerAxis = metadata.get_StaggerAxis();
        m_staggerIndex = metadata.get_StaggerIndex();
        m_width = metadata.get_Width();
        m_height = metadata.get_Height();
        m_tileWidth = metadata.get_TileWidth();
        m_tileHeight = metadata.get_TileHeight();
        m_hexSideLength = metadata.get_HexSideLength();
        if( Detail::makeTiledMapProperties( metadata.get_Includes_Property(), &m_mapProperties ) == false )
        {
            LOGGER_ERROR( "resource tiled map '%s' has invalid map properties"
                , this->getName().c_str()
            );

            this->clearData_();

            return false;
        }

        for( const auto & metadataTileset : metadata.get_Includes_Tileset() )
        {
            TiledMapTileset tileset;
            tileset.firstGid = metadataTileset.get_FirstGid();
            tileset.tileCount = metadataTileset.get_TileCount();
            tileset.columns = metadataTileset.get_Columns();
            tileset.tileWidth = metadataTileset.get_TileWidth();
            tileset.tileHeight = metadataTileset.get_TileHeight();
            tileset.margin = metadataTileset.get_Margin();
            tileset.spacing = metadataTileset.get_Spacing();
            tileset.imageWidth = metadataTileset.get_ImageWidth();
            tileset.imageHeight = metadataTileset.get_ImageHeight();
            tileset.offset = metadataTileset.get_Offset();
            tileset.name = metadataTileset.get_Name();
            tileset.resourceName = metadataTileset.get_Resource();

            for( const auto & metadataImage : metadataTileset.get_Includes_Image() )
            {
                TiledMapTileImage image;
                image.localId = metadataImage.get_LocalId();
                image.resourceName = metadataImage.get_Resource();
                image.imageWidth = metadataImage.get_Width();
                image.imageHeight = metadataImage.get_Height();
                tileset.tileImages.emplace_back( std::move( image ) );
            }

            m_tilesets.emplace_back( std::move( tileset ) );
        }

        for( const auto & metadataLayer : metadata.get_Includes_TileLayer() )
        {
            TiledMapTileLayer layer;
            layer.id = metadataLayer.get_Id();
            layer.name = metadataLayer.get_Name();
            layer.visible = metadataLayer.get_Visible();
            layer.opacity = metadataLayer.get_Opacity();
            layer.offset = metadataLayer.get_Offset();
            if( Detail::makeTiledMapProperties( metadataLayer.get_Includes_Property(), &layer.properties ) == false )
            {
                LOGGER_ERROR( "resource tiled map '%s' has invalid tile layer properties"
                    , this->getName().c_str()
                );

                this->clearData_();

                return false;
            }

            for( const auto & metadataTile : metadataLayer.get_Includes_Tile() )
            {
                const mt::vec2f & position = metadataTile.get_Position();
                TiledMapTile tile;
                tile.x = (int32_t)position.x;
                tile.y = (int32_t)position.y;
                tile.gid = metadataTile.get_Gid();
                layer.tiles.emplace_back( tile );
            }

            m_tileLayers.emplace_back( std::move( layer ) );
        }

        for( const auto & metadataLayer : metadata.get_Includes_ObjectLayer() )
        {
            TiledMapLayer layer;
            layer.id = metadataLayer.get_Id();
            layer.name = metadataLayer.get_Name();
            layer.visible = metadataLayer.get_Visible();
            layer.opacity = metadataLayer.get_Opacity();
            layer.offset = metadataLayer.get_Offset();
            if( Detail::makeTiledMapProperties( metadataLayer.get_Includes_Property(), &layer.properties ) == false )
            {
                LOGGER_ERROR( "resource tiled map '%s' has invalid object layer properties"
                    , this->getName().c_str()
                );

                this->clearData_();

                return false;
            }

            for( const auto & metadataObject : metadataLayer.get_Includes_Object() )
            {
                const uint32_t shape = metadataObject.get_Shape();

                if( shape > ETMOS_TEXT )
                {
                    LOGGER_ERROR( "resource tiled map '%s' has invalid object shape %u"
                        , this->getName().c_str()
                        , shape
                    );

                    this->clearData_();

                    return false;
                }

                TiledMapObject object;
                object.id = metadataObject.get_Id();
                object.name = metadataObject.get_Name();
                object.type = metadataObject.get_Type();
                object.className = metadataObject.get_Class();
                object.shape = (ETiledMapObjectShape)shape;
                object.visible = metadataObject.get_Visible();
                object.position = metadataObject.get_Position();
                object.size = metadataObject.get_Size();
                object.rotation = metadataObject.get_Rotation();
                object.gid = metadataObject.get_Gid();
                if( Detail::makeTiledMapProperties( metadataObject.get_Includes_Property(), &object.properties ) == false )
                {
                    LOGGER_ERROR( "resource tiled map '%s' has invalid object properties"
                        , this->getName().c_str()
                    );

                    this->clearData_();

                    return false;
                }

                for( const auto & metadataPoint : metadataObject.get_Includes_Point() )
                {
                    object.points.emplace_back( metadataPoint.get_Position() );
                }

                layer.objects.emplace_back( std::move( object ) );
            }

            m_objectLayers.emplace_back( std::move( layer ) );
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceTiledMap::_release()
    {
        this->clearData_();
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceTiledMap::clearData_()
    {
        m_tilesets.clear();
        m_tileLayers.clear();
        m_objectLayers.clear();
        m_mapProperties.clear();
        m_orientation = ETMO_ORTHOGONAL;
        m_width = 0;
        m_height = 0;
        m_tileWidth = 0;
        m_tileHeight = 0;
        m_hexSideLength = 0;
        m_staggerAxis = 0;
        m_staggerIndex = 0;
    }
    //////////////////////////////////////////////////////////////////////////
}
