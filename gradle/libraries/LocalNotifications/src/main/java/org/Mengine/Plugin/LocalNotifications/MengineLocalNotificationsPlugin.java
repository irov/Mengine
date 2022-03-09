package org.Mengine.Plugin.LocalNotifications;

import org.Mengine.Base.MenginePlugin;
import org.Mengine.Base.MengineActivity;

import androidx.annotation.RequiresApi;
import androidx.core.app.NotificationCompat;

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
import android.graphics.Color;
import android.os.Build;
import android.os.PersistableBundle;
import android.os.SystemClock;
import android.os.Bundle;

public class MengineLocalNotificationsPlugin extends MenginePlugin {
    private static final String CHANNEL_ID = "mengine_channel_id";
    private static final CharSequence CHANNEL_NAME = "Mengine Channel";

    @Override
    public void onPythonEmbedding(MengineActivity activity) {
        this.addPythonPlugin("LocalNotifications");
    }

    @Override
    public void onCreate(MengineActivity activity, Bundle savedInstanceState) {
    }

    @Override
    public void onNewIntent(MengineActivity activity, Intent intent) {
        if (intent.hasExtra(NotificationPublisher.NOTIFICATION_ID)) {
            int id = intent.getIntExtra(NotificationPublisher.NOTIFICATION_ID, 0);

            this.pythonCall("onLocalNotificationsPress", id);
        }
    }

    @RequiresApi(api = Build.VERSION_CODES.LOLLIPOP)
    public void scheduleJobServiceNotification(int id, String title, String content, int delay) {
        this.scheduleJobNotification(delay, NotificationJobService.notificationBundle(id, title, content));
    }

    public void scheduleNotification(Notification notification, int id, int delay) {
        MengineActivity activity = this.getActivity();
        
        Intent notificationIntent = new Intent(activity, NotificationPublisher.class);

        notificationIntent.putExtra(NotificationPublisher.NOTIFICATION_ID, id);
        notificationIntent.putExtra(NotificationPublisher.NOTIFICATION, notification);
        PendingIntent pendingIntent = PendingIntent.getBroadcast(activity, 0, notificationIntent, PendingIntent.FLAG_UPDATE_CURRENT);

        long futureInMillis = SystemClock.elapsedRealtime() + delay;
        AlarmManager alarmManager = (AlarmManager)activity.getSystemService(Context.ALARM_SERVICE);
        alarmManager.set(AlarmManager.ELAPSED_REALTIME_WAKEUP, futureInMillis, pendingIntent);
    }

    public void instantlyPresentNotification(Notification notification, int id) {
        MengineActivity activity = this.getActivity();
        
        NotificationManager notificationManager = (NotificationManager)activity.getSystemService(Context.NOTIFICATION_SERVICE);
        notificationManager.notify(id, notification);
    }
    
    public void run() {
        MengineActivity activity = this.getActivity();
        
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            NotificationManager notificationManager = (NotificationManager)activity.getSystemService(Context.NOTIFICATION_SERVICE);

            int importance = NotificationManager.IMPORTANCE_HIGH;
            NotificationChannel notificationChannel = new NotificationChannel(CHANNEL_ID, CHANNEL_NAME, importance);
            notificationChannel.enableLights(true);
            notificationChannel.setLightColor(Color.RED);
            notificationChannel.enableVibration(true);
            notificationChannel.setVibrationPattern(new long[]{1000, 2000});
            notificationManager.createNotificationChannel(notificationChannel);
        }
        
        if (activity.getIntent().hasExtra(NotificationPublisher.NOTIFICATION_ID)) {
            int id = activity.getIntent().getIntExtra(NotificationPublisher.NOTIFICATION_ID, 0);
        
            this.pythonCall("onLocalNotificationsPress", id);
        }
    }
    
    public void cancelAll() {
        MengineActivity activity = this.getActivity();
        
        NotificationManager notificationManager = (NotificationManager)activity.getSystemService(Context.NOTIFICATION_SERVICE);
        notificationManager.cancelAll();
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
            JobScheduler jobScheduler = (JobScheduler)activity.getSystemService(Context.JOB_SCHEDULER_SERVICE);
            jobScheduler.cancelAll();
        }
    }

    public static Notification getNotification(Context context, int id, String title, String content) {
        Intent intent = new Intent(context, MengineActivity.class);
        intent.setFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP | Intent.FLAG_ACTIVITY_SINGLE_TOP);
        intent.putExtra(NotificationPublisher.NOTIFICATION_ID, id);
        PendingIntent pendingIntent = PendingIntent.getActivity(context, 0, intent, PendingIntent.FLAG_UPDATE_CURRENT);

        NotificationCompat.Builder builder = new NotificationCompat.Builder(context, CHANNEL_ID);

        return builder.setContentTitle(title)
                .setContentText(content)
                .setSmallIcon(R.drawable.ic_stat_onesignal_default)
                .setColor(Color.parseColor("#422b00"))  // todo: take out in xml
                .setDefaults(NotificationCompat.DEFAULT_ALL)
                .setContentIntent(pendingIntent)
                .setPriority(NotificationCompat.PRIORITY_MAX)
                .build();
    }

    @RequiresApi(api = Build.VERSION_CODES.LOLLIPOP)
    private void scheduleJobNotification(int delayMillis, PersistableBundle bundle){
        this.log("Schedule notification with delay: %u", delayMillis);
        
        MengineActivity activity = this.getActivity();

        JobInfo.Builder builder = new JobInfo.Builder((int)SystemClock.elapsedRealtime(), new ComponentName(activity, NotificationJobService.class));
        JobInfo jobInfo = builder
                .setMinimumLatency(delayMillis)
                .setOverrideDeadline(delayMillis + 10000)
                .setExtras(bundle)
                .build();
        JobScheduler jobScheduler = (JobScheduler)activity.getSystemService(Context.JOB_SCHEDULER_SERVICE);
        jobScheduler.schedule(jobInfo);
    }
}