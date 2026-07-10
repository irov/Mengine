#include "FigmaData.h"

#include "Kernel/AllocatorHelper.h"
#include "Kernel/Logger.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    namespace Detail
    {
        //////////////////////////////////////////////////////////////////////////
        static const Char * getFigmaResultMessage( ::Figma::EResult _result )
        {
            switch( _result )
            {
            case ::Figma::EResult::Ok:
                return "Ok";
            case ::Figma::EResult::InvalidArgument:
                return "InvalidArgument";
            case ::Figma::EResult::OutOfMemory:
                return "OutOfMemory";
            case ::Figma::EResult::IoFailed:
                return "IoFailed";
            case ::Figma::EResult::ParseFailed:
                return "ParseFailed";
            case ::Figma::EResult::UnsupportedFormat:
                return "UnsupportedFormat";
            case ::Figma::EResult::MissingEntry:
                return "MissingEntry";
            case ::Figma::EResult::NotFound:
                return "NotFound";
            case ::Figma::EResult::InvalidState:
                return "InvalidState";
            }

            return "Unknown";
        }
        //////////////////////////////////////////////////////////////////////////
        static void * figmaAlloc( size_t _size, void * _userData )
        {
            MENGINE_UNUSED( _userData );

            void * p = Helper::allocateMemory( _size, "figma" );

            return p;
        }
        //////////////////////////////////////////////////////////////////////////
        static void * figmaRealloc( void * _ptr, size_t _size, void * _userData )
        {
            MENGINE_UNUSED( _userData );

            void * p = Helper::reallocateMemory( _ptr, _size, "figma" );

            return p;
        }
        //////////////////////////////////////////////////////////////////////////
        static void figmaFree( void * _ptr, void * _userData )
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
        ::Figma::RuntimeDesc runtimeDesc;
        runtimeDesc.allocator.alloc = &Detail::figmaAlloc;
        runtimeDesc.allocator.realloc = &Detail::figmaRealloc;
        runtimeDesc.allocator.free = &Detail::figmaFree;
        runtimeDesc.allocator.userData = nullptr;

        ::Figma::RuntimeInterface * runtime = nullptr;
        ::Figma::EResult createRuntimeResult = ::Figma::createRuntime( runtimeDesc, &runtime );

        if( createRuntimeResult != ::Figma::EResult::Ok )
        {
            LOGGER_ERROR( "invalid create figma runtime result '%s'"
                , Detail::getFigmaResultMessage( createRuntimeResult )
            );

            return false;
        }

        void * memory = _memory->getBuffer();
        size_t size = _memory->getSize();

        ::Figma::LoadOptions loadOptions;
        loadOptions.sourceName = ::Figma::FigmaStringView( _sourceName.c_str() );
        loadOptions.extractImageAssets = true;
        loadOptions.keepCanvasBytes = false;

        ::Figma::DocumentInterface * document = nullptr;
        ::Figma::EResult loadResult = runtime->loadDocumentFromFigData( memory, size, loadOptions, &document );

        if( loadResult != ::Figma::EResult::Ok )
        {
            LOGGER_ERROR( "invalid load figma document '%s' result '%s'"
                , _sourceName.c_str()
                , Detail::getFigmaResultMessage( loadResult )
            );

            runtime->destroy();

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

        ::Figma::FigmaStringView ux( static_cast<const ::Figma::Char *>(_memory), _size );
        ::Figma::EResult result = m_document->loadUX( ux );

        if( result != ::Figma::EResult::Ok )
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
            m_document->destroy();
            m_document = nullptr;
        }

        if( m_runtime != nullptr )
        {
            m_runtime->destroy();
            m_runtime = nullptr;
        }

        m_memory = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    ::Figma::RuntimeInterface * FigmaData::getFigmaRuntime() const
    {
        return m_runtime;
    }
    //////////////////////////////////////////////////////////////////////////
    ::Figma::DocumentInterface * FigmaData::getFigmaDocument() const
    {
        return m_document;
    }
    //////////////////////////////////////////////////////////////////////////
}
