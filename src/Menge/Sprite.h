#	pragma once

#	include "../Interface/RenderSystemInterface.h"
#	include "Node.h"
#	include "ColourValue.h"
#	include "ValueInterpolator.h"

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
		: public Node
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
		virtual void setImageIndex( unsigned int _index );

		//! ���������� ������� ������ � ������ �����������
		/*!
		\return ������
		*/
		virtual unsigned int getImageIndex() const;

		//! ��������� �������.
		/*!
		\param _name ��� �������.
		*/
		virtual void setImageResource( const std::string & _name );

		virtual mt::vec2f getImageSize();
		virtual bool getCenterAlign();

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
		virtual const mt::vec2f& getScale() const;

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
		virtual void colorTo( const ColourValue & _color, float _time );

		void setAlpha( float _alpha );
		void alphaTo( float _alpha, float _time );
		//! ��������� ����� �������.
		/*!
		\param _color �������� �����
		*/
		void setColor( const ColourValue & _color );

		//! ���������� ���� �������.
		/*!
		\return ����
		*/

		const ColourValue & getColor() const;

	public:
		void loader( XmlElement * _xml ) override;

	protected:
		void _render( const Viewport & _viewport, bool _enableDebug ) override;

		bool _activate() override;
		void _deactivate() override;

		bool _compile() override;
		void _release() override;

		void _update( float _timing ) override;
		void _setListener() override;

	private:
		void updateAlign_();
		void updateVisibility_();
		void updateFlip_();
		void updateAlphaBlend_();
			private:
				void updateSprite_();

	protected:


		ResourceImage * m_resource;
		std::string m_resourcename;

		unsigned int m_currentImageIndex;

		bool m_centerAlign;
		bool m_flipX;
		bool m_flipY;
		//bool m_changingColor;

		//float m_changingColorTime;

		mt::vec2f m_scale;
		mt::vec2f m_alignOffset;
		mt::vec2f m_size;
		mt::vec2f m_offset;

		mt::vec4f m_percent;
		mt::vec4f m_uv;


		ColourValue m_color;
		ValueInterpolator<ColourValue> m_colorTo;
		//Color m_newColor;

		EBlendFactor m_blendSrc;
		EBlendFactor m_blendDest;
		//PyObject * m_listener;
		//virtual void _onSetListener();
	};
}
