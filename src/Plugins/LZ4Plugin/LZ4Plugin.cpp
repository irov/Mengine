#include "LZ4Plugin.h"

#include "Interface/VocabularyServiceInterface.h"

#include "ArchivatorLZ4.h"

#include "Kernel/FactorableUnique.h"
#include "Kernel/ConstStringHelper.h"
#include "Kernel/AssertionAllocator.h"

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
}