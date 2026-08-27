#include "ResourceTiledMap.h"

#include "TiledMapReader.h"

#include "Kernel/Base64.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/Data.h"
#include "Kernel/FilePathHelper.h"
#include "Kernel/Hexadecimal.h"
#include "Kernel/JSONHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/PathHelper.h"
#include "Kernel/VectorString.h"

#include "Config/StdAlgorithm.h"
#include "Config/StdString.h"

#include "zlib.h"
#include "zstd.h"

#include <limits>

namespace Mengine
{
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        struct TiledMapData
        {
            VectorTiledMapTilesets tilesets;
            VectorTiledMapTileLayers tileLayers;
            VectorTiledMapLayers objectLayers;
            TiledMapProperties mapProperties;
            ETiledMapOrientation orientation = ETMO_ORTHOGONAL;
            uint32_t width = 0;
            uint32_t height = 0;
            uint32_t tileWidth = 0;
            uint32_t tileHeight = 0;
            uint32_t hexSideLength = 0;
            uint32_t staggerAxis = 0;
            uint32_t staggerIndex = 0;
        };
        //////////////////////////////////////////////////////////////////////////
        class TiledMapReader
        {
        public:
            TiledMapReader( const FileGroupInterfacePtr & _fileGroup, const FilePath & _mapPath, TiledMapData * const _data )
                : m_fileGroup( _fileGroup )
                , m_mapPath( _mapPath )
                , m_data( _data )
                , m_reverseX( false )
                , m_reverseY( false )
            {
            }

        public:
            bool read()
            {
                jpp::object map;

                if( Detail::loadTiledMapDocument( m_fileGroup, m_mapPath, ETMDT_MAP, &map ) == false )
                {
                    return false;
                }

                const Char * orientation = map.get( "orientation", "orthogonal" );

                if( StdString::strcmp( orientation, "orthogonal" ) == 0 )
                {
                    m_data->orientation = ETMO_ORTHOGONAL;
                }
                else if( StdString::strcmp( orientation, "isometric" ) == 0 )
                {
                    m_data->orientation = ETMO_ISOMETRIC;
                }
                else if( StdString::strcmp( orientation, "hexagonal" ) == 0 )
                {
                    m_data->orientation = ETMO_HEXAGONAL;
                }
                else
                {
                    LOGGER_ERROR( "resource tiled map '%s' has unsupported orientation '%s'"
                        , m_mapPath.c_str()
                        , orientation
                    );

                    return false;
                }

                if( this->readUInt32_( map, "width", 0, &m_data->width ) == false
                    || this->readUInt32_( map, "height", 0, &m_data->height ) == false
                    || this->readUInt32_( map, "tilewidth", 0, &m_data->tileWidth ) == false
                    || this->readUInt32_( map, "tileheight", 0, &m_data->tileHeight ) == false
                    || this->readUInt32_( map, "hexsidelength", 0, &m_data->hexSideLength ) == false )
                {
                    return this->error_( "has invalid map dimensions" );
                }

                const Char * staggerAxis = map.get( "staggeraxis", "x" );
                m_data->staggerAxis = StdString::strcmp( staggerAxis, "y" ) == 0 ? 1 : 0;

                const Char * staggerIndex = map.get( "staggerindex", "even" );
                m_data->staggerIndex = StdString::strcmp( staggerIndex, "odd" ) == 0 ? 1 : 0;

                const Char * renderOrder = map.get( "renderorder", "right-down" );
                String renderOrderValue( renderOrder );
                m_reverseX = renderOrderValue.compare( 0, 4, "left" ) == 0;
                m_reverseY = renderOrderValue.size() >= 2 && renderOrderValue.compare( renderOrderValue.size() - 2, 2, "up" ) == 0;

                if( this->readProperties_( map, &m_data->mapProperties ) == false )
                {
                    return false;
                }

                if( this->readTilesets_( map ) == false )
                {
                    return false;
                }

                TiledMapProperties parentProperties;

                if( this->readLayers_( map, String(), true, 1.f, 0.f, 0.f, parentProperties ) == false )
                {
                    return false;
                }

                return true;
            }

        private:
            bool error_( const Char * _message ) const
            {
                LOGGER_ERROR( "resource tiled map '%s' %s"
                    , m_mapPath.c_str()
                    , _message
                );

                return false;
            }

            bool readString_( const jpp::object & _owner, const Char * _key, const Char * _default, String * const _value ) const
            {
                jpp::object jsonValue;

                if( _owner.exist( _key, &jsonValue ) == false )
                {
                    *_value = _default;

                    return true;
                }

                if( jsonValue.is_type_string() == false )
                {
                    return false;
                }

                const Char * value = jsonValue;
                *_value = value;

                return true;
            }

            bool readUInt32_( const jpp::object & _owner, const Char * _key, uint32_t _default, uint32_t * const _value ) const
            {
                jpp::object jsonValue;

                if( _owner.exist( _key, &jsonValue ) == false )
                {
                    *_value = _default;

                    return true;
                }

                if( jsonValue.is_type_integer() == false )
                {
                    return false;
                }

                int64_t value = jsonValue;

                if( value < 0 || value > (int64_t)std::numeric_limits<uint32_t>::max() )
                {
                    return false;
                }

                *_value = (uint32_t)value;

                return true;
            }

