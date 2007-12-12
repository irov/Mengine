#	pragma once

#	include "../Interface/RenderSystemInterface.h"
#	include "SceneNode2D.h"
#	include "Color.h"

#	include "math/mat3.h"
#	include "math/vec4.h"

#	include <vector>

namespace Menge
{
	class ResourceImage;

	//! Sprite - ������. � �������� ������� ��������� ResourceImage, ������� ����� ��������� (� ����������� �� ���� �������, �������� ResourceImageDefault ��� ResourceImageSet) N �����������. ��� ���������� ��� ����, ��� �� ��� ���� �������� �������� ����� ���� ��������� ��������� ����������� � ��������. ������� Animation ����������� �� Sprite � ����� Sprite::getImageIndex �������� ������� ������ �����������.

	/*! xml - ���� ����� ��������� ���������:
	*	<Node Name = "���_����" Type = "Sprite">
	*		<ImageMap Name = "���_�������" />
	*		<ImageIndex Value = "������_��������" />
	*		<CenterAlign Value = "1/0" />
	*		<Scale Value = "value" />
	*	</Node>
	*/

	class Sprite
		: public SceneNode2D
	{
		OBJECT_DECLARE(Sprite);
	public:
		//! �����������.
		/*!
		*/
		Sprite();

		//! ����������.
		/*!
		*/
		~Sprite();

	public:
		//! ��������� ������� � ������ �����������.
		/*!
		\param _index ������.
		*/
		virtual void setImageIndex( size_t _index );

		//! ���������� ������� ������ � ������ �����������
		/*!
		\return ������
		*/
		virtual size_t getImageIndex() const;

		//! ��������� �������.
		/*!
		\param _name ��� �������.
		*/
		virtual void setImageResource( const std::string & _name );

		//! ���������� ��� �������
		/*!
		\return ��� �������
		*/
		virtual const std::string & getImageResource() const;

		//! ��������� ������ �� ����.
		/*!
		\param _scale �����.
		*/
		virtual void setScale( const mt::vec2f& _scale );

		//! ���������� �����.
		/*!
		\return �����
		*/
		virtual mt::vec2f getScale() const;

		//! �������������� ������� �� ��� x, ���� _x = true, ����� �� ��� y.
		/*!
		\param _x ���.
		*/
		virtual void flip( bool _x );

		//! ��������� �������� ��������� ������� �� �����. 0.0 - ��� �����. 1.0 - �� �����.
		/*!
		\param _percentX - ����������� ��������� �� x, y �����.
		\param _percentY - ����������� ��������� �� ������, ������ ������.
		*/
		virtual void setPercentVisibility( const mt::vec2f & _percentX, const mt::vec2f & _percentY );

		//! ��������� ������� _time, � ������� �������� ����� ��������� ���� ������� � ����� _color.
		/*!
		\param _color - �������������� ����
		\param _time - �����, � ������� �������� ����� ��������� ����
		*/
		virtual void colorTo( const Color & _color, float _time );

		//! ��������� ����� �������.
		/*!
		\param _color �������� �����
		*/
		void setColor( const Color & _color );

		//! ���������� ���� �������.
		/*!
		\return ����
		*/

		const Color & getColor() const;

	public:
		virtual bool isVisible( const Viewport & _viewPort );

		void loader( TiXmlElement * _xml ) override;

	protected:
		void _render() override;
		void _debugRender() override;

		bool _activate() override;
		void _deactivate() override;

		bool _compile() override;
		void _release() override;

		void _update( float _timing ) override;
	
	private:
		void updateAlign_();
		void updateVisibility_();
		void updateFlip_();

	protected:

		ResourceImage * m_resource;
		std::string m_resourcename;

		size_t m_currentImageIndex;

		bool m_centerAlign;
		bool m_flipX;
		bool m_flipY;
		bool m_changingColor;

		float m_changingColorTime;

		mt::vec2f m_scale;
		mt::vec2f m_alignOffset;
		mt::vec2f m_size;
		mt::vec2f m_offset;

		mt::vec4f m_percent;
		mt::vec4f m_uv;


		Color m_color;
		Color m_newColor;

		EBlendFactor m_blendSrc;
		EBlendFactor m_blendDest;
	};
}
