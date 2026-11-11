package org.Mengine.Plugin.LocalNotifications;

import org.Mengine.Base.MengineService;
import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MengineFragmentEngine;
import org.Mengine.Base.MengineListenerActivity;
import org.Mengine.Base.MengineListenerEngine;
import org.Mengine.Base.MengineUtils;
import org.xmlpull.v1.XmlPullParserException;

import androidx.annotation.NonNull;
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
import android.content.SharedPreferences;
import android.content.pm.PackageManager;
import android.content.res.Resources;
import android.content.res.XmlResourceParser;
import android.graphics.Color;
import android.os.Build;
import android.os.PersistableBundle;
import android.os.SystemClock;
import android.os.Bundle;
import android.service.notification.StatusBarNotification;

import java.io.IOException;
import java.time.DateTimeException;
import java.time.Instant;
import java.time.LocalTime;
import java.time.MonthDay;
import java.time.ZoneId;
import java.time.ZonedDateTime;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Locale;
import java.util.Map;
import java.util.Objects;
import java.util.Set;

public class MengineLocalNotificationsPlugin extends MengineService implements MengineListenerActivity, MengineListenerEngine {
    public static final String SERVICE_NAME = "LNotifications";
    public static final boolean SERVICE_EMBEDDING = true;

    private static final String CHANNEL_ID = "mengine_channel_id";
    private static final CharSequence CHANNEL_NAME = "Mengine Channel";
    static final String NOTIFICATION_TAG = "mengine_local_notification";
    private static final String PREFERENCES_NAME = "mengine.local_notifications";
    private static final String PREFERENCES_SCHEDULED_ALARM_IDS = "scheduled_alarm_ids";
    private static final String PREFERENCES_LAUNCH_COUNT = "launch_count";
    private static final String RESOURCE_REQUEST_LAUNCH = "mengine_local_notifications_request_launch";
    private static final long DEBUG_NOTIFICATION_INTERVAL_MILLIS = 5000;

    static class LocalNotificationDesc {
        public int id;
        public String name;
        public String title;
        public String content;
        public long delay;
        public int month;
        public int day;
        public int hour;
        public int minute;
        public boolean script;
        public boolean yearly;
        public Set<String> languages;
    }

    private static class ScheduledNotification {
        public final LocalNotificationDesc notification;
        public final long delay;

        ScheduledNotification(LocalNotificationDesc notification, long delay) {
            this.notification = notification;
            this.delay = delay;
        }
    }

    private List<LocalNotificationDesc> m_internalLocalNotifications = new ArrayList<>();
    private final Set<Integer> m_scheduledAlarmNotificationIds = new HashSet<>();
    private final Map<String, Long> m_scriptNotificationTimes = new HashMap<>();

    private boolean m_notificationPermissionGranted = false;
    private boolean m_notificationPermissionRequested = false;
    private int m_launchCount = 0;

    @Override
    public void onCreate(@NonNull MengineActivity activity, Bundle savedInstanceState) {
        SharedPreferences preferences = activity.getSharedPreferences(PREFERENCES_NAME, Context.MODE_PRIVATE);
        m_launchCount = preferences.getInt(PREFERENCES_LAUNCH_COUNT, 0);

        if (savedInstanceState == null) {
            m_launchCount += 1;

            SharedPreferences.Editor editor = preferences.edit();
            editor.putInt(PREFERENCES_LAUNCH_COUNT, m_launchCount);
            editor.apply();
        }

        if (MengineLocalNotificationsPlugin.hasPostNotificationsPermission(activity) == false) {
            return;
        }

        this.startLocalNotifications(activity);
    }

    @Override
    public void onResume(@NonNull MengineActivity activity) {
        if (org.Mengine.Base.BuildConfig.MENGINE_RUNTIME_NATIVE == true) {
            if (MengineFragmentEngine.INSTANCE.isMenginePlatformRun() == false) {
                return;
            }
        }

        this.requestNotificationPermission(activity);
    }

