#include "MCPFileGroup.h"

#include "Interface/MemoryServiceInterface.h"

#include "Config/StdString.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    MCPFileGroup::MCPFileGroup()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    MCPFileGroup::~MCPFileGroup()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool MCPFileGroup::_initialize()
    {
        bool successful = m_baseFileGroup != nullptr;

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPFileGroup::_finalize()
    {
        m_overlays.clear();
        m_overlayStreams.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPFileGroup::setOverlay( const FilePath & _filePath, const Data & _data )
    {
        const Char * filePath = _filePath.c_str();
        m_overlays[filePath] = _data;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MCPFileGroup::getOverlay( const FilePath & _filePath, Data * const _data ) const
    {
        const Char * filePath = _filePath.c_str();
        Map<String, Data>::const_iterator it = m_overlays.find( filePath );

        if( it == m_overlays.end() )
        {
            return false;
        }

        *_data = it->second;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MCPFileGroup::hasOverlay( const FilePath & _filePath ) const
    {
        const Char * filePath = _filePath.c_str();
        bool result = m_overlays.find( filePath ) != m_overlays.end();

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPFileGroup::removeOverlay( const FilePath & _filePath )
    {
        const Char * filePath = _filePath.c_str();
        m_overlays.erase( filePath );
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPFileGroup::clearOverlays()
    {
        m_overlays.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    bool MCPFileGroup::emptyOverlays() const
    {
        bool empty = m_overlays.empty();

        return empty;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MCPFileGroup::isPacked() const
    {
        bool packed = m_baseFileGroup->isPacked();

        return packed;
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPFileGroup::getFullPath( const FilePath & _filePath, Char * const _fullPath ) const
    {
        m_baseFileGroup->getFullPath( _filePath, _fullPath );
    }
    //////////////////////////////////////////////////////////////////////////
    bool MCPFileGroup::existDirectory( const FilePath & _folderPath, bool _recursive ) const
    {
        bool exist = m_baseFileGroup->existDirectory( _folderPath, _recursive );

        return exist;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MCPFileGroup::createDirectory( const FilePath & _folderPath ) const
    {
        bool successful = m_baseFileGroup->createDirectory( _folderPath );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MCPFileGroup::existFile( const FilePath & _filePath, bool _recursive ) const
    {
        bool exist = this->hasOverlay( _filePath ) == true || m_baseFileGroup->existFile( _filePath, _recursive );

        return exist;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MCPFileGroup::removeFile( const FilePath & _filePath ) const
    {
        bool successful = m_baseFileGroup->removeFile( _filePath );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MCPFileGroup::moveFile( const FilePath & _oldFilePath, const FilePath & _newFilePath ) const
    {
        bool successful = m_baseFileGroup->moveFile( _oldFilePath, _newFilePath );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MCPFileGroup::findFiles( const FilePath & _folderPath, const Char * _mask, const LambdaFilePath & _lambda ) const
    {
        bool successful = m_baseFileGroup->findFiles( _folderPath, _mask, _lambda );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    InputStreamInterfacePtr MCPFileGroup::createInputFile( const FilePath & _filePath, bool _streaming, FileGroupInterface ** const _fileGroup, const DocumentInterfacePtr & _doc )
    {
        if( this->hasOverlay( _filePath ) == false )
        {
            InputStreamInterfacePtr stream = m_baseFileGroup->createInputFile( _filePath, _streaming, _fileGroup, _doc );

            return stream;
        }

        MemoryInputInterfacePtr memory = MEMORY_SERVICE()->createMemoryInput( _doc );
        MemoryInputInterface * memoryValue = memory.get();
        const Char * filePath = _filePath.c_str();
        m_overlayStreams[memoryValue] = filePath;
        *_fileGroup = this;

        return memory;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MCPFileGroup::openOverlayStream_( const FilePath & _filePath, const InputStreamInterfacePtr & _stream, size_t _offset, size_t _size )
    {
        const Char * filePath = _filePath.c_str();
        Map<String, Data>::const_iterator it = m_overlays.find( filePath );

        if( it == m_overlays.end() || _offset > it->second.size() )
        {
            return false;
        }

        size_t available = it->second.size() - _offset;
        size_t size = _size == MENGINE_UNKNOWN_SIZE ? available : _size;
        if( size > available )
        {
            return false;
        }

        InputStreamInterface * stream = _stream.get();
        MemoryInputInterface * memory = static_cast<MemoryInputInterface *>(stream);
        void * buffer = memory->newBuffer( size );
        if( size != 0 )
        {
            const uint8_t * data = it->second.data();
            const uint8_t * source = data + _offset;
            StdString::memcpy( buffer, source, size );
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MCPFileGroup::openInputFile( const FilePath & _filePath, const InputStreamInterfacePtr & _stream, size_t _offset, size_t _size, bool _streaming, bool _share )
    {
        InputStreamInterface * streamValue = _stream.get();

        if( m_overlayStreams.find( streamValue ) != m_overlayStreams.end() )
        {
            bool successful = this->openOverlayStream_( _filePath, _stream, _offset, _size );

            return successful;
        }

        bool successful = m_baseFileGroup->openInputFile( _filePath, _stream, _offset, _size, _streaming, _share );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPFileGroup::closeInputFile( const InputStreamInterfacePtr & _stream )
    {
        InputStreamInterface * streamValue = _stream.get();

        if( m_overlayStreams.erase( streamValue ) != 0 )
        {
            return;
        }

        m_baseFileGroup->closeInputFile( _stream );
    }
    //////////////////////////////////////////////////////////////////////////
    InputStreamInterfacePtr MCPFileGroup::createInputMutexFile( const FilePath & _filePath, const InputStreamInterfacePtr & _stream, const ThreadMutexInterfacePtr & _mutex, FileGroupInterface ** const _fileGroup, const DocumentInterfacePtr & _doc )
    {
        if( this->hasOverlay( _filePath ) == true )
        {
            InputStreamInterfacePtr stream = this->createInputFile( _filePath, false, _fileGroup, _doc );

            return stream;
        }

        InputStreamInterfacePtr stream = m_baseFileGroup->createInputMutexFile( _filePath, _stream, _mutex, _fileGroup, _doc );

        return stream;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MCPFileGroup::openInputMutexFile( const FilePath & _filePath, const InputStreamInterfacePtr & _stream, size_t _offset, size_t _size )
    {
        InputStreamInterface * streamValue = _stream.get();

        if( m_overlayStreams.find( streamValue ) != m_overlayStreams.end() )
        {
            bool successful = this->openOverlayStream_( _filePath, _stream, _offset, _size );

            return successful;
        }

        bool successful = m_baseFileGroup->openInputMutexFile( _filePath, _stream, _offset, _size );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPFileGroup::closeInputMutexFile( const InputStreamInterfacePtr & _stream )
    {
        InputStreamInterface * streamValue = _stream.get();

        if( m_overlayStreams.erase( streamValue ) != 0 )
        {
            return;
        }

        m_baseFileGroup->closeInputMutexFile( _stream );
    }
    //////////////////////////////////////////////////////////////////////////
    OutputStreamInterfacePtr MCPFileGroup::createOutputFile( const DocumentInterfacePtr & _doc )
    {
        OutputStreamInterfacePtr stream = m_baseFileGroup->createOutputFile( _doc );

        return stream;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MCPFileGroup::openOutputFile( const FilePath & _filePath, const OutputStreamInterfacePtr & _stream, bool _withTemp )
    {
        bool successful = m_baseFileGroup->openOutputFile( _filePath, _stream, _withTemp );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MCPFileGroup::closeOutputFile( const OutputStreamInterfacePtr & _stream )
    {
        bool successful = m_baseFileGroup->closeOutputFile( _stream );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MCPFileGroup::isAvailableMappedFile() const
    {
        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    MappedInterfacePtr MCPFileGroup::createMappedFile( const FilePath & _filePath, FileGroupInterface ** const _fileGroup, const DocumentInterfacePtr & _doc )
    {
        if( this->hasOverlay( _filePath ) == true )
        {
            return nullptr;
        }

        MappedInterfacePtr mapped = m_baseFileGroup->createMappedFile( _filePath, _fileGroup, _doc );

        return mapped;
    }
    //////////////////////////////////////////////////////////////////////////
    bool MCPFileGroup::openMappedFile( const FilePath & _filePath, const MappedInterfacePtr & _stream, bool _shared )
    {
        bool successful = m_baseFileGroup->openMappedFile( _filePath, _stream, _shared );

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    void MCPFileGroup::closeMappedFile( const MappedInterfacePtr & _stream )
    {
        m_baseFileGroup->closeMappedFile( _stream );
    }
    //////////////////////////////////////////////////////////////////////////
}