            bool readInt32_( const jpp::object & _owner, const Char * _key, int32_t _default, int32_t * const _value ) const
            {
                jpp::object jsonValue;

                if( _owner.exist( _key, &jsonValue ) == false )
                {
                    *_value = _default;

                    return true;
                }

                if( jsonValue.is_type_integer() == false )
                {
                    return false;
                }

                int64_t value = jsonValue;

                if( value < (int64_t)std::numeric_limits<int32_t>::min() || value > (int64_t)std::numeric_limits<int32_t>::max() )
                {
                    return false;
                }

                *_value = (int32_t)value;

                return true;
            }

            bool readFloat_( const jpp::object & _owner, const Char * _key, float _default, float * const _value ) const
            {
                jpp::object jsonValue;

                if( _owner.exist( _key, &jsonValue ) == false )
                {
                    *_value = _default;

                    return true;
                }

                if( jsonValue.is_type_integer() == false && jsonValue.is_type_real() == false )
                {
                    return false;
                }

                double value = jsonValue;
                *_value = (float)value;

                return true;
            }

            bool readBool_( const jpp::object & _owner, const Char * _key, bool _default, bool * const _value ) const
            {
                jpp::object jsonValue;

                if( _owner.exist( _key, &jsonValue ) == false )
                {
                    *_value = _default;

                    return true;
                }

                if( jsonValue.is_true() == true )
                {
                    *_value = true;

                    return true;
                }

                if( jsonValue.is_false() == true )
                {
                    *_value = false;

                    return true;
                }

                return false;
            }

            bool readArray_( const jpp::object & _owner, const Char * _key, jpp::array * const _array, bool * const _exist ) const
            {
                jpp::object jsonArray;

                if( _owner.exist( _key, &jsonArray ) == false )
                {
                    *_exist = false;

                    return true;
                }

                if( jsonArray.is_type_array() == false )
                {
                    return false;
                }

                *_array = jpp::array( jsonArray );
                *_exist = true;

                return true;
            }

            bool makePropertyValueString_( const jpp::object & _property, String * const _value ) const
            {
                jpp::object jsonValue;

                if( _property.exist( "value", &jsonValue ) == false || jsonValue.is_null() == true )
                {
                    _value->clear();

                    return true;
                }

                if( jsonValue.is_type_string() == true )
                {
                    const Char * value = jsonValue;
                    *_value = value;

                    return true;
                }

                *_value = Helper::JSONToStringCompact( jsonValue );

                return true;
            }

            bool readProperties_( const jpp::object & _owner, TiledMapProperties * const _properties ) const
            {
                jpp::array properties;
                bool exist;

                if( this->readArray_( _owner, "properties", &properties, &exist ) == false )
                {
                    return this->error_( "has invalid properties array" );
                }

                if( exist == false )
                {
                    return true;
                }

                for( const jpp::object & jsonProperty : properties )
                {
                    if( jsonProperty.is_type_object() == false )
                    {
                        return this->error_( "has a non-object property" );
                    }

                    String name;
                    String type;

                    if( this->readString_( jsonProperty, "name", "", &name ) == false
                        || this->readString_( jsonProperty, "type", "string", &type ) == false )
                    {
                        return this->error_( "has an invalid property name or type" );
                    }

                    ParamVariant property;
                    jpp::object jsonValue;
                    bool hasValue = jsonProperty.exist( "value", &jsonValue ) == true;

                    if( type == "string" || type == "color" )
                    {
                        String value;
                        this->makePropertyValueString_( jsonProperty, &value );
                        property.emplace<ParamString>( std::move( value ) );
                    }
                    else if( type == "bool" )
                    {
                        if( hasValue == false || (jsonValue.is_true() == false && jsonValue.is_false() == false) )
                        {
                            return this->error_( "has an invalid bool property" );
                        }

                        property.emplace<ParamBool>( jsonValue.is_true() == true );
                    }
                    else if( type == "int" || type == "object" )
                    {
                        if( hasValue == false || jsonValue.is_type_integer() == false )
                        {
                            return this->error_( "has an invalid integer property" );
                        }

                        int64_t value = jsonValue;
                        property.emplace<ParamInteger>( value );
                    }
                    else if( type == "float" )
                    {
                        if( hasValue == false || (jsonValue.is_type_integer() == false && jsonValue.is_type_real() == false) )
                        {
                            return this->error_( "has an invalid float property" );
                        }

                        double value = jsonValue;
                        property.emplace<ParamDouble>( value );
                    }
                    else if( type == "file" )
                    {
                        String value;
                        this->makePropertyValueString_( jsonProperty, &value );
                        property.emplace<ParamFilePath>( Helper::stringizeFilePath( value ) );
                    }
                    else
                    {
                        LOGGER_ERROR( "resource tiled map '%s' has unsupported property type '%s'"
                            , m_mapPath.c_str()
                            , type.c_str()
                        );

                        return false;
                    }

                    ConstString propertyName = Helper::stringizeString( name );
                    _properties->erase( propertyName );
                    _properties->emplace( propertyName, std::move( property ) );
                }

                return true;
            }

