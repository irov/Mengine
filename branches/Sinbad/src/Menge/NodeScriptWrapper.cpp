
#	include "ScriptWrapper.h"

#	include "ScriptClassWrapperDefine.h"
#	include "ScriptDeclarationDefine.h"

#	include "InputEngine.h"
#	include "SceneManager.h"
#	include "FileEngine.h"
#	include "Scene.h"
#	include "Game.h"

#	include "ScriptEngine.h"
#	include "ScheduleManager.h"

#	include "TextManager.h"

#	include "ResourceManager.h"
#	include "ResourceImageDynamic.h"
#	include "ResourceImageDefault.h"
#	include "ResourceHotspotImage.h"

#	include "Player.h"
#	include "Application.h"

#	include "Amplifier.h"
#	include "Sprite.h"
#	include "Animation.h"
#	include "HotSpot.h"
#	include "Light2D.h"
#	include "ShadowCaster2D.h"
#	include "Arrow.h"
#	include "TextField.h"
#	include "SoundEmitter.h"
#	include "Emitter.h"
#	include "Point.h"
#	include "Camera3d.h"
//#	include "RigidBody3D.h"
//#	include "CapsuleController.h"
#	include "Skeleton.h"
#	include "Camera2D.h"

#	include "Layer2D.h"

#	include "RigidBody2D.h"

#	include "TilePolygon.h"

#	include "Video.h"
#	include "Window.h"
#	include "HotSpotImage.h"
#	include "Mesh_40_30.h"

#	include "Entity.h"

//#	include "DiscreteEntity.h"

#	include "SoundEngine.h"
#	include "LogEngine.h"
#	include "RenderEngine.h"
#	include "PhysicEngine2D.h"

#	include "Identity.h"

//#	include "Layer3D.h"

//#	include "FFCamera.h"
#	include "SceneNode3D.h"
#	include "Camera3D.h"
#	include "RenderMesh.h"

#	include "XmlEngine.h"

#	include "TaskManager.h"
#	include "TaskDeferredLoading.h"
#	include "Utils/Math/clamp.h"


namespace Menge
{
	namespace ScriptMethod
	{
		static std::size_t schedule( float _timing, PyObject * _script )
		{
			return Player::hostage()
				->schedule( _timing, _script );
		}

		static void scheduleRemove( std::size_t _id )
		{
			Player::hostage()
				->scheduleRemove( _id );
		}

		static void scheduleRemoveAll()
		{
			Player::hostage()
				->scheduleRemoveAll();
		}	

		static void scheduleStopAll()
		{
			Player::hostage()
				->scheduleSetUpdatable( false );
		}

		static void scheduleResumeAll()
		{
			Player::hostage()
				->scheduleSetUpdatable( true );
		}

		static void s_scheduleFreeze( std::size_t _id, bool _freeze )
		{
			Player::hostage()
				->scheduleFreeze( _id, _freeze );
		}

		static float getMouseX()
		{
			const Resolution& contRes = Game::hostage()->getContentResolution();
			float mx = Player::hostage()->getArrow()->getLocalPosition().x;
			mx = mt::clamp( 0.0f, mx, static_cast<float>( contRes[0] ) );
			return mx;
		}
		static mt::vec2f s_getContentResolution()
		{
			const Resolution& contRes = Game::hostage()->getContentResolution();
			mt::vec2f res;
			res.x = contRes.getWidth();
			res.y = contRes.getHeight();
			return res;
		}

		static float getMouseY()
		{
			const Resolution& contRes = Game::hostage()->getContentResolution();
			float my = Player::hostage()->getArrow()->getLocalPosition().y;
			my = mt::clamp( 0.0f, my, static_cast<float>( contRes[1] ) );
			return my;
		}

		static bool s_isMouseDown( int _button )
		{
			return InputEngine::hostage()
				->isButtonDown( _button	);
		}

		static bool s_isKeyDown( int _key )
		{
			return InputEngine::hostage()
				->isKeyDown( _key );
		}

		static void setCurrentScene( const String& _name, bool _destroyOld = false )
		{
			MENGE_LOG( "set current scene '%s'"
				, _name.c_str() 
				);

			Player::hostage()
				->setCurrentScene( _name, _destroyOld );
		}

		static void s_setCurrentSceneCb( const String& _name, PyObject* _cb )
		{
			MENGE_LOG( "set current scene '%s'"
				, _name.c_str() 
				);

			ScriptEngine::hostage()
				->incref( _cb );

			Player::hostage()
				->setCurrentSceneCb( _name, _cb );
		}

		static Scene * getCurrentScene()
		{
			Scene * scene = Player::hostage()
				->getCurrentScene();

			return scene;
		}

		static void setArrow( const std::string & _name )
		{
			Arrow * arrow = Game::hostage()
				->getArrow( _name );

			if( arrow == 0 )
			{
				MENGE_LOG_ERROR( "Error: can't setup arrow '%s'"
					, _name.c_str() 
					);

				return;
			}

			Player::hostage()->setArrow( arrow );
		}

		static Arrow * getArrow()
		{
			Arrow * arrow = Holder<Player>::hostage()
				->getArrow();

			return arrow;
		}

		static PyObject * s_getHotSpotPoints( HotSpot * _hotspot )
		{
			if( _hotspot == 0 || _hotspot->isCompile() == false  )
			{
				return pybind::ret_none();
			}

			const mt::polygon & pg = _hotspot->getPolygon();

			std::size_t sz = pg.num_points();

			PyObject * pyret = pybind::list_new(0);
			for( std::size_t i = 0; i != sz; ++i )
			{
				PyObject * pypoint = pybind::ptr( pg[i] );

				pybind::list_appenditem( pyret, pypoint );

				pybind::decref( pypoint );
			}

			return pyret;
		}

		static mt::vec2f s_getHotSpotImageSize( HotSpotImage * _hotspotImage )
		{
			if( _hotspotImage == 0 || _hotspotImage->isCompile() == false )
			{
				return mt::vec2f(0.f,0.f);
			}

			ResourceHotspotImage * resourceImage = _hotspotImage->getResourseHotspotImage();

			const mt::vec2f & size = resourceImage->getSize();

			return size;
		}

		static void setCamera2DPosition( float x, float y )
		{
			//Holder<Player>::hostage()
			//	->setCamera2DPosition( mt::vec2f(x, y) );
			// deprecated
		}

