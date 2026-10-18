package org.Mengine.Base;

import android.content.Intent;
import android.os.Bundle;

import android.util.Base64;
import android.os.Build;
import android.webkit.WebView;
import android.webkit.WebViewRenderProcess;

import androidx.annotation.NonNull;

import org.json.JSONException;
import org.json.JSONObject;

public final class MengineWebViewMCP extends MengineService implements MengineListenerApplication, MengineListenerActivity, MengineWebViewMCPTransport.HandlerInterface {
    public static final String SERVICE_NAME = "WebViewMCP";
    public static final boolean SERVICE_EMBEDDING = false;

    private MengineWebViewService m_webView;
    private MengineWebViewMCPTransport m_mcp;

    @Override
    public void onAppCreate(@NonNull MengineApplication application) throws MengineServiceInvalidInitializeException {
        m_webView = this.getService(MengineWebViewService.class);

        if (m_webView == null) {
            throw new MengineServiceInvalidInitializeException("WebView MCP requires the WebView service");
        }

        m_webView.setWebViewResultCallback(this::onWebViewResult);
    }

    @Override
    public void onAppTerminate(@NonNull MengineApplication application) {
        if (m_mcp != null) {
            m_mcp.stop();
            m_mcp = null;
        }

        if (m_webView == null) {
            return;
        }

        m_webView.setWebViewResultCallback(null);
    }

    @Override
    public void onCreate(@NonNull MengineActivity activity, Bundle savedInstanceState) {
        Intent intent = activity.getIntent();
        this.startMCP(intent);
    }

    @Override
    public void onNewIntent(@NonNull MengineActivity activity, Intent intent) {
        this.startMCP(intent);
    }

    public void startMCP(Intent intent) {
        if (m_mcp != null) {
            return;
        }

        String host = intent.getStringExtra("mengine.mcp.host");

        if (host == null) {
            return;
        }

        String port = intent.getStringExtra("mengine.mcp.port");

        if (port == null) {
            return;
        }

        String token = intent.getStringExtra("mengine.mcp.token");

        if (token == null) {
            return;
        }

        int portNumber;
        try {
            portNumber = Integer.parseInt(port);
        } catch (NumberFormatException error) {
            this.logError("Invalid WebView MCP port: %s", port);

            return;
        }

        if (portNumber <= 0) {
            return;
        }

        if (portNumber > 65535) {
            return;
        }

        MengineWebViewMCPTransport transport = new MengineWebViewMCPTransport(this);
        transport.start(host, portNumber, token);
        m_mcp = transport;
    }

