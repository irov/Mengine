#include "JSONDataHelper.h"

#include "Kernel/ConstStringHelper.h"
#include "Kernel/FilePathHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/ColorHelper.h"

#include "Config/Typeinfo.h"

namespace Mengine
{
    namespace Helper
    {
        namespace Detail
        {
            //////////////////////////////////////////////////////////////////////////
            template<class T>
            static bool getJSONValueT( const jpp::object & _json, const Char * _key, T * const _value )
            {
                jpp::object j_value;
                if( _json.exist( _key, &j_value ) == false )
                {
                    return false;
                }

                if( jpp::check_object_internal()(j_value.ptr(), static_cast<T *>(nullptr)) == false )
                {
                    LOGGER_ERROR( "json key '%s' invalid type '%s' cast '%s'"
                        , _key
                        , jpp::get_jpp_type_string( j_value )
                        , MENGINE_TYPEINFO_NAME( T )
                    );

                    return false;
                }

                jpp::cast_object_internal()(j_value.ptr(), _value);

                return true;
            }
            //////////////////////////////////////////////////////////////////////////
            static bool getJSONMemberValue( const jpp::object & _json, const Char * _node, const Char * _key, jpp::object * const _value )
            {
                jpp::object j_node;
                if( _json.exist( _node, &j_node ) == false )
                {
                    return false;
                }

                if( j_node.exist( _key, _value ) == false )
                {
                    return false;
                }

                return true;
            }
            //////////////////////////////////////////////////////////////////////////
            template<class T>
            static bool getJSONMemberValueT( const jpp::object & _json, const Char * _node, const Char * _key, T * const _value )
            {
                jpp::object j_value;
                if( Detail::getJSONMemberValue( _json, _node, _key, &j_value ) == false )
                {
                    return false;
                }

                if( jpp::check_object_internal()(j_value.ptr(), static_cast<T *>(nullptr)) == false )
                {
                    LOGGER_ERROR( "json node '%s' key '%s' invalid type '%s' cast '%s'"
                        , _node
                        , _key
                        , jpp::get_jpp_type_string( j_value )
                        , MENGINE_TYPEINFO_NAME( T )
                    );

                    return false;
                }

                jpp::cast_object_internal()(j_value.ptr(), _value);

                return true;
            }
            //////////////////////////////////////////////////////////////////////////
            static bool isArraySize( const jpp::object & _json, jpp::array * const _array, jpp::jpp_size_t _size )
            {
                if( _json.is_type_array() == false )
                {
                    return false;
                }

                *_array = jpp::array( _json );

                return _array->size() >= _size;
            }
            //////////////////////////////////////////////////////////////////////////
            static bool getJSONVec2fValue( const jpp::object & _json, mt::vec2f * const _value )
            {
                jpp::array j_array;
                if( Detail::isArraySize( _json, &j_array, 2 ) == true )
                {
                    _value->x = j_array[0];
                    _value->y = j_array[1];

                    return true;
                }

                if( _json.is_type_object() == true )
                {
                    _value->x = _json.get( "x", 0.f );
                    _value->y = _json.get( "y", 0.f );

                    return true;
                }

                return false;
            }
            //////////////////////////////////////////////////////////////////////////
            static bool getJSONVec3fValue( const jpp::object & _json, mt::vec3f * const _value )
            {
                jpp::array j_array;
                if( Detail::isArraySize( _json, &j_array, 3 ) == true )
                {
                    _value->x = j_array[0];
                    _value->y = j_array[1];
                    _value->z = j_array[2];

                    return true;
                }

                if( _json.is_type_object() == true )
                {
                    _value->x = _json.get( "x", 0.f );
                    _value->y = _json.get( "y", 0.f );
                    _value->z = _json.get( "z", 0.f );

                    return true;
                }

                return false;
            }
            //////////////////////////////////////////////////////////////////////////
            static bool getJSONVec4fValue( const jpp::object & _json, mt::vec4f * const _value )
            {
                jpp::array j_array;
                if( Detail::isArraySize( _json, &j_array, 4 ) == true )
                {
                    _value->x = j_array[0];
                    _value->y = j_array[1];
                    _value->z = j_array[2];
                    _value->w = j_array[3];

                    return true;
                }

                if( _json.is_type_object() == true )
                {
                    _value->x = _json.get( "x", 0.f );
                    _value->y = _json.get( "y", 0.f );
                    _value->z = _json.get( "z", 0.f );
                    _value->w = _json.get( "w", 0.f );

                    return true;
                }

                return false;
            }
            //////////////////////////////////////////////////////////////////////////
            static bool getJSONUV4fValue( const jpp::object & _json, mt::uv4f * const _value )
            {
                jpp::array j_array;
                if( Detail::isArraySize( _json, &j_array, 8 ) == true )
                {
                    for( uint32_t index = 0; index != 4; ++index )
                    {
                        (*_value)[index].x = j_array[index * 2 + 0];
                        (*_value)[index].y = j_array[index * 2 + 1];
                    }

                    return true;
                }

                if( Detail::isArraySize( _json, &j_array, 4 ) == true )
                {
                    for( uint32_t index = 0; index != 4; ++index )
                    {
                        if( Detail::getJSONVec2fValue( j_array[index], &(*_value)[index] ) == false )
                        {
                            return false;
                        }
                    }

                    return true;
                }

                return false;
            }
            //////////////////////////////////////////////////////////////////////////
            static bool getJSONBox2fValue( const jpp::object & _json, mt::box2f * const _value )
            {
                jpp::array j_array;
                if( Detail::isArraySize( _json, &j_array, 4 ) == true )
                {
                    _value->minimum.x = j_array[0];
                    _value->minimum.y = j_array[1];
                    _value->maximum.x = j_array[2];
                    _value->maximum.y = j_array[3];

                    return true;
                }

                if( _json.is_type_object() == true )
                {
                    jpp::object j_minimum;
                    jpp::object j_maximum;

                    bool hasMinimum = _json.exist( "minimum", &j_minimum );
                    bool hasMaximum = _json.exist( "maximum", &j_maximum );

                    if( hasMinimum == true || hasMaximum == true )
                    {
                        if( hasMinimum == false || hasMaximum == false )
                        {
                            return false;
                        }

                        return Detail::getJSONVec2fValue( j_minimum, &_value->minimum ) && Detail::getJSONVec2fValue( j_maximum, &_value->maximum );
                    }

                    _value->minimum.x = _json.get( "left", 0.f );
                    _value->minimum.y = _json.get( "top", 0.f );
                    _value->maximum.x = _json.get( "right", 0.f );
                    _value->maximum.y = _json.get( "bottom", 0.f );

                    return true;
                }

                return false;
            }
            //////////////////////////////////////////////////////////////////////////
            static bool getJSONMat4fValue( const jpp::object & _json, mt::mat4f * const _value )
            {
                jpp::array j_array;
                if( Detail::isArraySize( _json, &j_array, 16 ) == false )
                {
                    return false;
                }

                float * matrix = _value->buff();

                for( uint32_t index = 0; index != 16; ++index )
                {
                    matrix[index] = j_array[index];
                }

                return true;
            }
            //////////////////////////////////////////////////////////////////////////
            static bool getJSONColorValue( const jpp::object & _json, Color * const _value )
            {
                jpp::array j_array;
                if( Detail::isArraySize( _json, &j_array, 4 ) == true )
                {
                    if( j_array[0].is_type_integer() == true )
                    {
                        uint8_t r = j_array[0];
                        uint8_t g = j_array[1];
                        uint8_t b = j_array[2];
                        uint8_t a = j_array[3];

                        *_value = Helper::makeColor8( r, g, b, a );
                    }
                    else
                    {
                    float r = j_array[0];
                    float g = j_array[1];
                    float b = j_array[2];
                    float a = j_array[3];

                    *_value = Helper::makeColorF( r, g, b, a );
                    }

                    return true;
                }

                if( _json.is_type_object() == true )
                {
                    float r = _json.get( "r", 1.f );
                    float g = _json.get( "g", 1.f );
                    float b = _json.get( "b", 1.f );
                    float a = _json.get( "a", 1.f );

                    *_value = Helper::makeColorF( r, g, b, a );

                    return true;
                }

                return false;
            }
            //////////////////////////////////////////////////////////////////////////
            static bool getJSONPolygonValue( const jpp::object & _json, Polygon * const _value )
            {
                if( _json.is_type_array() == false )
                {
                    return false;
                }

                jpp::array j_array( _json );

                _value->clear();
                _value->reserve( j_array.size() );

                for( const jpp::object & j_point : j_array )
                {
                    mt::vec2f point;
                    if( Detail::getJSONVec2fValue( j_point, &point ) == false )
                    {
                        return false;
                    }

                    _value->append( point );
                }

                return true;
            }
            //////////////////////////////////////////////////////////////////////////
            template<class T>
            static bool getJSONTypedField( const jpp::object & _json, const Char * _key, T * const _value, bool (* _getter)(const jpp::object &, T * const) )
            {
                jpp::object j_value;
                if( _json.exist( _key, &j_value ) == false )
                {
                    return false;
                }

                return _getter( j_value, _value );
            }
            //////////////////////////////////////////////////////////////////////////
            template<class T>
            static bool getJSONTypedMemberField( const jpp::object & _json, const Char * _node, const Char * _key, T * const _value, bool (* _getter)(const jpp::object &, T * const) )
            {
                jpp::object j_value;
                if( Detail::getJSONMemberValue( _json, _node, _key, &j_value ) == false )
                {
                    return false;
                }

                return _getter( j_value, _value );
            }
            //////////////////////////////////////////////////////////////////////////
            template<class T>
            static bool getJSONEnumValue( const jpp::object & _json, const Char * _key, T * const _value, bool (* _parser)(const ConstString &, T * const) )
            {
                ConstString value;
                if( Helper::getJSONConstString( _json, _key, &value ) == false )
                {
                    return false;
                }

                if( _parser( value, _value ) == false )
                {
                    LOGGER_ERROR( "json key '%s' invalid enum '%s'"
                        , _key
                        , value.c_str()
                    );

                    return false;
                }

                return true;
            }
            //////////////////////////////////////////////////////////////////////////
            static bool stringEqual( const ConstString & _value, const Char * _test )
            {
                return _value == Helper::stringizeString( _test );
            }
            //////////////////////////////////////////////////////////////////////////
            static bool parseVertexAttributeType( const ConstString & _value, EVertexAttributeType * const _type )
            {
                if( Detail::stringEqual( _value, "FLOAT" ) == true || Detail::stringEqual( _value, "VAT_FLOAT" ) == true )
                {
                    *_type = VAT_FLOAT;
                    return true;
                }

                if( Detail::stringEqual( _value, "UNSIGNED_BYTE" ) == true || Detail::stringEqual( _value, "VAT_UNSIGNED_BYTE" ) == true )
                {
                    *_type = VAT_UNSIGNED_BYTE;
                    return true;
                }

                return false;
            }
            //////////////////////////////////////////////////////////////////////////
            static bool parseBlendFactor( const ConstString & _value, EBlendFactor * const _factor )
            {
#define MENGINE_JSON_BLEND_FACTOR(Name, Value)\
                if( Detail::stringEqual( _value, Name ) == true || Detail::stringEqual( _value, "BF_" Name ) == true ) { *_factor = Value; return true; }

                MENGINE_JSON_BLEND_FACTOR( "ONE", BF_ONE );
                MENGINE_JSON_BLEND_FACTOR( "ZERO", BF_ZERO );
                MENGINE_JSON_BLEND_FACTOR( "DEST_COLOUR", BF_DEST_COLOUR );
                MENGINE_JSON_BLEND_FACTOR( "SOURCE_COLOUR", BF_SOURCE_COLOUR );
                MENGINE_JSON_BLEND_FACTOR( "ONE_MINUS_DEST_COLOUR", BF_ONE_MINUS_DEST_COLOUR );
                MENGINE_JSON_BLEND_FACTOR( "ONE_MINUS_SOURCE_COLOUR", BF_ONE_MINUS_SOURCE_COLOUR );
                MENGINE_JSON_BLEND_FACTOR( "DEST_ALPHA", BF_DEST_ALPHA );
                MENGINE_JSON_BLEND_FACTOR( "SOURCE_ALPHA", BF_SOURCE_ALPHA );
                MENGINE_JSON_BLEND_FACTOR( "ONE_MINUS_DEST_ALPHA", BF_ONE_MINUS_DEST_ALPHA );
                MENGINE_JSON_BLEND_FACTOR( "ONE_MINUS_SOURCE_ALPHA", BF_ONE_MINUS_SOURCE_ALPHA );

#undef MENGINE_JSON_BLEND_FACTOR

                return false;
            }
            //////////////////////////////////////////////////////////////////////////
            static bool parseBlendOp( const ConstString & _value, EBlendOp * const _op )
            {
#define MENGINE_JSON_BLEND_OP(Name, Value)\
                if( Detail::stringEqual( _value, Name ) == true || Detail::stringEqual( _value, "BOP_" Name ) == true ) { *_op = Value; return true; }

                MENGINE_JSON_BLEND_OP( "ADD", BOP_ADD );
                MENGINE_JSON_BLEND_OP( "SUBTRACT", BOP_SUBTRACT );
                MENGINE_JSON_BLEND_OP( "REVSUBTRACT", BOP_REVSUBTRACT );
                MENGINE_JSON_BLEND_OP( "MIN", BOP_MIN );
                MENGINE_JSON_BLEND_OP( "MAX", BOP_MAX );

#undef MENGINE_JSON_BLEND_OP

                return false;
            }
            //////////////////////////////////////////////////////////////////////////
            static bool parseTextureAddressMode( const ConstString & _value, ETextureAddressMode * const _mode )
            {
#define MENGINE_JSON_TEXTURE_ADDRESS_MODE(Name, Value)\
                if( Detail::stringEqual( _value, Name ) == true || Detail::stringEqual( _value, "TAM_" Name ) == true ) { *_mode = Value; return true; }

                MENGINE_JSON_TEXTURE_ADDRESS_MODE( "WRAP", TAM_WRAP );
                MENGINE_JSON_TEXTURE_ADDRESS_MODE( "MIRROR", TAM_MIRROR );
                MENGINE_JSON_TEXTURE_ADDRESS_MODE( "CLAMP", TAM_CLAMP );
                MENGINE_JSON_TEXTURE_ADDRESS_MODE( "BORDER", TAM_BORDER );
                MENGINE_JSON_TEXTURE_ADDRESS_MODE( "MIRRORONCE", TAM_MIRRORONCE );

#undef MENGINE_JSON_TEXTURE_ADDRESS_MODE

                return false;
            }
            //////////////////////////////////////////////////////////////////////////
        }
        //////////////////////////////////////////////////////////////////////////
        bool hasJSONField( const jpp::object & _json, const Char * _key )
        {
            jpp::object j_value;

            return _json.exist( _key, &j_value );
        }
        //////////////////////////////////////////////////////////////////////////
        bool getJSONField( const jpp::object & _json, const Char * _key, jpp::object * const _value )
        {
            return _json.exist( _key, _value );
        }
        //////////////////////////////////////////////////////////////////////////
        bool getJSONMemberField( const jpp::object & _json, const Char * _node, const Char * _key, jpp::object * const _value )
        {
            return Detail::getJSONMemberValue( _json, _node, _key, _value );
        }
        //////////////////////////////////////////////////////////////////////////
        bool getJSONConstString( const jpp::object & _json, const Char * _key, ConstString * const _value )
        {
            return Detail::getJSONValueT( _json, _key, _value );
        }
        //////////////////////////////////////////////////////////////////////////
        bool getJSONFilePath( const jpp::object & _json, const Char * _key, FilePath * const _value )
        {
            return Detail::getJSONValueT( _json, _key, _value );
        }
        //////////////////////////////////////////////////////////////////////////
        bool getJSONBool( const jpp::object & _json, const Char * _key, bool * const _value )
        {
            return Detail::getJSONValueT( _json, _key, _value );
        }
        //////////////////////////////////////////////////////////////////////////
        bool getJSONFloat( const jpp::object & _json, const Char * _key, float * const _value )
        {
            return Detail::getJSONValueT( _json, _key, _value );
        }
        //////////////////////////////////////////////////////////////////////////
        bool getJSONUInt32( const jpp::object & _json, const Char * _key, uint32_t * const _value )
        {
            return Detail::getJSONValueT( _json, _key, _value );
        }
        //////////////////////////////////////////////////////////////////////////
        bool getJSONMemberConstString( const jpp::object & _json, const Char * _node, const Char * _key, ConstString * const _value )
        {
            return Detail::getJSONMemberValueT( _json, _node, _key, _value );
        }
        //////////////////////////////////////////////////////////////////////////
        bool getJSONMemberFilePath( const jpp::object & _json, const Char * _node, const Char * _key, FilePath * const _value )
        {
            return Detail::getJSONMemberValueT( _json, _node, _key, _value );
        }
        //////////////////////////////////////////////////////////////////////////
        bool getJSONMemberBool( const jpp::object & _json, const Char * _node, const Char * _key, bool * const _value )
        {
            return Detail::getJSONMemberValueT( _json, _node, _key, _value );
        }
        //////////////////////////////////////////////////////////////////////////
        bool getJSONMemberFloat( const jpp::object & _json, const Char * _node, const Char * _key, float * const _value )
        {
            return Detail::getJSONMemberValueT( _json, _node, _key, _value );
        }
        //////////////////////////////////////////////////////////////////////////
        bool getJSONMemberUInt32( const jpp::object & _json, const Char * _node, const Char * _key, uint32_t * const _value )
        {
            return Detail::getJSONMemberValueT( _json, _node, _key, _value );
        }
        //////////////////////////////////////////////////////////////////////////
        bool getJSONVec2f( const jpp::object & _json, const Char * _key, mt::vec2f * const _value )
        {
            return Detail::getJSONTypedField( _json, _key, _value, &Detail::getJSONVec2fValue );
        }
        //////////////////////////////////////////////////////////////////////////
        bool getJSONVec3f( const jpp::object & _json, const Char * _key, mt::vec3f * const _value )
        {
            return Detail::getJSONTypedField( _json, _key, _value, &Detail::getJSONVec3fValue );
        }
        //////////////////////////////////////////////////////////////////////////
        bool getJSONVec4f( const jpp::object & _json, const Char * _key, mt::vec4f * const _value )
        {
            return Detail::getJSONTypedField( _json, _key, _value, &Detail::getJSONVec4fValue );
        }
        //////////////////////////////////////////////////////////////////////////
        bool getJSONUV4f( const jpp::object & _json, const Char * _key, mt::uv4f * const _value )
        {
            return Detail::getJSONTypedField( _json, _key, _value, &Detail::getJSONUV4fValue );
        }
        //////////////////////////////////////////////////////////////////////////
        bool getJSONBox2f( const jpp::object & _json, const Char * _key, mt::box2f * const _value )
        {
            return Detail::getJSONTypedField( _json, _key, _value, &Detail::getJSONBox2fValue );
        }
        //////////////////////////////////////////////////////////////////////////
        bool getJSONMat4f( const jpp::object & _json, const Char * _key, mt::mat4f * const _value )
        {
            return Detail::getJSONTypedField( _json, _key, _value, &Detail::getJSONMat4fValue );
        }
        //////////////////////////////////////////////////////////////////////////
        bool getJSONColor( const jpp::object & _json, const Char * _key, Color * const _value )
        {
            return Detail::getJSONTypedField( _json, _key, _value, &Detail::getJSONColorValue );
        }
        //////////////////////////////////////////////////////////////////////////
        bool getJSONPolygon( const jpp::object & _json, const Char * _key, Polygon * const _value )
        {
            return Detail::getJSONTypedField( _json, _key, _value, &Detail::getJSONPolygonValue );
        }
        //////////////////////////////////////////////////////////////////////////
        bool getJSONUInt32s( const jpp::object & _json, const Char * _key, UInt32s * const _value )
        {
            jpp::object j_value;
            if( _json.exist( _key, &j_value ) == false )
            {
                return false;
            }

            if( j_value.is_type_array() == false )
            {
                return false;
            }

            _value->clear();

            for( const jpp::object & j_element : jpp::array( j_value ) )
            {
                uint32_t value = j_element;
                _value->emplace_back( value );
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        bool getJSONMemberVec2f( const jpp::object & _json, const Char * _node, const Char * _key, mt::vec2f * const _value )
        {
            return Detail::getJSONTypedMemberField( _json, _node, _key, _value, &Detail::getJSONVec2fValue );
        }
        //////////////////////////////////////////////////////////////////////////
        bool getJSONMemberUV4f( const jpp::object & _json, const Char * _node, const Char * _key, mt::uv4f * const _value )
        {
            return Detail::getJSONTypedMemberField( _json, _node, _key, _value, &Detail::getJSONUV4fValue );
        }
        //////////////////////////////////////////////////////////////////////////
        bool getJSONMemberPolygon( const jpp::object & _json, const Char * _node, const Char * _key, Polygon * const _value )
        {
            return Detail::getJSONTypedMemberField( _json, _node, _key, _value, &Detail::getJSONPolygonValue );
        }
        //////////////////////////////////////////////////////////////////////////
        bool getJSONMemberColor( const jpp::object & _json, const Char * _node, const Char * _key, Color * const _value )
        {
            return Detail::getJSONTypedMemberField( _json, _node, _key, _value, &Detail::getJSONColorValue );
        }
        //////////////////////////////////////////////////////////////////////////
        bool getJSONVertexAttributeType( const jpp::object & _json, const Char * _key, EVertexAttributeType * const _value )
        {
            return Detail::getJSONEnumValue( _json, _key, _value, &Detail::parseVertexAttributeType );
        }
        //////////////////////////////////////////////////////////////////////////
        bool getJSONBlendFactor( const jpp::object & _json, const Char * _key, EBlendFactor * const _value )
        {
            return Detail::getJSONEnumValue( _json, _key, _value, &Detail::parseBlendFactor );
        }
        //////////////////////////////////////////////////////////////////////////
        bool getJSONBlendOp( const jpp::object & _json, const Char * _key, EBlendOp * const _value )
        {
            return Detail::getJSONEnumValue( _json, _key, _value, &Detail::parseBlendOp );
        }
        //////////////////////////////////////////////////////////////////////////
        bool getJSONTextureAddressMode( const jpp::object & _json, const Char * _key, ETextureAddressMode * const _value )
        {
            return Detail::getJSONEnumValue( _json, _key, _value, &Detail::parseTextureAddressMode );
        }
        //////////////////////////////////////////////////////////////////////////
        bool getJSONMemberBlendFactor( const jpp::object & _json, const Char * _node, const Char * _key, EBlendFactor * const _value )
        {
            jpp::object j_value;
            if( Detail::getJSONMemberValue( _json, _node, _key, &j_value ) == false )
            {
                return false;
            }

            ConstString value = j_value;

            return Detail::parseBlendFactor( value, _value );
        }
        //////////////////////////////////////////////////////////////////////////
        bool getJSONMemberBlendOp( const jpp::object & _json, const Char * _node, const Char * _key, EBlendOp * const _value )
        {
            jpp::object j_value;
            if( Detail::getJSONMemberValue( _json, _node, _key, &j_value ) == false )
            {
                return false;
            }

            ConstString value = j_value;

            return Detail::parseBlendOp( value, _value );
        }
        //////////////////////////////////////////////////////////////////////////
        bool getJSONMemberTextureAddressMode( const jpp::object & _json, const Char * _node, const Char * _key, ETextureAddressMode * const _value )
        {
            jpp::object j_value;
            if( Detail::getJSONMemberValue( _json, _node, _key, &j_value ) == false )
            {
                return false;
            }

            ConstString value = j_value;

            return Detail::parseTextureAddressMode( value, _value );
        }
        //////////////////////////////////////////////////////////////////////////
    }
}