    @Override
    public void onMenginePlatformRun(@NonNull MengineApplication application) {
        MengineUtils.performOnMainThread(() -> {
            MengineActivity activity = this.getMengineActivity();

            if (activity == null) {
                return;
            }

            MengineActivity.ELifecycleState lifecycleState = activity.getLifecycleState();

            if (lifecycleState != MengineActivity.ELifecycleState.ACTIVITY_RESUME) {
                return;
            }

            this.requestNotificationPermission(activity);
        });
    }

    private void requestNotificationPermission(@NonNull MengineActivity activity) {
        if (m_notificationPermissionGranted == true) {
            return;
        }

        if (MengineLocalNotificationsPlugin.hasPostNotificationsPermission(activity) == true) {
            this.startLocalNotifications(activity);

            return;
        }

        if (m_notificationPermissionRequested == true) {
            return;
        }

        int requestLaunch = MengineLocalNotificationsPlugin.getRequestPermissionLaunch(activity);

        if (requestLaunch <= 0) {
            this.logInfo("notification permission is not requested on launch");

            return;
        }

        if (m_launchCount < requestLaunch) {
            this.logInfo("notification permission is requested from launch %d, current launch %d"
                , requestLaunch
                , m_launchCount
            );

            return;
        }

        m_notificationPermissionRequested = true;

        activity.checkPermissionPostNotifications(() -> {
                this.startLocalNotifications(activity);
            }
            , () -> {}
        );
    }

    private static int getRequestPermissionLaunch(@NonNull Context context) {
        Resources resources = context.getResources();
        String packageName = context.getPackageName();
        int id = resources.getIdentifier(RESOURCE_REQUEST_LAUNCH, "integer", packageName);

        if (id == 0) {
            return 1;
        }

        int requestLaunch = resources.getInteger(id);

        return requestLaunch;
    }

    private static boolean hasPostNotificationsPermission(@NonNull Context context) {
        if (Build.VERSION.SDK_INT < Build.VERSION_CODES.TIRAMISU) {
            return true;
        }

        if (ContextCompat.checkSelfPermission(context, Manifest.permission.POST_NOTIFICATIONS) != PackageManager.PERMISSION_GRANTED) {
            return false;
        }

        return true;
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

        Locale locale = Locale.getDefault();
        String language = locale.getLanguage();
        long now = System.currentTimeMillis();
        List<ScheduledNotification> notifications = new ArrayList<>();

        for (LocalNotificationDesc desc : m_internalLocalNotifications) {
            if (desc.languages != null) {
                if (desc.languages.contains(language) == false) {
                    continue;
                }
            }

            long delay = this.calculateNotificationDelay(desc, now);

            if (delay <= 0) {
                continue;
            }

            ScheduledNotification notification = new ScheduledNotification(desc, delay);
            notifications.add(notification);
        }

        notifications.sort((left, right) -> {
            int order = Long.compare(left.delay, right.delay);

            if (order != 0) {
                return order;
            }

            int identifierOrder = Integer.compare(left.notification.id, right.notification.id);

            return identifierOrder;
        });

        long debugIndex = 0;

        for (ScheduledNotification notification : notifications) {
            LocalNotificationDesc desc = notification.notification;
            long delay = notification.delay;
            PersistableBundle bundle = MengineLocalNotificationsJobService.notificationBundle(desc.id, desc.title, desc.content);

            if (BuildConfig.DEBUG == true) {
                ++debugIndex;
                delay = debugIndex * DEBUG_NOTIFICATION_INTERVAL_MILLIS;
            } else if (desc.yearly == true) {
                bundle.putInt(MengineLocalNotificationsConstants.KEY_MONTH, desc.month);
                bundle.putInt(MengineLocalNotificationsConstants.KEY_DAY, desc.day);
                bundle.putInt(MengineLocalNotificationsConstants.KEY_HOUR, desc.hour);
                bundle.putInt(MengineLocalNotificationsConstants.KEY_MINUTE, desc.minute);
            }

            this.scheduleJobNotification(delay, bundle);
        }
    }

