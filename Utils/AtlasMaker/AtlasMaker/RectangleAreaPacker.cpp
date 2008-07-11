#	include "RectangleAreaPacker.h"
#	include <algorithm>
//////////////////////////////////////////////////////////////////////////
RectangleAreaPacker::RectangleAreaPacker()
: m_packingAreaWidth(0)
, m_packingAreaHeight(0)
, m_collumn(0)
, m_currentLine(0)
, m_lineHeight(0)
, m_maxWidth(0)
, m_maxHeight(0)
{
}
//////////////////////////////////////////////////////////////////////////
int RectangleAreaPacker::getMaxWidth() const
{
	return m_maxWidth;
}
//////////////////////////////////////////////////////////////////////////
int RectangleAreaPacker::getMaxHeight() const
{
	return m_maxHeight;
}
//////////////////////////////////////////////////////////////////////////
bool RectangleAreaPacker::insert(int _width, int _height, int & X, int & Y)
{
	if( (_width > m_packingAreaWidth) || (_height > m_packingAreaHeight))
	{ 
		return false; 
	} 
	  
	if(m_collumn + _width > m_packingAreaWidth) 
	{ 
		m_currentLine += m_lineHeight; 
		m_lineHeight = 0; 
		m_collumn = 0; 
	} 

	if(m_currentLine + _height > m_packingAreaHeight) 
	{ 
		return false; 
	} 

	X = m_collumn;
	Y = m_currentLine;
	
	m_collumn += _width; 

	if(_height > m_lineHeight) 
	{
		m_lineHeight = _height;
	}

	m_maxWidth = std::max(m_maxWidth, X + _width);
	m_maxHeight = std::max(m_maxHeight, Y + _height);

	return true; 
}
//////////////////////////////////////////////////////////////////////////
void RectangleAreaPacker::reset(int _width, int _height)
{
	m_collumn = 0;
	m_currentLine = 0;
	m_lineHeight = 0;

	m_packingAreaWidth = _width;
	m_packingAreaHeight = _height;
}
//////////////////////////////////////////////////////////////////////////