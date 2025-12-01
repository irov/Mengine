package org.Mengine.Base;

import android.app.ActivityManager;
import android.app.Notification;
import android.content.ActivityNotFoundException;
import android.content.Context;
import android.content.Intent;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.content.res.Configuration;
import android.content.res.Resources;
import android.graphics.Rect;
import android.net.Uri;
import android.os.BatteryManager;
import android.os.Build;
import android.os.Bundle;
import android.os.Handler;
import android.os.LocaleList;
import android.os.Looper;
import android.util.DisplayMetrics;
import android.view.Display;
import android.view.WindowManager;
import android.view.WindowMetrics;

import androidx.annotation.NonNull;
import androidx.core.content.FileProvider;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;
import org.w3c.dom.Document;
import org.xml.sax.SAXException;

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.PrintWriter;
import java.io.StringWriter;
import java.lang.reflect.Constructor;
import java.lang.reflect.Method;
import java.nio.charset.StandardCharsets;
import java.security.SecureRandom;
import java.time.Instant;
import java.time.ZoneId;
import java.time.format.DateTimeFormatter;
import java.util.Arrays;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.ArrayList;
import java.util.Locale;
import java.util.Map;
import java.util.Objects;
import java.util.UUID;
import java.util.stream.Collectors;
import java.util.zip.ZipEntry;
import java.util.zip.ZipOutputStream;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.ParserConfigurationException;

public class MengineUtils {
    public static final MengineTag TAG = MengineTag.of("MNGUtils");

    public static Class<?> getClazz(@NonNull ClassLoader cl, @NonNull MengineTag TAG, String name, boolean required) {
        try {
            Class<?> clazz = cl.loadClass(name);

            return clazz;
        } catch (final ClassNotFoundException e) {
            if (required == true) {
                MengineLog.logError(TAG, "[ERROR] invalid create new instance: %s ClassNotFoundException: %s"
                    , name
                    , e.getMessage()
                );
            }
        }

        return null;
    }

    public static Class<?> getPackageBuildConfig(@NonNull ClassLoader cl, @NonNull MengineTag TAG, Package p) {
        String packageName = p.getName();
        String buildConfigName = packageName + ".BuildConfig";

        Class<?> clazz = MengineUtils.getClazz(cl, TAG, buildConfigName, true);

        return clazz;
    }

    public static Object newInstance(@NonNull ClassLoader cl, @NonNull MengineTag TAG, @NonNull String name, boolean required, Object ... args) {
        Class<?> clazz = MengineUtils.getClazz(cl, TAG, name, required);

        if (clazz == null) {
            return null;
        }

        Object ob = MengineUtils.newInstance(TAG, clazz, required, args);

        return ob;
    }

    public static Object newInstance(@NonNull MengineTag TAG, Class<?> cls, boolean required, Object ... args) {
        try {
            for (Constructor<?> constructor : cls.getDeclaredConstructors()) {
                Class<?>[] paramTypes = constructor.getParameterTypes();

                if (paramTypes.length != args.length) {
                    continue;
                }

                boolean match = true;
                for (int i = 0; i != paramTypes.length; ++i) {
                    Class<?> paramType = paramTypes[i];
                    Class<?> argType = args[i].getClass();

                    if (paramType.isAssignableFrom(argType) == false) {
                        match = false;
                        break;
                    }
                }

                if (match == false) {
                    continue;
                }

                constructor.setAccessible(true);

                Object ob = constructor.newInstance(args);

                return ob;
            }
        } catch (final Exception e) {
            MengineLog.logException(TAG, e, Map.of("cls", cls.getName()));
        }

        return null;
    }

    public static String getCurrentThreadName() {
        Thread thread = Thread.currentThread();

        String threadName = thread.getName();

        return threadName;
    }

    public static class Code {
        Code(String file, int line, String function) {
            this.file = file;
            this.line = line;
            this.method = function;
        }

        public final String file;
        public final int line;
        public final String method;
    }

    public static Code getCurrentThreadCode(int index) {
        if (BuildConfig.DEBUG == false) {
            return new Code("", 0, "");
        }

        StackTraceElement[] trace = Thread.currentThread().getStackTrace();

        if (trace.length < index) {
            return new Code("", 0, "");
        }

        StackTraceElement top = trace[index];

        String file = top.getFileName();
        int line = top.getLineNumber();
        String method = top.getMethodName();

        Code code = new Code(file, line, method);

        return code;
    }

    public static Handler performOnMainThread(@NonNull Runnable runnable) {
        final Looper mainLooper = Looper.getMainLooper();

        if (Looper.myLooper() == mainLooper) {
            runnable.run();

            return null;
        }

        Handler handler = new Handler(mainLooper);

        if (handler.post(runnable) == false) {
            MengineLog.logError(TAG, "[ERROR] performOnMainThread runnable: %s failed to post to main thread"
                , runnable
            );

            return null;
        }

        return handler;
    }

    public static Handler performOnMainThreadDelayed(@NonNull Runnable runnable, long delayMillis) {
        final Looper mainLooper = Looper.getMainLooper();
        final Handler handler = new Handler(mainLooper);

        if (handler.postDelayed(runnable, delayMillis) == false) {
            MengineLog.logError(TAG, "[ERROR] performOnMainThreadDelayed runnable: %s failed to post to main thread with delay: %d"
                , runnable
                , delayMillis
            );

            return null;
        }

        return handler;
    }