    @Override
    public void onWebViewMCPTransportRequest(String method, JSONObject params, int id) throws JSONException {
        MengineActivity activity = m_webView.getWebViewActivity();
        MengineWebViewActivityContent content = m_webView.getWebViewContent();
        WebView browser = content == null ? null : content.getBrowser();

        switch (method) {
            case "script_call": {
                String name = params.getString("name");

                switch (name) {
                    case "webview.status": {
                        JSONObject state = m_webView.getWebViewStatus();
                        JSONObject report = state.optJSONObject("report");
                        JSONObject status;
                        if (report == null) {
                            status = new JSONObject();
                        } else {
                            String json = report.toString();
                            status = new JSONObject(json);
                        }

                        boolean documentReady = state.optBoolean("documentReady");
                        status.put("documentReady", documentReady);
                        boolean appReady = state.optBoolean("appReady");
                        status.put("appReady", appReady);
                        String generation = state.optString("generation");
                        status.put("generation", generation);
                        boolean runtimeTimer = state.optBoolean("runtimeTimer");
                        status.put("runtimeTimer", runtimeTimer);
                        boolean complete = status.optBoolean("complete", true) && appReady;
                        status.put("complete", complete);
                        m_mcp.reply(id, status);
                        break;
                    }
                    case "webview.reload": {
                        m_webView.queueWebViewEvent(MengineWebViewEvent.RELOAD, null);
                        JSONObject result = new JSONObject();
                        result.put("loading", true);
                        m_mcp.reply(id, result);
                        break;
                    }
                    case "webview.recreate": {
                        if (activity == null) {
                            m_mcp.error(id, "no Activity");
                            break;
                        }

                        activity.recreate();
                        JSONObject result = new JSONObject();
                        result.put("recreating", true);
                        m_mcp.reply(id, result);
                        break;
                    }
                    case "webview.evaluate": {
                        if (browser == null) {
                            m_mcp.error(id, "no Activity");
                            break;
                        }

                        JSONObject arguments = params.getJSONObject("arguments");
                        String script = arguments.getString("script");
                        MengineWebViewMCPTransport transport = m_mcp;
                        browser.evaluateJavascript(script, value -> {
                            transport.reply(id, value);
                        });
                        break;
                    }
                    case "webview.terminateRenderer": {
                        boolean terminated = false;

                        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.Q && browser != null) {
                            WebViewRenderProcess renderer = browser.getWebViewRenderProcess();

                            if (renderer != null) {
                                terminated = renderer.terminate();
                            }
                        }

                        JSONObject result = new JSONObject();
                        result.put("terminated", terminated);
                        m_mcp.reply(id, result);
                        break;
                    }
                    default: {
                        m_mcp.error(id, "unknown native WebView handler");
                        break;
                    }
                }
                break;
            }
            case "logs_read": {
                long after = params.optLong("after", 0);
                after = Math.max(0, after);
                int limit = params.optInt("limit", 500);

                if (limit <= 0) {
                    m_mcp.error(id, "limit must be positive");

                    break;
                }

                limit = Math.min(limit, 5000);
                JSONObject event = new JSONObject();
                String requestId = Integer.toString(id);
                event.put("id", requestId);
                event.put("after", after);
                event.put("limit", limit);
                m_webView.queueWebViewEvent(MengineWebViewEvent.LOGS_READ, event);
                break;
            }
            case "frame_capture": {
                JSONObject event = new JSONObject();
                String requestId = Integer.toString(id);
                event.put("id", requestId);
                m_webView.queueWebViewEvent(MengineWebViewEvent.CAPTURE, event);
                break;
            }
            case "app_stop": {
                JSONObject result = new JSONObject();
                result.put("stopped", true);
                m_mcp.reply(id, result);
                m_mcp.afterReplies(() -> {
                    if (activity != null) {
                        activity.linkingQuitApplication();
                    } else {
                        MengineApplication application = this.getMengineApplication();
                        application.quit();
                    }
                });
                break;
            }
            default: {
                m_mcp.error(id, "unsupported native WebView method");
                break;
            }
        }
    }

    public void onWebViewResult(JSONObject result) {
        if (m_mcp == null) {
            return;
        }

        String requestId = result.optString("id");
        int id = Integer.parseInt(requestId);
        String kind = result.optString("kind");

        if ("logsResult".equals(kind) == true) {
            JSONObject response = result.optJSONObject("result");
            m_mcp.reply(id, response);

            return;
        }

        String data = result.optString("data");
        byte[] bytes = Base64.decode(data, Base64.DEFAULT);

        if (bytes.length == 0) {
            m_mcp.error(id, "snapshot cancelled or unavailable");

            return;
        }

        JSONObject response = new JSONObject();
        try {
            int width = result.optInt("width");
            response.put("width", width);
            int height = result.optInt("height");
            response.put("height", height);
            JSONObject attachment = new JSONObject();
            attachment.put("size", bytes.length);
            attachment.put("mimeType", "image/png");
            response.put("attachment", attachment);
        } catch (JSONException error) {
            String message = error.toString();
            m_mcp.error(id, message);

            return;
        }

        m_mcp.reply(id, response);
        m_mcp.binary(id, bytes);
    }

    @Override
    public void onWebViewMCPTransportDisconnect() {
        MengineActivity activity = m_webView.getWebViewActivity();

        if (activity != null) {
            activity.linkingQuitApplication();
        } else {
            MengineApplication application = this.getMengineApplication();
            application.quit();
        }
    }
}
