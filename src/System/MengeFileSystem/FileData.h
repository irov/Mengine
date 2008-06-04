#	pragma once

#	include "Interface/FileSystemInterface.h"

#define MENGE_STREAM_TEMP_SIZE 128

class DataStream : public DataStreamInterface
{
public:
	/// Constructor for creating unnamed streams
	DataStream();

	/// Constructor for creating named streams
	DataStream( const Menge::String& _name );

	/// Returns the name of the stream, if it has one.
	const Menge::String& getName() const;

	virtual ~DataStream() {}

	// Streaming operators
	template<typename T> DataStream& operator>>( T& _val );

	//Read the requisite number of bytes from the stream, 
	//stopping at the end of the file.
	//@param buf Reference to a buffer pointer
	//@param count Number of bytes to read
	//@returns The number of bytes read
	virtual std::size_t read( void* _buf, std::size_t _count ) = 0;

	//Get a single line from the stream.
	//@remarks
	//The delimiter character is not included in the data
	//returned, and it is skipped over so the next read will occur
	//after it. The buffer contents will include a
	//terminating character.
	//@note
	//If you used this function, you <b>must</b> open the stream in <b>binary mode</b>,
	//otherwise, it'll produce unexpected results.
	//@param buf Reference to a buffer pointer
	//@param maxCount The maximum length of data to be read, excluding the terminating character
	//@param delim The delimiter to stop at
	//@returns The number of bytes read, excluding the terminating character
	virtual std::size_t readLine( char* _buf, std::size_t _maxCount, const Menge::String& _delim = "\n" );

	//Returns a String containing the next line of data, optionally 
	//trimmed for whitespace. 
	//@remarks
	//This is a convenience method for text streams only, allowing you to 
	//retrieve a String object containing the next line of data. The data
	//is read up to the next newline character and the result trimmed if
	//required.
	//@note
	//If you used this function, you <b>must</b> open the stream in <b>binary mode</b>,
	//otherwise, it'll produce unexpected results.
	//@param 
	//trimAfter If true, the line is trimmed for whitespace (as in 
	//String.trim(true,true))
	virtual Menge::String getLine( bool _trimAfter = true );

	//Returns a String containing the entire stream. 
	//@remarks
	//This is a convenience method for text streams only, allowing you to 
	//retrieve a String object containing all the data in the stream.
	virtual Menge::String getAsString( void );

	//Skip a single line from the stream.
	//@note
	//If you used this function, you <b>must</b> open the stream in <b>binary mode</b>,
	//otherwise, it'll produce unexpected results.
	//@param delim The delimiter(s) to stop at
	//@returns The number of bytes skipped
	virtual std::size_t skipLine( const Menge::String& _delim = "\n" );

	//Skip a defined number of bytes. This can also be a negative value, in which case
	//the file pointer rewinds a defined number of bytes.
	virtual void skip( long _count ) = 0;

	// Repositions the read point to a specified byte.
	virtual void seek( std::size_t _pos ) = 0;

	// Returns the current byte offset from beginning
	virtual std::size_t tell() const = 0;

	// Returns true if the stream has reached the end.
	virtual bool eof() const = 0;

	//Returns the total size of the data to be read from the stream, 
	//or 0 if this is indeterminate for this stream. 
	std::size_t size() const;

	// Close the stream; this makes further operations invalid.
	virtual void close( void ) = 0;

	// 
	virtual void* getBuffer() = 0;

	//
	virtual bool isMemory() const = 0;

	//
	virtual void setFreeOnClose( bool _free ) = 0;

protected:
	/// The name (e.g. resource name) that can be used to identify the source fot his data (optional)
	Menge::String m_name;		
	/// Size of the data in the stream (may be 0 if size cannot be determined)
	std::size_t m_size;

};

