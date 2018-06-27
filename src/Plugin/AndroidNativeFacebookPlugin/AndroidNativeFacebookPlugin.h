//
// Created by Alex Korzh on 6/26/18.
//
#pragma once

#include <stdio.h>
#include "Core/PluginBase.h"

#include "FacebookLoginCallback.h"
#include "FacebookShareCallback.h"
#include "FacebookUserCallback.h"

namespace Mengine {
    class AndroidNativeFacebookPlugin
            : public PluginBase {
    PLUGIN_DECLARE("AndroidNativeFacebook")

    public:
        AndroidNativeFacebookPlugin();

        ~AndroidNativeFacebookPlugin() override;

        static void
        performLogin(FacebookLoginCallback *facebookLoginCallback,
                     std::vector<char *> *readPermissions);

        static void getUser(FacebookUserCallback *facebookUserCallback);

        static void shareLink(char *link, FacebookShareCallback *facebookShareCallback);
    };
}
