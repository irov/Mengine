#	pragma once

#	include "Config/Typedef.h"

namespace Menge
{
	class Rect
	{
	public:
		Rect();
		Rect( size_t _left, size_t _top, size_t _right, size_t _bottom );

	public:
		inline size_t getWidth() const;
		inline size_t getHeight() const;

	public:
		size_t top;
		size_t bottom;
		size_t left;
		size_t right;
	};
	//////////////////////////////////////////////////////////////////////////
	inline size_t Rect::getWidth() const
	{
		return this->right - this->left;
	}
	//////////////////////////////////////////////////////////////////////////
	inline size_t Rect::getHeight() const
	{
		return this->bottom - this->top;
	}
}