// Common subclass of DataStream for handling data from chunks of memory.
class MemoryDataStream : public DataStream
{
protected:
	/// Pointer to the start of the data area
	unsigned char* m_data;
	/// Pointer to the current position in the memory
	unsigned char* m_pos;
	/// Pointer to the end of the memory
	unsigned char* m_end;
	/// Do we delete the memory on close
	bool m_freeOnClose;			
public:

	//Wrap an existing memory chunk in a stream.
	//@param pMem Pointer to the exising memory
	//@param size The size of the memory chunk in bytes
	//@param freeOnClose If true, the memory associated will be destroyed
	//when the stream is destroyed.
	MemoryDataStream( void* _pMem, std::size_t _size, bool _freeOnClose = false );

	//Wrap an existing memory chunk in a named stream.
	//@param name The name to give the stream
	//@param pMem Pointer to the exising memory
	//@param size The size of the memory chunk in bytes
	//@param freeOnClose If true, the memory associated will be destroyed
	//when the stream is destroyed.
	MemoryDataStream( const Menge::String& _name, void* _pMem, std::size_t _size, 
		bool _freeOnClose = false);

	//Create a stream which pre-buffers the contents of another stream.
	//@remarks
	//This constructor can be used to intentionally read in the entire
	//contents of another stream, copying them to the internal buffer
	//and thus making them available in memory as a single unit.
	//@param sourceStream Another DataStream which will provide the source
	//of data
	//@param freeOnClose If true, the memory associated will be destroyed
	//when the stream is destroyed.
	MemoryDataStream( DataStream& _sourceStream, bool _freeOnClose = true);

	//Create a stream which pre-buffers the contents of another stream.
	//@remarks
	//This constructor can be used to intentionally read in the entire
	//contents of another stream, copying them to the internal buffer
	//and thus making them available in memory as a single unit.
	//@param sourceStream Weak reference to another DataStream which will provide the source
	//of data
	//@param freeOnClose If true, the memory associated will be destroyed
	//when the stream is destroyed.
	MemoryDataStream( DataStream* _sourceStream, bool _freeOnClose = true );

	//Create a named stream which pre-buffers the contents of 
	//another stream.
	//@remarks
	//This constructor can be used to intentionally read in the entire
	//contents of another stream, copying them to the internal buffer
	//and thus making them available in memory as a single unit.
	//@param name The name to give the stream
	//@param sourceStream Another DataStream which will provide the source
	//of data
	//@param freeOnClose If true, the memory associated will be destroyed
	//when the stream is destroyed.
	MemoryDataStream( const Menge::String& _name, DataStream& _sourceStream, bool _freeOnClose = true );

	//Create a named stream which pre-buffers the contents of 
	//another stream.
	//@remarks
	//This constructor can be used to intentionally read in the entire
	//contents of another stream, copying them to the internal buffer
	//and thus making them available in memory as a single unit.
	//@param name The name to give the stream
	//@param sourceStream Another DataStream which will provide the source
	//of data
	//@param freeOnClose If true, the memory associated will be destroyed
	//when the stream is destroyed.
	MemoryDataStream( const Menge::String& _name, DataStream* _sourceStream, bool _freeOnClose = true);

	//Create a stream with a brand new empty memory chunk.
	//@param size The size of the memory chunk to create in bytes
	//@param freeOnClose If true, the memory associated will be destroyed
	//when the stream is destroyed.
	MemoryDataStream( std::size_t _size, bool _freeOnClose = true );

	//Create a named stream with a brand new empty memory chunk.
	//@param name The name to give the stream
	//@param size The size of the memory chunk to create in bytes
	//@param freeOnClose If true, the memory associated will be destroyed
	//when the stream is destroyed.
	
	MemoryDataStream( const Menge::String& _name, std::size_t _size, bool _freeOnClose = true);

	~MemoryDataStream();

	// Get a pointer to the start of the memory block this stream holds.
	unsigned char* getPtr();

	// Get a pointer to the current position in the memory block this stream holds.
	unsigned char* getCurrentPtr();

