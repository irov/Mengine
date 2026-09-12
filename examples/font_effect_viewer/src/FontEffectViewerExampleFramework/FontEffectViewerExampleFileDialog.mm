#include "FontEffectViewerExampleFileDialog.h"

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
        static void s_setupPanelDirectory( NSSavePanel * _panel, const Char * _currentPath )
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

            NSString * directory = path.stringByDeletingLastPathComponent;

            if( directory.length == 0 )
            {
                return;
            }

            _panel.directoryURL = [NSURL fileURLWithPath:directory isDirectory:YES];
        }
        //////////////////////////////////////////////////////////////////////////
        static bool s_copySelectedPanelPath( NSSavePanel * _panel, Char * const _path, size_t _capacity )
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
    bool FontEffectViewerExampleSelectFilePath( const Char * _title, const Char * _currentPath, Char * const _path, size_t _capacity )
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

            s_setupPanelDirectory( panel, _currentPath );

            if( [panel runModal] != NSModalResponseOK )
            {
                return false;
            }

            return s_copySelectedPanelPath( panel, _path, _capacity );
        }
    }
    //////////////////////////////////////////////////////////////////////////
    bool FontEffectViewerExampleSelectSaveFilePath( const Char * _title, const Char * _currentPath, const Char * _defaultName, Char * const _path, size_t _capacity )
    {
        @autoreleasepool
        {
            NSSavePanel * panel = [NSSavePanel savePanel];
            panel.title = s_makeNSString( _title, @"Save File" );
            panel.prompt = @"Save";
            panel.canCreateDirectories = YES;
            panel.nameFieldStringValue = s_makeNSString( _defaultName, @"Effects.json" );

            s_setupPanelDirectory( panel, _currentPath );

            if( [panel runModal] != NSModalResponseOK )
            {
                return false;
            }

            return s_copySelectedPanelPath( panel, _path, _capacity );
        }
    }
    //////////////////////////////////////////////////////////////////////////
}
