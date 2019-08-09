#pragma once

#include "Config/Lambda.h"
#include "Config/Vector.h"
#include "Config/String.h"
#include "Config/Char.h"

#include "math/vec2.h"
#include "math/vec3.h"

#include "Kernel/Color.h"
#include "Kernel/ConstString.h"
#include "Kernel/ConstStringHelper.h"

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

    MENGINE_INLINE void InsertPacketHeader( Vector<uint8_t> & _payload, const PacketHeader & _hdr )
    {
        const uint8_t * begin = reinterpret_cast<const uint8_t *>(&_hdr);
        const uint8_t * end = begin + sizeof( PacketHeader );
        _payload.insert( _payload.begin(), begin, end );
    }

    namespace Detail
    {
        template <typename T>
        void setXmlValue( pugi::xml_attribute & _attrib, const T & _value )
        {
            _attrib.set_value( _value );
        }

        template <>
        MENGINE_INLINE void setXmlValue<float>( pugi::xml_attribute & _attrib, const float & _value )
        {
            std::string str = std::to_string( _value );

            _attrib.set_value( str.c_str() );
        }

        template <>
        MENGINE_INLINE void setXmlValue<mt::vec2f>( pugi::xml_attribute & _attrib, const mt::vec2f & _value )
        {
            std::string str = std::to_string( _value.x ) + '/' + std::to_string( _value.y );

            _attrib.set_value( str.c_str() );
        }

        template <>
        MENGINE_INLINE void setXmlValue<mt::vec3f>( pugi::xml_attribute & _attrib, const mt::vec3f & _value )
        {
            std::string str = std::to_string( _value.x ) + '/' + std::to_string( _value.y ) + '/' + std::to_string( _value.z );

            _attrib.set_value( str.c_str() );
        }

        template <>
        MENGINE_INLINE void setXmlValue<Color>( pugi::xml_attribute & _attrib, const Color & _value )
        {
            setXmlValue<uint32_t>( _attrib, _value.getAsARGB() );
        }

        template <>
        MENGINE_INLINE void setXmlValue<String>( pugi::xml_attribute & _attrib, const String & _value )
        {
            _attrib.set_value( _value.c_str() );
        }

        template <>
        MENGINE_INLINE void setXmlValue<ConstString>( pugi::xml_attribute & _attrib, const ConstString & _value )
        {
            _attrib.set_value( _value.c_str() );
        }

        template <typename T>
        T getXmlValue( const pugi::xml_attribute & _attrib );

        template <>
        MENGINE_INLINE bool getXmlValue<bool>( const pugi::xml_attribute & _attrib )
        {
            return _attrib.as_bool();
        }

        template <>
        MENGINE_INLINE uint32_t getXmlValue<uint32_t>( const pugi::xml_attribute & _attrib )
        {
            return _attrib.as_uint();
        }

        template <>
        MENGINE_INLINE float getXmlValue<float>( const pugi::xml_attribute & _attrib )
        {
            return _attrib.as_float();
        }

        MENGINE_INLINE void deserializeFloats( const pugi::xml_attribute & _attrib, float * _floats, const size_t _numFloats )
        {
            std::string strValue = _attrib.value();
            std::string::size_type lastPos = 0;
            for( size_t i = 0; i < _numFloats; ++i )
            {
                std::string::size_type endPos = strValue.find( '/', lastPos );
                if( endPos == std::string::npos )
                {
                    endPos = strValue.length();
                }
                std::string strFloat = strValue.substr( lastPos, endPos - lastPos );
                lastPos = endPos + 1;

                _floats[i] = std::stof( strFloat );
            }
        }

        template <>
        MENGINE_INLINE mt::vec2f getXmlValue<mt::vec2f>( const pugi::xml_attribute & _attrib )
        {
            mt::vec2f result;

            deserializeFloats( _attrib, result.buff(), 2 );

            return result;
        }

        template <>
        MENGINE_INLINE mt::vec3f getXmlValue<mt::vec3f>( const pugi::xml_attribute & _attrib )
        {
            mt::vec3f result;

            deserializeFloats( _attrib, result.buff(), 3 );

            return result;
        }

        template <>
        MENGINE_INLINE Color getXmlValue<Color>( const pugi::xml_attribute & _attrib )
        {
            return Color( getXmlValue<uint32_t>( _attrib ) );
        }

        template <>
        MENGINE_INLINE String getXmlValue<String>( const pugi::xml_attribute & _attrib )
        {
            return _attrib.as_string();
        }

        template <>
        MENGINE_INLINE ConstString getXmlValue<ConstString>( const pugi::xml_attribute & _attrib )
        {
            return Helper::stringizeString( _attrib.as_string() );
        }

        template <typename T>
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

            String type = typeAttrib.value();
            if( type != _propType )
            {
                return false;
            }

            pugi::xml_attribute valueAttrib = propNode.attribute( "value" );
            if( !valueAttrib )
            {
                return false;
            }

            _lambda( getXmlValue<T>( valueAttrib ) );

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
    }

    template <typename T>
    void serializeNodeProp( const T & _prop, const Char * _propName, pugi::xml_node & _xmlParentNode )
    {
        pugi::xml_node propNode = _xmlParentNode.append_child( _propName );
        const Char * type_name = Detail::prop_type_name<T>::get_value();
        propNode.append_attribute( "type" ).set_value( type_name );
        pugi::xml_attribute valueAttrib = propNode.append_attribute( "value" );
        Detail::setXmlValue<T>( valueAttrib, _prop );
    }

    template<typename T>
    bool deserializeNodeProp( const Char * _propName,
        const pugi::xml_node & _xmlParentNode,
        const Lambda<void( const T & )> & _lambda )
    {
        const Char * type_name = Detail::prop_type_name<T>::get_value();

        return Detail::deserializeNodePropImpl<T>( type_name, _propName, _xmlParentNode, _lambda );
    }

    class MyXMLWriter
        : public pugi::xml_writer
    {
    public:
        MyXMLWriter() = delete;
        MyXMLWriter( const MyXMLWriter & ) = delete;
        ~MyXMLWriter() = default;

        MyXMLWriter( Vector<uint8_t> & _buffer )
            : m_buffer( _buffer )
        {
        }

        void write( const void * _data, size_t _size ) override
        {
            const uint8_t * ptr = reinterpret_cast<const uint8_t *>(_data);
            m_buffer.insert( m_buffer.end(), ptr, ptr + _size );

            m_debug.append( (const char *)_data, _size );
        }

    private:
        Vector<uint8_t> & m_buffer;

        String m_debug;
    };
}