            bool readLayerProperties_( const jpp::object & _owner, const TiledMapProperties & _parentProperties, TiledMapProperties * const _properties ) const
            {
                *_properties = _parentProperties;

                return this->readProperties_( _owner, _properties );
            }

            bool findPropertyString_( const jpp::object & _owner, const Char * _name, String * const _value ) const
            {
                jpp::array properties;
                bool exist;

                if( this->readArray_( _owner, "properties", &properties, &exist ) == false || exist == false )
                {
                    return false;
                }

                for( const jpp::object & property : properties )
                {
                    if( property.is_type_object() == false )
                    {
                        continue;
                    }

                    const Char * name = property.get( "name", "" );

                    if( StdString::strcmp( name, _name ) != 0 )
                    {
                        continue;
                    }

                    if( this->makePropertyValueString_( property, _value ) == false )
                    {
                        return false;
                    }

                    return _value->empty() == false;
                }

                return false;
            }

            String makeImageResourceName_( const jpp::object & _owner, const String & _image ) const
            {
                String explicitResource;

                if( this->findPropertyString_( _owner, "mengine_resource", &explicitResource ) == true )
                {
                    return explicitResource;
                }

                String::size_type slash = _image.find_last_of( "/\\" );
                String::size_type begin = slash == String::npos ? 0 : slash + 1;
                String::size_type dot = _image.find_last_of( '.' );

                if( dot == String::npos || dot < begin )
                {
                    dot = _image.size();
                }

                return _image.substr( begin, dot - begin );
            }

            bool makeExternalPath_( const FilePath & _ownerPath, const String & _source, FilePath * const _filePath ) const
            {
                if( _source.empty() == true || _source[0] == '/' || (_source.size() > 1 && _source[1] == ':') )
                {
                    return false;
                }

                String path = Helper::getFolderPath( _ownerPath ).c_str();
                path += _source;
                Helper::pathCorrectBackslashA( path.data() );

                VectorString parts;
                String::size_type position = 0;

                while( position <= path.size() )
                {
                    String::size_type delimiter = path.find( '/', position );
                    String::size_type end = delimiter == String::npos ? path.size() : delimiter;
                    String part = path.substr( position, end - position );

                    if( part.empty() == false && part != "." )
                    {
                        if( part == ".." )
                        {
                            if( parts.empty() == true )
                            {
                                return false;
                            }

                            parts.pop_back();
                        }
                        else
                        {
                            parts.emplace_back( std::move( part ) );
                        }
                    }

                    if( delimiter == String::npos )
                    {
                        break;
                    }

                    position = delimiter + 1;
                }

                if( parts.empty() == true )
                {
                    return false;
                }

                String normalized;

                for( const String & part : parts )
                {
                    if( normalized.empty() == false )
                    {
                        normalized += '/';
                    }

                    normalized += part;
                }

                *_filePath = Helper::stringizeFilePath( normalized );

                return true;
            }

