package org.Mengine.Base;

import android.app.ActivityManager;
import android.content.Context;
import android.os.BatteryManager;
import android.os.Handler;
import android.os.Looper;
import android.util.Log;

import java.lang.reflect.Constructor;
import java.lang.reflect.InvocationTargetException;
import java.util.UUID;

public class MengineUtils {
    public static Class<?> getClazz(String TAG, String name, boolean required) {
        ClassLoader cl = MengineActivity.class.getClassLoader();

        if (cl == null) {
            if (required == true) {
                Log.e(TAG, "invalid create new instance: " + name + " ClassLoader is null");
            }

            return null;
        }

        try {
            Class<?> clazz = cl.loadClass(name);

            return clazz;
        } catch (ClassNotFoundException e) {
            if (required == true) {
                Log.e(TAG, "invalid create new instance: " + name + " ClassNotFoundException: " + e.getLocalizedMessage());
            }
        }

        return null;
    }

    public static Class<?> getPackageBuildConfig(String TAG, Package p) {
        String packageName = p.getName();
        String buildConfigName = packageName + ".BuildConfig";

        Class<?> clazz = MengineUtils.getClazz(TAG, buildConfigName, true);

        return clazz;
    }

    @SuppressWarnings("unchecked")
    public static <T> T newInstance(String TAG, String name, boolean required) {
        Class<?> clazz = MengineUtils.getClazz(TAG, name, required);

        if (clazz == null) {
            return null;
        }

        try {
            Constructor<?> ctr = clazz.getConstructor();
            T ob = (T)ctr.newInstance();

            return ob;
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

    public static Handler performOnMainThread(Runnable runnable) {
        Looper looper = Looper.getMainLooper();
        Handler handler = new Handler(looper);

        handler.post(runnable);

        return handler;
    }

    public static Handler performOnMainThreadDelayed(Runnable runnable, long delayMillis) {
        Looper looper = Looper.getMainLooper();
        Handler handler = new Handler(looper);

        handler.postDelayed(runnable, delayMillis);

        return handler;
    }

    public static boolean getDebugValue(boolean debug, boolean release) {
        if (BuildConfig.DEBUG == true) {
            return debug;
        } else {
            return release;
        }
    }

    public static long getTimestamp() {
        long timestamp = System.currentTimeMillis();

        return timestamp;
    }

    public static long getDurationTimestamp(long begin) {
        long timestamp = MengineUtils.getTimestamp();

        long duration = timestamp - begin;

        return duration;
    }

    public static long getRandomNumber() {
        long randomNumber = java.util.concurrent.ThreadLocalRandom.current().nextLong();

        return randomNumber;
    }

    public static String getRandomUUIDString() {
        String uuid = UUID.randomUUID().toString();

        return uuid;
    }

    public static int getBatteryLevel(Context context) {
        BatteryManager bm = (BatteryManager)context.getSystemService(Context.BATTERY_SERVICE);

        int batteryLevel = bm.getIntProperty(BatteryManager.BATTERY_PROPERTY_CAPACITY);

        return batteryLevel;
    }

    public static boolean isBatteryCharging(Context context) {
        BatteryManager bm = (BatteryManager)context.getSystemService(Context.BATTERY_SERVICE);

        int batteryStatus = bm.getIntProperty(BatteryManager.BATTERY_PROPERTY_STATUS);

        if (batteryStatus != BatteryManager.BATTERY_STATUS_CHARGING) {
            return false;
        }

        return true;
    }

    public static boolean isBatteryDischarging(Context context) {
        BatteryManager bm = (BatteryManager)context.getSystemService(Context.BATTERY_SERVICE);

        int batteryStatus = bm.getIntProperty(BatteryManager.BATTERY_PROPERTY_STATUS);

        if (batteryStatus != BatteryManager.BATTERY_STATUS_DISCHARGING) {
            return false;
        }

        return true;
    }

    public static boolean isBatteryNotCharging(Context context) {
        BatteryManager bm = (BatteryManager)context.getSystemService(Context.BATTERY_SERVICE);

        int batteryStatus = bm.getIntProperty(BatteryManager.BATTERY_PROPERTY_STATUS);

        if (batteryStatus != BatteryManager.BATTERY_STATUS_NOT_CHARGING) {
            return false;
        }

        return true;
    }

    public static boolean isBatteryFull(Context context) {
        BatteryManager bm = (BatteryManager)context.getSystemService(Context.BATTERY_SERVICE);

        int batteryStatus = bm.getIntProperty(BatteryManager.BATTERY_PROPERTY_STATUS);

        if (batteryStatus != BatteryManager.BATTERY_STATUS_FULL) {
            return false;
        }

        return true;
    }

    public static long getRamTotal(Context context) {
        ActivityManager actManager = (ActivityManager)context.getSystemService(Context.ACTIVITY_SERVICE);

        ActivityManager.MemoryInfo memInfo = new ActivityManager.MemoryInfo();
        actManager.getMemoryInfo(memInfo);

        long totalMemory = memInfo.totalMem;

        return totalMemory;
    }

    public static long getRamUsage(Context context) {
        ActivityManager actManager = (ActivityManager)context.getSystemService(Context.ACTIVITY_SERVICE);

        ActivityManager.MemoryInfo memInfo = new ActivityManager.MemoryInfo();
        actManager.getMemoryInfo(memInfo);

        long totalMemory = memInfo.totalMem;
        long availMemory = memInfo.availMem;

        long usageMemory = totalMemory - availMemory;

        return usageMemory;
    }
}
