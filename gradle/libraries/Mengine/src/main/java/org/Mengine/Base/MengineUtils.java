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
        } catch (ClassNotFoundException ex) {
            if (exist == true) {
                Log.e(TAG, "invalid create new instance: " + name + " ClassNotFoundException: " + ex.toString());
            }
        } catch (NoSuchMethodException ex) {
            Log.e(TAG, "invalid create mediation extension: " + name + " NoSuchMethodException: " + ex.toString());
        } catch (IllegalAccessException ex) {
            Log.e(TAG, "invalid create mediation extension: " + name + " IllegalAccessException: " + ex.toString());
        } catch (InstantiationException ex) {
            Log.e(TAG, "invalid create mediation extension: " + name + " InstantiationException: " + ex.toString());
        } catch (InvocationTargetException ex) {
            Log.e(TAG, "invalid create mediation extension: " + name + " InvocationTargetException: " + ex.toString());
        } catch (NullPointerException ex) {
            Log.e(TAG, "invalid create mediation extension: " + name + " NullPointerException: " + ex.toString());
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
