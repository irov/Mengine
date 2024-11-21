package org.Mengine.Plugin.LocalNotifications;

import org.Mengine.Base.MenginePlugin;
import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.MenginePluginActivityListener;

import androidx.activity.result.ActivityResultLauncher;
import androidx.activity.result.contract.ActivityResultContracts;
import androidx.annotation.RequiresApi;
import androidx.core.app.ActivityCompat;
import androidx.core.app.NotificationCompat;
import androidx.core.content.ContextCompat;

import android.Manifest;
import android.app.AlarmManager;
import android.app.Notification;
import android.app.NotificationChannel;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.app.job.JobInfo;
import android.app.job.JobScheduler;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.graphics.Color;
import android.os.Build;
import android.os.PersistableBundle;
import android.os.SystemClock;
import android.os.Bundle;

public class MengineLocalNotificationsPlugin extends MenginePlugin implements MenginePluginActivityListener {
    public static final String PLUGIN_NAME = "MengineLNotifications";
    public static final boolean PLUGIN_EMBEDDING = true;

    private static final String CHANNEL_ID = "mengine_channel_id";
    private static final CharSequence CHANNEL_NAME = "Mengine Channel";

    private boolean m_notificationPermission = false;

    @Override
    public void onCreate(MengineActivity activity, Bundle savedInstanceState) {
        activity.checkPermission(Manifest.permission.POST_NOTIFICATIONS, () -> {
            MengineLocalNotificationsPlugin.this.startLocalNotifications(activity);
        }, () -> {
            MengineLocalNotificationsPlugin.this.logMessage("[POST_NOTIFICATIONS] permission denied");
        });
    }

    @Override
    public void onNewIntent(MengineActivity activity, Intent intent) {
        if (m_notificationPermission == false) {
            return;
        }

        if (intent.hasExtra(MengineLocalNotificationPublisher.NOTIFICATION_ID) == false) {
            return;
        }

        int id = intent.getIntExtra(MengineLocalNotificationPublisher.NOTIFICATION_ID, 0);

        this.logMessage("onNewIntent has notification: %d"
            , id
        );

        this.pythonCall("onLocalNotificationsPress", id);
    }

    public void scheduleJobServiceNotification(int id, String title, String content, long delay) {
        this.logMessage("scheduleJobServiceNotification id: %d title: %s content: %s delay: %d"
            , id
            , title
            , content
            , delay
        );

        if (m_notificationPermission == false) {
            return;
        }

        PersistableBundle bundle = MengineLocalNotificationJobService.notificationBundle(id, title, content);

        this.scheduleJobNotification(delay, bundle);
    }

    public void scheduleNotification(Notification notification, int id, long delay) {
        this.logMessage("scheduleNotification id: %d delay: %d"
            , id
            , delay
        );

        if (m_notificationPermission == false) {
            return;
        }

        MengineActivity activity = this.getMengineActivity();
        
        Intent notificationIntent = new Intent(activity, MengineLocalNotificationPublisher.class);

        notificationIntent.putExtra(MengineLocalNotificationPublisher.NOTIFICATION_ID, id);
        notificationIntent.putExtra(MengineLocalNotificationPublisher.NOTIFICATION, notification);

        int pendingFlags = PendingIntent.FLAG_UPDATE_CURRENT;

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            pendingFlags |= PendingIntent.FLAG_IMMUTABLE;
        }

        PendingIntent pendingIntent = PendingIntent.getBroadcast(activity, 0, notificationIntent, pendingFlags);

