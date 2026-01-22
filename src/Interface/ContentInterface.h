#pragma once

#include "Interface/ServantInterface.h"

#include "Interface/FileGroupInterface.h"
#include "Interface/MemoryInterface.h"
#include "Interface/InputStreamInterface.h"
#include "Interface/OutputStreamInterface.h"
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
        virtual void setGroupName( const ConstString & _groupName ) = 0;
        virtual const ConstString & getGroupName() const = 0;

    public:
        virtual void setFileGroup( const FileGroupInterfacePtr & _fileGroup ) = 0;
        virtual const FileGroupInterfacePtr & getFileGroup() const = 0;

        virtual void setFilePath( const FilePath & _filePath ) = 0;
        virtual const FilePath & getFilePath() const = 0;

        virtual void getFullPath( Char * const _fullPath ) const = 0;

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

    public:
        virtual MemoryInterfacePtr createMemoryFile( bool _stream, bool _share, const DocumentInterfacePtr & _doc ) = 0;
        virtual MemoryInterfacePtr createMemoryFileString( bool _stream, bool _share, const DocumentInterfacePtr & _doc ) = 0;

        virtual InputStreamInterfacePtr openInputStreamFile( bool _streaming, bool _share, const DocumentInterfacePtr & _doc ) = 0;
        virtual void closeInputStreamFile( const InputStreamInterfacePtr & _stream ) = 0;

        virtual OutputStreamInterfacePtr openOutputStreamFile( bool _withTemp, const DocumentInterfacePtr & _doc ) = 0;
        virtual bool closeOutputStreamFile( const OutputStreamInterfacePtr & _stream ) = 0;

    public:
        virtual bool createDirectory() = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<ContentInterface> ContentInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}
