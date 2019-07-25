package org.Mengine.Build.LocalNotifications;

import android.app.AlarmManager;
import android.app.Notification;
import android.app.PendingIntent;
import android.content.Context;
import android.content.Intent;
import android.os.SystemClock;
import android.support.v4.app.NotificationManagerCompat;

import org.Mengine.Build.R;

public class LocalNotificationsInteractionLayer {
    private static Context _currentContext;

    public LocalNotificationsInteractionLayer(Context context) {
        _currentContext = context;
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
        NotificationManagerCompat notificationManager = NotificationManagerCompat.from(_currentContext);
        notificationManager.notify(id, notification);
    }

    public Notification getNotification(String title, String content) {
        Notification.Builder builder = new Notification.Builder(_currentContext);
        builder.setContentTitle(title);
        builder.setContentText(content);
        builder.setSmallIcon(R.mipmap.ic_launcher);
        return builder.build();
    }
}
