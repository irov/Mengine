package org.Mengine.Base;

import android.os.Handler;
import android.os.Looper;

import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.util.concurrent.ArrayBlockingQueue;
import java.util.Map;
import java.nio.charset.StandardCharsets;
import java.util.function.Consumer;

public class MengineWebViewService extends MengineService implements MengineListenerApplication {
    public static final String SERVICE_NAME = "WebView";
    public static final boolean SERVICE_EMBEDDING = true;
    private final ArrayBlockingQueue<MengineWebViewEvent> m_events = new ArrayBlockingQueue<>(256);
    private final Handler m_ui;
    private MengineWebViewActivityContent m_content;
    private long m_documentSequence;
    private String m_documentGeneration = "";
    private long m_binding;
    private volatile JSONObject m_status = new JSONObject();

    private Consumer<JSONObject> m_resultCallback;

    public MengineWebViewService() {
        Looper mainLooper = Looper.getMainLooper();
        m_ui = new Handler(mainLooper);
    }

    public void setWebViewResultCallback(Consumer<JSONObject> callback) {
        m_resultCallback = callback;
    }

    @Override
    public void onAppTerminate(@NonNull MengineApplication application) {
        m_documentGeneration = "";
        this.onWebViewDocumentClosed();
    }

    @Override
    public void onAppFinalize(@NonNull MengineApplication application) {
        m_ui.removeCallbacksAndMessages(null);
        m_content = null;
        m_resultCallback = null;
        m_events.clear();
        m_status = new JSONObject();

        super.onAppFinalize(application);
    }

    public void attachWebView(MengineWebViewActivityContent content) {
        m_content = content;
        m_status = new JSONObject();
        JSONObject event = new JSONObject();
        String binding = Long.toString(++m_binding);
        try {
            event.put("binding", binding);
        } catch (JSONException error) {
            throw new IllegalStateException(error);
        }

        this.queueWebViewEvent(MengineWebViewEvent.ATTACH, event);
    }

    public void detachWebView(MengineWebViewActivityContent content) {
        if (m_content != content) {
            return;
        }

        m_content = null;
        m_documentGeneration = "";
        this.onWebViewDocumentClosed();
        m_status = new JSONObject();
        this.queueWebViewEvent(MengineWebViewEvent.DETACH, null);
    }

    public long nextWebViewDocument() {
        m_documentGeneration = "";
        this.onWebViewDocumentClosed();

        long documentSequence = ++m_documentSequence;

        return documentSequence;
    }

    public byte[] getWebViewHandlerNames() {
        JSONArray names = new JSONArray();
        String[] handlers = this.getWebViewHandlers();

        for (String name : handlers) {
            names.put(name);
        }

        String json = names.toString();
        byte[] bytes = json.getBytes(StandardCharsets.UTF_8);

        return bytes;
    }

    public void connectWebViewDocument(String generation) {
        m_documentGeneration = generation;
    }

    public String getWebViewDocumentGeneration() {
        return m_documentGeneration;
    }

    protected String[] getWebViewHandlers() {
        String[] handlers = new String[0];

        return handlers;
    }

    protected void onWebViewRequest(String method, String generation, String id, JSONObject arguments) {
        this.completeWebViewRequest(generation, id, null, "Unknown native handler");
    }

    protected void onWebViewDocumentClosed() {
        //Empty
    }

    protected void onWebViewExternalNavigation(String url) {
        //Empty
    }

    protected void completeWebViewRequest(String generation, String id, Object result, String error) {
        JSONObject event = new JSONObject();
        Object value = result == null ? JSONObject.NULL : result;
        try {
            event.put("generation", generation);
            event.put("id", id);
            event.put("result", value);
            event.put("error", error);
        } catch (JSONException exception) {
            throw new IllegalArgumentException(exception);
        }

        this.queueWebViewEvent(MengineWebViewEvent.COMPLETE_REQUEST, event);
    }

    public void queueWebViewEvent(int id, @Nullable JSONObject data) {
        MengineWebViewEvent event = new MengineWebViewEvent(id, data);

        if (m_events.offer(event) == true) {
            return;
        }

        this.logError("WebView event queue overflow");
        m_ui.post(() -> {
            MengineWebViewActivityContent content = m_content;

            if (content == null) {
                MengineApplication application = this.getMengineApplication();
                application.quit();

                return;
            }

            MengineActivity activity = content.getMengineActivity();
            activity.linkingQuitApplication();
        });
    }

