package org.Mengine.Base;

import android.app.Activity;
import android.app.ActivityManager;
import android.app.AlertDialog;
import android.content.ActivityNotFoundException;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.net.Uri;
import android.os.BatteryManager;
import android.os.Handler;
import android.os.Looper;
import android.widget.Toast;

import androidx.core.content.FileProvider;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;
import org.w3c.dom.Document;
import org.xml.sax.SAXException;

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.BufferedReader;
import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.PrintWriter;
import java.io.StringWriter;
import java.lang.reflect.Constructor;
import java.lang.reflect.InvocationTargetException;
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
        } catch (ClassNotFoundException e) {
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
        } catch (NoSuchMethodException e) {
            MengineLog.logError(TAG, "[ERROR] invalid create new instance: %s NoSuchMethodException: %s"
                , name
                , e.getMessage()
            );
        } catch (IllegalAccessException e) {
            MengineLog.logError(TAG, "[ERROR] invalid create new instance: %s IllegalAccessException: %s"
                , name
                , e.getMessage()
            );
        } catch (InstantiationException e) {
            MengineLog.logError(TAG, "[ERROR] invalid create new instance: %s InstantiationException: %s"
                , name
                , e.getMessage()
            );
        } catch (InvocationTargetException e) {
            MengineLog.logError(TAG, "[ERROR] invalid create new instance: %s InvocationTargetException: %s"
                , name
                , e.getMessage()
            );
        } catch (NullPointerException e) {
            MengineLog.logError(TAG, "[ERROR] invalid create new instance: %s NullPointerException: %s"
                , name
                , e.getMessage()
            );
        }

        return null;
    }

    public static Handler performOnMainThread(Runnable runnable) {
        Looper looper = Looper.getMainLooper();

        if (Looper.myLooper() == looper) {
            runnable.run();

            return null;
        }

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
        StringBuffer sb = new StringBuffer();
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
        } catch (Exception e) {
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

        File tempFile = null;

        try {
            tempFile = File.createTempFile(prefix, suffix, cacheDir);
        } catch (IOException e) {
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
        } catch (IllegalArgumentException e) {
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
        } catch (ParserConfigurationException e) {
            MengineLog.logError(TAG, "parseDocument catch ParserConfigurationException: %s", e.getMessage());
        } catch (SAXException e) {
            MengineLog.logError(TAG, "parseDocument catch SAXException: %s", e.getMessage());
        } catch (IOException e) {
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
        } catch (ActivityNotFoundException e) {
            MengineLog.logWarning(TAG, "openUrl url: %s catch ActivityNotFoundException: %s"
                , url
                , e.getMessage()
            );

            return false;
        }

        return true;
    }

    public static void showToast(Context context, String format, Object ... args) {
        String message = MengineLog.buildTotalMsg(format, args);

        Toast toast = Toast.makeText(context, message, Toast.LENGTH_SHORT);

        toast.show();
    }

    public static void showAlertDialog(Context context, Runnable cb, String format, Object ... args) {
        String message = MengineLog.buildTotalMsg(format, args);

        MengineLog.logMessage(TAG, "show alert dialog: %s"
            , message
        );

        AlertDialog.Builder builder = new AlertDialog.Builder(context);

        builder.setMessage(message);
        builder.setCancelable(false);
        builder.setPositiveButton("OK", new DialogInterface.OnClickListener() {
            @Override
            public void onClick(DialogInterface dialog, int which) {
                cb.run();
                dialog.cancel();
            }
        });

        AlertDialog alert = builder.create();

        alert.show();
    }

    public static void finishActivityWithAlertDialog(Activity activity, String format, Object... args) {
        MengineLog.logErrorException(TAG, format, args);

        MengineUtils.performOnMainThreadDelayed(() -> {
            MengineUtils.showAlertDialog(activity, () -> {
                activity.finishAndRemoveTask();
            }, format, args);
        }, 0);
    }

    public static void sleep(long millis) {
        try {
            Thread.sleep(millis);
        } catch (InterruptedException e) {
            MengineLog.logWarning(TAG, "sleep %d catch InterruptedException: %s", millis, e.getMessage());
        }
    }

    public static String inputStreamToString(InputStream stream) throws IOException {
        StringBuilder stringBuilder = new StringBuilder();

        BufferedReader bufferedReader = new BufferedReader(new InputStreamReader(stream, StandardCharsets.UTF_8));

        String line;
        while ((line = bufferedReader.readLine()) != null) {
            stringBuilder.append(line).append("\n");
        }

        bufferedReader.close();

        return stringBuilder.toString();
    }

    public static byte [] inputStreamToByteArray(InputStream stream) throws IOException {
        byte [] buffer = new byte[1024];
        int len;
        ByteArrayOutputStream out = new ByteArrayOutputStream();
        while ((len = stream.read(buffer)) != -1) {
            out.write(buffer, 0, len);
        }
        out.close();
        return out.toByteArray();
    }

    public static Map<String, Object> parseJSONMap(String json) {
        try {
            JSONObject jsonObject = new JSONObject(json);

            Map<String, Object> map = MengineUtils.jsonObjectToMap(jsonObject);

            return map;
        } catch (JSONException e) {
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
        while (keys.hasNext()) {
            String key = keys.next();
            Object value = obj.get(key);
            Object parsedValue = MengineUtils.parseJsonValue(value);
            map.put(key, parsedValue);
        }

        return map;
    }

    private static List<Object> jsonArrayToList(JSONArray array) throws JSONException {
        List<Object> list = new ArrayList<>();

        for (int i = 0; i < array.length(); i++) {
            Object value = array.get(i);
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
        } else if (value == JSONObject.NULL ) {
            return null;
        } else {
            return value;
        }
    }
}