		static const mt::vec2f& s_getCamera2DPosition()
		{
			return Player::hostage()
				->getRenderCamera2D()->getViewport().begin;
		}

		static void setCamera2DDirection( float x, float y )
		{
			assert(!"NOT IMPLEMENTED");
			Scene * scene = getCurrentScene();
			/*
			struct ForeachRender
			: public NodeForeach
			{
			mt::vec2f dir;
			ForeachRender( const mt::vec2f& _dir )
			: dir(_dir)
			{}

			void apply( Node * children ) override
			{
			Node * child = dynamic_cast<Node*>(children);
			child->setLocalDirection(dir);
			}
			};

			scene->foreachChildren( ForeachRender(mt::vec2f(x,y)) );
			*/
		}

		static void destroyNode( Node * _node )
		{
			//if( Node * parent = _node->getParent() )
			//{
			//	parent->removeChildren( _node );
			//}

			_node->destroy();			
		}

		static void destroyScene( Scene * _scene )
		{
			Game::hostage()
				->destroyScene( _scene );
		}

		static void destroySceneByName( const std::string & _nameScene )
		{
			Game::hostage()
				->destroySceneByName( _nameScene );
		}

		static PyObject * createNode( const std::string & _type )
		{
			Node * node = SceneManager::hostage()
				->createNode( _type );

			if( node == 0 )
			{
				return pybind::ret_none();
			}

			PyObject * embedding = node->getEmbedding();

			if( embedding == 0 )
			{
				node->destroy();
				return pybind::ret_none();
			}

			Game::hostage()
				->addHomeless( embedding );

			return embedding;
		}

		static PyObject * createNodeFromXml( PyObject * _params  )
		{
			if( pybind::string_check( _params ) == false )
			{
				return pybind::ret_none();
			}

			size_t xml_data_size;
			const TChar * xml_data = pybind::string_to_char( _params, xml_data_size );

			Node * node = Holder<SceneManager>::hostage()
				->createNodeFromXmlData( xml_data );

			if( node == 0 )
			{
				return pybind::ret_none();
			}

			PyObject * embedding = node->getEmbedding();

			if( embedding == 0 )
			{
				node->destroy();
				return pybind::ret_none();
			}

			Game::hostage()
				->addHomeless( embedding );

			return embedding;
		}

		static void quitApplication()
		{
			Application::hostage()->quit();
		}

		static bool openUrlInDefaultBrowser( const String & _url )
		{
			bool result = Application::hostage()->openUrlInDefaultBrowser( _url );
			return result;
		}

		static bool isExistConfiguration( const String & _configurationName )
		{
			bool result = Application::hostage()->isExistConfiguration( _configurationName );
			return result;
		}

		static bool existFile( const String & _fileSystemName, const String & _filename )
		{
			bool result = Holder<FileEngine>::hostage()->existFile( _fileSystemName , _filename );
			return result;
		}

		static bool executeProgram( const String & _programName )
		{
			bool result =  Application::hostage()->executeProgram( _programName  );
			return result;
		}

		static bool directResourceCompile( const String& _nameResource )
		{
			bool result = ResourceManager::hostage()
				->directResourceCompile( _nameResource );

			return result;
		}

		static bool s_validResource( const String & _resourceName )
		{
			bool valid =ResourceManager::hostage()
				->validResource( _resourceName );

			return valid;
		}

		static void directResourceRelease( const String& _nameResource )
		{
			ResourceManager::hostage()
				->directResourceRelease( _nameResource );
		}

		static void directResourceUnload( const String& _nameResource )
		{
			ResourceManager::hostage()
				->directResourceUnload( _nameResource );
		}

		static void s_directResourceFileCompile( const String& _resourceFile )
		{
			ResourceManager::hostage()
				->directResourceFileCompile( _resourceFile );
		}

		static void s_deferredResourceFileCompile( PyObject* _resourceFiles, PyObject* _progressCallback )
		{
			TStringVector resourceFiles;
			if( pybind::string_check( _resourceFiles ) == true )
			{
				String resourceFile = pybind::extract_nt<String>( _resourceFiles );
				resourceFiles.push_back( resourceFile );
			}
			else if( pybind::list_check( _resourceFiles ) == true )
			{
				std::size_t listSize = pybind::list_size( _resourceFiles );
				for( std::size_t i = 0; i < listSize; ++i )
				{
					PyObject* listItem = pybind::list_getitem( _resourceFiles, i );
					if( pybind::string_check( listItem ) == false )
					{
						MENGE_LOG_ERROR( "Error: (Menge.deferredResourceFileCompile) invalid argument" );
						return;
					}
					String resourceFile = pybind::extract_nt<String>( listItem );
					resourceFiles.push_back( resourceFile );
				}
			}
			else
			{
				MENGE_LOG_ERROR( "Error: (Menge.deferredResourceFileCompile) invalid argument" );
				return;
			}

			TaskDeferredLoading* task = new TaskDeferredLoading( resourceFiles, _progressCallback );
			TaskManager::hostage()
				->addTask( task );
		}

		static void s_directResourceFileRelease( const String& _resourceFile )
		{
			ResourceManager::hostage()
				->directResourceFileRelease( _resourceFile );
		}

		static void s_directResourceFileUnload( const String& _resourceFile )
		{
			ResourceManager::hostage()
				->directResourceFileUnload( _resourceFile );
		}

