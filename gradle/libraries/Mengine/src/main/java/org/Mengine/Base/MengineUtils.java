package org.Mengine.Base;

import android.app.Activity;
import android.app.ActivityManager;
import android.app.AlertDialog;
import android.content.ActivityNotFoundException;
import android.content.ComponentName;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.net.ConnectivityManager;
import android.net.Network;
import android.net.NetworkCapabilities;
import android.net.Uri;
import android.os.BatteryManager;
import android.os.Handler;
import android.os.Looper;
import android.widget.Toast;

import androidx.core.content.FileProvider;

import org.w3c.dom.Document;
import org.xml.sax.SAXException;

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.PrintWriter;
import java.io.StringWriter;
import java.lang.reflect.Constructor;
import java.lang.reflect.InvocationTargetException;
import java.security.SecureRandom;
import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Date;
import java.util.List;
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

    public static void makeToastLong(Context context, String text, long delayed) {
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

    public static int getConectivityStatus(Context context) {
        if (android.os.Build.VERSION.SDK_INT < android.os.Build.VERSION_CODES.M) {
            return -2;
        }

        ConnectivityManager connectivityManager = (ConnectivityManager)context.getSystemService(Context.CONNECTIVITY_SERVICE);

        Network network = connectivityManager.getActiveNetwork();

        if (network == null) {
            return -1;
        }

        NetworkCapabilities capabilities = connectivityManager.getNetworkCapabilities(network);

        if (capabilities == null) {
            return -1;
        }

        if (capabilities.hasTransport(NetworkCapabilities.TRANSPORT_CELLULAR) == true) {
            return 1;
        }

        if (capabilities.hasTransport(NetworkCapabilities.TRANSPORT_WIFI) == true) {
            return 2;
        }

        if (capabilities.hasTransport(NetworkCapabilities.TRANSPORT_BLUETOOTH) == true) {
            return 3;
        }

        if (capabilities.hasTransport(NetworkCapabilities.TRANSPORT_ETHERNET) == true) {
            return 4;
        }

        if (capabilities.hasTransport(NetworkCapabilities.TRANSPORT_VPN) == true) {
            return 5;
        }

        if (capabilities.hasTransport(NetworkCapabilities.TRANSPORT_WIFI_AWARE) == true) {
            return 6;
        }

        if (capabilities.hasTransport(NetworkCapabilities.TRANSPORT_LOWPAN) == true) {
            return 7;
        }

        if (capabilities.hasTransport(NetworkCapabilities.TRANSPORT_USB) == true) {
            return 8;
        }

        return 0;
    }

    private static void zipFile(ZipOutputStream out, File file, int basePathLength) throws IOException {
        final int BUFFER = 2048;

        byte data[] = new byte[BUFFER];
        String unmodifiedFilePath = file.getPath();
        String relativePath = unmodifiedFilePath.substring(basePathLength);
        FileInputStream fi = new FileInputStream(unmodifiedFilePath);
        BufferedInputStream origin = new BufferedInputStream(fi, BUFFER);
        ZipEntry entry = new ZipEntry(relativePath);
        long modified = file.lastModified();
        entry.setTime(modified);
        out.putNextEntry(entry);

        int count;
        while ((count = origin.read(data, 0, BUFFER)) != -1) {
            out.write(data, 0, count);
        }

        origin.close();
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
        SimpleDateFormat df = new SimpleDateFormat(format);
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
            MengineLog.logError("[Banner] parseDocument catch ParserConfigurationException: %s", e.getMessage());
        } catch (SAXException e) {
            MengineLog.logError("[Banner] parseDocument catch SAXException: %s", e.getMessage());
        } catch (IOException e) {
            MengineLog.logError("[Banner] parseDocument catch IOException: %s", e.getMessage());
        }

        return null;
    }

    public static boolean openUrl(Context context, String url) {
        Uri uri = Uri.parse(url);
        Intent intent = new Intent(Intent.ACTION_VIEW, uri);

        PackageManager packageManager = context.getPackageManager();

        ComponentName componentName = intent.resolveActivity(packageManager);

        if (componentName == null) {
            MengineLog.logWarning(TAG, "openUrl url: %s not found activity"
                , url
            );

            return false;
        }

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

    public static void startChainActivity(Activity activity, String className, ArrayList<String> activities) throws MengineChainActivityNotFoundException {
        Class<?> intentClazz = MengineUtils.getClazz(TAG, className, true);

        if (intentClazz == null) {
            throw new MengineChainActivityNotFoundException(className, "not found chain activity class");
        }

        Intent intent = new Intent(activity, intentClazz);
        intent.putStringArrayListExtra("MENGINE_NEXT_ACTIVITIES", activities);
        activity.startActivity(intent);
        activity.finish();
    }

    public static void nextChainActivity(Activity activity) throws MengineChainActivityNotFoundException {
        Intent intent = activity.getIntent();

        ArrayList<String> activities = intent.getStringArrayListExtra("MENGINE_NEXT_ACTIVITIES");

        String nextClassName = activities.get(0);
        activities.remove(0);

        MengineUtils.startChainActivity(activity, nextClassName, activities);
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
        MengineLog.logError(TAG, format, args);

        MengineUtils.performOnMainThreadDelayed(() -> {
            MengineUtils.showAlertDialog(activity, new Runnable() {
                @Override
                public void run() {
                    activity.finish();
                }
            }, format, args);
        }, 0);
    }
}
