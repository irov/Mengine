#pragma once

#include "Interface/ServantInterface.h"
#include "Interface/InputStreamInterface.h"
#include "Interface/ContentInterface.h"

#include "Kernel/ConstString.h"
#include "Kernel/Params.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    struct ConverterOptions
    {
        ContentInterfacePtr inputContent;
        ContentInterfacePtr outputContent;

        Params params;
    };
    //////////////////////////////////////////////////////////////////////////
    class ConverterInterface
        : public ServantInterface
    {
    public:
        virtual bool initialize( const FileGroupInterfacePtr & _fileGroupDev ) = 0;
        virtual void finalize() = 0;

    public:
        virtual void setOptions( const ConverterOptions & _options ) = 0;
        virtual const ConverterOptions & getOptions() const = 0;

    public:
        virtual const ConstString & getConvertExt() const = 0;

    public:
        virtual bool validateVersion( const InputStreamInterfacePtr & _stream ) const = 0;

    public:
        virtual bool convert() = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ConverterInterface> ConverterInterfacePtr;
    //////////////////////////////////////////////////////////////////////////  
}
