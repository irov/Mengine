package org.Mengine.Base;

public interface MenginePluginEngineListener extends MenginePluginInterface {
    default void onMengineInitializeBaseServices(MengineApplication application) {
        //Empty
    }

    default void onMengineCreateApplication(MengineApplication application) {
        //Empty
    }

    default void onMenginePlatformRun(MengineApplication application) {
        //Empty
    }

    default void onMenginePlatformStop(MengineApplication application) {
        //Empty
    }

    default void onMengineCurrentSceneChange(MengineApplication application, String name) {
        //Empty
    }

    default void onMengineCaughtException(MengineApplication application, Throwable e) {
        //Empty
    }
}