#include "FigmaViewerExampleFileDialog.h"

#include "Config/StdString.h"

#import <AppKit/AppKit.h>

namespace Mengine
{
    namespace
    {
        //////////////////////////////////////////////////////////////////////////
        static NSString * s_makeNSString( const Char * _value, NSString * _fallback )
        {
            if( _value == nullptr || _value[0] == '\0' )
            {
                return _fallback;
            }

            NSString * string = [NSString stringWithUTF8String:_value];

            if( string == nil )
            {
                return _fallback;
            }

            return string;
        }
        //////////////////////////////////////////////////////////////////////////
        static void s_setupPanelDirectory( NSOpenPanel * _panel, const Char * _currentPath, BOOL _directory )
        {
            if( _currentPath == nullptr || _currentPath[0] == '\0' )
            {
                return;
            }

            NSString * path = [NSString stringWithUTF8String:_currentPath];

            if( path == nil || path.length == 0 )
            {
                return;
            }

            NSString * directory = _directory == YES
                ? path
                : path.stringByDeletingLastPathComponent;

            if( directory.length == 0 )
            {
                return;
            }

            _panel.directoryURL = [NSURL fileURLWithPath:directory isDirectory:YES];
        }
        //////////////////////////////////////////////////////////////////////////
        static bool s_copySelectedPanelPath( NSOpenPanel * _panel, Char * const _path, size_t _capacity )
        {
            NSURL * url = _panel.URL;

            if( url == nil )
            {
                return false;
            }

            const Char * path = url.path.fileSystemRepresentation;

            if( path == nullptr )
            {
                return false;
            }

            StdString::strcpy_safe( _path, path, _capacity );

            return true;
        }
        //////////////////////////////////////////////////////////////////////////
    }
    //////////////////////////////////////////////////////////////////////////
    bool FigmaViewerExampleSelectFilePath( const Char * _title, const Char * _currentPath, Char * const _path, size_t _capacity )
    {
        @autoreleasepool
        {
            NSOpenPanel * panel = [NSOpenPanel openPanel];
            panel.title = s_makeNSString( _title, @"Open File" );
            panel.prompt = @"Open";
            panel.canChooseFiles = YES;
            panel.canChooseDirectories = NO;
            panel.allowsMultipleSelection = NO;
            panel.treatsFilePackagesAsDirectories = NO;

            s_setupPanelDirectory( panel, _currentPath, NO );

            if( [panel runModal] != NSModalResponseOK )
            {
                return false;
            }

            return s_copySelectedPanelPath( panel, _path, _capacity );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool FigmaViewerExampleSelectFolderPath( const Char * _title, const Char * _currentPath, Char * const _path, size_t _capacity )
    {
        @autoreleasepool
        {
            NSOpenPanel * panel = [NSOpenPanel openPanel];
            panel.title = s_makeNSString( _title, @"Choose Folder" );
            panel.prompt = @"Use Folder";
            panel.canChooseFiles = NO;
            panel.canChooseDirectories = YES;
            panel.allowsMultipleSelection = NO;
            panel.treatsFilePackagesAsDirectories = YES;

            s_setupPanelDirectory( panel, _currentPath, YES );

            if( [panel runModal] != NSModalResponseOK )
            {
                return false;
            }

            return s_copySelectedPanelPath( panel, _path, _capacity );
        }
    }
    //////////////////////////////////////////////////////////////////////////
}
