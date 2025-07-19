#pragma once

#include "Kernel/Color.h"
#include "Kernel/ConstString.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/FilePath.h"
#include "Kernel/Vector.h"
#include "Kernel/String.h"
#include "Kernel/Data.h"
#include "Kernel/ColorHelper.h"
#include "Kernel/Stringalized.h"

#include "Config/Lambda.h"
#include "Config/Char.h"
#include "Config/StdString.h"

#include "math/vec2.h"
#include "math/vec3.h"
#include "math/uv4.h"

#define PUGIXML_NO_STL
#define PUGIXML_HEADER_ONLY
#include "pugixml.hpp"

#define PACKET_MAGIC 0xCAFEB00B

namespace Mengine
{
    struct PacketHeader
    {
        uint32_t magic;
        uint32_t compressedSize;
        uint32_t uncompressedSize;  // 0 if packed is not compressed
    };

    namespace Detail
    {
        MENGINE_INLINE void InsertPacketHeader( Data & _payload, const PacketHeader & _hdr )
        {
            const uint8_t * begin = reinterpret_cast<const uint8_t *>(&_hdr);
            const uint8_t * end = begin + sizeof( PacketHeader );

            _payload.insert( _payload.begin(), begin, end );
        }

        template<typename T>
        void setXmlValue( pugi::xml_attribute & _attrib, const T & _value )
        {
            _attrib.set_value( _value );
        }

        template<>
        MENGINE_INLINE void setXmlValue<mt::vec2f>( pugi::xml_attribute & _attrib, const mt::vec2f & _value )
        {
            Char xml_value[64 + 1] = {'\0'};
            Helper::stringalized( _value, xml_value, 64 );

            _attrib.set_value( xml_value );
        }

        template<>
        MENGINE_INLINE void setXmlValue<mt::vec3f>( pugi::xml_attribute & _attrib, const mt::vec3f & _value )
        {
            Char xml_value[128 + 1] = {'\0'};
            Helper::stringalized( _value, xml_value, 128 );

            _attrib.set_value( xml_value );
        }

        template<>
        MENGINE_INLINE void setXmlValue<mt::uv4f>( pugi::xml_attribute & _attrib, const mt::uv4f & _value )
        {
            Char xml_value[512 + 1] = {'\0'};
            Helper::stringalized( _value, xml_value, 512 );

            _attrib.set_value( xml_value );
        }

        template<>
        MENGINE_INLINE void setXmlValue<Color>( pugi::xml_attribute & _attrib, const Color & _value )
        {
            Char xml_value[512 + 1] = {'\0'};
            Helper::stringalized( _value, xml_value, 512 );

            _attrib.set_value( xml_value );
        }

        template<>
        MENGINE_INLINE void setXmlValue<String>( pugi::xml_attribute & _attrib, const String & _value )
        {
            _attrib.set_value( _value.c_str() );
        }

        template<>
        MENGINE_INLINE void setXmlValue<ConstString>( pugi::xml_attribute & _attrib, const ConstString & _value )
        {
            _attrib.set_value( _value.c_str() );
        }

        template<>
        MENGINE_INLINE void setXmlValue<FilePath>( pugi::xml_attribute & _attrib, const FilePath & _value )
        {
            _attrib.set_value( _value.c_str() );
        }

        template<typename T>
        T getXmlValue( const pugi::xml_attribute & _attrib );

        template<>
        MENGINE_INLINE bool getXmlValue<bool>( const pugi::xml_attribute & _attrib )
        {
            return _attrib.as_bool();
        }

        template<>
        MENGINE_INLINE int8_t getXmlValue<int8_t>( const pugi::xml_attribute & _attrib )
        {
            return (int8_t)_attrib.as_int();
        }

        template<>
        MENGINE_INLINE int32_t getXmlValue<int32_t>( const pugi::xml_attribute & _attrib )
        {
            return _attrib.as_int();
        }

        template<>
        MENGINE_INLINE uint32_t getXmlValue<uint32_t>( const pugi::xml_attribute & _attrib )
        {
            return _attrib.as_uint();
        }

        template<>
        MENGINE_INLINE float getXmlValue<float>( const pugi::xml_attribute & _attrib )
        {
            return _attrib.as_float();
        }

        MENGINE_INLINE void deserializeFloats( const pugi::xml_attribute & _attrib, float * const _floats, const size_t _numFloats )
        {
            std::string strValue = _attrib.value();
            std::string::size_type lastPos = 0;
            for( size_t i = 0; i < _numFloats; ++i )
            {
                std::string::size_type endPos = strValue.find( MENGINE_PATH_FORWARDSLASH, lastPos );
                if( endPos == std::string::npos )
                {
                    endPos = strValue.length();
                }
                std::string strFloat = strValue.substr( lastPos, endPos - lastPos );
                lastPos = endPos + 1;

                _floats[i] = std::stof( strFloat );
            }
        }

        template<>
        MENGINE_INLINE mt::vec2f getXmlValue<mt::vec2f>( const pugi::xml_attribute & _attrib )
        {
            const Char * attrib_value = _attrib.value();

            mt::vec2f result;
            Helper::stringalized( attrib_value, &result );

            return result;
        }

        template<>
        MENGINE_INLINE mt::vec3f getXmlValue<mt::vec3f>( const pugi::xml_attribute & _attrib )
        {
            const Char * attrib_value = _attrib.value();

            mt::vec3f result;
            Helper::stringalized( attrib_value, &result );

            return result;
        }

