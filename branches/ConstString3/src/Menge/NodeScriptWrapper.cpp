
#	include "ScriptWrapper.h"

#	include "ScriptClassWrapperDefine.h"
#	include "ScriptDeclarationDefine.h"

#	include "InputEngine.h"
#	include "NodeManager.h"
#	include "FileEngine.h"
#	include "Scene.h"
#	include "Game.h"

#	include "Consts.h"

#	include "ScriptEngine.h"
#	include "ScheduleManager.h"

#	include "TextManager.h"
#	include "ArrowManager.h"
#	include "SceneManager.h"
#	include "AccountManager.h"

#	include "ResourceManager.h"
#	include "ResourceImageDynamic.h"
#	include "ResourceImageDefault.h"
#	include "ResourceHotspotImage.h"

#	include "Player.h"
#	include "Application.h"
#	include "MousePickerSystem.h"

#	include "Amplifier.h"
#	include "Sprite.h"
#	include "Animation.h"
#	include "HotSpot.h"
#	include "Light2D.h"
#	include "ShadowCaster2D.h"
#	include "Arrow.h"
#	include "TextField.h"
#	include "SoundEmitter.h"
#	include "ParticleEmitter.h"
#	include "Point.h"
#	include "Camera3d.h"
//#	include "RigidBody3D.h"
//#	include "CapsuleController.h"
#	include "Skeleton.h"
#	include "Camera2D.h"

#	include "Layer2D.h"
#	include "Layer2DTexture.h"

#	include "RigidBody2D.h"

#	include "TilePolygon.h"

#	include "Video.h"
#	include "Window.h"
#	include "HotSpotImage.h"
#	include "Mesh_40_30.h"

#	include "Entity.h"

//#	include "DiscreteEntity.h"

#	include "SoundEngine.h"
#	include "Logger/Logger.h"
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

#	include <sstream>

namespace Menge
{
	namespace ScriptMethod
	{
		static std::size_t schedule( float _timing, PyObject * _script )
		{
			ScheduleManager * sm = Player::get()->getScheduleManager();

			std::size_t id = sm->schedule( _timing, _script );

			return id;
		}

		static void scheduleRemove( std::size_t _id )
		{
			ScheduleManager * sm = Player::get()->getScheduleManager();

			sm->remove( _id );
		}

		static void scheduleRemoveAll()
		{
			ScheduleManager * sm = Player::get()->getScheduleManager();

			sm->removeAll();
		}	

		static void scheduleStopAll()
		{
			ScheduleManager * sm = Player::get()->getScheduleManager();

			sm->setUpdatable( false );
		}

		static void scheduleResumeAll()
		{
			ScheduleManager * sm = Player::get()->getScheduleManager();

			sm->setUpdatable( true );
		}

		static void s_scheduleFreeze( std::size_t _id, bool _freeze )
		{
			ScheduleManager * sm = Player::get()->getScheduleManager();

			sm->freeze( _id, _freeze );
		}

		static void s_scheduleFreezeAll()
		{
			ScheduleManager* sm = Player::get()->getScheduleManager();
			sm->freezeAll( true );
		}

		static float s_scheduleTime( std::size_t _id )
		{
			ScheduleManager* sm = Player::get()->getScheduleManager();
			return sm->time( _id );
		}

		static float getMouseX()
		{
			const Resolution& contRes = Game::get()->getContentResolution();
			float mx = Player::get()->getArrow()->getLocalPosition().x;
			mx = mt::clamp( 0.0f, mx, static_cast<float>( contRes[0] ) );
			return mx;
		}

		static float getMouseY()
		{
			const Resolution& contRes = Game::get()->getContentResolution();
			float my = Player::get()->getArrow()->getLocalPosition().y;
			my = mt::clamp( 0.0f, my, static_cast<float>( contRes[1] ) );
			return my;
		}

		static bool s_isMouseDown( int _button )
		{
			return InputEngine::get()
				->isMouseButtonDown( _button );
		}

		static bool s_isKeyDown( int _key )
		{
			return InputEngine::get()
				->isKeyDown( static_cast<KeyCode>( _key ) );
		}

		static void setCurrentScene( const ConstString& _name, bool _destroyOld = false )
		{
			MENGE_LOG_INFO( "set current scene '%s'"
				, _name.c_str() 
				);

			Player::get()
				->setCurrentScene( _name, _destroyOld );
		}

		static void s_setCurrentSceneCb( const ConstString& _name, PyObject* _cb )
		{
			MENGE_LOG_INFO( "set current scene '%s'"
				, _name.c_str() 
				);

			ScriptEngine::get()
				->incref( _cb );

			Player::get()
				->setCurrentSceneCb( _name, _cb );
		}

