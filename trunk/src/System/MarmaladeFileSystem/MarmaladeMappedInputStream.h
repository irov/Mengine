//#	pragma once
//
//#	include "Interface/FileSystemInterface.h"
//
//namespace Menge
//{
//	class MarmaladeMappedInputStream
//		: public MappedFileInputStreamInterface
//	{
//	public:
//		MarmaladeMappedInputStream( ServiceProviderInterface * _serviceProvider );
//		~MarmaladeMappedInputStream();
//
//    public:
//        InputStreamInterface * createInputMemory() override;
//        void openInputMemory( InputStreamInterface * _stream, const FilePath & _filename, size_t _offset, size_t _size ) override;
//
//	public:
//		bool open( const FilePath& _filename ) override;		
//
//	public:
//		int read( void* _buf, int _count ) override;
//		void seek( int _pos ) override;
//		int tell() const override;
//		int size() const override;		
//
//    public:
//        bool time( time_t & _time ) const override;
//
//    public:
//        void destroy() override;
//
//	private:
//        ServiceProviderInterface * m_serviceProvider;
//        
//		s3eFile * m_hFile;
//		HANDLE m_hMapping;
//
//        void * m_memory;
//
//        unsigned char* m_data;
//        unsigned char* m_pos;
//        unsigned char* m_end;
//        int m_size;		
//	};
//}	// namespace Menge
