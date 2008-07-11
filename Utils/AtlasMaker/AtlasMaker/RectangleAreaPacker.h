#	pragma once
#	include <vector>

class RectangleAreaPacker
{
public:
	RectangleAreaPacker();
	int getMaxWidth() const;
	int getMaxHeight() const;
public:
	void reset(int _width, int _height);
	bool insert(int _width, int _height, int & X, int & Y);
private:
	int m_maxWidth;
	int m_maxHeight;
	int m_currentLine;
	int m_lineHeight;
	int m_collumn;

	int m_packingAreaWidth;
	int m_packingAreaHeight;
};