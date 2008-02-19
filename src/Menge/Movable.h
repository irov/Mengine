#	pragma once

#	include <string>

class XmlElement;

namespace Menge
{
	class SceneNode3D;

	class Movable
	{
	public:
		Movable();
		virtual ~Movable();
	public:
		virtual void loader( XmlElement * _xml ) = 0;
		virtual bool activate() = 0;
		virtual void deactivate() = 0;
	public:
		SceneNode3D * getParentNode() const;
		void setParentNode( SceneNode3D * _node );
	private:
		SceneNode3D * m_parent;
	};
}