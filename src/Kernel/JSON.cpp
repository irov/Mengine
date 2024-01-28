#include "JSON.h"

#include "Kernel/ConstStringHelper.h"
#include "Kernel/FilePathHelper.h"
#include "Kernel/ColorHelper.h"

namespace jpp
{
    //////////////////////////////////////////////////////////////////////////    
    jpp::object extract_object_extern<Mengine::Tags>::operator()( const Mengine::Tags & _value ) const
    {
        jpp::array j_tags = jpp::make_array();

        for( const Mengine::ConstString & tag : _value )
        {
            jpp::object j_tag = jpp::make_stringn( tag.c_str(), tag.size() );

            j_tags.push_back( j_tag );
        }

        return j_tags.to_object();
    };
    //////////////////////////////////////////////////////////////////////////    
    jpp::object extract_object_extern<Mengine::Resolution>::operator()( const Mengine::Resolution & _value ) const
    {
        jpp::array j_resolution = jpp::make_array();

        uint32_t width = _value.getWidth();
        uint32_t height = _value.getHeight();

        j_resolution.push_back( width );
        j_resolution.push_back( height );

        return j_resolution.to_object();
    };
    //////////////////////////////////////////////////////////////////////////    
    jpp::object extract_object_extern<Mengine::Color>::operator()( const Mengine::Color & _value ) const
    {
        jpp::array j_color = jpp::make_array();

        uint8_t r8 = _value.getR8();
        uint8_t g8 = _value.getG8();
        uint8_t b8 = _value.getB8();
        uint8_t a8 = _value.getA8();
        
        j_color.push_back( r8 );
        j_color.push_back( g8 );
        j_color.push_back( b8 );
        j_color.push_back( a8 );

        return j_color.to_object();
    };
    //////////////////////////////////////////////////////////////////////////    
    jpp::object extract_object_extern<Mengine::ConstString>::operator()( const Mengine::ConstString & _value ) const
    {
        const Mengine::Char * value_str = _value.c_str();

        return jpp::make_string( value_str );
    };
    //////////////////////////////////////////////////////////////////////////    
    jpp::object extract_object_extern<Mengine::FilePath>::operator()( const Mengine::FilePath & _value ) const
    {
        const Mengine::Char * value_str = _value.c_str();

        return jpp::make_string( value_str );
    };
    //////////////////////////////////////////////////////////////////////////    
    jpp::object extract_object_extern<Mengine::String>::operator()( const Mengine::String & _value ) const
    {
        const Mengine::Char * value_str = _value.c_str();

        return jpp::make_string( value_str );
    };
    //////////////////////////////////////////////////////////////////////////    
    jpp::object extract_object_extern<Mengine::VectorChar>::operator()( const Mengine::VectorChar & _value ) const
    {
        const Mengine::Char * value_str = _value.data();
        Mengine::VectorChar::size_type value_size = _value.size();

        return jpp::make_stringn( value_str, value_size );
    };
    //////////////////////////////////////////////////////////////////////////
    void cast_object_extern<Mengine::ConstString>::operator()( const jpp::object & _obj, Mengine::ConstString * const _value ) const
    {
        const Mengine::Char * value = _obj;

        *_value = Mengine::Helper::stringizeString( value );
    };
    //////////////////////////////////////////////////////////////////////////
    void cast_object_extern<Mengine::FilePath>::operator()( const jpp::object & _obj, Mengine::FilePath * const _value ) const
    {
        const Mengine::Char * value = _obj;

        *_value = Mengine::Helper::stringizeFilePath( value );
    };
    //////////////////////////////////////////////////////////////////////////
    void cast_object_extern<Mengine::Tags>::operator()( const jpp::object & _obj, Mengine::Tags * const _value ) const
    {
        Mengine::Tags tags;

        for( const jpp::object & j : jpp::array( _obj ) )
        {
            Mengine::ConstString tag = j;

            tags.addTag( tag );
        }

        *_value = tags;
    };
    //////////////////////////////////////////////////////////////////////////
    void cast_object_extern<Mengine::Resolution>::operator()( const jpp::object & _obj, Mengine::Resolution * const _value ) const
    {
        const jpp::array j_a( _obj );

        uint32_t width = j_a[0];
        uint32_t height = j_a[1];

        *_value = Mengine::Resolution( width, height );
    };
    //////////////////////////////////////////////////////////////////////////
    void cast_object_extern<Mengine::Color>::operator()( const jpp::object & _obj, Mengine::Color * const _value ) const
    {
        const jpp::array j_a( _obj );

        uint8_t r = j_a[0];
        uint8_t g = j_a[1];
        uint8_t b = j_a[2];
        uint8_t a = j_a[3];

        *_value = Mengine::Helper::makeColor8( r, g, b, a );
    };
    //////////////////////////////////////////////////////////////////////////
    void cast_object_extern<Mengine::AspectRatioViewport>::operator()( const jpp::object & _obj, Mengine::AspectRatioViewport * const _value ) const
    {
        const jpp::array j_a( _obj );

        Mengine::AspectRatioViewport aspect;

        aspect.width = j_a[0];
        aspect.height = j_a[1];
        aspect.viewport.begin.x = j_a[2];
        aspect.viewport.begin.y = j_a[3];
        aspect.viewport.end.x = j_a[4];
        aspect.viewport.end.y = j_a[5];

        *_value = aspect;
    };
    //////////////////////////////////////////////////////////////////////////
    void cast_object_extern<Mengine::String>::operator()( const jpp::object & _obj, Mengine::String * const _value ) const
    {
        const Mengine::Char * value = _obj;

        *_value = Mengine::String( value );
    };
    //////////////////////////////////////////////////////////////////////////
    void cast_object_extern<Mengine::VectorChar>::operator()( const jpp::object & _obj, Mengine::VectorChar * const _value ) const
    {
        jpp_size_t size;
        const Mengine::Char * value = jpp::get_string_and_size( _obj, &size );

        *_value = Mengine::VectorChar( value, value + size );
    };
    //////////////////////////////////////////////////////////////////////////
    bool check_object_extern<Mengine::ConstString>::operator()( const jpp::object & _obj, Mengine::ConstString * const ) const
    {
        bool result = _obj.is_type_string();

        return result;
    };
    //////////////////////////////////////////////////////////////////////////
    bool check_object_extern<Mengine::FilePath>::operator()( const jpp::object & _obj, Mengine::FilePath * const ) const
    {
        bool result = _obj.is_type_string();

        return result;
    };
    //////////////////////////////////////////////////////////////////////////
    bool check_object_extern<Mengine::Tags>::operator()( const jpp::object & _obj, Mengine::Tags * const ) const
    {
        bool result = _obj.is_type_array();

        return result;
    };
    //////////////////////////////////////////////////////////////////////////
    bool check_object_extern<Mengine::Resolution>::operator()( const jpp::object & _obj, Mengine::Resolution * const ) const
    {
        bool result = _obj.is_type_array();

        return result;
    };
    //////////////////////////////////////////////////////////////////////////
    bool check_object_extern<Mengine::Color>::operator()( const jpp::object & _obj, Mengine::Color * const ) const
    {
        bool result = _obj.is_type_array();

        return result;
    };
    //////////////////////////////////////////////////////////////////////////
    bool check_object_extern<Mengine::AspectRatioViewport>::operator()( const jpp::object & _obj, Mengine::AspectRatioViewport * const ) const
    {
        bool result = _obj.is_type_array();

        return result;
    };
    //////////////////////////////////////////////////////////////////////////
    bool check_object_extern<Mengine::String>::operator()( const jpp::object & _obj, Mengine::String * const ) const
    {
        bool result = _obj.is_type_string();

        return result;
    };
    //////////////////////////////////////////////////////////////////////////
    bool check_object_extern<Mengine::VectorChar>::operator()( const jpp::object & _obj, Mengine::VectorChar * const ) const
    {
        bool result = _obj.is_type_string();

        return result;
    };
    //////////////////////////////////////////////////////////////////////////
}