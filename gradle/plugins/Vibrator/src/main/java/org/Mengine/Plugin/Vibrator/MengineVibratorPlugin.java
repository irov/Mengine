package org.Mengine.Plugin.Vibrator;

import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MengineService;
import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.MengineListenerActivity;
import org.Mengine.Base.MengineUtils;

import androidx.annotation.NonNull;

import android.Manifest;
import android.content.Context;
import android.os.Build;
import android.os.Bundle;
import android.os.VibrationEffect;
import android.os.Vibrator;
import android.os.VibratorManager;

import java.util.Arrays;
import java.util.List;

public class MengineVibratorPlugin extends MengineService implements MengineListenerActivity {
    public static final String SERVICE_NAME = "Vibrator";
    public static final boolean SERVICE_EMBEDDING = true;
    public static final int SAVE_VERSION = 1;

    protected Vibrator m_vibrator;

    protected boolean m_mute;

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

    @Override
    public Bundle onSave(@NonNull MengineApplication application) {
        Bundle bundle = new Bundle();

        bundle.putInt("version", SAVE_VERSION);
        bundle.putBoolean("mute", m_mute);

        return bundle;
    }

    @Override
    public void onLoad(@NonNull MengineApplication application, @NonNull Bundle bundle) {
        int version = bundle.getInt("version", 0);

        m_mute = bundle.getBoolean("mute", false);
    }

    public boolean vibrateOneShot(long milliseconds) {
        this.logMessage("vibrate OneShot milliseconds: %d"
            , milliseconds
        );

        if (m_vibrator == null) {
            return false;
        }

        if (m_mute == true) {
            return true;
        }

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            m_vibrator.vibrate(VibrationEffect.createOneShot(milliseconds, VibrationEffect.DEFAULT_AMPLITUDE));
        } else {
            m_vibrator.vibrate(milliseconds);
        }

        return true;
    }

    public boolean vibrateWaveform(List<Long> timings, int repeat) {
        this.logMessage("vibrate Waveform timings: %s repeat: %d"
            , timings
            , repeat
        );

        if (m_vibrator == null) {
            return false;
        }

        if (m_mute == true) {
            return false;
        }

        long[] timmingsArray = MengineUtils.longArrayFromList(timings);

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            m_vibrator.vibrate(VibrationEffect.createWaveform(timmingsArray, repeat));
        } else {
            m_vibrator.vibrate(timmingsArray, repeat);
        }

        return true;
    }

    public boolean vibrateWaveformAmplitudes(List<Long> timings, List<Integer> amplitudes, int repeat) {
        this.logMessage("vibrate Waveform timings: %s amplitudes: %s repeat: %d"
            , timings
            , amplitudes
            , repeat
        );

        if (m_vibrator == null) {
            return false;
        }

        if (m_mute == true) {
            return false;
        }

        long[] timmingsArray = MengineUtils.longArrayFromList(timings);
        int[] amplitudesArray = MengineUtils.intArrayFromList(amplitudes);

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            m_vibrator.vibrate(VibrationEffect.createWaveform(timmingsArray, amplitudesArray, repeat));
        } else {
            m_vibrator.vibrate(timmingsArray, repeat);
        }

        return true;
    }

    public void mute(boolean mute) {
        this.logMessage("mute: %s"
            , mute ? "true" : "false"
        );

        m_mute = mute;
    }

    public boolean isMute() {
        return m_mute;
    }

    public void cancel() {
        this.logMessage("cancel");

        if (m_vibrator == null) {
            return;
        }

        m_vibrator.cancel();
    }
}