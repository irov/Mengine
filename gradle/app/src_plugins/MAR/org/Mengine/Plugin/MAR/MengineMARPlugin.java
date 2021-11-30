package org.Mengine.Plugin.MAR;

import org.Mengine.Build.MenginePlugin;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.DialogInterface;
import android.content.ClipData;
import android.content.ClipboardManager;
import android.content.Context;

import android.content.Intent;
import android.content.res.Configuration;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.os.Handler;
import android.os.Looper;

import com.mar.sdk.MARCallBack;
import com.mar.sdk.SDKParams;
import com.mar.sdk.IAction;
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

import org.Mengine.Build.MengineActivity;
import org.Mengine.Build.ThreadUtil;

import org.json.JSONObject;

import android.util.Log;
import android.view.KeyEvent;

import java.util.Calendar;
import java.util.TimeZone;

public class MengineMARPlugin extends MenginePlugin implements MARInitListener {
    private static final String TAG = "MarSDK";

    public void onPythonEmbedding() {
        MengineActivity activity = this.getActivity();

        activity.addPythonPlugin("MarSDK", this);
    }

    public void initialize() {
        MengineActivity activity = this.getActivity();
        MengineMARPlugin self = this;
        ThreadUtil.performOnMainThread(new Runnable() {
            @Override
            public void run() {
                MARPlatform.getInstance().init(activity, self);
            }
        });
    }

    public void login(){
        MengineActivity activity = this.getActivity();

        MARPlatform.getInstance().login(activity);
    }

    public void loginCustom(final String loginType){
        MengineActivity activity = this.getActivity();

        MARPlatform.getInstance().loginCustom(activity, loginType);
    }

    public void pay(int coinNum, String productID,
                                 String productName, String productDesc, int price){
        PayParams params = new PayParams();
        params.setCoinNum(coinNum);        //当前玩家身上拥有的游戏币数量

        params.setPrice(price);        //单位 元
        params.setProductId(productID);    //产品ID
        params.setProductName(productName);    //产品名称
        params.setProductDesc(productDesc);    //产品描述

        params.setBuyNum(1);		//购买数量，固定1

        params.setExtension(System.currentTimeMillis()+"");	//游戏自定义数据，充值成功，回调游戏服的时候，会原封不动返回
        params.setRoleId("100");		//角色ID
        params.setRoleLevel(10);		//角色等级
        params.setRoleName("test_112");	//角色名称
        params.setServerId("10");		//服务器ID
        params.setServerName("测试");		//服务器名称
        params.setVip("1");			//角色VIP等级

        // params.setPayNotifyUrl("http://www.game.com/pay/callback");
        MengineActivity activity = this.getActivity();

        MARPlatform.getInstance().pay(activity, params);
    }

    public void redeemCode(String code)
    {
        Log.d(TAG, "try redeem code " + code);
        MARPlatform.getInstance().exchangeGift(code);
    }

    public void showAd(){
        if (MARGgPlatform.getInstance().getVideoFlag()) {
            MARGgPlatform.getInstance().showVideo();
        }
    }

    public void updateData(String json_str){
        MARPlatform.getInstance().updateGameArchive(json_str,1);
    }

    public void marSDKGetData(int serialNumber){
        MengineActivity activity = this.getActivity();

        MARPlatform.getInstance().getGameArchive(serialNumber, new MARCallBack() {
            @Override
            public void onCallBack(String var1) {
                Log.d(TAG, "get data from server, start...");

                activity.pythonCall("onMarSDKGetData", var1);
            }
        });
    }

    @Override
    public void onCreate(Bundle savedInstanceState)
    {
    }
    @Override
    public void onMengineInitializeBaseServices()
    {
    }
    @Override
    public void onMengineCreateApplication()
    {
    }

    @Override
    public void onActivityResult(int requestCode, int resultCode, Intent data) {
        MARSDK.getInstance().onActivityResult(requestCode, resultCode, data);
    }