    public static MengineRunnablePeriodically scheduleOnMainThreadFixedRate(long delay, long period, @NonNull Runnable runnable) {
        final Looper mainLooper = Looper.getMainLooper();
        final Handler handler = new Handler(mainLooper);

        MengineRunnablePeriodically runnablePeriodically = new MengineRunnablePeriodically(handler, runnable, period);

        runnablePeriodically.start(delay);

        return runnablePeriodically;
    }

    public static Thread performOnNewThread(@NonNull Runnable runnable, String name) {
        Thread thread = new Thread(runnable, name);

        try {
            thread.start();
        } catch (final Exception e) {
            MengineLog.logError(TAG, "[ERROR] performOnNewThread runnable: %s failed to start thread exception: %s"
                , runnable
                , e.getMessage()
            );

            return null;
        }

        return thread;
    }

    public static Thread performOnNewThreadDelay(long delay, @NonNull Runnable runnable, String name) {
        Thread thread = new Thread(() -> {
            try {
                Thread.sleep(delay);
            } catch (final InterruptedException e) {
                Thread.currentThread().interrupt();
                return;
            }

            runnable.run();
        }, name);

        try {
            thread.start();
        } catch (final Exception e) {
            MengineLog.logError(TAG, "[ERROR] performOnNewThreadDelay runnable: %s failed to start thread exception: %s"
                , runnable
                , e.getMessage()
            );

            return null;
        }

        return thread;
    }

    public static Object getDebugValue(Object d, Object r) {
        if (BuildConfig.DEBUG == true) {
            return d;
        } else {
            return r;
        }
    }

