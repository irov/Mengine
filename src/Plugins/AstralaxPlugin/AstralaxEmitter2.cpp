#include "AstralaxEmitter2.h"

#include "Kernel/Logger.h"

#include "Config/StdMath.h"

namespace Mengine
{
    //////////////////////////////////////////////////////////////////////////
    AstralaxEmitter2::AstralaxEmitter2()
        : m_emitterId( 0 )
        , m_updateSpeed( 0.f )
        , m_leftBorder( 0.0 )
        , m_rightBorder( 0.0 )
        , m_duration( 0.0 )
        , m_time( 0.0 )
        , m_rect( 0.f, 0.f, 0.f, 0.f )
        , m_angle( 0.f )
        , m_start( false )
        , m_looped( false )
        , m_is3d( false )
        , m_relative( false )
    {
    }
    //////////////////////////////////////////////////////////////////////////
    AstralaxEmitter2::~AstralaxEmitter2()
    {
    }
    //////////////////////////////////////////////////////////////////////////
    bool AstralaxEmitter2::initialize( const AstralaxEmitterContainerInterfacePtr & _container )
    {
        m_container = AstralaxEmitterContainerPtr::from( _container );

        HM_EMITTER emitterId = m_container->createEmitterId();

        if( emitterId == 0 )
        {
            LOGGER_ERROR( "invalid create emitter" );

            return false;
        }

        m_emitterId = emitterId;

        Magic_SetRandomMode( m_emitterId, false );

        // set interpolation
        Magic_SetInterpolationMode( m_emitterId, true );
        Magic_SetEmitterPositionMode( m_emitterId, false );

        m_updateSpeed = Magic_GetUpdateSpeed( m_emitterId );
        m_leftBorder = Magic_GetInterval1( m_emitterId );
        m_rightBorder = Magic_GetInterval2( m_emitterId );

        m_is3d = Magic_Is3d( m_emitterId );

        m_duration = m_rightBorder - m_leftBorder;
        //m_duration = Magic_GetDuration( m_emitterId );
        //Magic_SetInterval2( m_emitterId, m_duration );

        if( this->setupBasePosition_() == false )
        {
            return false;
        }

        mt::infinity_box( m_box );

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AstralaxEmitter2::finalize()
    {
        m_container->destroyEmitterId( m_emitterId );
        m_emitterId = 0;

        m_container = nullptr;
        m_positionProvider = nullptr;
        m_cameraProvider = nullptr;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AstralaxEmitter2::setupBasePosition_()
    {
        if( m_is3d == false )
        {
            m_rect.x = -1024.f;
            m_rect.y = -1024.f;
            m_rect.z = 1024.f;
            m_rect.w = 1024.f;

            MAGIC_POSITION pos;
            pos.x = 0.f;
            pos.y = 0.f;
            pos.z = 0.f;

            Magic_SetEmitterPosition( m_emitterId, &pos );
        }
        else
        {
            MAGIC_VIEW view;
            if( Magic_GetView( m_emitterId, &view ) == MAGIC_ERROR )
            {
                return false;
            }

            m_rect.x = 0.f;
            m_rect.y = 0.f;
            m_rect.z = (float)view.viewport_width;
            m_rect.w = (float)view.viewport_height;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    const mt::box2f & AstralaxEmitter2::getBoundingBox() const
    {
        return m_box;
    }
    //////////////////////////////////////////////////////////////////////////
    void AstralaxEmitter2::play()
    {
        Magic_Restart( m_emitterId );

        if( Magic_IsInterval1( m_emitterId ) == true )
        {
            Magic_EmitterToInterval1( m_emitterId, 1.f, nullptr );
        }

        m_time = 0.0;

        m_start = true;

        if( m_positionProvider != nullptr )
        {
            bool old_translate = Magic_GetEmitterPositionMode( m_emitterId );
            Magic_SetEmitterPositionMode( m_emitterId, true );

            mt::vec3f pos;
            m_positionProvider->onProviderEmitterPosition( &pos );

            MAGIC_POSITION mp;
            mp.x = pos.x;
            mp.y = pos.y;
            mp.z = pos.z;

            Magic_SetEmitterPosition( m_emitterId, &mp );

            Magic_SetEmitterPositionMode( m_emitterId, old_translate );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void AstralaxEmitter2::setLoop( bool _loop )
    {
        if( _loop == true )
        {
            Magic_SetLoopMode( m_emitterId, MAGIC_LOOP );
        }
        else
        {
            Magic_SetLoopMode( m_emitterId, MAGIC_NOLOOP );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool AstralaxEmitter2::getLoop() const
    {
        MAGIC_LOOP_ENUM loopMode = Magic_GetLoopMode( m_emitterId );

        return loopMode != MAGIC_NOLOOP;
    }
    //////////////////////////////////////////////////////////////////////////
    void AstralaxEmitter2::interrupt()
    {
        if( Magic_IsInterrupt( m_emitterId ) == true )
        {
            return;
        }

        Magic_Update( m_emitterId, 0.f );
        Magic_SetLoopMode( m_emitterId, MAGIC_NOLOOP );
        Magic_SetInterrupt( m_emitterId, true );
    }
    //////////////////////////////////////////////////////////////////////////
    bool AstralaxEmitter2::is3d() const
    {
        return m_is3d;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AstralaxEmitter2::getCamera( AstralaxCamera * const _camera ) const
    {
        MAGIC_VIEW view;
        if( Magic_GetView( m_emitterId, &view ) == MAGIC_ERROR )
        {
            return false;
        }

        _camera->pos.x = view.pos.x;
        _camera->pos.y = view.pos.y;
        _camera->pos.z = view.pos.z;

        _camera->dir.x = view.dir.x;
        _camera->dir.y = view.dir.y;
        _camera->dir.z = view.dir.z;

        _camera->up.x = view.up.x;
        _camera->up.y = view.up.y;
        _camera->up.z = view.up.z;

        _camera->fov = view.fov;
        _camera->aspect = view.aspect_ratio;
        _camera->znear = view.znear;
        _camera->zfar = view.zfar;

        _camera->width = (float)view.viewport_width;
        _camera->height = (float)view.viewport_height;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AstralaxEmitter2::stop()
    {
        m_start = false;

        Magic_Stop( m_emitterId );
    }
    //////////////////////////////////////////////////////////////////////////
    void AstralaxEmitter2::pause()
    {
        m_start = false;
    }
    //////////////////////////////////////////////////////////////////////////
    void AstralaxEmitter2::resume()
    {
        m_start = true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AstralaxEmitter2::update( float _time, bool * const _stop )
    {
        if( m_start == false )
        {
            return false;
        }

        m_time += _time;

        if( m_cameraProvider != nullptr )
        {
            bool orthogonality;
            mt::vec3f position;
            mt::vec3f direction;
            m_cameraProvider->onProviderEmitterCamera( &orthogonality, &position, &direction );

            MAGIC_VIEW view;
            if( Magic_GetView( m_emitterId, &view ) == MAGIC_ERROR )
            {
                return false;
            }

            MAGIC_CAMERA camera;

            if( orthogonality == true )
            {
                camera.mode = MAGIC_CAMERA_ORTHO;
            }
            else
            {
                camera.mode = MAGIC_CAMERA_FREE;
            }

            camera.pos.x = position.x;
            camera.pos.y = position.y;
            camera.pos.z = position.z;

            camera.dir.x = direction.x;
            camera.dir.y = direction.y;
            camera.dir.z = direction.z;

            Magic_SetCamera( &camera );
        }
        else
        {
            if( m_is3d == true )
            {
                MAGIC_VIEW view;
                if( Magic_GetView( m_emitterId, &view ) == MAGIC_ERROR )
                {
                    return false;
                }

                MAGIC_CAMERA camera;
                camera.mode = MAGIC_CAMERA_FREE;
                camera.pos = view.pos;
                camera.dir = view.dir;

                Magic_SetCamera( &camera );
            }
            else
            {
                MAGIC_CAMERA camera;
                camera.mode = MAGIC_CAMERA_ORTHO;
                camera.pos.x = 0.f;
                camera.pos.y = 0.f;
                camera.pos.z = 0.f;

                camera.dir.x = 0.f;
                camera.dir.y = 0.f;
                camera.dir.z = 1.f;

                Magic_SetCamera( &camera );
            }
        }

        if( m_positionProvider != nullptr )
        {
            mt::vec3f pos;
            m_positionProvider->onProviderEmitterPosition( &pos );

            MAGIC_POSITION mp;
            mp.x = pos.x;
            mp.y = pos.y;
            mp.z = pos.z;

            Magic_SetEmitterPosition( m_emitterId, &mp );
        }

        float total_timing = _time /** m_updateSpeed*/;

        bool restart = Magic_Update( m_emitterId, total_timing );

        if( restart == false )
        {
            m_start = false;

            *_stop = true;
        }
        else
        {
            *_stop = false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    HM_EMITTER AstralaxEmitter2::getId() const
    {
        return m_emitterId;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AstralaxEmitter2::inInterval() const
    {
        if( m_emitterId == 0 )
        {
            return false;
        }

        bool result = Magic_InInterval( m_emitterId );

        return result;
    }
    //////////////////////////////////////////////////////////////////////////
    void AstralaxEmitter2::setEmitterTranslateWithParticle( bool _value )
    {
        Magic_SetEmitterPositionMode( m_emitterId, _value );
    }
    //////////////////////////////////////////////////////////////////////////
    bool AstralaxEmitter2::changeEmitterImage( uint32_t _width, uint32_t _height, const void * _data, size_t _bytes )
    {
        uint8_t * magic_data = const_cast<uint8_t *>(reinterpret_cast<const uint8_t *>(_data));

        if( Magic_ChangeImage( m_emitterId, -1, (int32_t)_width, (int32_t)_height, magic_data, (int32_t)_bytes ) == MAGIC_ERROR )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AstralaxEmitter2::changeEmitterModel( float * const _points, uint32_t _count )
    {
        MAGIC_TRIANGLE * triangle = reinterpret_cast<MAGIC_TRIANGLE *>(_points);

        if( Magic_ChangeModel( m_emitterId, -1, (int32_t)_count, triangle ) == MAGIC_ERROR )
        {
            return false;
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AstralaxEmitter2::setPositionProvider( const AstralaxPositionProviderInterfacePtr & _positionProvider )
    {
        m_positionProvider = _positionProvider;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AstralaxEmitter2::setCameraProvider( const AstralaxCameraProviderInterfacePtr & _cameraProvider )
    {
        m_cameraProvider = _cameraProvider;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    void AstralaxEmitter2::setScale( float _scale )
    {
        Magic_SetScale( m_emitterId, _scale );
    }
    //////////////////////////////////////////////////////////////////////////
    void AstralaxEmitter2::restart()
    {
        Magic_Restart( m_emitterId );

        if( Magic_IsInterval1( m_emitterId ) == true )
        {
            //Magic_SetInterval1( m_id, m_leftBorder );
            Magic_EmitterToInterval1( m_emitterId, 1.f, nullptr );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    void AstralaxEmitter2::setAngle( float _radians )
    {
        m_angle = MENGINE_RAD2DEG( _radians );

        Magic_SetDiagramAddition( m_emitterId, -1, MAGIC_DIAGRAM_DIRECTION, m_angle );

        int32_t k_par = Magic_GetParticlesTypeCount( m_emitterId );

        for( int32_t i = 0; i != k_par; ++i )
        {
            Magic_SetDiagramAddition( m_emitterId, i, MAGIC_DIAGRAM_DIRECTION, m_angle );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    float AstralaxEmitter2::getLeftBorder() const
    {
        float float_leftBorder = (float)m_leftBorder;

        return float_leftBorder;
    }
    //////////////////////////////////////////////////////////////////////////
    float AstralaxEmitter2::getRightBorder() const
    {
        float float_rightBorder = (float)m_rightBorder;

        return float_rightBorder;
    }
    //////////////////////////////////////////////////////////////////////////
    float AstralaxEmitter2::getDuration() const
    {
        double duration = m_duration / m_updateSpeed;

        float float_duration = (float)duration;

        return float_duration;
    }
    //////////////////////////////////////////////////////////////////////////
    void AstralaxEmitter2::seek( float _time )
    {
        Magic_Restart( m_emitterId );

        float duration = this->getDuration();

        if( _time >= duration )
        {
            m_time = (double)duration;

            return;
        }

        m_time = (double)_time;

        if( Magic_IsInterval1( m_emitterId ) == true )
        {
            Magic_EmitterToInterval1( m_emitterId, 1.f, nullptr );
        }

        double rate = Magic_GetUpdateTime( m_emitterId );

        double time = _time;

        while( time >= rate && rate != 0.0 )
        {
            time -= rate;

            if( Magic_Update( m_emitterId, rate ) == false )
            {
                return;
            }
        }

        Magic_Update( m_emitterId, time );
    }
    //////////////////////////////////////////////////////////////////////////
    float AstralaxEmitter2::getUpdateTemp() const
    {
        float tempScale = Magic_GetUpdateSpeed( m_emitterId );

        return tempScale;
    }
    //////////////////////////////////////////////////////////////////////////
    void AstralaxEmitter2::setRandomMode( bool _randomMode )
    {
        Magic_SetRandomMode( m_emitterId, _randomMode );
    }
    //////////////////////////////////////////////////////////////////////////
    bool AstralaxEmitter2::getRandomMode() const
    {
        bool mode = Magic_IsRandomMode( m_emitterId );

        return mode;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AstralaxEmitter2::prepareParticles( AstralaxEmitterRenderFlush * const _flush )
    {
        _flush->meshCount = 0;
        _flush->vertexCount = 0;
        _flush->indexCount = 0;
        _flush->arrays = 0;
        _flush->context = nullptr;

        if( this->inInterval() == false )
        {
            return false;
        }

        if( ASTRALAX_SERVICE()
            ->updateAtlas() == false )
        {
            return false;
        }

#if defined(MENGINE_RENDER_COLOR_RGBA)
        MAGIC_ARGB_ENUM color_mode = MAGIC_ARGB;
#else
        MAGIC_ARGB_ENUM color_mode = MAGIC_ABGR;
#endif

        MAGIC_RENDERING_START start;
        void * context = Magic_PrepareRenderArrays( m_emitterId, &start, 100, color_mode, sizeof( RenderIndex ) == 4 );

        if( start.vertices == 0 || start.indexes == 0 )
        {
            return false;
        }

        _flush->vertexCount = start.vertices;
        _flush->indexCount = start.indexes;
        _flush->arrays = start.arrays;
        _flush->context = context;

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
    bool AstralaxEmitter2::flushParticles( AstralaxMesh * const _meshes, uint32_t _meshLimit, RenderVertex2D * const _vertices, RenderIndex * const _indices, AstralaxEmitterRenderFlush * const _flush )
    {
        for( uint32_t i = 0; i != _flush->arrays; ++i )
        {
            MAGIC_ARRAY_INFO array_info;
            Magic_GetRenderArrayData( _flush->context, i, &array_info );

            switch( array_info.type )
            {
            case MAGIC_VERTEX_FORMAT_INDEX:
                {
                    if( Magic_SetRenderArrayData( _flush->context, i, _indices, 0, sizeof( RenderIndex ) ) == MAGIC_ERROR )
                    {
                        return false;
                    }
                }break;
            case MAGIC_VERTEX_FORMAT_POSITION:
                {
                    if( Magic_SetRenderArrayData( _flush->context, i, _vertices, MENGINE_OFFSETOF( RenderVertex2D, position ), sizeof( RenderVertex2D ) ) == MAGIC_ERROR )
                    {
                        return false;
                    }
                }break;
            case MAGIC_VERTEX_FORMAT_COLOR:
                {
                    if( Magic_SetRenderArrayData( _flush->context, i, _vertices, MENGINE_OFFSETOF( RenderVertex2D, color ), sizeof( RenderVertex2D ) ) == MAGIC_ERROR )
                    {
                        return false;
                    }
                }break;
            case MAGIC_VERTEX_FORMAT_UV:
                {
                    if( array_info.index >= MENGINE_RENDER_VERTEX_UV_COUNT )
                    {
                        return false;
                    }

                    if( Magic_SetRenderArrayData( _flush->context, i, _vertices, MENGINE_OFFSETOF( RenderVertex2D, uv ) + sizeof( mt::vec2f ) * array_info.index, sizeof( RenderVertex2D ) ) == false )
                    {
                        return false;
                    }
                }break;
            case MAGIC_VERTEX_FORMAT_NORMAL:
                {

                }break;
            case MAGIC_VERTEX_FORMAT_TANGENT:
                {

                }break;
            case MAGIC_VERTEX_FORMAT_BINORMAL:
                {

                }break;
            default:
                return false;
                break;
            }
        }

        Magic_FillRenderArrays( _flush->context );

        MAGIC_RENDER_VERTICES vrts;
        while( Magic_GetVertices( _flush->context, &vrts ) == MAGIC_SUCCESS )
        {
            if( _flush->meshCount >= _meshLimit )
            {
                return false;
            }

            AstralaxMesh & mesh = _meshes[_flush->meshCount];

            mesh.vertexOffset = vrts.starting_index * 4 / 6;
            mesh.vertexCount = vrts.indexes_count * 4 / 6;
            mesh.indexOffset = vrts.starting_index;
            mesh.indexCount = vrts.indexes_count;

            if( mesh.vertexOffset != 0 )
            {
                for( RenderIndex
                    * it = _indices + mesh.indexOffset,
                    *it_end = _indices + mesh.indexOffset + mesh.indexCount;
                    it != it_end;
                    ++it )
                {
                    RenderIndex & indices = *it;

                    indices -= (RenderIndex)mesh.vertexOffset;
                }
            }

            MAGIC_RENDER_STATE state;
            while( Magic_GetNextRenderState( _flush->context, &state ) == MAGIC_SUCCESS )
            {
                if( state.state != MAGIC_RENDER_STATE_TEXTURE )
                {
                    return false;
                }

                if( state.index == -1 )
                {
                    return false;
                }

                mesh.texture[state.index] = state.value;
            }

            mesh.material = vrts.material;

            MAGIC_MATERIAL m;
            if( Magic_GetMaterial( vrts.material, &m ) != MAGIC_SUCCESS )
            {
                return false;
            }

            mesh.textures = m.textures;

            _flush->meshCount++;
        }

        if( m_positionProvider == nullptr )
        {
            mt::mat4f vpm;

            if( m_is3d == true )
            {
                AstralaxCamera pc;
                if( this->getCamera( &pc ) == false )
                {
                    return false;
                }

                mt::mat4f vm;
                mt::make_lookat_m4( vm, pc.pos, pc.dir, -pc.up, -1.f );

                mt::mat4f pm;
                mt::make_projection_fov_m4( pm, pc.fov, pc.aspect, pc.znear, pc.zfar );

                mt::mul_m4_m4( vpm, vm, pm );
            }
            else
            {
                mt::mat4f vm;
                mt::ident_m4( vm );

                mt::mat4f pm;
                mt::make_projection_ortho_lh_m4( pm, m_rect.x, m_rect.z, m_rect.w, m_rect.y, -1024.f, 1024.f );

                mt::mul_m4_m4( vpm, vm, pm );
            }

            float half_width = (m_rect.z - m_rect.x) * 0.5f;
            float half_height = (m_rect.w - m_rect.y) * 0.5f;

            RenderVertex2D * vertices_iterator = _vertices;
            RenderVertex2D * vertices_end = _vertices + _flush->vertexCount;

            for( ; vertices_iterator != vertices_end; ++vertices_iterator )
            {
                mt::vec3f v_vpm;
                mt::mul_v3_v3_m4_homogenize( v_vpm, vertices_iterator->position, vpm );

                mt::vec3f v_new;
                v_new.x = (1.f + v_vpm.x) * half_width;
                v_new.y = (1.f + v_vpm.y) * half_height;
                v_new.z = v_vpm.z;

                vertices_iterator->position = v_new;
            }
        }

        return true;
    }
    //////////////////////////////////////////////////////////////////////////
}