		static PyObject * createShot( const String& _name, mt::vec2f _min,  mt::vec2f _max )
		{
			mt::vec4f rect( _min, _max );

			ResourceImageDynamic * resourceImage 
				= ResourceManager::hostage()->getResourceT<ResourceImageDynamic>( _name );

			if( resourceImage == NULL )
			{
				ResourceFactoryParam param;

				param.name = _name;
				param.category = "user";

				String group;
				Account * acc = Game::hostage()->getCurrentAccount();
				if( acc != 0 )
				{
					param.group = acc->getFolder() + "/";
				}

				resourceImage = ResourceManager::hostage()
					->createResourceWithParamT<ResourceImageDynamic>( "ResourceImageDynamic", param );

				//FIXME
				Texture* texture
					= RenderEngine::hostage()->createTexture( _name, 
					::floorf( rect[2] - rect[0] + 0.5f ), 
					::floorf( rect[3] - rect[1] + 0.5f ), PF_R8G8B8 );

				resourceImage->setRenderImage( texture );

				ResourceManager::hostage()->registerResource( resourceImage );
			}

			Texture* image = resourceImage->getTexture( 0 );

			//Holder<Application>::hostage()->update( 0.0f );
			Game::hostage()->tick(0.0f);

			RenderEngine::hostage()->beginScene();

			Game::hostage()->render();

			RenderEngine::hostage()->endScene();

			Application::hostage()->screenshot( image, mt::vec4f( _min.x, _min.y, _max.x, _max.y) );


			//image->writeToFile( "bl.bmp" );

			Sprite * node = Holder<SceneManager>::hostage()
				->createNodeT<Sprite>( "Sprite" );

			if( node == 0 )
			{
				return pybind::ret_none();
			}

			node->setImageResource( _name );

			node->activate();

			PyObject * embedding = node->getEmbedding();

			if( embedding == 0 )
			{
				node->destroy();
				return pybind::ret_none();
			}

			Game::hostage()
				->addHomeless( embedding );

			return embedding;		
		}

		static void setFullscreenMode( bool _fullscreen )
		{
			Application::hostage()->setFullscreenMode( _fullscreen );
		}

		static bool s_getFullscreenMode()
		{
			return Application::hostage()->getFullscreenMode();
		}

		static void addResourceListener( PyObject* _listener )
		{
			ResourceManager::hostage()->addListener( _listener );
		}
		static void removeResourceListener( PyObject* _listener )
		{
			ResourceManager::hostage()->removeListener( _listener );
		}

		static void renderOneFrame()
		{
			RenderEngine::hostage()->beginScene();
			Game::hostage()->render();
			RenderEngine::hostage()->endScene();
			RenderEngine::hostage()->swapBuffers();
		}
		static void writeImageToFile( const String& _resource, int _frame, const String& _filename )
		{
			ResourceImage * resource = ResourceManager::hostage()
				->getResourceT<ResourceImage>( _resource );

			if( resource == 0 )
			{
				MENGE_LOG_ERROR( "Error: Image resource not getting '%s'"
					, _resource.c_str() 
					);
			}

			Texture * img = resource->getTexture( _frame );

			RenderEngine::hostage()
				->saveImage( img, "user", _filename );
		}
		static void setSoundEnabled( bool _enabled )
		{
			Application::hostage()->setSoundEnabled( _enabled );
		}
		static void setParticlesEnabled( bool _enabled )
		{
			Application::hostage()->setParticlesEnabled( _enabled );
		}
		static void createResourceFromXml( const String& _xml )
		{
			ResourceManager::hostage()->createResourceFromXml( _xml );
		}

		static void s_createImageResource( const String& _resourceName, const String& _pakName, const String& _filename )
		{
			ResourceImageDefault* resImage = 
				ResourceManager::hostage()->getResourceT<ResourceImageDefault>( _resourceName );
			if( resImage == NULL )
			{
				ResourceFactoryParam param;
				param.category = _pakName;
				param.file = "";
				param.group = "";
				param.name = _resourceName;
				resImage = 
					static_cast<ResourceImageDefault*>
					( ResourceManager::hostage()->createResourceWithParam( "ResourceImageDefault", param  ) );

				ResourceManager::hostage()->registerResource( resImage );
			}
			resImage->addImagePath( _filename );
		}
		//static bool createFolder( const String& _path )
		//{
		//	return Holder<FileEngine>::hostage()->createFolder( _path );
		//}
		//static bool deleteFolder( const String& _path )
		//{
		//	return Holder<FileEngine>::hostage()->deleteFolder( _path );
		//}
		static mt::vec2f screenToLocal( const String& _layerName, const mt::vec2f& _point )
		{
			return Player::hostage()->getCurrentScene()->screenToLocal( _layerName, _point );
		}
		static void minimizeWindow()
		{
			Application::hostage()->minimizeWindow();
		}
		static void s_setMouseBounded( bool _bounded )
		{
			Application::hostage()->setMouseBounded( _bounded );
		}
		static bool setBlow( bool _active )
		{
			return SoundEngine::hostage()
				->setBlow( _active );
		}
		static float getBlow()
		{
			return SoundEngine::hostage()
				->getBlow();
		}
		static void setEnoughBlow( float _enoughBlow )
		{
			SoundEngine::hostage()
				->setEnoughBlow( _enoughBlow );
		}

		class PySoundSulkCallback
			: public SoundSulkCallback
		{
		public:
			PySoundSulkCallback( PyObject * _callback )
				: m_callback(_callback)
			{
				pybind::incref( m_callback );
			}

			~PySoundSulkCallback()
			{
				pybind::decref( m_callback );
			}

			void blow( float _blow ) override
			{
				pybind::call( m_callback, "(f)", _blow );
			}

		protected:
			PyObject * m_callback;
		};

		static void setBlowCallback( PyObject * _sulkcallback )
		{
			SoundEngine::hostage()
				->setSulkCallback( new PySoundSulkCallback( _sulkcallback ) );
		}

		static PhysicJoint2DInterface* s_createDistanceJoint( RigidBody2D* _body1, RigidBody2D* _body2, const mt::vec2f& _offset1, const mt::vec2f& _offset2, bool _collideBodies )
		{
			return PhysicEngine2D::hostage()->createDistanceJoint( _body1, _body2, _offset1, _offset2, _collideBodies );
		}

		static PhysicJoint2DInterface* s_createHingeJoint( RigidBody2D* _body1, RigidBody2D* _body2, const mt::vec2f& _offset1, const mt::vec2f& _limits, bool _collideBodies )
		{
			return PhysicEngine2D::hostage()->createHingeJoint( _body1, _body2, _offset1, _limits, _collideBodies );
		}

		static PhysicJoint2DInterface* s_createMouseJoint( RigidBody2D* _body, int _x, int _y )
		{
			return PhysicEngine2D::hostage()->createMouseJoint( _body, _x, _y );
		}

		static void s_destroyJoint( PhysicJoint2DInterface* _joint )
		{
			return PhysicEngine2D::hostage()->destroyJoint( _joint );
		}

		static void s_setCamera2DTarget( PyObject* _object )
		{
			Entity * entity = pybind::extract_nt<Entity*>( _object);

			Player::hostage()->getRenderCamera2D()->setTarget( (Node*)entity );
		}

