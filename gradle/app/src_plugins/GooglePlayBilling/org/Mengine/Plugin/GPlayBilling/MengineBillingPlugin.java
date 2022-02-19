package org.Mengine.Plugin.GPlayBilling;

import android.os.Bundle;

import androidx.annotation.AnyThread;
import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import com.android.billingclient.api.AcknowledgePurchaseParams;
import com.android.billingclient.api.BillingClient;
import com.android.billingclient.api.BillingClientStateListener;
import com.android.billingclient.api.BillingFlowParams;
import com.android.billingclient.api.BillingResult;
import com.android.billingclient.api.ConsumeParams;
import com.android.billingclient.api.ConsumeResponseListener;
import com.android.billingclient.api.Purchase;
import com.android.billingclient.api.PurchasesUpdatedListener;
import com.android.billingclient.api.SkuDetails;
import com.android.billingclient.api.SkuDetailsParams;
import com.android.billingclient.api.SkuDetailsResponseListener;
import com.google.android.datatransport.runtime.logging.Logging;

import org.Mengine.Build.MengineActivity;
import org.Mengine.Build.MenginePlugin;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

public class MengineBillingPlugin extends MenginePlugin {

    private void Log(String msg) {
        Logging.i("GPlayBilling", msg);
    }

    public interface IBillingResponse {
        void skuResponse(List<SkuDetails> priceOffers);

        void consume(ArrayList<String> skus);

        void acknowledge(ArrayList<String> skus);

        boolean isAcknowledge(ArrayList<String> skus);
    }

    private IBillingResponse _callbackListener;
    private List<String> _skusNames = null;
    private final ArrayList<SkuDetails> _skusDetails = new ArrayList();

    private BillingClient _billingClient = null;
    private Boolean _connectionSucces = false;
    private final PurchasesUpdatedListener _purchasesUpdatedListener = new PurchasesUpdatedListener() {

        @Override
        public void onPurchasesUpdated(@NonNull BillingResult billingResult, @Nullable List<Purchase> purchases) {


            int responseCode = billingResult.getResponseCode();
            String debugMessage = billingResult.getDebugMessage();
            Log("onPurchasesUpdated: " + responseCode + " " + debugMessage);

            switch (responseCode) {
                case BillingClient.BillingResponseCode.OK: {
                    if (purchases != null) {
                        for (Purchase purchase : purchases) {
                            handlePurchase(purchase);
                        }
                    }
                    break;
                }
                case BillingClient.BillingResponseCode.USER_CANCELED: {
                    Log("onPurchasesUpdated: User canceled the purchase");
                    break;
                }
                case BillingClient.BillingResponseCode.ITEM_ALREADY_OWNED: {
                    Log("onPurchasesUpdated: The user already owns this item");
                    queryPurchases();
                    break;
                }
                case BillingClient.BillingResponseCode.DEVELOPER_ERROR: {
                    Log(
                            "onPurchasesUpdated: Developer error means that Google Play " +
                                    "does not recognize the configuration. If you are just getting started, " +
                                    "make sure you have configured the application correctly in the " +
                                    "Google Play Console. The SKU product ID must match and the APK you " +
                                    "are using must be signed with release keys."
                    );
                    break;
                }
                case BillingClient.BillingResponseCode.SERVICE_DISCONNECTED: {
                    Log("SERVICE_DISCONNECTED");
                    connectAndQuerySku();
                    break;
                }
            }
        }
    };


    @Override
    public void onPythonEmbedding(MengineActivity activity) {
        super.onPythonEmbedding(activity);
        this.addPythonPlugin("GPlayBilling");
    }

    @Override
    public void onCreate(MengineActivity activity, Bundle savedInstanceState) {
        super.onCreate(activity, savedInstanceState);


        _skusNames = Arrays.asList("name_1", "name_2", "name_3");
        _callbackListener = new IBillingResponse() {
            @Override
            public void skuResponse(List<SkuDetails> priceOffers) {
                //пришол товар / относительно списка запрошенных - только InApps ()
                for (SkuDetails skuDetails : priceOffers) {
                    Log("" + skuDetails.getSku() + " " + skuDetails.getPrice());
                }
            }

            @Override
            public void consume(ArrayList<String> skus) {
                //обратный вызов после успешной платёжки
            }

            @Override
            public boolean isAcknowledge(ArrayList<String> skus) {

                //проверка - реализовали ли мы товар в игре
                //если да то сбрасываем чекер покупки и можем покупть новую
                //если нето то купить ещё нельзя - через 3 дня уничтожается покупка
                return false;
            }

            @Override
            public void acknowledge(ArrayList<String> skus) {

            }
        };
        _billingClient = BillingClient.newBuilder(activity.getBaseContext())
                .setListener(_purchasesUpdatedListener)
                .enablePendingPurchases()
                .build();
    }

    MengineActivity _activity = null;

    @Override
    public void onResume(MengineActivity activity) {
        super.onResume(activity);

        this._activity = activity;
        connectAndQuerySku();
    }

    @Override
    public void onPause(MengineActivity activity) {
        super.onPause(activity);
        this._activity = null;
    }

