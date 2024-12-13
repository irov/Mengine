package org.Mengine.Base;

import android.content.Intent;
import android.content.res.Configuration;
import android.os.Bundle;

import androidx.annotation.NonNull;

public interface MengineListenerActivity extends MengineServiceInterface {
    default void onCreate(@NonNull MengineActivity activity, Bundle savedInstanceState) throws MengineServiceInvalidInitializeException {
        //Empty
    }

    default void onActivityResultBefore(@NonNull MengineActivity activity, int requestCode, int resultCode, Intent data) {
        //Empty
    }

    default void onActivityResult(@NonNull MengineActivity activity, int requestCode, int resultCode, Intent data) {
        //Empty
    }

    default void onStart(@NonNull MengineActivity activity) {
        //Empty
    }

    default void onStop(@NonNull MengineActivity activity) {
        //Empty
    }

    default void onPause(@NonNull MengineActivity activity) {
        //Empty
    }

    default void onResume(@NonNull MengineActivity activity) {
        //Empty
    }

    default void onNewIntent(@NonNull MengineActivity activity, Intent intent) {
        //Empty
    }

    default void onDestroy(@NonNull MengineActivity activity) {
        //Empty
    }

    default void onRestart(@NonNull MengineActivity activity) {
        //Empty
    }

    default void onConfigurationChanged(@NonNull MengineActivity activity, Configuration newConfig) {
        //Empty
    }

    default void onRequestPermissionsResult(@NonNull MengineActivity activity, int requestCode, String[] permissions, int[] grantResults) {
        //Empty
    }
}