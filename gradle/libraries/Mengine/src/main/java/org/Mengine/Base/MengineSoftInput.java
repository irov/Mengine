package org.Mengine.Base;

import android.content.Context;
import android.text.InputType;
import android.view.KeyEvent;
import android.view.View;
import android.view.inputmethod.BaseInputConnection;
import android.view.inputmethod.EditorInfo;
import android.view.inputmethod.InputConnection;
import android.view.inputmethod.InputMethodManager;

import androidx.annotation.NonNull;

public class MengineSoftInput extends View implements View.OnKeyListener {
    public static final String TAG = "MengineSoftInputView";

    InputMethodManager m_imm;

    View m_prevView;

    public MengineSoftInput(Context context, View prevView) {
        super(context);

        InputMethodManager imm = (InputMethodManager)context.getSystemService(Context.INPUT_METHOD_SERVICE);

        m_imm = imm;

        m_prevView = prevView;

        this.setFocusableInTouchMode(true);
        this.setFocusable(true);
        this.setOnKeyListener(this);
    }

    @Override
    public boolean onCheckIsTextEditor() {
        return true;
    }

    @Override
    public InputConnection onCreateInputConnection(EditorInfo outAttrs) {
        outAttrs.inputType = InputType.TYPE_CLASS_TEXT | InputType.TYPE_TEXT_FLAG_NO_SUGGESTIONS;
        outAttrs.imeOptions = EditorInfo.IME_ACTION_DONE;

        return new BaseInputConnection(this, false);
    }

    @Override
    public boolean onKey(View v, int keyCode, KeyEvent event) {
        int action = event.getAction();

        if (action == KeyEvent.ACTION_UP && keyCode == KeyEvent.KEYCODE_ENTER) {
            this.hideKeyboard();
        }

        return m_prevView.dispatchKeyEvent(event);
    }

    @Override
    public boolean onKeyPreIme(int keyCode, @NonNull KeyEvent event) {
        int action = event.getAction();

        if (action == KeyEvent.ACTION_UP && keyCode == KeyEvent.KEYCODE_BACK) {
            this.hideKeyboard();
        }

        return super.onKeyPreIme(keyCode, event);
    }

    public void showKeyboard() {
        if (m_imm == null) {
            return;
        }

        this.post(() -> {
            this.requestFocus();

            m_imm.showSoftInput(this, InputMethodManager.SHOW_IMPLICIT);

            MengineLog.logMessage(TAG, "input method show succesful");
        });
    }

    public void hideKeyboard() {
        if (m_imm == null) {
            return;
        }

        this.post(() -> {
            m_imm.hideSoftInputFromWindow(this.getWindowToken(), 0);

            MengineLog.logMessage(TAG, "input method hide succesful");

            m_prevView.requestFocus();
        });
    }

    public boolean isShowKeyboard() {
        if (m_imm == null) {
            return false;
        }

        return m_imm.isActive(this);
    }
}