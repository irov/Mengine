#pragma once

#include "Kernel/Tags.h"
#include "Kernel/Resolution.h"
#include "Kernel/ConstString.h"
#include "Kernel/FilePath.h"
#include "Kernel/Tags.h"
#include "Kernel/Resolution.h"
#include "Kernel/Color.h"
#include "Kernel/AspectRatioViewport.h"
#include "Kernel/String.h"
#include "Kernel/VectorChar.h"

#include "jpp/jpp.hpp"

namespace jpp
{
    //////////////////////////////////////////////////////////////////////////
    template<>
    struct extract_object_extern<Mengine::Tags>
    {
        jpp::object operator()( const Mengine::Tags & _value ) const;
    };
    //////////////////////////////////////////////////////////////////////////
    template<>
    struct extract_object_extern<Mengine::Resolution>
    {
        jpp::object operator()( const Mengine::Resolution & _value ) const;
    };
    //////////////////////////////////////////////////////////////////////////
    template<>
    struct extract_object_extern<Mengine::Color>
    {
        jpp::object operator()( const Mengine::Color & _value ) const;
    };
    //////////////////////////////////////////////////////////////////////////
    template<>
    struct extract_object_extern<Mengine::ConstString>
    {
        jpp::object operator()( const Mengine::ConstString & _value ) const;
    };
    //////////////////////////////////////////////////////////////////////////
    template<>
    struct extract_object_extern<Mengine::FilePath>
    {
        jpp::object operator()( const Mengine::FilePath & _value ) const;
    };
    //////////////////////////////////////////////////////////////////////////
    template<>
    struct extract_object_extern<Mengine::String>
    {
        jpp::object operator()( const Mengine::String & _value ) const;
    };
    //////////////////////////////////////////////////////////////////////////
    template<>
    struct extract_object_extern<Mengine::VectorChar>
    {
        jpp::object operator()( const Mengine::VectorChar & _value ) const;
    };
    //////////////////////////////////////////////////////////////////////////
    template<>
    struct cast_object_extern<Mengine::ConstString>
    {
        void operator()( const jpp::object & _obj, Mengine::ConstString * const _value ) const;
    };
    //////////////////////////////////////////////////////////////////////////
    template<>
    struct cast_object_extern<Mengine::FilePath>
    {
        void operator()( const jpp::object & _obj, Mengine::FilePath * const _value ) const;
    };
    //////////////////////////////////////////////////////////////////////////
    template<>
    struct cast_object_extern<Mengine::Tags>
    {
        void operator()( const jpp::object & _obj, Mengine::Tags * const _value ) const;
    };
    //////////////////////////////////////////////////////////////////////////
    template<>
    struct cast_object_extern<Mengine::Resolution>
    {
        void operator()( const jpp::object & _obj, Mengine::Resolution * const _value ) const;
    };
    //////////////////////////////////////////////////////////////////////////
    template<>
    struct cast_object_extern<Mengine::Color>
    {
        void operator()( const jpp::object & _obj, Mengine::Color * const _value ) const;
    };
    //////////////////////////////////////////////////////////////////////////
    template<>
    struct cast_object_extern<Mengine::AspectRatioViewport>
    {
        void operator()( const jpp::object & _obj, Mengine::AspectRatioViewport * const _value ) const;
    };
    //////////////////////////////////////////////////////////////////////////
    template<>
    struct cast_object_extern<Mengine::String>
    {
        void operator()( const jpp::object & _obj, Mengine::String * const _value ) const;
    };
    //////////////////////////////////////////////////////////////////////////
    template<>
    struct cast_object_extern<Mengine::VectorChar>
    {
        void operator()( const jpp::object & _obj, Mengine::VectorChar * const _value ) const;
    };
    //////////////////////////////////////////////////////////////////////////    
    template<>
    struct check_object_extern<Mengine::ConstString>
    {
        bool operator()( const jpp::object & _obj, Mengine::ConstString * const ) const;
    };
    //////////////////////////////////////////////////////////////////////////
    template<>
    struct check_object_extern<Mengine::FilePath>
    {
        bool operator()( const jpp::object & _obj, Mengine::FilePath * const ) const;
    };
    //////////////////////////////////////////////////////////////////////////
    template<>
    struct check_object_extern<Mengine::Tags>
    {
        bool operator()( const jpp::object & _obj, Mengine::Tags * const ) const;
    };
    //////////////////////////////////////////////////////////////////////////
    template<>
    struct check_object_extern<Mengine::Resolution>
    {
        bool operator()( const jpp::object & _obj, Mengine::Resolution * const ) const;
    };
    //////////////////////////////////////////////////////////////////////////
    template<>
    struct check_object_extern<Mengine::Color>
    {
        bool operator()( const jpp::object & _obj, Mengine::Color * const ) const;
    };
    //////////////////////////////////////////////////////////////////////////
    template<>
    struct check_object_extern<Mengine::AspectRatioViewport>
    {
        bool operator()( const jpp::object & _obj, Mengine::AspectRatioViewport * const ) const;
    };
    //////////////////////////////////////////////////////////////////////////
    template<>
    struct check_object_extern<Mengine::String>
    {
        bool operator()( const jpp::object & _obj, Mengine::String * const ) const;
    };
    //////////////////////////////////////////////////////////////////////////
    template<>
    struct check_object_extern<Mengine::VectorChar>
    {
        bool operator()( const jpp::object & _obj, Mengine::VectorChar * const ) const;
    };
    //////////////////////////////////////////////////////////////////////////
}