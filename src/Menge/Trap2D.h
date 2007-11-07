#	pragma once

#	include "SceneNode2D.h"

#	include "math/polygon.h"

#	include "math/box2.h"

#	include <list>


namespace Menge
{
	/*	Trapped under Ice!  */

	class	Trap2D 
		: public SceneNode2D
	{
		OBJECT_DECLARE(Trap2D);
	public:
		Trap2D();
	public:
		void loader( TiXmlElement * _xml ) override;
		void update( float _timing ) override;
		void debugRender() override;

	public:
		void setEnterZoneCallback( PyObject * _event );
		void setLeaveZoneCallback( PyObject * _event );

	protected:
		bool _activate() override;
		void _deactivate() override;

	private:
		mt::polygon	m_polygon;

		mt::box2f	m_bbox;

		typedef std::list<std::string> TListSceneNodesNames;
		TListSceneNodesNames	m_sceneNodesNames;

		typedef std::list<SceneNode2D*> TListSceneNodes;
		TListSceneNodes m_sceneNodes;

		typedef std::list<Node*> TListTrapped;
		TListTrapped m_trapped;

		typedef std::list<Node*> TListInZone;
		TListInZone	m_inZone;
	};
}