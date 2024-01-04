#pragma once

#include "Interface/ServantInterface.h"

#include "Interface/FileGroupInterface.h"
#include "Interface/DataflowInterface.h"

#include "Kernel/FilePath.h"
#include "Kernel/ConstString.h"
#include "Kernel/FilePath.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class ContentInterface
        : public ServantInterface
    {
    public:
        virtual void setFileGroup( const FileGroupInterfacePtr & _fileGroup ) = 0;
        virtual const FileGroupInterfacePtr & getFileGroup() const = 0;

        virtual void setFilePath( const FilePath & _filePath ) = 0;
        virtual const FilePath & getFilePath() const = 0;

    public:
        virtual void setCodecType( const ConstString & _codecType ) = 0;
        virtual const ConstString & getCodecType() const = 0;

        virtual void setConverterType( const ConstString & _converterType ) = 0;
        virtual const ConstString & getConverterType() const = 0;

        virtual void setDataflow( const DataflowInterfacePtr & _dataflow ) = 0;
        virtual const DataflowInterfacePtr & getDataflow() const = 0;

    public:
        virtual void setValidNoExist( bool _validNoExist ) = 0;
        virtual bool isValidNoExist() const = 0;

    public:
        virtual bool exist( bool _recursive ) const = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ContentInterface> ContentInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}
