#include "TiledMapReader.h"

#include "Interface/MemoryInterface.h"

#include "Kernel/Logger.h"
#include "Kernel/MemoryStreamHelper.h"

#include "Config/StdString.h"

#include "pugixml.hpp"

#include <cerrno>
#include <cstdlib>

namespace Mengine
{
    namespace Detail
    {
        namespace
        {
            //////////////////////////////////////////////////////////////////////////
            bool isAttributeName_( const Char * _name, const Char * const * _names, size_t _count )
            {
                for( size_t index = 0; index != _count; ++index )
                {
                    if( StdString::strcmp( _name, _names[index] ) == 0 )
                    {
                        return true;
                    }
                }

                return false;
            }
            //////////////////////////////////////////////////////////////////////////
            bool setNumericAttribute_( jpp::object * const _target, const Char * _name, const Char * _value )
            {
                if( StdString::strchr( _value, '.' ) != nullptr
                    || StdString::strchr( _value, 'e' ) != nullptr
                    || StdString::strchr( _value, 'E' ) != nullptr )
                {
                    Char * end;
                    errno = 0;
                    double number = std::strtod( _value, &end );

                    if( errno != 0 || end == _value || *end != '\0' )
                    {
                        return false;
                    }

                    _target->set( _name, jpp::make_real( number ) );

                    return true;
                }

                Char * end;
                errno = 0;
                long long number = std::strtoll( _value, &end, 10 );

                if( errno != 0 || end == _value || *end != '\0' )
                {
                    return false;
                }

                _target->set( _name, jpp::make_integer( number ) );

                return true;
            }
            //////////////////////////////////////////////////////////////////////////
            bool copyAttributes_( const pugi::xml_node & _node, jpp::object * const _target )
            {
                static const Char * const numericAttributes[] = {
                    "columns",
                    "firstgid",
                    "gid",
                    "height",
                    "hexsidelength",
                    "id",
                    "imageheight",
                    "imagewidth",
                    "margin",
                    "nextlayerid",
                    "nextobjectid",
                    "spacing",
                    "tilecount",
                    "tileheight",
                    "tileid",
                    "tilewidth",
                    "width",
                    "x",
                    "y"
                };

                static const Char * const realAttributes[] = {
                    "offsetx",
                    "offsety",
                    "opacity",
                    "parallaxoriginx",
                    "parallaxoriginy",
                    "parallaxx",
                    "parallaxy",
                    "rotation"
                };

                static const Char * const boolAttributes[] = {
                    "infinite",
                    "visible"
                };

                for( const pugi::xml_attribute & attribute : _node.attributes() )
                {
                    const Char * name = attribute.name();
                    const Char * value = attribute.value();

                    if( isAttributeName_( name, numericAttributes, MENGINE_ARRAY_SIZE( numericAttributes ) ) == true )
                    {
                        if( setNumericAttribute_( _target, name, value ) == false )
                        {
                            return false;
                        }
                    }
                    else if( isAttributeName_( name, realAttributes, MENGINE_ARRAY_SIZE( realAttributes ) ) == true )
                    {
                        Char * end;
                        errno = 0;
                        double number = std::strtod( value, &end );

                        if( errno != 0 || end == value || *end != '\0' )
                        {
                            return false;
                        }

                        _target->set( name, jpp::make_real( number ) );
                    }
                    else if( isAttributeName_( name, boolAttributes, MENGINE_ARRAY_SIZE( boolAttributes ) ) == true )
                    {
                        bool boolean = StdString::strcmp( value, "0" ) != 0 && StdString::strcmp( value, "false" ) != 0;
                        _target->set( name, jpp::make_boolean( boolean ) );
                    }
                    else
                    {
                        _target->set( name, jpp::make_string( value ) );
                    }
                }

                return true;
            }
            //////////////////////////////////////////////////////////////////////////
            bool convertProperties_( const pugi::xml_node & _owner, jpp::object * const _target )
            {
                pugi::xml_node propertiesNode = _owner.child( "properties" );

                if( propertiesNode.empty() == true )
                {
                    return true;
                }

                jpp::array properties = jpp::make_array();

                for( const pugi::xml_node & propertyNode : propertiesNode.children( "property" ) )
                {
                    jpp::object property = jpp::make_object();

                    if( copyAttributes_( propertyNode, &property ) == false )
                    {
                        return false;
                    }

                    const Char * type = propertyNode.attribute( "type" ).as_string( "string" );
                    const Char * value = propertyNode.attribute( "value" ).as_string( propertyNode.text().as_string( "" ) );

                    if( StdString::strcmp( type, "int" ) == 0 || StdString::strcmp( type, "object" ) == 0 )
                    {
                        if( setNumericAttribute_( &property, "value", value ) == false )
                        {
                            return false;
                        }
                    }
                    else if( StdString::strcmp( type, "float" ) == 0 )
                    {
                        Char * end;
                        errno = 0;
                        double number = std::strtod( value, &end );

                        if( errno != 0 || end == value || *end != '\0' )
                        {
                            return false;
                        }

                        property.set( "value", jpp::make_real( number ) );
                    }
                    else if( StdString::strcmp( type, "bool" ) == 0 )
                    {
                        bool boolean = StdString::strcmp( value, "0" ) != 0 && StdString::strcmp( value, "false" ) != 0;
                        property.set( "value", jpp::make_boolean( boolean ) );
                    }
                    else
                    {
                        property.set( "value", jpp::make_string( value ) );
                    }

                    properties.push_back( property );
                }

                if( properties.empty() == false )
                {
                    _target->set( "properties", properties );
                }

                return true;
            }
            //////////////////////////////////////////////////////////////////////////
            bool convertImage_( const pugi::xml_node & _imageNode, jpp::object * const _target )
            {
                _target->set( "image", jpp::make_string( _imageNode.attribute( "source" ).as_string( "" ) ) );

                pugi::xml_attribute width = _imageNode.attribute( "width" );

                if( width.empty() == false && setNumericAttribute_( _target, "imagewidth", width.value() ) == false )
                {
                    return false;
                }

                pugi::xml_attribute height = _imageNode.attribute( "height" );

                if( height.empty() == false && setNumericAttribute_( _target, "imageheight", height.value() ) == false )
                {
                    return false;
                }

                pugi::xml_attribute transparentColor = _imageNode.attribute( "trans" );

                if( transparentColor.empty() == false )
                {
                    String value = transparentColor.value();

                    if( value.empty() == false && value[0] != '#' )
                    {
                        value.insert( value.begin(), '#' );
                    }

                    _target->set( "transparentcolor", jpp::make_string( value.c_str() ) );
                }

                return true;
            }
            //////////////////////////////////////////////////////////////////////////
            bool convertTileset_( const pugi::xml_node & _tilesetNode, jpp::object * const _tileset )
            {
                if( copyAttributes_( _tilesetNode, _tileset ) == false )
                {
                    return false;
                }

                pugi::xml_node imageNode = _tilesetNode.child( "image" );

                if( imageNode.empty() == false && convertImage_( imageNode, _tileset ) == false )
                {
                    return false;
                }

                pugi::xml_node tileOffsetNode = _tilesetNode.child( "tileoffset" );

                if( tileOffsetNode.empty() == false )
                {
                    jpp::object tileOffset = jpp::make_object();

                    if( copyAttributes_( tileOffsetNode, &tileOffset ) == false )
                    {
                        return false;
                    }

                    _tileset->set( "tileoffset", tileOffset );
                }

                jpp::array tiles = jpp::make_array();

                for( const pugi::xml_node & tileNode : _tilesetNode.children( "tile" ) )
                {
                    jpp::object tile = jpp::make_object();

                    if( copyAttributes_( tileNode, &tile ) == false )
                    {
                        return false;
                    }

                    pugi::xml_node tileImageNode = tileNode.child( "image" );

                    if( tileImageNode.empty() == false && convertImage_( tileImageNode, &tile ) == false )
                    {
                        return false;
                    }

                    if( convertProperties_( tileNode, &tile ) == false )
                    {
                        return false;
                    }

                    tiles.push_back( tile );
                }

                if( tiles.empty() == false )
                {
                    _tileset->set( "tiles", tiles );
                }

                return convertProperties_( _tilesetNode, _tileset );
            }
            //////////////////////////////////////////////////////////////////////////
            bool parseCSV_( const Char * _value, jpp::array * const _values )
            {
                const Char * position = _value;

                for( ;; )
                {
                    while( *position == ' ' || *position == '\t' || *position == '\r' || *position == '\n' || *position == ',' )
                    {
                        ++position;
                    }

                    if( *position == '\0' )
                    {
                        return true;
                    }

                    Char * end;
                    errno = 0;
                    unsigned long long gid = std::strtoull( position, &end, 10 );

                    if( errno != 0 || end == position )
                    {
                        return false;
                    }

                    _values->push_back( jpp::make_integer( (int64_t)gid ) );
                    position = end;
                }
            }
            //////////////////////////////////////////////////////////////////////////
            bool convertData_( const pugi::xml_node & _dataNode, const Char * _inheritedEncoding, const Char * _inheritedCompression, jpp::object * const _target )
            {
                const Char * encoding = _dataNode.attribute( "encoding" ).as_string( _inheritedEncoding );
                const Char * compression = _dataNode.attribute( "compression" ).as_string( _inheritedCompression );

                if( StdString::strcmp( encoding, "csv" ) == 0 )
                {
                    jpp::array values = jpp::make_array();

                    if( parseCSV_( _dataNode.text().as_string( "" ), &values ) == false )
                    {
                        return false;
                    }

                    _target->set( "data", values );

                    return true;
                }

                if( StdString::strcmp( encoding, "base64" ) == 0 )
                {
                    _target->set( "data", jpp::make_string( _dataNode.text().as_string( "" ) ) );
                    _target->set( "encoding", jpp::make_string( encoding ) );

                    if( compression[0] != '\0' )
                    {
                        _target->set( "compression", jpp::make_string( compression ) );
                    }

                    return true;
                }

                if( encoding[0] != '\0' )
                {
                    return false;
                }

                jpp::array values = jpp::make_array();

                for( const pugi::xml_node & tileNode : _dataNode.children( "tile" ) )
                {
                    values.push_back( jpp::make_integer( tileNode.attribute( "gid" ).as_llong( 0 ) ) );
                }

                _target->set( "data", values );

                return true;
            }
            //////////////////////////////////////////////////////////////////////////
            bool convertPoints_( const Char * _value, jpp::array * const _points )
            {
                const Char * position = _value;

                while( *position != '\0' )
                {
                    while( *position == ' ' || *position == '\t' || *position == '\r' || *position == '\n' )
                    {
                        ++position;
                    }

                    if( *position == '\0' )
                    {
                        break;
                    }

                    Char * endX;
                    errno = 0;
                    double x = std::strtod( position, &endX );

                    if( errno != 0 || endX == position || *endX != ',' )
                    {
                        return false;
                    }

                    Char * endY;
                    errno = 0;
                    double y = std::strtod( endX + 1, &endY );

                    if( errno != 0 || endY == endX + 1 )
                    {
                        return false;
                    }

                    jpp::object point = jpp::make_object();
                    point.set( "x", jpp::make_real( x ) );
                    point.set( "y", jpp::make_real( y ) );
                    _points->push_back( point );
                    position = endY;
                }

                return true;
            }
            //////////////////////////////////////////////////////////////////////////
            bool convertObject_( const pugi::xml_node & _objectNode, jpp::object * const _object )
            {
                if( copyAttributes_( _objectNode, _object ) == false )
                {
                    return false;
                }

                if( _objectNode.child( "ellipse" ).empty() == false )
                {
                    _object->set( "ellipse", jpp::make_boolean( true ) );
                }

                if( _objectNode.child( "point" ).empty() == false )
                {
                    _object->set( "point", jpp::make_boolean( true ) );
                }

                static const Char * const pointShapes[] = {"polygon", "polyline"};

                for( const Char * shape : pointShapes )
                {
                    pugi::xml_node shapeNode = _objectNode.child( shape );

                    if( shapeNode.empty() == true )
                    {
                        continue;
                    }

                    jpp::array points = jpp::make_array();

                    if( convertPoints_( shapeNode.attribute( "points" ).as_string( "" ), &points ) == false )
                    {
                        return false;
                    }

                    _object->set( shape, points );
                }

                pugi::xml_node textNode = _objectNode.child( "text" );

                if( textNode.empty() == false )
                {
                    jpp::object text = jpp::make_object();

                    if( copyAttributes_( textNode, &text ) == false )
                    {
                        return false;
                    }

                    text.set( "text", jpp::make_string( textNode.text().as_string( "" ) ) );
                    _object->set( "text", text );
                }

                return convertProperties_( _objectNode, _object );
            }
            //////////////////////////////////////////////////////////////////////////
            bool convertLayers_( const pugi::xml_node & _owner, jpp::array * const _layers )
            {
                for( const pugi::xml_node & layerNode : _owner.children() )
                {
                    const Char * nodeName = layerNode.name();
                    jpp::object layer;

                    if( StdString::strcmp( nodeName, "layer" ) == 0 )
                    {
                        layer = jpp::make_object();

                        if( copyAttributes_( layerNode, &layer ) == false )
                        {
                            return false;
                        }

                        layer.set( "type", jpp::make_string( "tilelayer" ) );
                        pugi::xml_node dataNode = layerNode.child( "data" );

                        if( dataNode.empty() == true )
                        {
                            layer.set( "data", jpp::make_array() );
                        }
                        else
                        {
                            const Char * encoding = dataNode.attribute( "encoding" ).as_string( "" );
                            const Char * compression = dataNode.attribute( "compression" ).as_string( "" );
                            jpp::array chunks = jpp::make_array();

                            for( const pugi::xml_node & chunkNode : dataNode.children( "chunk" ) )
                            {
                                jpp::object chunk = jpp::make_object();

                                if( copyAttributes_( chunkNode, &chunk ) == false
                                    || convertData_( chunkNode, encoding, compression, &chunk ) == false )
                                {
                                    return false;
                                }

                                chunks.push_back( chunk );
                            }

                            if( chunks.empty() == false )
                            {
                                layer.set( "chunks", chunks );
                            }
                            else if( convertData_( dataNode, "", "", &layer ) == false )
                            {
                                return false;
                            }
                        }

                        if( convertProperties_( layerNode, &layer ) == false )
                        {
                            return false;
                        }
                    }
                    else if( StdString::strcmp( nodeName, "objectgroup" ) == 0 )
                    {
                        layer = jpp::make_object();

                        if( copyAttributes_( layerNode, &layer ) == false )
                        {
                            return false;
                        }

                        layer.set( "type", jpp::make_string( "objectgroup" ) );
                        jpp::array objects = jpp::make_array();

                        for( const pugi::xml_node & objectNode : layerNode.children( "object" ) )
                        {
                            jpp::object object = jpp::make_object();

                            if( convertObject_( objectNode, &object ) == false )
                            {
                                return false;
                            }

                            objects.push_back( object );
                        }

                        layer.set( "objects", objects );

                        if( convertProperties_( layerNode, &layer ) == false )
                        {
                            return false;
                        }
                    }
                    else if( StdString::strcmp( nodeName, "group" ) == 0 )
                    {
                        layer = jpp::make_object();

                        if( copyAttributes_( layerNode, &layer ) == false )
                        {
                            return false;
                        }

                        layer.set( "type", jpp::make_string( "group" ) );
                        jpp::array children = jpp::make_array();

                        if( convertLayers_( layerNode, &children ) == false )
                        {
                            return false;
                        }

                        layer.set( "layers", children );

                        if( convertProperties_( layerNode, &layer ) == false )
                        {
                            return false;
                        }
                    }
                    else
                    {
                        continue;
                    }

                    _layers->push_back( layer );
                }

                return true;
            }
            //////////////////////////////////////////////////////////////////////////
            bool convertMap_( const pugi::xml_node & _mapNode, jpp::object * const _map )
            {
                if( copyAttributes_( _mapNode, _map ) == false )
                {
                    return false;
                }

                _map->set( "type", jpp::make_string( "map" ) );
                jpp::array tilesets = jpp::make_array();

                for( const pugi::xml_node & tilesetNode : _mapNode.children( "tileset" ) )
                {
                    jpp::object tileset = jpp::make_object();
                    pugi::xml_attribute firstGid = tilesetNode.attribute( "firstgid" );

                    if( firstGid.empty() == false && setNumericAttribute_( &tileset, "firstgid", firstGid.value() ) == false )
                    {
                        return false;
                    }

                    pugi::xml_attribute source = tilesetNode.attribute( "source" );

                    if( source.empty() == false )
                    {
                        tileset.set( "source", jpp::make_string( source.value() ) );
                    }
                    else if( convertTileset_( tilesetNode, &tileset ) == false )
                    {
                        return false;
                    }

                    tilesets.push_back( tileset );
                }

                _map->set( "tilesets", tilesets );
                jpp::array layers = jpp::make_array();

                if( convertLayers_( _mapNode, &layers ) == false )
                {
                    return false;
                }

                _map->set( "layers", layers );

                return convertProperties_( _mapNode, _map );
            }
            //////////////////////////////////////////////////////////////////////////
        }
        //////////////////////////////////////////////////////////////////////////
        bool loadTiledMapXMLDocument( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, ETiledMapDocumentType _documentType, jpp::object * const _document )
        {
            if( _fileGroup->existFile( _filePath, true ) == false )
            {
                LOGGER_ERROR( "can't find Tiled XML '%s'"
                    , _filePath.c_str()
                );

                return false;
            }

            MemoryInterfacePtr memory = Helper::createMemoryFileString( _fileGroup, _filePath, false, false, MENGINE_DOCUMENT_FUNCTION );

            if( memory == nullptr || memory->getSize() == 0 )
            {
                LOGGER_ERROR( "can't read Tiled XML '%s'"
                    , _filePath.c_str()
                );

                return false;
            }

            pugi::xml_document xmlDocument;
            pugi::xml_parse_result parseResult = xmlDocument.load_buffer( memory->getBuffer(), memory->getSize() - 1 );

            if( parseResult == false )
            {
                LOGGER_ERROR( "can't parse Tiled XML '%s' at offset %zu: %s"
                    , _filePath.c_str()
                    , parseResult.offset
                    , parseResult.description()
                );

                return false;
            }

            const Char * rootName = _documentType == ETMDT_MAP ? "map" : "tileset";
            pugi::xml_node root = xmlDocument.child( rootName );

            if( root.empty() == true )
            {
                LOGGER_ERROR( "Tiled XML '%s' has no '%s' root"
                    , _filePath.c_str()
                    , rootName
                );

                return false;
            }

            jpp::object document = jpp::make_object();
            bool successful;

            if( _documentType == ETMDT_MAP )
            {
                successful = convertMap_( root, &document );
            }
            else
            {
                successful = convertTileset_( root, &document );
                document.set( "type", jpp::make_string( "tileset" ) );
            }

            if( successful == false )
            {
                LOGGER_ERROR( "invalid Tiled XML fields in '%s'"
                    , _filePath.c_str()
                );

                return false;
            }

            *_document = std::move( document );

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
    }
}