	// @copydoc DataStream::read
	std::size_t read( void* _buf, std::size_t _count );

	// @copydoc DataStream::readLine
	std::size_t readLine( char* _buf, std::size_t _maxCount, const Menge::String& _delim = "\n" );

	// @copydoc DataStream::skipLine
	std::size_t skipLine( const Menge::String& _delim = "\n" );

	// @copydoc DataStream::skip
	void skip( long _count );

	// @copydoc DataStream::seek
	void seek( std::size_t _pos );

	// @copydoc DataStream::tell
	std::size_t tell() const;

	// @copydoc DataStream::eof
	bool eof() const;

	// @copydoc DataStream::close
	void close();

	// Sets whether or not to free the encapsulated memory on close.
	void setFreeOnClose( bool _free );

	// 
	void* getBuffer();

	//
	bool isMemory() const;

};

class FileStreamDataStream : public DataStream
{
public:
	//Construct stream from an STL stream
	//@param s Pointer to source stream
	//@param freeOnClose Whether to delete the underlying stream on 
	//destruction of this class
	FileStreamDataStream( std::ifstream* _s, bool _freeOnClose = true );

	//Construct named stream from an STL stream
	//@param name The name to give this stream
	//@param s Pointer to source stream
	//@param freeOnClose Whether to delete the underlying stream on 
	//destruction of this class
	FileStreamDataStream( const Menge::String& _name, std::ifstream* _s, bool _freeOnClose = true );

	//Construct named stream from an STL stream, and tell it the size
	//@remarks
	//This variant tells the class the size of the stream too, which 
	//means this class does not need to seek to the end of the stream 
	//to determine the size up-front. This can be beneficial if you have
	//metadata about the contents of the stream already.
	//@param name The name to give this stream
	//@param s Pointer to source stream
	//@param size Size of the stream contents in bytes
	//@param freeOnClose Whether to delete the underlying stream on 
	//destruction of this class
	FileStreamDataStream( const Menge::String& _name, std::ifstream* _s, std::size_t _size, 
		bool _freeOnClose = true );

	~FileStreamDataStream();

	// @copydoc DataStream::read
	std::size_t read( void* _buf, std::size_t _count );

	// @copydoc DataStream::readLine
	std::size_t readLine( char* _buf, std::size_t _maxCount, const Menge::String& _delim = "\n");

	// @copydoc DataStream::skip
	void skip( long _count );

	// @copydoc DataStream::seek
	void seek( std::size_t _pos );

	// @copydoc DataStream::tell
	std::size_t tell() const;

	// @copydoc DataStream::eof
	bool eof() const;

	// @copydoc DataStream::close
	void close();

	// 
	virtual void* getBuffer();

	//
	virtual bool isMemory() const;

	//
	void setFreeOnClose( bool _free );

protected:
	/// Reference to source stream
	std::ifstream* m_stream;
	bool m_freeOnClose;
	bool m_freeBuffer;
	unsigned char* m_buffer;
};

class FileHandleDataStream : public DataStream
{
protected:
	FILE* m_fileHandle;
public:
	/// Create stream from a C file handle
	FileHandleDataStream( FILE* _handle );
	/// Create named stream from a C file handle
	FileHandleDataStream( const Menge::String& _name, FILE* _handle);
	~FileHandleDataStream();

	// @copydoc DataStream::read
	std::size_t read( void* _buf, std::size_t _count );

	// @copydoc DataStream::skip
	void skip( long _count );

	// @copydoc DataStream::seek
	void seek( std::size_t _pos );

	// @copydoc DataStream::tell
	std::size_t tell() const;

	// @copydoc DataStream::eof
	bool eof() const;

	// @copydoc DataStream::close
	void close(void);

	// 
	void* getBuffer();

	//
	bool isMemory() const;

	//
	void setFreeOnClose( bool _free );
};