    private void connectAndQuerySku() {
        if (_activity == null) return;
        if (_billingClient == null) return;

        if (_connectionSucces) {
            querySkuDetails();
        } else {
            _billingClient.startConnection(new BillingClientStateListener() {
                @Override
                public void onBillingServiceDisconnected() {
                    _connectionSucces = false;
                }

                @Override
                public void onBillingSetupFinished(@NonNull BillingResult billingResult) {
                    _connectionSucces = true;
                    if (billingResult.getResponseCode() == BillingClient.BillingResponseCode.OK) {
                        querySkuDetails();
                        queryPurchases();
                    }
                }

            });
        }
    }

    final Boolean _mutex = true;
    Boolean _responseEnd = true;

    //Запрос на прайс покупок, по списку необходимых товаров
    //делаю при каждом заходе в приложение  onResume - можно оставить только после успешного запроса
    @AnyThread
    private void querySkuDetails() {
        if (_billingClient == null) return;
        synchronized (_mutex) {
            if (!_responseEnd) return;
            _responseEnd = false;

            SkuDetailsParams.Builder params = SkuDetailsParams.newBuilder();
            params.setSkusList(_skusNames).setType(BillingClient.SkuType.INAPP);

//            scopeIO.launch {
            _billingClient.querySkuDetailsAsync(
                    params.build(),
                    new SkuDetailsResponseListener() {
                        @Override
                        public void onSkuDetailsResponse(@NonNull BillingResult billingResult, @Nullable List<SkuDetails> skuDetailsList) {

                            Log("sku responses " + billingResult.getResponseCode());
                            _responseEnd = true;
                            if (billingResult.getResponseCode() == BillingClient.BillingResponseCode.OK) {
                                _skusDetails.clear();
                                for (SkuDetails skuDetails : skuDetailsList) {

                                    Log(skuDetails.toString());
                                    _skusDetails.add(skuDetails);
                                }
                                _callbackListener.skuResponse(_skusDetails);
                            }

                        }
                    });
//            }

        }
    }

    //Запрос на покупки которые ещё неиспользовали
    //все из нужно обработать - иначе всё удалится
    private void queryPurchases() {
        _billingClient.queryPurchasesAsync(BillingClient.SkuType.INAPP, (billingResult, purchasesList) -> {
            for (Purchase purchase : purchasesList) {
                handlePurchase(purchase);
            }
        });
    }

    boolean buyInApp(String sku) {
        if (_billingClient == null) return false;
        for (SkuDetails skuDetail : _skusDetails) {
            if (!skuDetail.getSku().equals(sku)) continue;

            BillingFlowParams flowParams = BillingFlowParams.newBuilder()
                    .setSkuDetails(skuDetail)
                    .build();
            int responseCode = _billingClient.launchBillingFlow(_activity, flowParams).getResponseCode();
            if (responseCode == BillingClient.BillingResponseCode.OK) {
                Log("buyInApp OK " + sku);
                return true;
            }
            break;
        }
        return false;
    }

    @AnyThread
    private void handlePurchase(Purchase purchase) {
        Log("handlePurchase " + purchase);
        if (purchase == null) return;
        if (purchase.getPurchaseState() != Purchase.PurchaseState.PURCHASED) return;

        Log(purchase.toString());

        //использовали ли мы покупку
        if (purchase.isAcknowledged()) {
//            покупка уже использована(признана что её покупали и заплатили)
//            нужно сбросить исползование прежде чем купить новую
//            ожидание на сброс 3 дня - после анулируется

            //!!!!!!!!!!!!!!нужно проверить реализовли ли мы купленный товар
            // если нет то реализуем а после сбрасываем чекер

            if (_callbackListener.isAcknowledge(purchase.getSkus())) {
                AcknowledgePurchaseParams.Builder acknowledgePurchaseParams = AcknowledgePurchaseParams.newBuilder()
                        .setPurchaseToken(purchase.getPurchaseToken());
//            scopeIO.launch {
                _billingClient.acknowledgePurchase(acknowledgePurchaseParams.build(), billingResult -> {
                    Log("acknowledgePurchase: " + billingResult.getResponseCode() + " " + billingResult.getDebugMessage());
                    if (billingResult.getResponseCode() == BillingClient.BillingResponseCode.OK) {
                        _callbackListener.acknowledge(purchase.getSkus());
                        Log("success acknowledge");
                    }
                });
            }
//            }
        } else {
//            покупку неиспользовали - новый запрос
            ConsumeParams consumeParams = ConsumeParams.newBuilder()
                    .setPurchaseToken(purchase.getPurchaseToken())
                    .build();
//            scopeIO.launch {
            _billingClient.consumeAsync(consumeParams, new ConsumeResponseListener() {
                @Override
                public void onConsumeResponse(@NonNull BillingResult billingResult, @NonNull String purchaseToken) {
                    Log("consumeAsync: " + billingResult.getResponseCode() + " " + billingResult.getDebugMessage());
                    if (billingResult.getResponseCode() == BillingClient.BillingResponseCode.OK) {
                        _callbackListener.consume(purchase.getSkus());
                    }
                }
            });
//            }
            //одноразовые покупки - нужно подтвердить после приобритения (иначе они отменятся автоматом через 3 дня)
        }
    }

}
