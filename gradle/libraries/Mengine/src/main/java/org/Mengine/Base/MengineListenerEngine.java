package org.Mengine.Base;

import androidx.annotation.NonNull;

public interface MengineListenerEngine extends MengineServiceInterface {
    default void onMengineInitializeBaseServices(@NonNull MengineApplication application) {
        //Empty
    }

    default void onMengineCreateApplication(@NonNull MengineApplication application) {
        //Empty
    }

    default void onMenginePlatformRun(@NonNull MengineApplication application) {
        //Empty
    }

    default void onMenginePlatformStop(@NonNull MengineApplication application) {
        //Empty
    }

    default void onMengineCaughtException(@NonNull MengineApplication application, Throwable e) {
        //Empty
    }
}