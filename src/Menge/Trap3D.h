#	pragma once

#	include "SceneNode3D.h"

#	include <list>

#	include "math\box3.h"

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
	protected:
		bool _activate() override;
		void _deactivate() override;
	private:
		mt::box3f	m_bbox;

		typedef std::list<std::string> TListSceneNodesNames;
		TListSceneNodesNames	m_sceneNodesNames;

		typedef std::list<SceneNode3D*> TListSceneNodes;
		TListSceneNodes m_sceneNodes;

		typedef std::list<Node*> TListTrapped;
		TListTrapped m_trapped;

		typedef std::list<Node*> TListInZone;
		TListInZone	m_inZone;
	};
}