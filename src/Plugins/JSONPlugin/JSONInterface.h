#pragma once

#include "Interface/UnknownInterface.h"
#include "Interface/ServiceInterface.h"
#include "Interface/InputStreamInterface.h"
#include "Interface/FileGroupInterface.h"
#include "Interface/MemoryInterface.h"

#include "jpp/jpp.hpp"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class JSONStorageInterface
        : public Interface
    {
    public:
        virtual const jpp::object & getJSON() const = 0;

    public:
        virtual void mergeJSON( const jpp::object & _obj ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<JSONStorageInterface> JSONStorageInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class UnknownResourceJSONInterface
        : public UnknownInterface
    {
    public:
        virtual const JSONStorageInterfacePtr & getJSONStorage() const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    class JSONServiceInterface
        : public ServiceInterface
    {
        SERVICE_DECLARE( "JSONService" );

    public:
        virtual JSONStorageInterfacePtr createStorage( const jpp::object & _json, const DocumentPtr & _doc ) const = 0;

    public:
        virtual JSONStorageInterfacePtr loadJSON( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, const DocumentPtr & _doc ) const = 0;
        virtual JSONStorageInterfacePtr loadJSONStream( const InputStreamInterfacePtr & _stream, const DocumentPtr & _doc ) const = 0;
        virtual JSONStorageInterfacePtr createJSON( const MemoryInterfacePtr & _memory, const DocumentPtr & _doc ) const = 0;
        virtual JSONStorageInterfacePtr createJSONBuffer( const void * _buffer, size_t _size, const DocumentPtr & _doc ) const = 0;
    };
}
//////////////////////////////////////////////////////////////////////////
#define JSON_SERVICE()\
	((Mengine::JSONServiceInterface *)SERVICE_GET(Mengine::JSONServiceInterface))
//////////////////////////////////////////////////////////////////////////