            bool readTilesets_( const jpp::object & _map )
            {
                jpp::array tilesets;
                bool exist;

                if( this->readArray_( _map, "tilesets", &tilesets, &exist ) == false )
                {
                    return this->error_( "has an invalid tilesets array" );
                }

                if( exist == false )
                {
                    return true;
                }

                for( const jpp::object & entry : tilesets )
                {
                    if( entry.is_type_object() == false )
                    {
                        return this->error_( "has a non-object tileset" );
                    }

                    uint32_t firstGid;

                    if( this->readUInt32_( entry, "firstgid", 0, &firstGid ) == false )
                    {
                        return this->error_( "has an invalid tileset firstgid" );
                    }

                    jpp::object tileset = entry;
                    FilePath tilesetPath = m_mapPath;
                    String source;

                    if( this->readString_( entry, "source", "", &source ) == false )
                    {
                        return this->error_( "has an invalid external tileset source" );
                    }

                    if( source.empty() == false )
                    {
                        if( this->makeExternalPath_( m_mapPath, source, &tilesetPath ) == false )
                        {
                            return this->error_( "references an invalid external tileset path" );
                        }

                        if( Detail::loadTiledMapDocument( m_fileGroup, tilesetPath, ETMDT_TILESET, &tileset ) == false )
                        {
                            return false;
                        }
                    }

                    TiledMapTileset result;
                    result.firstGid = firstGid;

                    if( this->readUInt32_( tileset, "tilewidth", m_data->tileWidth, &result.tileWidth ) == false
                        || this->readUInt32_( tileset, "tileheight", m_data->tileHeight, &result.tileHeight ) == false
                        || this->readUInt32_( tileset, "columns", 0, &result.columns ) == false
                        || this->readUInt32_( tileset, "margin", 0, &result.margin ) == false
                        || this->readUInt32_( tileset, "spacing", 0, &result.spacing ) == false
                        || this->readUInt32_( tileset, "imagewidth", 0, &result.imageWidth ) == false
                        || this->readUInt32_( tileset, "imageheight", 0, &result.imageHeight ) == false
                        || this->readUInt32_( tileset, "tilecount", 0, &result.tileCount ) == false
                        || this->readString_( tileset, "name", "", &result.name ) == false )
                    {
                        return this->error_( "has invalid tileset fields" );
                    }

                    String image;

                    if( this->readString_( tileset, "image", "", &image ) == false )
                    {
                        return this->error_( "has an invalid tileset image" );
                    }

                    result.resourceName = image.empty() == false ? this->makeImageResourceName_( tileset, image ) : String();

                    if( image.empty() == false && this->makeExternalPath_( tilesetPath, image, &result.imagePath ) == false )
                    {
                        return this->error_( "references an invalid tileset image path" );
                    }

                    result.offset = mt::vec2f( 0.f, 0.f );

                    String transparentColor;

                    if( this->readString_( tileset, "transparentcolor", "", &transparentColor ) == false )
                    {
                        return this->error_( "has an invalid tileset transparent color" );
                    }

                    if( transparentColor.empty() == false )
                    {
                        if( transparentColor.size() != 7 || transparentColor[0] != '#' )
                        {
                            return this->error_( "has an invalid tileset transparent color" );
                        }

                        uint8_t color[3];
                        size_t colorSize;

                        if( Helper::decodeHexadecimal( transparentColor.c_str() + 1, 6, color, 3, &colorSize ) == false || colorSize != 3 )
                        {
                            return this->error_( "has an invalid tileset transparent color" );
                        }

                        result.transparentColor = ((uint32_t)color[0] << 16) | ((uint32_t)color[1] << 8) | (uint32_t)color[2];
                    }

                    jpp::object tileOffset;

                    if( tileset.exist( "tileoffset", &tileOffset ) == true )
                    {
                        if( tileOffset.is_type_object() == false
                            || this->readFloat_( tileOffset, "x", 0.f, &result.offset.x ) == false
                            || this->readFloat_( tileOffset, "y", 0.f, &result.offset.y ) == false )
                        {
                            return this->error_( "has an invalid tileset tileoffset" );
                        }
                    }

                    int64_t maximumLocalId = -1;
                    jpp::array tiles;
                    bool tilesExist;

                    if( this->readArray_( tileset, "tiles", &tiles, &tilesExist ) == false )
                    {
                        return this->error_( "has an invalid tileset tiles array" );
                    }

                    if( tilesExist == true )
                    {
                        for( const jpp::object & tile : tiles )
                        {
                            if( tile.is_type_object() == false )
                            {
                                return this->error_( "has a non-object tileset tile" );
                            }

                            uint32_t localId;

                            if( this->readUInt32_( tile, "id", 0, &localId ) == false )
                            {
                                return this->error_( "has an invalid tileset local tile id" );
                            }

                            maximumLocalId = StdAlgorithm::max( maximumLocalId, (int64_t)localId );

                            String tileImage;

                            if( this->readString_( tile, "image", "", &tileImage ) == false )
                            {
                                return this->error_( "has an invalid collection tile image" );
                            }

                            if( tileImage.empty() == true )
                            {
                                continue;
                            }

                            TiledMapTileImage resultImage;
                            resultImage.localId = localId;
                            resultImage.resourceName = this->makeImageResourceName_( tile, tileImage );

                            if( this->makeExternalPath_( tilesetPath, tileImage, &resultImage.imagePath ) == false )
                            {
                                return this->error_( "references an invalid collection tile image path" );
                            }

                            if( this->readUInt32_( tile, "imagewidth", result.tileWidth, &resultImage.imageWidth ) == false
                                || this->readUInt32_( tile, "imageheight", result.tileHeight, &resultImage.imageHeight ) == false
                                || this->readUInt32_( tile, "x", 0, &resultImage.sourceX ) == false
                                || this->readUInt32_( tile, "y", 0, &resultImage.sourceY ) == false
                                || this->readUInt32_( tile, "width", result.tileWidth, &resultImage.sourceWidth ) == false
                                || this->readUInt32_( tile, "height", result.tileHeight, &resultImage.sourceHeight ) == false )
                            {
                                return this->error_( "has invalid collection tile image dimensions" );
                            }

                            result.tileImages.emplace_back( std::move( resultImage ) );
                        }
                    }

                    if( result.tileCount == 0 && result.columns != 0 && result.tileHeight != 0 && result.imageHeight != 0 )
                    {
                        int64_t numerator = (int64_t)result.imageHeight - (int64_t)result.margin * 2 + result.spacing;
                        int64_t denominator = (int64_t)result.tileHeight + result.spacing;
                        int64_t rows = numerator > 0 && denominator > 0 ? numerator / denominator : 0;
                        uint64_t inferredCount = (uint64_t)result.columns * (uint64_t)rows;

                        if( inferredCount > std::numeric_limits<uint32_t>::max() )
                        {
                            return this->error_( "has an overflowing inferred tileset tile count" );
                        }

                        result.tileCount = (uint32_t)inferredCount;
                    }

                    if( maximumLocalId >= 0 )
                    {
                        uint64_t collectionCount = (uint64_t)maximumLocalId + 1;

                        if( collectionCount > std::numeric_limits<uint32_t>::max() )
                        {
                            return this->error_( "has an overflowing collection tileset tile count" );
                        }

                        result.tileCount = StdAlgorithm::max( result.tileCount, (uint32_t)collectionCount );
                    }

                    if( result.tileCount == 0 && result.resourceName.empty() == true )
                    {
                        LOGGER_ERROR( "resource tiled map '%s' tileset '%s' has no tiles"
                            , m_mapPath.c_str()
                            , result.name.c_str()
                        );

                        return false;
                    }

                    m_data->tilesets.emplace_back( std::move( result ) );
                }

                StdAlgorithm::sort( m_data->tilesets.begin(), m_data->tilesets.end(), []( const TiledMapTileset & _left, const TiledMapTileset & _right )
                {
                    return _left.firstGid < _right.firstGid;
                } );

                return true;
            }

