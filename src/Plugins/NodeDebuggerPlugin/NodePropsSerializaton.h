#pragma once
#include "Config/Lambda.h"


#ifdef NODE_SERIALIZATION_INGAME
#define NODE_TYPE Mengine::NodePtr
using Mengine::Color;
#else
#define NODE_TYPE Node
#define MENGINE_ASSERTION( _cond, _msg ) assert( _cond )

namespace Mengine
{
    template <typename T>
    using Vector = std::vector<T>;

    using String = std::string;
}
#endif


namespace _util
{
    template <typename T>
    void setXmlValue( pugi::xml_attribute & _attrib, const T & _value )
    {
        _attrib.set_value( _value );
    }

    template <>
    void setXmlValue<mt::vec2f>( pugi::xml_attribute & _attrib, const mt::vec2f & _value )
    {
        std::string str = std::to_string( _value.x ) + '/' + std::to_string( _value.y );

        _attrib.set_value( str.c_str() );
    }

    template <>
    void setXmlValue<mt::vec3f>( pugi::xml_attribute & _attrib, const mt::vec3f & _value )
    {
        std::string str = std::to_string( _value.x ) + '/' + std::to_string( _value.y ) + '/' + std::to_string( _value.z );

        _attrib.set_value( str.c_str() );
    }

    template <>
    void setXmlValue<Color>( pugi::xml_attribute & _attrib, const Color & _value )
    {
        setXmlValue<uint32_t>( _attrib, _value.getAsARGB() );
    }


    template <typename T>
    T getXmlValue( const pugi::xml_attribute & _attrib )
    {
        // nothing, specialize!
        MENGINE_ASSERTION( false, "Specialize getXmlValue!!!" );
    }

    template <>
    inline bool getXmlValue<bool>( const pugi::xml_attribute & _attrib )
    {
        return _attrib.as_bool();
    }

    template <>
    inline uint32_t getXmlValue<uint32_t>( const pugi::xml_attribute & _attrib )
    {
        return _attrib.as_uint();
    }

    inline void deserializeFloats( const pugi::xml_attribute & _attrib, float * _floats, const size_t _numFloats )
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
    inline mt::vec2f getXmlValue<mt::vec2f>( const pugi::xml_attribute & _attrib )
    {
        mt::vec2f result;

        deserializeFloats( _attrib, result.buff(), 2 );

        return result;
    }

    template <>
    inline mt::vec3f getXmlValue<mt::vec3f>( const pugi::xml_attribute & _attrib )
    {
        mt::vec3f result;

        deserializeFloats( _attrib, result.buff(), 3 );

        return result;
    }

    template <>
    inline Color getXmlValue<Color>( const pugi::xml_attribute & _attrib )
    {
        return Color( getXmlValue<uint32_t>( _attrib ) );
    }


    template <typename T>
    bool deserializeNodePropImpl( NODE_TYPE & _node,
                                  const char* _propType,
                                  const char* _propName,
                                  const pugi::xml_node & _xmlParentNode,
                                  const Mengine::Lambda<void( NODE_TYPE &, const T & )> & _lambda )
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

        Mengine::String type = typeAttrib.value();
        if( type != _propType )
        {
            return false;
        }

        pugi::xml_attribute valueAttrib = propNode.attribute( "value" );
        if( !valueAttrib)
        {
            return false;
        }

        _lambda( _node, getXmlValue<T>( valueAttrib ) );

        return true;
    }
}



template <typename T>
void serializeNodeProp( const T & _prop, const char * _propName, pugi::xml_node & _xmlParentNode )
{
    // nothing, specialize!
    MENGINE_ASSERTION( false, "Specialize serializeNodeProp!!!" );
}

#define PROP_SERIALIZATION_SPECIAL( _type )                                                                     \
template <>                                                                                                     \
void serializeNodeProp<_type>( const _type & _prop, const char * _propName, pugi::xml_node & _xmlParentNode )   \
{                                                                                                               \
    pugi::xml_node propNode = _xmlParentNode.append_child( _propName );                                         \
    propNode.append_attribute( "type" ).set_value( #_type );                                                    \
    pugi::xml_attribute valueAttrib = propNode.append_attribute( "value" );                                     \
    _util::setXmlValue<_type>( valueAttrib, _prop );                                                            \
}


PROP_SERIALIZATION_SPECIAL( bool )
PROP_SERIALIZATION_SPECIAL( uint32_t )
PROP_SERIALIZATION_SPECIAL( mt::vec2f )
PROP_SERIALIZATION_SPECIAL( mt::vec3f )
PROP_SERIALIZATION_SPECIAL( Color )


#undef PROP_SERIALIZATION_SPECIAL



template <typename T>
bool deserializeNodeProp( NODE_TYPE & _node,
                          const char * _propName,
                          const pugi::xml_node & _xmlParentNode,
                          const Mengine::Lambda<void( NODE_TYPE &, const T & )> & _lambda )

{
    // nothing, specialize!
    MENGINE_ASSERTION( false, "Specialize deserializeNodeProp!!!" );
}


#define PROP_DESERIALIZATION_SPECIAL( _type )                                                                   \
template <>                                                                                                     \
inline bool deserializeNodeProp<_type>( NODE_TYPE & _node,                                                      \
                                        const char * _propName,                                                 \
                                        const pugi::xml_node & _xmlParentNode,                                  \
                                        const Mengine::Lambda<void( NODE_TYPE &, const _type & )> & _lambda )   \
{                                                                                                               \
    return _util::deserializeNodePropImpl<_type>( _node, #_type, _propName, _xmlParentNode, _lambda );          \
}


PROP_DESERIALIZATION_SPECIAL( bool )
PROP_DESERIALIZATION_SPECIAL( uint32_t )
PROP_DESERIALIZATION_SPECIAL( mt::vec2f )
PROP_DESERIALIZATION_SPECIAL( mt::vec3f )
PROP_DESERIALIZATION_SPECIAL( Color )


#undef PROP_DESERIALIZATION_SPECIAL



class MyXMLWriter : public pugi::xml_writer
{
public:
    MyXMLWriter() = delete;
    MyXMLWriter( const MyXMLWriter& ) = delete;
    ~MyXMLWriter() = default;

    MyXMLWriter( Mengine::Vector<uint8_t>& _buffer )
        : m_buffer( _buffer )
    {
    }

    virtual void write( const void* _data, size_t _size ) override
    {
        const uint8_t* ptr = reinterpret_cast<const uint8_t*>( _data );
        m_buffer.insert( m_buffer.end(), ptr, ptr + _size );
    }

private:
    Mengine::Vector<uint8_t>& m_buffer;
};
