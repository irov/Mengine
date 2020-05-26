package org.Mengine.Build.LocalNotifications;

import android.app.AlarmManager;
import android.app.Notification;
import android.app.NotificationChannel;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.content.Context;
import android.content.Intent;
import android.graphics.Color;
import android.os.Build;
import android.os.SystemClock;

import androidx.core.app.NotificationCompat;

import org.Mengine.Build.MengineActivity;
import org.Mengine.Build.R;

public class LocalNotificationsInteractionLayer {
    private static Context _currentContext;
    private static final String channelId = "mengine_channel_id";
    private static final CharSequence channelName = "Mengine Channel";

    public LocalNotificationsInteractionLayer(Context context) {
        _currentContext = context;
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            NotificationManager notificationManager =
                    (NotificationManager) context.getSystemService(Context.NOTIFICATION_SERVICE);

            int importance = NotificationManager.IMPORTANCE_HIGH;
            NotificationChannel notificationChannel = new NotificationChannel(channelId, channelName, importance);
            notificationChannel.enableLights(true);
            notificationChannel.setLightColor(Color.RED);
            notificationChannel.enableVibration(true);
            notificationChannel.setVibrationPattern(new long[]{1000, 2000});
            notificationManager.createNotificationChannel(notificationChannel);
        }
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
    }    

    public Notification getNotification(int id, String title, String content) {
        Intent intent = new Intent(_currentContext, MengineActivity.class);
        intent.setFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP | Intent.FLAG_ACTIVITY_SINGLE_TOP);
        intent.putExtra(NotificationPublisher.NOTIFICATION_ID, id);
        PendingIntent pendingIntent = PendingIntent.getActivity(_currentContext, 0, intent, PendingIntent.FLAG_UPDATE_CURRENT);

        NotificationCompat.Builder builder = new NotificationCompat.Builder(_currentContext, channelId);

        return builder.setContentTitle(title)
                .setContentText(content)
                .setSmallIcon(R.mipmap.ic_notification)
                .setDefaults(NotificationCompat.DEFAULT_ALL)
                .setContentIntent(pendingIntent)
                .setPriority(NotificationCompat.PRIORITY_MAX)
                .build();
    }
}