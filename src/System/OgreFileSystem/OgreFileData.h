#	pragma once

#	include "Interface/FileSystemInterface.h"

#	include "Ogre.h"

class OgreFileData
	: public FileDataInterface
{
public:
	OgreFileData( Ogre::DataStream * _data );
	~OgreFileData();

public:
	size_t read( void* _buffer, size_t _count ) override;
	bool eof() const override;

	size_t size() const override;

	void seek( size_t _pos ) override;
	size_t tell() const override;

private:
	Ogre::DataStream * m_data;
};