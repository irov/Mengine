#pragma once

#include "tinyini/tinyini.hpp"

#include "Interface/ServiceInterface.h"
#include "Interface/InputStreamInterface.h"
#include "Interface/FileGroupInterface.h"
#include "Interface/MemoryInterface.h"

#include "Kernel/Params.h"
#include "Kernel/ConstString.h"
#include "Kernel/VectorConstString.h"
#include "Kernel/FilePath.h"
#include "Kernel/VectorFilePath.h"
#include "Kernel/Tags.h"
#include "Kernel/Resolution.h"
#include "Kernel/Viewport.h"
#include "Kernel/AspectRatioViewport.h"
#include "Kernel/VectorAspectRatioViewports.h"
#include "Kernel/Color.h"
#include "Kernel/String.h"
#include "Kernel/VectorString.h"

namespace Mengine
{
    namespace INIUtil
    {
        //////////////////////////////////////////////////////////////////////////
        struct IniStore
        {
            FilePath path;
            MemoryInterfacePtr memory;

            tinyini_t ini;
        };
        //////////////////////////////////////////////////////////////////////////
        bool loadIni( IniStore & _ini, const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, const DocumentPtr & _doc );
        bool loadIni( IniStore & _ini, const InputStreamInterfacePtr & _stream, const DocumentPtr & _doc );
        //////////////////////////////////////////////////////////////////////////
        bool hasIniSection( const IniStore & _ini, const Char * _section );
        bool hasIniValue( const IniStore & _ini, const Char * _section, const Char * _key );
        //////////////////////////////////////////////////////////////////////////
        bool getIniValue( const IniStore & _ini, const Char * _section, const Char * _key, const Char ** const _value );
        bool getIniValue( const IniStore & _ini, const Char * _section, const Char * _key, ConstString * const _value );
        bool getIniValue( const IniStore & _ini, const Char * _section, const Char * _key, Tags * const _value );
        bool getIniValue( const IniStore & _ini, const Char * _section, const Char * _key, Resolution * const _value );
        bool getIniValue( const IniStore & _ini, const Char * _section, const Char * _key, Color * const _value );
        bool getIniValue( const IniStore & _ini, const Char * _section, const Char * _key, FilePath * const _value );
        bool getIniValue( const IniStore & _ini, const Char * _section, const Char * _key, bool * const _value );
        bool getIniValue( const IniStore & _ini, const Char * _section, const Char * _key, int8_t * const _value );
        bool getIniValue( const IniStore & _ini, const Char * _section, const Char * _key, uint8_t * const _value );
        bool getIniValue( const IniStore & _ini, const Char * _section, const Char * _key, int32_t * const _value );
        bool getIniValue( const IniStore & _ini, const Char * _section, const Char * _key, uint32_t * const _value );
        bool getIniValue( const IniStore & _ini, const Char * _section, const Char * _key, int64_t * const _value );
        bool getIniValue( const IniStore & _ini, const Char * _section, const Char * _key, uint64_t * const _value );
        bool getIniValue( const IniStore & _ini, const Char * _section, const Char * _key, float * const _value );
        bool getIniValue( const IniStore & _ini, const Char * _section, const Char * _key, double * const _value );
        bool getIniValues( const IniStore & _ini, const Char * _section, const Char * _key, VectorString * const  _values );
        bool getIniValues( const IniStore & _ini, const Char * _section, const Char * _key, VectorConstString * const _values );
        bool getIniValues( const IniStore & _ini, const Char * _section, const Char * _key, VectorFilePath * const _values );
        bool getIniValues( const IniStore & _ini, const Char * _section, const Char * _key, VectorAspectRatioViewports * const _values );
        //////////////////////////////////////////////////////////////////////////
        bool getIniAllSettings( const IniStore & _ini, const Char * _section, MapParams * _values );
        //////////////////////////////////////////////////////////////////////////
    }
}