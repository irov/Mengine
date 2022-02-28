package org.Mengine.Project;


import org.Mengine.Project.BuildConfig;
import org.Mengine.Base.MenginePlugin;

import java.util.ArrayList;

class FinalApplication extends org.Mengine.Build.MengineApplication {

    public FinalApplication() {
        this.plugins = new ArrayList<MenginePlugin>();

        for (String n : BuildConfig.MENGINE_GRADLE_ANDROID_PLUGINS) {
            this.createPlugin(n);
        }
    }

}