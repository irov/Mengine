package org.Mengine.Base;

import android.content.ClipData;
import android.content.ClipboardManager;
import android.content.Context;
import android.view.View;

public class MengineClipboard extends View implements ClipboardManager.OnPrimaryClipChangedListener {
    public static final String TAG = "MengineClipboard";

    protected ClipboardManager m_cm;

    public MengineClipboard(Context context) {
        super(context);

        ClipboardManager cm = context.getSystemService(ClipboardManager.class);

        m_cm = cm;

        if (m_cm != null) {
            m_cm.addPrimaryClipChangedListener(this);
        }
    }

    public void handleDestroy() {
        if (m_cm != null) {
            m_cm.removePrimaryClipChangedListener(this);
            m_cm = null;
        }
    }

    public boolean hasText() {
        if (m_cm == null) {
            return false;
        }

        boolean result = m_cm.hasPrimaryClip();

        return result;
    }

    public String getText() {
        if (m_cm == null) {
            MengineLog.logWarning(TAG, "invalid get clipboard text [CLIPBOARD_SERVICE is null]");

            return null;
        }

        ClipData clip = m_cm.getPrimaryClip();

        if (clip == null) {
            return null;
        }

        ClipData.Item item = clip.getItemAt(0);

        if (item == null) {
            MengineLog.logWarning(TAG, "invalid get clipboard text [not found items]");

            return null;
        }

        CharSequence text = item.getText();

        if (text == null) {
            MengineLog.logWarning(TAG, "invalid get clipboard text [text is null]");

            return null;
        }

        String text_str = text.toString();

        MengineLog.logMessage(TAG, "get clipboard text: %s"
            , text_str
        );

        return text_str;
    }

    public void setText(String text) {
        if (m_cm == null) {
            MengineLog.logWarning(TAG, "invalid setup clipboard text: %s [CLIPBOARD_SERVICE is null]"
                , text
            );

            return;
        }

        m_cm.removePrimaryClipChangedListener(this);

        ClipData clip = ClipData.newPlainText(null, text);
        m_cm.setPrimaryClip(clip);

        m_cm.addPrimaryClipChangedListener(this);

        MengineLog.logMessage(TAG, "setup clipboard text: %s"
            , text
        );
    }

    @Override
    public void onPrimaryClipChanged() {
        MengineLog.logMessage(TAG, "clipboard changed");

        MengineNative.AndroidPlatform_clipboardChangedEvent();
    }
}