		static Scene * getCurrentScene()
		{
			Scene * scene = Player::get()
				->getCurrentScene();

			return scene;
		}

		static void s_setArrow( const ConstString & _name )
		{
			Arrow * arrow = ArrowManager::get()
				->getArrow( _name );

			if( arrow == 0 )
			{
				MENGE_LOG_ERROR( "Error: can't setup arrow '%s'"
					, _name.c_str() 
					);

				return;
			}

			Player::get()->setArrow( arrow );
		}

		static Arrow * s_getArrow()
		{
			Arrow * arrow = Player::get()
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
			//Holder<Player>::get()
			//	->setCamera2DPosition( mt::vec2f(x, y) );
			// deprecated
		}

		static const mt::vec2f& s_getCamera2DPosition()
		{
			return Player::get()
				->getRenderCamera2D()->getViewport().begin;
		}

		static void setCamera2DDirection( float x, float y )
		{
			assert(!"NOT IMPLEMENTED");
			Scene * scene = getCurrentScene();
			
			//struct ForeachRender
			//	: public NodeForeach
			//{
			//	mt::vec2f dir;
			//	ForeachRender( const mt::vec2f& _dir )
			//		: dir(_dir)
			//	{}

			//	void apply( Node * children ) override
			//	{
			//		Node * child = dynamic_cast<Node*>(children);
			//		child->setLocalDirection(dir);
			//	}
			//};
			//scene->foreachChildren( ForeachRender(mt::vec2f(x,y)) );			
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
			const ConstString & name = _scene->getName();

			SceneManager::get()
				->destroyScene( name );
		}

		static void destroySceneByName( const ConstString & _nameScene )
		{
			SceneManager::get()
				->destroyScene( _nameScene );
		}

		static PyObject * createNode( const ConstString & _type )
		{
			Node * node = NodeManager::get()
				->createNode( _type );

			if( node == 0 )
			{
				return pybind::ret_none();
			}

			Holder<Game>::get()
				->addHomeless( node );

			PyObject * embedding = node->getEmbedding();

			if( embedding == 0 )
			{
				node->destroy();
				return pybind::ret_none();
			}

			return embedding;
		}

		static PyObject * createNodeFromXml( PyObject * _params  )
		{
			if( pybind::convert::is_string( _params ) == false )
			{
				return pybind::ret_none();
			}

			const TChar * xml_data = pybind::convert::to_string( _params );

			Node * node = Holder<NodeManager>::get()
				->createNodeFromXmlData( xml_data );

			if( node == 0 )
			{
				return pybind::ret_none();
			}

			Holder<Game>::get()
				->addHomeless( node );

			PyObject * embedding = node->getEmbedding();

			if( embedding == 0 )
			{
				node->destroy();
				return pybind::ret_none();
			}

			return embedding;
		}

		static void quitApplication()
		{
			Application::get()->quit();
		}

		static bool directResourceCompile( const ConstString& _nameResource )
		{
			bool result = ResourceManager::get()
				->directResourceCompile( _nameResource );

			return result;
		}

		static void directResourceRelease( const ConstString& _nameResource )
		{
			ResourceManager::get()
				->directResourceRelease( _nameResource );
		}

		static void directResourceUnload( const ConstString& _nameResource )
		{
			ResourceManager::get()
				->directResourceUnload( _nameResource );
		}

		static void s_directResourceFileCompile( const ConstString& _resourceFile )
		{
			ResourceManager::get()
				->directResourceFileCompile( _resourceFile );
		}

		static void s_deferredResourceFileCompile( PyObject* _resourceFiles, PyObject* _progressCallback )
		{
			TVectorConstString resourceFiles;
			if( pybind::convert::is_string( _resourceFiles ) == true )
			{
				ConstString resourceFile = pybind::extract<ConstString>( _resourceFiles );
				resourceFiles.push_back( resourceFile );
			}
			else if( pybind::list_check( _resourceFiles ) == true )
			{
				std::size_t listSize = pybind::list_size( _resourceFiles );
				for( std::size_t i = 0; i < listSize; ++i )
				{
					PyObject* listItem = pybind::list_getitem( _resourceFiles, i );
					if( pybind::convert::is_string( listItem ) == false )
					{
						MENGE_LOG_ERROR( "Error: (Menge.deferredResourceFileCompile) invalid argument" );
						return;
					}
					ConstString resourceFile = pybind::extract<ConstString>( listItem );
					resourceFiles.push_back( resourceFile );
				}
			}
			else
			{
				MENGE_LOG_ERROR( "Error: (Menge.deferredResourceFileCompile) invalid argument" );
				return;
			}

			TaskDeferredLoading* task = 
				new TaskDeferredLoading( resourceFiles, _progressCallback );

			TaskManager::get()
				->addTask( task );
		}

