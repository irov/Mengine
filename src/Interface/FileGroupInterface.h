#pragma once

#include "Interface/ServantInterface.h"
#include "Interface/InputStreamInterface.h"
#include "Interface/OutputStreamInterface.h"
#include "Interface/MappedInterface.h"

#include "Kernel/Mixin.h"
#include "Kernel/ConstString.h"
#include "Kernel/Document.h"
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
        virtual void getFullPath( const FilePath & _filePath, Char * const _fullPath ) const = 0;

    public:
        virtual bool isPacked() const = 0;

    public:
        virtual bool existDirectory( const FilePath & _folderPath, bool _recursive ) const = 0;
        virtual bool createDirectory( const FilePath & _folderPath ) const = 0;

    public:
        virtual bool existFile( const FilePath & _filePath, bool _recursive ) const = 0;
        virtual bool removeFile( const FilePath & _filePath ) const = 0;
        virtual bool moveFile( const FilePath & _oldFilePath, const FilePath & _newFilePath ) const = 0;

    public:
        virtual bool findFiles( const FilePath & _folderPath, const Char * _mask, const LambdaFilePath & _lambda ) const = 0;

    public:
        virtual InputStreamInterfacePtr createInputFile( const FilePath & _filePath, bool _streaming, FileGroupInterface ** const _fileGroup, const DocumentPtr & _doc ) = 0;
        virtual bool openInputFile( const FilePath & _filePath, const InputStreamInterfacePtr & _stream, size_t _offset, size_t _size, bool _streaming, bool _share ) = 0;
        virtual bool closeInputFile( const InputStreamInterfacePtr & _stream ) = 0;

    public:
        virtual OutputStreamInterfacePtr createOutputFile( const DocumentPtr & _doc ) = 0;
        virtual bool openOutputFile( const FilePath & _filePath, const OutputStreamInterfacePtr & _stream ) = 0;
        virtual bool closeOutputFile( const OutputStreamInterfacePtr & _stream ) = 0;

    public:
        virtual bool isAvailableMappedFile() const = 0;

    public:
        virtual MappedInterfacePtr createMappedFile( const FilePath & _filePath, FileGroupInterface ** const _fileGroup, const DocumentPtr & _doc ) = 0;
        virtual bool openMappedFile( const FilePath & _filePath, const MappedInterfacePtr & _stream, bool _shared ) = 0;
        virtual bool closeMappedFile( const MappedInterfacePtr & _stream ) = 0;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<FileGroupInterface> FileGroupInterfacePtr;
    //////////////////////////////////////////////////////////////////////////
}