    @Override
    public void onStart(){
        MARSDK.getInstance().onStart();
    }

    @Override
    public void onPause(){
        MARSDK.getInstance().onPause();
    }

    @Override
    public void onResume(){
        MARSDK.getInstance().onResume();
    }

    @Override
    public void onNewIntent(Intent intent) {
        MARSDK.getInstance().onNewIntent(intent);
    }

    @Override
    public void onDestroy() {
        MARSDK.getInstance().onDestroy();
    }

    @Override
    public void onRestart() {
        MARSDK.getInstance().onRestart();
    }

    @Override
    public void onConfigurationChanged(Configuration newConfig){
        MARSDK.getInstance().onConfigurationChanged(newConfig);
    }

    @Override
    public boolean dispatchKeyEvent(KeyEvent event) {
        int action = event.getAction();
        int keyCode = event.getKeyCode();

        if(keyCode == KeyEvent.KEYCODE_BACK && action == KeyEvent.ACTION_DOWN ) {
            MengineActivity activity = this.getActivity();

            MARPlatform.getInstance().exitSDK(new MARExitListener() {
                @Override
                public void onGameExit() {
                    //游戏自己的退出确认框

                    AlertDialog.Builder builder = new AlertDialog.Builder(activity);
                    builder.setTitle("退出确认");
                    builder.setMessage("主公，现在还早，要不要再玩一会？");
                    builder.setCancelable(true);
                    builder.setPositiveButton("好吧",
                            new DialogInterface.OnClickListener() {
                                public void onClick(DialogInterface dialog,
                                                    int whichButton) {
                                    //这里什么都不用做
                                }
                            });
                    builder.setNeutralButton("一会再玩",
                            new DialogInterface.OnClickListener() {
                                public void onClick(DialogInterface dialog,
                                                    int whichButton) {
                                    //退出游戏

                                    activity.quitMengineApplication();
                                }
                            });

                    builder.show();
                }
            });

            return true;
        }

        return false;
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, String[] permissions, int[] grantResults) {
        MARSDK.getInstance().onRequestPermissionResult(requestCode, permissions, grantResults);
    }

    @Override
    public void onInitResult(int code, String msg) {
        Log.d(TAG, "marsdk.onInitResult code: " + code + " msg:" + msg);

        MengineActivity activity = this.getActivity();

        switch(code){
            case MARCode.CODE_INIT_SUCCESS:
                Log.d(TAG, "marsdk init success");

                activity.pythonCall("onMarSDKInitSuccess");
                break;
            case MARCode.CODE_INIT_FAIL:
                Log.d(TAG, "marsdk init fail");

                activity.pythonCall("onMarSDKInitFail");
                break;
        }
    }

    @Override
    public void onLoginResult(int code, UToken uToken) {
        Log.d(TAG, "marsdk.onLoginResult code: " + code + " uToken: " + uToken);

        MengineActivity activity = this.getActivity();
        
        switch(code){
            case MARCode.CODE_LOGIN_SUCCESS:
                Log.d(TAG, "marsdk login success");

                //get control info
                int gameType = MARSDK.getInstance().getGameType();

                Log.d(TAG, "marsdk login game type: " + gameType);
                
                if ((gameType == 1 || gameType == 3) && !MggControl.getInstance().getFreeFlag()){
                    MggControl.getInstance().reqAdControlInfo();
                }

                submitExtraData(UserExtraData.TYPE_ENTER_GAME);

                activity.pythonCall("onMarSDKLoginSuccess");

                //m_activity.getData(1);
                break;
            case MARCode.CODE_LOGIN_FAIL:
                Log.d(TAG, "marsdk login fail");

                if (MARSDK.getInstance().getGameType() == 1){
                    MARPlatform.getInstance().visitorLogin();
                }

                activity.pythonCall("onMarSDKLoginFail");
                break;
        }
    }
    
