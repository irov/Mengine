-keep public class org.Mengine.Base.MengineWebViewService {
    public boolean isWebViewQuitRequested();
    public org.Mengine.Base.MengineWebViewEvent pollWebViewEvent();
    public byte[] getWebViewHandlerNames();
    public void postWebViewCommand(byte[]);
    public void onWebViewRuntimeFailed();
    public java.lang.String getWebViewTemporaryDirectory();
}

-keep public class org.Mengine.Base.MengineWebViewEvent {
    public final int id;
    public final org.json.JSONObject data;
}
