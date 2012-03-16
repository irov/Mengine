#	pragma once

namespace Menge
{
	class Rect
	{
	public:
		Rect();
		Rect( int _left, int _top, int _right, int _bottom );

		int getWidth() const
		{
			return this->right - this->left;
		}
		
		int getHeight() const
		{
			return this->bottom - this->top;
		}

		int top;
		int bottom;
		int left;
		int right;
	};
}