            bool decodeLayerData_( const jpp::object & _owner, size_t _expectedCount, Vector<uint32_t> * const _gids ) const
            {
                jpp::object jsonData;

                if( _owner.exist( "data", &jsonData ) == false )
                {
                    return _expectedCount == 0 ? true : this->error_( "has a tile layer without data" );
                }

                if( jsonData.is_type_array() == true )
                {
                    jpp::array values( jsonData );

                    if( values.size() != _expectedCount )
                    {
                        return this->error_( "has a tile layer with an unexpected gid count" );
                    }

                    _gids->reserve( _expectedCount );

                    for( const jpp::object & jsonGid : values )
                    {
                        if( jsonGid.is_type_integer() == false )
                        {
                            return this->error_( "has a non-integer tile gid" );
                        }

                        int64_t gid = jsonGid;

                        if( gid < 0 || gid > (int64_t)std::numeric_limits<uint32_t>::max() )
                        {
                            return this->error_( "has an out-of-range tile gid" );
                        }

                        _gids->emplace_back( (uint32_t)gid );
                    }

                    return true;
                }

                if( jsonData.is_type_string() == false )
                {
                    return this->error_( "has tile data that is neither an array nor base64" );
                }

                const Char * encodedValue = jsonData;
                String encoded;

                for( const Char * ch = encodedValue; *ch != '\0'; ++ch )
                {
                    if( *ch != ' ' && *ch != '\t' && *ch != '\r' && *ch != '\n' )
                    {
                        encoded += *ch;
                    }
                }

                if( _expectedCount == 0 && encoded.empty() == true )
                {
                    return true;
                }

                if( encoded.size() < 4 || encoded.size() % 4 != 0 )
                {
                    return this->error_( "has malformed base64 tile data" );
                }

                size_t encodedSize = encoded.size();
                size_t decodedSize = Helper::getBase64DecodeSize( encoded.c_str(), encodedSize );
                Data decoded( decodedSize );
                Helper::decodeBase64( encoded.c_str(), encodedSize, decoded.data() );

                if( _expectedCount > std::numeric_limits<size_t>::max() / 4 )
                {
                    return this->error_( "has an overflowing tile data size" );
                }

                size_t expectedSize = _expectedCount * 4;
                const Char * compression = _owner.get( "compression", "" );
                Data unpacked;
                const Data * bytes = &decoded;

                if( StdString::strcmp( compression, "zlib" ) == 0 || StdString::strcmp( compression, "gzip" ) == 0 )
                {
                    if( decoded.size() > std::numeric_limits<uInt>::max() || expectedSize > std::numeric_limits<uInt>::max() )
                    {
                        return this->error_( "has tile data too large for zlib" );
                    }

                    unpacked.resize( expectedSize );

                    z_stream stream = {};
                    stream.next_in = decoded.data();
                    stream.avail_in = (uInt)decoded.size();
                    stream.next_out = unpacked.data();
                    stream.avail_out = (uInt)unpacked.size();

                    int32_t initResult = ::inflateInit2( &stream, MAX_WBITS + 32 );

                    if( initResult != Z_OK )
                    {
                        return this->error_( "can't initialize zlib tile decompression" );
                    }

                    int32_t inflateResult = ::inflate( &stream, Z_FINISH );
                    size_t totalOut = stream.total_out;
                    ::inflateEnd( &stream );

                    if( inflateResult != Z_STREAM_END || totalOut != expectedSize )
                    {
                        return this->error_( "can't decompress zlib/gzip tile data" );
                    }

                    bytes = &unpacked;
                }
                else if( StdString::strcmp( compression, "zstd" ) == 0 )
                {
                    unpacked.resize( expectedSize );
                    size_t result = ::ZSTD_decompress( unpacked.data(), unpacked.size(), decoded.data(), decoded.size() );

                    if( ::ZSTD_isError( result ) != 0 || result != expectedSize )
                    {
                        return this->error_( "can't decompress zstd tile data" );
                    }

                    bytes = &unpacked;
                }
                else if( compression[0] != '\0' )
                {
                    LOGGER_ERROR( "resource tiled map '%s' has unsupported tile compression '%s'"
                        , m_mapPath.c_str()
                        , compression
                    );

                    return false;
                }

                if( bytes->size() != expectedSize )
                {
                    return this->error_( "has a tile layer with an unexpected decoded byte count" );
                }

                _gids->reserve( _expectedCount );

                for( size_t index = 0; index != _expectedCount; ++index )
                {
                    const uint8_t * value = bytes->data() + index * 4;
                    uint32_t gid = (uint32_t)value[0]
                        | (uint32_t)value[1] << 8
                        | (uint32_t)value[2] << 16
                        | (uint32_t)value[3] << 24;

                    _gids->emplace_back( gid );
                }

                return true;
            }

