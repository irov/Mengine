#include "PersistentConfig.h"

#include "Interface/FileServiceInterface.h"

#include "Kernel/Exception.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/AssertionNotImplemented.h"
#include "Kernel/Logger.h"
#include "Kernel/DocumentHelper.h"
#include "Kernel/Stringalized.h"
#include "Kernel/ArrayString.h"
#include "Kernel/FilePathHelper.h"
#include "Kernel/MemoryStreamHelper.h"
#include "Kernel/ThreadMutexScope.h"

#include "Config/StdIO.h"
#include "Config/StdString.h"

//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static FilePath makePersistentPropertyFilePath( const Char * _section, const Char * _key )
        {
            ArrayString<MENGINE_MAX_PATH> propertyPath;
            propertyPath += ".persistentconfig";
            propertyPath += MENGINE_PATH_DELIM;
            propertyPath += _section;
            propertyPath += '.';
            propertyPath += _key;
            propertyPath += ".property";

            FilePath cs_propertyPath = Helper::stringizeFilePath( propertyPath );

            return cs_propertyPath;
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        static bool hasValueT( const Char * _section, const Char * _key, T _default, T * const _value, const DocumentPtr & _doc )
        {
            FileGroupInterfacePtr userFileGroup;
            if( FILE_SERVICE()
                ->hasFileGroup( STRINGIZE_STRING_LOCAL( "user" ), &userFileGroup ) == false )
            {
                *_value = _default;

                return false;
            }

            FilePath propertyPath = Detail::makePersistentPropertyFilePath( _section, _key );

            if( userFileGroup->existFile( propertyPath, false ) == false )
            {
                *_value = _default;

                return false;
            }

            FileGroupInterface * fileGroup;
            InputStreamInterfacePtr stream = userFileGroup->createInputFile( propertyPath, false, &fileGroup, _doc );

            MENGINE_ASSERTION_MEMORY_PANIC( stream );

            if( fileGroup->openInputFile( propertyPath, stream, 0, MENGINE_UNKNOWN_SIZE, false, true ) == false )
            {
                *_value = _default;

                return false;
            }

            MemoryInterfacePtr memory = Helper::createMemoryStreamString( stream, _doc );

            MENGINE_ASSERTION_MEMORY_PANIC( memory );

            const Char * memory_buffer = memory->getBuffer();

            if( Helper::stringalized( memory_buffer, _value ) == false )
            {
                *_value = _default;

                return false;
            }

            fileGroup->closeInputFile( stream );

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        template<class T>
        static bool setValueT( const Char * _section, const Char * _key, T _value, const DocumentPtr & _doc )
        {
            FileGroupInterfacePtr userFileGroup;
            if( FILE_SERVICE()
                ->hasFileGroup( STRINGIZE_STRING_LOCAL( "user" ), &userFileGroup ) == false )
            {
                return false;
            }

            FilePath propertyPath = Detail::makePersistentPropertyFilePath( _section, _key );

            OutputStreamInterfacePtr stream = userFileGroup->createOutputFile( _doc );

            MENGINE_ASSERTION_MEMORY_PANIC( stream );

            if( userFileGroup->openOutputFile( propertyPath, stream, false ) == false )
            {
                return false;
            }

            Char buffer[1024] = {'\0'};
            if( Helper::stringalized( _value, buffer, 1024 ) == false )
            {
                return false;
            }

            size_t buffer_size = MENGINE_STRLEN( buffer );

            if( stream->write( buffer, buffer_size ) != buffer_size )
            {
                return false;
            }

            userFileGroup->closeOutputFile( stream );

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    PersistentConfig::PersistentConfig()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    PersistentConfig::~PersistentConfig()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void PersistentConfig::setMutex( const ThreadMutexInterfacePtr & _mutex )
    {
        m_mutex = _mutex;
    }
    //////////////////////////////////////////////////////////////////////////
    const ThreadMutexInterfacePtr & PersistentConfig::getMutex() const
    {
        return m_mutex;
    }
    //////////////////////////////////////////////////////////////////////////
    void PersistentConfig::setPlatformTags( const Tags & _platformTags )
    {
        m_platformTags = _platformTags;
    }
    //////////////////////////////////////////////////////////////////////////
    const Tags & PersistentConfig::getPlatformTags() const
    {
        return m_platformTags;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PersistentConfig::existValue( const Char * _section, const Char * _key ) const
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        FileGroupInterfacePtr userFileGroup;

        if( FILE_SERVICE()
            ->hasFileGroup( STRINGIZE_STRING_LOCAL( "user" ), &userFileGroup ) == false )
        {
            return false;
        }

        FilePath propertyPath = Detail::makePersistentPropertyFilePath( _section, _key );

        if( userFileGroup->existFile( propertyPath, false ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool PersistentConfig::load( const InputStreamInterfacePtr & _stream, const DocumentPtr & _doc )
    {
        MENGINE_UNUSED( _stream );
        MENGINE_UNUSED( _doc );

        MENGINE_ASSERTION_NOT_IMPLEMENTED();

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void PersistentConfig::unload()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    bool PersistentConfig::hasValue( const Char * _section, const Char * _key, bool _default, bool * const _value ) const
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        return Detail::hasValueT( _section, _key, _default, _value, MENGINE_DOCUMENT_FACTORABLE );
    }
    //////////////////////////////////////////////////////////////////////////
    bool PersistentConfig::hasValue( const Char * _section, const Char * _key, int8_t _default, int8_t * const _value ) const
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        return Detail::hasValueT( _section, _key, _default, _value, MENGINE_DOCUMENT_FACTORABLE );
    }
    //////////////////////////////////////////////////////////////////////////
    bool PersistentConfig::hasValue( const Char * _section, const Char * _key, uint8_t _default, uint8_t * const _value ) const
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        return Detail::hasValueT( _section, _key, _default, _value, MENGINE_DOCUMENT_FACTORABLE );
    }
    //////////////////////////////////////////////////////////////////////////
    bool PersistentConfig::hasValue( const Char * _section, const Char * _key, int32_t _default, int32_t * const _value ) const
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        return Detail::hasValueT( _section, _key, _default, _value, MENGINE_DOCUMENT_FACTORABLE );
    }
    //////////////////////////////////////////////////////////////////////////
    bool PersistentConfig::hasValue( const Char * _section, const Char * _key, uint32_t _default, uint32_t * const _value ) const
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        return Detail::hasValueT( _section, _key, _default, _value, MENGINE_DOCUMENT_FACTORABLE );
    }
    //////////////////////////////////////////////////////////////////////////
    bool PersistentConfig::hasValue( const Char * _section, const Char * _key, int64_t _default, int64_t * const  _value ) const
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        return Detail::hasValueT( _section, _key, _default, _value, MENGINE_DOCUMENT_FACTORABLE );
    }
    //////////////////////////////////////////////////////////////////////////
    bool PersistentConfig::hasValue( const Char * _section, const Char * _key, uint64_t _default, uint64_t * const  _value ) const
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        return Detail::hasValueT( _section, _key, _default, _value, MENGINE_DOCUMENT_FACTORABLE );
    }
    //////////////////////////////////////////////////////////////////////////
    bool PersistentConfig::hasValue( const Char * _section, const Char * _key, float _default, float * const _value ) const
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        return Detail::hasValueT( _section, _key, _default, _value, MENGINE_DOCUMENT_FACTORABLE );
    }
    //////////////////////////////////////////////////////////////////////////
    bool PersistentConfig::hasValue( const Char * _section, const Char * _key, double _default, double * const _value ) const
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        return Detail::hasValueT( _section, _key, _default, _value, MENGINE_DOCUMENT_FACTORABLE );
    }
    //////////////////////////////////////////////////////////////////////////
    bool PersistentConfig::hasValue( const Char * _section, const Char * _key, const Char * _default, const Char ** const _value ) const
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        return Detail::hasValueT( _section, _key, _default, _value, MENGINE_DOCUMENT_FACTORABLE );
    }
    //////////////////////////////////////////////////////////////////////////
    bool PersistentConfig::hasValue( const Char * _section, const Char * _key, const ConstString & _default, ConstString * const _value ) const
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        return Detail::hasValueT( _section, _key, _default, _value, MENGINE_DOCUMENT_FACTORABLE );
    }
    //////////////////////////////////////////////////////////////////////////
    bool PersistentConfig::hasValue( const Char * _section, const Char * _key, const FilePath & _default, FilePath * const _value ) const
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        return Detail::hasValueT( _section, _key, _default, _value, MENGINE_DOCUMENT_FACTORABLE );
    }
    //////////////////////////////////////////////////////////////////////////
    bool PersistentConfig::hasValue( const Char * _section, const Char * _key, const Tags & _default, Tags * const _value ) const
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        return Detail::hasValueT( _section, _key, _default, _value, MENGINE_DOCUMENT_FACTORABLE );
    }
    //////////////////////////////////////////////////////////////////////////
    bool PersistentConfig::hasValue( const Char * _section, const Char * _key, const Resolution & _default, Resolution * const _value ) const
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        return Detail::hasValueT( _section, _key, _default, _value, MENGINE_DOCUMENT_FACTORABLE );
    }
    //////////////////////////////////////////////////////////////////////////
    bool PersistentConfig::hasValue( const Char * _section, const Char * _key, const Color & _default, Color * const _value ) const
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        return Detail::hasValueT( _section, _key, _default, _value, MENGINE_DOCUMENT_FACTORABLE );
    }
    //////////////////////////////////////////////////////////////////////////
    bool PersistentConfig::setValue( const Char * _section, const Char * _key, bool _value )
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        return Detail::setValueT( _section, _key, _value, MENGINE_DOCUMENT_FACTORABLE );
    }
    //////////////////////////////////////////////////////////////////////////
    bool PersistentConfig::setValue( const Char * _section, const Char * _key, int8_t _value )
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        return Detail::setValueT( _section, _key, _value, MENGINE_DOCUMENT_FACTORABLE );
    }
    //////////////////////////////////////////////////////////////////////////
    bool PersistentConfig::setValue( const Char * _section, const Char * _key, uint8_t _value )
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        return Detail::setValueT( _section, _key, _value, MENGINE_DOCUMENT_FACTORABLE );
    }
    //////////////////////////////////////////////////////////////////////////
    bool PersistentConfig::setValue( const Char * _section, const Char * _key, int32_t _value )
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        return Detail::setValueT( _section, _key, _value, MENGINE_DOCUMENT_FACTORABLE );
    }
    //////////////////////////////////////////////////////////////////////////
    bool PersistentConfig::setValue( const Char * _section, const Char * _key, uint32_t _value )
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        return Detail::setValueT( _section, _key, _value, MENGINE_DOCUMENT_FACTORABLE );
    }
    //////////////////////////////////////////////////////////////////////////
    bool PersistentConfig::setValue( const Char * _section, const Char * _key, int64_t _value )
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        return Detail::setValueT( _section, _key, _value, MENGINE_DOCUMENT_FACTORABLE );
    }
    //////////////////////////////////////////////////////////////////////////
    bool PersistentConfig::setValue( const Char * _section, const Char * _key, uint64_t _value )
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        return Detail::setValueT( _section, _key, _value, MENGINE_DOCUMENT_FACTORABLE );
    }
    //////////////////////////////////////////////////////////////////////////
    bool PersistentConfig::setValue( const Char * _section, const Char * _key, float _value )
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        return Detail::setValueT( _section, _key, _value, MENGINE_DOCUMENT_FACTORABLE );
    }
    //////////////////////////////////////////////////////////////////////////
    bool PersistentConfig::setValue( const Char * _section, const Char * _key, double _value )
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        return Detail::setValueT( _section, _key, _value, MENGINE_DOCUMENT_FACTORABLE );
    }
    //////////////////////////////////////////////////////////////////////////
    bool PersistentConfig::setValue( const Char * _section, const Char * _key, const Char * _value )
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        return Detail::setValueT( _section, _key, _value, MENGINE_DOCUMENT_FACTORABLE );
    }
    //////////////////////////////////////////////////////////////////////////
    bool PersistentConfig::setValue( const Char * _section, const Char * _key, const ConstString & _value )
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        return Detail::setValueT( _section, _key, _value, MENGINE_DOCUMENT_FACTORABLE );
    }
    //////////////////////////////////////////////////////////////////////////
    bool PersistentConfig::setValue( const Char * _section, const Char * _key, const FilePath & _value )
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        return Detail::setValueT( _section, _key, _value, MENGINE_DOCUMENT_FACTORABLE );
    }
    //////////////////////////////////////////////////////////////////////////
    bool PersistentConfig::setValue( const Char * _section, const Char * _key, const Tags & _value )
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        return Detail::setValueT( _section, _key, _value, MENGINE_DOCUMENT_FACTORABLE );
    }
    //////////////////////////////////////////////////////////////////////////
    bool PersistentConfig::setValue( const Char * _section, const Char * _key, const Resolution & _value )
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        return Detail::setValueT( _section, _key, _value, MENGINE_DOCUMENT_FACTORABLE );
    }
    //////////////////////////////////////////////////////////////////////////
    bool PersistentConfig::setValue( const Char * _section, const Char * _key, const Color & _value )
    {
        MENGINE_THREAD_MUTEX_SCOPE( m_mutex );

        return Detail::setValueT( _section, _key, _value, MENGINE_DOCUMENT_FACTORABLE );
    }
    //////////////////////////////////////////////////////////////////////////
    void PersistentConfig::getValues( const Char * _section, const Char * _key, VectorAspectRatioViewports * const _values ) const
    {
        MENGINE_UNUSED( _section );
        MENGINE_UNUSED( _key );
        MENGINE_UNUSED( _values );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void PersistentConfig::getValues( const Char * _section, const Char * _key, VectorFilePath * const _values ) const
    {
        MENGINE_UNUSED( _section );
        MENGINE_UNUSED( _key );
        MENGINE_UNUSED( _values );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void PersistentConfig::getValues( const Char * _section, const Char * _key, VectorConstString * const _values ) const
    {
        MENGINE_UNUSED( _section );
        MENGINE_UNUSED( _key );
        MENGINE_UNUSED( _values );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    void PersistentConfig::getValues( const Char * _section, const Char * _key, VectorString * const _values ) const
    {
        MENGINE_UNUSED( _section );
        MENGINE_UNUSED( _key );
        MENGINE_UNUSED( _values );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    bool PersistentConfig::hasSection( const Char * _section ) const
    {
        MENGINE_UNUSED( _section );

        //Empty

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
}
