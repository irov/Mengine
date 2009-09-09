#	pragma once
#	include "math/vec2.h"
#	include "math/polygon.h"
#	include "Node.h"

namespace Menge
{
	class ShadowCaster2D
		: public Node
	{
		FACTORABLE_DECLARE(ShadowCaster2D)

	public:
	
		ShadowCaster2D();

	public:
		void loader( XmlElement * _xml ) override;

		void _render( unsigned int _debugMask ) override;
	protected:

		bool _activate() override;
		void _deactivate() override;

		bool _compile() override;
		void _release() override;

		void _update( float _timing ) override;

	public:
		void addVertex(const mt::vec2f & _vertex);
		const mt::vec2f & getVertex( std::size_t i ) const;
		mt::vec2f getEdge( std::size_t i ) const;
		std::size_t size() const;
	private:
		mt::polygon m_poly;
	};
};
