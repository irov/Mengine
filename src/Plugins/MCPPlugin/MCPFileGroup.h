#pragma once

#include "Kernel/BaseFileGroup.h"
#include "Kernel/Data.h"
#include "Kernel/Map.h"
#include "Kernel/String.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    class MCPFileGroup
        : public BaseFileGroup
    {
        DECLARE_FACTORABLE( MCPFileGroup );

    public:
        MCPFileGroup();
        ~MCPFileGroup() override;

    public:
        bool _initialize() override;
        void _finalize() override;

    public:
        void setOverlay( const FilePath & _filePath, const Data & _data );
        bool getOverlay( const FilePath & _filePath, Data * const _data ) const;
        bool hasOverlay( const FilePath & _filePath ) const;
        void removeOverlay( const FilePath & _filePath );
        void clearOverlays();
        bool emptyOverlays() const;

    public:
        bool isPacked() const override;
        void getFullPath( const FilePath & _filePath, Char * const _fullPath ) const override;
        bool existDirectory( const FilePath & _folderPath, bool _recursive ) const override;
        bool createDirectory( const FilePath & _folderPath ) const override;
        bool existFile( const FilePath & _filePath, bool _recursive ) const override;
        bool removeFile( const FilePath & _filePath ) const override;
        bool moveFile( const FilePath & _oldFilePath, const FilePath & _newFilePath ) const override;
        bool findFiles( const FilePath & _folderPath, const Char * _mask, const LambdaFilePath & _lambda ) const override;

    public:
        InputStreamInterfacePtr createInputFile( const FilePath & _filePath, bool _streaming, FileGroupInterface ** const _fileGroup, const DocumentInterfacePtr & _doc ) override;
        bool openInputFile( const FilePath & _filePath, const InputStreamInterfacePtr & _stream, size_t _offset, size_t _size, bool _streaming, bool _share ) override;
        void closeInputFile( const InputStreamInterfacePtr & _stream ) override;
        InputStreamInterfacePtr createInputMutexFile( const FilePath & _filePath, const InputStreamInterfacePtr & _stream, const ThreadMutexInterfacePtr & _mutex, FileGroupInterface ** const _fileGroup, const DocumentInterfacePtr & _doc ) override;
        bool openInputMutexFile( const FilePath & _filePath, const InputStreamInterfacePtr & _stream, size_t _offset, size_t _size ) override;
        void closeInputMutexFile( const InputStreamInterfacePtr & _stream ) override;

    public:
        OutputStreamInterfacePtr createOutputFile( const DocumentInterfacePtr & _doc ) override;
        bool openOutputFile( const FilePath & _filePath, const OutputStreamInterfacePtr & _stream, bool _withTemp ) override;
        bool closeOutputFile( const OutputStreamInterfacePtr & _stream ) override;

    public:
        bool isAvailableMappedFile() const override;
        MappedInterfacePtr createMappedFile( const FilePath & _filePath, FileGroupInterface ** const _fileGroup, const DocumentInterfacePtr & _doc ) override;
        bool openMappedFile( const FilePath & _filePath, const MappedInterfacePtr & _stream, bool _shared ) override;
        void closeMappedFile( const MappedInterfacePtr & _stream ) override;

    protected:
        bool openOverlayStream_( const FilePath & _filePath, const InputStreamInterfacePtr & _stream, size_t _offset, size_t _size );

    protected:
        Map<String, Data> m_overlays;
        Map<InputStreamInterface *, String> m_overlayStreams;
    };
    //////////////////////////////////////////////////////////////////////////
    typedef IntrusivePtr<MCPFileGroup, FileGroupInterface> MCPFileGroupPtr;
    //////////////////////////////////////////////////////////////////////////
}
