package org.Mengine.Base;

import android.content.Intent;
import android.os.Bundle;
import android.view.Surface;

import androidx.annotation.NonNull;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;

public class MenginePlatformEventQueue {
    private static final MengineTag TAG = MengineTag.of("MNGPlatformEventQueue");

    public sealed interface PlatformEvent permits
        QuitEvent, PauseEvent, ResumeEvent, StopEvent, StartEvent, RestartEvent, DestroyEvent,
        FreezeEvent, SurfaceCreateEvent, SurfaceDestroyEvent, SurfaceChangedEvent,
        ClipboardChangedEvent, WindowFocusChangedEvent, LowMemoryEvent, TrimMemoryEvent, ChangeLocaleEvent,
        IntentStartEvent, IntentNewEvent {
    }

    record QuitEvent() implements PlatformEvent {}
    record PauseEvent(float x, float y) implements PlatformEvent {}
    record ResumeEvent(float x, float y) implements PlatformEvent {}
    record StopEvent() implements PlatformEvent {}
    record StartEvent() implements PlatformEvent {}
    record RestartEvent() implements PlatformEvent {}
    record DestroyEvent() implements PlatformEvent {}
    record FreezeEvent(String owner, boolean freeze) implements PlatformEvent {}
    record SurfaceCreateEvent(Surface surface) implements PlatformEvent {}
    record SurfaceDestroyEvent() implements PlatformEvent {}
    record SurfaceChangedEvent(Surface surface, int surfaceWidth, int surfaceHeight, int deviceWidth, int deviceHeight, float rate) implements PlatformEvent {}
    record ClipboardChangedEvent() implements PlatformEvent {}
    record WindowFocusChangedEvent(boolean focus) implements PlatformEvent {}
    record LowMemoryEvent() implements PlatformEvent {}
    record TrimMemoryEvent(int level) implements PlatformEvent {}
    record ChangeLocaleEvent(String locale) implements PlatformEvent {}
    record IntentStartEvent(String action, String data, String type, int flags, Map<String, Object> extras) implements PlatformEvent {
        // The generated record toString exposes extras values, which may contain sensitive data.
        @NonNull
        @Override
        public String toString() {
            String description = "IntentStartEvent{action='" + action + '\''
                + ", data='" + data + '\''
                + ", type='" + type + '\''
                + ", flags=" + flags
                + ", extras=" + extras.size()
                + '}';

            return description;
        }
    }
    record IntentNewEvent(String action, String data, String type, int flags, Map<String, Object> extras) implements PlatformEvent {
        // The generated record toString exposes extras values, which may contain sensitive data.
        @NonNull
        @Override
        public String toString() {
            String description = "IntentNewEvent{action='" + action + '\''
                + ", data='" + data + '\''
                + ", type='" + type + '\''
                + ", flags=" + flags
                + ", extras=" + extras.size()
                + '}';

            return description;
        }
    }

    private static final Object m_syncEvents = new Object();
    private static final List<PlatformEvent> m_events = new ArrayList<>();
    private static final List<PlatformEvent> m_eventsAux = new ArrayList<>();

    protected static void pushEvent(PlatformEvent event) {
        if (BuildConfig.DEBUG == true) {
            MengineLog.logInfo(TAG, "platform event pushed: %s", event);
        }

        synchronized (m_syncEvents) {
            m_events.add(event);
        }
    }

    public static void pushPauseEvent(float x, float y) {
        MengineNative.AndroidPlatform_lockActivity();
        MenginePlatformEventQueue.pushEvent(new PauseEvent(x, y));
        MengineNative.AndroidPlatform_unlockActivity();
    }

    public static void pushResumeEvent(float x, float y) {
        MengineNative.AndroidPlatform_lockActivity();
        MenginePlatformEventQueue.pushEvent(new ResumeEvent(x, y));
        MengineNative.AndroidPlatform_unlockActivity();
    }

    public static void pushStopEvent() {
        MengineNative.AndroidPlatform_lockActivity();
        MenginePlatformEventQueue.pushEvent(new StopEvent());
        MengineNative.AndroidPlatform_unlockActivity();
    }

    public static void pushStartEvent() {
        MengineNative.AndroidPlatform_lockActivity();
        MenginePlatformEventQueue.pushEvent(new StartEvent());
        MengineNative.AndroidPlatform_unlockActivity();
    }

    public static void pushRestartEvent() {
        MengineNative.AndroidPlatform_lockActivity();
        MenginePlatformEventQueue.pushEvent(new RestartEvent());
        MengineNative.AndroidPlatform_unlockActivity();
    }