    private static boolean isValidNotificationDate(int month, int day, int hour, int minute) {
        try {
            MonthDay.of(month, day);
            LocalTime.of(hour, minute);
        } catch (final DateTimeException e) {
            return false;
        }

        return true;
    }

    private long calculateNotificationDelay(@NonNull LocalNotificationDesc desc, long now) {
        Long scriptTime = null;

        if (desc.script == true) {
            synchronized (m_scriptNotificationTimes) {
                scriptTime = m_scriptNotificationTimes.get(desc.name);
            }

            if (scriptTime == null) {
                return 0;
            }
        }

        if (scriptTime != null) {
            return scriptTime - now;
        }

        if (desc.month > 0) {
            long delay = MengineLocalNotificationsPlugin.calculateCalendarDelay(desc.month, desc.day, desc.hour, desc.minute, now);

            return delay;
        }

        return desc.delay;
    }

    static long calculateCalendarDelay(int month, int day, int hour, int minute, long now) {
        MonthDay monthDay = MonthDay.of(month, day);
        ZoneId zone = ZoneId.systemDefault();
        Instant nowInstant = Instant.ofEpochMilli(now);
        ZonedDateTime nowDate = nowInstant.atZone(zone);
        int year = nowDate.getYear();

        for (;; ++year) {
            if (monthDay.isValidYear(year) == false) {
                continue;
            }

            ZonedDateTime triggerDate = ZonedDateTime.of(year, month, day, hour, minute, 0, 0, zone);
            triggerDate = triggerDate.withLaterOffsetAtOverlap();

            Instant triggerInstant = triggerDate.toInstant();
            long triggerTime = triggerInstant.toEpochMilli();

            if (triggerTime <= now) {
                continue;
            }

            long delay = triggerTime - now;

            return delay;
        }
    }

    public void setNotificationDelay(String name, int delaySeconds) {
        this.logInfo("setNotificationDelay name: %s delay: %d"
            , name
            , delaySeconds
        );

        long delayMillis = delaySeconds * 1000L;

        this.setNotificationDelayMillis(name, delayMillis);
    }

    public void setNotificationDelay(String name, float delaySeconds) {
        this.logInfo("setNotificationDelay name: %s delay: %f"
            , name
            , delaySeconds
        );

        long delayMillis = (long)(delaySeconds * 1000.0);

        this.setNotificationDelayMillis(name, delayMillis);
    }

    private void setNotificationDelayMillis(String name, long delayMillis) {
        synchronized (m_scriptNotificationTimes) {
            if (delayMillis <= 0) {
                m_scriptNotificationTimes.remove(name);

                return;
            }

            long time = System.currentTimeMillis() + delayMillis;

            m_scriptNotificationTimes.put(name, time);
        }
    }

    public void requestPermission() {
        this.logInfo("requestPermission");

        MengineUtils.performOnMainThread(() -> {
            MengineActivity activity = this.getMengineActivity();

            if (activity == null) {
                this.logWarning("requestPermission invalid activity");

                this.nativeCall("onLocalNotificationsPermission", false);

                return;
            }

            m_notificationPermissionRequested = true;

            activity.checkPermissionPostNotifications(() -> {
                    if (m_notificationPermissionGranted == false) {
                        this.startLocalNotifications(activity);
                    }

                    this.nativeCall("onLocalNotificationsPermission", true);
                }
                , () -> {
                    this.nativeCall("onLocalNotificationsPermission", false);
                }
            );
        });
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

        this.nativeCall("onLocalNotificationsPress", id);
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

        if (activity == null) {
            this.logWarning("scheduleNotification invalid activity");

            return;
        }
        
        PendingIntent pendingIntent = this.makePublisherPendingIntent(activity, id, notification, PendingIntent.FLAG_UPDATE_CURRENT);

        AlarmManager alarmManager = activity.getSystemService(AlarmManager.class);

        if (alarmManager == null) {
            return;
        }

        long futureInMillis = SystemClock.elapsedRealtime() + delay;

        alarmManager.set(AlarmManager.ELAPSED_REALTIME_WAKEUP, futureInMillis, pendingIntent);

        m_scheduledAlarmNotificationIds.add(id);
        this.saveScheduledAlarmNotificationIds(activity);
    }