		static void s_enableCamera2DTargetFollowing( bool _enable, float _force )
		{
			Player::hostage()->getRenderCamera2D()->enableTargetFollowing( _enable, _force );
		}

		static void s_setCamera2DBounds( const mt::vec2f& _leftUpper, const mt::vec2f& _rightLower )
		{
			Player::hostage()->getRenderCamera2D()->setBounds( _leftUpper, _rightLower );
		}

		static void s_setCursorPosition( float _x, float _y )
		{
			Arrow* arrow = Holder<Player>::hostage()->getArrow();
			arrow->setLocalPosition( mt::vec2f( _x, _y ) + arrow->getOffsetClick() );
		}

		static bool s_isInViewport( const mt::vec2f & _pos )
		{
			return Player::hostage()->getRenderCamera2D()->getViewport().testPoint( _pos );
		}

		static size_t s_getResourceCount( const String& _resourceFile )
		{
			return ResourceManager::hostage()
				->getResourceCount( _resourceFile );
		}

		static void s_enableTextureFiltering( bool _enable )
		{
			RenderEngine::hostage()
				->enableTextureFiltering( _enable );
		}

		static bool s_isTextureFilteringEnabled()
		{
			return RenderEngine::hostage()
				->isTextureFilteringEnabled();
		}

		static bool s_existText( const String & _key )
		{
			bool exist = TextManager::hostage()->existText( _key );

			return exist;
		}
	}

	static void classWrapping()
	{
		SCRIPT_CLASS_WRAPPING( Node );
		SCRIPT_CLASS_WRAPPING( Layer );
		SCRIPT_CLASS_WRAPPING( Layer2D );
		SCRIPT_CLASS_WRAPPING( Scene );
		SCRIPT_CLASS_WRAPPING( HotSpot );
		SCRIPT_CLASS_WRAPPING( Light2D );
		SCRIPT_CLASS_WRAPPING( ShadowCaster2D );
		SCRIPT_CLASS_WRAPPING( Sprite );
		SCRIPT_CLASS_WRAPPING( Animation );
		SCRIPT_CLASS_WRAPPING( Arrow );
		SCRIPT_CLASS_WRAPPING( TextField );
		SCRIPT_CLASS_WRAPPING( SoundEmitter );
		SCRIPT_CLASS_WRAPPING( Emitter );
		SCRIPT_CLASS_WRAPPING( Point );
		SCRIPT_CLASS_WRAPPING( TilePolygon );
		SCRIPT_CLASS_WRAPPING( Video );
		//SCRIPT_CLASS_WRAPPING( FFCamera3D );
		//SCRIPT_CLASS_WRAPPING( DiscreteEntity );
		//SCRIPT_CLASS_WRAPPING( RigidBody3D );
		//SCRIPT_CLASS_WRAPPING( Layer3D );
		SCRIPT_CLASS_WRAPPING( RigidBody2D );
		//SCRIPT_CLASS_WRAPPING( CapsuleController );
		SCRIPT_CLASS_WRAPPING( SceneNode3D );
		SCRIPT_CLASS_WRAPPING( Camera3D );
		SCRIPT_CLASS_WRAPPING( RenderMesh );
		SCRIPT_CLASS_WRAPPING( Window );
		SCRIPT_CLASS_WRAPPING( HotSpotImage );
		SCRIPT_CLASS_WRAPPING( Mesh_40_30 );
		//SCRIPT_CLASS_WRAPPING( Camera2D );
	}

