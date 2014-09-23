#	pragma once

#	include "Config/Typedef.h"

namespace Menge
{
	class Rect
	{
	public:
		Rect();
		Rect( uint32_t _left, uint32_t _top, uint32_t _right, uint32_t _bottom );

	public:
		inline uint32_t getWidth() const;
		inline uint32_t getHeight() const;

	public:
		uint32_t top;
		uint32_t bottom;
		uint32_t left;
		uint32_t right;
	};
	//////////////////////////////////////////////////////////////////////////
	inline uint32_t Rect::getWidth() const
	{
		return this->right - this->left;
	}
	//////////////////////////////////////////////////////////////////////////
	inline uint32_t Rect::getHeight() const
	{
		return this->bottom - this->top;
	}
}