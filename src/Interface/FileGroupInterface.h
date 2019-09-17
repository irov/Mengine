#pragma once

#include "Interface/ServantInterface.h"
#include "Interface/InputStreamInterface.h"
#include "Interface/OutputStreamInterface.h"

#include "Kernel/Mixin.h"
#include "Kernel/ConstString.h"
#include "Kernel/FilePath.h"
#include "Kernel/LambdaFilePath.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<class FileGroupInterface> FileGroupInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class FileGroupInterface
        : public ServantInterface
    {
    public:
        virtual bool initialize() = 0;
        virtual void finalize() = 0;

    public:
        virtual void setName( const ConstString & _name ) = 0;
        virtual const ConstString & getName() const = 0;

    public:
        virtual void setBaseFileGroup( const FileGroupInterfacePtr & _baseFileGroup ) = 0;
        virtual const FileGroupInterfacePtr & getBaseFileGroup() const = 0;

        virtual void setParentFileGroup( const FileGroupInterfacePtr & _parentFileGroup ) = 0;
        virtual const FileGroupInterfacePtr & getParentFileGroup() const = 0;

    public:
        virtual void setRelationPath( const FilePath & _relationPath ) = 0;
        virtual const FilePath & getRelationPath() const = 0;

        virtual void setFolderPath( const FilePath & _folderPath ) = 0;
        virtual const FilePath & getFolderPath() const = 0;

    public:
        virtual bool isPacked() const = 0;

    public:
        virtual bool existFile( const FilePath & _filePath, bool _recursive ) const = 0;
        virtual bool existDirectory( const FilePath & _folderPath, bool _recursive ) const = 0;

    public:
        virtual bool createDirectory( const FilePath & _folderPath ) const = 0;

    public:
        virtual bool findFiles( const FilePath & _folderPath, const Char * _mask, const LambdaFilePath & _lambda ) const = 0;

    public:
        virtual InputStreamInterfacePtr createInputFile( const FilePath & _filePath, bool _streaming, FileGroupInterface ** _fileGroup, const Char * _doc ) = 0;
        virtual bool openInputFile( const FilePath & _filePath, const InputStreamInterfacePtr & _stream, size_t _offset, size_t _size, bool _streaming ) = 0;

    public:
        virtual OutputStreamInterfacePtr createOutputFile( const Char * _doc ) = 0;
        virtual bool openOutputFile( const FilePath & _filePath, const OutputStreamInterfacePtr & _stream ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<FileGroupInterface> FileGroupInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}