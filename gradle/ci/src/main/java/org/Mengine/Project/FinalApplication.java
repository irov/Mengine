package org.Mengine.Project;

public class FinalApplication extends org.Mengine.Base.MengineApplication {
    @Override
    public String[] getAndroidPlugins() {
        return BuildConfig.MENGINE_ANDROID_PLUGINS;
    }

    @Override
    public String[] getAndroidActivities() {
        return BuildConfig.MENGINE_ANDROID_ACTIVITIES;
    }

    @Override
    public String getApplicationId() {
        return BuildConfig.APPLICATION_ID;
    }

    @Override
    public int getVersionCode() {
        return BuildConfig.VERSION_CODE;
    }

    @Override
    public String getVersionName() {
        return BuildConfig.VERSION_NAME;
    }

    @Override
    public boolean isBuildPublish() {
        return BuildConfig.ANDROID_APP_BUILD_PUBLISH;
    }

    @Override
    public String getApplicationOptions() {
        return BuildConfig.MENGINE_APP_OPTIONS;
    }
}