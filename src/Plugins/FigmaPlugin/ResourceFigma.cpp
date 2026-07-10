#include "ResourceFigma.h"

#include "Kernel/Dataflow.h"
#include "Kernel/MemoryStreamHelper.h"
#include "Kernel/ContentHelper.h"
#include "Kernel/Logger.h"
#include "Kernel/AssertionMemoryPanic.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    ResourceFigma::ResourceFigma()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    ResourceFigma::~ResourceFigma()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceFigma::setSidecarPath( const FilePath & _sidecarPath )
    {
        m_sidecarPath = _sidecarPath;
    }
    //////////////////////////////////////////////////////////////////////////
    const FilePath & ResourceFigma::getSidecarPath() const
    {
        return m_sidecarPath;
    }
    //////////////////////////////////////////////////////////////////////////
    const FigmaDataInterfacePtr & ResourceFigma::getData() const
    {
        return m_data;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceFigma::getFrameViewportSize( const String & _startFrameId, mt::vec2f * const _size )
    {
        if( _size == nullptr )
        {
            return false;
        }

        if( this->compile() == false )
        {
            return false;
        }

        bool successful = false;

        if( m_data != nullptr )
        {
            ::Figma::DocumentInterface * document = m_data->getFigmaDocument();
            ::Figma::Rectf frameRect;
            bool found = false;

            if( document != nullptr )
            {
                if( _startFrameId.empty() == false )
                {
                    const ::Figma::FigmaStringView frameId( _startFrameId.c_str(), _startFrameId.size() );
                    found = document->getFrameRect( frameId, &frameRect );
                }

                if( found == false )
                {
                    found = document->getPrototypeStartFrameRect( &frameRect );
                }

                if( found == true && frameRect.w > 0.f && frameRect.h > 0.f )
                {
                    *_size = mt::vec2f( frameRect.w, frameRect.h );
                    successful = true;
                }
            }
        }

        this->release();

        return successful;
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceFigma::_compile()
    {
        const ContentInterfacePtr & content = this->getContent();

        const FilePath & filePath = content->getFilePath();

        if( filePath.empty() == true )
        {
            LOGGER_ERROR( "resource figma '%s' group '%s' don't set file path"
                , this->getName().c_str()
                , this->getGroupName().c_str()
            );

            return false;
        }

        DataflowContext context;
        context.filePath = filePath;
        context.groupName = this->getGroupName();

        FigmaDataInterfacePtr data = Helper::getDataflow( content, &context, MENGINE_DOCUMENT_FACTORABLE );

        MENGINE_ASSERTION_MEMORY_PANIC( data, "resource '%s' group '%s' invalid compile figma data"
            , this->getName().c_str()
            , this->getGroupName().c_str()
        );

        if( data->acquire() == false )
        {
            return false;
        }

        if( this->loadSidecar_( data ) == false )
        {
            data->release();

            return false;
        }

        m_data = data;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void ResourceFigma::_release()
    {
        if( m_data != nullptr )
        {
            m_data->release();
            m_data = nullptr;
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool ResourceFigma::loadSidecar_( const FigmaDataInterfacePtr & _data )
    {
        if( m_sidecarPath.empty() == true )
        {
            return true;
        }

        const ContentInterfacePtr & content = this->getContent();
        const FileGroupInterfacePtr & fileGroup = content->getFileGroup();

        MemoryInterfacePtr sidecarMemory = Helper::createMemoryFileString( fileGroup, m_sidecarPath, false, false, MENGINE_DOCUMENT_FACTORABLE );

        if( sidecarMemory == nullptr )
        {
            LOGGER_ERROR( "resource figma '%s' group '%s' invalid load sidecar '%s'"
                , this->getName().c_str()
                , this->getGroupName().c_str()
                , m_sidecarPath.c_str()
            );

            return false;
        }

        void * sidecarBuffer = sidecarMemory->getBuffer();
        size_t sidecarSize = sidecarMemory->getSize();

        if( sidecarBuffer == nullptr || sidecarSize == 0 )
        {
            LOGGER_ERROR( "resource figma '%s' group '%s' invalid sidecar memory '%s'"
                , this->getName().c_str()
                , this->getGroupName().c_str()
                , m_sidecarPath.c_str()
            );

            return false;
        }

        if( _data->loadUX( sidecarBuffer, sidecarSize - 1 ) == false )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}
