#include "LZ4Plugin.h"

#include "ArchivatorLZ4.h"

#include "Kernel/FactorableUnique.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/AssertionAllocator.h"
#include "Kernel/OptionHelper.h"
#include "Kernel/ConfigHelper.h"
#include "Kernel/VocabularyHelper.h"
#include "Kernel/PluginHelper.h"

//////////////////////////////////////////////////////////////////////////
extern "C"
{
    //////////////////////////////////////////////////////////////////////////
    void * LZ4_malloc( size_t _size )
    {
        void * p = Mengine::Helper::allocateMemory( _size, "lz4" );

        return p;
    }
    //////////////////////////////////////////////////////////////////////////
    void * LZ4_calloc( size_t _count, size_t _size )
    {
        void * p = Mengine::Helper::callocateMemory( _count, _size, "lz4" );

        return p;
    }
    //////////////////////////////////////////////////////////////////////////
    void  LZ4_free( void * _ptr )
    {
        Mengine::Helper::deallocateMemory( _ptr, "lz4" );
    }
    //////////////////////////////////////////////////////////////////////////
}
//////////////////////////////////////////////////////////////////////////
PLUGIN_FACTORY( LZ4, Mengine::LZ4Plugin );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    LZ4Plugin::LZ4Plugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    LZ4Plugin::~LZ4Plugin()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool LZ4Plugin::_availablePlugin() const
    {
        if( HAS_OPTION( "nolz4" ) == true )
        {
            return false;
        }

        bool LZ4Plugin_Available = CONFIG_VALUE_BOOLEAN( "LZ4Plugin", "Available", true );

        if( LZ4Plugin_Available == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool LZ4Plugin::_initializePlugin()
    {
        VOCABULARY_SET( ArchivatorInterface, STRINGIZE_STRING_LOCAL( "Archivator" ), STRINGIZE_STRING_LOCAL( "lz4" ), Helper::makeFactorableUnique<ArchivatorLZ4>( MENGINE_DOCUMENT_FACTORABLE ), MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void LZ4Plugin::_finalizePlugin()
    {
        VOCABULARY_REMOVE( STRINGIZE_STRING_LOCAL( "Archivator" ), STRINGIZE_STRING_LOCAL( "lz4" ) );
    }
    //////////////////////////////////////////////////////////////////////////
    void LZ4Plugin::_destroyPlugin()
    {
        MENGINE_ASSERTION_ALLOCATOR( "lz4" );
    }
    //////////////////////////////////////////////////////////////////////////
}