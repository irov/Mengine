#import "MacOSWebViewApplicationDelegate.h"

#include "Config/Config.h"

//////////////////////////////////////////////////////////////////////////
int main( int _argc, char ** _argv )
{
    MENGINE_UNUSED( _argc );
    MENGINE_UNUSED( _argv );

    @autoreleasepool
    {
        [NSApplication sharedApplication];

        BOOL hidden = [NSProcessInfo.processInfo.arguments containsObject:@"--windowhidden"];
        NSApplicationActivationPolicy activationPolicy = hidden == YES ? NSApplicationActivationPolicyAccessory : NSApplicationActivationPolicyRegular;
        [NSApp setActivationPolicy:activationPolicy];

        NSString * delegateName = [NSBundle.mainBundle objectForInfoDictionaryKey:@"MengineApplicationDelegate"];
        Class delegateClass = NSClassFromString( delegateName );
        MacOSWebViewApplicationDelegate * delegate = [[delegateClass alloc] init];

        if( delegate == nil )
        {
            return EXIT_FAILURE;
        }

        NSApp.delegate = delegate;

        NSMenu * menu = [[NSMenu alloc] init];

        NSMenuItem * appItem = [[NSMenuItem alloc] init];
        NSMenu * appMenu = [[NSMenu alloc] init];
        [appMenu addItemWithTitle:@"Quit" action:@selector(terminate:) keyEquivalent:@"q"];
        appItem.submenu = appMenu;
        [menu addItem:appItem];

        NSMenuItem * editItem = [[NSMenuItem alloc] init];
        NSMenu * editMenu = [[NSMenu alloc] initWithTitle:@"Edit"];
        [editMenu addItemWithTitle:@"Undo" action:@selector(undo:) keyEquivalent:@"z"];
        [editMenu addItemWithTitle:@"Cut" action:@selector(cut:) keyEquivalent:@"x"];
        [editMenu addItemWithTitle:@"Copy" action:@selector(copy:) keyEquivalent:@"c"];
        [editMenu addItemWithTitle:@"Paste" action:@selector(paste:) keyEquivalent:@"v"];
        [editMenu addItemWithTitle:@"Select All" action:@selector(selectAll:) keyEquivalent:@"a"];
        editItem.submenu = editMenu;
        [menu addItem:editItem];

        NSApp.mainMenu = menu;

        [NSApp run];

        [delegate finalizeApplication];

        return EXIT_SUCCESS;
    }
}
//////////////////////////////////////////////////////////////////////////