		static void s_directResourceFileRelease( const ConstString& _resourceFile )
		{
			ResourceManager::get()
				->directResourceFileRelease( _resourceFile );
		}

		static void s_directResourceFileUnload( const ConstString& _resourceFile )
		{
			ResourceManager::get()
				->directResourceFileUnload( _resourceFile );
		}

		static PyObject * createShot( const ConstString& _name, mt::vec2f _min,  mt::vec2f _max )
		{
			mt::vec4f rect( _min, _max );

			ResourceImageDynamic * resourceImage 
				= ResourceManager::get()->getResourceT<ResourceImageDynamic>( _name );

			if( resourceImage == NULL )
			{
				ResourceFactoryParam param;

				param.name = _name;
				param.category = Consts::get()->c_user;

				String group;

				Account * acc = AccountManager::get()
					->getCurrentAccount();

				if( acc != 0 )
				{
					String folder = acc->getFolder() + "/";
					param.group = folder;
				}

				resourceImage = ResourceManager::get()
					->createResourceWithParamT<ResourceImageDynamic>( Consts::get()->c_ResourceImageDynamic, param );

				//FIXME
				Texture* texture
					= RenderEngine::get()->createTexture( _name, 
					::floorf( rect[2] - rect[0] + 0.5f ), 
					::floorf( rect[3] - rect[1] + 0.5f ), PF_R8G8B8 );

				resourceImage->setRenderImage( texture );

				ResourceManager::get()->registerResource( resourceImage );
			}

			Texture* image = resourceImage->getTexture( 0 );

			//Holder<Application>::get()->update( 0.0f );
			Game::get()->tick(0.0f);

			RenderEngine::get()->beginScene();

			Game::get()->render();

			RenderEngine::get()->endScene();

			Application::get()->screenshot( image, mt::vec4f( _min.x, _min.y, _max.x, _max.y) );


			//image->writeToFile( "bl.bmp" );

			Sprite * node = Holder<NodeManager>::get()
				->createNodeT<Sprite>( Consts::get()->c_Sprite );

			if( node == 0 )
			{
				return pybind::ret_none();
			}

			node->setImageResource( _name );

			node->activate();

			Holder<Game>::get()
				->addHomeless( node );

			PyObject * embedding = node->getEmbedding();

			if( embedding == 0 )
			{
				node->destroy();
				return pybind::ret_none();
			}

			return embedding;		
		}

		static void setFullscreenMode( bool _fullscreen )
		{
			Application::get()->setFullscreenMode( _fullscreen );
		}

		static bool s_getFullscreenMode()
		{
			return Application::get()->getFullscreenMode();
		}

		static void addResourceListener( PyObject* _listener )
		{
			ResourceManager::get()->addListener( _listener );
		}
		static void removeResourceListener( PyObject* _listener )
		{
			ResourceManager::get()->removeListener( _listener );
		}

		static void renderOneFrame()
		{
			RenderEngine::get()->beginScene();
			Game::get()->render();
			RenderEngine::get()->endScene();
			RenderEngine::get()->swapBuffers();
		}
		static void writeImageToFile( const ConstString& _resource, int _frame, const String& _filename )
		{
			ResourceImage * resource = ResourceManager::get()
				->getResourceT<ResourceImage>( _resource );

			if( resource == 0 )
			{
				MENGE_LOG_ERROR( "Error: Image resource not getting '%s'"
					, _resource.c_str() 
					);
			}

			Texture * img = resource->getTexture( _frame );

			RenderEngine::get()
				->saveImage( img, Consts::get()->c_user, _filename );
		}
		static void setSoundEnabled( bool _enabled )
		{
			Application::get()->setSoundEnabled( _enabled );
		}
		static void setParticlesEnabled( bool _enabled )
		{
			Application::get()->setParticlesEnabled( _enabled );
		}
		static void createResourceFromXml( const String& _xml )
		{
			ResourceManager::get()->createResourceFromXml( _xml );
		}

		static void s_createImageResource( const ConstString& _resourceName, const ConstString& _pakName, const ConstString& _filename )
		{
			ResourceImageDefault* resImage = ResourceManager::get()
				->getResourceT<ResourceImageDefault>( _resourceName );

			if( resImage == NULL )
			{
				ResourceFactoryParam param;
				param.category = _pakName;
				param.group = Consts::get()->c_builtin_empty;
				param.name = _resourceName;

				resImage = ResourceManager::get()
					->createResourceWithParamT<ResourceImageDefault>( Consts::get()->c_ResourceImageDefault, param );

				ResourceManager::get()
					->registerResource( resImage );
			}

			resImage->addImagePath( _filename );
		}