    public void instantlyPresentNotification(Notification notification, int id) {
        this.logInfo("instantlyPresentNotification id: %d"
            , id
        );

        if (m_notificationPermissionGranted == false) {
            return;
        }

        MengineActivity activity = this.getMengineActivity();

        if (activity == null) {
            this.logWarning("instantlyPresentNotification invalid activity");

            return;
        }
        
        NotificationManager notificationManager = activity.getSystemService(NotificationManager.class);

        if (notificationManager == null) {
            return;
        }

        notificationManager.notify(NOTIFICATION_TAG, id, notification);
    }
    
    public void startLocalNotifications(MengineActivity activity) {
        this.loadScheduledAlarmNotificationIds(activity);
        this.cancelAll();

        m_notificationPermissionGranted = true;

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

        Intent intent = activity.getIntent();

        if (intent.hasExtra(MengineLocalNotificationsPublisher.NOTIFICATION_ID)) {
            int id = intent.getIntExtra(MengineLocalNotificationsPublisher.NOTIFICATION_ID, 0);

            this.logInfo("start with notification: %d"
                , id
            );

            this.nativeCall("onLocalNotificationsPress", id);
        }

        this.parseInternalLocalNotifications(activity);
    }
    
    public void cancelAll() {
        this.logInfo("cancelAll");

        MengineActivity activity = this.getMengineActivity();

        if (activity == null) {
            this.logWarning("cancelAll invalid activity");

            return;
        }

        NotificationManager notificationManager = activity.getSystemService(NotificationManager.class);

        if (notificationManager != null) {
            StatusBarNotification[] notifications = notificationManager.getActiveNotifications();
            for (StatusBarNotification notification : notifications) {
                String notificationTag = notification.getTag();

                if (Objects.equals(notificationTag, NOTIFICATION_TAG) == false) {
                    continue;
                }

                notificationManager.cancel(NOTIFICATION_TAG, notification.getId());
            }
        }

        AlarmManager alarmManager = activity.getSystemService(AlarmManager.class);

        if (alarmManager != null) {
            for (int id : m_scheduledAlarmNotificationIds) {
                PendingIntent pendingIntent = this.makePublisherPendingIntent(activity, id, null, PendingIntent.FLAG_NO_CREATE);

                if (pendingIntent != null) {
                    alarmManager.cancel(pendingIntent);
                    pendingIntent.cancel();
                }
            }
        }

        m_scheduledAlarmNotificationIds.clear();
        this.saveScheduledAlarmNotificationIds(activity);

        JobScheduler jobScheduler = activity.getSystemService(JobScheduler.class);

        if (jobScheduler != null) {
            ComponentName localNotificationsJobService = new ComponentName(activity, MengineLocalNotificationsJobService.class);

            List<JobInfo> jobs = jobScheduler.getAllPendingJobs();
            for (JobInfo job : jobs) {
                ComponentName jobService = job.getService();

                if (Objects.equals(jobService, localNotificationsJobService) == false) {
                    continue;
                }

                jobScheduler.cancel(job.getId());
            }
        }
    }

    private void loadScheduledAlarmNotificationIds(@NonNull Context context) {
        SharedPreferences preferences = context.getSharedPreferences(PREFERENCES_NAME, Context.MODE_PRIVATE);
        Set<String> ids = preferences.getStringSet(PREFERENCES_SCHEDULED_ALARM_IDS, Set.of());

        m_scheduledAlarmNotificationIds.clear();

        for (String id : ids) {
            try {
                m_scheduledAlarmNotificationIds.add(Integer.parseInt(id));
            } catch (final NumberFormatException e) {
                this.logWarning("invalid persisted alarm notification id: %s", id);
            }
        }
    }

    private void saveScheduledAlarmNotificationIds(@NonNull Context context) {
        Set<String> ids = new HashSet<>();

        for (int id : m_scheduledAlarmNotificationIds) {
            ids.add(String.valueOf(id));
        }

        SharedPreferences preferences = context.getSharedPreferences(PREFERENCES_NAME, Context.MODE_PRIVATE);
        preferences.edit().putStringSet(PREFERENCES_SCHEDULED_ALARM_IDS, ids).apply();
    }