        long futureInMillis = SystemClock.elapsedRealtime() + delay;
        AlarmManager alarmManager = (AlarmManager)activity.getSystemService(Context.ALARM_SERVICE);
        alarmManager.set(AlarmManager.ELAPSED_REALTIME_WAKEUP, futureInMillis, pendingIntent);
    }

    public void instantlyPresentNotification(Notification notification, int id) {
        this.logMessage("instantlyPresentNotification id: %d"
            , id
        );

        if (m_notificationPermission == false) {
            return;
        }

        MengineActivity activity = this.getMengineActivity();
        
        NotificationManager notificationManager = (NotificationManager)activity.getSystemService(Context.NOTIFICATION_SERVICE);
        notificationManager.notify(id, notification);
    }
    
    public void startLocalNotifications(MengineActivity activity) {
        this.cancelAll();

        m_notificationPermission = true;

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            NotificationManager notificationManager = (NotificationManager)activity.getSystemService(Context.NOTIFICATION_SERVICE);

            int importance = NotificationManager.IMPORTANCE_HIGH;
            NotificationChannel channel = new NotificationChannel(CHANNEL_ID, CHANNEL_NAME, importance);
            channel.enableLights(true);
            channel.setLightColor(Color.RED);
            channel.enableVibration(true);
            channel.setVibrationPattern(new long[]{1000, 2000});
            notificationManager.createNotificationChannel(channel);
        }

        Intent intent = activity.getIntent();

        if (intent.hasExtra(MengineLocalNotificationPublisher.NOTIFICATION_ID)) {
            int id = intent.getIntExtra(MengineLocalNotificationPublisher.NOTIFICATION_ID, 0);

            this.logMessage("start with notification: %d"
                , id
            );

            this.pythonCall("onLocalNotificationsPress", id);
        }
    }
    
    public void cancelAll() {
        this.logMessage("cancelAll");

        MengineActivity activity = this.getMengineActivity();
        
        NotificationManager notificationManager = (NotificationManager)activity.getSystemService(Context.NOTIFICATION_SERVICE);
        notificationManager.cancelAll();

        JobScheduler jobScheduler = (JobScheduler)activity.getSystemService(Context.JOB_SCHEDULER_SERVICE);
        jobScheduler.cancelAll();
    }

    @RequiresApi(api = Build.VERSION_CODES.M)
    public static Notification getNotification(Context context, int id, String title, String content) {
        Intent intent = new Intent(context, MengineActivity.class);
        intent.setFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP | Intent.FLAG_ACTIVITY_SINGLE_TOP);
        intent.putExtra(MengineLocalNotificationPublisher.NOTIFICATION_ID, id);

        int pendingFlags = PendingIntent.FLAG_UPDATE_CURRENT;
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            pendingFlags |= PendingIntent.FLAG_IMMUTABLE;
        }

        PendingIntent pendingIntent = PendingIntent.getActivity(context, 0, intent, pendingFlags);

        NotificationCompat.Builder builder = new NotificationCompat.Builder(context, CHANNEL_ID);

        return builder.setContentTitle(title)
            .setContentText(content)
            .setSmallIcon(R.drawable.mengine_local_notification_small_icon)
            .setColor(ContextCompat.getColor(context, R.color.mengine_local_notification_color))  // <color name="mengine_local_notification_color">#422b00</color>
            .setDefaults(NotificationCompat.DEFAULT_ALL)
            .setContentIntent(pendingIntent)
            .setPriority(NotificationCompat.PRIORITY_MAX)
            .build();
    }

    private void scheduleJobNotification(long delayMillis, PersistableBundle bundle){
        this.logMessage("schedule notification with delay: %d", delayMillis);

        if (m_notificationPermission == false) {
            return;
        }
        
        MengineActivity activity = this.getMengineActivity();

        int jobId = (int)SystemClock.elapsedRealtime();
        ComponentName jobService = new ComponentName(activity, MengineLocalNotificationJobService.class);

        JobInfo.Builder builder = new JobInfo.Builder(jobId, jobService);
        JobInfo jobInfo = builder
            .setMinimumLatency(delayMillis)
            .setOverrideDeadline(delayMillis + 10000)
            .setExtras(bundle)
            .build();

        JobScheduler jobScheduler = (JobScheduler)activity.getSystemService(Context.JOB_SCHEDULER_SERVICE);
        jobScheduler.schedule(jobInfo);
    }
}