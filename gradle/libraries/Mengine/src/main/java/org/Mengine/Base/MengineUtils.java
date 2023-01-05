package org.Mengine.Base;

import android.os.Handler;
import android.os.Looper;
import android.util.Log;

import java.lang.reflect.Constructor;
import java.lang.reflect.InvocationTargetException;

public class MengineUtils {
    public static <T> T newInstance(String TAG, String name, boolean exist) {
        ClassLoader cl = MengineActivity.class.getClassLoader();

        try {
            Class<?> clazz = cl.loadClass(name);
            Constructor<?> ctr = clazz.getConstructor();
            T ob = (T)ctr.newInstance();

            return ob;
        } catch (ClassNotFoundException e) {
            if (exist == true) {
                Log.e(TAG, "invalid create new instance: " + name + " ClassNotFoundException: " + e.getLocalizedMessage());
            }
        } catch (NoSuchMethodException e) {
            Log.e(TAG, "invalid create mediation extension: " + name + " NoSuchMethodException: " + e.getLocalizedMessage());
        } catch (IllegalAccessException e) {
            Log.e(TAG, "invalid create mediation extension: " + name + " IllegalAccessException: " + e.getLocalizedMessage());
        } catch (InstantiationException e) {
            Log.e(TAG, "invalid create mediation extension: " + name + " InstantiationException: " + e.getLocalizedMessage());
        } catch (InvocationTargetException e) {
            Log.e(TAG, "invalid create mediation extension: " + name + " InvocationTargetException: " + e.getLocalizedMessage());
        } catch (NullPointerException e) {
            Log.e(TAG, "invalid create mediation extension: " + name + " NullPointerException: " + e.getLocalizedMessage());
        }

        return null;
    }

    public static void performOnMainThread(Runnable runnable) {
        new Handler(Looper.getMainLooper()).post(runnable);
    }

    public static void performOnMainThread(Runnable runnable, long delayMillis) {
        new Handler(Looper.getMainLooper()).postDelayed(runnable, delayMillis);
    }
}
