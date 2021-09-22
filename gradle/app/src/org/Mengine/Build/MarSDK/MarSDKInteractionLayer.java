package org.Mengine.Build.MarSDK;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.content.res.Configuration;
import android.os.Build;
import android.os.Handler;
import android.os.Looper;

import com.mar.sdk.MARCode;
import com.mar.sdk.MARSDK;
import com.mar.sdk.PayParams;
import com.mar.sdk.UserExtraData;
import com.mar.sdk.gg.MARGgPlatform;
import com.mar.sdk.gg.control.MggControl;
import com.mar.sdk.platform.MARExitListener;
import com.mar.sdk.platform.MARInitListener;
import com.mar.sdk.platform.MARPlatform;
import com.mar.sdk.plugin.MARUser;
import com.mar.sdk.utils.ResourceHelper;
import com.mar.sdk.verify.UToken;

import org.Mengine.Build.ThreadUtil;

import org.json.JSONObject;

import android.util.Log;

public class MarSDKInteractionLayer implements MARInitListener {

    private static final String TAG = "MarSDK";

    private boolean m_logined = false;
    private Activity m_activity;

    public MarSDKInteractionLayer(Activity _activity) {
        m_activity = _activity;

        MARPlatform.getInstance().init(m_activity, this);
    }

    public void onActivityResult(int requestCode, int resultCode, Intent data) {
        MARSDK.getInstance().onActivityResult(requestCode, resultCode, data);
    }

    public void onStart(){
        MARSDK.getInstance().onStart();
    }

    public void onPause(){
        MARSDK.getInstance().onPause();
    }

    public void onResume(){
        MARSDK.getInstance().onResume();
    }

    public void onNewIntent(Intent intent) {
        MARSDK.getInstance().onNewIntent(intent);
    }

    public void onDestroy() {
        MARSDK.getInstance().onDestroy();
    }

    public void onRestart() {
        MARSDK.getInstance().onRestart();
    }

    public void onConfigurationChanged(Configuration newConfig){
        MARSDK.getInstance().onConfigurationChanged(newConfig);
    }

    public void onRequestPermissionsResult(int requestCode, String[] permissions, int[] grantResults) {
        MARSDK.getInstance().onRequestPermissionResult(requestCode, permissions, grantResults);
    }

    @Override
    public void onInitResult(int code, String msg) {
        Log.d(MarSDKInteractionLayer.TAG, "marsdk.onInitResult code: " + code + " msg:" + msg);

        switch(code){
            case MARCode.CODE_INIT_SUCCESS:
                Log.d(MarSDKInteractionLayer.TAG, "marsdk init success");
                break;
            case MARCode.CODE_INIT_FAIL:
                Log.d(MarSDKInteractionLayer.TAG, "marsdk init fail");
                break;
        }
    }
    
    @Override
    public void onLoginResult(int code, UToken uToken) {
        Log.d(MarSDKInteractionLayer.TAG, "marsdk.onLoginResult code: " + code + " uToken: " + uToken);
        
        switch(code){
            case MARCode.CODE_LOGIN_SUCCESS:
                Log.d(MarSDKInteractionLayer.TAG, "marsdk login success");

                m_logined = true;
                //get control info
                int gameType = MARSDK.getInstance().getGameType();

                Log.d(MarSDKInteractionLayer.TAG, "marsdk login game type: " + gameType);
                
                if ((gameType == 1 || gameType == 3) && !MggControl.getInstance().getFreeFlag()){
                    MggControl.getInstance().reqAdControlInfo();
                }
                break;
            case MARCode.CODE_LOGIN_FAIL:
                Log.d(MarSDKInteractionLayer.TAG, "marsdk login fail");
                break;
        }

        submitExtraData(UserExtraData.TYPE_ENTER_GAME);
    }
    
    @Override
    public void onSwitchAccount(UToken uToken) {
        Log.d(MarSDKInteractionLayer.TAG, "marsdk.onSwitchAccount uToken: " + uToken);
        if(uToken != null){
            m_logined = true;
        }
    }
    
    @Override
    public void onLogout() {
        Log.d(MarSDKInteractionLayer.TAG, "marsdk.onLogout");
        
    }    
    
    @Override
    public void onPayResult(int code, String msg) {
        Log.d(MarSDKInteractionLayer.TAG, "marsdk.onPayResult code: " + code + " msg: " + msg);
        
        try {
            JSONObject json = new JSONObject(msg);
            if (json.getInt("payResult") == 0){
                Log.d("MarSDKInteractionLayer.TAG", "pay complete orderId: " + json.getString("orderId"));
                
                setPropDeliveredComplete(json.getString("orderId"));
            }
        }catch (Exception e){
            e.printStackTrace();
        }
    }

    @Override
    public void onRedeemResult(String msg) {
        Log.d(MarSDKInteractionLayer.TAG, "marsdk.onRedeemResult msg: " + msg);
        
        //ToDo
    }

    @Override
    public void onResult(int code, String msg) {
        Log.d(MarSDKInteractionLayer.TAG, "marsdk.onResult code: " + code + " msg: " + msg);
        
        if(MARCode.CODE_AD_VIDEO_CALLBACK == code){
            //play video callback msg : 1 suc 0 fail
        }
    }

    private void login(){
        MARPlatform.getInstance().login(m_activity);
    }

	private void submitExtraData(final int dataType){
		UserExtraData data = new UserExtraData();
		data.setDataType(dataType);
		data.setMoneyNum(100);
		data.setRoleCreateTime(System.currentTimeMillis()/1000);
		data.setRoleID("100");
		data.setRoleName("test_112");
		data.setRoleLevel("10");
		data.setRoleLevelUpTime(System.currentTimeMillis()/1000);
		data.setServerID(10);
		data.setServerName("server_10");
		MARPlatform.getInstance().submitExtraData(data);
		// queryRealNameInfo();
	}
    
    public void setPropDeliveredComplete(String orderID){
		MARPlatform.getInstance().setPropDeliveredComplete(orderID);
	}
}
