//#	pragma once
//
//#	include "Interface/FileSystemInterface.h"
//#	include "Interface/WindowsLayerInterface.h"
//
//#   include "Core/MemoryProxyInput.h"
//
//#   include "Factory/FactoryStore.h"
//
//namespace Menge
//{
//	class PosixFileMapped
//		: public FileMappedInterface
//	{
//	public:
//		PosixFileMapped();
//		~PosixFileMapped();
//
//    public:
//        void setServiceProvider( ServiceProviderInterface * _serviceProvider );
//
//	public:
//		bool open( const FilePath & _folder, const FilePath & _fileName ) override;
//
//	public:
//        InputStreamInterfacePtr createFileStream() override;
//        bool openFileStream( const InputStreamInterfacePtr & _stream, size_t _offset, size_t _size, void ** _memory ) override;
//        
//	private:
//        ServiceProviderInterface * m_serviceProvider;
//        
//		HANDLE m_hFile;
//		HANDLE m_hMapping;
//		LPVOID m_memory;
//
//		typedef FactoryPoolStore<MemoryProxyInput, 16> TFactoryMemoryProxyInput;
//		TFactoryMemoryProxyInput m_factoryMemoryProxyInput;
//	};
//}	// namespace Menge
