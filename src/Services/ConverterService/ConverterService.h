#pragma once

#include "Interface/ConverterServiceInterface.h"
#include "Interface/ConverterFactoryInterface.h"

#include "Kernel/ServiceBase.h"

#include "Kernel/ConstString.h"

#include "Config/Map.h"

namespace Mengine
{
    class ConverterService
        : public ServiceBase<ConverterServiceInterface>
    {
    public:
        ConverterService();
        ~ConverterService() override;

    public:
        bool registerConverter( const ConstString & _type, const ConverterFactoryInterfacePtr & _interface ) override;
        bool unregisterConverter( const ConstString & _type ) override;

        ConverterInterfacePtr createConverter( const ConstString & _type, const Char * _doc ) override;

    public:
        bool convert( const ConstString & _converter, const FileGroupInterfacePtr & _fileGroup, const FilePath & _in, FilePath * _out, const Char * _doc ) override;

    protected:
        typedef Map<ConstString, ConverterFactoryInterfacePtr> MapConverterSystem;
        MapConverterSystem m_mapConverterSystem;
    };
}
