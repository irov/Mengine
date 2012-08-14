///*
//*	FileOutput.h
//*
//*	Created by _Berserk_ on 7.9.2009
//*	Copyright 2009 Menge. All rights reserved.
//*
//*/
//
//#	pragma once
//
//#	include "Interface/FileSystemInterface.h"
//
//namespace Menge
//{
//	class FileGroupInterface;
//
//	class FileOutput
//		: public FileOutputStreamInterface
//	{
//	public:
//		FileOutput();
//		virtual ~FileOutput();
//
//		void setFileSystem( FileGroupInterface* _fileSystem );
//		FileGroupInterface* getFileSystem();
//
//		bool open( const WString& _filename ) override;
//		void close();
//
//	protected:
//		FileGroupInterface* m_fileSystem;
//		bool m_opened;
//	};
//}	// namespace Menge
