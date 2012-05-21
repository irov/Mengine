#	pragma once

#	include "Entity.h"
#	include "InputEngine.h"

#	include "Core/Resolution.h"

#	include "Math/vec4.h"

#	include "Core/Polygon.h"

namespace Menge
{
	//! Arrow - курсор. ћожно повесить на курсор HotSpot, и пересечение будет провер€тс€ как хот спот.

    /*! xml - файл имеет следующую структуру:
	 *	<Node Name = "им€_ноды" Type = "TextField">
     *      <ClickOffset Value = "значение_смещени€_курсора"/>
	 *	</Node>
	*/

	class Arrow
		: public Entity
		, public CursorPositionProvider
	{
	public:
		Arrow();

	public:
		void hide( bool _value ) override;

	public:
		Scene * getScene() override;

	public:
		void setOffsetClick( const mt::vec2f & _offsetClick );
		const mt::vec2f & getOffsetClick() const;

		void setPolygon( const Polygon & _polygon );
		const Polygon & getPolygon() const;

		void setRadius( float _radius );
		float getRadius() const;

		void setContentResolution( const Resolution & _resolution );
		void setCurrentResolution( const Resolution & _resolution );
		
	public:
		const mt::mat3f & getClickMatrix();
		const mt::vec2f & getClickPosition();

	public:
		void updateClickMatrix_();
		void invalidateClickMatrix_();

	protected:
		void addPoint_( const mt::vec2f & _v );

	public:
		void onAppMouseLeave();
		void onAppMouseEnter();

	protected:
		bool _compile() override;

		bool _activate() override;
		void _deactivate() override;

	protected:
		void onCursorPositionChange( const mt::vec2f & _position ) override;

	protected:
		mt::vec2f m_offsetClick;

		bool m_invalidateClickMatrix;
		mt::mat3f m_clickMatrix;

		Resolution m_contentResolution;
		Resolution m_currentResolution;
		
		Polygon m_polygon;

		float m_radius;

		bool m_hided;
	};
}
