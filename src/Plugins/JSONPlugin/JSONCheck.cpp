#include "JSONCheck.h"

#include "Kernel/ConstStringHelper.h"
#include "Kernel/FilePathHelper.h"
#include "Kernel/ColorHelper.h"

namespace jpp
{
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