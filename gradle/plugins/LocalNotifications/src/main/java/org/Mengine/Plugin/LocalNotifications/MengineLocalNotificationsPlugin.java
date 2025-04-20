package org.Mengine.Plugin.LocalNotifications;

import org.Mengine.Base.MengineService;
import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.MengineListenerActivity;
import org.Mengine.Base.MengineUtils;
import org.xmlpull.v1.XmlPullParserException;

import androidx.annotation.NonNull;
import androidx.annotation.RequiresApi;
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
import android.content.res.Resources;
import android.content.res.XmlResourceParser;
import android.graphics.Color;
import android.os.Build;
import android.os.PersistableBundle;
import android.os.SystemClock;
import android.os.Bundle;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;
import java.util.Objects;

public class MengineLocalNotificationsPlugin extends MengineService implements MengineListenerActivity {
    public static final String SERVICE_NAME = "LNotifications";
    public static final boolean SERVICE_EMBEDDING = true;

    private static final String CHANNEL_ID = "mengine_channel_id";
    private static final CharSequence CHANNEL_NAME = "Mengine Channel";

    static class LocalNotificationDesc {
        public int id;
        public String title;
        public String content;
        public long delay;
    }

    private List<LocalNotificationDesc> m_internalLocalNotifications = new ArrayList<>();

    private boolean m_notificationPermissionGranted = false;

    @Override
    public void onCreate(@NonNull MengineActivity activity, Bundle savedInstanceState) {
        activity.checkPermission(Manifest.permission.POST_NOTIFICATIONS
            , () -> {
                this.startLocalNotifications(activity);
            }
            , () -> {}
        );
    }

    @Override
    public void onStart(@NonNull MengineActivity activity) {
        if (m_notificationPermissionGranted == false) {
            return;
        }

        this.cancelAll();
    }

    @Override
    public void onStop(@NonNull MengineActivity activity) {
        if (m_notificationPermissionGranted == false) {
            return;
        }

        if (MengineUtils.isAppInForeground(activity) == true) {
            return;
        }

        for (LocalNotificationDesc desc : m_internalLocalNotifications) {
            this.scheduleJobServiceNotification(desc.id, desc.title, desc.content, desc.delay);
        }
    }

    @Override
    public void onNewIntent(@NonNull MengineActivity activity, Intent intent) {
        if (m_notificationPermissionGranted == false) {
            return;
        }

        if (intent.hasExtra(MengineLocalNotificationsPublisher.NOTIFICATION_ID) == false) {
            return;
        }

        int id = intent.getIntExtra(MengineLocalNotificationsPublisher.NOTIFICATION_ID, 0);

        this.logInfo("onNewIntent has notification: %d"
            , id
        );

        this.pythonCall("onLocalNotificationsPress", id);
    }

    public void scheduleJobServiceNotification(int id, String title, String content, long delay) {
        this.logInfo("scheduleJobServiceNotification id: %d title: %s content: %s delay: %d"
            , id
            , title
            , content
            , delay
        );

        if (m_notificationPermissionGranted == false) {
            return;
        }

        PersistableBundle bundle = MengineLocalNotificationsJobService.notificationBundle(id, title, content);

        this.scheduleJobNotification(delay, bundle);
    }

    public void scheduleNotification(Notification notification, int id, long delay) {
        this.logInfo("scheduleNotification id: %d delay: %d"
            , id
            , delay
        );

        if (m_notificationPermissionGranted == false) {
            return;
        }

        MengineActivity activity = this.getMengineActivity();
        
        Intent notificationIntent = new Intent(activity, MengineLocalNotificationsPublisher.class);

        notificationIntent.putExtra(MengineLocalNotificationsPublisher.NOTIFICATION_ID, id);
        notificationIntent.putExtra(MengineLocalNotificationsPublisher.NOTIFICATION, notification);

        int pendingFlags = PendingIntent.FLAG_UPDATE_CURRENT;

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            pendingFlags |= PendingIntent.FLAG_IMMUTABLE;
        }

        PendingIntent pendingIntent = PendingIntent.getBroadcast(activity, 0, notificationIntent, pendingFlags);

        AlarmManager alarmManager = activity.getSystemService(AlarmManager.class);

        if (alarmManager == null) {
            return;
        }

        long futureInMillis = SystemClock.elapsedRealtime() + delay;

