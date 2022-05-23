package org.Mengine.Project;

import org.Mengine.Base.MenginePlugin;

import java.util.ArrayList;

public class FinalApplication extends org.Mengine.Base.MengineApplication {
    @Override
    public String[] getGradleAndroidPlugins() {
        return BuildConfig.MENGINE_GRADLE_ANDROID_PLUGINS;
    }
}