        template<>
        MENGINE_INLINE mt::uv4f getXmlValue<mt::uv4f>( const pugi::xml_attribute & _attrib )
        {
            const Char * attrib_value = _attrib.value();

            mt::uv4f result;
            Helper::stringalized( attrib_value, &result );

            return result;
        }

        template<>
        MENGINE_INLINE Color getXmlValue<Color>( const pugi::xml_attribute & _attrib )
        {
            const Char * attrib_value = _attrib.value();

            Color result;
            Helper::stringalized( attrib_value, &result );

            return result;
        }

        template<>
        MENGINE_INLINE String getXmlValue<String>( const pugi::xml_attribute & _attrib )
        {
            return _attrib.as_string();
        }

        template<>
        MENGINE_INLINE ConstString getXmlValue<ConstString>( const pugi::xml_attribute & _attrib )
        {
            return Helper::stringizeString( _attrib.as_string() );
        }

        template<typename T>
        bool deserializeNodePropImpl( const Char * _propType, const Char * _propName,
            const pugi::xml_node & _xmlParentNode,
            const Lambda<void( const T & )> & _lambda )
        {
            pugi::xml_node propNode = _xmlParentNode.child( _propName );
            if( !propNode )
            {
                return false;
            }

            pugi::xml_attribute typeAttrib = propNode.attribute( "type" );
            if( !typeAttrib )
            {
                return false;
            }

            const Char * type = typeAttrib.value();
            if( StdString::strcmp( type, _propType ) != 0 )
            {
                return false;
            }

            pugi::xml_attribute valueAttrib = propNode.attribute( "value" );
            if( !valueAttrib )
            {
                return false;
            }

            T t = getXmlValue<T>( valueAttrib );

            _lambda( t );

            return true;
        }

        template<class T>
        struct prop_type_name
        {
        };

        template<>
        struct prop_type_name<bool>
        {
            static const Char * get_value()
            {
                return "bool";
            }
        };

        template<>
        struct prop_type_name<int8_t>
        {
            static const Char * get_value()
            {
                return "int8_t";
            }
        };

        template<>
        struct prop_type_name<int32_t>
        {
            static const Char * get_value()
            {
                return "int32_t";
            }
        };

        template<>
        struct prop_type_name<uint32_t>
        {
            static const Char * get_value()
            {
                return "uint32_t";
            }
        };

        template<>
        struct prop_type_name<float>
        {
            static const Char * get_value()
            {
                return "float";
            }
        };

        template<>
        struct prop_type_name<mt::vec2f>
        {
            static const Char * get_value()
            {
                return "mt::vec2f";
            }
        };

        template<>
        struct prop_type_name<mt::vec3f>
        {
            static const Char * get_value()
            {
                return "mt::vec3f";
            }
        };

        template<>
        struct prop_type_name<mt::uv4f>
        {
            static const Char * get_value()
            {
                return "mt::uv4f";
            }
        };

        template<>
        struct prop_type_name<Color>
        {
            static const Char * get_value()
            {
                return "Color";
            }
        };

        template<>
        struct prop_type_name<String>
        {
            static const Char * get_value()
            {
                return "String";
            }
        };

        template<>
        struct prop_type_name<ConstString>
        {
            static const Char * get_value()
            {
                return "String";
            }
        };

        template<>
        struct prop_type_name<FilePath>
        {
            static const Char * get_value()
            {
                return "String";
            }
        };

        template<typename T>
        void serializeNodeProp( const T & _prop, const Char * _propName, pugi::xml_node & _xmlParentNode )
        {
            pugi::xml_node propNode = _xmlParentNode.append_child( _propName );
            const Char * type_name = Detail::prop_type_name<T>::get_value();
            propNode.append_attribute( "type" ).set_value( type_name );
            pugi::xml_attribute valueAttrib = propNode.append_attribute( "value" );
            Detail::setXmlValue<T>( valueAttrib, _prop );
        }

        MENGINE_INLINE void serializeNodeProp( const Char * _prop, const Char * _propName, pugi::xml_node & _xmlParentNode )
        {
            pugi::xml_node propNode = _xmlParentNode.append_child( _propName );
            const Char * type_name = Detail::prop_type_name<String>::get_value();
            propNode.append_attribute( "type" ).set_value( type_name );
            pugi::xml_attribute valueAttrib = propNode.append_attribute( "value" );
            valueAttrib.set_value( _prop );
        }

        template<size_t N>
        void serializeNodeProp( const Char (&_prop)[N], const Char * _propName, pugi::xml_node & _xmlParentNode )
        {
            const Char * str = _prop;

            Detail::serializeNodeProp( str, _propName, _xmlParentNode );
        }

        template<typename T>
        bool deserializeNodeProp( const Char * _propName, const pugi::xml_node & _xmlParentNode, const Lambda<void( const T & )> & _lambda )
        {
            const Char * type_name = Detail::prop_type_name<T>::get_value();

            bool result = Detail::deserializeNodePropImpl<T>( type_name, _propName, _xmlParentNode, _lambda );

            return result;
        }
    }

    class MyXMLWriter
        : public pugi::xml_writer
    {
    public:
        MyXMLWriter() = delete;
        MyXMLWriter( const MyXMLWriter & ) = delete;

    public:
        MyXMLWriter( Data & _buffer )
            : m_buffer( _buffer )
        {
        }

        ~MyXMLWriter()
        {
        }

    public:
        void write( const void * _data, size_t _size ) override
        {
            const uint8_t * ptr = reinterpret_cast<const uint8_t *>(_data);
            m_buffer.insert( m_buffer.end(), ptr, ptr + _size );

            m_debug.append( (const Char *)_data, _size );
        }

    protected:
        Data & m_buffer;

        String m_debug;
    };
}