            bool getLayerOrigin_( const jpp::object & _layer, int32_t * const _x, int32_t * const _y ) const
            {
                jpp::object value;

                if( _layer.exist( "x", &value ) == true )
                {
                    if( this->readInt32_( _layer, "x", 0, _x ) == false )
                    {
                        return false;
                    }
                }
                else if( this->readInt32_( _layer, "startx", 0, _x ) == false )
                {
                    return false;
                }

                if( _layer.exist( "y", &value ) == true )
                {
                    if( this->readInt32_( _layer, "y", 0, _y ) == false )
                    {
                        return false;
                    }
                }
                else if( this->readInt32_( _layer, "starty", 0, _y ) == false )
                {
                    return false;
                }

                return true;
            }

            bool appendTiles_( const jpp::object & _owner, uint32_t _width, uint32_t _height, int32_t _startX, int32_t _startY, VectorTiledMapTiles * const _tiles ) const
            {
                uint64_t expectedCount64 = (uint64_t)_width * (uint64_t)_height;

                if( expectedCount64 > std::numeric_limits<size_t>::max() )
                {
                    return this->error_( "has an overflowing tile layer size" );
                }

                Vector<uint32_t> gids;

                if( this->decodeLayerData_( _owner, (size_t)expectedCount64, &gids ) == false )
                {
                    return false;
                }

                for( size_t index = 0; index != gids.size(); ++index )
                {
                    uint32_t gid = gids[index];

                    if( gid == 0 )
                    {
                        continue;
                    }

                    int64_t x = (int64_t)_startX + (int64_t)(index % _width);
                    int64_t y = (int64_t)_startY + (int64_t)(index / _width);

                    if( x < std::numeric_limits<int32_t>::min() || x > std::numeric_limits<int32_t>::max()
                        || y < std::numeric_limits<int32_t>::min() || y > std::numeric_limits<int32_t>::max() )
                    {
                        return this->error_( "has an out-of-range tile position" );
                    }

                    TiledMapTile tile;
                    tile.x = (int32_t)x;
                    tile.y = (int32_t)y;
                    tile.gid = gid;
                    _tiles->emplace_back( tile );
                }

                return true;
            }

            bool readTileLayer_( const jpp::object & _layer, const String & _name, bool _visible, float _opacity, float _offsetX, float _offsetY, const TiledMapProperties & _properties )
            {
                TiledMapTileLayer result;

                if( this->readUInt32_( _layer, "id", 0, &result.id ) == false )
                {
                    return this->error_( "has an invalid tile layer id" );
                }

                result.name = _name;
                result.visible = _visible;
                result.opacity = _opacity;
                result.offset = mt::vec2f( _offsetX, _offsetY );
                result.properties = _properties;

                int32_t layerX;
                int32_t layerY;

                if( this->getLayerOrigin_( _layer, &layerX, &layerY ) == false )
                {
                    return this->error_( "has an invalid tile layer origin" );
                }

                jpp::array chunks;
                bool chunksExist;

                if( this->readArray_( _layer, "chunks", &chunks, &chunksExist ) == false )
                {
                    return this->error_( "has an invalid chunks array" );
                }

                if( chunksExist == true )
                {
                    for( const jpp::object & chunk : chunks )
                    {
                        if( chunk.is_type_object() == false )
                        {
                            return this->error_( "has a non-object tile chunk" );
                        }

                        uint32_t width;
                        uint32_t height;
                        int32_t chunkX;
                        int32_t chunkY;

                        if( this->readUInt32_( chunk, "width", 0, &width ) == false
                            || this->readUInt32_( chunk, "height", 0, &height ) == false
                            || this->readInt32_( chunk, "x", 0, &chunkX ) == false
                            || this->readInt32_( chunk, "y", 0, &chunkY ) == false )
                        {
                            return this->error_( "has invalid tile chunk fields" );
                        }

                        int64_t startX = (int64_t)layerX + chunkX;
                        int64_t startY = (int64_t)layerY + chunkY;

                        if( startX < std::numeric_limits<int32_t>::min() || startX > std::numeric_limits<int32_t>::max()
                            || startY < std::numeric_limits<int32_t>::min() || startY > std::numeric_limits<int32_t>::max() )
                        {
                            return this->error_( "has an out-of-range tile chunk origin" );
                        }

                        if( this->appendTiles_( chunk, width, height, (int32_t)startX, (int32_t)startY, &result.tiles ) == false )
                        {
                            return false;
                        }
                    }
                }
                else
                {
                    uint32_t width;
                    uint32_t height;

                    if( this->readUInt32_( _layer, "width", m_data->width, &width ) == false
                        || this->readUInt32_( _layer, "height", m_data->height, &height ) == false )
                    {
                        return this->error_( "has invalid tile layer dimensions" );
                    }

                    if( this->appendTiles_( _layer, width, height, layerX, layerY, &result.tiles ) == false )
                    {
                        return false;
                    }
                }

                StdAlgorithm::stable_sort( result.tiles.begin(), result.tiles.end(), [this]( const TiledMapTile & _left, const TiledMapTile & _right )
                {
                    int64_t leftY = m_reverseY == true ? -(int64_t)_left.y : (int64_t)_left.y;
                    int64_t rightY = m_reverseY == true ? -(int64_t)_right.y : (int64_t)_right.y;

                    if( leftY != rightY )
                    {
                        return leftY < rightY;
                    }

                    int64_t leftX = m_reverseX == true ? -(int64_t)_left.x : (int64_t)_left.x;
                    int64_t rightX = m_reverseX == true ? -(int64_t)_right.x : (int64_t)_right.x;

                    return leftX < rightX;
                } );

                m_data->tileLayers.emplace_back( std::move( result ) );

                return true;
            }

