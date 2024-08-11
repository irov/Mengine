#include "Win32PreferencesSystem.h"

#include "Interface/FileServiceInterface.h"

#include "Kernel/FilePath.h"
#include "Kernel/PathString.h"
#include "Kernel/FilePathHelper.h"
#include "Kernel/AssertionMemoryPanic.h"
#include "Kernel/MemoryStreamHelper.h"
#include "Kernel/Stringalized.h"
#include "Kernel/VocabularyHelper.h"

#include "Config/StdString.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( PreferencesSystem, Mengine::Win32PreferencesSystem );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static FilePath makePreferencesPropertyFilePath( const Char * _key )
        {
            PathString propertyPath;
            propertyPath += "preferences";
            propertyPath += MENGINE_PATH_DELIM;
            propertyPath += _key;
            propertyPath += ".property";

            FilePath cs_propertyPath = Helper::stringizeFilePath( propertyPath );

            return cs_propertyPath;
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    Win32PreferencesSystem::Win32PreferencesSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    Win32PreferencesSystem::~Win32PreferencesSystem()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32PreferencesSystem::_initializeService()
    {
        //Empty

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32PreferencesSystem::_finalizeService()
    {
        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
    int64_t Win32PreferencesSystem::getPreferenceInteger( const Char * _key, int64_t _default ) const
    {
        if( SERVICE_IS_INITIALIZE( FileServiceInterface ) == false )
        {
            return _default;
        }

        FileGroupInterfacePtr userFileGroup = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "FileGroup" ), STRINGIZE_STRING_LOCAL( "user" ) );

        MENGINE_ASSERTION_MEMORY_PANIC( userFileGroup, "invalid get user file group" );

        FilePath propertyPath = Detail::makePreferencesPropertyFilePath( _key );

        if( userFileGroup->existFile( propertyPath, false ) == false )
        {
            return _default;
        }

        MemoryInterfacePtr preferenceMemory = Helper::createMemoryFileString( userFileGroup, propertyPath, false, true, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( preferenceMemory, "invalid create memory file '%s'"
            , propertyPath.c_str()
        );

        const Char * memory_buffer = preferenceMemory->getBuffer();

        int64_t value;
        if( Helper::stringalized( memory_buffer, &value ) == false )
        {
            return _default;
        }

        return value;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32PreferencesSystem::setPreferenceInteger( const Char * _key, int64_t _value )
    {
        if( SERVICE_IS_INITIALIZE( FileServiceInterface ) == false )
        {
            return false;
        }

        FileGroupInterfacePtr userFileGroup = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "FileGroup" ), STRINGIZE_STRING_LOCAL( "user" ) );

        MENGINE_ASSERTION_MEMORY_PANIC( userFileGroup, "invalid get user file group" );

        FilePath propertyPath = Detail::makePreferencesPropertyFilePath( _key );

        OutputStreamInterfacePtr stream = userFileGroup->createOutputFile( MENGINE_DOCUMENT_FACTORABLE );

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

        size_t buffer_size = StdString::strlen( buffer );

        if( stream->write( buffer, buffer_size ) != buffer_size )
        {
            return false;
        }

        userFileGroup->closeOutputFile( stream );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32PreferencesSystem::getPreferenceString( const Char * _key, Char * const _value, size_t _capacity, size_t * const _size ) const
    {
        if( SERVICE_IS_INITIALIZE( FileServiceInterface ) == false )
        {
            return false;
        }

        FileGroupInterfacePtr userFileGroup = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "FileGroup" ), STRINGIZE_STRING_LOCAL( "user" ) );

        MENGINE_ASSERTION_MEMORY_PANIC( userFileGroup, "invalid get user file group" );

        FilePath propertyPath = Detail::makePreferencesPropertyFilePath( _key );

        if( userFileGroup->existFile( propertyPath, false ) == false )
        {
            return false;
        }

        MemoryInterfacePtr preferenceMemory = Helper::createMemoryFileString( userFileGroup, propertyPath, false, true, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( preferenceMemory, "invalid create memory file '%s'"
            , propertyPath.c_str()
        );

        const Char * memory_buffer = preferenceMemory->getBuffer();
        size_t memory_size = preferenceMemory->getSize();

        StdString::strncpy( _value, memory_buffer, _capacity );

        if( _size != nullptr )
        {
            *_size = memory_size;
        }

        return false;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32PreferencesSystem::setPreferenceString( const Char * _key, const Char * _value )
    {
        FileGroupInterfacePtr userFileGroup = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "FileGroup" ), STRINGIZE_STRING_LOCAL( "user" ) );

        MENGINE_ASSERTION_MEMORY_PANIC( userFileGroup, "invalid get user file group" );

        FilePath propertyPath = Detail::makePreferencesPropertyFilePath( _key );

        OutputStreamInterfacePtr stream = userFileGroup->createOutputFile( MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( stream );

        if( userFileGroup->openOutputFile( propertyPath, stream, false ) == false )
        {
            return false;
        }

        size_t value_size = StdString::strlen( _value );

        if( stream->write( _value, value_size ) != value_size )
        {
            return false;
        }

        userFileGroup->closeOutputFile( stream );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    ConstString Win32PreferencesSystem::getPreferenceConstString( const Char * _key, const ConstString & _default ) const
    {
        if( SERVICE_IS_INITIALIZE( FileServiceInterface ) == false )
        {
            return _default;
        }

        FileGroupInterfacePtr userFileGroup = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "FileGroup" ), STRINGIZE_STRING_LOCAL( "user" ) );

        MENGINE_ASSERTION_MEMORY_PANIC( userFileGroup, "invalid get user file group" );

        FilePath propertyPath = Detail::makePreferencesPropertyFilePath( _key );

        if( userFileGroup->existFile( propertyPath, false ) == false )
        {
            return _default;
        }

        MemoryInterfacePtr preferenceMemory = Helper::createMemoryFileString( userFileGroup, propertyPath, false, true, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( preferenceMemory, "invalid create memory file '%s'"
            , propertyPath.c_str()
        );

        const Char * memory_buffer = preferenceMemory->getBuffer();

        ConstString value = Helper::stringizeString( memory_buffer );

        return value;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32PreferencesSystem::setPreferenceConstString( const Char * _key, const ConstString & _value )
    {
        if( SERVICE_IS_INITIALIZE( FileServiceInterface ) == false )
        {
            return false;
        }

        FileGroupInterfacePtr userFileGroup = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "FileGroup" ), STRINGIZE_STRING_LOCAL( "user" ) );

        MENGINE_ASSERTION_MEMORY_PANIC( userFileGroup, "invalid get user file group" );

        FilePath propertyPath = Detail::makePreferencesPropertyFilePath( _key );

        OutputStreamInterfacePtr stream = userFileGroup->createOutputFile( MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( stream );

        if( userFileGroup->openOutputFile( propertyPath, stream, false ) == false )
        {
            return false;
        }

        const Char * value_str = _value.c_str();
        ConstString::size_type value_size = _value.size();

        if( stream->write( value_str, value_size ) != value_size )
        {
            return false;
        }

        userFileGroup->closeOutputFile( stream );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool Win32PreferencesSystem::hasPreference( const Char * _key ) const
    {
        if( SERVICE_IS_INITIALIZE( FileServiceInterface ) == false )
        {
            return false;
        }

        FileGroupInterfacePtr userFileGroup = VOCABULARY_GET( STRINGIZE_STRING_LOCAL( "FileGroup" ), STRINGIZE_STRING_LOCAL( "user" ) );

        MENGINE_ASSERTION_MEMORY_PANIC( userFileGroup, "invalid get user file group" );

        FilePath propertyPath = Detail::makePreferencesPropertyFilePath( _key );

        if( userFileGroup->existFile( propertyPath, false ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void Win32PreferencesSystem::removePreference( const Char * _key )
    {
        MENGINE_UNUSED( _key );

        //Empty
    }
    //////////////////////////////////////////////////////////////////////////
}
