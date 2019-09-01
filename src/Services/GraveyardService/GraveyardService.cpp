#include "GraveyardService.h"

#include "Interface/RenderImageInterface.h"
#include "Interface/RenderTextureInterface.h"
#include "Interface/RenderTextureServiceInterface.h"
#include "Interface/NotificatorInterface.h"
#include "Interface/ConfigServiceInterface.h"
#include "Interface/NotificationServiceInterface.h"

#include <algorithm>

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
        m_graveyardTime = CONFIG_VALUE( "Engine", "GraveyardTime", 1000.f );

        NOTIFICATION_ADDOBSERVERMETHOD( NOTIFICATOR_ENGINE_TEXTURE_DESTROY, this, &GraveyardService::onEngineTextureDestroy );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void GraveyardService::_finalizeService()
    {
        NOTIFICATION_REMOVEOBSERVER_THIS( NOTIFICATOR_ENGINE_TEXTURE_DESTROY );

        this->clearTextures();
    }
    //////////////////////////////////////////////////////////////////////////
    void GraveyardService::clearTextures()
    {
        for( RenderTextureGraveEntry & entry : m_textures )
        {
            m_count--;

            entry.image = nullptr;
        }

        m_textures.clear();
    }
    //////////////////////////////////////////////////////////////////////////
    void GraveyardService::tick( const UpdateContext * _context )
    {
        VectorTextureGrave::iterator it_erase = std::remove_if( m_textures.begin(), m_textures.end(), [_context]( RenderTextureGraveEntry & _entry )
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
        const FileGroupInterfacePtr & fileGroup = _texture->getFileGroup();
        const FilePath & filePath = _texture->getFilePath();

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

        entry.category = fileGroup;
        entry.filePath = filePath;

        entry.image = _texture->getImage();
        entry.width = _texture->getWidth();
        entry.height = _texture->getHeight();

        entry.time = m_graveyardTime;

        m_textures.emplace_back( entry );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    RenderTextureInterfacePtr GraveyardService::resurrectTexture( const FileGroupInterfacePtr & _fileGroup, const FilePath & _filePath, const Char * _doc )
    {
        if( _filePath.empty() == true )
        {
            return nullptr;
        }

        if( mt::equal_f_z( m_graveyardTime ) == true )
        {
            return nullptr;
        }

        VectorTextureGrave::iterator it_found = std::find_if( m_textures.begin(), m_textures.end(), [&_fileGroup, &_filePath]( const RenderTextureGraveEntry & _entry )
        {
            if( _entry.category != _fileGroup )
            {
                return false;
            }

            if( _entry.filePath != _filePath )
            {
                return false;
            }

            return true;
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
}