#	pragma once

#	include "SceneNode2D.h"

#	include "math/polygon.h"

#	include <list>


namespace Menge
{
	/*	Trapped under Ice  */

	class	Trap2D : public SceneNode2D
	{
		OBJECT_DECLARE(Trap2D);
	public:
		Trap2D();
	public:
		void loader( TiXmlElement * _xml ) override;
		void update( float _timing ) override;
		void debugRender() override;
	public:
		void setEnterZoneCallback(PyObject * _object);
		void setLeaveZoneCallback(PyObject * _object);
	private:

		mt::polygon	m_polygon;

		typedef std::list<std::string> TListSceneNodesNames;
		TListSceneNodesNames	m_sceneNodes;

		typedef std::list<SceneNode2D*> TListTrapped;
		TListTrapped m_trapped;

		typedef std::list<SceneNode2D*> TListInZone;
		TListInZone	m_inZone;
	};
}