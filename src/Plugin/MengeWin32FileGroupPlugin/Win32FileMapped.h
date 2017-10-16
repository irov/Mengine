#	pragma once

#	include "Interface/FileSystemInterface.h"
#	include "Interface/WindowsLayerInterface.h"

#   include "Core/ServantBase.h"

namespace Menge
{
	class Win32FileMapped
		: public ServantBase<FileMappedInterface>
	{
	public:
		Win32FileMapped();
		~Win32FileMapped();

	public:
		bool open( const FilePath & _relationPath, const FilePath & _folderPath, const FilePath & _filePath ) override;

	public:
        InputStreamInterfacePtr createFileStream() override;
        bool openFileStream( const InputStreamInterfacePtr & _stream, size_t _offset, size_t _size, void ** _memory ) override;
        
	private:
		HANDLE m_hFile;
		HANDLE m_hMapping;
		LPVOID m_memory;
	};
}	// namespace Menge
