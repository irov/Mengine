#	pragma once
#	include <vector>

class RectangleAreaPacker
{
public:
	RectangleAreaPacker();
	int getFilledWidth() const;
	int getFilledHeight() const;
public:
	void reset(int _width, int _height);
	bool insert(int _width, int _height, int & X, int & Y);
private:
	int m_currentLine;
	int m_lineHeight;
	int m_collumn;

	int m_packingAreaWidth;
	int m_packingAreaHeight;
};