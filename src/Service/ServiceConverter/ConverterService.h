#pragma once

#include "Interface/ConverterInterface.h"

#include "Kernel/ServiceBase.h"

#include "Kernel/ConstString.h"

#include "Config/Map.h"

namespace Mengine
{
    class ConverterFactoryInterface;
    class ConverterInterface;


    class ConverterService
        : public ServiceBase<ConverterServiceInterface>
    {
    public:
        ConverterService();
        ~ConverterService() override;

    public:
        bool registerConverter( const ConstString & _type, const ConverterFactoryInterfacePtr & _interface ) override;
        bool unregisterConverter( const ConstString & _type ) override;

        ConverterInterfacePtr createConverter( const ConstString & _type ) override;

    public:
        bool convert( const ConstString & _converter, const FileGroupInterfacePtr & _category, const FilePath & _in, FilePath & _out ) override;

    protected:
        typedef Map<ConstString, ConverterFactoryInterfacePtr> MapConverterSystem;
        MapConverterSystem m_mapConverterSystem;
    };
}
