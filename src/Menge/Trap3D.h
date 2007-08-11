#	pragma once

#	include "SceneNode3D.h"

#	include <list>

#	include "math\box.h"

namespace Menge
{
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
		void setEnterZoneCallback(PyObject * _object);
		void setLeaveZoneCallback(PyObject * _object);
	private:
		mt::boxf	m_boundingZone;

		typedef std::list<std::string> TListSceneNodesNames;
		TListSceneNodesNames	m_sceneNodes;

		typedef std::list<SceneNode3D*> TListTrapped;
		TListTrapped m_trapped;

		typedef std::list<SceneNode3D*> TListInZone;
		TListInZone	m_inZone;
	};
}