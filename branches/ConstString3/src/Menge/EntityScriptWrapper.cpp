#	include "ScriptWrapper.h"

#	include "ScriptClassWrapper.h"

#	include "Entity.h"
#	include "Arrow.h"
#	include "Scene.h"

#	include "ResourceManager.h"
#	include "ResourceBinary.h"
#	include "LoaderEngine.h"

#	include "FileEngine.h"
#	include "ScriptEngine.h"
#	include "EntityManager.h"
#	include "Logger/Logger.h"

#	include "Scene.h"
#	include "Game.h"

#	include "pybind/pybind.hpp"

namespace Menge
{
	namespace ScriptMethod
	{
		static PyObject * setupEntity_( 
			Entity * _entity, 
			const mt::vec2f & _pos, 
			const mt::vec2f & _dir )
		{
			_entity->setLocalPosition( _pos );
			_entity->setLocalDirection( _dir );

			Game::get()
				->addHomeless( _entity );

			PyObject * embedding = _entity->getEmbed();

			return embedding;
		}

		static PyObject * createEntity(
			const ConstString & _name,
			const ConstString & _prototype, 
			const mt::vec2f & _pos, 
			const mt::vec2f & _dir )
		{
			Entity * entity = EntityManager::get()
								->createEntity( _name, _prototype );

			if( entity == 0 )
			{
				MENGE_LOG_ERROR( "Error: can't create Entity '%s''%s'"
					, _name.c_str() 
					, _prototype.c_str()
					);

				return 0;
			}

			PyObject * embedding = setupEntity_( entity, _pos, _dir );

			return embedding;
		}

		static PyObject * createEntityFromBinary(
			const ConstString & _name,
			const ConstString & _prototype,
			const ConstString & _binary, 
			const mt::vec2f & _pos, 
			const mt::vec2f & _dir )
		{
			Entity * entity = EntityManager::get()
				->createEntity( _name, _prototype );

			if( entity == 0 )
			{
				MENGE_LOG_ERROR( "Error: can't create Entity '%s''%s'"
					, _name.c_str() 
					, _prototype.c_str()
					);

				return 0;
			}

			ResourceBinary * resourceBinary = ResourceManager::get()
				->getResourceT<ResourceBinary>( _binary );

			if( resourceBinary == 0 )
			{
				MENGE_LOG_ERROR( "Error: can't create Entity '%s''%s' invalid binary resource '%s'"
					, _name.c_str() 
					, _prototype.c_str()
					, _binary.c_str()
					);

				entity->destroy();

				return 0;
			}

			const TBlobject & blobject = resourceBinary->getBlobject();

			if( LoaderEngine::get()
				->loadBinary( blobject, entity ) == false )
			{
				MENGE_LOG_ERROR( "Error: can't create Entity '%s''%s' invalid load binary from '%s'"
					, _name.c_str() 
					, _prototype.c_str()
					, _binary.c_str()
					);

				entity->destroy();

				return 0;
			}

			PyObject * embedding = setupEntity_( entity, _pos, _dir );

			return embedding;
		}
	}

	static void classWrapping()
	{
		SCRIPT_CLASS_WRAPPING( Entity );
		SCRIPT_CLASS_WRAPPING( Scene );
	}

	//REGISTER_SCRIPT_CLASS( Menge, Entity, Node )
	void ScriptWrapper::entityWrap()
	{
		classWrapping();

		pybind::proxy_<Entity, pybind::bases<Node> >("Entity", false)
			;

		pybind::proxy_<Arrow, pybind::bases<Entity> >("Arrow", false)
			.def( "setOffsetClick", &Arrow::setOffsetClick )
			.def( "getOffsetClick", &Arrow::getOffsetClick )
			.def( "setPolygon", &Arrow::setPolygon )
			.def( "getPolygon", &Arrow::getPolygon )
			;

		pybind::proxy_<Scene, pybind::bases<Entity> >("Scene", false)
			.def( "isSubScene", &Scene::isSubScene )
			.def( "getParentScene", &Scene::getParentScene )
			.def( "setRenderTarget", &Scene::setRenderTarget )
			.def( "renderSelf", &Scene::renderSelf )
			.def( "blockInput", &Scene::blockInput )
			.def( "getBlockInput", &Scene::getBlockInput )
			.def( "setCameraPosition", &Scene::setCameraPosition )
			.def( "enableCameraFollowing", &Scene::enableCameraFollowing )
			.def( "setCameraTarget", &Scene::setCameraTarget )
			.def( "setCameraBounds", &Scene::setCameraBounds )
			.def( "getMainLayer", &Scene::getMainLayer )
			;


		pybind::def( "createEntity", &ScriptMethod::createEntity );
		pybind::def( "createEntityFromBinary", &ScriptMethod::createEntityFromBinary );
	}
}
