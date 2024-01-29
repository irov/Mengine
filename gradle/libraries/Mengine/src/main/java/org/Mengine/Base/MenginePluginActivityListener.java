package org.Mengine.Base;

import android.content.Intent;
import android.content.res.Configuration;
import android.os.Bundle;

public interface MenginePluginActivityListener extends MenginePluginInterface {
    default void onCreate(MengineActivity activity, Bundle savedInstanceState) throws MenginePluginInvalidInitializeException {
        //Empty
    }

    default void onActivityResultBefore(MengineActivity activity, int requestCode, int resultCode, Intent data) {
        //Empty
    }

    default void onActivityResult(MengineActivity activity, int requestCode, int resultCode, Intent data) {
        //Empty
    }

    default void onStart(MengineActivity activity) {
        //Empty
    }

    default void onStop(MengineActivity activity) {
        //Empty
    }

    default void onPause(MengineActivity activity) {
        //Empty
    }

    default void onResume(MengineActivity activity) {
        //Empty
    }

    default void onNewIntent(MengineActivity activity, Intent intent) {
        //Empty
    }

    default void onDestroy(MengineActivity activity) {
        //Empty
    }

    default void onRestart(MengineActivity activity) {
        //Empty
    }

    default void onConfigurationChanged(MengineActivity activity, Configuration newConfig) {
        //Empty
    }

    default void onRequestPermissionsResult(MengineActivity activity, int requestCode, String[] permissions, int[] grantResults) {
        //Empty
    }
}