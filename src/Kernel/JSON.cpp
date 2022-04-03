#include "JSON.h"

#include "Kernel/ConstStringHelper.h"
#include "Kernel/FilePathHelper.h"
#include "Kernel/ColorHelper.h"

namespace jpp
{
    //////////////////////////////////////////////////////////////////////////    
    jpp::object extract_object_extern<Mengine::ConstString>::operator()( const Mengine::ConstString & _value ) const
    {
        const Mengine::Char * value_str = _value.c_str();

        return jpp::make_string( value_str );
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
        const jpp::array a( _obj );

        *_value = Mengine::Resolution( a[0], a[1] );
    };
    //////////////////////////////////////////////////////////////////////////
    void cast_object_extern<Mengine::Color>::operator()( const jpp::object & _obj, Mengine::Color * const _value ) const
    {
        const jpp::array a( _obj );

        *_value = Mengine::Helper::makeColor( a[0], a[1], a[2], a[3] );
    };
    //////////////////////////////////////////////////////////////////////////
    void cast_object_extern<Mengine::AspectRatioViewport>::operator()( const jpp::object & _obj, Mengine::AspectRatioViewport * const _value ) const
    {
        const jpp::array a( _obj );

        Mengine::AspectRatioViewport aspect;

        aspect.width = a[0];
        aspect.height = a[1];
        aspect.viewport.begin.x = a[2];
        aspect.viewport.begin.y = a[3];
        aspect.viewport.end.x = a[4];
        aspect.viewport.end.y = a[5];

        *_value = aspect;
    };
    //////////////////////////////////////////////////////////////////////////
    void cast_object_extern<Mengine::String>::operator()( const jpp::object & _obj, Mengine::String * const _value ) const
    {
        const Mengine::Char * value = _obj;

        *_value = Mengine::String( value );
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
}