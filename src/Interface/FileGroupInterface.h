#pragma once

#include "Interface/ServantInterface.h"
#include "Interface/InputStreamInterface.h"
#include "Interface/OutputStreamInterface.h"

#include "Kernel/Mixin.h"
#include "Kernel/ConstString.h"
#include "Kernel/FilePath.h"

#include "Config/Lambda.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    typedef Lambda<void( const FilePath & )> LambdaFiles;
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<class FileGroupInterface> FileGroupInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
    class FileGroupInterface
        : public ServantInterface
    {
    public:
        virtual bool initialize( const ConstString & _name, const FileGroupInterfacePtr & _category, const FilePath & _path ) = 0;
        virtual void finalize() = 0;

    public:
        virtual const ConstString & getName() const = 0;
        virtual const FileGroupInterfacePtr & getCategory() const = 0;

    public:
        virtual bool isPacked() const = 0;

    public:
        virtual const FilePath & getRelationPath() const = 0;
        virtual const FilePath & getFolderPath() const = 0;

    public:
        virtual bool existFile( const FilePath & _filePath ) const = 0;
        virtual bool existDirectory( const FilePath & _folderPath ) const = 0;

    public:
        virtual bool createDirectory( const FilePath & _folderPath ) const = 0;

    public:        
        virtual bool findFiles( const FilePath & _folderPath, const Char * _mask, const LambdaFiles & _lambda ) const = 0;

    public:
        virtual InputStreamInterfacePtr createInputFile( const FilePath & _filePath, bool _streaming ) = 0;
        virtual bool openInputFile( const FilePath & _filePath, const InputStreamInterfacePtr & _stream, size_t _offset, size_t _size, bool _streaming ) = 0;

    public:
        virtual OutputStreamInterfacePtr createOutputFile() = 0;
        virtual bool openOutputFile( const FilePath & _filePath, const OutputStreamInterfacePtr & _stream ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<FileGroupInterface> FileGroupInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}