    protected void parseInternalLocalNotifications(@NonNull MengineActivity activity) {
        m_internalLocalNotifications.clear();

        String packageName = activity.getPackageName();
        Resources resources = activity.getResources();
        Set<Integer> notificationIds = new HashSet<>();

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
                String notificationName = null;
                long delay = 0;
                String repeat = "once";
                int month = 0;
                int day = 0;
                int hour = 0;
                int minute = 0;
                boolean script = false;
                Set<String> languages = null;
                String title = null;
                String content = null;

                try {
                    for (int index = 0; index != attributeCount; index++) {
                        String attributeName = parser.getAttributeName(index);
                        String attributeValue = parser.getAttributeValue(index);

                        switch (attributeName) {
                            case "id":
                                id = Integer.parseInt(attributeValue);
                                break;
                            case "name":
                                notificationName = attributeValue;
                                break;
                            case "delay":
                                delay = Long.parseLong(attributeValue);
                                break;
                            case "repeat":
                                repeat = attributeValue;
                                break;
                            case "month":
                                month = Integer.parseInt(attributeValue);
                                break;
                            case "day":
                                day = Integer.parseInt(attributeValue);
                                break;
                            case "hour":
                                hour = Integer.parseInt(attributeValue);
                                break;
                            case "minute":
                                minute = Integer.parseInt(attributeValue);
                                break;
                            case "script":
                                script = Boolean.parseBoolean(attributeValue);
                                break;
                            case "languages":
                                languages = new HashSet<>();
                                String[] values = attributeValue.split(",");

                                for (String language : values) {
                                    String trimmedLanguage = language.trim();
                                    languages.add(trimmedLanguage);
                                }
                                break;
                            case "title":
                                title = attributeValue;
                                break;
                            case "content":
                                content = attributeValue;
                                break;
                        }
                    }
                } catch (final NumberFormatException e) {
                    this.logWarning("local notification invalid numeric attribute: %s"
                        , e.getMessage()
                    );

                    continue;
                }

                if (id <= 0) {
                    this.logWarning("local notification invalid id: %d"
                        , id
                    );

                    continue;
                }

                if (notificationIds.contains(id) == true) {
                    this.logWarning("local notification duplicate id: %d"
                        , id
                    );

                    continue;
                }

                if (delay < 0) {
                    this.logWarning("local notification %d invalid delay: %d"
                        , id
                        , delay
                    );

                    continue;
                }

                if (delay > Long.MAX_VALUE / 1000) {
                    this.logWarning("local notification %d delay too large: %d"
                        , id
                        , delay
                    );

                    continue;
                }

                if (Objects.equals(repeat, "once") == false) {
                    if (Objects.equals(repeat, "yearly") == false) {
                        this.logWarning("local notification %d invalid repeat: %s"
                            , id
                            , repeat
                        );

                        continue;
                    }
                }

                boolean yearly = Objects.equals(repeat, "yearly");

                if (yearly == true) {
                    if (month <= 0) {
                        this.logWarning("local notification %d yearly repeat requires month/day"
                            , id
                        );

                        continue;
                    }
                }

                int triggers = (delay > 0 ? 1 : 0) + (month > 0 ? 1 : 0) + (script == true ? 1 : 0);

                if (triggers != 1) {
                    this.logWarning("local notification %d needs exactly one of delay, month/day or script"
                        , id
                    );

                    continue;
                }

                if (month > 0) {
                    if (MengineLocalNotificationsPlugin.isValidNotificationDate(month, day, hour, minute) == false) {
                        this.logWarning("local notification %d invalid date/time: %d/%d %d:%d"
                            , id
                            , day
                            , month
                            , hour
                            , minute
                        );

                        continue;
                    }
                }

                if (script == true) {
                    if (notificationName == null) {
                        this.logWarning("local notification %d script has no name"
                            , id
                        );

                        continue;
                    }

                    if (notificationName.isEmpty() == true) {
                        this.logWarning("local notification %d script has empty name"
                            , id
                        );

                        continue;
                    }
                }

                if (title == null) {
                    this.logWarning("local notification %d has no title"
                        , id
                    );

                    continue;
                }

                if (content == null) {
                    this.logWarning("local notification %d has no content"
                        , id
                    );

                    continue;
                }

                int title_id = resources.getIdentifier(title, "string", packageName);
                int content_id = resources.getIdentifier(content, "string", packageName);

                if (title_id == 0) {
                    this.logWarning("local notification %d title resource not found: %s"
                        , id
                        , title
                    );

                    continue;
                }

                if (content_id == 0) {
                    this.logWarning("local notification %d content resource not found: %s"
                        , id
                        , content
                    );

                    continue;
                }

                String title_text = resources.getString(title_id);
                String content_text = resources.getString(content_id);

                LocalNotificationDesc desc = new LocalNotificationDesc();
                desc.id = id;
                desc.name = notificationName;
                desc.title = title_text;
                desc.content = content_text;
                desc.delay = delay * 1000;
                desc.month = month;
                desc.day = day;
                desc.hour = hour;
                desc.minute = minute;
                desc.script = script;
                desc.yearly = yearly;
                desc.languages = languages;

                m_internalLocalNotifications.add(desc);
                notificationIds.add(id);
            }