    public static Object getRedactedValue(Object d) {
        if (BuildConfig.DEBUG == true) {
            return d;
        } else {
            return "[REDACTED]";
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

    public static long getThreadLocalRandomNumber() {
        long randomNumber = java.util.concurrent.ThreadLocalRandom.current().nextLong();

        return randomNumber;
    }

    public static long getSecureRandomNumber() {
        SecureRandom r = new SecureRandom();

        long randomNumber = r.nextLong();

        if (randomNumber == 0) {
            randomNumber = 1;
        } else if (randomNumber < 0) {
            randomNumber = -randomNumber;
        }

        return randomNumber;
    }

    public static String getRandomUUIDString() {
        UUID uuid = UUID.randomUUID();
        String rand = uuid.toString();

        return rand;
    }

    public static String getSecureRandomHexString(int num) {
        SecureRandom r = new SecureRandom();
        StringBuilder sb = new StringBuilder();
        while(sb.length() < num) {
            int n = r.nextInt();
            String h = Integer.toHexString(n);
            sb.append(h);
        }

        String rand = sb.toString();
        String randN = rand.substring(0, num);

        return randN;
    }

    public static int getBatteryLevel(Context context) {
        BatteryManager bm = context.getSystemService(BatteryManager.class);

        if (bm == null) {
            return Integer.MIN_VALUE;
        }

        int batteryLevel = bm.getIntProperty(BatteryManager.BATTERY_PROPERTY_CAPACITY);

        return batteryLevel;
    }

    private static int getBatteryStatus(Context context) {
        if (android.os.Build.VERSION.SDK_INT < android.os.Build.VERSION_CODES.O) {
            return BatteryManager.BATTERY_STATUS_UNKNOWN;
        }

        BatteryManager bm = context.getSystemService(BatteryManager.class);

        if (bm == null) {
            return BatteryManager.BATTERY_STATUS_UNKNOWN;
        }

        int batteryStatus = bm.getIntProperty(BatteryManager.BATTERY_PROPERTY_STATUS);

        return batteryStatus;
    }

    public static boolean isBatteryCharging(Context context) {
        int batteryStatus = MengineUtils.getBatteryStatus(context);

        if (batteryStatus != BatteryManager.BATTERY_STATUS_CHARGING) {
            return false;
        }

        return true;
    }

    public static boolean isBatteryDischarging(Context context) {
        int batteryStatus = MengineUtils.getBatteryStatus(context);

        if (batteryStatus != BatteryManager.BATTERY_STATUS_DISCHARGING) {
            return false;
        }

        return true;
    }

    public static boolean isBatteryNotCharging(Context context) {
        int batteryStatus = MengineUtils.getBatteryStatus(context);

        if (batteryStatus != BatteryManager.BATTERY_STATUS_NOT_CHARGING) {
            return false;
        }

        return true;
    }

    public static boolean isBatteryFull(Context context) {
        int batteryStatus = MengineUtils.getBatteryStatus(context);

        if (batteryStatus != BatteryManager.BATTERY_STATUS_FULL) {
            return false;
        }

        return true;
    }

    public static int getMemoryClass(Context context) {
        ActivityManager activityManager = context.getSystemService(ActivityManager.class);

        if (activityManager == null) {
            return 0;
        }

        int memoryClass = activityManager.getMemoryClass();

        return memoryClass;
    }

    public static long getMemoryRamTotal(Context context) {
        ActivityManager activityManager = context.getSystemService(ActivityManager.class);

        if (activityManager == null) {
            return 0;
        }

        ActivityManager.MemoryInfo memInfo = new ActivityManager.MemoryInfo();
        activityManager.getMemoryInfo(memInfo);

        long totalMemory = memInfo.totalMem;

        return totalMemory;
    }

    public static long getMemoryRamUsage(Context context) {
        ActivityManager activityManager = context.getSystemService(ActivityManager.class);

        if (activityManager == null) {
            return 0;
        }

        ActivityManager.MemoryInfo memInfo = new ActivityManager.MemoryInfo();
        activityManager.getMemoryInfo(memInfo);

        long totalMemory = memInfo.totalMem;
        long availMemory = memInfo.availMem;

        long usageMemory = totalMemory - availMemory;

        return usageMemory;
    }

    private static void zipFile(ZipOutputStream out, File file, int basePathLength) throws IOException {
        final int BUFFER = 2048;

        String unmodifiedFilePath = file.getPath();
        String relativePath = unmodifiedFilePath.substring(basePathLength + 1);
        FileInputStream fi = new FileInputStream(unmodifiedFilePath);
        BufferedInputStream origin = new BufferedInputStream(fi, BUFFER);
        ZipEntry entry = new ZipEntry(relativePath);
        long modified = file.lastModified();
        entry.setTime(modified);
        out.putNextEntry(entry);

        byte[] data = new byte[BUFFER];
        int count;
        while ((count = origin.read(data, 0, BUFFER)) != -1) {
            out.write(data, 0, count);
        }

        origin.close();
        fi.close();
    }

    private static void zipSubFolder(ZipOutputStream out, File folder, int basePathLength) throws IOException {
        File[] fileList = folder.listFiles();

        if (fileList == null) {
            return;
        }

        for (File file : fileList) {
            boolean isDirectory = file.isDirectory();

            if (isDirectory == true) {
                MengineUtils.zipSubFolder(out, file, basePathLength);
            } else {
                MengineUtils.zipFile(out, file, basePathLength);
            }
        }
    }

    public static boolean zipFiles(File sourceFile, File toLocation) {
        MengineLog.logInfo(TAG, "zip source '%s' to: %s"
            , sourceFile.getPath()
            , toLocation.getPath()
        );

        try {
            FileOutputStream dest = new FileOutputStream(toLocation);
            ZipOutputStream out = new ZipOutputStream(new BufferedOutputStream(dest));

            String sourceFileParent = sourceFile.getParent();

            if (sourceFileParent == null) {
                return false;
            }

            int basePathLength = sourceFileParent.length();

            boolean isDirectory = sourceFile.isDirectory();

            if (isDirectory == true) {
                MengineUtils.zipSubFolder(out, sourceFile, basePathLength);
            } else {
                MengineUtils.zipFile(out, sourceFile, basePathLength);
            }

            out.close();
            dest.close();
        } catch (final Exception e) {
            MengineLog.logError(TAG, "[ERROR] zipFiles exception: %s"
                , e.getMessage()
            );

            return false;
        }

        return true;
    }

    public static List<String> splitToList(String value, String separator) {
        return Arrays.stream(value.split(separator))
            .filter(s -> !s.isEmpty())
            .collect(Collectors.toList());
    }

    public static String getLastPathComponent(String filePath) {
        List<String> segments = MengineUtils.splitToList(filePath, "/");

        if (segments.isEmpty() == true) {
            return "";
        }

        String lastPathComponent = segments.get(segments.size() - 1);

        return lastPathComponent;
    }

    public static File createTempFile(Context context, String prefix, String suffix) {
        File cacheDir = context.getCacheDir();

        File tempFile;

        try {
            tempFile = File.createTempFile(prefix, suffix, cacheDir);
        } catch (final IOException e) {
            MengineLog.logError(TAG, "[ERROR] failed create temp file %s***%s in dir: %s exception: %s"
                , prefix
                , suffix
                , cacheDir.getPath()
                , e.getMessage()
            );

            return null;
        }

        MengineLog.logInfo(TAG, "create temp file: %s in dir: %s"
            , tempFile.getAbsolutePath()
            , cacheDir.getPath()
        );

        return tempFile;
    }

    public static Uri getUriForFile(Context context, File file) {
        String packageName = context.getPackageName();

        try {
            Uri uri = FileProvider.getUriForFile(context, packageName + ".fileprovider", file);

            return uri;
        } catch (final IllegalArgumentException e) {
            MengineLog.logError(TAG, "[ERROR] failed get uri for file: %s exception: %s"
                , file.getPath()
                , e.getMessage()
            );
        }

        return null;
    }

    public static String getDateFormat(long timestamp, String format) {
        Instant instant = Instant.ofEpochMilli(timestamp);

        ZoneId zone = ZoneId.systemDefault();

        DateTimeFormatter formatter = DateTimeFormatter.ofPattern(format)
            .withLocale(Locale.US)
            .withZone(zone);

        String date = formatter.format(instant);

        return date;
    }

    public static String getThrowableStackTrace(Throwable e) {
        StringWriter sw = new StringWriter();
        PrintWriter pw = new PrintWriter(sw);

        e.printStackTrace(pw);

        String stack = sw.toString(); // stack trace as a string

        return stack;
    }

    public static String getProcessNameBeforeVersionP(Context context) {
        int mypid = android.os.Process.myPid();

        ActivityManager manager = context.getSystemService(ActivityManager.class);

        if (manager == null) {
            return null;
        }

        List<ActivityManager.RunningAppProcessInfo> infos = manager.getRunningAppProcesses();

        if (infos == null) {
            return null;
        }

        for (ActivityManager.RunningAppProcessInfo info : infos) {
            if (info.pid != mypid) {
                continue;
            }

            return info.processName;
        }

        return null;
    }

    public static Document parseDocument(@NonNull InputStream stream) {
        try {
            DocumentBuilderFactory factory = DocumentBuilderFactory.newInstance();
            DocumentBuilder builder = factory.newDocumentBuilder();

            Document document = builder.parse(stream);

            return document;
        } catch (final ParserConfigurationException e) {
            MengineLog.logError(TAG, "parseDocument catch ParserConfigurationException: %s", e.getMessage());
        } catch (final SAXException e) {
            MengineLog.logError(TAG, "parseDocument catch SAXException: %s", e.getMessage());
        } catch (final IOException e) {
            MengineLog.logError(TAG, "parseDocument catch IOException: %s", e.getMessage());
        }

        return null;
    }

    public static boolean openUrl(@NonNull MengineActivity activity, String url) {
        Uri uri = Uri.parse(url);
        Intent intent = new Intent(Intent.ACTION_VIEW, uri);
        intent.addCategory(Intent.CATEGORY_BROWSABLE);

        try {
            activity.startActivity(intent);
        } catch (final ActivityNotFoundException e) {
            MengineLog.logWarning(TAG, "openUrl url: %s catch ActivityNotFoundException: %s"
                , url
                , e.getMessage()
            );

            return false;
        }

        return true;
    }

    public static void sleep(long millis) {
        try {
            Thread.sleep(millis);
        } catch (final InterruptedException e) {
            // Ignore
        }
    }

    public static byte [] inputStreamToByteArray(InputStream stream) throws IOException {
        ByteArrayOutputStream out = new ByteArrayOutputStream();

        byte [] buffer = new byte[1024];
        int len;
        while ((len = stream.read(buffer)) != -1) {
            out.write(buffer, 0, len);
        }

        out.close();

        return out.toByteArray();
    }

    public static byte [] inputStreamToByteArraySize(InputStream stream, int size) throws IOException {
        byte [] buffer = new byte[size];

        int bytesRead = 0;

        while (bytesRead < size) {
            int read = stream.read(buffer, bytesRead, size - bytesRead);

            if (read == -1) {
                break;
            }

            bytesRead += read;
        }

        return buffer;
    }

    public static String inputStreamToString(InputStream stream) throws IOException {
        ByteArrayOutputStream out = new ByteArrayOutputStream();

        byte [] buffer = new byte[1024];
        int len;
        while ((len = stream.read(buffer)) != -1) {
            out.write(buffer, 0, len);
        }
        out.close();

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) {
            return out.toString(StandardCharsets.UTF_8);
        } else {
            return new String(out.toByteArray(), StandardCharsets.UTF_8);
        }
    }

    public static boolean validateJSON(String json) {
        try {
            new JSONObject(json);
        } catch (final JSONException e) {
            return false;
        }

        return true;
    }

    public static Map<String, Object> jsonObjectToMap(JSONObject obj) {
        Map<String, Object> map = new HashMap<>();

        if (obj == null) {
            return map;
        }

        Iterator<String> keys = obj.keys();
        while (keys.hasNext() == true) {
            String key = keys.next();

            try {
                Object value = obj.get(key);
                Object parsedValue = MengineUtils.parseJsonValue(value);
                map.put(key, parsedValue);
            } catch (final JSONException e) {
                MengineLog.logError(TAG, "[ERROR] jsonObjectToMap exception: %s", e.getMessage());
            }
        }

        return map;
    }

    private static List<Object> jsonArrayToList(JSONArray array) {
        List<Object> list = new ArrayList<>();

        int array_length = array.length();

        for (int index = 0; index != array_length; index++) {
            try {
                Object value = array.get(index);
                Object parsedValue = MengineUtils.parseJsonValue(value);
                list.add(parsedValue);
            } catch (final JSONException e) {
                MengineLog.logError(TAG, "[ERROR] jsonArrayToList exception: %s", e.getMessage());
            }
        }

        return list;
    }

    private static Object parseJsonValue(Object value) {
        if (value instanceof JSONArray) {
            return MengineUtils.jsonArrayToList((JSONArray) value);
        } else if (value instanceof JSONObject) {
            return MengineUtils.jsonObjectToMap((JSONObject) value);
        } else if (value == JSONObject.NULL) {
            return null;
        } else {
            return value;
        }
    }

    public static void loadLibrary(Context context, String libraryName) throws UnsatisfiedLinkError, SecurityException {
        try {
            Class<?> relinkClass = context.getClassLoader().loadClass("com.getkeepsafe.relinker.ReLinker");
            Class<?> relinkListenerClass = context.getClassLoader().loadClass("com.getkeepsafe.relinker.ReLinker$LoadListener");
            Class<?> contextClass = context.getClassLoader().loadClass("android.content.Context");
            Class<?> stringClass = context.getClassLoader().loadClass("java.lang.String");

            Method forceMethod = relinkClass.getDeclaredMethod("force");
            Object relinkInstance = forceMethod.invoke(null);
            Class<?> relinkInstanceClass = relinkInstance.getClass();

            Method loadMethod = relinkInstanceClass.getDeclaredMethod("loadLibrary", contextClass, stringClass, stringClass, relinkListenerClass);
            loadMethod.invoke(relinkInstance, context, libraryName, null, null);
        }
        catch (final Throwable e) {
            try {
                System.loadLibrary(libraryName);
            } catch (final UnsatisfiedLinkError e2) {
                throw e2;
            } catch (final SecurityException e2) {
                throw e2;
            } catch (final NullPointerException e3) {
                throw e3;
            }
        }
    }

    @SuppressWarnings("deprecation")
    public static Locale getConfigurationLocale(@NonNull Configuration configuration) {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.N) {
            LocaleList locales = configuration.getLocales();

            Locale locale = locales.get(0);

            return locale;
        } else {
            Locale locale = configuration.locale;

            return locale;
        }
    }

