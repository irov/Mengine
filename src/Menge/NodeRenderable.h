#	pragma once

#	include "Node.h"

namespace Menge
{
	class NodeRenderable
		: public virtual Node
	{
	public:
		NodeRenderable();

		void render() override;
		bool isRenderable() override;
	
	public:
		virtual void hide( bool _value );
		virtual bool isHide() const;	

	protected:
		virtual void _render();

		virtual bool _renderBegin();
		virtual void _renderEnd();

	public:
		void loader( TiXmlElement * _xml ) override;

	protected:
		bool m_hide;
	};
}