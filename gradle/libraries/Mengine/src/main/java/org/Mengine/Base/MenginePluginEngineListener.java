package org.Mengine.Base;

public interface MenginePluginEngineListener extends MenginePluginInterface {
    default void onMengineInitializeBaseServices(MengineActivity activity) {
        //Empty
    }

    default void onMengineCreateApplication(MengineActivity activity) {
        //Empty
    }

    default void onMenginePlatformRun(MengineActivity activity) {
        //Empty
    }

    default void onMenginePlatformReady(MengineActivity activity) {
        //Empty
    }

    default void onMenginePlatformStop(MengineActivity activity) {
        //Empty
    }

    default void onMengineCurrentSceneChange(MengineActivity activity, String name) {
        //Empty
    }

    default void onMengineCaughtException(MengineApplication activity, Throwable e) {
        //Empty
    }
}