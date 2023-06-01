package org.Mengine.Project;

public class FinalApplication extends org.Mengine.Base.MengineApplication {
    @Override
    public String[] getGradleAndroidPlugins() {
        return BuildConfig.MENGINE_GRADLE_ANDROID_PLUGINS;
    }

    @Override
    public String getApplicationId() {
        return BuildConfig.APPLICATION_ID;
    }

    @Override
    public boolean isBuildPublish() {
        return BuildConfig.MENGINE_APP_BUILD_PUBLISH;
    }

    @Override
    public int getVersionCode() {
        return BuildConfig.VERSION_CODE;
    }

    @Override
    public String getVersionName() {
        return BuildConfig.VERSION_NAME;
    }
}