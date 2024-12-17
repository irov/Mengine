package org.Mengine.Base;

import android.app.Activity;
import android.app.ActivityManager;
import android.app.AlertDialog;
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
import android.graphics.Typeface;
import android.net.Uri;
import android.os.BatteryManager;
import android.os.Build;
import android.os.Handler;
import android.os.LocaleList;
import android.os.Looper;
import android.text.Layout;
import android.text.Spannable;
import android.text.SpannableString;
import android.text.style.AlignmentSpan;
import android.text.style.StyleSpan;
import android.util.DisplayMetrics;
import android.view.Display;
import android.view.WindowManager;
import android.view.WindowMetrics;
import android.widget.Toast;

import androidx.annotation.NonNull;
import androidx.core.content.ContextCompat;
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
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.nio.charset.StandardCharsets;
import java.security.SecureRandom;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.HashMap;
import java.util.Iterator;
import java.util.List;
import java.util.ArrayList;
import java.util.Locale;
import java.util.Map;
import java.util.Timer;
import java.util.TimerTask;
import java.util.UUID;
import java.util.zip.ZipEntry;
import java.util.zip.ZipOutputStream;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.ParserConfigurationException;

public class MengineUtils {
    public static final String TAG = "MengineUtils";

    public static Class<?> getClazz(String TAG, String name, boolean required) {
        ClassLoader cl = MengineActivity.class.getClassLoader();

        if (cl == null) {
            if (required == true) {
                MengineLog.logError(TAG, "[ERROR] invalid create new instance: %s ClassLoader is null"
                    , name
                );
            }

            return null;
        }

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
        } catch (final NoSuchMethodException e) {
            MengineLog.logError(TAG, "[ERROR] invalid create new instance: %s NoSuchMethodException: %s"
                , name
                , e.getMessage()
            );
        } catch (final IllegalAccessException e) {
            MengineLog.logError(TAG, "[ERROR] invalid create new instance: %s IllegalAccessException: %s"
                , name
                , e.getMessage()
            );
        } catch (final InstantiationException e) {
            MengineLog.logError(TAG, "[ERROR] invalid create new instance: %s InstantiationException: %s"
                , name
                , e.getMessage()
            );
        } catch (final InvocationTargetException e) {
            MengineLog.logError(TAG, "[ERROR] invalid create new instance: %s InvocationTargetException: %s"
                , name
                , e.getMessage()
            );
        } catch (final NullPointerException e) {
            MengineLog.logError(TAG, "[ERROR] invalid create new instance: %s NullPointerException: %s"
                , name
                , e.getMessage()
            );
        }

