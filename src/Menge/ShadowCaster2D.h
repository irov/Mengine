#	pragma once
#	include "math\vec2.h"
#	include "math\polygon.h"
#	include "SceneNode2D.h"

namespace Menge
{
	class ShadowCaster2D
		: public SceneNode2D
	{
		OBJECT_DECLARE(ShadowCaster2D)

	public:
	
		ShadowCaster2D();

	public:
		virtual bool isVisible( const Viewport & _viewPort );

		void loader( XmlElement * _xml ) override;

	protected:
		void _render( bool _enableDebug ) override;

		bool _activate() override;
		void _deactivate() override;

		bool _compile() override;
		void _release() override;

		void _update( float _timing ) override;

	public:
		void addVertex(const mt::vec2f & _vertex);
		const mt::vec2f & getVertex(int i) const;
		mt::vec2f getEdge(int i) const;
		size_t size() const;
	private:
		mt::polygon m_poly;
	};
};