		//static bool createFolder( const String& _path )
		//{
		//	return Holder<FileEngine>::get()->createFolder( _path );
		//}
		//static bool deleteFolder( const String& _path )
		//{
		//	return Holder<FileEngine>::get()->deleteFolder( _path );
		//}
		static mt::vec2f screenToLocal( const ConstString& _layerName, const mt::vec2f& _point )
		{
			return Player::get()->getCurrentScene()->screenToLocal( _layerName, _point );
		}
		static void minimizeWindow()
		{
			Application::get()->minimizeWindow();
		}
		static void s_setMouseBounded( bool _bounded )
		{
			if( Application::get()->getMouseBounded() != _bounded )
			{
				Application::get()->setMouseBounded( _bounded );
			}
		}
		static bool s_getMouseBounded()
		{
			return Application::get()->getMouseBounded();
		}
		static bool setBlow( bool _active )
		{
			return SoundEngine::get()
				->setBlow( _active );
		}
		static float getBlow()
		{
			return SoundEngine::get()
				->getBlow();
		}
		static void setEnoughBlow( float _enoughBlow )
		{
			SoundEngine::get()
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
			SoundEngine::get()
				->setSulkCallback( new PySoundSulkCallback( _sulkcallback ) );
		}

		static PhysicJoint2DInterface* s_createDistanceJoint( RigidBody2D* _body1, RigidBody2D* _body2, const mt::vec2f& _offset1, const mt::vec2f& _offset2, bool _collideBodies )
		{
			return PhysicEngine2D::get()->createDistanceJoint( _body1, _body2, _offset1, _offset2, _collideBodies );
		}

		static PhysicJoint2DInterface* s_createHingeJoint( RigidBody2D* _body1, RigidBody2D* _body2, const mt::vec2f& _offset1, const mt::vec2f& _limits, bool _collideBodies )
		{
			return PhysicEngine2D::get()->createHingeJoint( _body1, _body2, _offset1, _limits, _collideBodies );
		}

		static PhysicJoint2DInterface* s_createMouseJoint( RigidBody2D* _body, int _x, int _y )
		{
			return PhysicEngine2D::get()->createMouseJoint( _body, _x, _y );
		}

		static void s_destroyJoint( PhysicJoint2DInterface* _joint )
		{
			return PhysicEngine2D::get()->destroyJoint( _joint );
		}

		static void s_setCamera2DTarget( PyObject* _object )
		{
			Entity * entity = pybind::extract_nt<Entity *>( _object);

			Player::get()
				->getRenderCamera2D()
				->setTarget( (Node*)entity );
		}

		static void s_enableCamera2DTargetFollowing( bool _enable, float _force )
		{
			Player::get()
				->getRenderCamera2D()
				->enableTargetFollowing( _enable, _force );
		}

		static void s_setCamera2DBounds( const mt::vec2f& _leftUpper, const mt::vec2f& _rightLower )
		{
			Player::get()
				->getRenderCamera2D()
				->setBounds( _leftUpper, _rightLower );
		}

		static void s_setCursorPosition( float _x, float _y )
		{
			Arrow* arrow = Player::get()
				->getArrow();

			arrow->setLocalPosition( mt::vec2f( _x, _y ) + arrow->getOffsetClick() );
		}

		static bool s_isInViewport( const mt::vec2f & _pos )
		{
			return Player::get()
				->getRenderCamera2D()->getViewport().testPoint( _pos );
		}

		static size_t s_getResourceCount( const ConstString& _resourceFile )
		{
			return ResourceManager::get()
				->getResourceCount( _resourceFile );
		}

		static void s_enableTextureFiltering( bool _enable )
		{
			RenderEngine::get()
				->enableTextureFiltering( _enable );
		}

		static bool s_isTextureFilteringEnabled()
		{
			return RenderEngine::get()
				->isTextureFilteringEnabled();
		}

		static bool s_existText( const ConstString & _key )
		{
			bool exist = TextManager::get()->existText( _key );

			return exist;
		}

		static PyObject * s_pickHotspot()
		{
			Arrow * arrow = Player::get()
				->getArrow();

			TVectorPickerTraps traps;

			MousePickerSystem::get()
				->pickTrap( arrow, traps );

			PyObject * pyret = pybind::list_new(0);

			for( TVectorPickerTraps::iterator
				it = traps.begin(),
				it_end = traps.end();
			it != it_end;
			++it )
			{
				PyObject * embedding = (*it)->getEmbedding();

				pybind::list_appenditem( pyret, embedding );

				pybind::decref( embedding );
			}

			return pyret;
		}

