//
//  MengineOpenGLView.h
//  Mengine
//
//  Created by Dgut on 26.10.11.
//  Copyright (c) 2011 __MyCompanyName__. All rights reserved.
//

#import <AppKit/AppKit.h>

@interface MengineOpenGLView : NSOpenGLView
{
    NSCursor * cursor;
}

@property(nonatomic, retain) NSCursor * cursor;

@end

extern MengineOpenGLView * pMengineOpenGLView;