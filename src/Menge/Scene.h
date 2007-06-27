#	pragma once

#	include "SceneNode2D.h"

namespace Menge
{
	class Layer2D;
	class Layer3D;
	class Entity;

	class Scene
		: public SceneNode2D
	{
		OBJECT_DECLARE( Scene )
	public:
		Scene();

	public:
		void layerAppend( const std::string & _layer, SceneNode2D * _node );

		void addLayer2D( Layer2D * _layer );
		void addLayer3D( Layer3D * _layer );

	public:
		void loader(TiXmlElement *_xml) override;

	protected:
		bool _activate() override;

	private:
		std::string m_scriptFile;

		typedef std::list< Layer2D * > TListLayer2D;
		TListLayer2D m_listLayer2D;

		typedef std::list< Layer3D * > TListLayer3D;
		TListLayer3D m_listLayer3D;
	};
}