		//////////////////////////////////////////////////////////////////////////
		class NodeGetChild
		{
		public:
			Node * getChild( const std::string & _name )
			{
				return m_parent->getChildren( _name, false );
			}

			void setParent( Node * _parent )
			{
				m_parent = _parent;
			}

		protected:
			Node * m_parent;
		};
		//////////////////////////////////////////////////////////////////////////
		static NodeGetChild * s_getChild( Node * _node )
		{
			static NodeGetChild * instance = 0;

			if( instance == 0 )
			{
				instance = new NodeGetChild;
			}

			instance->setParent( _node );

			return instance;
		}
		//////////////////////////////////////////////////////////////////////////
		static void s_setChild( Node * _node, NodeGetChild * _obj )
		{

		}

		class AffectorManager
		{
		public:
			//////////////////////////////////////////////////////////////////////////
			static void moveToStop( Node * _node )
			{
				_node->stopAffectors( ETA_POSITION );
				_node->setLinearSpeed( mt::vec2f::zero_v2 );
			}
			//////////////////////////////////////////////////////////////////////////
			static void moveToCb( Node * _node, float _time, const mt::vec2f& _point, PyObject* _cb )
			{
				moveToStop( _node );

				Affector* affector = 
					NodeAffectorCreator::newNodeAffectorInterpolateLinear(
					_cb, ETA_POSITION, _node, &Node::setLocalPosition
					, _node->getLocalPosition(), _point, _time
					, &mt::length_v2 
					);

				float invTime = 1.0f / _time;
				mt::vec2f linearSpeed = ( _point - _node->getLocalPosition() ) * invTime;

				_node->setLinearSpeed( linearSpeed );
				_node->addAffector( affector );
			}
			//////////////////////////////////////////////////////////////////////////
			static void accMoveToCb( Node * _node, float _time, const mt::vec2f& _point, PyObject* _cb )
			{
				mt::vec2f linearSpeed = _node->getLinearSpeed();

				moveToStop( _node );

				Affector* affector = 
					NodeAffectorCreator::newNodeAffectorInterpolateQuadratic(
					_cb, ETA_POSITION, _node, &Node::setLocalPosition
					, _node->getLocalPosition(), _point, linearSpeed, _time
					, &mt::length_v2
					);
				
				_node->addAffector( affector );
			}
			//////////////////////////////////////////////////////////////////////////
			static void angleToStop( Node * _node )
			{
				_node->stopAffectors( ETA_ANGLE );
				_node->setAngularSpeed(0.f);
			}
			//////////////////////////////////////////////////////////////////////////
			static void angleToCb( Node * _node, float _time, float _angle, PyObject* _cb )
			{
				angleToStop( _node );

				Affector* affector =
					NodeAffectorCreator::newNodeAffectorInterpolateLinear(
					_cb, ETA_ANGLE, _node, &Node::setAngle
					, _node->getAngle(), _angle, _time
					, &fabsf 
					);				

				float invTime = 1.0f / _time;
				float angularSpeed = ( _angle - _node->getAngle() ) * invTime;

				_node->setAngularSpeed( angularSpeed );				
				_node->addAffector( affector );
			}
			//////////////////////////////////////////////////////////////////////////
			static void accAngleToCb( Node * _node, float _time, float _angle, PyObject* _cb )
			{
				float angularSpeed = _node->getAngularSpeed();

				angleToStop( _node );

				Affector* affector = 
					NodeAffectorCreator::newNodeAffectorInterpolateQuadratic(
					_cb, ETA_ANGLE, _node, &Node::setAngle
					, _node->getAngle(), _angle, angularSpeed, _time
					, &fabsf
					);				

				_node->addAffector( affector );
			}
			//////////////////////////////////////////////////////////////////////////
			static void scaleToStop( Node * _node )
			{
				_node->stopAffectors( ETA_SCALE );
			}
			//////////////////////////////////////////////////////////////////////////
			static void scaleToCb( Node * _node, float _time, const mt::vec2f& _scale, PyObject* _cb )
			{
				scaleToStop( _node );

				Affector* affector = 
					NodeAffectorCreator::newNodeAffectorInterpolateLinear(
					_cb, ETA_SCALE, _node, &Node::setScale
					, _node->getScale(), _scale, _time
					, &mt::length_v2
					);

				_node->addAffector( affector );
			}
			//////////////////////////////////////////////////////////////////////////
			static void localColorToStop( Node * _node )
			{
				_node->stopAffectors( ETA_COLOR );
			}
			//////////////////////////////////////////////////////////////////////////
			static void localColorToCb( Node * _node, float _time, const ColourValue& _color, PyObject* _cb )
			{
				localColorToStop( _node );

				Affector* affector = 
					NodeAffectorCreator::newNodeAffectorInterpolateLinear(
					_cb, ETA_COLOR, _node, &Colorable::setLocalColor
					, _node->getLocalColor(), _color, _time, 
					&ColourValue::length_color
					);

				_node->addAffector( affector );
			}

