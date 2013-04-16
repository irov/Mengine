
#	include "ScriptWrapper.h"

#   include "Interface/ApplicationInterface.h"
#   include "Interface/FileSystemInterface.h"

#   include "Interface/InputSystemInterface.h"
#   include "Interface/EntityInterface.h"
#   include "Interface/NodeInterface.h"

#	include "Kernel/ThreadTask.h"
#	include "Kernel/Scene.h"

#	include "Game.h"

#	include "Consts.h"

#	include "ScriptClassWrapper.h"

#   include "Interface/ScriptSystemInterface.h"
#   include "Interface/ScheduleManagerInterface.h"

#   include "Interface/TextInterface.h"
#   include "Interface/ArrowInterface.h"

#   include "Interface/SceneInterface.h"
#   include "Interface/AccountInterface.h"

#   include "Interface/EventInterface.h"
#   include "Interface/UnicodeInterface.h"

#   include "Interface/ResourceInterface.h"

#	include "ResourceMovie.h"
//#	include "ResourceImageDynamic.h"
#	include "ResourceImageDefault.h"
#	include "ResourceTestPick.h"
#	include "ResourceHIT.h"
#	include "ResourceEmitterContainer.h"

#	include "Player.h"

#   include "Interface/ApplicationInterface.h"
#   include "Interface/MousePickerSystemInterface.h"

#	include "Sprite.h"
#   include "MovieSlot.h"
#	include "MovieInternalObject.h"
#	include "Animation.h"
#	include "HotSpot.h"
//#	include "Light2D.h"
#	include "ShadowCaster2D.h"
#	include "Arrow.h"
#	include "TextField.h"
#	include "SoundEmitter.h"
#	include "ParticleEmitter.h"
#	include "Point.h"
#	include "Camera3d.h"
//#	include "RigidBody3D.h"
//#	include "CapsuleController.h"
//#	include "Skeleton.h"
#	include "Camera2D.h"

#	include "Layer2D.h"
#	include "Layer2DPhysic.h"
#	include "Layer2DTexture.h"

//#	include "RigidBody2D.h"

#	include "PhysicalBody2D.h"
#	include "TilePolygon.h"

#	include "Movie.h"
#	include "Video.h"

#	include "Window.h"
#	include "HotSpotImage.h"

#	include "Kernel/Entity.h"

//#	include "DiscreteEntity.h"

//#	include "SoundEngine.h"
#	include "Logger/Logger.h"

#   include "Interface/RenderSystemInterface.h"
#   include "Interface/PhysicSystem2DInterface.h"
#   include "Interface/TimingManagerInterface.h"

#	include "Kernel/Identity.h"

#	include "Kernel/Affector.h"

#	include "Camera3D.h"

#	include "Kernel/ResourceVisitor.h"

#   include "Kernel/ThreadTask.h"

#   include "Interface/ThreadSystemInterface.h"

#	include "ThreadTaskDeferredLoading.h"
#	include "ThreadTaskLoadResourceImage.h"
#	include "ThreadTaskLoadImageResources.h"

#	include "Core/Polygon.h"

#	include "Utils/Math/angle.h"
#	include "Utils/Math/vec4.h"
#	include "Utils/Math/mat3.h"
#	include "Utils/Math/mat4.h"
#	include "Utils/Math/quat.h"
#	include "Utils/Math/clamp.h"

#	include "Utils/Core/Rect.h"
#	include "Utils/Core/String.h"

#	include "Kernel/Join.h"

#	include <sstream>

#pragma warning(push, 0) 
#	include <boost/geometry/geometry.hpp> 
#	include <boost/geometry/geometries/point_xy.hpp>
#	include <boost/geometry/geometries/polygon.hpp>
#	include <boost/geometry/algorithms/intersects.hpp>
#pragma warning(pop) 

namespace Menge
{
    //////////////////////////////////////////////////////////////////////////
    class NodeScriptMethod
    {
    public:
        NodeScriptMethod( ServiceProviderInterface * _serviceProvider )
            : m_serviceProvider(_serviceProvider)
        {
        }

