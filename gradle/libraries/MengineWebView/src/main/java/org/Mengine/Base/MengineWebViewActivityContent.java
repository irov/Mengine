package org.Mengine.Base;

import android.content.Intent;
import android.content.res.AssetManager;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageInfo;
import android.content.pm.PackageManager;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.net.Uri;
import android.view.Surface;
import android.view.View;
import android.view.ViewGroup;
import android.view.Window;
import android.webkit.RenderProcessGoneDetail;
import android.webkit.WebResourceError;
import android.webkit.WebResourceRequest;
import android.webkit.WebResourceResponse;
import android.webkit.WebSettings;
import android.webkit.WebView;
import android.webkit.WebViewClient;
import android.widget.RelativeLayout;
import android.widget.TextView;

import androidx.core.graphics.Insets;
import androidx.core.view.ViewCompat;
import androidx.core.view.WindowCompat;
import androidx.core.view.WindowInsetsCompat;
import androidx.core.view.WindowInsetsControllerCompat;
import androidx.webkit.WebViewAssetLoader;
import androidx.webkit.WebViewCompat;
import androidx.webkit.WebViewFeature;

import org.Mengine.Base.MengineActivity.ELifecycleState;

import org.json.JSONException;
import org.json.JSONObject;

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.util.Collections;
import java.util.Map;
import java.util.Set;

public class MengineWebViewActivityContent extends MengineActivityContent {
    private static final MengineTag TAG = MengineTag.of("MNGWebView");
    private static final String ORIGIN = "https://mengine.localhost";
    private static final int WEBVIEW_NATIVE_SAFE_AREA_VERSION = 144;

    private WebView m_webView;
    private TextView m_overlay;
    private long m_document;
    private boolean m_safeAreaFallback;
    private boolean m_documentCommitted;

    public MengineWebViewActivityContent(MengineActivity activity) {
        super(activity);
    }

    public WebView getBrowser() {
        WebView browser = m_webView;

        return browser;
    }

    protected MengineWebViewService getWebViewService() {
        MengineWebViewService service = m_activity.getService(MengineWebViewService.class);

        return service;
    }

    @Override
    public boolean isDevelopmentMode() {
        return BuildConfig.DEBUG;
    }

    @Override
    public String getCompanyName() {
        return BuildConfig.MENGINE_WEBVIEW_COMPANY;
    }

    @Override
    public String getProjectName() {
        ApplicationInfo applicationInfo = m_activity.getApplicationInfo();
        PackageManager packageManager = m_activity.getPackageManager();
        CharSequence label = applicationInfo.loadLabel(packageManager);
        String name = label.toString();

        return name;
    }

    @Override
    public int getProjectVersion() {
        MengineApplication application = (MengineApplication)m_activity.getApplication();
        int version = application.getVersionCode();

        return version;
    }

    @Override
    public Surface getSurface() {
        return null;
    }

    private static boolean trusted(Uri uri) {
        String scheme = uri.getScheme();

        if ("https".equals(scheme) == false) {
            return false;
        }

        String host = uri.getHost();

        if ("mengine.localhost".equals(host) == false) {
            return false;
        }

        int port = uri.getPort();

        if (port != -1 && port != 443) {
            return false;
        }

        String userInfo = uri.getUserInfo();

        if (userInfo != null) {
            return false;
        }

        return true;
    }

    @Override
    public void showKeyboard() {
        m_activity.postCommand(() -> {
            if (m_webView == null) {
                return;
            }

            Window window = m_activity.getWindow();
            WindowInsetsControllerCompat controller = WindowCompat.getInsetsController(window, m_webView);

            if (controller == null) {
                return;
            }

            int ime = WindowInsetsCompat.Type.ime();
            controller.show(ime);
        });
    }

    @Override
    public void hideKeyboard() {
        m_activity.postCommand(() -> {
            if (m_webView == null) {
                return;
            }

            Window window = m_activity.getWindow();
            WindowInsetsControllerCompat controller = WindowCompat.getInsetsController(window, m_webView);

            if (controller == null) {
                return;
            }

            int ime = WindowInsetsCompat.Type.ime();
            controller.hide(ime);
        });
    }