    public static void pushDestroyEvent() {
        MengineNative.AndroidPlatform_lockActivity();
        MenginePlatformEventQueue.pushEvent(new DestroyEvent());
        MengineNative.AndroidPlatform_unlockActivity();
    }

    public static void pushFreezeEvent(String owner, boolean freeze) {
        MenginePlatformEventQueue.pushEvent(new FreezeEvent(owner, freeze));
    }

    public static void pushClipboardChangedEvent() {
        MenginePlatformEventQueue.pushEvent(new ClipboardChangedEvent());
    }

    public static void pushWindowFocusChangedEvent(boolean focus) {
        MenginePlatformEventQueue.pushEvent(new WindowFocusChangedEvent(focus));
    }

    public static void pushQuitEvent() {
        MenginePlatformEventQueue.pushEvent(new QuitEvent());
    }

    public static void pushLowMemoryEvent() {
        MenginePlatformEventQueue.pushEvent(new LowMemoryEvent());
    }

    public static void pushTrimMemoryEvent(int level) {
        MenginePlatformEventQueue.pushEvent(new TrimMemoryEvent(level));
    }

    public static void pushChangeLocaleEvent(String locale) {
        MenginePlatformEventQueue.pushEvent(new ChangeLocaleEvent(locale));
    }

    private static boolean meaningfulIntent(@NonNull Intent intent) {
        Bundle bundle = intent.getExtras();
        String action = intent.getAction();
        String data = intent.getDataString();
        String type = intent.getType();

        if (bundle != null) {
            if (bundle.isEmpty() == false) {
                return true;
            }
        }

        if (data != null) {
            return true;
        }

        if (type != null) {
            return true;
        }

        if (action != null) {
            if (Intent.ACTION_MAIN.equals(action) == false) {
                return true;
            }
        }

        return false;
    }

    public static void pushIntentStartEvent(@NonNull Intent intent) {
        boolean meaningfulIntentStart = MenginePlatformEventQueue.meaningfulIntent(intent);

        if (meaningfulIntentStart == false) {
            return;
        }

        Bundle bundle = intent.getExtras();
        String action = intent.getAction();
        String data = intent.getDataString();
        String type = intent.getType();

        Map<String, Object> extras = MengineUtils.bundleToMap(bundle);

        if (action == null) {
            action = "";
        }

        if (data == null) {
            data = "";
        }

        if (type == null) {
            type = "";
        }

        int flags = intent.getFlags();

        MenginePlatformEventQueue.pushEvent(new IntentStartEvent(action, data, type, flags, extras));
    }

    public static void pushIntentNewEvent(@NonNull Intent intent) {
        boolean meaningfulIntentNew = MenginePlatformEventQueue.meaningfulIntent(intent);

        if (meaningfulIntentNew == false) {
            return;
        }

        Bundle bundle = intent.getExtras();
        String action = intent.getAction();
        String data = intent.getDataString();
        String type = intent.getType();

        Map<String, Object> extras = MengineUtils.bundleToMap(bundle);

        if (action == null) {
            action = "";
        }

        if (data == null) {
            data = "";
        }

        if (type == null) {
            type = "";
        }

        int flags = intent.getFlags();

        MenginePlatformEventQueue.pushEvent(new IntentNewEvent(action, data, type, flags, extras));
    }

    public static void pushSurfaceCreatedEvent(Surface surface) {
        MengineNative.AndroidPlatform_lockActivity();
        MenginePlatformEventQueue.pushEvent(new SurfaceCreateEvent(surface));
        MengineNative.AndroidPlatform_unlockActivity();
    }

    public static void pushSurfaceDestroyedEvent() {
        MengineNative.AndroidPlatform_lockActivity();
        MenginePlatformEventQueue.pushEvent(new SurfaceDestroyEvent());
        MengineNative.AndroidPlatform_unlockActivity();
    }

    public static void pushSurfaceChangedEvent(Surface surface, int surfaceWidth, int surfaceHeight, int deviceWidth, int deviceHeight, float rate) {
        MengineNative.AndroidPlatform_lockActivity();
        MenginePlatformEventQueue.pushEvent(new SurfaceChangedEvent(surface, surfaceWidth, surfaceHeight, deviceWidth, deviceHeight, rate));
        MengineNative.AndroidPlatform_unlockActivity();
    }

