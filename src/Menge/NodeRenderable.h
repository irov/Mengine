#	pragma once

#	include "Node.h"

namespace Menge
{
	class NodeRenderable
		: public virtual Node
	{
	public:
		NodeRenderable();

		void render( bool _enableDebug ) override;
		//bool isRenderable() override;

	protected:
		virtual void _render( bool _enableDebug );

		virtual bool _renderBegin();
		virtual void _renderEnd();

	public:
		void loader( XmlElement * _xml ) override;
	};
}