    @Override
    public void onSwitchAccount(UToken uToken) {
        Log.d(TAG, "marsdk.onSwitchAccount uToken: " + uToken);
        MengineActivity activity = this.getActivity();

        if(uToken != null){
            activity.pythonCall("onMarSDKSwitchAccount");
        }
    }
    
    @Override
    public void onLogout() {
        Log.d(TAG, "marsdk.onLogout");

        MengineActivity activity = this.getActivity();

        activity.pythonCall("onMarSDKLogout");
    }    
    
    @Override
    public void onPayResult(int code, String msg) {
        Log.d(TAG, "marsdk.onPayResult code: " + code + " msg: " + msg);

        MengineActivity activity = this.getActivity();
        
        try {
            JSONObject json = new JSONObject(msg);

            String productId = json.getString("productId");

            if (json.getInt("payResult") == 0){
                Log.d(TAG, "pay complete orderId: " + json.getString("orderId"));
                setPropDeliveredComplete(json.getString("orderId"));

                activity.pythonCall("onMarSDKPaySuccess", productId);
            }
            else
            {
                Log.d(TAG, "pay fail");

                activity.pythonCall("onMarSDKPayFail", productId);
            }
        }catch (Exception e){
            Log.d(TAG, "pay error");
            e.printStackTrace();
        }
    }

    @Override
    public void onRedeemResult(String msg) {
        Log.d(TAG, "marsdk.onRedeemResult msg: " + msg);

        MengineActivity activity = this.getActivity();

        int result;
        try {
            result = 0;

            JSONObject json = new JSONObject(msg);

            int propNumber = json.getInt("propNumber");
            String propType = json.getString("propType");
            String message = json.getString("msg");

            activity.pythonCall("onMarSDKRedeemResult", result, propNumber, propType, message);
        }
        catch(Exception e) {
            Log.d(TAG, "redeem error");

            result = -1;

            activity.pythonCall("onMarSDKRedeemResult", result, 0, "", "");
            e.printStackTrace();
        }
    }

    @Override
    public void onResult(int code, String msg) {
        Log.d(TAG, "marsdk.onResult code: " + code + " msg: " + msg);

        MengineActivity activity = this.getActivity();
        
        if(MARCode.CODE_AD_VIDEO_CALLBACK == code){
            //play video callback msg : 1 suc 0 fail
            Log.d(TAG, "Video callback: " + msg);
            String watchAdTime = getCurrentTime();

            activity.pythonCall("onMarSDKAdVideoCallback", msg, watchAdTime);
        }

        activity.pythonCall("onMarSDKResult", code, msg);
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

	public void pasteCode() {
        try {
            String code = null;

            MengineActivity activity = this.getActivity();

            /////////////////////////////////
            ClipboardManager clipboard = (ClipboardManager) activity.getContext().getSystemService(Context.CLIPBOARD_SERVICE);
            if (clipboard.hasPrimaryClip()) {
                ClipData clip = clipboard.getPrimaryClip();
                ClipData.Item item = clip.getItemAt(0);

                Object data = item.getText();
                if (data == null) {
                    code = null;
                } else
                    code = data.toString();
            }
            /////////////////////////////////

            Log.d(TAG, "try to paste code: \"" + code + "\"");

            activity.pythonCall("onMarSDKSaveClipboard", code);
        }
        catch (Exception e)
        {
            Log.d(TAG, "paste code error");
            e.printStackTrace();
        }
    }

    public String getCurrentTime()
    {
        TimeZone tz = TimeZone.getTimeZone("Asia/Beijing");
        Calendar calendar = Calendar.getInstance(tz);
        String time = String.valueOf(
                calendar.get(Calendar.DATE)) + "/" +
                String.valueOf(calendar.get(Calendar.MONTH)) + "/" +
                String.valueOf(calendar.get(Calendar.YEAR));

        Log.d(TAG, "China (Beijing) time: " + time);

        return time;
    }
}
