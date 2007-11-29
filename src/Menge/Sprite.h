#	pragma once

#	include "SceneNode2D.h"

#	include "math/mat3.h"
#	include "math/vec4.h"

#	include <vector>

class RenderImageInterface;

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

		//! ��������� ������.
		/*!
		\param _scale �����.
		*/
		//virtual void setScale( float _scale );

		virtual void setScale( mt::vec2f _scale );

		//! ���������� �����.
		/*!
		\return �����
		*/
		virtual mt::vec2f getScale() const;

		virtual void flip( bool _x );

		virtual void setPercentVisibility( const mt::vec2f & _percentX, const mt::vec2f & _percentY );

		void setColorTime( unsigned int _color, float _time )	{ m_newColor = _color; m_changingColorTime = _time; }

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
	
		bool	m_flipX;
		bool	m_flipY;

		mt::vec2f m_scale;
		mt::vec2f m_alignOffset;
		mt::vec2f m_size;
		mt::vec2f m_offset;

		mt::vec4f m_percent;
		mt::vec4f m_uv;

		unsigned int m_color;
		float m_changingColorTime;
		unsigned int m_newColor;
		//unsigned int m_oldColor;
		bool m_changingColor;
	};
}
