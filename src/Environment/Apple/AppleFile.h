#pragma once

#import "Environment/Apple/AppleIncluder.h"

#include "Interface/FileGroupInterface.h"

@interface AppleFile : NSObject

+ (NSData * _Nullable)getNSDataFromFileGroup:(const Mengine::FileGroupInterfacePtr &)_fileGroup filePath:(const Mengine::FilePath &)_filePath error:(NSError * _Nullable * _Nullable)_error;

@end