        return null;
    }

    public static Handler performOnMainThread(Runnable runnable) {
        Looper mainLooper = Looper.getMainLooper();

        if (Looper.myLooper() == mainLooper) {
            runnable.run();

            return null;
        }

        Handler handler = new Handler(mainLooper);

        handler.post(runnable);

        return handler;
    }

    public static Handler performOnMainThreadDelayed(Runnable runnable, long delayMillis) {
        Looper mainLooper = Looper.getMainLooper();
        Handler handler = new Handler(mainLooper);

        handler.postDelayed(runnable, delayMillis);

        return handler;
    }

    public static Timer scheduleAtFixedRate(long delay, long period, Runnable runnable) {
        Timer timer = new Timer();

        TimerTask task = new TimerTask() {
            @Override
            public void run() {
                runnable.run();
            }
        };

        timer.scheduleAtFixedRate(task, delay, period);

        return timer;
    }

    public static Timer scheduleOnUiAtFixedRate(Activity activity, long delay, long period, Runnable runnable) {
        Timer timer = new Timer();

        TimerTask task = new TimerTask() {
            @Override
            public void run() {
                activity.runOnUiThread(runnable);
            }
        };

        timer.scheduleAtFixedRate(task, delay, period);

        return timer;
    }

    public static void makeToastDelayed(Context context, String text, long delayed) {
        MengineLog.logMessage(TAG, "Toast show after %d milliseconds text: %s"
            , delayed
            , text
        );

        MengineUtils.performOnMainThreadDelayed(() -> {
            Toast toast = Toast.makeText(context, text, Toast.LENGTH_LONG);
            toast.show();
        }, delayed);
    }

    public static Object getDebugValue(Object d, Object r) {
        if (BuildConfig.DEBUG == true) {
            return d;
        } else {
            return r;
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
        BatteryManager bm = (BatteryManager)context.getSystemService(Context.BATTERY_SERVICE);

        int batteryLevel = bm.getIntProperty(BatteryManager.BATTERY_PROPERTY_CAPACITY);

        return batteryLevel;
    }

    private static int getBatteryStatus(Context context) {
        if (android.os.Build.VERSION.SDK_INT < android.os.Build.VERSION_CODES.O) {
            return BatteryManager.BATTERY_STATUS_UNKNOWN;
        }

        BatteryManager bm = (BatteryManager)context.getSystemService(Context.BATTERY_SERVICE);

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

    public static long getRamTotal(Context context) {
        ActivityManager activityManager = (ActivityManager)context.getSystemService(Context.ACTIVITY_SERVICE);

        ActivityManager.MemoryInfo memInfo = new ActivityManager.MemoryInfo();
        activityManager.getMemoryInfo(memInfo);

        long totalMemory = memInfo.totalMem;

        return totalMemory;
    }

    public static long getRamUsage(Context context) {
        ActivityManager activityManager = (ActivityManager)context.getSystemService(Context.ACTIVITY_SERVICE);

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

            if( sourceFileParent == null ) {
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

    public static String getLastPathComponent(String filePath) {
        String[] segments = filePath.split("/");

        if (segments.length == 0) {
            return "";
        }

        String lastPathComponent = segments[segments.length - 1];

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
        SimpleDateFormat df = new SimpleDateFormat(format, Locale.US);
        Date date = new Date(timestamp);
        String s = df.format(date);

        return s;
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

        ActivityManager manager = (ActivityManager)context.getSystemService(Context.ACTIVITY_SERVICE);
        List<ActivityManager.RunningAppProcessInfo> infos = manager.getRunningAppProcesses();

        if (infos == null) {
            return null;
        }

        for(ActivityManager.RunningAppProcessInfo info : infos) {
            if (info.pid != mypid) {
                continue;
            }

            return info.processName;
        }

        return null;
    }

    public static Document parseDocument(InputStream stream) {
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

    public static boolean openUrl(Context context, String url) {
        Uri uri = Uri.parse(url);
        Intent intent = new Intent(Intent.ACTION_VIEW, uri);
        intent.addCategory(Intent.CATEGORY_BROWSABLE);

        try {
            context.startActivity(intent);
        } catch (final ActivityNotFoundException e) {
            MengineLog.logWarning(TAG, "openUrl url: %s catch ActivityNotFoundException: %s"
                , url
                , e.getMessage()
            );

            return false;
        }

        return true;
    }

    public static void showToast(Activity activity, String format, Object ... args) {
        activity.runOnUiThread(() -> {
            String message = MengineLog.buildTotalMsg(format, args);

            Toast toast = Toast.makeText(activity, message, Toast.LENGTH_SHORT);

            toast.show();
        });
    }

    public static void showOkAlertDialog(Activity activity, Runnable ok, String title, String format, Object ... args) {
        activity.runOnUiThread(() -> {
            String message = MengineLog.buildTotalMsg(format, args);

            MengineLog.logMessage(TAG, "show OK alert dialog title: %s message: %s"
                , title
                , message
            );

            AlertDialog.Builder builder = new AlertDialog.Builder(activity);

            builder.setTitle(title);
            builder.setMessage(message);
            builder.setCancelable(false);
            builder.setPositiveButton("OK", (dialog, which) -> {
                MengineLog.logMessage(TAG, "show OK alert dialog OK clicked");

                ok.run();
                dialog.dismiss();
            });

            AlertDialog alert = builder.create();

            alert.show();
        });
    }

    public static void showAreYouSureAlertDialog(Activity activity, Runnable yes, Runnable cancel, long delayMillis, String title, String format, Object ... args) {
        activity.runOnUiThread(() -> {
            String messageText = MengineLog.buildTotalMsg(format, args);

            MengineLog.logMessage(TAG, "show YES|CANCEL alert dialog title: %s message: %s"
                , title
                , messageText
            );

            String AreYouSureText = "\n\nAre you sure?";

            SpannableString spannableMessage = new SpannableString(messageText + AreYouSureText);

            spannableMessage.setSpan(new StyleSpan(Typeface.BOLD), messageText.length(), (messageText + AreYouSureText).length(), Spannable.SPAN_EXCLUSIVE_EXCLUSIVE);
            spannableMessage.setSpan(new AlignmentSpan.Standard(Layout.Alignment.ALIGN_OPPOSITE), messageText.length(), spannableMessage.length(), Spannable.SPAN_EXCLUSIVE_EXCLUSIVE);

            AlertDialog.Builder builder = new AlertDialog.Builder(activity);

            builder.setTitle(title);
            builder.setMessage(spannableMessage);
            builder.setCancelable(false);
            builder.setPositiveButton("YES", (dialog, which) -> {
                MengineLog.logMessage(TAG, "show YES|CANCEL alert dialog YES clicked");

                yes.run();
                dialog.dismiss();
            });
            builder.setNegativeButton("CANCEL", (dialog, id) -> {
                MengineLog.logMessage(TAG, "show YES|CANCEL alert dialog CANCEL clicked");

                cancel.run();
                dialog.dismiss();
            });

            AlertDialog alert = builder.create();

            alert.show();

            int darker_gray = ContextCompat.getColor(activity, android.R.color.darker_gray);
            int holo_red_light = ContextCompat.getColor(activity, android.R.color.holo_red_light);

            if (delayMillis > 0) {
                alert.getButton(AlertDialog.BUTTON_POSITIVE).setTextColor(darker_gray);
                alert.getButton(AlertDialog.BUTTON_POSITIVE).setEnabled(false);

                MengineUtils.performOnMainThreadDelayed(() -> {
                    alert.getButton(AlertDialog.BUTTON_POSITIVE).setTextColor(holo_red_light);
                    alert.getButton(AlertDialog.BUTTON_POSITIVE).setEnabled(true);
                }, delayMillis);
            } else {
                alert.getButton(AlertDialog.BUTTON_POSITIVE).setTextColor(holo_red_light);
            }
        });
    }

    public static void finishActivityWithAlertDialog(Activity activity, String format, Object... args) {
        MengineLog.logErrorException(TAG, format, args);

        MengineUtils.showOkAlertDialog(activity, () -> {
            activity.finishAndRemoveTask();
        }, "Mengine", format, args);
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

    public static Map<String, Object> parseJSONMap(String json) {
        try {
            JSONObject jsonObject = new JSONObject(json);

            Map<String, Object> map = MengineUtils.jsonObjectToMap(jsonObject);

            return map;
        } catch (final JSONException e) {
            MengineLog.logError(TAG, "parseJSONMap json: %s JSONException: %s"
                , json
                , e.getMessage()
            );
        }

        return null;
    }

    private static Map<String, Object> jsonObjectToMap(JSONObject obj)  throws JSONException {
        Map<String, Object> map = new HashMap<>();

        Iterator<String> keys = obj.keys();
        while (keys.hasNext() == true) {
            String key = keys.next();
            Object value = obj.get(key);
            Object parsedValue = MengineUtils.parseJsonValue(value);
            map.put(key, parsedValue);
        }

        return map;
    }

    private static List<Object> jsonArrayToList(JSONArray array) throws JSONException {
        List<Object> list = new ArrayList<>();

        int array_length = array.length();

        for (int index = 0; index != array_length; index++) {
            Object value = array.get(index);
            Object parsedValue = MengineUtils.parseJsonValue(value);
            list.add(parsedValue);
        }

        return list;
    }

    private static Object parseJsonValue(Object value) throws JSONException {
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
            WindowManager windowManager = (WindowManager)context.getSystemService(Context.WINDOW_SERVICE);
            return windowManager.getDefaultDisplay();
        }
    }

    @SuppressWarnings("deprecation")
    public static Rect getDeviceWindowRect(Context context, Display display)
    {
        if (android.os.Build.VERSION.SDK_INT >= android.os.Build.VERSION_CODES.R) {
            WindowManager windowManager = (WindowManager)context.getSystemService(Context.WINDOW_SERVICE);
            WindowMetrics windowMetrics = windowManager.getCurrentWindowMetrics();

            Rect bounds = windowMetrics.getBounds();

            return bounds;
        } else {
            DisplayMetrics realMetrics = new DisplayMetrics();
            display.getRealMetrics( realMetrics );

            int width = realMetrics.widthPixels;
            int height = realMetrics.heightPixels;

            return new Rect(0, 0, width, height);
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
        int screenLayoutSize = MengineUtils.getDeviceScreenSize(context);

        if (screenLayoutSize == Configuration.SCREENLAYOUT_SIZE_LARGE || screenLayoutSize == Configuration.SCREENLAYOUT_SIZE_XLARGE) {
            return true;
        }

        return false;
    }

    public static boolean isDebuggerConnected() {
        return android.os.Debug.isDebuggerConnected();
    }

    public static boolean isAppInForeground(Context context) {
        ActivityManager activityManager = (ActivityManager)context.getSystemService(Context.ACTIVITY_SERVICE);
        List<ActivityManager.RunningAppProcessInfo> appProcesses = activityManager.getRunningAppProcesses();

        if (appProcesses == null) {
            return false;
        }

        String packageName = context.getPackageName();

        for (ActivityManager.RunningAppProcessInfo appProcess : appProcesses) {
            if (appProcess.importance != ActivityManager.RunningAppProcessInfo.IMPORTANCE_FOREGROUND) {
                continue;
            }

            if (appProcess.processName.equals(packageName) == false) {
                continue;
            }

            return true;
        }

        return false;
    }
}
