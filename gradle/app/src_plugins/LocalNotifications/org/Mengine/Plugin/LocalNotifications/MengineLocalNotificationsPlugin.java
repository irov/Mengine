package org.Mengine.Plugin.LocalNotifications;

import org.Mengine.Build.MenginePlugin;

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
import android.util.Log;

import androidx.annotation.RequiresApi;
import androidx.core.app.NotificationCompat;

import org.Mengine.Build.MengineActivity;
import org.Mengine.Build.R;

public class MengineLocalNotificationsPlugin extends MenginePlugin {
    private static Context _currentContext;
    private static final String channelId = "mengine_channel_id";
    private static final CharSequence channelName = "Mengine Channel";

    public MengineLocalNotificationsPlugin(MengineActivity activity) {
        _currentContext = activity;

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            NotificationManager notificationManager = (NotificationManager) _currentContext.getSystemService(Context.NOTIFICATION_SERVICE);

            int importance = NotificationManager.IMPORTANCE_HIGH;
            NotificationChannel notificationChannel = new NotificationChannel(channelId, channelName, importance);
            notificationChannel.enableLights(true);
            notificationChannel.setLightColor(Color.RED);
            notificationChannel.enableVibration(true);
            notificationChannel.setVibrationPattern(new long[]{1000, 2000});
            notificationManager.createNotificationChannel(notificationChannel);
        }
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        //if (_instance.getIntent().hasExtra(NotificationPublisher.NOTIFICATION_ID)) {
        //    AndroidNativeLocalNotifications_onLocalNotificationsPress(_instance.getIntent().getIntExtra(NotificationPublisher.NOTIFICATION_ID, 0));
        //}
    }

    @Override
    public void onNewIntent(Intent intent) {
        //if (intent.hasExtra(NotificationPublisher.NOTIFICATION_ID)) {
        //    AndroidNativeLocalNotifications_onLocalNotificationsPress(intent.getIntExtra(NotificationPublisher.NOTIFICATION_ID, 0));
        //}
    }

    @RequiresApi(api = Build.VERSION_CODES.LOLLIPOP)
    public void scheduleJobServiceNotification(int id, String title, String content, int delay){
        scheduleJobNotification(delay, NotificationJobService.notificationBundle(id, title, content));
    }

    public void scheduleNotification(Notification notification, int id, int delay) {
        Intent notificationIntent = new Intent(_currentContext, NotificationPublisher.class);

        notificationIntent.putExtra(NotificationPublisher.NOTIFICATION_ID, id);
        notificationIntent.putExtra(NotificationPublisher.NOTIFICATION, notification);
        PendingIntent pendingIntent = PendingIntent.getBroadcast(_currentContext, 0, notificationIntent, PendingIntent.FLAG_UPDATE_CURRENT);

        long futureInMillis = SystemClock.elapsedRealtime() + delay;
        AlarmManager alarmManager = (AlarmManager) _currentContext.getSystemService(Context.ALARM_SERVICE);
        alarmManager.set(AlarmManager.ELAPSED_REALTIME_WAKEUP, futureInMillis, pendingIntent);
    }

    public void instantlyPresentNotification(Notification notification, int id) {
        NotificationManager notificationManager = (NotificationManager)_currentContext.getSystemService(Context.NOTIFICATION_SERVICE);
        notificationManager.notify(id, notification);
    }
    
    public void cancelAll() {
        NotificationManager notificationManager = (NotificationManager)_currentContext.getSystemService(Context.NOTIFICATION_SERVICE);
        notificationManager.cancelAll();
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
            JobScheduler jobScheduler = (JobScheduler) _currentContext.getSystemService(Context.JOB_SCHEDULER_SERVICE);
            jobScheduler.cancelAll();
        }
    }

    public static Notification getNotification(Context context, int id, String title, String content) {
        Intent intent = new Intent(context, MengineActivity.class);
        intent.setFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP | Intent.FLAG_ACTIVITY_SINGLE_TOP);
        intent.putExtra(NotificationPublisher.NOTIFICATION_ID, id);
        PendingIntent pendingIntent = PendingIntent.getActivity(context, 0, intent, PendingIntent.FLAG_UPDATE_CURRENT);

        NotificationCompat.Builder builder = new NotificationCompat.Builder(context, channelId);

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
        Log.d(MengineActivity.TAG, "Schedule notification with delay " + delayMillis);

        JobInfo.Builder builder = new JobInfo.Builder((int)SystemClock.elapsedRealtime(), new ComponentName(_currentContext, NotificationJobService.class));
        JobInfo jobInfo = builder
                .setMinimumLatency(delayMillis)
                .setOverrideDeadline(delayMillis + 10000)
                .setExtras(bundle)
                .build();
        JobScheduler jobScheduler = (JobScheduler) _currentContext.getSystemService(Context.JOB_SCHEDULER_SERVICE);
        jobScheduler.schedule(jobInfo);
    }
}