            bool readObjectPoints_( const jpp::object & _object, const Char * _key, VectorTiledMapPoints * const _points ) const
            {
                jpp::array points;
                bool exist;

                if( this->readArray_( _object, _key, &points, &exist ) == false )
                {
                    return false;
                }

                if( exist == false )
                {
                    return true;
                }

                for( const jpp::object & point : points )
                {
                    if( point.is_type_object() == false )
                    {
                        return false;
                    }

                    mt::vec2f value;

                    if( this->readFloat_( point, "x", 0.f, &value.x ) == false
                        || this->readFloat_( point, "y", 0.f, &value.y ) == false )
                    {
                        return false;
                    }

                    _points->emplace_back( value );
                }

                return true;
            }

            bool readObjectLayer_( const jpp::object & _layer, const String & _name, bool _visible, float _opacity, float _offsetX, float _offsetY, const TiledMapProperties & _properties )
            {
                TiledMapLayer result;

                if( this->readUInt32_( _layer, "id", 0, &result.id ) == false )
                {
                    return this->error_( "has an invalid object layer id" );
                }

                result.name = _name;
                result.visible = _visible;
                result.opacity = _opacity;
                result.offset = mt::vec2f( _offsetX, _offsetY );
                result.properties = _properties;

                jpp::array objects;
                bool exist;

                if( this->readArray_( _layer, "objects", &objects, &exist ) == false )
                {
                    return this->error_( "has an invalid objects array" );
                }

                if( exist == true )
                {
                    for( const jpp::object & object : objects )
                    {
                        if( object.is_type_object() == false )
                        {
                            return this->error_( "has a non-object map object" );
                        }

                        TiledMapObject resultObject;

                        if( this->readUInt32_( object, "id", 0, &resultObject.id ) == false
                            || this->readString_( object, "name", "", &resultObject.name ) == false
                            || this->readString_( object, "type", "", &resultObject.type ) == false
                            || this->readString_( object, "class", resultObject.type.c_str(), &resultObject.className ) == false
                            || this->readBool_( object, "visible", true, &resultObject.visible ) == false
                            || this->readFloat_( object, "x", 0.f, &resultObject.position.x ) == false
                            || this->readFloat_( object, "y", 0.f, &resultObject.position.y ) == false
                            || this->readFloat_( object, "width", 0.f, &resultObject.size.x ) == false
                            || this->readFloat_( object, "height", 0.f, &resultObject.size.y ) == false
                            || this->readFloat_( object, "rotation", 0.f, &resultObject.rotation ) == false
                            || this->readUInt32_( object, "gid", 0, &resultObject.gid ) == false )
                        {
                            return this->error_( "has invalid object fields" );
                        }

                        resultObject.position.x += _offsetX;
                        resultObject.position.y += _offsetY;
                        resultObject.shape = ETMOS_RECTANGLE;

                        struct ShapeDesc
                        {
                            const Char * name;
                            ETiledMapObjectShape shape;
                        };

                        static const ShapeDesc shapes[] = {
                            {"ellipse", ETMOS_ELLIPSE},
                            {"point", ETMOS_POINT},
                            {"polygon", ETMOS_POLYGON},
                            {"polyline", ETMOS_POLYLINE},
                            {"text", ETMOS_TEXT}
                        };

                        for( const ShapeDesc & shape : shapes )
                        {
                            jpp::object shapeValue;

                            if( object.exist( shape.name, &shapeValue ) == true && shapeValue.is_false() == false )
                            {
                                resultObject.shape = shape.shape;

                                break;
                            }
                        }

                        jpp::object gidValue;

                        if( object.exist( "gid", &gidValue ) == true )
                        {
                            resultObject.shape = ETMOS_TILE;
                        }

                        if( resultObject.shape == ETMOS_POLYGON )
                        {
                            if( this->readObjectPoints_( object, "polygon", &resultObject.points ) == false )
                            {
                                return this->error_( "has invalid polygon points" );
                            }
                        }
                        else if( resultObject.shape == ETMOS_POLYLINE )
                        {
                            if( this->readObjectPoints_( object, "polyline", &resultObject.points ) == false )
                            {
                                return this->error_( "has invalid polyline points" );
                            }
                        }

                        if( this->readProperties_( object, &resultObject.properties ) == false )
                        {
                            return false;
                        }

                        result.objects.emplace_back( std::move( resultObject ) );
                    }
                }

                m_data->objectLayers.emplace_back( std::move( result ) );

                return true;
            }

