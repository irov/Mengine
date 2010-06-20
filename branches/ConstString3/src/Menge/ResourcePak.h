#	pragma once

#	include "Manager/ConstManager.h"

#	include "XmlEngine.h"

#	include <set>

namespace Menge
{
	class Scene;
	class Arrow;
	class Entity;
	class Resource;

	struct ResourcePakDesc
	{
		ConstString name;
		String path;
		String description;
		bool preload;
	};

	class ResourcePak
	{
	public:
		ResourcePak( const ResourcePakDesc & _desc, const String & _baseDir );

	public:
		bool preload() const;

		const ConstString & getName() const;
		const String & getPath() const;

	public:
		void load();
		void loadResource();

	public:
		void loaderPak_( XmlElement * _xml );
		void loaderResources_( XmlElement * _xml );
		void loaderScenes_( XmlElement * _xml );
		void loaderArrows_( XmlElement * _xml );
		void loaderEntities_( XmlElement * _xml );
		void loaderResource_( XmlElement * _xml );

	protected:
		void addModulePath_( const String & _path );

	protected:
		void addScene_( const ConstString & _name );
		void addArrow_( const ConstString & _name );
		void addEntity_( const ConstString & _name );
		void addResource_( const ConstString & _name );

	protected:
		ResourcePakDesc m_desc;
		String m_baseDir;

		String m_pathScenes;
		String m_pathArrows;
		String m_pathEntities;
		String m_pathResources;
		String m_pathScripts;
		String m_pathTexts;

		typedef std::set<ConstString> TSetArrows;
		TSetArrows m_arrows;

		typedef std::set<ConstString> TSetEntities;
		TSetEntities m_entities;

		typedef std::set<ConstString> TSetResources;
		TSetResources m_resources;
	};
}