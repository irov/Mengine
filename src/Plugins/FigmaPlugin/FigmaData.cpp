#include "FigmaData.h"

#include "Kernel/AllocatorHelper.h"
#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static const Char * getFigmaResultMessage( figma_result_t _result )
        {
            switch( _result )
            {
            case FIGMA_RESULT_OK:
                return "Ok";
            case FIGMA_RESULT_INVALID_ARGUMENT:
                return "InvalidArgument";
            case FIGMA_RESULT_OUT_OF_MEMORY:
                return "OutOfMemory";
            case FIGMA_RESULT_IO_FAILED:
                return "IoFailed";
            case FIGMA_RESULT_PARSE_FAILED:
                return "ParseFailed";
            case FIGMA_RESULT_UNSUPPORTED_FORMAT:
                return "UnsupportedFormat";
            case FIGMA_RESULT_MISSING_ENTRY:
                return "MissingEntry";
            case FIGMA_RESULT_NOT_FOUND:
                return "NotFound";
            case FIGMA_RESULT_INVALID_STATE:
                return "InvalidState";
            case FIGMA_RESULT_VERSION_MISMATCH:
                return "VersionMismatch";
            }

            return "Unknown";
        }
        //////////////////////////////////////////////////////////////////////////
        static void * FIGMA_CALL figmaAlloc( size_t _size, void * _userData )
        {
            MENGINE_UNUSED( _userData );

            void * p = Helper::allocateMemory( _size, "figma" );

            return p;
        }
        //////////////////////////////////////////////////////////////////////////
        static void * FIGMA_CALL figmaRealloc( void * _ptr, size_t _size, void * _userData )
        {
            MENGINE_UNUSED( _userData );

            void * p = Helper::reallocateMemory( _ptr, _size, "figma" );

            return p;
        }
        //////////////////////////////////////////////////////////////////////////
        static void FIGMA_CALL figmaFree( void * _ptr, void * _userData )
        {
            MENGINE_UNUSED( _userData );

            Helper::deallocateMemory( _ptr, "figma" );
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    FigmaData::FigmaData()
        : m_runtime( nullptr )
        , m_document( nullptr )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    FigmaData::~FigmaData()
    {
        this->release();
    }
    //////////////////////////////////////////////////////////////////////////
    bool FigmaData::loadDocument( const MemoryInterfacePtr & _memory, const FilePath & _sourceName )
    {
        figma_runtime_desc_t runtimeDesc{};
        runtimeDesc.allocator.alloc = &Detail::figmaAlloc;
        runtimeDesc.allocator.realloc = &Detail::figmaRealloc;
        runtimeDesc.allocator.free = &Detail::figmaFree;
        runtimeDesc.allocator.user_data = nullptr;

        figma_runtime_t * runtime = nullptr;
        figma_result_t createRuntimeResult = figma_runtime_create( FIGMA_SDK_VERSION, &runtimeDesc, &runtime );

        if( createRuntimeResult != FIGMA_RESULT_OK )
        {
            LOGGER_ERROR( "invalid create figma runtime result '%s'"
                , Detail::getFigmaResultMessage( createRuntimeResult )
            );

            return false;
        }

        void * memory = _memory->getBuffer();
        size_t size = _memory->getSize();

        figma_load_options_t loadOptions{};
        loadOptions.source_name = {_sourceName.c_str(), _sourceName.size()};
        loadOptions.extract_image_assets = FIGMA_TRUE;
        loadOptions.keep_canvas_bytes = FIGMA_FALSE;

        figma_document_t * document = nullptr;
        figma_result_t loadResult = figma_runtime_load_document_from_fig_data( runtime, memory, size, &loadOptions, &document );

        if( loadResult != FIGMA_RESULT_OK )
        {
            LOGGER_ERROR( "invalid load figma document '%s' result '%s'"
                , _sourceName.c_str()
                , Detail::getFigmaResultMessage( loadResult )
            );

            figma_runtime_destroy( runtime );

            return false;
        }

        m_memory = _memory;
        m_runtime = runtime;
        m_document = document;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FigmaData::loadUX( const void * _memory, size_t _size )
    {
        if( m_document == nullptr )
        {
            return false;
        }

        figma_string_view_t ux{static_cast<const char *>(_memory), _size};
        figma_result_t result = figma_document_load_ux( m_document, ux );

        if( result != FIGMA_RESULT_OK )
        {
            LOGGER_ERROR( "invalid load figma UX result '%s'"
                , Detail::getFigmaResultMessage( result )
            );

            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool FigmaData::acquire()
    {
        return m_document != nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    void FigmaData::release()
    {
        if( m_document != nullptr )
        {
            figma_document_destroy( m_document );
            m_document = nullptr;
        }

        if( m_runtime != nullptr )
        {
            figma_runtime_destroy( m_runtime );
            m_runtime = nullptr;
        }

        m_memory = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    figma_runtime_t * FigmaData::getFigmaRuntime() const
    {
        return m_runtime;
    }
    //////////////////////////////////////////////////////////////////////////
    figma_document_t * FigmaData::getFigmaDocument() const
    {
        return m_document;
    }
    //////////////////////////////////////////////////////////////////////////
}