    @Override
    public boolean isShowKeyboard() {
        Window window = m_activity.getWindow();
        View decorView = window.getDecorView();
        WindowInsetsCompat insets = ViewCompat.getRootWindowInsets(decorView);

        if (insets == null) {
            return false;
        }

        int ime = WindowInsetsCompat.Type.ime();
        if (insets.isVisible(ime) == false) {
            return false;
        }

        return true;
    }

    @Override
    public void onActivityCreate(RelativeLayout contentView) {
        Window window = m_activity.getWindow();
        WindowCompat.enableEdgeToEdge(window);
        ViewCompat.setOnApplyWindowInsetsListener(contentView, (view, windowInsets) -> {
            int ime = WindowInsetsCompat.Type.ime();
            Insets keyboard = windowInsets.getInsets(ime);
            view.setPadding(keyboard.left, keyboard.top, keyboard.right, keyboard.bottom);
            WebView browser = m_webView;

            if (m_safeAreaFallback == true && browser != null) {
                browser.post(this::updateSafeArea_);
            }

            WindowInsetsCompat.Builder builder = new WindowInsetsCompat.Builder(windowInsets);
            builder.setInsets(ime, Insets.NONE);
            WindowInsetsCompat consumed = builder.build();

            return consumed;
        });
        this.createBrowser(contentView);
    }

    @Override
    public void onContentChanged() {
        View view = m_activity.getContentViewGroup();

        while (view != null && view.getParent() instanceof View parent) {
            parent.setFitsSystemWindows(false);
            parent.setPadding(0, 0, 0, 0);
            view = parent;
        }
    }

    private static boolean isNativeSafeAreaSupported_(PackageInfo _packageInfo) {
        if (_packageInfo == null) {
            return false;
        }

        if (_packageInfo.versionName == null) {
            return false;
        }

        String[] version = _packageInfo.versionName.split("\\.", 2);
        int major;
        try {
            major = Integer.parseInt(version[0]);
        } catch (NumberFormatException error) {
            return false;
        }

        if (major < WEBVIEW_NATIVE_SAFE_AREA_VERSION) {
            return false;
        }

        return true;
    }

    private void updateSafeArea_() {
        if (m_safeAreaFallback == false) {
            return;
        }

        WebView browser = m_webView;

        if (browser == null) {
            return;
        }

        String url = browser.getUrl();

        if (url == null) {
            return;
        }

        Uri uri = Uri.parse(url);

        if (MengineWebViewActivityContent.trusted(uri) == false) {
            return;
        }

        MengineWebViewService service = this.getWebViewService();
        String generation = service.getWebViewDocumentGeneration();

        if (generation.isEmpty() == true) {
            return;
        }

        WindowInsetsCompat windowInsets = ViewCompat.getRootWindowInsets(browser);

        if (windowInsets == null) {
            return;
        }

        View root = browser.getRootView();
        int width = browser.getWidth();
        int height = browser.getHeight();
        int rootWidth = root.getWidth();
        int rootHeight = root.getHeight();

        if (width == 0 || height == 0 || rootWidth == 0 || rootHeight == 0) {
            return;
        }

        int systemBars = WindowInsetsCompat.Type.systemBars();
        int displayCutout = WindowInsetsCompat.Type.displayCutout();
        int types = systemBars | displayCutout;
        Insets insets = windowInsets.getInsets(types);
        int[] rootLocation = new int[2];
        int[] browserLocation = new int[2];
        root.getLocationInWindow(rootLocation);
        browser.getLocationInWindow(browserLocation);
        int x = browserLocation[0] - rootLocation[0];
        int y = browserLocation[1] - rootLocation[1];
        int left = insets.left - x;
        left = Math.max(0, left);
        left = Math.min(left, width);
        int top = insets.top - y;
        top = Math.max(0, top);
        top = Math.min(top, height);
        int right = x + width - (rootWidth - insets.right);
        right = Math.max(0, right);
        right = Math.min(right, width);
        int bottom = y + height - (rootHeight - insets.bottom);
        bottom = Math.max(0, bottom);
        bottom = Math.min(bottom, height);

        JSONObject message = new JSONObject();
        try {
            message.put("generation", generation);
            message.put("kind", "safeArea");
            message.put("width", width);
            message.put("left", left);
            message.put("top", top);
            message.put("right", right);
            message.put("bottom", bottom);
        } catch (JSONException error) {
            throw new IllegalArgumentException(error);
        }

        this.receiveWebViewMessage_(message);
    }

