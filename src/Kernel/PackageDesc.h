#pragma once

#include "Kernel/ConstString.h"
#include "Kernel/VectorConstString.h"
#include "Kernel/FilePath.h"
#include "Kernel/Tags.h"

namespace Mengine
{
    struct PackageDesc
    {
        ConstString name;
        ConstString type;
        ConstString format;

        VectorConstString locales;
        Tags platform;
        Tags tags;

        ConstString fileGroupName;
        ConstString parent;
        FilePath descriptionPath;
        FilePath path;

        FilePath fontsPath;
        FilePath textsPath;

        bool dev;
        bool immediately;
        bool preload;
    };
}