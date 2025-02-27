package org.Mengine.Plugin.LocalNotifications;

import org.Mengine.Base.MengineService;
import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.MengineListenerActivity;

import androidx.annotation.NonNull;

import android.Manifest;
import android.content.Context;
import android.os.Build;
import android.os.Bundle;
import android.os.VibrationEffect;
import android.os.Vibrator;
import android.os.VibratorManager;

public class MengineVibratorPlugin extends MengineService implements MengineListenerActivity {
    public static final String SERVICE_NAME = "Vibrator";
    public static final boolean SERVICE_EMBEDDING = true;

    protected Vibrator m_vibrator;

    @Override
    public void onCreate(@NonNull MengineActivity activity, Bundle savedInstanceState) {
        activity.checkPermission(Manifest.permission.VIBRATE, () -> {
            Vibrator vibrator;
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S) {
                VibratorManager vibratorManager = (VibratorManager) activity.getSystemService(Context.VIBRATOR_MANAGER_SERVICE);
                vibrator = vibratorManager.getDefaultVibrator();
            } else {
                vibrator = (Vibrator) activity.getSystemService(Context.VIBRATOR_SERVICE);
            }

            if (vibrator.hasVibrator() == false) {
                this.logInfo("vibrator not found");

                return;
            }

            m_vibrator = vibrator;
        }, () -> {
            this.logMessage("[VIBRATOR] permission denied");
        });
    }

    public boolean vibrate(long milliseconds) {
        this.logMessage("vibrate milliseconds: %d"
            , milliseconds
        );

        if (m_vibrator == null) {
            return false;
        }

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            m_vibrator.vibrate(VibrationEffect.createOneShot(milliseconds, VibrationEffect.DEFAULT_AMPLITUDE));
        } else {
            m_vibrator.vibrate(milliseconds);
        }

        return true;
    }
}