    public boolean isWebViewQuitRequested() {
        MengineApplication application = this.getMengineApplication();

        if (application.isQuitRequested() == true) {
            return true;
        }

        Thread thread = Thread.currentThread();

        if (thread.isInterrupted() == true) {
            return true;
        }

        return false;
    }

    @Nullable
    public MengineWebViewEvent pollWebViewEvent() {
        if (this.isWebViewQuitRequested() == true) {
            MengineWebViewEvent event = new MengineWebViewEvent(MengineWebViewEvent.QUIT, null);

            return event;
        }

        MengineWebViewEvent event = m_events.poll();

        return event;
    }

    public void postWebViewCommand(byte[] json) {
        JSONObject command;
        String binding;
        try {
            String data = new String(json, StandardCharsets.UTF_8);
            JSONObject envelope = new JSONObject(data);
            command = envelope.getJSONObject("command");
            binding = envelope.getString("binding");
        } catch (JSONException error) {
            throw new IllegalArgumentException(error);
        }

        m_ui.post(() -> {
            String kind = command.optString("kind");

            if ("captureResult".equals(kind) == true) {
                this.onWebViewResult(command);

                return;
            }

            if ("logsResult".equals(kind) == true) {
                this.onWebViewResult(command);

                return;
            }

            MengineWebViewActivityContent content = m_content;

            if (content == null) {
                return;
            }

            String currentBinding = Long.toString(m_binding);

            if (binding.equals(currentBinding) == false) {
                return;
            }

            if ("status".equals(kind) == true) {
                m_status = command;

                return;
            }

            if ("request".equals(kind) == true) {
                String generation = command.optString("generation");

                if (m_documentGeneration.equals(generation) == false) {
                    return;
                }

                String method = command.optString("method");
                String id = command.optString("id");
                JSONObject arguments = command.optJSONObject("arguments");
                this.onWebViewRequest(method, generation, id, arguments);

                return;
            }

            if ("external".equals(kind) == true) {
                String url = command.optString("url");
                this.onWebViewExternalNavigation(url);

                return;
            }

            content.handleWebViewCommand(command);
        });
    }

    public void onWebViewRuntimeFailed() {
        m_ui.post(() -> {
            MengineWebViewActivityContent content = m_content;

            if (content == null) {
                MengineApplication application = this.getMengineApplication();
                MengineServiceInvalidInitializeException error = new MengineServiceInvalidInitializeException("WebView runtime initialization failed");
                Map<String, Object> properties = Map.of();
                application.invalidInitialize(error, properties);
                application.quit();

                return;
            }

            MengineActivity activity = content.getMengineActivity();
            activity.finishWithAlertDialog("Cannot initialize the WebView runtime");
        });
    }

    public String getWebViewTemporaryDirectory() {
        String directory = this.getMengineApplication().getCacheDir().getAbsolutePath() + "/";

        return directory;
    }

    protected MengineWebViewActivityContent getWebViewContent() {
        return m_content;
    }

    protected MengineActivity getWebViewActivity() {
        if (m_content == null) {
            return null;
        }

        MengineActivity activity = m_content.getMengineActivity();

        return activity;
    }

    protected void onWebViewResult(JSONObject result) {
        if (m_resultCallback == null) {
            return;
        }

        m_resultCallback.accept(result);
    }

    public JSONObject getWebViewStatus() {
        JSONObject status = m_status;

        return status;
    }

    @Override
    public void onAppLowMemory(@NonNull MengineApplication application) {
        this.queueMemoryEvent_(-1);
    }

    @Override
    public void onAppTrimMemory(@NonNull MengineApplication application, int level) {
        this.queueMemoryEvent_(level);
    }

    private void queueMemoryEvent_(int level) {
        JSONObject event = new JSONObject();
        try {
            event.put("level", level);
        } catch (JSONException error) {
            throw new IllegalArgumentException(error);
        }

        this.queueWebViewEvent(MengineWebViewEvent.MEMORY, event);
    }
}
