#include "GraveyardService.h"

#include "Interface/RenderImageInterface.h"
#include "Interface/RenderTextureInterface.h"
#include "Interface/RenderTextureServiceInterface.h"
#include "Interface/TimepipeServiceInterface.h"

#include "Kernel/ConfigHelper.h"
#include "Kernel/NotificationHelper.h"

#include "Config/StdAlgorithm.h"

//////////////////////////////////////////////////////////////////////////
SERVICE_FACTORY( GraveyardService, Mengine::GraveyardService );
//////////////////////////////////////////////////////////////////////////
namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    GraveyardService::GraveyardService()
        : m_count( 0 )
        , m_graveyardTime( 1000.f )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    GraveyardService::~GraveyardService()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool GraveyardService::_initializeService()
    {
        m_graveyardTime = CONFIG_VALUE_FLOAT( "Engine", "GraveyardTime", 1000.f );

        NOTIFICATION_ADDOBSERVERMETHOD_THIS( NOTIFICATOR_RENDER_TEXTURE_DESTROY, &GraveyardService::onEngineTextureDestroy, MENGINE_DOCUMENT_FACTORABLE );

        TIMEPIPE_SERVICE()
            ->addTimepipe( TimepipeInterfacePtr::from( this ), MENGINE_DOCUMENT_FACTORABLE );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void GraveyardService::_finalizeService()
    {
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_RENDER_TEXTURE_DESTROY );

        this->clearTextures();

        TIMEPIPE_SERVICE()
            ->removeTimepipe( TimepipeInterfacePtr::from( this ) );
    }
    //////////////////////////////////////////////////////////////////////////
    void GraveyardService::clearTextures()
    {
        for( RenderTextureGraveEntry & entry : m_textures )
        {
            m_count--;

            entry.content = nullptr;
            entry.image = nullptr;
        }

        m_textures.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    void GraveyardService::onTimepipe( const UpdateContext * _context )
    {
        VectorTextureGrave::iterator it_erase = StdAlgorithm::remove_if( m_textures.begin(), m_textures.end(), [_context]( RenderTextureGraveEntry & _entry )
        {
            _entry.time -= _context->time;

            if( _entry.time > 0.f )
            {
                return false;
            }

            return true;
        } );

        for( VectorTextureGrave::iterator
            it = it_erase,
            it_end = m_textures.end();
            it != it_end;
            ++it )
        {
            RenderTextureGraveEntry & entry = *it;

            entry.image = nullptr;

            m_count--;
        }

        m_textures.erase( it_erase, m_textures.end() );
    }
    //////////////////////////////////////////////////////////////////////////
    bool GraveyardService::buryTexture( RenderTextureInterface * _texture )
    {
        const ContentInterfacePtr & content = _texture->getContent();

        const FilePath & filePath = content->getFilePath();

        if( filePath.empty() == true )
        {
            return false;
        }

        if( mt::equal_f_z( m_graveyardTime ) == true )
        {
            return false;
        }

        m_count++;

        RenderTextureGraveEntry entry;

        entry.content = content;

        entry.image = _texture->getImage();
        entry.width = _texture->getWidth();
        entry.height = _texture->getHeight();

        entry.time = m_graveyardTime;

        m_textures.emplace_back( entry );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderTextureInterfacePtr GraveyardService::resurrectTexture( const ContentInterfacePtr & _content, const DocumentInterfacePtr & _doc )
    {
        const FilePath & filePath = _content->getFilePath();

        if( filePath.empty() == true )
        {
            return nullptr;
        }

        if( mt::equal_f_z( m_graveyardTime ) == true )
        {
            return nullptr;
        }

        VectorTextureGrave::iterator it_found = StdAlgorithm::find_if( m_textures.begin(), m_textures.end(), [_content]( const RenderTextureGraveEntry & _entry )
        {
            return _entry.content == _content;
        } );

        if( it_found == m_textures.end() )
        {
            return nullptr;
        }

        --m_count;

        RenderTextureGraveEntry & entry = *it_found;

        RenderTextureInterfacePtr texture = RENDERTEXTURE_SERVICE()
            ->createRenderTexture( entry.image, entry.width, entry.height, _doc );

        entry.image = nullptr;

        *it_found = m_textures.back();
        m_textures.pop_back();

        return texture;
    }
    //////////////////////////////////////////////////////////////////////////
    void GraveyardService::onEngineTextureDestroy( RenderTextureInterface * _texture )
    {
        this->buryTexture( _texture );
    }
    //////////////////////////////////////////////////////////////////////////
}