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
		virtual void setScale( float _scale );

		//! ���������� �����.
		/*!
		\return �����
		*/
		virtual float getScale() const;

		virtual void flip( bool _x );

		virtual void setPercentVisibility( const mt::vec2f & _percentX, const mt::vec2f & _percentY );

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
	
	private:
		void updateAlign_();
		void updateVisibility_();

	protected:

		ResourceImage * m_resource;
		std::string m_resourcename;

		size_t m_currentImageIndex;

		bool m_centerAlign;
		bool m_flipX;
		bool m_flipY;

		int	inverse_x;
		int	inverse_y;

		float m_scale;

		mt::vec2f m_alignOffset;
		mt::vec2f m_size;
		mt::vec2f m_offset;

		mt::vec4f m_percent;
		mt::vec4f m_uv;

		unsigned int m_color;
	};
}