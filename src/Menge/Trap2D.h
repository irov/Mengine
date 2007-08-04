#	pragma once

#	include "SceneNode2D.h"

#	include "math/polygon.h"

#	include <list>


namespace Menge
{
	typedef void (*EnterZoneCallback2D)(SceneNode2D* obj);
	typedef void (*LeaveZoneCallback2D)(SceneNode2D* obj);

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
		void setEnterZoneCallback(EnterZoneCallback2D _callback);
		void setLeaveZoneCallback(LeaveZoneCallback2D _callback);
	private:

		mt::polygon	m_polygon;

		typedef std::list<std::string> TListSceneNodesNames;
		TListSceneNodesNames	m_sceneNodes;

		EnterZoneCallback2D	m_enterZoneCallback;
		LeaveZoneCallback2D	m_leaveZoneCallback;

		typedef std::list<SceneNode2D*> TListTrapped;
		TListTrapped m_trapped;

		typedef std::list<SceneNode2D*> TListInZone;
		TListInZone	m_inZone;
	};
}