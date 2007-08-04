#	pragma once

#	include "SceneNode3D.h"

#	include <list>

#	include "math\box.h"

namespace Menge
{
	typedef void (*EnterZoneCallback3D)(SceneNode3D* obj);
	typedef void (*LeaveZoneCallback3D)(SceneNode3D* obj);

	class	Trap3D : public SceneNode3D
	{
		OBJECT_DECLARE(Trap3D);
	public:
		Trap3D();
	public:
		void loader( TiXmlElement * _xml ) override;
		void update( float _timing ) override;
		void debugRender() override;
	public:
		void setEnterZoneCallback(EnterZoneCallback3D _callback);
		void setLeaveZoneCallback(LeaveZoneCallback3D _callback);
	private:
		mt::boxf	m_boundingZone;

		typedef std::list<std::string> TListSceneNodesNames;
		TListSceneNodesNames	m_sceneNodes;

		EnterZoneCallback3D	m_enterZoneCallback;
		LeaveZoneCallback3D	m_leaveZoneCallback;

		typedef std::list<SceneNode3D*> TListTrapped;
		TListTrapped m_trapped;

		typedef std::list<SceneNode3D*> TListInZone;
		TListInZone	m_inZone;
	};
}