    private void receiveWebViewMessage_(JSONObject _message) {
        WebView browser = m_webView;

        if (browser == null) {
            return;
        }

        String json = String.valueOf(_message);
        String script = "window.Mengine && window.Mengine.receive(" + json + ")";
        browser.evaluateJavascript(script, null);
    }

    private void createBrowser(ViewGroup contentView) {
        try {
            this.initializeBrowser(contentView);
        } catch (RuntimeException error) {
            this.destroyBrowser();
            String message = error.getMessage();
            m_activity.finishWithAlertDialog("Cannot initialize Android WebView: %s", message);
        }
    }

    private void initializeBrowser(ViewGroup contentView) {
        boolean webMessageSupported = WebViewFeature.isFeatureSupported(WebViewFeature.WEB_MESSAGE_LISTENER);
        boolean documentStartSupported = WebViewFeature.isFeatureSupported(WebViewFeature.DOCUMENT_START_SCRIPT);

        if (webMessageSupported == false || documentStartSupported == false) {
            m_activity.finishWithAlertDialog("Android System WebView must support origin-aware messages and document-start scripts. Update WebView and restart the application.");

            return;
        }

        AssetManager assets = m_activity.getAssets();
        String bridge;
        try (java.io.InputStream stream = assets.open("MengineWebViewBridge.js")) {
            bridge = MengineUtils.inputStreamToString(stream);
        } catch (IOException error) {
            String message = error.getMessage();
            m_activity.finishWithAlertDialog("Cannot load the WebView bridge: %s", message);

            return;
        }

        WebView browser = new WebView(m_activity);
        m_webView = browser;
        PackageInfo packageInfo = WebViewCompat.getCurrentWebViewPackage(m_activity);
        boolean nativeSafeArea = MengineWebViewActivityContent.isNativeSafeAreaSupported_(packageInfo);
        m_safeAreaFallback = nativeSafeArea == false;

        if (m_safeAreaFallback == true) {
            browser.addOnLayoutChangeListener((view, left, top, right, bottom, oldLeft, oldTop, oldRight, oldBottom) -> {
                this.updateSafeArea_();
            });
        }

        WebSettings settings = browser.getSettings();
        settings.setJavaScriptEnabled(true);
        settings.setDomStorageEnabled(true);
        settings.setAllowFileAccess(false);
        settings.setAllowContentAccess(false);
        WebViewAssetLoader.AssetsPathHandler pathHandler = new WebViewAssetLoader.AssetsPathHandler(m_activity);
        WebViewAssetLoader.Builder loaderBuilder = new WebViewAssetLoader.Builder();
        loaderBuilder.setDomain("mengine.localhost");
        loaderBuilder.addPathHandler("/", pathHandler);
        WebViewAssetLoader loader = loaderBuilder.build();
        WebViewClient client = new WebViewClient() {
            @Override
            public WebResourceResponse shouldInterceptRequest(WebView view, WebResourceRequest request) {
                Uri url = request.getUrl();
                WebResourceResponse response = loader.shouldInterceptRequest(url);

                if (response != null) {
                    return response;
                }

                if (MengineWebViewActivityContent.trusted(url) == false) {
                    return null;
                }

                Map<String, String> headers = Collections.emptyMap();
                ByteArrayInputStream body = new ByteArrayInputStream(new byte[0]);
                response = new WebResourceResponse("text/plain", "UTF-8", 404, "Not Found", headers, body);

                return response;
            }

            @Override
            public boolean shouldOverrideUrlLoading(WebView view, WebResourceRequest request) {
                if (view != m_webView) {
                    return true;
                }

                Uri url = request.getUrl();

                if (MengineWebViewActivityContent.trusted(url) == true) {
                    return false;
                }

                if (request.isForMainFrame() == true) {
                    String externalUrl = url.toString();
                    MengineWebViewActivityContent.this.queueEvent(MengineWebViewEvent.EXTERNAL, "url", externalUrl);
                }

                return true;
            }

            @Override
            public void onPageStarted(WebView view, String url, Bitmap favicon) {
                if (view != m_webView) {
                    return;
                }

                m_documentCommitted = false;
                MengineWebViewService service = MengineWebViewActivityContent.this.getWebViewService();
                m_document = service.nextWebViewDocument();
                m_overlay.setText("Loading…");
                m_overlay.setVisibility(View.VISIBLE);
                MengineWebViewActivityContent.this.queueEvent(MengineWebViewEvent.NAVIGATION, "url", url);
            }

            @Override
            public void onPageCommitVisible(WebView view, String url) {
                if (view != m_webView) {
                    return;
                }

                Uri uri = Uri.parse(url);

                if (MengineWebViewActivityContent.trusted(uri) == false) {
                    return;
                }

                m_documentCommitted = true;
                MengineWebViewActivityContent.this.queueEvent(MengineWebViewEvent.LOADED, "url", url);
            }

            @Override
            public void onPageFinished(WebView view, String url) {
                if (view != m_webView) {
                    return;
                }

                Uri uri = Uri.parse(url);

                if (MengineWebViewActivityContent.trusted(uri) == true) {
                    MengineWebViewActivityContent.this.updateSafeArea_();
                } else {
                    m_overlay.setVisibility(View.GONE);
                }

                JSONObject history = new JSONObject();
                try {
                    boolean back = view.canGoBack();
                    history.put("back", back);
                    boolean forward = view.canGoForward();
                    history.put("forward", forward);
                } catch (JSONException error) {
                    throw new IllegalStateException(error);
                }

                MengineWebViewActivityContent.this.queueEvent(MengineWebViewEvent.HISTORY, "history", history);
            }

            @Override
            public void onReceivedError(WebView view, WebResourceRequest request, WebResourceError error) {
                if (view != m_webView) {
                    return;
                }

                if (request.isForMainFrame() == false) {
                    return;
                }

                Uri uri = request.getUrl();
                String url = uri.toString();
                String currentUrl = view.getUrl();

                if (url.equals(currentUrl) == false) {
                    return;
                }

                CharSequence description = error.getDescription();
                String message = "Cannot load the page: " + description;
                m_overlay.setText(message);
                m_overlay.setVisibility(View.VISIBLE);
                MengineWebViewActivityContent.this.queueEvent(MengineWebViewEvent.FAILED, "url", url);
            }

            @Override
            public void onReceivedHttpError(WebView view, WebResourceRequest request, WebResourceResponse response) {
                if (view != m_webView) {
                    return;
                }

                if (request.isForMainFrame() == false) {
                    return;
                }

                Uri uri = request.getUrl();
                String url = uri.toString();
                String currentUrl = view.getUrl();

                if (url.equals(currentUrl) == false) {
                    return;
                }

                int status = response.getStatusCode();
                String message = "Cannot load the page: HTTP " + status;
                m_overlay.setText(message);
                m_overlay.setVisibility(View.VISIBLE);
                MengineWebViewActivityContent.this.queueEvent(MengineWebViewEvent.FAILED, "url", url);
            }

            @Override
            public boolean onRenderProcessGone(WebView view, RenderProcessGoneDetail detail) {
                if (view != m_webView) {
                    return true;
                }

                MengineWebViewActivityContent.this.destroyBrowser();
                ViewGroup viewGroup = m_activity.getContentViewGroup();
                MengineWebViewActivityContent.this.createBrowser(viewGroup);

                return true;
            }
        };
        browser.setWebViewClient(client);
        Set<String> origins = Collections.singleton(ORIGIN);
        WebViewCompat.addWebMessageListener(browser, "mengine", origins, (view, message, origin, mainFrame, reply) -> {
            if (view != m_webView) {
                return;
            }

            if (mainFrame == false) {
                return;
            }

            if (MengineWebViewActivityContent.trusted(origin) == false) {
                return;
            }

            if (m_documentCommitted == false) {
                return;
            }

            String data = message.getData();

            if (data == null) {
                return;
            }

            if (data.length() > 1024 * 1024) {
                return;
            }

            JSONObject envelope;
            try {
                envelope = new JSONObject(data);
            } catch (JSONException error) {
                MengineLog.logError(TAG, "Invalid WebView message: %s"
                    , error.getMessage()
                );

                return;
            }

            String envelopeKind = envelope.optString("kind");
            int id = "connect".equals(envelopeKind) == true ? MengineWebViewEvent.LOADED : MengineWebViewEvent.MESSAGE;
            this.queueEvent(id, "message", envelope);
        });
        String script = "if (window.top === window) {" + bridge + "}";
        WebViewCompat.addDocumentStartJavaScript(browser, script, origins);
        ViewGroup.LayoutParams browserLayoutParams = new ViewGroup.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.MATCH_PARENT);
        contentView.addView(browser, browserLayoutParams);
        m_overlay = new TextView(m_activity);
        m_overlay.setText("Loading…");
        m_overlay.setGravity(android.view.Gravity.CENTER);
        m_overlay.setBackgroundColor(0xFFFAFAFA);
        ViewGroup.LayoutParams overlayLayoutParams = new ViewGroup.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.MATCH_PARENT);
        contentView.addView(m_overlay, overlayLayoutParams);
        MengineWebViewService service = this.getWebViewService();
        service.attachWebView(this);
    }

    protected void queueEvent(int id, String key, Object value) {
        JSONObject event = new JSONObject();
        String document = Long.toString(m_document);
        try {
            event.put("document", document);
            event.put(key, value);
        } catch (JSONException error) {
            throw new IllegalArgumentException(error);
        }

        MengineWebViewService service = this.getWebViewService();
        service.queueWebViewEvent(id, event);
    }

    public void handleWebViewCommand(JSONObject command) {
        WebView browser = m_webView;

        if (browser == null) {
            return;
        }

        String document = command.optString("document");

        if (document.isEmpty() == false) {
            String currentDocument = Long.toString(m_document);

            if (document.equals(currentDocument) == false) {
                return;
            }
        }

        switch (command.optString("kind")) {
            case "open": {
                String url = command.optString("url");
                browser.loadUrl(url);
                break;
            }
            case "openAsset": {
                Uri.Builder builder = new Uri.Builder();
                builder.scheme("https");
                builder.authority("mengine.localhost");
                String path = "/" + command.optString("path");
                builder.path(path);
                Uri uri = builder.build();
                String url = uri.toString();
                browser.loadUrl(url);
                break;
            }
            case "reload": {
                browser.reload();
                break;
            }
            case "back": {
                if (browser.canGoBack() == true) {
                    browser.goBack();
                }
                break;
            }
            case "forward": {
                if (browser.canGoForward() == true) {
                    browser.goForward();
                }
                break;
            }
            case "connect": {
                MengineWebViewService service = this.getWebViewService();
                String generation = command.optString("generation");
                service.connectWebViewDocument(generation);
                String quotedGeneration = JSONObject.quote(generation);
                String script = "window.Mengine && window.Mengine.connect(" + quotedGeneration + ")";
                browser.evaluateJavascript(script, value -> {
                    this.updateSafeArea_();
                });
                break;
            }
            case "send": {
                JSONObject message = command.optJSONObject("message");
                this.receiveWebViewMessage_(message);
                break;
            }
            case "appReady": {
                m_overlay.setVisibility(View.GONE);
                break;
            }
            case "snapshot": {
                String id = command.optString("id");
                this.captureSnapshot(id);
                break;
            }
            case "close": {
                this.destroyBrowser();
                break;
            }
            default: {
                break;
            }
        }
    }

    private void captureSnapshot(String id) {
        byte[] bytes = new byte[0];
        int width = m_webView.getWidth();
        int height = m_webView.getHeight();

        if (width > 0 && height > 0 && (long)width * height <= 8 * 1024 * 1024) {
            Bitmap image = null;
            try {
                image = Bitmap.createBitmap(width, height, Bitmap.Config.ARGB_8888);
                Canvas canvas = new Canvas(image);
                m_webView.draw(canvas);
                ByteArrayOutputStream stream = new ByteArrayOutputStream();

                if (image.compress(Bitmap.CompressFormat.PNG, 100, stream) == true) {
                    if (stream.size() <= 8 * 1024 * 1024) {
                        bytes = stream.toByteArray();
                    }
                }
            } catch (OutOfMemoryError error) {
                MengineLog.logError(TAG, "Cannot capture WebView snapshot: %s"
                    , error.getMessage()
                );
            } finally {
                if (image != null) {
                    image.recycle();
                }
            }
        }

        JSONObject snapshot = new JSONObject();
        try {
            snapshot.put("id", id);
            snapshot.put("width", width);
            snapshot.put("height", height);
            String data = android.util.Base64.encodeToString(bytes, android.util.Base64.NO_WRAP);
            snapshot.put("data", data);
        } catch (JSONException error) {
            throw new IllegalStateException(error);
        }

        this.queueEvent(MengineWebViewEvent.SNAPSHOT, "snapshot", snapshot);
    }

    private void destroyBrowser() {
        m_documentCommitted = false;
        MengineWebViewService service = this.getWebViewService();
        service.detachWebView(this);

        if (m_webView != null) {
            ViewGroup parent = (ViewGroup)m_webView.getParent();

            if (parent != null) {
                parent.removeView(m_webView);
            }

            m_webView.stopLoading();
            m_webView.destroy();
            m_webView = null;
        }

        if (m_overlay == null) {
            return;
        }

        ViewGroup parent = (ViewGroup)m_overlay.getParent();

        if (parent != null) {
            parent.removeView(m_overlay);
        }

        m_overlay = null;
    }

    @Override
    public void onActivityDestroy() {
        this.destroyBrowser();
    }

    @Override
    public void onActivityPostDestroy() {
        if (m_activity.isFinishing() == false) {
            return;
        }

        if (m_activity.isChangingConfigurations() == true) {
            return;
        }

        MengineApplication application = (MengineApplication)m_activity.getApplication();
        application.quit();
    }

    @Override
    public void onContentLifecycle(ELifecycleState state) {
        if (m_webView == null) {
            return;
        }

        if (state == ELifecycleState.ACTIVITY_RESUME) {
            m_webView.onResume();
        } else if (state == ELifecycleState.ACTIVITY_PAUSE) {
            m_webView.onPause();
        }
    }

    @Override
    public void onPlatformLifecycle(ELifecycleState state) {
        int id;
        switch (state) {
            case ACTIVITY_START: {
                id = MengineWebViewEvent.ACTIVITY_START;
                break;
            }
            case ACTIVITY_STOP: {
                id = MengineWebViewEvent.ACTIVITY_STOP;
                break;
            }
            case ACTIVITY_RESUME: {
                id = MengineWebViewEvent.ACTIVITY_RESUME;
                break;
            }
            case ACTIVITY_PAUSE: {
                id = MengineWebViewEvent.ACTIVITY_PAUSE;
                break;
            }
            default:
                return;
        }

        MengineWebViewService service = this.getWebViewService();
        service.queueWebViewEvent(id, null);
    }

    @Override
    public void onPlatformFocus(boolean focus) {
        this.queueEvent(MengineWebViewEvent.FOCUS, "focus", focus);
    }

    @Override
    public void onPlatformIntent(Intent intent, boolean start) {
        String url = intent.getDataString();
        this.queueEvent(MengineWebViewEvent.INTENT, "url", url);
    }

    @Override
    public void onPlatformLocale(String locale) {
        this.queueEvent(MengineWebViewEvent.LOCALE, "locale", locale);
    }
}
