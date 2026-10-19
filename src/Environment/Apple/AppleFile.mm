#import "AppleFile.h"

#include "Kernel/FileStreamHelper.h"

@implementation AppleFile

+ (NSData *)getNSDataFromFileGroup:(const Mengine::FileGroupInterfacePtr &)_fileGroup filePath:(const Mengine::FilePath &)_filePath error:(NSError **)_error
{
    Mengine::InputStreamInterfacePtr stream = Mengine::Helper::openInputStreamFile( _fileGroup, _filePath, false, false, nullptr );

    if( stream == nullptr )
    {
        if( _error != nullptr )
        {
            *_error = [NSError errorWithDomain:NSURLErrorDomain code:NSURLErrorCannotOpenFile userInfo:nil];
        }

        return nil;
    }

    size_t size = stream->size();
    NSMutableData * data = [NSMutableData dataWithLength:size];
    void * buffer = data.mutableBytes;
    size_t read = stream->read( buffer, size );
    Mengine::Helper::closeInputStreamFile( _fileGroup, stream );

    if( read != size )
    {
        if( _error != nullptr )
        {
            *_error = [NSError errorWithDomain:NSURLErrorDomain code:NSURLErrorCannotDecodeContentData userInfo:nil];
        }

        return nil;
    }

    return data;
}

@end