			//////////////////////////////////////////////////////////////////////////
			static void localAlphaToCb( Node * _node, float _time, float _alpha, PyObject* _cb )
			{
				ColourValue color = _node->getLocalColor();
				color.setA( _alpha );

				localColorToCb( _node, _time, color, _cb );
			}
			//////////////////////////////////////////////////////////////////////////
			static void setPercentVisibilityToCb( Sprite * _sprite, float _time, const mt::vec2f& _percentX, const mt::vec2f& _percentY, PyObject* _cb )
			{
				_sprite->stopAffectors( ETA_VISIBILITY );

				Affector* affector = 
					NodeAffectorCreator::newNodeAffectorInterpolateLinear(
					_cb, ETA_VISIBILITY, _sprite, &Sprite::setPercentVisibilityVec4f
					, _sprite->getPercentVisibility(), mt::vec4f( _percentX, _percentY ), _time, 
					&mt::length_v4 
					);

				_sprite->addAffector( affector );
			}
			//////////////////////////////////////////////////////////////////////////
			static void setPercentVisibilityToStop( Sprite * _sprite )
			{
				_sprite->stopAffectors( ETA_VISIBILITY );
			}
		};

		//////////////////////////////////////////////////////////////////////////
		std::string vec2f_repr( PyObject * _obj, mt::vec2f * _v )
		{
			std::stringstream ss;
			ss << "<" << Py_TYPE(_obj)->tp_name << " object at " << _obj << " value " << _v->x << ", " << _v->y << ">";
			return ss.str();
		}

		//////////////////////////////////////////////////////////////////////////
		std::string color_repr( PyObject * _obj, ColourValue * _v )
		{
			std::stringstream ss;
			ss << "<" << Py_TYPE(_obj)->tp_name << " object at " << _obj << " value " << _v->getA() << ", " << _v->getR() << ", " << _v->getG() << ", " << _v->getB() << ">";
			return ss.str();
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
		SCRIPT_CLASS_WRAPPING( ParticleEmitter );
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
		SCRIPT_CLASS_WRAPPING( Layer2DTexture );
	}

	struct extract_const_string_type
		: public pybind::type_cast_result<ConstString>
	{
		ConstString apply( PyObject * _obj ) override
		{
			m_valid = false;

			if( PyString_Check( _obj ) )
			{
				m_valid = true;
				char * ch_buff;
				Py_ssize_t ch_size;

				if( PyString_AsStringAndSize( _obj, &ch_buff, &ch_size ) == 0 )
				{
					return ConstString( ch_buff, ch_size );
				}
			}
			else if( PyUnicode_Check( _obj ) )
			{
				m_valid = true;
				PyObject* strObj = PyUnicode_AsUTF8String( _obj );

				char * ch_buff;
				Py_ssize_t ch_size;

				if( PyString_AsStringAndSize( strObj, &ch_buff, &ch_size ) == 0 )
				{
					return ConstString( ch_buff, ch_size );
				}
			}

			pybind::throw_exception();

			return ConstString::none;
		}
		PyObject * wrap( ConstString _value ) override
		{
			return PyString_FromStringAndSize( _value.c_str(), _value.size() );
		}
	};

