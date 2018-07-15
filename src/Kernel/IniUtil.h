#pragma once

#include "tinyini/tinyini.h"

#include "Interface/ServiceInterface.h"
#include "Interface/StreamInterface.h"
#include "Interface/FileGroupInterface.h"

#include "Kernel/Params.h"
#include "Kernel/String.h"
#include "Kernel/ConstString.h"
#include "Kernel/ConstStringTypes.h"
#include "Kernel/Tags.h"
#include "Kernel/FilePath.h"
#include "Kernel/Resolution.h"
#include "Kernel/Viewport.h"
#include "Kernel/AspectRatioViewports.h"
#include "Kernel/ColourValue.h"

#ifndef MENGINE_INI_BUFFER_SIZE 
#	define MENGINE_INI_BUFFER_SIZE 16384U
#endif

namespace Mengine
{
    namespace IniUtil
    {
        //////////////////////////////////////////////////////////////////////////
        struct IniStore
            : public tinyini::tinyini
        {
            FilePath path;

            char buff[MENGINE_INI_BUFFER_SIZE];
        };
        //////////////////////////////////////////////////////////////////////////
        bool loadIni( IniStore & _ini, const FileGroupInterfacePtr & _category, const FilePath & _path );
        bool loadIni( IniStore & _ini, const InputStreamInterfacePtr & _input );
        //////////////////////////////////////////////////////////////////////////
        bool hasIniValue( const IniStore & _ini, const Char * _section, const Char * _key );
        //////////////////////////////////////////////////////////////////////////
        bool getIniValue( const IniStore & _ini, const Char * _section, const Char * _key, WString & _value );
        bool getIniValue( const IniStore & _ini, const Char * _section, const Char * _key, String & _value );
        bool getIniValue( const IniStore & _ini, const Char * _section, const Char * _key, ConstString & _value );
        bool getIniValue( const IniStore & _ini, const Char * _section, const Char * _key, Tags & _value );
        bool getIniValue( const IniStore & _ini, const Char * _section, const Char * _key, Resolution & _value );
        bool getIniValue( const IniStore & _ini, const Char * _section, const Char * _key, ColourValue & _value );
        bool getIniValue( const IniStore & _ini, const Char * _section, const Char * _key, FilePath & _value );
        bool getIniValue( const IniStore & _ini, const Char * _section, const Char * _key, bool & _value );
        bool getIniValue( const IniStore & _ini, const Char * _section, const Char * _key, int32_t & _value );
        bool getIniValue( const IniStore & _ini, const Char * _section, const Char * _key, uint32_t & _value );
        bool getIniValue( const IniStore & _ini, const Char * _section, const Char * _key, uint64_t & _value );
        bool getIniValue( const IniStore & _ini, const Char * _section, const Char * _key, float & _value );
        bool getIniValue( const IniStore & _ini, const Char * _section, const Char * _key, double & _value );
        bool getIniValue( const IniStore & _ini, const Char * _section, const Char * _key, TVectorString & _values );
        bool getIniValue( const IniStore & _ini, const Char * _section, const Char * _key, TVectorConstString & _values );
        bool getIniValue( const IniStore & _ini, const Char * _section, const Char * _key, TVectorWString & _values );
        bool getIniValue( const IniStore & _ini, const Char * _section, const Char * _key, TVectorAspectRatioViewports & _values );
        //////////////////////////////////////////////////////////////////////////
        bool getIniAllSettings( const IniStore & _ini, const Char * _section, TMapWParams & _values );
        //////////////////////////////////////////////////////////////////////////
        bool writeIniSetting( const OutputStreamInterfacePtr & _file, const char * _key, const ConstString & _value );
        bool writeIniSetting( const OutputStreamInterfacePtr & _file, const char * _key, const WString & _value );
        bool writeIniSection( const OutputStreamInterfacePtr & _file, const char * _section, uint32_t _sectionSize );
        //////////////////////////////////////////////////////////////////////////
        template<class T, size_t I_Size>
        static bool writeIniSection( const OutputStreamInterfacePtr & _file, T( &_section )[I_Size] )
        {
            bool result = writeIniSection( _file, _section, sizeof( _section ) - 1 );

            return result;
        }
    }
}