    @SuppressWarnings("deprecation")
    public static ApplicationInfo getPackageApplicationInfo(PackageManager packageManager, String packageName) throws PackageManager.NameNotFoundException {
        ApplicationInfo applicationInfo;

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) {
            PackageManager.ApplicationInfoFlags flags = PackageManager.ApplicationInfoFlags.of(PackageManager.GET_META_DATA);

            applicationInfo = packageManager.getApplicationInfo(packageName, flags);
        } else {
            applicationInfo = packageManager.getApplicationInfo(packageName, PackageManager.GET_META_DATA);
        }

        return applicationInfo;
    }

    @SuppressWarnings("deprecation")
    public static PackageInfo getPackageInfo(PackageManager manager, String packageName) {
        PackageInfo packageInfo;

        try {
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) {
                packageInfo = manager.getPackageInfo(packageName, PackageManager.PackageInfoFlags.of(0));
            } else {
                packageInfo = manager.getPackageInfo(packageName, 0);
            }
        } catch (final PackageManager.NameNotFoundException e) {
            return null;
        }

        return packageInfo;
    }

    @SuppressWarnings("deprecation")
    public static Notification getParcelableExtra(Intent intent, String name) {
        Notification notification;
        if (android.os.Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) {
            notification = intent.getParcelableExtra(name, Notification.class);
        } else {
            notification = intent.getParcelableExtra(name);
        }

        return notification;
    }

    @SuppressWarnings("deprecation")
    public static Display getDefaultDisplay(Context context) {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.R) {
            return context.getDisplay();
        } else {
            WindowManager windowManager = context.getSystemService(WindowManager.class);

            if (windowManager == null) {
                return null;
            }

            return windowManager.getDefaultDisplay();
        }
    }

    @SuppressWarnings("deprecation")
    public static Rect getDeviceWindowRectQ(Context context, Display display) {
        DisplayMetrics realMetrics = new DisplayMetrics();
        display.getRealMetrics( realMetrics );

        int width = realMetrics.widthPixels;
        int height = realMetrics.heightPixels;

        return new Rect(0, 0, width, height);
    }

    public static Rect getDeviceWindowRect(Context context, Display display)
    {
        if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.R) {
            WindowManager windowManager = context.getSystemService(WindowManager.class);

            if (windowManager == null) {
                return MengineUtils.getDeviceWindowRectQ(context, display);
            }

            WindowMetrics windowMetrics = windowManager.getCurrentWindowMetrics();

            Rect bounds = windowMetrics.getBounds();

            return bounds;
        } else {
            return MengineUtils.getDeviceWindowRectQ(context, display);
        }
    }

    public static int getDeviceScreenSize(Context context) {
        Resources resources = context.getResources();
        Configuration configuration = resources.getConfiguration();
        int screenLayout = configuration.screenLayout;

        int screenLayoutSize = screenLayout & Configuration.SCREENLAYOUT_SIZE_MASK;

        return screenLayoutSize;
    }

    public static boolean isDeviceTablet(Context context) {
        Resources resources = context.getResources();
        Configuration configuration = resources.getConfiguration();

        int smallestWidthDp = configuration.smallestScreenWidthDp;

        if (smallestWidthDp >= 600) {
            return true;
        }

        return false;
    }

    public static boolean isDebuggerConnected() {
        return android.os.Debug.isDebuggerConnected();
    }

    public static boolean isAppInForeground(Context context) {
        ActivityManager activityManager = context.getSystemService(ActivityManager.class);

        if (activityManager == null) {
            return false;
        }

        List<ActivityManager.RunningAppProcessInfo> appProcesses = activityManager.getRunningAppProcesses();

        if (appProcesses == null) {
            return false;
        }

        String packageName = context.getPackageName();

        for (ActivityManager.RunningAppProcessInfo appProcess : appProcesses) {
            if (appProcess.importance != ActivityManager.RunningAppProcessInfo.IMPORTANCE_FOREGROUND) {
                continue;
            }

            if (Objects.equals(appProcess.processName, packageName) == false) {
                continue;
            }

            return true;
        }

        return false;
    }

    public static long getUsedJVMMemory() {
        Runtime runtime = Runtime.getRuntime();

        long usedJVMMemory = runtime.totalMemory() - runtime.freeMemory();

        return usedJVMMemory;
    }

    public static void printCurrentStackTrace() {
        new Exception("StackTrace").printStackTrace();
    }

    public static void throwRuntimeException(String message, Throwable throwable) {
        throw new RuntimeException(message, throwable);
    }

    public static void throwAssertionError(@NonNull MengineTag tag, Throwable throwable, String format, Object ... args) {
        String message = MengineLog.buildTotalMsg(format, args);

        if (MengineActivity.INSTANCE == null ||
            MengineActivity.INSTANCE.isFinishing() == true ||
            MengineActivity.INSTANCE.isDestroyed() == true) {
            MengineUtils.throwRuntimeException(message, throwable);

            return;
        }

        MengineUI.finishActivityWithAlertDialog(MengineActivity.INSTANCE, "AssertionError", "[%s] %s", tag, message);
    }

    public static ArrayList<Bundle> parcelableArrayListFromJSON(@NonNull JSONArray value) {
        ArrayList<Bundle> list = new ArrayList<>();

        for (int i = 0; i < value.length(); i++) {
            try {
                JSONObject object = value.getJSONObject(i);

                Bundle bundle = MengineUtils.bundleFromJSONObject(object);

                list.add(bundle);
            } catch (JSONException e) {
                MengineLog.logException(TAG, e, Map.of(
                    "index", i,
                    "value", value.opt(i)
                ));
            }
        }

        return list;
    }

    public static Bundle bundleFromJSONObject(@NonNull JSONObject value) {
        Bundle bundle = new Bundle();
        MengineUtils.copyBundleFromJSONObject(bundle, value);

        return bundle;
    }

    public static void copyBundleFromJSONObject(@NonNull Bundle bundle, @NonNull JSONObject value) {
        Iterator<String> keys = value.keys();
        while (keys.hasNext()) {
            String key = keys.next();
            try {
                Object object = value.get(key);
                if (object instanceof String) {
                    bundle.putString(key, (String) object);
                } else if (object instanceof Integer) {
                    bundle.putInt(key, (Integer) object);
                } else if (object instanceof Long) {
                    bundle.putLong(key, (Long) object);
                } else if (object instanceof Boolean) {
                    bundle.putBoolean(key, (Boolean) object);
                } else if (object instanceof Double) {
                    bundle.putDouble(key, (Double) object);
                } else if (object instanceof JSONObject) {
                    Bundle subBundle = MengineUtils.bundleFromJSONObject((JSONObject) object);
                    bundle.putBundle(key, subBundle);
                } else if (object instanceof JSONArray) {
                    ArrayList<Bundle> bundles = MengineUtils.parcelableArrayListFromJSON((JSONArray) object);
                    bundle.putParcelableArrayList(key, bundles);
                }
            } catch (JSONException e) {
                MengineLog.logException(TAG, e, Map.of(
                    "key", key,
                    "value", value.opt(key)
                ));
            }
        }
    }

    public static JSONArray jsonArrayFromArrayList(List list) {
        JSONArray array = new JSONArray();
        for (Object object : list) {
            if (object instanceof String) {
                array.put(object);
            } else if (object instanceof Integer) {
                array.put(object);
            } else if (object instanceof Long) {
                array.put(object);
            } else if (object instanceof Boolean) {
                array.put(object);
            } else if (object instanceof Double) {
                array.put(object);
            } else if (object instanceof Bundle) {
                JSONObject subObject = MengineUtils.jsonObjectFromBundle((Bundle) object);
                array.put(subObject);
            } else if (object instanceof List) {
                JSONArray subArray = MengineUtils.jsonArrayFromArrayList((List) object);
                array.put(subArray);
            }
        }

        return array;
    }

    public static JSONObject jsonObjectFromBundle(@NonNull Bundle bundle) {
        JSONObject object = new JSONObject();
        for (String key : bundle.keySet()) {
            Object value = bundle.get(key);
            try {
                if (value instanceof String) {
                    object.put(key, value);
                } else if (value instanceof Integer) {
                    object.put(key, value);
                } else if (value instanceof Long) {
                    object.put(key, value);
                } else if (value instanceof Boolean) {
                    object.put(key, value);
                } else if (value instanceof Double) {
                    object.put(key, value);
                } else if (value instanceof Bundle) {
                    JSONObject subObject = MengineUtils.jsonObjectFromBundle((Bundle) value);

                    object.put(key, subObject);
                } else if (value instanceof List) {
                    JSONArray array = MengineUtils.jsonArrayFromArrayList((List) value);

                    object.put(key, array);
                }
            } catch (JSONException e) {
                MengineLog.logException(TAG, e, Map.of(
                    "key", key,
                    "value", value
                ));
            }
        }

        return object;
    }

    public static long [] longArrayFromList(@NonNull List<Long> l) {
        int s = l.size();
        long[] a = new long[s];

        for (int i = 0; i < s; i++) {
            a[i] = l.get(i);
        }

        return a;
    }

    public static int [] intArrayFromList(@NonNull List<Integer> l) {
        int s = l.size();
        int[] a = new int[s];

        for (int i = 0; i < s; i++) {
            a[i] = l.get(i);
        }

        return a;
    }

    public static double getMapDouble(@NonNull Map<String, Object> params, @NonNull String name) {
        Object value = params.get(name);

        if (value instanceof Double) {
            return (Double) value;
        }

        return 0.0;
    }

    private static char escapeJsonHEX(int nibble) {
        final char[] HEX = "0123456789ABCDEF".toCharArray();

        return HEX[nibble & 0xF];
    }

    public static String escapeJsonString(String s) {
        StringBuilder sb = new StringBuilder(s.length() + 16);

        for (int i = 0; i < s.length(); ) {
            int cp = s.codePointAt(i);

            i += Character.charCount(cp);

            switch (cp) {
                case '"':  sb.append("\\\""); break;
                case '\\': sb.append("\\\\"); break;
                case '\b': sb.append("\\b");  break;
                case '\f': sb.append("\\f");  break;
                case '\n': sb.append("\\n");  break;
                case '\r': sb.append("\\r");  break;
                case '\t': sb.append("\\t");  break;
                case '/':  sb.append("\\/");  break;
                case 0x2028: sb.append("\\u2028"); break;
                case 0x2029: sb.append("\\u2029"); break;
                default:
                    if (cp < 0x20 || cp > 0x7E) {
                        sb.append("\\u")
                                .append(MengineUtils.escapeJsonHEX(cp >> 12))
                                .append(MengineUtils.escapeJsonHEX(cp >>  8))
                                .append(MengineUtils.escapeJsonHEX(cp >>  4))
                                .append(MengineUtils.escapeJsonHEX(cp));
                    } else {
                        sb.appendCodePoint(cp);
                    }
            }
        }

        return sb.toString();
    }

    static public boolean hasOption(@NonNull MengineApplication application, String option) {
        if (BuildConfig.DEBUG == false) {
            return false;
        }

        String options_str = application.getApplicationOptions();

        List<String> options = MengineUtils.splitToList(options_str, " ");

        if (options.isEmpty() == true) {
            return false;
        }

        String hyphen_option = "-" + option;
        String double_hyphen_option = "--" + option;

        String hyphen_option_value = hyphen_option + ":";
        String double_hyphen_option_value = double_hyphen_option + ":";

        for(String o : options) {
            if (o.equals(hyphen_option) == true) {
                return true;
            }

            if (o.equals(double_hyphen_option) == true) {
                return true;
            }

            if (o.startsWith(hyphen_option_value) == true) {
                return true;
            }

            if (o.startsWith(double_hyphen_option_value) == true) {
                return true;
            }
        }

        return false;
    }

    static public int getOptionValueInteger(@NonNull MengineApplication application, String option, int defaultValue) {
        if (BuildConfig.DEBUG == false) {
            return defaultValue;
        }

        String options_str = application.getApplicationOptions();

        List<String> options = MengineUtils.splitToList(options_str, " ");

        if (options.isEmpty() == true) {
            return defaultValue;
        }

        String hyphen_option = "-" + option;
        String double_hyphen_option = "--" + option;

        String hyphen_option_value = hyphen_option + ":";
        String double_hyphen_option_value = double_hyphen_option + ":";

        for(String o : options) {
            if (o.equals(hyphen_option) == true) {
                MengineLog.logSingleWarning(TAG, "option [%s] has no value", option);

                return defaultValue;
            }

            if (o.equals(double_hyphen_option) == true) {
                MengineLog.logSingleWarning(TAG, "option [%s] has no value", option);

                return defaultValue;
            }

            if (o.startsWith(hyphen_option_value) == true) {
                String value = o.substring(hyphen_option_value.length());

                try {
                    return Integer.parseInt(value);
                } catch (final NumberFormatException e) {
                    MengineLog.logSingleWarning(TAG, "option [%s] invalid integer value [%s]", option, value);

                    return defaultValue;
                }
            }

            if (o.startsWith(double_hyphen_option_value) == true) {
                String value = o.substring(double_hyphen_option_value.length());

                try {
                    return Integer.parseInt(value);
                } catch (final NumberFormatException e) {
                    MengineLog.logSingleWarning(TAG, "option [%s] invalid integer value [%s]", option, value);

                    return defaultValue;
                }
            }
        }

        return defaultValue;
    }

    static public long getOptionValueLong(@NonNull MengineApplication application, String option, long defaultValue) {
        if (BuildConfig.DEBUG == false) {
            return defaultValue;
        }

        String options_str = application.getApplicationOptions();

        List<String> options = MengineUtils.splitToList(options_str, " ");

        if (options.isEmpty() == true) {
            return defaultValue;
        }

        String hyphen_option = "-" + option;
        String double_hyphen_option = "--" + option;

        String hyphen_option_value = hyphen_option + ":";
        String double_hyphen_option_value = double_hyphen_option + ":";

        for(String o : options) {
            if (o.equals(hyphen_option) == true) {
                MengineLog.logSingleWarning(TAG, "option [%s] has no value", option);

                return defaultValue;
            }

            if (o.equals(double_hyphen_option) == true) {
                MengineLog.logSingleWarning(TAG, "option [%s] has no value", option);

                return defaultValue;
            }

            if (o.startsWith(hyphen_option_value) == true) {
                String value = o.substring(hyphen_option_value.length());

                try {
                    return Long.parseLong(value);
                } catch (final NumberFormatException e) {
                    MengineLog.logSingleWarning(TAG, "option [%s] invalid long value [%s]", option, value);

                    return defaultValue;
                }
            }

            if (o.startsWith(double_hyphen_option_value) == true) {
                String value = o.substring(double_hyphen_option_value.length());

                try {
                    return Long.parseLong(value);
                } catch (final NumberFormatException e) {
                    MengineLog.logSingleWarning(TAG, "option [%s] invalid long value [%s]", option, value);

                    return defaultValue;
                }
            }
        }

        return defaultValue;
    }

    static public String getOptionValueString(@NonNull MengineApplication application, String option, String defaultValue) {
        if (BuildConfig.DEBUG == false) {
            return defaultValue;
        }

        String options_str = application.getApplicationOptions();

        List<String> options = MengineUtils.splitToList(options_str, " ");

        if (options.isEmpty() == true) {
            return defaultValue;
        }

        String hyphen_option = "-" + option;
        String double_hyphen_option = "--" + option;

        String hyphen_option_value = hyphen_option + ":";
        String double_hyphen_option_value = double_hyphen_option + ":";

        for (String o : options) {
            if (o.equals(hyphen_option) == true) {
                MengineLog.logSingleWarning(TAG, "option [%s] has no value", option);

                return defaultValue;
            }

            if (o.equals(double_hyphen_option) == true) {
                MengineLog.logSingleWarning(TAG, "option [%s] has no value", option);

                return defaultValue;
            }

            if (o.startsWith(hyphen_option_value) == true) {
                String value = o.substring(hyphen_option_value.length());
                return value;
            }

            if (o.startsWith(double_hyphen_option_value) == true) {
                String value = o.substring(double_hyphen_option_value.length());
                return value;
            }
        }

        return defaultValue;
    }

    static public String getPrintDeviceInfo() {
        String manufacturer = Build.MANUFACTURER;
        String model = Build.MODEL;
        String brand = Build.BRAND;
        String device = Build.DEVICE;
        String hardware = Build.HARDWARE;
        String product = Build.PRODUCT;
        String board = Build.BOARD;
        String androidVersion = Build.VERSION.RELEASE;
        int sdkInt = Build.VERSION.SDK_INT;
        String id = Build.ID;
        String fingerprint = Build.FINGERPRINT;

        StringBuilder deviceInfo = new StringBuilder();

        String lineSeparator = System.lineSeparator();

        deviceInfo.append("=== Device Info ===").append(lineSeparator);
        deviceInfo.append("Manufacturer: ").append(manufacturer).append(lineSeparator);
        deviceInfo.append("Model: ").append(model).append(lineSeparator);
        deviceInfo.append("Brand: ").append(brand).append(lineSeparator);
        deviceInfo.append("Device: ").append(device).append(lineSeparator);
        deviceInfo.append("Hardware: ").append(hardware).append(lineSeparator);
        deviceInfo.append("Product: ").append(product).append(lineSeparator);
        deviceInfo.append("Board: ").append(board).append(lineSeparator);
        deviceInfo.append("Android Version: ").append(androidVersion).append(lineSeparator);
        deviceInfo.append("SDK Int: ").append(sdkInt).append(lineSeparator);
        deviceInfo.append("ID: ").append(id).append(lineSeparator);
        deviceInfo.append("Fingerprint: ").append(fingerprint).append(lineSeparator);
        deviceInfo.append("===================");

        return deviceInfo.toString();
    }

    static public Bundle getMetaDataBundle(@NonNull MengineApplication application) {
        Context context = application.getApplicationContext();

        String packageName = context.getPackageName();
        PackageManager packageManager = context.getPackageManager();

        try {
            ApplicationInfo ai = MengineUtils.getPackageApplicationInfo(packageManager, packageName);

            Bundle bundle = ai.metaData;

            return bundle;
        } catch (final PackageManager.NameNotFoundException e) {
            MengineLog.logError(TAG, "[ERROR] Unable to load meta-data: %s"
                , e.getMessage()
            );
        }

        return null;
    }

    static public boolean hasMetaData(@NonNull MengineApplication application, String name) {
        Bundle bundle = MengineUtils.getMetaDataBundle(application);

        if (bundle == null) {
            return false;
        }

        if (bundle.containsKey(name) == false) {
            return false;
        }

        return true;
    }

    static private Bundle assertMetaDataBundle(@NonNull MengineApplication application, String name) throws RuntimeException {
        Bundle bundle = MengineUtils.getMetaDataBundle(application);

        if (bundle == null) {
            String msg = String.format(Locale.US, "invalid get meta data bundle for [%s]", name);

            MengineUtils.throwRuntimeException(msg, null);
        }

        if (bundle.containsKey(name) == false) {
            String msg = String.format(Locale.US, "invalid setup meta data [%s]", name);

            MengineUtils.throwRuntimeException(msg, null);
        }

        return bundle;
    }

    static public String getMetaDataString(@NonNull MengineApplication application, String name) {
        Bundle bundle = MengineUtils.assertMetaDataBundle(application, name);

        String value = bundle.getString(name);

        if (value == null) {
            String msg = String.format(Locale.US, "invalid setup meta data [%s]", name);

            MengineUtils.throwRuntimeException(msg, null);
        }

        return value;
    }

    static public boolean getMetaDataBoolean(@NonNull MengineApplication application, String name) {
        Bundle bundle = MengineUtils.assertMetaDataBundle(application, name);

        boolean value = bundle.getBoolean(name);

        return value;
    }

    static public int getMetaDataInteger(@NonNull MengineApplication application, String name) {
        Bundle bundle = MengineUtils.assertMetaDataBundle(application, name);

        int value = bundle.getInt(name);

        return value;
    }

    static public long getMetaDataLong(@NonNull MengineApplication application, String name) {
        Bundle bundle = MengineUtils.assertMetaDataBundle(application, name);

        long value = bundle.getLong(name);

        return value;
    }
}
