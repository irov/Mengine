package org.Mengine.Plugin.GPlayBilling;

import org.Mengine.Build.MenginePlugin;
import org.Mengine.Build.MengineActivity;

import com.facebook.AccessToken;
import com.facebook.CallbackManager;
import com.facebook.FacebookCallback;
import com.facebook.FacebookException;
import com.facebook.GraphRequest;
import com.facebook.GraphResponse;
import com.facebook.HttpMethod;
import com.facebook.login.LoginManager;
import com.facebook.login.LoginResult;
import com.facebook.share.Sharer;
import com.facebook.share.model.ShareLinkContent;
import com.facebook.share.widget.ShareDialog;
import com.facebook.appevents.AppEventsLogger;

import android.app.Application;
import android.net.Uri;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;

import org.json.JSONException;
import org.json.JSONObject;

import java.util.Arrays;
import java.util.List;

public class MengineBillingPlugin extends MenginePlugin {

    @Override
    public void onPythonEmbedding() {
        MengineActivity activity = this.getActivity();

        this.addPythonPlugin("GPlayBilling");
    }
    

    @Override
    public void onCreate(Bundle savedInstanceState) {
    }
    

    @Override
    public void onMengineInitializeBaseServices()
    {
    }

    @Override
    public void onMengineCreateApplication()
    {
    }
}