	//REGISTER_SCRIPT_CLASS( Menge, Node, Base )
	void ScriptWrapper::nodeWrap()
	{
		classWrapping();

		new extract_const_string_type();

		pybind::class_<ScriptMethod::NodeGetChild>( "NodeGetChild" )
			.def_getattro( &ScriptMethod::NodeGetChild::getChild )
			.def_getmap( &ScriptMethod::NodeGetChild::getChild )
			;

		pybind::class_<mt::vec2f>("vec2f")
			.def( pybind::init<float,float>() )
			.def_member( "x", &mt::vec2f::x )
			.def_member( "y", &mt::vec2f::y )
			.def_repr( &ScriptMethod::vec2f_repr )
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
			.def_member( "x", &mt::vec3f::x )
			.def_member( "y", &mt::vec3f::y )
			.def_member( "z", &mt::vec3f::z )
			;

		pybind::class_<mt::vec4f>("vec4f")
			.def( pybind::init<float,float,float,float>() )
			.def_member( "x", &mt::vec4f::x )
			.def_member( "y", &mt::vec4f::y )
			.def_member( "z", &mt::vec4f::z )
			.def_member( "w", &mt::vec4f::w )
			;

		pybind::class_<mt::quatf>("quatf")
			.def( pybind::init<float,float,float,float>() )
			.def_member( "w", &mt::quatf::w )
			.def_member( "x", &mt::quatf::x )
			.def_member( "y", &mt::quatf::y )
			.def_member( "z", &mt::quatf::z )			
			;

		pybind::class_<mt::polygon>("polygon")
			.def( "add_point", &mt::polygon::add_point )
			;

		pybind::class_<Viewport>("Viewport")
			.def( pybind::init<mt::vec2f,mt::vec2f>() )
			.def_member( "begin", &Viewport::begin )
			.def_member( "end", &Viewport::end )
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
			.def_property( "a", &ColourValue::getA, &ColourValue::setA )
			.def_property( "r", &ColourValue::getR, &ColourValue::setR )
			.def_property( "g", &ColourValue::getG, &ColourValue::setG )
			.def_property( "b", &ColourValue::getB, &ColourValue::setB )
			.def_repr( &ScriptMethod::color_repr )
			;

		pybind::class_<PhysicJoint2DInterface>("Joint2D")
			//.def( pybind::init<float,float>() )
			;

		pybind::class_<Identity>("Identity")
			.def( "setName", &Identity::setName )
			.def( "getName", &Identity::getName )
			.def( "getType", &Identity::getType )
			;

		pybind::interface_<Transformation2D>("Transformation2D", false)
			.def( "getLocalPosition", &Transformation2D::getLocalPosition )
			.def( "getLocalDirection", &Transformation2D::getLocalDirection )
			.def( "getOrigin", &Transformation2D::getOrigin )
			.def( "getScale", &Transformation2D::getScale )
			.def( "getAngle", &Transformation2D::getAngle )

			.def( "setLocalPosition", &Transformation2D::setLocalPositionInt )
			.def( "setLocalDirection", &Transformation2D::setLocalDirection )
			.def( "setScale", &Transformation2D::setScale )
			.def( "setOrigin", &Transformation2D::setOrigin )
			.def( "setRotate", &Transformation2D::setAngle ) //depricated
			.def( "setAngle", &Transformation2D::setAngle )
			.def( "translate", &Transformation2D::translate )
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

		pybind::interface_<Resource>( "Resource", false )
			.def( "compile", &Resource::compile )
			.def( "release", &Resource::release )
			.def( "isCompile", &Resource::isCompile )
			;

		pybind::interface_<Renderable>( "Renderable", false )
			.def( "hide", &Node::hide )
			.def( "isHide", &Node::isHide )
			;

		pybind::interface_<Colorable>( "Colorable", false )
			.def( "setLocalColor", &Node::setLocalColor )
			.def( "setLocalAlpha", &Node::setLocalAlpha )
			.def( "getLocalColor", &Node::getLocalColor )
			;

		pybind::interface_<Node, pybind::bases<Identity,Transformation2D,Colorable,Resource,Renderable> >("Node", false)
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
			//.def( "getListener", &Node::getListener )

			.def( "getWorldPosition", &Node::getWorldPosition )
			.def( "getWorldDirection", &Node::getWorldDirection )
			.def( "getScreenPosition", &Node::getScreenPosition )

			.def( "getWorldColor", &Node::getWorldColor )

			.def_static( "localColorToCb", &ScriptMethod::AffectorManager::localColorToCb )
			.def_static( "localAlphaToCb", &ScriptMethod::AffectorManager::localAlphaToCb )
			.def_static( "localColorToStop", &ScriptMethod::AffectorManager::localColorToStop )

			.def_static( "moveToCb", &ScriptMethod::AffectorManager::moveToCb )
			.def_static( "moveToStop", &ScriptMethod::AffectorManager::moveToStop )

			.def_static( "angleToCb", &ScriptMethod::AffectorManager::angleToCb )
			.def_static( "angleToStop", &ScriptMethod::AffectorManager::angleToStop )
			.def_static( "scaleToCb", &ScriptMethod::AffectorManager::scaleToCb )
			.def_static( "scaleToStop", &ScriptMethod::AffectorManager::scaleToStop )

			.def_static( "accMoveToCb", &ScriptMethod::AffectorManager::accMoveToCb )
			.def_static( "accAngleToCb", &ScriptMethod::AffectorManager::accAngleToCb )
			.def_property_static( "child", &ScriptMethod::s_getChild, &ScriptMethod::s_setChild )
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

			//pybind::proxy_<Layer3D, pybind::bases<SceneNode3D> >("Layer3D", false)
			//	//.def( "addCamera", &Layer3D::addCamera )
			//	//.def( "getCamera", &Layer3D::getCamera )	
			//	.def( "addController", &Layer3D::addController )
			//	.def( "getController", &Layer3D::getController )		
			//	//.def( "getNode", &Layer3D::getNode )
			//	;

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
				pybind::proxy_<ParticleEmitter, pybind::bases<Node> >("ParticleEmitter", false)
					.def( "play", &ParticleEmitter::play )
					.def( "playFromPosition", &ParticleEmitter::playFromPosition )
					.def( "stop", &ParticleEmitter::stop )
					.def( "pause", &ParticleEmitter::pause )
					.def( "restart", &ParticleEmitter::restart )
					.def( "setLooped", &ParticleEmitter::setLooped )
					.def( "getLooped", &ParticleEmitter::getLooped )
					.def( "setAutoPlay", &ParticleEmitter::setAutoPlay )
					.def( "getAutoPlay", &ParticleEmitter::getAutoPlay )				
					.def( "setLeftBorder", &ParticleEmitter::setLeftBorder )
					.def( "setResource", &ParticleEmitter::setResource )
					.def( "setEmitter", &ParticleEmitter::setEmitter )
					.def( "setEmitterRelative", &ParticleEmitter::setEmitterRelative )
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
					.def( "setPolygon", &Arrow::setPolygon )
					.def( "getPolygon", &Arrow::getPolygon )
					;

				pybind::proxy_<Point, pybind::bases<Node> >("Point", false)
					.def( "testHotSpot", &Point::testHotSpot )
					;

				pybind::interface_<Layer, pybind::bases<Node> >("Layer", false)
					.def( "getSize", &Layer::getSize )
					;

				pybind::proxy_<Layer2D, pybind::bases<Layer> >("Layer2D", false)
					.def( "setParallaxFactor", &Layer2D::setParallaxFactor )
					.def( "getParallaxFactor", &Layer2D::getParallaxFactor )
					.def( "setRenderViewport", &Layer2D::setRenderViewport )
					.def( "getRenderViewport", &Layer2D::getRenderViewport )
					.def( "screenToLocal", &Layer2D::screenToLocal )
					;

				pybind::proxy_<Layer2DTexture, pybind::bases<Layer2D> >("Layer2DTexture", false)
					.def( "setCameraOffset", &Layer2DTexture::setCameraOffset )
					.def( "getCameraOffset", &Layer2DTexture::getCameraOffset )
					.def( "setRenderTargetName", &Layer2DTexture::setRenderTargetName )
					.def( "getRenderTargetName", &Layer2DTexture::getRenderTargetName )
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

				pybind::interface_<GlobalHandleAdapter>("GlobalHandleAdapter", false)
					.def( "enableGlobalMouseEvent", &HotSpot::enableGlobalMouseEvent )
					.def( "enableGlobalKeyEvent", &HotSpot::enableGlobalKeyEvent )				
					;

				pybind::proxy_<HotSpot, pybind::bases<Node, GlobalHandleAdapter> >("HotSpot", false)
					.def( "addPoint", &HotSpot::addPoint )
					.def( "clearPoints", &HotSpot::clearPoints )
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
					//.def( "setScale", &Sprite::setScale )
					//.def( "getScale", &Sprite::getScale )
					.def( "setPercentVisibility", &Sprite::setPercentVisibility )
					.def_static( "setPercentVisibilityToCb", &ScriptMethod::AffectorManager::setPercentVisibilityToCb )
					.def_static( "setPercentVisibilityToStop", &ScriptMethod::AffectorManager::setPercentVisibilityToStop )
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
			pybind::def( "scheduleFreezeAll", &ScriptMethod::s_scheduleFreezeAll );
			pybind::def( "scheduleTime", &ScriptMethod::s_scheduleTime );

			pybind::def( "getMouseX", &ScriptMethod::getMouseX );
			pybind::def( "getMouseY", &ScriptMethod::getMouseY );
			pybind::def( "setCursorPosition", &ScriptMethod::s_setCursorPosition );

			pybind::def( "setArrow", &ScriptMethod::s_setArrow );
			pybind::def( "getArrow", &ScriptMethod::s_getArrow );

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
			pybind::def( "getMouseBounded", &ScriptMethod::s_getMouseBounded );

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

			pybind::def( "existText", &ScriptMethod::s_existText );

			pybind::def( "pickHotspot", &ScriptMethod::s_pickHotspot );
		}
	}
}