        alarmManager.set(AlarmManager.ELAPSED_REALTIME_WAKEUP, futureInMillis, pendingIntent);
    }

    public void instantlyPresentNotification(Notification notification, int id) {
        this.logInfo("instantlyPresentNotification id: %d"
            , id
        );

        if (m_notificationPermissionGranted == false) {
            return;
        }

        MengineActivity activity = this.getMengineActivity();
        
        NotificationManager notificationManager = activity.getSystemService(NotificationManager.class);

        if (notificationManager == null) {
            return;
        }

        notificationManager.notify(id, notification);
    }
    
    public void startLocalNotifications(MengineActivity activity) {
        this.cancelAll();

        m_notificationPermissionGranted = true;

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.O) {
            NotificationManager notificationManager = activity.getSystemService(NotificationManager.class);

            if (notificationManager == null) {
                return;
            }

            int importance = NotificationManager.IMPORTANCE_HIGH;
            NotificationChannel channel = new NotificationChannel(CHANNEL_ID, CHANNEL_NAME, importance);
            channel.enableLights(true);
            channel.setLightColor(Color.RED);
            channel.enableVibration(true);
            channel.setVibrationPattern(new long[]{1000, 2000});
            notificationManager.createNotificationChannel(channel);
        }

        Intent intent = activity.getIntent();

        if (intent.hasExtra(MengineLocalNotificationsPublisher.NOTIFICATION_ID)) {
            int id = intent.getIntExtra(MengineLocalNotificationsPublisher.NOTIFICATION_ID, 0);

            this.logInfo("start with notification: %d"
                , id
            );

            this.pythonCall("onLocalNotificationsPress", id);
        }

        this.parseInternalLocalNotifications(activity);
    }
    
    public void cancelAll() {
        this.logInfo("cancelAll");

        MengineActivity activity = this.getMengineActivity();
        
        NotificationManager notificationManager = activity.getSystemService(NotificationManager.class);

        if (notificationManager != null) {
            notificationManager.cancelAll();
        }

        JobScheduler jobScheduler = activity.getSystemService(JobScheduler.class);

        if (jobScheduler != null) {
            jobScheduler.cancelAll();
        }
    }

    protected void parseInternalLocalNotifications(MengineActivity activity) {
        String packageName = activity.getPackageName();
        Resources resources = activity.getResources();

        try {
            XmlResourceParser parser = resources.getXml(R.xml.local_notifications);

            while (parser.next() != XmlResourceParser.END_DOCUMENT) {
                if (parser.getEventType() != XmlResourceParser.START_TAG) {
                    continue;
                }

                String name = parser.getName();

                if (Objects.equals(name, "notification") == false) {
                    continue;
                }

                int attributeCount = parser.getAttributeCount();

                int id = 0;
                long delay = 0;
                String title = null;
                String content = null;

                for (int index = 0; index != attributeCount; index++) {
                    String attributeName = parser.getAttributeName(index);
                    String attributeValue = parser.getAttributeValue(index);

                    switch (attributeName) {
                        case "id":
                            id = Integer.parseInt(attributeValue);
                            break;
                        case "delay":
                            delay = Long.parseLong(attributeValue);
                            break;
                        case "title":
                            title = attributeValue;
                            break;
                        case "content":
                            content = attributeValue;
                            break;
                    }
                }

                if (id <= 0) {
                    this.logWarning("[LOCAL_NOTIFICATIOINS] invalid id: %d"
                        , id
                    );

                    continue;
                }

                if (delay <= 0) {
                    this.logWarning("[LOCAL_NOTIFICATIOINS] invalid delay: %d"
                        , delay
                    );

                    continue;
                }

                if (title == null) {
                    this.logWarning("[LOCAL_NOTIFICATIOINS] invalid title");

                    continue;
                }

                if (content == null) {
                    this.logWarning("[LOCAL_NOTIFICATIOINS] invalid content");

                    continue;
                }

                if (BuildConfig.DEBUG == true) {
                    delay /= 1000;
                }

                int title_id = resources.getIdentifier(title, "string", packageName);
                int content_id = resources.getIdentifier(content, "string", packageName);

                String title_text = resources.getString(title_id);
                String content_text = resources.getString(content_id);

                LocalNotificationDesc desc = new LocalNotificationDesc();
                desc.id = id;
                desc.title = title_text;
                desc.content = content_text;
                desc.delay = delay * 1000;

                m_internalLocalNotifications.add(desc);
            }

            parser.close();
        } catch (final XmlPullParserException e) {
            this.logError("[LOCAL_NOTIFICATIOINS] XmlPullParserException: %s"
                , e.getMessage()
            );

            return;
        } catch (final IOException e) {
            this.logError("[LOCAL_NOTIFICATIOINS] IOException: %s"
                , e.getMessage()
            );

            return;
        }
    }

    @RequiresApi(api = Build.VERSION_CODES.M)
    public static Notification getNotification(Context context, int id, String title, String content) {
        Intent intent = new Intent(context, MengineActivity.class);
        intent.setFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP | Intent.FLAG_ACTIVITY_SINGLE_TOP);
        intent.putExtra(MengineLocalNotificationsPublisher.NOTIFICATION_ID, id);

        int pendingFlags = PendingIntent.FLAG_UPDATE_CURRENT;
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            pendingFlags |= PendingIntent.FLAG_IMMUTABLE;
        }

        PendingIntent pendingIntent = PendingIntent.getActivity(context, 0, intent, pendingFlags);

        NotificationCompat.Builder builder = new NotificationCompat.Builder(context, CHANNEL_ID)
            .setContentTitle(title)
            .setContentText(content)
            .setStyle(new NotificationCompat.BigTextStyle().bigText(content))
            .setSmallIcon(R.drawable.ic_notification)
            .setColor(ContextCompat.getColor(context, R.color.mengine_local_notification_color))  // <color name="mengine_local_notification_color">#422b00</color>
            .setDefaults(NotificationCompat.DEFAULT_ALL)
            .setContentIntent(pendingIntent)
            .setAutoCancel(true)
            .setPriority(NotificationCompat.PRIORITY_DEFAULT);

        Notification notification = builder.build();

        return notification;
    }

    private void scheduleJobNotification(long delayMillis, PersistableBundle bundle){
        this.logInfo("schedule notification with delay: %d", delayMillis);

        if (m_notificationPermissionGranted == false) {
            return;
        }
        
        MengineActivity activity = this.getMengineActivity();

        int jobId = (int)SystemClock.elapsedRealtime();
        ComponentName jobService = new ComponentName(activity, MengineLocalNotificationsJobService.class);

        JobInfo.Builder builder = new JobInfo.Builder(jobId, jobService);
        JobInfo jobInfo = builder
            .setMinimumLatency(delayMillis)
            .setOverrideDeadline(delayMillis + 10000)
            .setExtras(bundle)
            .build();

        JobScheduler jobScheduler = activity.getSystemService(JobScheduler.class);

        if (jobScheduler == null) {
            return;
        }

        jobScheduler.schedule(jobInfo);
    }
}