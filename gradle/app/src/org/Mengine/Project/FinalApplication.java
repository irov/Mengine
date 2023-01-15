package org.Mengine.Project;

public class FinalApplication extends org.Mengine.Base.MengineApplication {
    @Override
    public String[] getGradleAndroidPlugins() {
        return BuildConfig.MENGINE_GRADLE_ANDROID_PLUGINS;
    }
}