#	pragma once

#	include "Entity.h"
#	include "Core/Resolution.h"

#	include "Math/vec4.h"
#	include "Math/polygon.h"

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

		void setPolygon( const mt::polygon & _polygon );
		const mt::polygon & getPolygon() const;

		void setContentResolution( const Resolution & _resolution );
		void setCurrentResolution( const Resolution & _resolution );

		void setCursorMode( bool _mode );

	public:
		const mt::mat3f & getClickMatrix();
		const mt::vec2f & getClickPosition();

	public:
		void updateClickMatrix_();
		void invalidateClickMatrix_();

	public:
		void loader( BinParser * _parser ) override;

	protected:
		void addPoint_( const mt::vec2f & _v );

	public:
		void onAppMouseLeave();
		void onAppMouseEnter();

	protected:
		void _update( float _timing ) override;
		bool _compile() override;

	protected:
		mt::vec2f m_offsetClick;

		bool m_invalidateClickMatrix;
		mt::mat3f m_clickMatrix;

		Resolution m_contentResolution;
		Resolution m_currentResolution;
		mt::polygon m_polygon;
		bool m_hided;
	};
}
