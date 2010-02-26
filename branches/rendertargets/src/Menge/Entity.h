#	pragma once

#	include "Node.h"

namespace Menge
{
	class Scene;

	class Entity
		: public Node
	{
		FACTORABLE_DECLARE( Entity )

	public:
		Entity();
		~Entity();

	protected:
		bool _activate() override;
		void _deactivate() override;
		bool _compile() override;
		void _release() override;
		void _update( float _timing ) override;

	protected:
		bool m_onUpdateEvent;
	};
}
