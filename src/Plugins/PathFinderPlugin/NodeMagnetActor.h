#pragma once

#include "Kernel/Node.h"

#include "MagnetWorld.h"
#include "MagnetActor.h"

namespace Mengine
{
	class NodeMagnetActor
		: public Node
		, public MagnetActorProviderInterface
	{
	public:
		NodeMagnetActor();
		~NodeMagnetActor() override;

	public:
		void setMagnetRadius( float _collisionRadius );
		float getMagnetRadius() const;

		void setMagnetForce( float _magnetForce );
		float getMagnetForce() const;

		void setMagnetActive( bool _magnetActive );
		bool isMagnetActive() const;

	public:
		void setMagnetIFF( uint32_t _collisionIFF );
		uint32_t getMagnetIFF() const;

	public:
		void setMagnetNodeAffect( Node * _magnetNode );

	public:
		void addMagnetException( NodeMagnetActor * _actor );

	public:
		void setMagnetWorld( const MagnetWorldPtr & _collision );
		const MagnetWorldPtr & getMagnetWorld() const;

	protected:
		void onMagnetPositionProvider( mt::vec3f & _position ) const override;
		void onMagnetAffect( const mt::vec3f & _velocity ) override;

	protected:
		bool _activate() override;
		void _deactivate() override;

	protected:
		void _debugRender( Mengine::RenderServiceInterface * _renderService, const RenderContext * _state ) override;

	protected:
		MagnetWorldPtr m_magnetWorld;

		MagnetActorPtr m_actor;

		Node * m_magnetNode;

		typedef Vector<MagnetActorPtr> TVectorActorException;
		TVectorActorException m_exceptions;

		float m_magnetRadius;
		float m_magnetForce;
		bool m_magnetActive;
		uint32_t m_iff;
	};
}