    public static boolean processEvents(@NonNull MengineApplication application) {
        synchronized (m_syncEvents) {
            if (m_events.isEmpty() == true) {
                return false;
            }

            m_eventsAux.addAll(m_events);
            m_events.clear();
        }

        boolean shouldQuit = false;

        for (PlatformEvent event : m_eventsAux) {
            application.setState("event.call", event.getClass().getSimpleName());

            if (event instanceof PauseEvent pauseEvent) {
                float pauseX = pauseEvent.x();
                float pauseY = pauseEvent.y();
                MengineNative.AndroidPlatform_pauseEvent(pauseX, pauseY);
            } else if (event instanceof ResumeEvent resumeEvent) {
                float resumeX = resumeEvent.x();
                float resumeY = resumeEvent.y();
                MengineNative.AndroidPlatform_resumeEvent(resumeX, resumeY);
            } else if (event instanceof StopEvent) {
                MengineNative.AndroidPlatform_stopEvent();
            } else if (event instanceof StartEvent) {
                MengineNative.AndroidPlatform_startEvent();
            } else if (event instanceof RestartEvent) {
                MengineNative.AndroidPlatform_restartEvent();
            } else if (event instanceof DestroyEvent) {
                MengineNative.AndroidPlatform_destroyEvent();
            } else if (event instanceof FreezeEvent freezeEvent) {
                String freezeOwner = freezeEvent.owner();
                boolean freezeValue = freezeEvent.freeze();
                MengineNative.AndroidPlatform_freezeEvent(freezeOwner, freezeValue);
            } else if (event instanceof ClipboardChangedEvent) {
                MengineNative.AndroidPlatform_clipboardChangedEvent();
            } else if (event instanceof WindowFocusChangedEvent focusEvent) {
                boolean focusValue = focusEvent.focus();
                MengineNative.AndroidPlatform_windowFocusChangedEvent(focusValue);
            } else if (event instanceof QuitEvent) {
                MengineNative.AndroidPlatform_quitEvent();
                shouldQuit = true;
            } else if (event instanceof LowMemoryEvent) {
                MengineNative.AndroidPlatform_lowMemory();
            } else if (event instanceof TrimMemoryEvent trimEvent) {
                int trimMemoryLevel = trimEvent.level();
                MengineNative.AndroidPlatform_trimMemory(trimMemoryLevel);
            } else if (event instanceof ChangeLocaleEvent localeEvent) {
                String localeValue = localeEvent.locale();
                MengineNative.AndroidPlatform_changeLocale(localeValue);
            } else if (event instanceof IntentStartEvent intentStartEvent) {
                String intentAction = intentStartEvent.action();
                String intentData = intentStartEvent.data();
                String intentType = intentStartEvent.type();
                int intentFlags = intentStartEvent.flags();
                Map<String, Object> intentExtras = intentStartEvent.extras();
                MengineNative.AndroidPlatform_intentStart(
                    intentAction,
                    intentData,
                    intentType,
                    intentFlags,
                    intentExtras
                );
            } else if (event instanceof IntentNewEvent intentNewEvent) {
                String intentAction = intentNewEvent.action();
                String intentData = intentNewEvent.data();
                String intentType = intentNewEvent.type();
                int intentFlags = intentNewEvent.flags();
                Map<String, Object> intentExtras = intentNewEvent.extras();
                MengineNative.AndroidPlatform_intentNew(
                    intentAction,
                    intentData,
                    intentType,
                    intentFlags,
                    intentExtras
                );
            } else if (event instanceof SurfaceCreateEvent surfaceCreateEvent) {
                Surface surfaceCreated = surfaceCreateEvent.surface();
                MengineNative.AndroidPlatform_surfaceCreatedEvent(surfaceCreated);
            } else if (event instanceof SurfaceDestroyEvent) {
                MengineNative.AndroidPlatform_surfaceDestroyedEvent();
            } else if (event instanceof SurfaceChangedEvent surfaceChangedEvent) {
                Surface surfaceChanged = surfaceChangedEvent.surface();
                int surfaceWidth = surfaceChangedEvent.surfaceWidth();
                int surfaceHeight = surfaceChangedEvent.surfaceHeight();
                int deviceWidth = surfaceChangedEvent.deviceWidth();
                int deviceHeight = surfaceChangedEvent.deviceHeight();
                float rate = surfaceChangedEvent.rate();
                MengineNative.AndroidPlatform_surfaceChangedEvent(
                    surfaceChanged,
                    surfaceWidth,
                    surfaceHeight,
                    deviceWidth,
                    deviceHeight,
                    rate
                );
            } else {
                MengineLog.logError(TAG, "unknown platform event type: %s", event);
            }
        }

        m_eventsAux.clear();

        return shouldQuit;
    }
}
