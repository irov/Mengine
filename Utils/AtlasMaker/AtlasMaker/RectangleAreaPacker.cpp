#	include "RectangleAreaPacker.h"
#	include <algorithm>

RectangleAreaPacker::RectangleAreaPacker()
: m_packingAreaWidth(0)
, m_packingAreaHeight(0)
, m_collumn(0)
, m_currentLine(0)
, m_lineHeight(0)
{
	reset(2048,2048);
}

int RectangleAreaPacker::getFilledWidth() const
{
	return m_collumn;
}

int RectangleAreaPacker::getFilledHeight() const
{
	return m_lineHeight;
}

bool RectangleAreaPacker::insert(int _width, int _height, int & X, int & Y)
{
	if( (_width > m_packingAreaWidth) || (_height > m_packingAreaHeight))
	{ 
		X = 0;
		Y = 0;
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
		X = 0;
		Y = 0;
		return false; 
	} 

	X = m_collumn;
	Y = m_currentLine;
	
	m_collumn += _width; 

	if(_height > m_lineHeight) 
	{
		m_lineHeight = _height;
	}
	
	return true; 
}

void RectangleAreaPacker::reset(int _width, int _height)
{
	m_collumn = 0;
	m_currentLine = 0;
	m_lineHeight = 0;

	m_packingAreaWidth = _width;
	m_packingAreaHeight = _height;
}