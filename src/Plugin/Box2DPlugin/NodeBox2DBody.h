#pragma once

#include "Kernel/Node.h"
#include "Core/Polygon.h"

#include "Box2DBody.h"

namespace Mengine
{
    class NodeBox2DBody
        : public Node
    {
    public:
		NodeBox2DBody();
		~NodeBox2DBody();

	public:
		void setBox2DBody( const Box2DBodyPtr & _body );
		const Box2DBodyPtr & getBox2DBody() const;

	protected:
		void _update( float _current, float _timing ) override;

	protected:
		void updateLocalMatrix() const override;
        
	protected:
		Box2DBodyPtr m_body;
    };
}