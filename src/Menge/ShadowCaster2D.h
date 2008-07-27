#	pragma once
#	include "math\vec2.h"
#	include "math\polygon.h"
#	include "Node.h"

namespace Menge
{
	class ShadowCaster2D
		: public Node
	{
		OBJECT_DECLARE(ShadowCaster2D)

	public:
	
		ShadowCaster2D();

	public:
		void loader( XmlElement * _xml ) override;

		void _render( const Viewport & _viewport, bool _enableDebug ) override;
	protected:

		bool _activate() override;
		void _deactivate() override;

		bool _compile() override;
		void _release() override;

		void _update( float _timing ) override;

	public:
		void addVertex(const mt::vec2f & _vertex);
		const mt::vec2f & getVertex(int i) const;
		mt::vec2f getEdge(int i) const;
		std::size_t size() const;
	private:
		mt::polygon m_poly;
	};
};