	//REGISTER_SCRIPT_CLASS( Menge, Node, Base )
	void ScriptWrapper::nodeWrap()
	{
		classWrapping();

		pybind::class_<mt::vec2f>("vec2f")
			.def( pybind::init<float,float>() )
			//.attr( "x", &vec2f::x )
			//.def( boost::python::init<float,float>() )
			//.def( boost::python::self + boost::python::self )	// __add__
			//.def( boost::python::self - boost::python::self )          // __radd__
			//.def( boost::python::self * float() )           // __sub__
			//.def( boost::python::self / float() )          // __sub__
			//.def( float() * boost::python::self )         // __iadd__
			//.def( boost::python::self += boost::python::self )
			//.def( boost::python::self -= boost::python::self )
			//.def( boost::python::self *= float() )
			//.def( boost::python::self /= float() )
			;

		pybind::class_<mt::vec3f>("vec3f")
			.def( pybind::init<float,float,float>() )
			;

		pybind::class_<mt::vec4f>("vec4f")
			.def( pybind::init<float,float,float,float>() )
			;

		pybind::class_<mt::quatf>("quatf")
			.def( pybind::init<float,float,float,float>() )
			;

		pybind::class_<Viewport>("Viewport")
			.def( pybind::init<mt::vec2f,mt::vec2f>() )
			;

		/*pybind::class_<Color>("Color")
		.def( pybind::init<float,float,float,float>() )
		.def( "set", &Color::set )
		.def( "getA", &Color::getA )
		.def( "getR", &Color::getR )
		.def( "getG", &Color::getG )
		.def( "getB", &Color::getB )
		;*/

		pybind::class_<ColourValue>("Color")
			.def( pybind::init<float, float, float, float>() )
			.def( "getA", &ColourValue::getA )
			.def( "getR", &ColourValue::getR )
			.def( "getG", &ColourValue::getG )
			.def( "getB", &ColourValue::getB )
			;

		pybind::class_<PhysicJoint2DInterface>("Joint2D")
			//.def( pybind::init<float,float>() )
			;

		pybind::class_<Identity>("Identity")
			.def( "setName", &Node::setName )
			.def( "getName", &Node::getName )
			;

		pybind::interface_<Allocator2D>("Allocator2D", false)
			.def( "getLocalPosition", &Allocator2D::getLocalPosition )
			.def( "getLocalDirection", &Allocator2D::getLocalDirection )
			.def( "getOrigin", &Allocator2D::getOrigin )
			.def( "getScale", &Allocator2D::getScale )
			.def( "getAngle", &Allocator2D::getAngle )

			.def( "getWorldPosition", &Allocator2D::getWorldPosition )
			.def( "getWorldDirection", &Allocator2D::getWorldDirection )

			.def( "setLocalPosition", &Allocator2D::setLocalPositionInt )
			.def( "setLocalDirection", &Allocator2D::setLocalDirection )
			.def( "setScale", &Allocator2D::setScale )
			.def( "setOrigin", &Allocator2D::setOrigin )
			.def( "setRotate", &Allocator2D::setAngle ) //depricated
			.def( "setAngle", &Allocator2D::setAngle )
			.def( "translate", &Allocator2D::translate )
			;

		/*pybind::class_<FFCamera3D>("FFCamera3D")
		.def( pybind::init<>() )
		.def( "update", &FFCamera3D::update )
		.def( "activate", &FFCamera3D::activate )
		.def( "forward", &FFCamera3D::forward )
		.def( "left", &FFCamera3D::left )
		.def( "setActor", &FFCamera3D::setActor )
		.def( "yaw", &FFCamera3D::yaw )
		.def( "pitch", &FFCamera3D::pitch )
		.def( "zoom", &FFCamera3D::zoom )
		;*/

		/*	pybind::interface_<NodeRenderable>("NodeRenderable", false)
		.def( "hide", &NodeRenderable::hide )
		;*/

		/*	pybind::proxy_<Node, pybind::bases<Node, Allocator2D, NodeRenderable>>("Node", false)
		.def( "getScreenPosition", &Node::getScreenPosition )
		.def( "getParent", &Node::getParent )
		//.def( "setListener", &Node::setListener )
		.def( "isHide", &Node::isHide )
		.def( "alphaTo", &Node::alphaTo )
		.def( "setAlpha", &Node::setAlpha )
		;*/

		pybind::interface_<Resource>( "Resource", false )
			.def( "compile", &Resource::compile )
			.def( "release", &Resource::release )
			.def( "isCompile", &Resource::isCompile )
			;

		pybind::interface_<Renderable>( "Renderable", false )
			.def( "hide", &Node::hide )
			.def( "isHide", &Node::isHide )
			;

		pybind::interface_<Node, pybind::bases<Identity,Allocator2D,Resource,Renderable> >("Node", false)
			.def( "activate", &Node::activate )
			.def( "deactivate", &Node::deactivate )
			.def( "isActivate", &Node::isActivate )
			.def( "enable", &Node::enable )
			.def( "disable", &Node::disable )
			.def( "isEnable", &Node::isEnable )
			.def( "setUpdatable", &Node::setUpdatable )
			.def( "isUpdatable", &Node::isUpdatable )
			.def( "addChildren", &Node::addChildren )
			.def( "addChildrenFront", &Node::addChildrenFront )
			.def( "removeChildren", &Node::removeChildren )
			.def( "getChildren", &Node::getChildren )
			.def( "isChildren", &Node::isChildren )
			.def( "updatable", &Node::updatable )
			.def( "update", &Node::update )
			.def( "getParent", &Node::getParent )
			.def( "setListener", &Node::setListener )
			.def( "getListener", &Node::getListener )

			//.def( "getWorldPosition", &Node::getWorldPosition )
			//.def( "getWorldDirection", &Node::getWorldDirection )
			//.def( "setAlpha", &Node::setAlpha )
			.def( "getScreenPosition", &Node::getScreenPosition )
			.def( "setLocalColor", &Node::setLocalColor )
			.def( "setLocalAlpha", &Node::setLocalAlpha )
			.def( "getWorldColor", &Node::getWorldColor )
			.def( "getLocalColor", &Node::getLocalColor )

			.def( "localColorToCb", &Node::localColorToCb )
			.def( "localAlphaToCb", &Node::localAlphaToCb )
			.def( "localColorToStop", &Node::localColorToStop )

			.def( "moveToCb", &Node::moveToCb )
			.def( "moveToStop", &Node::moveToStop )

			.def( "angleToCb", &Node::angleToCb )
			.def( "angleToStop", &Node::angleToStop )
			.def( "scaleToCb", &Node::scaleToCb )
			.def( "scaleToStop", &Node::scaleToStop )

			.def( "accMoveToCb", &Node::accMoveToCb )
			.def( "accAngleToCb", &Node::accAngleToCb )
			;


		/*pybind::proxy_<SceneNode3D, pybind::bases<Node>>("SceneNode3D", false)
		.def( "getWorldOrient", &SceneNode3D::getWorldOrient )
		.def( "getWorldPosition", &SceneNode3D::getWorldPosition )
		.def( "getLocalOrient", &SceneNode3D::getLocalOrient )
		.def( "getPosition", &SceneNode3D::getLocalPosition )
		.def( "setPosition", &SceneNode3D::setLocalPosition )
		.def( "setOrient", &SceneNode3D::setLocalOrient )
		.def( "setScale", &SceneNode3D::setScale )
		.def( "yaw", &SceneNode3D::yaw )
		.def( "pitch", &SceneNode3D::pitch )
		.def( "roll", &SceneNode3D::roll )
		.def( "setFixedYawAxis", &SceneNode3D::setFixedYawAxis )
		.def( "translate", &SceneNode3D::translate )
		.def( "addChild", &SceneNode3D::addChild )
		.def( "setYawSpeed", &SceneNode3D::setYawSpeed )
		.def( "setYawLimits", &SceneNode3D::setYawLimits )
		.def( "getYaw", &SceneNode3D::getYaw )
		.def( "getPitch", &SceneNode3D::getPitch )
		.def( "setListener", &SceneNode3D::setListener )

		//.def( "getCamera", &SceneNode3D::getCamera )
		;*/

		{

			/*pybind::proxy_<RigidBody3D, pybind::bases<Node>>("RigidBody3D", false)
			.def( "applyForce", &RigidBody3D::applyForce )
			.def( "applyImpulse", &RigidBody3D::applyImpulse )
			.def( "applyAngularImpulse", &RigidBody3D::applyAngularImpulse )
			.def( "applyTorque", &RigidBody3D::applyTorque )
			.def( "setLinearVelocity", &RigidBody3D::setLinearVelocity )
			.def( "setAngularVelocity", &RigidBody3D::setAngularVelocity )
			.def( "setActive", &RigidBody3D::setActive )
			;

			pybind::proxy_<CapsuleController, pybind::bases<Node>>("CapsuleController", false)
			.def( "move", &CapsuleController::move )
			.def( "setPosition", &CapsuleController::setPosition )
			.def( "getFilteredPosition", &CapsuleController::getFilteredPosition )
			;

			pybind::proxy_<Camera3D, pybind::bases<SceneNode3D> >("Camera3D", false)
			.def( "setPosition", &Camera3D::setPosition )
			.def( "lookAt", &Camera3D::lookAt )
			//	.def( "yaw", &Camera3D::yaw )
			//	.def( "pitch", &Camera3D::pitch )
			//	.def( "roll", &Camera3D::roll )
			.def( "getDirection", &Camera3D::getDirection )
			;

			pybind::proxy_<DiscreteEntity, pybind::bases<SceneNode3D>>("DiscreteEntity", false)
			.def( "createRenderToTexture", &DiscreteEntity::createRenderToTexture )
			.def( "playAnimation", &DiscreteEntity::playAnimation )
			.def( "setMaterial", &DiscreteEntity::setMaterial )
			//.def( "playAnimation", &DiscreteEntity::playAnimation )
			;

			;*/

			pybind::interface_<Allocator3D>("Allocator3D", false)
				.def( "scale", &Allocator3D::scale )
				;

			pybind::proxy_<SceneNode3D, pybind::bases<Node, Allocator3D> >("SceneNode3D", false)
				.def( "yaw", &SceneNode3D::yaw )
				.def( "pitch", &SceneNode3D::pitch )
				.def( "getYaw", &SceneNode3D::getYaw )
				.def( "getPitch", &SceneNode3D::getPitch )
				.def( "yawTime", &SceneNode3D::yawTime )
				.def( "pitchTime", &SceneNode3D::pitchTime )
				.def( "setListener", &SceneNode3D::setListener )
				;

			/*pybind::proxy_<Layer3D, pybind::bases<SceneNode3D> >("Layer3D", false)
			//.def( "addCamera", &Layer3D::addCamera )
			//.def( "getCamera", &Layer3D::getCamera )	
			.def( "addController", &Layer3D::addController )
			.def( "getController", &Layer3D::getController )		
			//.def( "getNode", &Layer3D::getNode )
			;*/

			{

				pybind::proxy_< RenderMesh, pybind::bases<SceneNode3D> >("RenderMesh", false)
					//.def( "createRenderTarget", &RenderMesh::createRenderTarget )
					//.def( "setMaterial", &RenderMesh::setMaterial )
					;

				pybind::proxy_<Camera3D, pybind::bases<SceneNode3D> >("Camera3D", false)
					//	.def( "setPosition", &Camera3D::setPosition )
					.def( "lookAt", &Camera3D::lookAt )
					.def( "yaw", &Camera3D::yaw )
					.def( "pitch", &Camera3D::pitch )
					//.def( "roll", &Camera3D::roll )
					//	.def( "getDirection", &Camera3D::getDirection )
					;
			}

			{
				pybind::proxy_<Emitter, pybind::bases<Node> >("Emitter", false)
					.def( "play", &Emitter::play )
					.def( "playFromPosition", &Emitter::playFromPosition )
					.def( "stop", &Emitter::stop )
					.def( "pause", &Emitter::pause )
					.def( "restart", &Emitter::restart )
					.def( "setLooped", &Emitter::setLooped )
					.def( "getLooped", &Emitter::getLooped )
					.def( "setAutoPlay", &Emitter::setAutoPlay )
					.def( "getAutoPlay", &Emitter::getAutoPlay )				
					.def( "setLeftBorder", &Emitter::setLeftBorder )
					.def( "setResource", &Emitter::setResource )
					.def( "setEmitter", &Emitter::setEmitter )
					.def( "setEmitterRelative", &Emitter::setEmitterRelative )
					;

				pybind::proxy_<SoundEmitter, pybind::bases<Node> >("SoundEmitter", false)
					.def( "play", &SoundEmitter::play )
					.def( "pause", &SoundEmitter::pause )
					.def( "stop", &SoundEmitter::stop )
					.def( "isPlaying", &SoundEmitter::isPlaying )
					.def( "setVolume", &SoundEmitter::setVolume )
					.def( "getVolume", &SoundEmitter::getVolume )
					.def( "setLooped", &SoundEmitter::setLooped )
					.def( "isLooping", &SoundEmitter::isLooping )
					.def( "setSoundResource", &SoundEmitter::setSoundResource )
					;

				pybind::proxy_<TextField, pybind::bases<Node> >("TextField", false)
					.def( "setText", &TextField::setText )
					.def( "setHeight", &TextField::setHeight )
					.def( "getText", &TextField::getText )
					.def( "getHeight", &TextField::getHeight )
					.def( "setOutlineColor", &TextField::setOutlineColor )
					.def( "getOutlineColor", &TextField::getOutlineColor )
					.def( "getLength", &TextField::getLength )
					.def( "setMaxLen", &TextField::setMaxLen )
					.def( "getLineOffset", &TextField::getLineOffset )
					.def( "setLineOffset", &TextField::setLineOffset )
					.def( "setResource", &TextField::setResource )
					.def( "getResource", &TextField::getResource )
					.def( "setOutlineResource", &TextField::setOutlineResource )
					.def( "getCenterAlign", &TextField::getCenterAlign )
					.def( "setCenterAlign", &TextField::setCenterAlign )
					.def( "getRightAlign", &TextField::getRightAlign )
					.def( "setRightAlign", &TextField::setRightAlign )
					.def( "getLeftAlign", &TextField::getLeftAlign )
					.def( "setLeftAlign", &TextField::setLeftAlign )
					.def( "getCharOffset", &TextField::getCharOffset )
					.def( "setCharOffset", &TextField::setCharOffset )
					.def( "setTextByKey", &TextField::setTextByKey )
					.def( "getTextKey", &TextField::getTextKey )
					;

				pybind::proxy_<Arrow, pybind::bases<Node> >("Arrow", false)
					.def( "setOffsetClick", &Arrow::setOffsetClick )
					.def( "getOffsetClick", &Arrow::getOffsetClick )
					.def( "addHotSpot", &Arrow::addHotSpot )
					.def( "getCurrentHotSpot", &Arrow::getCurrentHotSpot )
					;

				pybind::proxy_<Point, pybind::bases<Node> >("Point", false)
					.def( "testHotSpot", &Point::testHotSpot )
					;

				pybind::interface_<Layer, pybind::bases<Node> >("Layer", false)
					.def( "getSize", &Layer::getSize )
					.def( "setRenderViewport", &Layer::setRenderViewport )
					.def( "getRenderViewport", &Layer::getRenderViewport )
					;

				pybind::proxy_<Layer2D, pybind::bases<Layer> >("Layer2D", false)
					.def( "setParallaxFactor", &Layer2D::setParallaxFactor )
					.def( "getParallaxFactor", &Layer2D::getParallaxFactor )
					.def( "screenToLocal", &Layer2D::screenToLocal )
					;

				pybind::proxy_<Scene, pybind::bases<Node> >("Scene", false)
					.def( "layerAppend", &Scene::layerAppend )
					.def( "layerRemove", &Scene::layerRemove )
					.def( "layerHide", &Scene::layerHide ) // depricated
					.def( "getNode", &Scene::getNode )
					.def( "getLayerSize", &Scene::getLayerSize ) // depricated
					.def( "setRenderTarget", &Scene::setRenderTarget )
					.def( "renderSelf", &Scene::renderSelf )
					.def( "blockInput", &Scene::blockInput )
					.def( "getBlockInput", &Scene::getBlockInput )
					.def( "setCameraPosition", &Scene::setCameraPosition )
					.def( "enableCameraFollowing", &Scene::enableCameraFollowing )
					.def( "setCameraTarget", &Scene::setCameraTarget )
					.def( "setCameraBounds", &Scene::setCameraBounds )
					;



				pybind::proxy_<HotSpot, pybind::bases<Node> >("HotSpot", false)
					.def( "enableGlobalMouseEvent", &HotSpot::enableGlobalMouseEvent )
					.def( "enableGlobalKeyEvent", &HotSpot::enableGlobalKeyEvent )				
					.def( "addPoint", &HotSpot::addPoint )
					.def( "clearPoints", &HotSpot::clearPoints )
					.def( "pick", &HotSpot::pick )
					;

				pybind::proxy_<HotSpotImage, pybind::bases<HotSpot> >("HotSpotImage", false)
					.def( "setResourceName", &HotSpotImage::setResourceName )
					.def( "setFrame", &HotSpotImage::setFrame )
					.def( "setAlphaTest", &HotSpotImage::setAlphaTest )
					.def( "getAlphaTest", &HotSpotImage::getAlphaTest )
					;


				pybind::proxy_<Sprite, pybind::bases<Node> >("Sprite", false)
					.def( "setImageIndex", &Sprite::setImageIndex )
					.def( "getImageIndex", &Sprite::getImageIndex )
					.def( "getImageCount", &Sprite::getImageCount )
					.def( "setImageResource", &Sprite::setImageResource )
					.def( "getImageResource", &Sprite::getImageResource )
					.def( "getImageSize", &Sprite::getImageSize )
					.def( "setScale", &Sprite::setScale )
					.def( "getScale", &Sprite::getScale )
					.def( "setPercentVisibility", &Sprite::setPercentVisibility )
					.def( "setPercentVisibilityToCb", &Sprite::setPercentVisibilityToCb )
					.def( "setPercentVisibilityToStop", &Sprite::setPercentVisibilityToStop )
					.def( "flip", &Sprite::flip )
					.def( "getCenterAlign", &Sprite::getCenterAlign )
					.def( "setCenterAlign", &Sprite::setCenterAlign )
					.def( "setImageAlpha", &Sprite::setImageAlpha )
					.def( "disableTextureColor", &Sprite::disableTextureColor )
					.def( "setTextureMatrixOffset", &Sprite::setTextureMatrixOffset )
					.def( "setAlphaImageIndex", &Sprite::setAlphaImageIndex )
					;
				{
					pybind::proxy_<Animation, pybind::bases<Sprite> >("Animation", false)
						.def( "play", &Animation::play )
						.def( "stop", &Animation::stop )
						.def( "pause", &Animation::pause )
						.def( "resume", &Animation::resume )

						.def( "setLooped", &Animation::setLooped )
						.def( "getLooped", &Animation::getLooped )					
						.def( "setAnimationResource", &Animation::setAnimationResource )
						.def( "setAnimationFactor", &Animation::setAnimationFactor )
						.def( "getAnimationFactor", &Animation::getAnimationFactor )
						.def( "getCurrentFrame", &Animation::getCurrentFrame )
						.def( "getFrameCount", &Animation::getFrameCount )
						.def( "setCurrentFrame", &Animation::setCurrentFrame )
						;
				}

				pybind::proxy_<RigidBody2D, pybind::bases<Node> >("RigidBody2D", false)
					.def( "applyForce", &RigidBody2D::applyForce )
					.def( "applyImpulse", &RigidBody2D::applyImpulse )
					.def( "applyConstantForce", &RigidBody2D::applyConstantForce )
					.def( "removeConstantForce", &RigidBody2D::removeConstantForce )
					.def( "setDirectionForce", &RigidBody2D::setDirectionForce )
					.def( "wakeUp", &RigidBody2D::wakeUp )
					.def( "getMass", &RigidBody2D::getMass )
					.def( "getLinearVelocity", &RigidBody2D::getLinearVelocity )
					.def( "setLinearVelocity", &RigidBody2D::setLinearVelocity )
					.def( "unsetLinearVelocity", &RigidBody2D::unsetLinearVelocity )
					.def( "freeze", &RigidBody2D::freeze )
					.def( "setCollisionMask", &RigidBody2D::setCollisionMask )
					.def( "enableStabilization", &RigidBody2D::enableStabilization )
					;

				pybind::proxy_<TilePolygon, pybind::bases<RigidBody2D> >("TilePolygon", false)
					;

				pybind::proxy_<Video, pybind::bases<Node> >("Video", false)
					.def( "play", &Video::play )
					.def( "stop", &Video::stop )
					.def( "pause", &Video::pause )
					;

				pybind::proxy_<Window, pybind::bases<Node> >("Window", false)
					.def( "setClientSize", &Window::setClientSize )
					.def( "setClientSizeClip", &Window::setClientSizeClip )
					.def( "setClientSizeInTiles", &Window::setClientSizeInTiles )
					.def( "getClientSize", &Window::getClientSize )
					.def( "getWindowSize", &Window::getWindowSize )
					.def( "getTileSize", &Window::getTileSize )
					;
				pybind::proxy_<Mesh_40_30, pybind::bases<Node> >("Mesh_40_30", false)
					.def( "setAmplitude", &Mesh_40_30::setAmplitude )
					.def( "setFrequency", &Mesh_40_30::setFrequency )
					;

				//pybind::proxy_<Camera2D, pybind::bases<Node> >("Camera2D", false)
				//	;
			}		

			pybind::def( "setCurrentScene", &ScriptMethod::setCurrentScene );
			pybind::def( "setCurrentSceneCb", &ScriptMethod::s_setCurrentSceneCb );
			pybind::def( "getCurrentScene", &ScriptMethod::getCurrentScene );
			pybind::def( "setCamera2DPosition", &ScriptMethod::setCamera2DPosition );
			pybind::def( "getCamera2DPosition", &ScriptMethod::s_getCamera2DPosition );
			pybind::def( "setCamera2DDirection", &ScriptMethod::setCamera2DDirection );
			pybind::def( "setCamera2DTarget", &ScriptMethod::s_setCamera2DTarget );
			pybind::def( "enableCamera2DFollowing", &ScriptMethod::s_enableCamera2DTargetFollowing );
			pybind::def( "setCamera2DBounds", &ScriptMethod::s_setCamera2DBounds );

			pybind::def( "createNode", &ScriptMethod::createNode );
			pybind::def( "createNodeFromXml", &ScriptMethod::createNodeFromXml );
			pybind::def( "destroyNode", &ScriptMethod::destroyNode );

			pybind::def( "destroyScene", &ScriptMethod::destroyScene );
			pybind::def( "destroySceneByName", &ScriptMethod::destroySceneByName );

			pybind::def( "schedule", &ScriptMethod::schedule );
			pybind::def( "scheduleRemove", &ScriptMethod::scheduleRemove );
			pybind::def( "scheduleRemoveAll", &ScriptMethod::scheduleRemoveAll );
			pybind::def( "scheduleStopAll", &ScriptMethod::scheduleStopAll );
			pybind::def( "scheduleResumeAll", &ScriptMethod::scheduleResumeAll );
			pybind::def( "scheduleFreeze", &ScriptMethod::s_scheduleFreeze );

			pybind::def( "getMouseX", &ScriptMethod::getMouseX );
			pybind::def( "getMouseY", &ScriptMethod::getMouseY );
			pybind::def( "setCursorPosition", &ScriptMethod::s_setCursorPosition );

			pybind::def( "setArrow", &ScriptMethod::setArrow );
			pybind::def( "getArrow", &ScriptMethod::getArrow );

			pybind::def( "directResourceCompile", &ScriptMethod::directResourceCompile );
			pybind::def( "directResourceRelease", &ScriptMethod::directResourceRelease );
			pybind::def( "directResourceUnload", &ScriptMethod::directResourceUnload );
			pybind::def( "directResourceFileCompile", &ScriptMethod::s_directResourceFileCompile );
			pybind::def( "deferredResourceFileCompile", &ScriptMethod::s_deferredResourceFileCompile );
			pybind::def( "directResourceFileRelease", &ScriptMethod::s_directResourceFileRelease );
			pybind::def( "directResourceFileUnload", &ScriptMethod::s_directResourceFileUnload );

			pybind::def( "quitApplication", &ScriptMethod::quitApplication );
			pybind::def( "createShot", &ScriptMethod::createShot );
			pybind::def( "setFullscreenMode", &ScriptMethod::setFullscreenMode );
			pybind::def( "getFullscreenMode", &ScriptMethod::s_getFullscreenMode );
			pybind::def( "addResourceListener", &ScriptMethod::addResourceListener );
			pybind::def( "removeResourceListener", &ScriptMethod::removeResourceListener );
			pybind::def( "renderOneFrame", &ScriptMethod::renderOneFrame );
			pybind::def( "writeImageToFile", &ScriptMethod::writeImageToFile );
			pybind::def( "createResourceFromXml", &ScriptMethod::createResourceFromXml );
			pybind::def( "createImageResource", &ScriptMethod::s_createImageResource );
			//pybind::def( "createFolder", &ScriptMethod::createFolder );
			//pybind::def( "deleteFolder", &ScriptMethod::deleteFolder );
			pybind::def( "screenToLocal", &ScriptMethod::screenToLocal );
			pybind::def( "minimizeWindow", &ScriptMethod::minimizeWindow );
			pybind::def( "setMouseBounded", &ScriptMethod::s_setMouseBounded );

			pybind::def( "getHotSpotPoints", &ScriptMethod::s_getHotSpotPoints );
			pybind::def( "getHotSpotImageSize", &ScriptMethod::s_getHotSpotImageSize );

			pybind::def( "setBlow", &ScriptMethod::setBlow );
			pybind::def( "getBlow", &ScriptMethod::getBlow );
			pybind::def( "setEnoughBlow", &ScriptMethod::setEnoughBlow );
			pybind::def( "setBlowCallback", &ScriptMethod::setBlowCallback );

			pybind::def( "createDistanceJoint", &ScriptMethod::s_createDistanceJoint );
			pybind::def( "createHingeJoint", &ScriptMethod::s_createHingeJoint );
			pybind::def( "createMouseJoint", &ScriptMethod::s_createMouseJoint );
			pybind::def( "destroyJoint", &ScriptMethod::s_destroyJoint );

			pybind::def( "isKeyDown", &ScriptMethod::s_isKeyDown );
			pybind::def( "isMouseDown", &ScriptMethod::s_isMouseDown );
			pybind::def( "isInViewport", &ScriptMethod::s_isInViewport );
			pybind::def( "getResourceCount", &ScriptMethod::s_getResourceCount );
			pybind::def( "enableTextureFiltering", &ScriptMethod::s_enableTextureFiltering );
			pybind::def( "isTextureFilteringEnabled", &ScriptMethod::s_isTextureFilteringEnabled );

			pybind::def( "getContentResolution", &ScriptMethod::s_getContentResolution );
			pybind::def( "validResource", &ScriptMethod::s_validResource );
			pybind::def( "existText", &ScriptMethod::s_existText );
			pybind::def( "openUrlInDefaultBrowser", &ScriptMethod::openUrlInDefaultBrowser );
			pybind::def( "isExistConfiguration", &ScriptMethod::isExistConfiguration );
			pybind::def( "existFile", &ScriptMethod::existFile );
			pybind::def( "executeProgram", &ScriptMethod::executeProgram );
		}
	}
}