            bool readLayers_( const jpp::object & _owner, const String & _prefix, bool _parentVisible, float _parentOpacity, float _parentOffsetX, float _parentOffsetY, const TiledMapProperties & _parentProperties )
            {
                jpp::array layers;
                bool exist;

                if( this->readArray_( _owner, "layers", &layers, &exist ) == false )
                {
                    return this->error_( "has an invalid layers array" );
                }

                if( exist == false )
                {
                    return true;
                }

                for( const jpp::object & layer : layers )
                {
                    if( layer.is_type_object() == false )
                    {
                        return this->error_( "has a non-object layer" );
                    }

                    String type;
                    String name;

                    if( this->readString_( layer, "type", "", &type ) == false
                        || this->readString_( layer, "name", "", &name ) == false )
                    {
                        return this->error_( "has an invalid layer type or name" );
                    }

                    String fullName;

                    if( _prefix.empty() == false && name.empty() == false )
                    {
                        fullName = _prefix + "/" + name;
                    }
                    else
                    {
                        fullName = _prefix.empty() == false ? _prefix : name;
                    }

                    bool localVisible;
                    float localOpacity;
                    float localOffsetX;
                    float localOffsetY;

                    if( this->readBool_( layer, "visible", true, &localVisible ) == false
                        || this->readFloat_( layer, "opacity", 1.f, &localOpacity ) == false
                        || this->readFloat_( layer, "offsetx", 0.f, &localOffsetX ) == false
                        || this->readFloat_( layer, "offsety", 0.f, &localOffsetY ) == false )
                    {
                        return this->error_( "has invalid layer visibility, opacity, or offset" );
                    }

                    bool visible = _parentVisible && localVisible;
                    float opacity = _parentOpacity * localOpacity;
                    float offsetX = _parentOffsetX + localOffsetX;
                    float offsetY = _parentOffsetY + localOffsetY;
                    TiledMapProperties properties;

                    if( this->readLayerProperties_( layer, _parentProperties, &properties ) == false )
                    {
                        return false;
                    }

                    if( type == "group" )
                    {
                        if( this->readLayers_( layer, fullName, visible, opacity, offsetX, offsetY, properties ) == false )
                        {
                            return false;
                        }
                    }
                    else if( type == "tilelayer" )
                    {
                        if( this->readTileLayer_( layer, fullName, visible, opacity, offsetX, offsetY, properties ) == false )
                        {
                            return false;
                        }
                    }
                    else if( type == "objectgroup" )
                    {
                        if( this->readObjectLayer_( layer, fullName, visible, opacity, offsetX, offsetY, properties ) == false )
                        {
                            return false;
                        }
                    }
                }

                return true;
            }

        private:
            FileGroupInterfacePtr m_fileGroup;
            FilePath m_mapPath;
            TiledMapData * m_data;
            bool m_reverseX;
            bool m_reverseY;
        };
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
        const ContentInterfacePtr & content = this->getContent();
        const FileGroupInterfacePtr & fileGroup = content->getFileGroup();
        const FilePath & filePath = content->getFilePath();

        if( fileGroup == nullptr )
        {
            LOGGER_ERROR( "resource tiled map '%s' has no file group"
                , filePath.c_str()
            );

            return false;
        }

        Detail::TiledMapData data;
        Detail::TiledMapReader reader( fileGroup, filePath, &data );

        if( reader.read() == false )
        {
            this->clearData_();

            return false;
        }

        m_tilesets = std::move( data.tilesets );
        m_tileLayers = std::move( data.tileLayers );
        m_objectLayers = std::move( data.objectLayers );
        m_mapProperties = std::move( data.mapProperties );
        m_orientation = data.orientation;
        m_width = data.width;
        m_height = data.height;
        m_tileWidth = data.tileWidth;
        m_tileHeight = data.tileHeight;
        m_hexSideLength = data.hexSideLength;
        m_staggerAxis = data.staggerAxis;
        m_staggerIndex = data.staggerIndex;

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