    public:
        //////////////////////////////////////////////////////////////////////////
        float movie_getFrameDuration( Movie * _movie )
        {
            ResourceMovie * resourceMovie = _movie->getResourceMovie();

            if( resourceMovie == NULL )
            {
                LOGGER_ERROR(m_serviceProvider)( "Movie.getFrameDuration: '%s' not activate"
                    , _movie->getName().c_str()
                    );

                return 0.f;
            }

            float frameDuration = resourceMovie->getFrameDuration();

            return frameDuration;
        }
        //////////////////////////////////////////////////////////////////////////
        float movie_getDuration( Movie * _movie )
        {
            ResourceMovie * resourceMovie = _movie->getResourceMovie();

            if( resourceMovie == NULL )
            {
                LOGGER_ERROR(m_serviceProvider)( "Movie.getDuration: '%s' not activate"
                    , _movie->getName().c_str()
                    );

                return 0.f;
            }

            float duration = resourceMovie->getDuration();

            return duration; 
        }
        //////////////////////////////////////////////////////////////////////////
        size_t movie_getFrameCount( Movie * _movie )
        {
            ResourceMovie * resourceMovie = _movie->getResourceMovie();

            if( resourceMovie == NULL )
            {
                LOGGER_ERROR(m_serviceProvider)( "Movie.getFrameCount: '%s' not activate"
                    , _movie->getName().c_str()
                    );

                return 0;
            }

            size_t count = resourceMovie->getFrameCount();

            return count; 
        }
        //////////////////////////////////////////////////////////////////////////
        const mt::vec2f & movie_getSize( Movie * _movie )
        {
            ResourceMovie * resourceMovie = _movie->getResourceMovie();

            if( resourceMovie == NULL )
            {
                LOGGER_ERROR(m_serviceProvider)( "Movie.getSize: '%s' not activate"
                    , _movie->getName().c_str()
                    );

                return mt::zero_v2;
            }

            const mt::vec2f & size = resourceMovie->getSize();

            return size; 
        }
        //////////////////////////////////////////////////////////////////////////
        void s_removeRelationTransformation( Transformation3D * _transformation )
        {
            _transformation->setRelationTransformation( NULL );
        }
        //////////////////////////////////////////////////////////////////////////
        mt::vec2f s_getLocalPolygonCenter( HotSpot * _hs )
        {
            mt::vec2f pc(0.f, 0.f);

            const Polygon & polygon = _hs->getPolygon();

            const Polygon::ring_type & ring = polygon.outer();

            if( ring.empty() == true )
            {
                return mt::vec2f(0.f, 0.f);
            }

            for( Polygon::ring_type::const_iterator 
                it = ring.begin(),
                it_end = ring.end();
            it != it_end;
            ++it )
            {
                pc += *it;
            }

            pc -= ring.back();

            float size = (float)boost::geometry::num_points(polygon);
            pc /= (size - 1);

            return pc;
        }
        //////////////////////////////////////////////////////////////////////////
        mt::vec2f s_getWorldPolygonCenter( HotSpot * _hs )
        {
            mt::vec2f pc = s_getLocalPolygonCenter( _hs );

            const mt::mat4f & wm = _hs->getWorldMatrix();

            mt::vec2f world_pc;
            mt::mul_v2_m4(world_pc, pc, wm);

            return world_pc;
        }
        //////////////////////////////////////////////////////////////////////////
        Polygon s_getWorldPolygon( HotSpot * _hs )
        {
            const Polygon & polygon = _hs->getPolygon();

            const mt::mat4f & wm = _hs->getWorldMatrix();

            Polygon pwm;
            polygon_wm(pwm, polygon, wm);

            return pwm;
        }
        //////////////////////////////////////////////////////////////////////////
        Polygon s_polygon_wm( const Polygon & _polygon, const mt::mat4f & _wm )
        {
            Polygon polygon;
            polygon_wm(polygon, _polygon, _wm);

            return polygon;
        }
        //////////////////////////////////////////////////////////////////////////
        bool s_testHotspot( HotSpot * _left, HotSpot * _right )
        {
            const Polygon & left_poligon = _left->getPolygon();
            const mt::mat4f & left_wm = _left->getWorldMatrix();

            const Polygon & right_poligon = _right->getPolygon();
            const mt::mat4f & right_wm = _right->getWorldMatrix();

            Polygon left_polygon_wm;

            polygon_wm(left_polygon_wm, left_poligon, left_wm);

            Polygon right_polygon_wm;

            polygon_wm(right_polygon_wm, right_poligon, right_wm);

            bool result = boost::geometry::intersects( left_polygon_wm, right_polygon_wm );

            return result;

            //return _left->testPolygon( left_wm, right_poligon, right_wm );
        }
        //////////////////////////////////////////////////////////////////////////
        bool s_loadPlugin( const String & _pluginName )
        {
            PluginInterface * plugin = PLUGIN_SERVICE(m_serviceProvider)
                ->loadPlugin( _pluginName );

            if( plugin == NULL )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        void s_blockInput( bool _value )
        {
            MousePickerSystemInterface * mousePickerSystem = PLAYER_SERVICE(m_serviceProvider)
                ->getMousePickerSystem();

            mousePickerSystem->block(_value);
        }
        //////////////////////////////////////////////////////////////////////////
        void s_setInputMouseButtonEventBlock( bool _value )
        {
            APPLICATION_SERVICE(m_serviceProvider)
                ->setInputMouseButtonEventBlock( _value );
        }
        //////////////////////////////////////////////////////////////////////////
        bool s_getInputMouseButtonEventBlock()
        {
            return APPLICATION_SERVICE(m_serviceProvider)
                ->getInputMouseButtonEventBlock();
        }
        //////////////////////////////////////////////////////////////////////////
        class MyTimingListener
            : public TimingListener
        {
        public:
            MyTimingListener( ServiceProviderInterface * _serviceProvider, PyObject * _script )
                : m_serviceProvider(_serviceProvider)
                , m_script(_script)
            {
                pybind::incref(m_script);
            }

            ~MyTimingListener()
            {
                pybind::decref(m_script);
            }

        protected:
            bool updateTiming( size_t _id, float _timing ) override
            {
                EVENT_SERVICE(m_serviceProvider)
                    ->addEventFormat( EVENT_TIMING, m_script, "(if)", _id, _timing );

                return false;
            }

            void removeTiming( size_t _id ) override
            {
                EVENT_SERVICE(m_serviceProvider)
                    ->addEventFormat( EVENT_TIMING, m_script, "(iO)", _id, pybind::get_bool(true) );
            }

            void deleteTimingListener() const override
            {
                delete this;
            }

        protected:
            ServiceProviderInterface * m_serviceProvider;
            PyObject * m_script;
        };
        //////////////////////////////////////////////////////////////////////////
        size_t timing( bool _portion, bool _global, float _timing, PyObject * _script )
        {
            TimingManagerInterface * tm = PLAYER_SERVICE(m_serviceProvider)
                ->getTimingManager();

            TimingListener * listener = new MyTimingListener(m_serviceProvider, _script);

            size_t id = tm->timing( _portion, _global, _timing, listener );

            return id;
        }
        //////////////////////////////////////////////////////////////////////////
        void timingRemove( size_t _id )
        {
            TimingManagerInterface * tm = PLAYER_SERVICE(m_serviceProvider)
                ->getTimingManager();

            tm->remove( _id );
        }
        //////////////////////////////////////////////////////////////////////////
        class PyObjectScheduleListener
            : public ScheduleListener
        {
        public:
            PyObjectScheduleListener( ServiceProviderInterface * _serviceProvider, PyObject * _pyObject )
                : m_serviceProvider(_serviceProvider)
                , m_pyObject(_pyObject)
            {
                pybind::incref( m_pyObject );
            }

            PyObjectScheduleListener()
            {
                pybind::decref( m_pyObject );
            }

        protected:
            void onScheduleComplete( size_t _id ) override
            {
                SCRIPT_SERVICE(m_serviceProvider)
                    ->callFunction( m_pyObject, "(iO)", _id, pybind::get_bool(false) );
                //EVENT_SERVICE(m_serviceProvider)
                //    ->addEventFormat( EVENT_SCHEDULE, m_pyObject, "(iO)", _id, pybind::get_bool(false) );
            }

            void onScheduleStop( size_t _id ) override
            {
                SCRIPT_SERVICE(m_serviceProvider)
                    ->callFunction( m_pyObject, "(iO)", _id, pybind::get_bool(true) );
                //EVENT_SERVICE(m_serviceProvider)
                //    ->addEventFormat( EVENT_SCHEDULE, m_pyObject, "(iO)", _id, pybind::get_bool(true) );
            }

        protected:
            void destroyScheduleListener() const override
            {
                delete this;
            }

        protected:
            ServiceProviderInterface * m_serviceProvider;
            PyObject * m_pyObject;
        };
        //////////////////////////////////////////////////////////////////////////
        size_t schedule( float _timing, PyObject * _script )
        {
            ScheduleManagerInterface * sm = PLAYER_SERVICE(m_serviceProvider)
                ->getScheduleManager();

            ScheduleListener * sl = new PyObjectScheduleListener(m_serviceProvider, _script);

            size_t id = sm->schedule( _timing, sl );

            return id;
        }
        //////////////////////////////////////////////////////////////////////////
        void scheduleRemove( size_t _id )
        {
            ScheduleManagerInterface * sm = PLAYER_SERVICE(m_serviceProvider)
                ->getScheduleManager();

            sm->remove( _id );
        }
        //////////////////////////////////////////////////////////////////////////
        void scheduleRemoveAll()
        {
            ScheduleManagerInterface * sm = PLAYER_SERVICE(m_serviceProvider)
                ->getScheduleManager();

            sm->removeAll();
        }	
        //////////////////////////////////////////////////////////////////////////
        void s_scheduleFreeze( size_t _id, bool _freeze )
        {
            ScheduleManagerInterface * sm = PLAYER_SERVICE(m_serviceProvider)
                ->getScheduleManager();

            sm->freeze( _id, _freeze );
        }
        //////////////////////////////////////////////////////////////////////////
        void s_scheduleFreezeAll()
        {
            ScheduleManagerInterface * sm = PLAYER_SERVICE(m_serviceProvider)
                ->getScheduleManager();

            sm->freezeAll( true );
        }

        void scheduleResumeAll()
        {
            ScheduleManagerInterface * sm = PLAYER_SERVICE(m_serviceProvider)
                ->getScheduleManager();

            sm->freezeAll( false );
        }
        //////////////////////////////////////////////////////////////////////////
        bool s_scheduleIsFreeze( size_t _id )
        {
            ScheduleManagerInterface * sm = PLAYER_SERVICE(m_serviceProvider)
                ->getScheduleManager();

            return sm->isFreeze( _id );
        }
        //////////////////////////////////////////////////////////////////////////
        float s_scheduleTime( size_t _id )
        {
            ScheduleManagerInterface * sm = PLAYER_SERVICE(m_serviceProvider)
                ->getScheduleManager();

            float time = sm->time( _id );

            return time;
        }
        //////////////////////////////////////////////////////////////////////////
        size_t s_scheduleGlobal( float _timing, PyObject * _script )
        {
            ScheduleManagerInterface * sm = PLAYER_SERVICE(m_serviceProvider)
                ->getScheduleManagerGlobal();

            ScheduleListener * sl = new PyObjectScheduleListener(m_serviceProvider, _script);

            size_t id = sm->schedule( _timing, sl );

            return id;
        }
        //////////////////////////////////////////////////////////////////////////
        void s_scheduleGlobalRemove( size_t _id )
        {
            ScheduleManagerInterface * sm = PLAYER_SERVICE(m_serviceProvider)
                ->getScheduleManagerGlobal();

            sm->remove( _id );
        }
        //////////////////////////////////////////////////////////////////////////
        void s_scheduleGlobalRemoveAll()
        {
            ScheduleManagerInterface * sm = PLAYER_SERVICE(m_serviceProvider)
                ->getScheduleManagerGlobal();

            sm->removeAll();
        }	
        //////////////////////////////////////////////////////////////////////////
        void s_scheduleGlobalFreeze( size_t _id, bool _freeze )
        {
            ScheduleManagerInterface * sm = PLAYER_SERVICE(m_serviceProvider)
                ->getScheduleManagerGlobal();

            sm->freeze( _id, _freeze );
        }
        //////////////////////////////////////////////////////////////////////////
        void s_scheduleGlobalFreezeAll()
        {
            ScheduleManagerInterface* sm = PLAYER_SERVICE(m_serviceProvider)
                ->getScheduleManagerGlobal();

            sm->freezeAll( true );
        }
        //////////////////////////////////////////////////////////////////////////
        void s_scheduleGlobalResumeAll()
        {
            ScheduleManagerInterface * sm = PLAYER_SERVICE(m_serviceProvider)
                ->getScheduleManagerGlobal();

            sm->freezeAll( false );
        }
        //////////////////////////////////////////////////////////////////////////
        bool s_scheduleGlobalIsFreeze( size_t _id )
        {
            ScheduleManagerInterface* sm = PLAYER_SERVICE(m_serviceProvider)
                ->getScheduleManagerGlobal();

            bool freeze = sm->isFreeze( _id );

            return freeze;
        }
        //////////////////////////////////////////////////////////////////////////
        float s_scheduleGlobalTime( size_t _id )
        {
            ScheduleManagerInterface* sm = PLAYER_SERVICE(m_serviceProvider)
                ->getScheduleManagerGlobal();

            float time = sm->time( _id );

            return time;
        }
        //////////////////////////////////////////////////////////////////////////
        float getMouseX()
        {
            const Resolution& contRes = APPLICATION_SERVICE(m_serviceProvider)
                ->getContentResolution();

            float mx = PLAYER_SERVICE(m_serviceProvider)->getArrow()->getLocalPosition().x;

            mx = mt::clamp( 0.0f, mx, static_cast<float>( contRes.getWidth() ) );

            return mx;
        }
        //////////////////////////////////////////////////////////////////////////
        float getMouseY()
        {
            const Resolution& contRes = APPLICATION_SERVICE(m_serviceProvider)
                ->getContentResolution();

            float my = PLAYER_SERVICE(m_serviceProvider)->getArrow()->getLocalPosition().y;

            my = mt::clamp( 0.0f, my, static_cast<float>( contRes.getHeight() ) );

            return my;
        }
        //////////////////////////////////////////////////////////////////////////
        bool s_isMouseButtonDown( int _button )
        {
            return INPUT_SERVICE(m_serviceProvider)
                ->isMouseButtonDown( _button );
        }
        //////////////////////////////////////////////////////////////////////////
        bool s_isKeyDown( int _key )
        {
            return INPUT_SERVICE(m_serviceProvider)
                ->isKeyDown( static_cast<KeyCode>( _key ) );
        }
        //////////////////////////////////////////////////////////////////////////
        void s_setTimingFactor( float _factor )
        {
            GAME_SERVICE(m_serviceProvider)->setTimingFactor( _factor );
        }
        //////////////////////////////////////////////////////////////////////////
        void s_addHomeless( Node * _node )
        {
            NODE_SERVICE(m_serviceProvider)
                ->addHomeless( _node );
        }
        //////////////////////////////////////////////////////////////////////////
        float s_getTimingFactor( )
        {
            float factor = GAME_SERVICE(m_serviceProvider)
                ->getTimingFactor();

            return factor;
        }
        //////////////////////////////////////////////////////////////////////////
        void setCurrentScene( const ConstString& _name, bool _destroyOld, bool _destroyAfterSwitch, PyObject* _cb )
        {
            LOGGER_INFO(m_serviceProvider)( "set current scene '%s'"
                , _name.c_str()
                );

            PLAYER_SERVICE(m_serviceProvider)
                ->setCurrentScene( _name, _destroyOld, _destroyAfterSwitch, _cb );
        }
        //////////////////////////////////////////////////////////////////////////
        Scene * getCurrentScene()
        {
            Scene * scene = PLAYER_SERVICE(m_serviceProvider)
                ->getCurrentScene();

            return scene;
        }
        //////////////////////////////////////////////////////////////////////////
        Scene * s_createScene( const ConstString & _name )
        {
            Scene * scene = SCENE_SERVICE(m_serviceProvider)
                ->createScene( _name );

            return scene;
        }
        //////////////////////////////////////////////////////////////////////////
        void s_setArrow( const ConstString & _prototype )
        {
            Arrow * arrow = ARROW_SERVICE(m_serviceProvider)
                ->createArrow( _prototype );

            if( arrow == 0 )
            {
                LOGGER_ERROR(m_serviceProvider)( "Error: can't setup arrow '%s'"
                    , _prototype.c_str()
                    );

                return;
            }

            PLAYER_SERVICE(m_serviceProvider)
                ->setArrow( arrow );
        }
        //////////////////////////////////////////////////////////////////////////
        Arrow * s_getArrow()
        {
            Arrow * arrow = PLAYER_SERVICE(m_serviceProvider)
                ->getArrow();

            return arrow;
        }
        //////////////////////////////////////////////////////////////////////////
        const Resolution & s_getCurrentResolution()
        {
            return APPLICATION_SERVICE(m_serviceProvider)
                ->getCurrentResolution();
        }
        //////////////////////////////////////////////////////////////////////////
        const Resolution & s_getContentResolution()
        {
            return APPLICATION_SERVICE(m_serviceProvider)
                ->getContentResolution();
        }
        //////////////////////////////////////////////////////////////////////////
        void s_setArrowLayer( Layer * _layer )
        {
            Arrow * arrow = PLAYER_SERVICE(m_serviceProvider)
                ->getArrow();

            _layer->addChildren( arrow );
        }
        //////////////////////////////////////////////////////////////////////////
        mt::vec2f s_getHotSpotImageSize( HotSpotImage * _hotspotImage )
        {
            if( _hotspotImage == 0 || _hotspotImage->isCompile() == false )
            {
                return mt::vec2f(0.f,0.f);
            }

            ResourceHIT * resourceHIT = _hotspotImage->getResourseHIT();

            mt::vec2f size;
            size.x = (float)resourceHIT->getWidth();
            size.y = (float)resourceHIT->getHeight();

            return size;
        }
        //////////////////////////////////////////////////////////////////////////
        const mt::vec2f& s_getCamera2DPosition()
        {
            return PLAYER_SERVICE(m_serviceProvider)
                ->getRenderCamera2D()->getViewport().begin;
        }
        //////////////////////////////////////////////////////////////////////////
        size_t s_Animatable_play( Animatable * _animatable )
        {
            float time = PLAYER_SERVICE(m_serviceProvider)
                ->getTime();

            size_t id = _animatable->play( time );

            return id;
        }
        //////////////////////////////////////////////////////////////////////////
        void destroyNode( Node * _node )
        {
            _node->destroy();
        }
        //////////////////////////////////////////////////////////////////////////
        Node * createNode( const ConstString & _type )
        {
            Node * node = NODE_SERVICE(m_serviceProvider)
                ->createNode( _type );

            if( node == NULL )
            {
                return NULL;
            }

            NODE_SERVICE(m_serviceProvider)
                ->addHomeless( node );
        
            return node;
        }
        //////////////////////////////////////////////////////////////////////////
        void quitApplication()
        {
            APPLICATION_SERVICE(m_serviceProvider)->quit();
        }
        //////////////////////////////////////////////////////////////////////////
        bool directResourceCompile( const ConstString& _nameResource )
        {
            bool result = RESOURCE_SERVICE(m_serviceProvider)
                ->directResourceCompile( _nameResource );

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
        Reference * s_getResourceReference( const ConstString& _nameResource )
        {
            Reference * reference = RESOURCE_SERVICE(m_serviceProvider)
                ->getResourceReference( _nameResource );

            if(reference == NULL )
            {
                LOGGER_ERROR(m_serviceProvider)( "Error: (Menge.getResourceReference) not exist resource %s"
                    , _nameResource.c_str() 
                    );

                return NULL;
            }

            return reference;
        }
        //////////////////////////////////////////////////////////////////////////
        void directResourceRelease( const ConstString& _nameResource )
        {
            RESOURCE_SERVICE(m_serviceProvider)
                ->directResourceRelease( _nameResource );
        }
        //////////////////////////////////////////////////////////////////////////
        void s_cancelTask( ThreadTask * _task )
        {
            THREAD_SERVICE(m_serviceProvider)
                ->cancelTask( _task );
        }
        //////////////////////////////////////////////////////////////////////////
        void s_joinTask( ThreadTask * _task )
        {
            THREAD_SERVICE(m_serviceProvider)
                ->joinTask( _task );
        }
        //////////////////////////////////////////////////////////////////////////
        void s_setFullscreenMode( bool _fullscreen )
        {
            APPLICATION_SERVICE(m_serviceProvider)
                ->setFullscreenMode( _fullscreen );
        }
        //////////////////////////////////////////////////////////////////////////
        bool s_getFullscreenMode()
        {
            return APPLICATION_SERVICE(m_serviceProvider)
                ->getFullscreenMode();
        }
        //////////////////////////////////////////////////////////////////////////
        void s_setFixedContentResolution( bool _fullscreen )
        {
            APPLICATION_SERVICE(m_serviceProvider)
                ->setFixedContentResolution( _fullscreen );
        }
        //////////////////////////////////////////////////////////////////////////
        bool s_getFixedContentResolution()
        {
            return APPLICATION_SERVICE(m_serviceProvider)
                ->getFixedContentResolution();
        }
        //////////////////////////////////////////////////////////////////////////
        void renderOneFrame()
        {
            RENDER_SERVICE(m_serviceProvider)
                ->beginScene();

            GAME_SERVICE(m_serviceProvider)
                ->render();

            RENDER_SERVICE(m_serviceProvider)
                ->endScene();

            RENDER_SERVICE(m_serviceProvider)
                ->swapBuffers();
        }
        //////////////////////////////////////////////////////////////////////////
        void writeImageToFile( const ConstString& _resource, const FilePath& _filename )
        {
            ResourceImage * resource = RESOURCE_SERVICE(m_serviceProvider)
                ->getResourceT<ResourceImage>( _resource );

            if( resource == 0 )
            {
                LOGGER_ERROR(m_serviceProvider)( "Error: Image resource not getting '%s'"
                    , _resource.c_str() 
                    );

                return;
            }

            RenderTextureInterface * img = resource->getTexture();

            RENDER_SERVICE(m_serviceProvider)
                ->saveImage( img, CONST_STRING(m_serviceProvider, user), _filename );
        }
        //////////////////////////////////////////////////////////////////////////
        void setParticlesEnabled( bool _enabled )
        {
            APPLICATION_SERVICE(m_serviceProvider)
                ->setParticlesEnabled( _enabled );
        }
        //////////////////////////////////////////////////////////////////////////
        void s_createImageResource( const ConstString& _resourceName, const ConstString& _pakName, const FilePath& _filename )
        {
            ResourceImageDefault* resImage = RESOURCE_SERVICE(m_serviceProvider)
                ->getResourceT<ResourceImageDefault>( _resourceName );

            if( resImage == NULL )
            {
                resImage = RESOURCE_SERVICE(m_serviceProvider)
                    ->createResourceT<ResourceImageDefault>( _pakName, ConstString::none(), _resourceName, CONST_STRING(m_serviceProvider, ResourceImageDefault) );
            }

            resImage->setImagePath( _filename );
        }
        //////////////////////////////////////////////////////////////////////////
        void minimizeWindow()
        {
            APPLICATION_SERVICE(m_serviceProvider)
                ->minimizeWindow();
        }
        //////////////////////////////////////////////////////////////////////////
        void s_setMouseBounded( bool _bounded )
        {
            bool mouseBound = APPLICATION_SERVICE(m_serviceProvider)->getMouseBounded();

            if( mouseBound != _bounded )
            {
                APPLICATION_SERVICE(m_serviceProvider)
                    ->setMouseBounded( _bounded );
            }
        }
        //////////////////////////////////////////////////////////////////////////
        bool s_getMouseBounded()
        {
            return APPLICATION_SERVICE(m_serviceProvider)
                ->getMouseBounded();
        }
        //////////////////////////////////////////////////////////////////////////
        const mt::vec2f & s_getCursorPosition()
        {
            const mt::vec2f & pos = INPUT_SERVICE(m_serviceProvider)
                ->getCursorPosition();

            return pos;
        }
        //////////////////////////////////////////////////////////////////////////
        bool s_isInViewport( const mt::vec2f & _pos )
        {
            return PLAYER_SERVICE(m_serviceProvider)
                ->getRenderCamera2D()->getViewport().testPoint( _pos );
        }
        //////////////////////////////////////////////////////////////////////////
        void s_enableTextureFiltering( bool _enable )
        {
            RENDER_SERVICE(m_serviceProvider)
                ->enableTextureFiltering( _enable );
        }
        //////////////////////////////////////////////////////////////////////////
        bool s_isTextureFilteringEnabled()
        {
            return RENDER_SERVICE(m_serviceProvider)
                ->isTextureFilteringEnabled();
        }
        //////////////////////////////////////////////////////////////////////////
        bool s_existText( const ConstString & _key )
        {
            bool exist = TEXT_SERVICE(m_serviceProvider)->existText( _key, NULL );

            return exist;
        }
        //////////////////////////////////////////////////////////////////////////
        PyObject * s_pickHotspot( const mt::vec2f & _point )
        {
            TVectorPickerTraps traps;

            MousePickerSystemInterface * mousePickerSystem = PLAYER_SERVICE(m_serviceProvider)
                ->getMousePickerSystem();

            mousePickerSystem->pickTrap( _point, traps );

            PyObject * pyret = pybind::list_new(0);

            bool onFocus = APPLICATION_SERVICE(m_serviceProvider)
                ->isFocus();

            if( onFocus == false )
            {
                return pyret;
            }

            for( TVectorPickerTraps::iterator
                it = traps.begin(),
                it_end = traps.end();
            it != it_end;
            ++it )
            {
                MousePickerTrapInterface * mousePickerTrap = (*it);

                PyObject * py_embedding = mousePickerTrap->getPickerEmbed();

                pybind::list_appenditem( pyret, py_embedding );
            }

            return pyret;
        }
        //////////////////////////////////////////////////////////////////////////
        const String & s_getPlatformName()
        {
            const String & platformName = APPLICATION_SERVICE(m_serviceProvider)
                ->getPlatformName();

            return platformName;
        }
        //////////////////////////////////////////////////////////////////////////
        mt::vec2f s_getImageSize( Sprite * _sprite )
        {
            ResourceImage * resource = _sprite->getImageResource();

            mt::vec2f size;

            if( resource != NULL )
            {
                size = resource->getSize();
            }
            else
            {
                const ConstString & resourceName = _sprite->getImageResourceName();

                ResourceImage * resource = RESOURCE_SERVICE(m_serviceProvider)
                    ->getResourceT<ResourceImage>( resourceName );

                if( resource == NULL )
                {
                    size.x = 0.f;
                    size.y = 0.f;

                    return size;
                }

                size = resource->getSize();

                resource->decrementReference();
            }
            
            return size;
        }
        //////////////////////////////////////////////////////////////////////////
        mt::vec2f s_getLocalImageCenter( Sprite * _sprite )
        {
            const mt::vec2f & imageSize = s_getImageSize( _sprite );

            mt::vec2f imageCenter(imageSize.x * 0.5f, imageSize.y * 0.5f);

            return imageCenter;
        }
        //////////////////////////////////////////////////////////////////////////
        mt::vec2f s_getWorldImageCenter( Sprite * _sprite )
        {
            mt::vec2f imageCenter = s_getLocalImageCenter( _sprite );

            const mt::mat4f & wm = _sprite->getWorldMatrix();

            mt::vec2f imageCenter_wm;
            mt::mul_v2_m4( imageCenter_wm, imageCenter, wm );

            return imageCenter_wm;
        }
        //////////////////////////////////////////////////////////////////////////
        Node * createChildren( Node * _node, const ConstString & _type )
        {
            Node * newNode = this->createNode(_type );

            if( newNode == NULL )
            {
                return NULL;
            }

            _node->addChildren( newNode );

            return newNode;
        }
        //////////////////////////////////////////////////////////////////////////
        void moveStop( Node * _node )
        {
            _node->stopAffectors( ETA_POSITION );
            _node->setLinearSpeed( mt::zero_v3 );
        }
        //////////////////////////////////////////////////////////////////////////
        size_t velocityTo( Node * _node, float _speed, const mt::vec3f& _dir, PyObject* _cb )
        {
            if( _node->isActivate() == false )
            {
                return 0;
            }

            moveStop( _node );

            Affector * affector = 
                NodeAffectorCreator::newNodeAffectorAccumulateLinear(
                m_serviceProvider,
                _cb, ETA_POSITION, _node, &Node::setLocalPosition
                , _node->getLocalPosition(), _dir, _speed, &mt::length_v3
                );

            mt::vec3f linearSpeed = _dir * _speed;
            _node->setLinearSpeed( linearSpeed );

            size_t id = _node->addAffector( affector );

            return id;
        }
        //////////////////////////////////////////////////////////////////////////
        size_t moveTo( Node * _node, float _time, const mt::vec3f& _point, PyObject* _cb )
        {
            if( _node->isActivate() == false )
            {
                return 0;
            }

            moveStop( _node );

            Affector* affector = 
                NodeAffectorCreator::newNodeAffectorInterpolateLinear(
                m_serviceProvider,
                _cb, ETA_POSITION, _node, &Node::setLocalPosition
                , _node->getLocalPosition(), _point, _time
                , &mt::length_v3 
                );

            float invTime = 1.0f / _time;
            const mt::vec3f & pos = _node->getLocalPosition();
            mt::vec3f linearSpeed = ( _point - pos ) * invTime;

            _node->setLinearSpeed( linearSpeed );

            size_t id = _node->addAffector( affector );

            return id;
        }
        //////////////////////////////////////////////////////////////////////////
        size_t accMoveTo( Node * _node, float _time, const mt::vec3f& _point, PyObject* _cb )
        {
            if( _node->isActivate() == false )
            {
                return 0;
            }

            mt::vec3f linearSpeed = _node->getLinearSpeed();

            moveStop( _node );

            Affector* affector = 
                NodeAffectorCreator::newNodeAffectorInterpolateQuadratic(
                m_serviceProvider,
                _cb, ETA_POSITION, _node, &Node::setLocalPosition
                , _node->getLocalPosition(), _point, linearSpeed, _time
                , &mt::length_v3
                );

            size_t id = _node->addAffector( affector );

            return id;
        }
        //////////////////////////////////////////////////////////////////////////
        size_t bezier2To( Node * _node
            , float _time
            , const mt::vec3f& _point1
            , const mt::vec3f& _point2
            , PyObject* _cb )
        {
            if( _node->isActivate() == false )
            {
                return 0;
            }

            moveStop( _node );

            Affector* affector = 
                NodeAffectorCreator::newNodeAffectorInterpolateQuadraticBezier(
                m_serviceProvider,
                _cb, ETA_POSITION, _node, &Node::setLocalPosition
                , _node->getLocalPosition(), _point1, _point2, _time
                , &mt::length_v3
                );

            size_t id = _node->addAffector( affector );

            return id;
        }
        //////////////////////////////////////////////////////////////////////////
        size_t bezier3To( Node * _node
            , float _time
            , const mt::vec3f& _point1
            , const mt::vec3f& _point2
            , const mt::vec3f& _point3
            , PyObject* _cb )
        {
            if( _node->isActivate() == false )
            {
                return 0;
            }

            moveStop( _node );

            Affector* affector = 
                NodeAffectorCreator::newNodeAffectorInterpolateCubicBezier(
                m_serviceProvider,
                _cb, ETA_POSITION, _node, &Node::setLocalPosition
                , _node->getLocalPosition(), _point1, _point2, _point3, _time
                , &mt::length_v3
                );

            size_t id = _node->addAffector( affector );

            return id;
        }
        //////////////////////////////////////////////////////////////////////////
        void angleStop( Node * _node )
        {
            _node->stopAffectors( ETA_ANGLE );
            _node->setAngularSpeed(0.f);
        }
        //////////////////////////////////////////////////////////////////////////
        size_t angleTo( Node * _node, float _time, float _angle, PyObject* _cb )
        {
            if( _node->isActivate() == false )
            {
                return 0;
            }

            angleStop( _node );

            float angle = _node->getRotateZ();

            float correct_angle_from = angle;
            float correct_angle_to = _angle;
            //mt::angle_correct_interpolate_from_to( angle, _angle, correct_angle_from, correct_angle_to );

            Affector* affector =
                NodeAffectorCreator::newNodeAffectorInterpolateLinear(
                m_serviceProvider,
                _cb, ETA_ANGLE, _node, &Node::setRotateX
                , correct_angle_from, correct_angle_to, _time
                , &fabsf 
                );				

            float invTime = 1.0f / _time;
            float angularSpeed = fabsf( correct_angle_from - correct_angle_to ) * invTime;

            _node->setAngularSpeed( angularSpeed );				
            size_t id = _node->addAffector( affector );

            return id;
        }
        //////////////////////////////////////////////////////////////////////////
        size_t accAngleTo( Node * _node, float _time, float _angle, PyObject* _cb )
        {
            if( _node->isActivate() == false )
            {
                return 0;
            }

            float angularSpeed = _node->getAngularSpeed();

            angleStop( _node );

            float angle = _node->getRotateZ();

            float correct_angle_from = angle;
            float correct_angle_to = _angle;
            //mt::angle_correct_interpolate_from_to( angle, _angle, correct_angle_from, correct_angle_to );

            Affector* affector = 
                NodeAffectorCreator::newNodeAffectorInterpolateQuadratic(
                m_serviceProvider,
                _cb, ETA_ANGLE, _node, &Node::setRotateX
                , correct_angle_from, correct_angle_to, angularSpeed, _time
                , &fabsf
                );				

            size_t id = _node->addAffector( affector );

            return id;
        }
        //////////////////////////////////////////////////////////////////////////
        void scaleStop( Node * _node )
        {
            _node->stopAffectors( ETA_SCALE );
        }	
        //////////////////////////////////////////////////////////////////////////
        size_t scaleTo( Node * _node, float _time, const mt::vec3f& _scale, PyObject* _cb )
        {
            if( _node->isActivate() == false )
            {
                return 0;
            }

            scaleStop( _node );

            Affector* affector = 
                NodeAffectorCreator::newNodeAffectorInterpolateLinear(
                m_serviceProvider,
                _cb, ETA_SCALE, _node, &Node::setScale
                , _node->getScale(), _scale, _time
                , &mt::length_v3
                );

            size_t id = _node->addAffector( affector );

            return id;
        }
        //////////////////////////////////////////////////////////////////////////
        void colorStop( Node * _node )
        {
            _node->stopAffectors( ETA_COLOR );
        }
        //////////////////////////////////////////////////////////////////////////
        size_t colorTo( Node * _node, float _time, const ColourValue& _color, PyObject* _cb )
        {
            if( _node->isActivate() == false )
            {
                return 0;
            }

            colorStop( _node );

            Affector* affector = 
                NodeAffectorCreator::newNodeAffectorInterpolateLinear(
                m_serviceProvider,
                _cb, ETA_COLOR, _node, &Colorable::setLocalColor
                , _node->getLocalColor(), _color, _time, 
                &ColourValue::length_color
                );

            size_t id = _node->addAffector( affector );

            return id;
        }
        //////////////////////////////////////////////////////////////////////////
        size_t alphaTo( Node * _node, float _time, float _alpha, PyObject* _cb )
        {
            if( _node->isActivate() == false )
            {
                return 0;
            }

            ColourValue color = _node->getLocalColor();
            color.setA( _alpha );

            size_t id = colorTo( _node, _time, color, _cb );

            return id;
        }
        //////////////////////////////////////////////////////////////////////////
        size_t setPercentVisibilityTo( Sprite * _sprite, float _time, const mt::vec4f& _percent, PyObject* _cb )
        {
            if( _sprite->isActivate() == false )
            {
                return 0;
            }

            _sprite->stopAffectors( ETA_VISIBILITY );

            Affector* affector = 
                NodeAffectorCreator::newNodeAffectorInterpolateLinear(
                m_serviceProvider,
                _cb, ETA_VISIBILITY, _sprite, &Sprite::setPercentVisibility
                , _sprite->getPercentVisibility(), _percent, _time, 
                &mt::length_v4 
                );

            size_t id = _sprite->addAffector( affector );

            return id;
        }
        //////////////////////////////////////////////////////////////////////////
        void setPercentVisibilityStop( Sprite * _sprite )
        {
            _sprite->stopAffectors( ETA_VISIBILITY );
        }
        //////////////////////////////////////////////////////////////////////////
        PyObject * s_filterTag( Node * _node, const ConstString & _tag )
        {
            PyObject * py_filter = pybind::list_new(0);

            const TListNodeChild & child = _node->getChild();

            for( TListNodeChild::const_iterator
                it = child.begin(),
                it_end = child.end();
            it != it_end;
            ++it )
            {
                Node * children = *it;

                const ConstString & tag = children->getTag();

                if( _tag != tag )
                {
                    continue;
                }

                PyObject * py_embedding = it->getEmbed();

                pybind::list_appenditem( py_filter, py_embedding );

            }

            return py_filter;			
        }
        //////////////////////////////////////////////////////////////////////////
        mt::vec2f s_getCameraPosition( Node * _node, RenderCameraInterface * _renderCamera )
        {
            mt::vec2f position;
            _node->getCameraPosition( _renderCamera, position );
            
            return position;
        }
        //////////////////////////////////////////////////////////////////////////
        Join * s_addJoin( Node * _left, Node * _right, const mt::vec2f & _offset )
        {
            Join * join = PLAYER_SERVICE(m_serviceProvider)
                ->addJoin( _left, _right, _offset );

            return join;
        }
        //////////////////////////////////////////////////////////////////////////
        void s_removeJoin( Join * _join )
        {
            PLAYER_SERVICE(m_serviceProvider)
                ->removeJoin( _join );
        }
        //////////////////////////////////////////////////////////////////////////
        bool s_isJoin( Node * _left, Node * _right )
        {
            bool result = PLAYER_SERVICE(m_serviceProvider)
                ->isJoin( _left, _right );

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
        PyObject * s_getJoins( Node * _left )
        {
            PyObject * py_list = pybind::list_new(0);

            TVectorNode joins;

            PLAYER_SERVICE(m_serviceProvider)
                ->getJoins( _left, joins );

            for( TVectorNode::iterator
                it = joins.begin(),
                it_end = joins.end();
            it != it_end;
            ++it )
            {
                PyObject * py_node = pybind::ptr(*it);

                pybind::list_appenditem( py_list, py_node );

                pybind::decref( py_node );
            }

            return py_list;
        }
        //////////////////////////////////////////////////////////////////////////
        Camera2D * s_getRenderCamera2D()
        {
            Camera2D * renderCamera2D = PLAYER_SERVICE(m_serviceProvider)
                ->getRenderCamera2D();

            return renderCamera2D;
        }
        //////////////////////////////////////////////////////////////////////////
        class AffectorScript
            : public Affector
        {
        public:
            AffectorScript( ServiceProviderInterface * _serviceProvider, PyObject * _cb, EAffectorType _type )
                : Affector(_serviceProvider, _cb, _type)
                , m_stop(false)
            {
            }

            ~AffectorScript()
            {
            }

        public:
            bool affect( float _timing ) override
            {
                if( m_stop == true )
                {
                    return true;
                }

                PyObject * py_result = pybind::ask( m_cb, "(f)", _timing );

                if( py_result == NULL )
                {
                    return false;
                }

                if( pybind::bool_check(py_result) == false )
                {
                    return false;
                }

                if( pybind::is_true(py_result) == true )
                {
                    return true;
                }

                return false;
            }

            void complete() override
            {
                //To Do
            };

            void stop() override
            {
                m_stop = true;
            }

        public:
            bool m_stop;		
        };
        //////////////////////////////////////////////////////////////////////////
        Affector * createAffector( PyObject * _cb )
        {
            Affector * affector = new AffectorScript( m_serviceProvider, _cb, ETA_SCRIPT );

            return affector;
        }
        //////////////////////////////////////////////////////////////////////////
        void showKeyboard()
        {
            APPLICATION_SERVICE(m_serviceProvider)
                ->showKeyboard();
        }
        //////////////////////////////////////////////////////////////////////////
        void hideKeyboard()
        {
            APPLICATION_SERVICE(m_serviceProvider)
                ->hideKeyboard();
        }
        //////////////////////////////////////////////////////////////////////////
        bool hasResource( const ConstString & _name )
        {
            return RESOURCE_SERVICE(m_serviceProvider)
                ->hasResource( _name );
        }
        //////////////////////////////////////////////////////////////////////////
        void removeCurrentScene( PyObject * _cb )
        {
            PLAYER_SERVICE(m_serviceProvider)
                ->removeCurrentScene( _cb );
        }
        //////////////////////////////////////////////////////////////////////////
        void setDefaultResourceFontName( const ConstString & _fontName )
        {
            TEXT_SERVICE(m_serviceProvider)
                ->setDefaultResourceFontName( _fontName );
        }
        //////////////////////////////////////////////////////////////////////////
        void s_addMouseMoveHandler( PyObject * _cb )
        {
            GAME_SERVICE(m_serviceProvider)
                ->addMouseMoveHandler( _cb );
        }
        //////////////////////////////////////////////////////////////////////////
        bool s_removeMouseMoveHandler( PyObject * _cb )
        {
            bool result = GAME_SERVICE(m_serviceProvider)
                ->removeMouseMoveHandler( _cb );

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
        void s_addMouseButtonHandler( PyObject * _cb )
        {
            GAME_SERVICE(m_serviceProvider)
                ->addMouseButtonHandler( _cb );
        }
        //////////////////////////////////////////////////////////////////////////
        bool s_removeMouseButtonHandler( PyObject * _cb )
        {
            bool result = GAME_SERVICE(m_serviceProvider)
                ->removeMouseButtonHandler( _cb );

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
        float s_getMovieDuration( const ConstString & _resourceName )
        {
            ResourceMovie * resourceMovie = RESOURCE_SERVICE(m_serviceProvider)
                ->getResourceT<ResourceMovie>( _resourceName );

            if( resourceMovie == NULL )
            {
                LOGGER_ERROR(m_serviceProvider)("Mengine.getMovieDuration invalid movie resource '%s'"
                    , _resourceName.c_str()
                    );

                return 0.f;
            }

            float duration = resourceMovie->getDuration();

            return duration;
        }
        //////////////////////////////////////////////////////////////////////////
        bool s_addEntityPrototype( const ConstString & _prototype, PyObject * _module )
        {
            bool result = ENTITY_SERVICE(m_serviceProvider)
                ->addPrototype( _prototype, _module );

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
        bool s_addScenePrototype( const ConstString & _prototype, PyObject * _module )
        {
            bool result = SCENE_SERVICE(m_serviceProvider)
                ->addPrototype( _prototype, _module );

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
        Viewport s_getGameViewport()
        {
            float aspect;
            Viewport viewport;

            APPLICATION_SERVICE(m_serviceProvider)
                ->getGameViewport( aspect, viewport );

            return viewport;
        }
        //////////////////////////////////////////////////////////////////////////
        bool s_addArrowPrototype( const ConstString & _prototype, PyObject * _module )
        {
            bool result = ARROW_SERVICE(m_serviceProvider)
                ->addPrototype( _prototype, _module );

            return result;
        }
        //////////////////////////////////////////////////////////////////////////
        bool s_hasGameParam( const WString & _paramName )
        {
            if(	GAME_SERVICE(m_serviceProvider)
                ->hasParam( _paramName ) == false )
            {
                return false;
            }

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
        PyObject * s_getGameParam( const WString & _paramName )
        {
            if(	GAME_SERVICE(m_serviceProvider)
                ->hasParam( _paramName ) == false )
            {
                return pybind::ret_none();
            }

            const WString & val = GAME_SERVICE(m_serviceProvider)
                ->getParam( _paramName );

            return pybind::ptr(val);
        }
        //////////////////////////////////////////////////////////////////////////
        PyObject * s_getGameParamFloat( const WString & _paramName )
        {
            if(	GAME_SERVICE(m_serviceProvider)
                ->hasParam( _paramName ) == false )
            {
                return pybind::ret_none();
            }

            const WString & val = GAME_SERVICE(m_serviceProvider)
                ->getParam( _paramName );

            float value;
            Utils::wstringToFloat( val, value );

            return pybind::ptr(value);
        }
        //////////////////////////////////////////////////////////////////////////
        PyObject * s_getGameParamInt( const WString & _paramName )
        {
            if(	GAME_SERVICE(m_serviceProvider)
                ->hasParam( _paramName ) == false )
            {
                return pybind::ret_none();
            }

            const WString & val = GAME_SERVICE(m_serviceProvider)
                ->getParam( _paramName );

            int value;
            Utils::wstringToInt( val, value );

            return pybind::ptr(value);
        }
        //////////////////////////////////////////////////////////////////////////
        PyObject * s_getGameParamBool( const WString & _paramName )
        {
            if(	GAME_SERVICE(m_serviceProvider)
                ->hasParam( _paramName ) == false )
            {
                return pybind::ret_none();
            }

            const WString & val = GAME_SERVICE(m_serviceProvider)
                ->getParam( _paramName );

            int value;
            Utils::wstringToInt( val, value );

            bool b_value = value != 0;
            
            return pybind::ret_bool( b_value );
        }
        //////////////////////////////////////////////////////////////////////////
        bool s_openUrlInDefaultBrowser( const WString & _url )
        {
            PlatformInterface * platform = APPLICATION_SERVICE(m_serviceProvider)
                ->getPlatform();

            bool val = platform->openUrlInDefaultBrowser( _url );

            return val;
        }
        //////////////////////////////////////////////////////////////////////////
        PyObject * s_getNullObjectsFromResourceVideo( const ConstString & _resourceName )
        {			
            class ResourceMovieVisitorNullLayers
                : public VisitorResourceMovie
            {
            public:
                ResourceMovieVisitorNullLayers( ServiceProviderInterface * _serviceProvider, PyObject * _dictResult, float _frameDuration )
                    : m_serviceProvider(_serviceProvider)
                    , m_dictResult(_dictResult)
                    , m_frameDuration( _frameDuration )
                {}

            protected:
                void visitLayer( const MovieLayer & _layer, const MovieLayerFrame & _frames ) override
                {
                    if( _layer.source != CONST_STRING(m_serviceProvider, MovieSlot) )
                    {
                        return;
                    }

                    PyObject * py_list_frames = pybind::list_new(0);

                    if( _frames.immutable == false )
                    {
                        for( TVectorMovieFrameSource::size_type
                            it_frame = 0,
                            it_frame_end = _frames.frames.size();
                        it_frame != it_frame_end;
                        ++it_frame )
                        {
                            const MovieFrameSource & frame_source = _frames.frames[it_frame];

                            PyObject * py_dict_frame = pybind::dict_new();

                            PyObject * py_pos = pybind::ptr( frame_source.position );
                            pybind::dict_set( py_dict_frame, "position", py_pos );
                            pybind::decref( py_pos );

                            float frameTime = _layer.in + it_frame * m_frameDuration;

                            PyObject * py_time = pybind::ptr( frameTime );
                            pybind::dict_set( py_dict_frame, "time", py_time );
                            pybind::decref( py_time );

                            pybind::list_appenditem( py_list_frames, py_dict_frame );
                            pybind::decref( py_dict_frame );
                        }
                    }
                    else
                    {
                        for( size_t
                            it_frame = 0,
                            it_frame_end = _frames.count;
                        it_frame != it_frame_end;
                        ++it_frame )
                        {
                            PyObject * py_dict_frame = pybind::dict_new();

                            PyObject * py_pos = pybind::ptr( _frames.source.position );
                            pybind::dict_set( py_dict_frame, "position", py_pos );
                            pybind::decref( py_pos );

                            float frameTime = _layer.in + it_frame * m_frameDuration;

                            PyObject * py_time = pybind::ptr( frameTime );
                            pybind::dict_set( py_dict_frame, "time", py_time );
                            pybind::decref( py_time );

                            pybind::list_appenditem( py_list_frames, py_dict_frame );
                            pybind::decref( py_dict_frame );
                        }
                    }

                    pybind::dict_set( m_dictResult, _layer.name.c_str(), py_list_frames );
                    pybind::decref( py_list_frames );
                }

            protected:
                ServiceProviderInterface * m_serviceProvider;
                PyObject * m_dictResult;
                float m_frameDuration;
            };		


            ResourceMovie* resource = RESOURCE_SERVICE(m_serviceProvider)
                ->getResourceT<ResourceMovie>( _resourceName );

            if( resource == NULL )
            {
                return pybind::ret_none();
            }

            PyObject * py_dict_result = pybind::dict_new();

            float frameTime = resource->getFrameDuration();
            ResourceMovieVisitorNullLayers visitor(m_serviceProvider, py_dict_result, frameTime);

            resource->visitResourceMovie( &visitor );

            return py_dict_result;
        }
        //////////////////////////////////////////////////////////////////////////
        PyObject * s_visitResourceEmitterContainer( const ConstString & _resourceName )
        {
            class ResourceEmitterContainerVisitor:
                public EmitterContainerInterface::EmitterContainerVisitor
            {
            public:
                void visitEmitterName( const ConstString& _emitterName )
                {
                    catchedNames.push_back( _emitterName );
                }

                void visitAtlas( const EmitterContainerInterface::EmitterAtlas & _atlas )
                {
                    cathedAtlas.push_back( _atlas );
                }

                EmitterContainerInterface::TVectorAtlas cathedAtlas;
                TVectorConstString catchedNames;
            };

            ResourceEmitterContainer* resource = RESOURCE_SERVICE(m_serviceProvider)
                ->getResourceT<ResourceEmitterContainer>( _resourceName );

            if( resource == NULL )
            {
                return pybind::ret_none();
            }

            PyObject * py_dict_result = pybind::dict_new();

            PyObject * py_list_names = pybind::list_new(0);
            PyObject * py_list_atlas = pybind::list_new(0);

            ResourceEmitterContainerVisitor visitor;
            EmitterContainerInterface * container = resource->getContainer();
            container->visitContainer( &visitor );

            for( TVectorConstString::const_iterator
                it = visitor.catchedNames.begin(),
                it_end = visitor.catchedNames.end();
            it != it_end;
            ++it )
            {
                PyObject * py_node = pybind::ptr(*it);
                pybind::list_appenditem( py_list_names, py_node );
                pybind::decref( py_node );
            }

            for( EmitterContainerInterface::TVectorAtlas::const_iterator
                it = visitor.cathedAtlas.begin(),
                it_end = visitor.cathedAtlas.end();
            it != it_end;
            ++it )
            {
                PyObject * py_dict = pybind::dict_new();
                PyObject * py_node_file = pybind::ptr( (*it).file );
                //PyObject * py_node_path = pybind::ptr((*it).path);

                pybind::dict_set( py_dict, "file", py_node_file );
                //pybind::dict_set(py_dict,"path",py_node_path);

                pybind::decref( py_node_file );
                //pybind::decref( py_node_path );
                pybind::list_appenditem( py_list_atlas, py_dict );
            }

            pybind::dict_set( py_dict_result, "Emitters", py_list_names );
            pybind::dict_set( py_dict_result, "Atlasses", py_list_atlas );

            return py_dict_result;
        }
        //////////////////////////////////////////////////////////////////////////
        void s_visitChild( Node * _node, PyObject * _cb )
        {
            const TListNodeChild & child = _node->getChild();

            for( TListNodeChild::const_iterator
                it = child.begin(),
                it_end = child.end();
            it != it_end;
            ++it )
            {
                Node * children = *it;
                PyObject * py_children = pybind::ptr( children);

                pybind::call( _cb, "(O)", py_children );

                pybind::decref( py_children );
            }
        }
        //////////////////////////////////////////////////////////////////////////
        void s_visitCompiledResources( const ConstString & _category, const ConstString & _groupName, PyObject * _cb )
        {
            class ResourceVisitorGetAlreadyCompiled
                : public ResourceVisitor
            {
            public:
                ResourceVisitorGetAlreadyCompiled( PyObject * _cb )
                    : m_cb(_cb)
                {
                    pybind::incref( m_cb );
                }

                ~ResourceVisitorGetAlreadyCompiled()
                {
                    pybind::decref( m_cb );
                }

            protected:
                void visit( ResourceReference* _resource )
                {
                    if( _resource->isCompile() == false )
                    {
                        return;
                    }

                    PyObject * py_resource = pybind::ptr( _resource );

                    pybind::call( m_cb, "(O)", py_resource );

                    pybind::decref( py_resource );
                }

            protected:
                PyObject * m_cb;
            };

            ResourceVisitorGetAlreadyCompiled rv_gac(_cb);

            RESOURCE_SERVICE(m_serviceProvider)
                ->visitResources( _category, _groupName, &rv_gac );						
        }
        //////////////////////////////////////////////////////////////////////////
        void s_visitResources( const ConstString & _category, const ConstString & _groupName, PyObject * _cb )
        {
            class MyResourceVisitor
                : public ResourceVisitor
            {
            public:
                MyResourceVisitor( PyObject * _cb )
                    : m_cb(_cb)
                {
                    pybind::incref( m_cb );
                }

                ~MyResourceVisitor()
                {
                    pybind::decref( m_cb );
                }

            protected:
                void visit( ResourceReference* _resource )
                {
                    PyObject * py_resource = pybind::ptr( _resource );

                    pybind::call( m_cb, "(O)", py_resource );

                    pybind::decref( py_resource );
                }

            protected:
                PyObject * m_cb;
            };

            MyResourceVisitor rv_gac(_cb);

            RESOURCE_SERVICE(m_serviceProvider)
                ->visitResources( _category, _groupName, &rv_gac );						
        }
        //////////////////////////////////////////////////////////////////////////
        bool s_validResource( const ConstString & _resourceName )
        {
            bool valid = RESOURCE_SERVICE(m_serviceProvider)
                ->validResource( _resourceName );

            return valid;
        }

    protected: 
        ServiceProviderInterface * m_serviceProvider;
    };
    //////////////////////////////////////////////////////////////////////////
    static void classWrapping( ServiceProviderInterface * _serviceProvider )
    {
        SCRIPT_CLASS_WRAPPING( _serviceProvider, Node );
        SCRIPT_CLASS_WRAPPING( _serviceProvider, Layer );
        SCRIPT_CLASS_WRAPPING( _serviceProvider, Layer2D );
        SCRIPT_CLASS_WRAPPING( _serviceProvider, Layer2DPhysic );
        SCRIPT_CLASS_WRAPPING( _serviceProvider, HotSpot );
        //SCRIPT_CLASS_WRAPPING( Light2D );
        //SCRIPT_CLASS_WRAPPING( ShadowCaster2D );
        SCRIPT_CLASS_WRAPPING( _serviceProvider, Sprite );
        SCRIPT_CLASS_WRAPPING( _serviceProvider, Animation );
        SCRIPT_CLASS_WRAPPING( _serviceProvider, Arrow );
        SCRIPT_CLASS_WRAPPING( _serviceProvider, TextField );
        SCRIPT_CLASS_WRAPPING( _serviceProvider, SoundEmitter );
        SCRIPT_CLASS_WRAPPING( _serviceProvider, ParticleEmitter );
        SCRIPT_CLASS_WRAPPING( _serviceProvider, Movie );
        SCRIPT_CLASS_WRAPPING( _serviceProvider, MovieSlot );
        SCRIPT_CLASS_WRAPPING( _serviceProvider, MovieInternalObject );
        SCRIPT_CLASS_WRAPPING( _serviceProvider, Point );
        //SCRIPT_CLASS_WRAPPING( TilePolygon );
        SCRIPT_CLASS_WRAPPING( _serviceProvider, Video );
        //SCRIPT_CLASS_WRAPPING( FFCamera3D );
        //SCRIPT_CLASS_WRAPPING( DiscreteEntity );
        //SCRIPT_CLASS_WRAPPING( RigidBody3D );
        //SCRIPT_CLASS_WRAPPING( Layer3D );
        //SCRIPT_CLASS_WRAPPING( RigidBody2D );
        //SCRIPT_CLASS_WRAPPING( PhysicalBody2D );
        //SCRIPT_CLASS_WRAPPING( CapsuleController );
        //SCRIPT_CLASS_WRAPPING( SceneNode3D );
        //SCRIPT_CLASS_WRAPPING( Camera3D );
        //SCRIPT_CLASS_WRAPPING( RenderMesh );
        SCRIPT_CLASS_WRAPPING( _serviceProvider, Window );

        SCRIPT_CLASS_WRAPPING( _serviceProvider, HotSpotImage );
        SCRIPT_CLASS_WRAPPING( _serviceProvider, Camera2D );
        //SCRIPT_CLASS_WRAPPING( Layer2DTexture );
    }

    struct extract_String_type
        : public pybind::type_cast_result<String>
    {
    public:
        bool apply( PyObject * _obj, String & _value ) override
        {
            if( pybind::string_check( _obj ) == true )
            {
                size_t size;
                const Char * string_char = pybind::string_to_char_and_size( _obj, size  );

                if( string_char == 0 )
                {
                    return false;
                }

                _value.assign( string_char, size );
            }
            else
            {
                return false;
            }

            return true;
        }

        PyObject * wrap( pybind::type_cast_result<String>::TCastRef _value ) override
        {
            PyObject * py_value = pybind::string_from_char_size( _value.c_str(), _value.size() );

            return py_value;
        }
    };


    struct extract_WString_type
        : public pybind::type_cast_result<WString>
    {
    public:
        bool apply( PyObject * _obj, WString & _value ) override
        {
            if( pybind::unicode_check( _obj ) == true )
            {
                const pybind_unicode_t * value_char = pybind::unicode_to_wchar( _obj );

                if( value_char == 0 )
                {
                    return false;
                }

                _value.assign( value_char );
            }
            else
            {
                return false;
            }

            return true;
        }

        PyObject * wrap( pybind::type_cast_result<WString>::TCastRef _value ) override
        {
            PyObject * py_value = pybind::unicode_from_wchar_size( _value.c_str(), _value.size() );

            return py_value;
        }
    };

    struct extract_TBlobject_type
        : public pybind::type_cast_result<TBlobject>
    {
        bool apply( PyObject * _obj, TBlobject & _value ) override
        {
            if( pybind::string_check( _obj ) == true )
            {
                size_t size;
                const char * value_char = pybind::string_to_char_and_size( _obj, size );

                if( value_char == 0 )
                {
                    return false;
                }

                _value.assign( value_char, value_char + size );
            }
            else
            {
                return false;
            }

            return true;
        }

        PyObject * wrap( pybind::type_cast_result<TBlobject>::TCastRef _value ) override
        {
            PyObject * py_value = pybind::string_from_char_size( reinterpret_cast<const char *>(&_value[0]), _value.size() );

            return py_value;
        }
    };

    struct extract_TVectorString_type
        : public pybind::type_cast_result<TVectorString>
    {
        bool apply( PyObject * _obj, TVectorString & _value ) override
        {
            if( pybind::list_check( _obj ) == true )
            {
                size_t size = pybind::list_size( _obj );

                for( size_t it = 0; it != size; ++it )
                {
                    PyObject * py_string = pybind::list_getitem( _obj, it );

                    String key;
                    pybind::extract_value( py_string, key );

                    _value.push_back( key );

                    pybind::decref( py_string );
                }
            }
            else
            {
                return false;
            }

            return true;
        }

        PyObject * wrap( pybind::type_cast_result<TVectorString>::TCastRef _value ) override
        {
            PyObject * py_value = pybind::list_new(0);

            for( TVectorString::const_iterator
                it = _value.begin(),
                it_end = _value.end();
            it != it_end;
            ++it )
            {
                PyObject * py_string = pybind::ptr( *it );

                pybind::list_appenditem( py_value, py_string );

                pybind::decref( py_string );
            }

            return py_value;
        }
    };

    struct extract_TVectorWString_type
        : public pybind::type_cast_result<TVectorWString>
    {
        bool apply( PyObject * _obj, TVectorWString & _value ) override
        {
            if( pybind::list_check( _obj ) == true )
            {
                size_t size = pybind::list_size( _obj );

                for( size_t it = 0; it != size; ++it )
                {
                    PyObject * py_string = pybind::list_getitem( _obj, it );

                    WString key;
                    pybind::extract_value( py_string, key );

                    _value.push_back( key );

                    pybind::decref( py_string );
                }
            }
            else
            {
                return false;
            }

            return true;
        }

        PyObject * wrap( pybind::type_cast_result<TVectorWString>::TCastRef _value ) override
        {
            PyObject * py_value = pybind::list_new(0);

            for( TVectorWString::const_iterator
                it = _value.begin(),
                it_end = _value.end();
            it != it_end;
            ++it )
            {
                PyObject * py_string = pybind::ptr( *it );

                pybind::list_appenditem( py_value, py_string );

                pybind::decref( py_string );
            }

            return py_value;
        }
    };

    void ScriptWrapper::nodeWrap( ServiceProviderInterface * _serviceProvider )
    {        
        NodeScriptMethod * nodeScriptMethod = new NodeScriptMethod(_serviceProvider);
        
        pybind::registration_type_cast<String>( new extract_String_type );
        pybind::registration_type_cast<WString>( new extract_WString_type );
        pybind::registration_type_cast<TBlobject>( new extract_TBlobject_type );
        pybind::registration_type_cast<TVectorString>( new extract_TVectorString_type );
        pybind::registration_type_cast<TVectorWString>( new extract_TVectorWString_type );

        classWrapping( _serviceProvider );

        pybind::interface_<Join>("Join")
            .def("getLeft", &Join::getLeft)
            .def("getRight", &Join::getRight)
            .def("getOffset", &Join::getOffset)
            ;

        pybind::interface_<Affector>("Affector", true)
            .def( "stop", &Affector::stop )
            ;

        pybind::interface_<Scriptable>("Scriptable")
            ;

        pybind::interface_<Identity>("Identity")
            .def( "setName", &Identity::setName )
            .def( "getName", &Identity::getName )
            .def( "getType", &Identity::getType )
            .def( "setTag", &Identity::setTag )
            .def( "getTag", &Identity::getTag )
            .def( "getUniqueId", &Identity::getUniqueId )
            ;

        pybind::interface_<Transformation3D>("Transformation3D")
            .def( "setLocalPosition", &Transformation3D::setLocalPosition )
            .def( "getLocalPosition", &Transformation3D::getLocalPosition )
            //.def( "getLocalDirection", &Transformation3D::getLocalDirection )
            //.def( "setLocalDirection", &Transformation3D::setLocalDirection )
            .def( "setCoordinate", &Transformation3D::setCoordinate )			
            .def( "getCoordinate", &Transformation3D::getCoordinate )
            .def( "setOrigin", &Transformation3D::setOrigin )
            .def( "getOrigin", &Transformation3D::getOrigin )
            .def( "setScale", &Transformation3D::setScale )
            .def( "getScale", &Transformation3D::getScale )
            .def( "setRotateX", &Transformation3D::setRotateX )
            .def( "getRotateX", &Transformation3D::getRotateX )
            .def( "setRotateY", &Transformation3D::setRotateY )
            .def( "getRotateY", &Transformation3D::getRotateY )
            .def( "setRotateZ", &Transformation3D::setRotateZ )
            .def( "getRotateZ", &Transformation3D::getRotateZ )
            .def( "setAngle", &Transformation3D::setRotateX )
            .def( "getAngle", &Transformation3D::getRotateX )

            .def( "translate", &Transformation3D::translate )

            .def( "resetTransformation", &Transformation3D::resetTransformation )
            .def( "setRelationTransformation", &Transformation3D::setRelationTransformation )
            .def_proxy_static( "removeRelationTransformation", nodeScriptMethod, &NodeScriptMethod::s_removeRelationTransformation )

            //.def( "setRotate", &Transformation3D::setAngle ) //depricated
            ;

        //pybind::class_<FFCamera3D>("FFCamera3D")
        //	.def( pybind::init<>() )
        //	.def( "update", &FFCamera3D::update )
        //	.def( "activate", &FFCamera3D::activate )
        //	.def( "forward", &FFCamera3D::forward )
        //	.def( "left", &FFCamera3D::left )
        //	.def( "setActor", &FFCamera3D::setActor )
        //	.def( "yaw", &FFCamera3D::yaw )
        //	.def( "pitch", &FFCamera3D::pitch )
        //	.def( "zoom", &FFCamera3D::zoom )
        //	;

        //pybind::interface_<NodeRenderable>("NodeRenderable", false)
        //	.def( "hide", &NodeRenderable::hide )
        //	;

        pybind::interface_<Resource>("Resource")
            .def( "compile", &Resource::compile )
            .def( "release", &Resource::release )
            .def( "isCompile", &Resource::isCompile )
            ;

        pybind::interface_<Reference>("Reference")
            .def( "incrementReference", &Reference::incrementReference )
            .def( "decrementReference", &Reference::decrementReference )
            .def( "countReference", &Reference::countReference )
            ;

        pybind::interface_<ResourceReference, pybind::bases<Resource, Identity, Reference> >("ResourceReference")
            .def( "getCategory", &ResourceReference::getCategory )
            .def( "getGroup", &ResourceReference::getGroup )
            ;

        pybind::interface_<Renderable>("Renderable")
            .def( "hide", &Renderable::hide )
            .def( "isHide", &Renderable::isHide )
            .def( "localHide", &Renderable::localHide )
            .def( "isLocalHide", &Renderable::isLocalHide )
            ;

        pybind::interface_<Colorable>("Colorable")
            .def( "setLocalColor", &Colorable::setLocalColor )
            .def( "getLocalColor", &Colorable::getLocalColor )
            .def( "setLocalAlpha", &Colorable::setLocalAlpha )
            .def( "getLocalAlpha", &Colorable::getLocalAlpha )
            .def( "setPersonalColor", &Colorable::setPersonalColor )
            .def( "getPersonalColor", &Colorable::getPersonalColor )
            .def( "setPersonalAlpha", &Colorable::setPersonalAlpha )
            .def( "getPersonalAlpha", &Colorable::getPersonalAlpha )			
            ;

        pybind::interface_<Animatable>("Animatable")
            .def_proxy_static( "play", nodeScriptMethod, &NodeScriptMethod::s_Animatable_play )
            .def( "stop", &Animatable::stop )
            .def( "isPlay", &Animatable::isPlay )
            .def( "getPlayId", &Animatable::getPlayId )
            .def( "setSpeedFactor", &Animatable::setSpeedFactor )
            .def( "getSpeedFactor", &Animatable::getSpeedFactor )
            .def( "setFirstFrame", &Animatable::setFirstFrame )
            .def( "setLastFrame", &Animatable::setLastFrame )
            .def( "setTiming", &Animatable::setTiming )
            .def( "getTiming", &Animatable::getTiming )
            .def( "setLoop", &Animatable::setLoop )
            .def( "getLoop", &Animatable::getLoop )
            .def( "setInterval", &Animatable::setInterval )
            .def( "getIntervalBegin", &Animatable::getIntervalBegin )
            .def( "getIntervalEnd", &Animatable::getIntervalEnd )
            .def( "interrupt", &ParticleEmitter::interrupt )
            ;

        pybind::interface_<GlobalHandleAdapter>("GlobalHandleAdapter")
            .def( "enableGlobalMouseEvent", &GlobalHandleAdapter::enableGlobalMouseEvent )
            .def( "enableGlobalKeyEvent", &GlobalHandleAdapter::enableGlobalKeyEvent )
            ;

        pybind::interface_<Affectorable>("Affectorable")
            .def( "addAffector", &Affectorable::addAffector )
            ;

        pybind::interface_<Node, pybind::bases<Scriptable, Identity, Transformation3D, Colorable, Resource, Renderable, GlobalHandleAdapter, Affectorable> >("Node", false)
            .def( "enable", &Node::enable )
            .def( "disable", &Node::disable )
            .def( "isEnable", &Node::isEnable )
            .def( "render", &Node::render )
            .def( "isActivate", &Node::isActivate )
            .def( "freeze", &Node::freeze )
            .def( "isFreeze", &Node::isFreeze )
            .def( "addChildren", &Node::addChildren )
            .def( "addChildrenFront", &Node::addChildrenFront )
            .def( "addChildrenAfter", &Node::addChildrenAfter )
            .def( "removeChildren", &Node::removeChildren )
            .def( "removeAllChild", &Node::removeAllChild )
            .def( "removeFromParent", &Node::removeFromParent )
            //.def_static( "getChild", &ScriptMethod::s_getChild )
            .def( "findChildren", &Node::findChildren )
            .def( "findTag", &Node::findTag )
            .def_proxy_static( "filterTag", nodeScriptMethod, &NodeScriptMethod::s_filterTag )
            .def( "emptyChild", &Node::emptyChild )
            .def( "hasChildren", &Node::hasChildren )
            .def( "update", &Node::update )
            .def( "getParent", &Node::getParent )
            .def( "getScene", &Node::getScene )
            .def( "getLayer", &Node::getLayer )
            .def_native( "setEventListener", &Node::setEventListener )
            .def( "removeEventListener", &Node::removeEventListener )
            //.def( "getListener", &Node::getListener )

            .def( "getWorldPosition", &Node::getWorldPosition )
            //.def( "getWorldDirection", &Node::getWorldDirection )
            .def_proxy_static( "getCameraPosition", nodeScriptMethod, &NodeScriptMethod::s_getCameraPosition )

            .def( "setWorldPosition", &Node::setWorldPosition )

            .def( "getWorldColor", &Node::getWorldColor )

            .def_proxy_static( "createChildren", nodeScriptMethod, &NodeScriptMethod::createChildren )

            .def_proxy_static( "colorTo", nodeScriptMethod, &NodeScriptMethod::colorTo )
            .def_proxy_static( "alphaTo", nodeScriptMethod, &NodeScriptMethod::alphaTo )
            .def_proxy_static( "colorStop", nodeScriptMethod, &NodeScriptMethod::colorStop )

            .def_proxy_static( "velocityTo", nodeScriptMethod, &NodeScriptMethod::velocityTo )

            .def_proxy_static( "moveTo", nodeScriptMethod, &NodeScriptMethod::moveTo )
            .def_proxy_static( "bezier2To", nodeScriptMethod, &NodeScriptMethod::bezier2To )
            .def_proxy_static( "bezier3To", nodeScriptMethod, &NodeScriptMethod::bezier3To )
            .def_proxy_static( "moveStop", nodeScriptMethod, &NodeScriptMethod::moveStop )

            .def_proxy_static( "angleTo", nodeScriptMethod, &NodeScriptMethod::angleTo )
            .def_proxy_static( "angleStop", nodeScriptMethod, &NodeScriptMethod::angleStop )
            .def_proxy_static( "scaleTo", nodeScriptMethod, &NodeScriptMethod::scaleTo )
            .def_proxy_static( "scaleStop", nodeScriptMethod, &NodeScriptMethod::scaleStop )

            .def_proxy_static( "accMoveTo", nodeScriptMethod, &NodeScriptMethod::accMoveTo )
            .def_proxy_static( "accAngleTo", nodeScriptMethod, &NodeScriptMethod::accAngleTo )

            //.def_property_static( "child", &ScriptMethod::s_get_child, &ScriptMethod::s_set_child )
            ;

        pybind::interface_<ThreadTask>("Task")
            ;

        pybind::interface_<RenderCameraInterface>("RenderCameraInterface")
            .def( "getViewport", &RenderCameraInterface::getViewport )
            ;

        pybind::interface_<Camera2D, pybind::bases<Node, RenderCameraInterface> >("Camera2D", false)
            .def( "setViewport", &Camera2D::setViewport )			
            //.def( "setRenderport", &Camera2D::setRenderport )
            //.def( "getRenderport", &Camera2D::getRenderport )
            .def( "setRenderTarget", &Camera2D::setRenderTarget )
            ;		

        //pybind::proxy_<SceneNode3D, pybind::bases<Node>>("SceneNode3D", false)
        //	.def( "getWorldOrient", &SceneNode3D::getWorldOrient )
        //	.def( "getWorldPosition", &SceneNode3D::getWorldPosition )
        //	.def( "getLocalOrient", &SceneNode3D::getLocalOrient )
        //	.def( "getPosition", &SceneNode3D::getLocalPosition )
        //	.def( "setPosition", &SceneNode3D::setLocalPosition )
        //	.def( "setOrient", &SceneNode3D::setLocalOrient )
        //	.def( "setScale", &SceneNode3D::setScale )
        //	.def( "yaw", &SceneNode3D::yaw )
        //	.def( "pitch", &SceneNode3D::pitch )
        //	.def( "roll", &SceneNode3D::roll )
        //	.def( "setFixedYawAxis", &SceneNode3D::setFixedYawAxis )
        //	.def( "translate", &SceneNode3D::translate )
        //	.def( "addChild", &SceneNode3D::addChild )
        //	.def( "setYawSpeed", &SceneNode3D::setYawSpeed )
        //	.def( "setYawLimits", &SceneNode3D::setYawLimits )
        //	.def( "getYaw", &SceneNode3D::getYaw )
        //	.def( "getPitch", &SceneNode3D::getPitch )
        //	.def( "setListener", &SceneNode3D::setListener )

        //	//.def( "getCamera", &SceneNode3D::getCamera )
        //	;

        {

            //pybind::proxy_<RigidBody3D, pybind::bases<Node>>("RigidBody3D", false)
            //	.def( "applyForce", &RigidBody3D::applyForce )
            //	.def( "applyImpulse", &RigidBody3D::applyImpulse )
            //	.def( "applyAngularImpulse", &RigidBody3D::applyAngularImpulse )
            //	.def( "applyTorque", &RigidBody3D::applyTorque )
            //	.def( "setLinearVelocity", &RigidBody3D::setLinearVelocity )
            //	.def( "setAngularVelocity", &RigidBody3D::setAngularVelocity )
            //	.def( "setActive", &RigidBody3D::setActive )
            //	;

            //pybind::proxy_<CapsuleController, pybind::bases<Node>>("CapsuleController", false)
            //	.def( "move", &CapsuleController::move )
            //	.def( "setPosition", &CapsuleController::setPosition )
            //	.def( "getFilteredPosition", &CapsuleController::getFilteredPosition )
            //	;

            //pybind::proxy_<Camera3D, pybind::bases<SceneNode3D> >("Camera3D", false)
            //	.def( "setPosition", &Camera3D::setPosition )
            //	.def( "lookAt", &Camera3D::lookAt )
            //	//	.def( "yaw", &Camera3D::yaw )
            //	//	.def( "pitch", &Camera3D::pitch )
            //	//	.def( "roll", &Camera3D::roll )
            //	.def( "getDirection", &Camera3D::getDirection )
            //	;

            //pybind::proxy_<DiscreteEntity, pybind::bases<SceneNode3D>>("DiscreteEntity", false)
            //	.def( "createRenderToTexture", &DiscreteEntity::createRenderToTexture )
            //	.def( "playAnimation", &DiscreteEntity::playAnimation )
            //	.def( "setMaterial", &DiscreteEntity::setMaterial )
            //	//.def( "playAnimation", &DiscreteEntity::playAnimation )
            //	;

            //pybind::interface_<Allocator3D>("Allocator3D", false)
            //	.def( "scale", &Allocator3D::scale )
            //	;

            //pybind::proxy_<SceneNode3D, pybind::bases<Node, Allocator3D> >("SceneNode3D", false)
            //	.def( "yaw", &SceneNode3D::yaw )
            //	.def( "pitch", &SceneNode3D::pitch )
            //	.def( "getYaw", &SceneNode3D::getYaw )
            //	.def( "getPitch", &SceneNode3D::getPitch )
            //	.def( "yawTime", &SceneNode3D::yawTime )
            //	.def( "pitchTime", &SceneNode3D::pitchTime )
            //	.def( "setListener", &SceneNode3D::setListener )
            //	;

            //pybind::proxy_<Layer3D, pybind::bases<SceneNode3D> >("Layer3D", false)
            //	//.def( "addCamera", &Layer3D::addCamera )
            //	//.def( "getCamera", &Layer3D::getCamera )	
            //	.def( "addController", &Layer3D::addController )
            //	.def( "getController", &Layer3D::getController )		
            //	//.def( "getNode", &Layer3D::getNode )
            //	;

            {

                //pybind::proxy_< RenderMesh, pybind::bases<SceneNode3D> >("RenderMesh", false)
                //	//.def( "createRenderTarget", &RenderMesh::createRenderTarget )
                //	//.def( "setMaterial", &RenderMesh::setMaterial )
                //	;

                //pybind::proxy_<Camera3D, pybind::bases<SceneNode3D> >("Camera3D", false)
                //	//	.def( "setPosition", &Camera3D::setPosition )
                //	.def( "lookAt", &Camera3D::lookAt )
                //	.def( "yaw", &Camera3D::yaw )
                //	.def( "pitch", &Camera3D::pitch )
                //	//.def( "roll", &Camera3D::roll )
                //	//	.def( "getDirection", &Camera3D::getDirection )
                //	;
            }

            {
                pybind::interface_<ParticleEmitter, pybind::bases<Node, Animatable> >("ParticleEmitter", false)
                    .def( "playFromPosition", &ParticleEmitter::playFromPosition )
                    .def( "pause", &ParticleEmitter::pause )
                    .def( "restart", &ParticleEmitter::restart )
                    .def( "setLoop", &ParticleEmitter::setLoop )
                    .def( "getLoop", &ParticleEmitter::getLoop )
                    //.def( "interrupt", &ParticleEmitter::interrupt )
                    .def( "setLeftBorder", &ParticleEmitter::setLeftBorder )
                    .def( "setResource", &ParticleEmitter::setResource )
                    .def( "setEmitter", &ParticleEmitter::setEmitter )
                    .def( "setEmitterTranslateWithParticle", &ParticleEmitter::setEmitterTranslateWithParticle )
                    .def( "setEmitterRelative", &ParticleEmitter::setEmitterRelative )
                    .def( "setEmitterImage", &ParticleEmitter::setEmitterImage )
                    .def( "removeEmitterImage", &ParticleEmitter::removeEmitterImage )
                    .def( "changeEmitterPolygon", &ParticleEmitter::changeEmitterPolygon )
                    .def( "removeEmitterPolygon", &ParticleEmitter::removeEmitterPolygon )
                    .def( "setStartPosition", &ParticleEmitter::setStartPosition )
                    .def( "getEmitterName" , &ParticleEmitter::getEmitterName )
                    .def( "getLeftBorder" , &ParticleEmitter::getLeftBorder )
                    .def( "getRightBorder" , &ParticleEmitter::getRightBorder )
                    .def( "getDuration" , &ParticleEmitter::getDuration )
                     .def( "getEmitterBoundingBox", &ParticleEmitter::getEmitterBoundingBox )
                    .def( "getEmitterPosition", &ParticleEmitter::getEmitterPosition )
                    .def( "setRandomMode", &ParticleEmitter::setRandomMode )
                    .def( "getRandomMode", &ParticleEmitter::getRandomMode )
                    ;

                pybind::interface_<SoundEmitter, pybind::bases<Node, Animatable> >("SoundEmitter", false)
                    .def( "setVolume", &SoundEmitter::setVolume )
                    .def( "getVolume", &SoundEmitter::getVolume )
                    .def( "setLoop", &SoundEmitter::setLoop )
                    .def( "isLooping", &SoundEmitter::getLoop )
                    .def( "setSoundResource", &SoundEmitter::setSoundResource )
                    ;

                pybind::interface_<TextField, pybind::bases<Node> >("TextField", false)
                    .def( "setText", &TextField::setText )
                    .def( "getText", &TextField::getText )
                    .def( "setHeight", &TextField::setHeight )
                    .def( "getHeight", &TextField::getHeight )
                    .def( "setOutlineColor", &TextField::setOutlineColor )
                    .def( "getOutlineColor", &TextField::getOutlineColor )
                    .def( "enableOutline", &TextField::enableOutline )
                    .def( "isOutline", &TextField::isOutline )
                    .def( "getLength", &TextField::getLength )
                    .def( "setMaxLen", &TextField::setMaxLen )
                    .def( "getLineOffset", &TextField::getLineOffset )
                    .def( "setLineOffset", &TextField::setLineOffset )
                    .def( "setResourceFont", &TextField::setResourceFont )
                    .def( "getResourceFont", &TextField::getResourceFont )

                    .def( "setNoneAlign", &TextField::setNoneAlign )
                    .def( "isNoneAlign", &TextField::isNoneAlign )
                    .def( "setCenterAlign", &TextField::setCenterAlign )
                    .def( "isCenterAlign", &TextField::isCenterAlign )
                    .def( "setRightAlign", &TextField::setRightAlign )
                    .def( "isRightAlign", &TextField::isRightAlign )
                    .def( "setLeftAlign", &TextField::setLeftAlign )
                    .def( "isLeftAlign", &TextField::isLeftAlign )

                    .def( "setVerticalNoneAlign", &TextField::setVerticalNoneAlign )
                    .def( "isVerticalNoneAlign", &TextField::isVerticalNoneAlign )
                    .def( "setVerticalCenterAlign", &TextField::setVerticalCenterAlign )
                    .def( "isVerticalCenterAlign", &TextField::isVerticalCenterAlign )

                    .def( "getCharOffset", &TextField::getCharOffset )
                    .def( "setCharOffset", &TextField::setCharOffset )
                    .def( "getDefaultText", &TextField::getDefaultText )
                    .def( "setTextByKey", &TextField::setTextByKey )
                    .def( "setTextByKeyFormat", &TextField::setTextByKeyFormat )					
                    .def( "getTextKey", &TextField::getTextKey )

                    .def( "setPixelsnap", &TextField::setPixelsnap )
                    .def( "getPixelsnap", &TextField::getPixelsnap )

                    .def( "setMaxCharCount", &TextField::setMaxCharCount )
                    .def( "getMaxCharCount", &TextField::getMaxCharCount )
                    .def( "getCharCount", &TextField::getCharCount )
                    ;

                pybind::interface_<Point, pybind::bases<Node> >("Point", false)
                    .def( "testHotSpot", &Point::testHotSpot )
                    ;

                pybind::interface_<Layer, pybind::bases<Node> >("Layer", false)
                    .def( "setMain", &Layer::setMain )
                    .def( "isMain", &Layer::isMain )
                    .def( "setSize", &Layer::setSize )
                    .def( "getSize", &Layer::getSize )
                    ;

                pybind::interface_<Layer2D, pybind::bases<Layer> >("Layer2D", false)
                    .def( "setParallaxFactor", &Layer2D::setParallaxFactor )
                    .def( "getParallaxFactor", &Layer2D::getParallaxFactor )
                    .def( "setRenderViewport", &Layer2D::setRenderViewport )
                    .def( "removeRenderViewport", &Layer2D::removeRenderViewport )
                    .def( "cameraToLocal", &Layer2D::cameraToLocal )
                    ;
                pybind::interface_<Layer2DPhysic, pybind::bases<Layer2D> >("Layer2DPhysic", false);
                //pybind::proxy_<Layer2DTexture, pybind::bases<Layer2D> >("Layer2DTexture", false)
                //	.def( "setCameraOffset", &Layer2DTexture::setCameraOffset )
                //	.def( "getCameraOffset", &Layer2DTexture::getCameraOffset )
                //	.def( "setRenderTargetName", &Layer2DTexture::setRenderTargetName )
                //	.def( "getRenderTargetName", &Layer2DTexture::getRenderTargetName )
                //	;

                pybind::interface_<MousePickerTrapInterface>("MousePickerTrap", false)
                    .def( "pick", &MousePickerTrapInterface::pick )
                    ;

                pybind::interface_<MousePickerAdapter, pybind::bases<MousePickerTrapInterface> >("MousePickerAdapter", false)
                    .def( "setDefaultHandle", &MousePickerAdapter::setDefaultHandle )
                    .def( "getDefaultHandle", &MousePickerAdapter::getDefaultHandle )
                    ;

                pybind::interface_<HotSpot, pybind::bases<Node, MousePickerAdapter> >("HotSpot", false)
                    .def( "testPoint", &HotSpot::testPoint )
                    .def( "clearPoints", &HotSpot::clearPoints )
                    .def( "getPolygon", &HotSpot::getPolygon )
                    .def( "setPolygon", &HotSpot::setPolygon )
                    .def_proxy_static( "getLocalPolygonCenter", nodeScriptMethod, &NodeScriptMethod::s_getLocalPolygonCenter )
                    .def_proxy_static( "getWorldPolygonCenter", nodeScriptMethod, &NodeScriptMethod::s_getWorldPolygonCenter )
                    .def_proxy_static( "getWorldPolygon", nodeScriptMethod, &NodeScriptMethod::s_getWorldPolygon )
                    ;

                pybind::interface_<HotSpotImage, pybind::bases<HotSpot> >("HotSpotImage", false)
                    .def( "setResourceHIT", &HotSpotImage::setResourceHIT )
                    .def( "getResourceHIT", &HotSpotImage::getResourceHIT )
                    //.def( "setFrame", &HotSpotImage::setFrame )
                    //.def( "getFrame", &HotSpotImage::getFrame )
                    .def( "setAlphaTest", &HotSpotImage::setAlphaTest )
                    .def( "getAlphaTest", &HotSpotImage::getAlphaTest )
                    .def( "getWidth", &HotSpotImage::getWidth )
                    .def( "getHeight", &HotSpotImage::getHeight )
                    ;

                pybind::interface_<Sprite, pybind::bases<Node> >("Sprite", false)
                    //.def( "setImageIndex", &Sprite::setImageIndex )
                    //.def( "getImageIndex", &Sprite::getImageIndex )
                    //.def( "getImageCount", &Sprite::getImageCount )
                    .def( "setImageResource", &Sprite::setImageResourceName )
                    .def( "getImageResource", &Sprite::getImageResourceName )

                    .def_proxy_static( "getImageSize", nodeScriptMethod, &NodeScriptMethod::s_getImageSize )
                    
                    .def_proxy_static( "getLocalImageCenter", nodeScriptMethod, &NodeScriptMethod::s_getLocalImageCenter )
                    .def_proxy_static( "getWorldImageCenter", nodeScriptMethod, &NodeScriptMethod::s_getWorldImageCenter )

                    //.def( "setScale", &Sprite::setScale )
                    //.def( "getScale", &Sprite::getScale )
                    .def( "setPercentVisibility", &Sprite::setPercentVisibility )
                    .def_proxy_static( "setPercentVisibilityTo", nodeScriptMethod, &NodeScriptMethod::setPercentVisibilityTo )
                    .def_proxy_static( "setPercentVisibilityStop", nodeScriptMethod, &NodeScriptMethod::setPercentVisibilityStop )
                    .def( "setFlipX", &Sprite::setFlipX )
                    .def( "getFlipX", &Sprite::getFlipX )
                    .def( "setFlipY", &Sprite::setFlipY )                    
                    .def( "getFlipY", &Sprite::getFlipY )
                    .def( "disableTextureColor", &Sprite::disableTextureColor )
                    .def( "setTextureMatrixOffset", &Sprite::setTextureMatrixOffset )

                    .def( "setTextureUVOffset", &Sprite::setTextureUVOffset )
                    .def( "getTextureUVOffset", &Sprite::getTextureUVOffset )
                    .def( "setTextureUVScale", &Sprite::setTextureUVScale )
                    .def( "getTextureUVScale", &Sprite::getTextureUVScale )

                    .def( "setSpriteSize", &Sprite::setSpriteSize )
                    ;
                {
                    pybind::interface_<Animation, pybind::bases<Sprite, Animatable> >("Animation", false)
                        .def( "setAnimationResource", &Animation::setAnimationResource )
                        .def( "getAnimationResource", &Animation::getAnimationResource )
                        .def( "getFrameCount", &Animation::getFrameCount )
                        .def( "getFrameDelay", &Animation::getFrameDelay )
                        .def( "setCurrentFrame", &Animation::setCurrentFrame )
                        .def( "getCurrentFrame", &Animation::getCurrentFrame )
                        ;
                }

                //pybind::proxy_<RigidBody2D, pybind::bases<Node> >("RigidBody2D", false)
                //	.def( "applyForce", &RigidBody2D::applyForce )
                //	.def( "applyImpulse", &RigidBody2D::applyImpulse )
                //	.def( "applyConstantForce", &RigidBody2D::applyConstantForce )
                //	.def( "removeConstantForce", &RigidBody2D::removeConstantForce )
                //	.def( "setDirectionForce", &RigidBody2D::setDirectionForce )
                //	.def( "wakeUp", &RigidBody2D::wakeUp )
                //	.def( "getMass", &RigidBody2D::getMass )
                //	.def( "getLinearVelocity", &RigidBody2D::getLinearVelocity )
                //	.def( "setLinearVelocity", &RigidBody2D::setLinearVelocity )
                //	.def( "unsetLinearVelocity", &RigidBody2D::unsetLinearVelocity )
                //	.def( "setCollisionMask", &RigidBody2D::setCollisionMask )
                //	.def( "enableStabilization", &RigidBody2D::enableStabilization )
                //	;

                //pybind::proxy_<PhysicalBody2D, pybind::bases<Node> >("PhysicalBody2D", false)
                //	.def( "applyForce", &PhysicalBody2D::applyForce )
                //	.def( "applyImpulse", &PhysicalBody2D::applyImpulse )
                //	.def( "applyConstantForce", &PhysicalBody2D::applyConstantForce )
                //	.def( "removeConstantForce", &PhysicalBody2D::removeConstantForce )
                //	.def( "setDirectionForce", &PhysicalBody2D::setDirectionForce )
                //	.def( "wakeUp", &PhysicalBody2D::wakeUp )
                //	.def( "getMass", &PhysicalBody2D::getMass )
                //	.def( "getLinearVelocity", &PhysicalBody2D::getLinearVelocity )
                //	.def( "setLinearVelocity", &PhysicalBody2D::setLinearVelocity )
                //	.def( "unsetLinearVelocity", &PhysicalBody2D::unsetLinearVelocity )
                //	.def( "enableStabilization", &PhysicalBody2D::enableStabilization )
                //	.def( "setOrientation", &PhysicalBody2D::setOrientation )
                //	.def( "addShapeBox", &PhysicalBody2D::addShapeBox )
                //	.def( "addShapeCircle", &PhysicalBody2D::addShapeCircle )
                //	.def( "addShapeConvex", &PhysicalBody2D::addShapeConvex )
                //	.def( "createBody", &PhysicalBody2D::createBody )
                //	.def( "setFilterData", &PhysicalBody2D::setFilterData )
                //	.def( "setCollisionMask", &PhysicalBody2D::setCollisionMask )
                //	.def( "setCategoryBits", &PhysicalBody2D::setCategoryBits )
                //	.def( "setGroupIndex", &PhysicalBody2D::setGroupIndex )
                //	.def( "setLinearDumping", &PhysicalBody2D::setLinearDumping )
                //	.def( "setAngularDumping", &PhysicalBody2D::setAngularDumping )
                //	.def( "setAllowSleep", &PhysicalBody2D::setAllowSleep )
                //	.def( "setIsBullet", &PhysicalBody2D::setIsBullet )
                //	.def( "setFixedRotation", &PhysicalBody2D::setFixedRotation )
                //	.def( "getCollisionMask", &PhysicalBody2D::getCollisionMask )
                //	.def( "getCategoryBits", &PhysicalBody2D::getCategoryBits )
                //	.def( "getGroupIndex", &PhysicalBody2D::getGroupIndex )
                //	.def( "getLinearDumping", &PhysicalBody2D::getLinearDumping )
                //	.def( "getAngularDumping", &PhysicalBody2D::getAngularDumping )
                //	.def( "getAllowSleep", &PhysicalBody2D::getAllowSleep )
                //	.def( "getIsBullet", &PhysicalBody2D::getIsBullet )
                //	.def( "getFixedRotation", &PhysicalBody2D::getFixedRotation )
                //	.def( "getIsSensor", &PhysicalBody2D::getIsSensor )
                //	;

                //pybind::proxy_<TilePolygon, pybind::bases<RigidBody2D> >("TilePolygon", false)
                //	;

                pybind::interface_<Movie, pybind::bases<Node, Animatable> >("Movie", false)
                    .def( "setReverse", &Movie::setReverse )
                    .def( "getReverse", &Movie::getReverse )
                    .def( "setResourceMovie", &Movie::setResourceMovieName )					
                    .def( "getMovieSlot", &Movie::getMovieSlot )
                    .def( "hasMovieSlot", &Movie::hasMovieSlot )
                    .def( "getSubMovie", &Movie::getSubMovie )
                    .def( "hasSubMovie", &Movie::hasSubMovie )
                    .def( "getSocket", &Movie::getSocket )
                    .def( "hasSocket", &Movie::hasSocket )
                    .def( "setMovieEvent", &Movie::setMovieEvent )
                    .def( "hasMovieEvent", &Movie::hasMovieEvent )
                    .def_proxy_static( "getFrameDuration", nodeScriptMethod, &NodeScriptMethod::movie_getFrameDuration )
                    .def_proxy_static( "getDuration", nodeScriptMethod, &NodeScriptMethod::movie_getDuration )
                    .def_proxy_static( "getFrameCount", nodeScriptMethod, &NodeScriptMethod::movie_getFrameCount )
                    .def_proxy_static( "getSize", nodeScriptMethod, &NodeScriptMethod::movie_getSize )
                    ;

                pybind::interface_<MovieSlot, pybind::bases<Node> >("MovieSlot", false)
                    ;

                pybind::interface_<MovieInternalObject, pybind::bases<Node> >("MovieInternalObject", false)
                    ;				

                pybind::interface_<Video, pybind::bases<Node , Animatable> >("Video", false)
                    .def( "pause",&Video::pause )
                    .def( "setVideoResource", &Video::setVideoResource )
                    ;

                pybind::interface_<Window, pybind::bases<Node> >("Window", false)
                    .def( "setClientSize", &Window::setClientSize )
                    .def( "setClientSizeClip", &Window::setClientSizeClip )
                    .def( "setClientSizeInTiles", &Window::setClientSizeInTiles )
                    .def( "getClientSize", &Window::getClientSize )
                    .def( "getWindowSize", &Window::getWindowSize )
                    .def( "getTileSize", &Window::getTileSize )
                    .def( "setResourceWindow", &Window::setResourceWindow )
                    ;

            }		

            pybind::def_functor( "setCurrentScene", nodeScriptMethod, &NodeScriptMethod::setCurrentScene );
            pybind::def_functor( "getCurrentScene", nodeScriptMethod, &NodeScriptMethod::getCurrentScene );

            pybind::def_functor( "createScene", nodeScriptMethod, &NodeScriptMethod::s_createScene );

            pybind::def_functor( "getCamera2DPosition", nodeScriptMethod, &NodeScriptMethod::s_getCamera2DPosition );

            pybind::def_functor( "createNode", nodeScriptMethod, &NodeScriptMethod::createNode );
            //pybind::def_function( "createNodeFromBinary", &ScriptMethod::createNodeFromBinary );
            pybind::def_functor( "destroyNode", nodeScriptMethod, &NodeScriptMethod::destroyNode );

            pybind::def_functor( "timing", nodeScriptMethod, &NodeScriptMethod::timing );
            pybind::def_functor( "timingRemove", nodeScriptMethod, &NodeScriptMethod::timingRemove );


            pybind::def_functor( "schedule", nodeScriptMethod, &NodeScriptMethod::schedule );
            pybind::def_functor( "scheduleRemove", nodeScriptMethod, &NodeScriptMethod::scheduleRemove );
            pybind::def_functor( "scheduleRemoveAll", nodeScriptMethod, &NodeScriptMethod::scheduleRemoveAll );
            pybind::def_functor( "scheduleFreeze", nodeScriptMethod, &NodeScriptMethod::s_scheduleFreeze );
            pybind::def_functor( "scheduleFreezeAll", nodeScriptMethod, &NodeScriptMethod::s_scheduleFreezeAll );
            pybind::def_functor( "scheduleResumeAll", nodeScriptMethod, &NodeScriptMethod::scheduleResumeAll );
            pybind::def_functor( "scheduleIsFreeze", nodeScriptMethod, &NodeScriptMethod::s_scheduleIsFreeze );
            pybind::def_functor( "scheduleTime", nodeScriptMethod, &NodeScriptMethod::s_scheduleTime );

            pybind::def_functor( "scheduleGlobal", nodeScriptMethod, &NodeScriptMethod::s_scheduleGlobal );
            pybind::def_functor( "scheduleGlobalRemove", nodeScriptMethod, &NodeScriptMethod::s_scheduleGlobalRemove );
            pybind::def_functor( "scheduleGlobalRemoveAll", nodeScriptMethod, &NodeScriptMethod::s_scheduleGlobalRemoveAll );
            pybind::def_functor( "scheduleGlobalFreeze", nodeScriptMethod, &NodeScriptMethod::s_scheduleGlobalFreeze );
            pybind::def_functor( "scheduleGlobalFreezeAll", nodeScriptMethod, &NodeScriptMethod::s_scheduleGlobalFreezeAll );
            pybind::def_functor( "scheduleGlobalResumeAll", nodeScriptMethod, &NodeScriptMethod::s_scheduleGlobalResumeAll );
            pybind::def_functor( "scheduleGlobalIsFreeze", nodeScriptMethod, &NodeScriptMethod::s_scheduleGlobalIsFreeze );
            pybind::def_functor( "scheduleGlobalTime", nodeScriptMethod, &NodeScriptMethod::s_scheduleGlobalTime );


            pybind::def_functor( "getMouseX", nodeScriptMethod, &NodeScriptMethod::getMouseX ); //deprecated
            pybind::def_functor( "getMouseY", nodeScriptMethod, &NodeScriptMethod::getMouseY ); //deprecated

            pybind::def_functor( "getCursorPosition", nodeScriptMethod, &NodeScriptMethod::s_getCursorPosition );

            pybind::def_functor( "setArrow", nodeScriptMethod, &NodeScriptMethod::s_setArrow );
            pybind::def_functor( "getArrow", nodeScriptMethod, &NodeScriptMethod::s_getArrow );

            pybind::def_functor( "setArrowLayer", nodeScriptMethod, &NodeScriptMethod::s_setArrowLayer );

            pybind::def_functor( "directResourceCompile", nodeScriptMethod, &NodeScriptMethod::directResourceCompile );
            pybind::def_functor( "directResourceRelease", nodeScriptMethod, &NodeScriptMethod::directResourceRelease );
            //pybind::def_function( "directResourceFileCompile", &ScriptMethod::s_directResourceFileCompile );
            //pybind::def_function( "deferredResourceFileCompile", &ScriptMethod::s_deferredResourceFileCompile );
            //pybind::def_function( "directResourceFileRelease", &ScriptMethod::s_directResourceFileRelease );
            pybind::def_functor( "getResourceReference", nodeScriptMethod, &NodeScriptMethod::s_getResourceReference );
            //pybind::def_function( "loadResourceImage", &ScriptMethod::s_loadResourceImage );
            //pybind::def_function( "loadImageResources", &ScriptMethod::s_loadImageResources );

            pybind::def_functor( "quitApplication", nodeScriptMethod, &NodeScriptMethod::quitApplication );
            //pybind::def_function( "createShot", &ScriptMethod::createShot );
            pybind::def_functor( "setFullscreenMode", nodeScriptMethod, &NodeScriptMethod::s_setFullscreenMode );
            pybind::def_functor( "getFullscreenMode", nodeScriptMethod, &NodeScriptMethod::s_getFullscreenMode );
            pybind::def_functor( "setFixedContentResolution", nodeScriptMethod, &NodeScriptMethod::s_setFixedContentResolution );
            pybind::def_functor( "getFixedContentResolution", nodeScriptMethod, &NodeScriptMethod::s_getFixedContentResolution );
            
            pybind::def_functor( "renderOneFrame", nodeScriptMethod, &NodeScriptMethod::renderOneFrame );
            pybind::def_functor( "writeImageToFile", nodeScriptMethod, &NodeScriptMethod::writeImageToFile );
            pybind::def_functor( "createImageResource", nodeScriptMethod, &NodeScriptMethod::s_createImageResource );
            //pybind::def_function( "createFolder", &ScriptMethod::createFolder );
            //pybind::def_function( "deleteFolder", &ScriptMethod::deleteFolder );
            pybind::def_functor( "minimizeWindow", nodeScriptMethod, &NodeScriptMethod::minimizeWindow );
            pybind::def_functor( "setMouseBounded", nodeScriptMethod, &NodeScriptMethod::s_setMouseBounded );
            pybind::def_functor( "getMouseBounded", nodeScriptMethod, &NodeScriptMethod::s_getMouseBounded );

            pybind::def_functor( "getCurrentResolution", nodeScriptMethod, &NodeScriptMethod::s_getCurrentResolution );
            pybind::def_functor( "getContentResolution", nodeScriptMethod, &NodeScriptMethod::s_getContentResolution );
            pybind::def_functor( "getHotSpotImageSize", nodeScriptMethod, &NodeScriptMethod::s_getHotSpotImageSize );

            //pybind::def_function( "setBlow", &ScriptMethod::setBlow );
            //pybind::def_function( "getBlow", &ScriptMethod::getBlow );
            //pybind::def_function( "setEnoughBlow", &ScriptMethod::setEnoughBlow );
            //pybind::def_function( "setBlowCallback", &ScriptMethod::setBlowCallback );

            pybind::def_functor( "isKeyDown", nodeScriptMethod, &NodeScriptMethod::s_isKeyDown );
            pybind::def_functor( "isMouseButtonDown", nodeScriptMethod, &NodeScriptMethod::s_isMouseButtonDown );
            pybind::def_functor( "isInViewport", nodeScriptMethod, &NodeScriptMethod::s_isInViewport );
            //pybind::def_function( "getResourceCount", &ScriptMethod::s_getResourceCount );
            pybind::def_functor( "enableTextureFiltering", nodeScriptMethod, &NodeScriptMethod::s_enableTextureFiltering );
            pybind::def_functor( "isTextureFilteringEnabled", nodeScriptMethod, &NodeScriptMethod::s_isTextureFilteringEnabled );

            pybind::def_functor( "existText", nodeScriptMethod, &NodeScriptMethod::s_existText );

            pybind::def_functor( "pickHotspot", nodeScriptMethod, &NodeScriptMethod::s_pickHotspot );

            pybind::def_functor( "blockInput", nodeScriptMethod, &NodeScriptMethod::s_blockInput );
            pybind::def_functor( "setInputMouseButtonEventBlock", nodeScriptMethod, &NodeScriptMethod::s_setInputMouseButtonEventBlock );
            pybind::def_functor( "getInputMouseButtonEventBlock", nodeScriptMethod, &NodeScriptMethod::s_getInputMouseButtonEventBlock );



            //pybind::def_function( "getParam", &ScriptMethod::s_getParam );
            pybind::def_functor( "addJoin", nodeScriptMethod, &NodeScriptMethod::s_addJoin );
            pybind::def_functor( "removeJoin", nodeScriptMethod, &NodeScriptMethod::s_removeJoin );
            pybind::def_functor( "isJoin", nodeScriptMethod, &NodeScriptMethod::s_isJoin );			
            pybind::def_functor( "getJoins", nodeScriptMethod, &NodeScriptMethod::s_getJoins );

            pybind::def_functor( "loadPlugin", nodeScriptMethod, &NodeScriptMethod::s_loadPlugin );

            //pybind::def_function( "loadText", &ScriptMethod::s_loadText );

            pybind::def_functor( "getRenderCamera2D", nodeScriptMethod, &NodeScriptMethod::s_getRenderCamera2D );

            pybind::def_functor( "testHotspot", nodeScriptMethod, &NodeScriptMethod::s_testHotspot);
            pybind::def_functor( "polygon_wm", nodeScriptMethod, &NodeScriptMethod::s_polygon_wm);

            pybind::def_functor( "createAffector", nodeScriptMethod, &NodeScriptMethod::createAffector );			

            pybind::def_functor( "showKeyboard", nodeScriptMethod, &NodeScriptMethod::showKeyboard );			
            pybind::def_functor( "hideKeyboard", nodeScriptMethod, &NodeScriptMethod::hideKeyboard );

            pybind::def_functor( "hasResource", nodeScriptMethod, &NodeScriptMethod::hasResource );

            pybind::def_functor( "removeCurrentScene", nodeScriptMethod, &NodeScriptMethod::removeCurrentScene );

            pybind::def_functor( "setDefaultResourceFontName", nodeScriptMethod, &NodeScriptMethod::setDefaultResourceFontName );

            pybind::def_functor( "addMouseMoveHandler", nodeScriptMethod, &NodeScriptMethod::s_addMouseMoveHandler );
            pybind::def_functor( "removeMouseMoveHandler", nodeScriptMethod, &NodeScriptMethod::s_removeMouseMoveHandler );

            pybind::def_functor( "addMouseButtonHandler", nodeScriptMethod, &NodeScriptMethod::s_addMouseButtonHandler );
            pybind::def_functor( "removeMouseButtonHandler", nodeScriptMethod, &NodeScriptMethod::s_removeMouseButtonHandler );

            pybind::def_functor( "visitChild", nodeScriptMethod, &NodeScriptMethod::s_visitChild );

            pybind::def_functor( "visitCompiledResources", nodeScriptMethod, &NodeScriptMethod::s_visitCompiledResources );
            pybind::def_functor( "visitResources", nodeScriptMethod, &NodeScriptMethod::s_visitResources );

            pybind::def_functor( "validResource", nodeScriptMethod, &NodeScriptMethod::s_validResource );

            //pybind::def_function( "createDistanceJoint", &ScriptMethod::s_createDistanceJoint );
            //pybind::def_function( "createHingeJoint", &ScriptMethod::s_createHingeJoint );
            //pybind::def_function( "createMouseJoint", &ScriptMethod::s_createMouseJoint );
            //
            //pybind::def_function( "createGearJoint", &ScriptMethod::s_createGearJoint );
            //pybind::def_function( "createPrismaticJoint", &ScriptMethod::s_createPrismaticJoint );
            //pybind::def_function( "createPulleyJoint", &ScriptMethod::s_createPulleyJoint );
            //pybind::def_function( "createRopeJoint", &ScriptMethod::s_createRopeJoint );
            //pybind::def_function( "createWheelJoint", &ScriptMethod::s_createWheelJoint );
            //pybind::def_function( "destroyJoint", &ScriptMethod::s_destroyJoint );
            //pybind::def_function( "physicsMouseMove", &ScriptMethod::s_physicsMouseMove );

            pybind::def_functor( "getPlatformName", nodeScriptMethod, &NodeScriptMethod::s_getPlatformName );
            pybind::def_functor( "getTimingFactor", nodeScriptMethod, &NodeScriptMethod::s_getTimingFactor );
            pybind::def_functor( "setTimingFactor", nodeScriptMethod, &NodeScriptMethod::s_setTimingFactor );

            pybind::def_functor( "addHomeless", nodeScriptMethod, &NodeScriptMethod::s_addHomeless );
            pybind::def_functor( "cancelTask", nodeScriptMethod, &NodeScriptMethod::s_cancelTask );
            pybind::def_functor( "joinTask", nodeScriptMethod, &NodeScriptMethod::s_joinTask );

            //For Astralax
            pybind::def_functor( "visitResourceEmitterContainer", nodeScriptMethod, &NodeScriptMethod::s_visitResourceEmitterContainer );

            pybind::def_functor( "getNullObjectsFromResourceVideo", nodeScriptMethod, &NodeScriptMethod::s_getNullObjectsFromResourceVideo );

            pybind::def_functor( "getMovieDuration", nodeScriptMethod, &NodeScriptMethod::s_getMovieDuration );

            pybind::def_functor( "addEntityPrototype", nodeScriptMethod, &NodeScriptMethod::s_addEntityPrototype );
            pybind::def_functor( "addScenePrototype", nodeScriptMethod, &NodeScriptMethod::s_addScenePrototype );
            pybind::def_functor( "addArrowPrototype", nodeScriptMethod, &NodeScriptMethod::s_addArrowPrototype );

            pybind::def_functor( "getGameViewport", nodeScriptMethod, &NodeScriptMethod::s_getGameViewport );			
                        
            pybind::def_functor( "getGameParam", nodeScriptMethod, &NodeScriptMethod::s_getGameParam );
            pybind::def_functor( "getGameParamFloat", nodeScriptMethod, &NodeScriptMethod::s_getGameParamFloat );
            pybind::def_functor( "getGameParamInt", nodeScriptMethod, &NodeScriptMethod::s_getGameParamInt );
            pybind::def_functor( "getGameParamBool", nodeScriptMethod, &NodeScriptMethod::s_getGameParamBool );
            
            pybind::def_functor( "hasGameParam", nodeScriptMethod, &NodeScriptMethod::s_hasGameParam );
            pybind::def_functor( "openUrlInDefaultBrowser", nodeScriptMethod, &NodeScriptMethod::s_openUrlInDefaultBrowser );
        }
    }
}