            parser.close();
        } catch (final XmlPullParserException e) {
            this.logException(e, Map.of());
        } catch (final IOException e) {
            this.logException(e, Map.of());
        }
    }

    public static Notification getNotification(Context context, int id, String title, String content) {
        PendingIntent pendingIntent = MengineLocalNotificationsPlugin.makeContentPendingIntent(context, id, PendingIntent.FLAG_UPDATE_CURRENT);

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

    private PendingIntent makePublisherPendingIntent(@NonNull Context context, int id, Notification notification, int pendingFlags) {
        Intent notificationIntent = new Intent(context, MengineLocalNotificationsPublisher.class);
        notificationIntent.setAction(MengineLocalNotificationsPublisher.NOTIFICATION + ":" + id);
        notificationIntent.putExtra(MengineLocalNotificationsPublisher.NOTIFICATION_ID, id);

        if (notification != null) {
            notificationIntent.putExtra(MengineLocalNotificationsPublisher.NOTIFICATION, notification);
        }

        pendingFlags |= PendingIntent.FLAG_IMMUTABLE;

        return PendingIntent.getBroadcast(context, id, notificationIntent, pendingFlags);
    }

    private static PendingIntent makeContentPendingIntent(@NonNull Context context, int id, int pendingFlags) {
        PackageManager packageManager = context.getPackageManager();
        String packageName = context.getPackageName();
        Intent intent = packageManager.getLaunchIntentForPackage(packageName);
        if (intent == null) {
            throw new IllegalStateException("application has no launcher activity");
        }
        intent.setFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP | Intent.FLAG_ACTIVITY_SINGLE_TOP);
        intent.setAction(MengineLocalNotificationsPublisher.NOTIFICATION_ID + ":" + id);
        intent.putExtra(MengineLocalNotificationsPublisher.NOTIFICATION_ID, id);

        pendingFlags |= PendingIntent.FLAG_IMMUTABLE;

        return PendingIntent.getActivity(context, id, intent, pendingFlags);
    }

    private void scheduleJobNotification(long delayMillis, PersistableBundle bundle){
        this.logInfo("schedule notification with delay: %d", delayMillis);

        if (m_notificationPermissionGranted == false) {
            return;
        }

        MengineActivity activity = this.getMengineActivity();

        if (activity == null) {
            this.logWarning("scheduleJobNotification invalid activity");

            return;
        }

        if (MengineLocalNotificationsJobService.scheduleNotification(activity, delayMillis, bundle) == false) {
            this.logWarning("scheduleJobNotification failed for id: %d"
                , bundle.getInt(MengineLocalNotificationsConstants.KEY_ID)
            );
        }
    }
}
