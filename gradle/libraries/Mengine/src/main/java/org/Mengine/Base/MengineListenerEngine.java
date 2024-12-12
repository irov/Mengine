package org.Mengine.Base;

public interface MengineListenerEngine extends MengineServiceInterface {
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

    default void onMengineCaughtException(MengineApplication application, Throwable e) {
        //Empty
    }
}