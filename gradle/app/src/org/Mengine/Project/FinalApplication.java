package org.Mengine.Project;

import org.Mengine.Base.MenginePlugin;

import java.util.ArrayList;

public class FinalApplication extends org.Mengine.Base.MengineApplication {

    public FinalApplication() {
        this.m_plugins = new ArrayList<MenginePlugin>();

        for (String n : BuildConfig.MENGINE_GRADLE_ANDROID_PLUGINS) {
            this.createPlugin(n);
        }
    }
}