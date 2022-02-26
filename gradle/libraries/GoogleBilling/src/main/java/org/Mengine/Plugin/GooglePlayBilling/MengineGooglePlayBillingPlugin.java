package org.Mengine.Plugin.GooglePlayBilling;

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

import org.Mengine.Build.MengineActivity;
import org.Mengine.Build.MenginePlugin;

import java.util.ArrayList;
import java.util.List;

public class MengineGooglePlayBillingPlugin extends MenginePlugin {

    /**
     * Метода для библиотеки
     * <p>
     * установить список покупок - по которому будут проходить вся логика
     * + запрос на стоимость товаров
     * setSkuList(List<String> skus)
     * <p>
     * Соверщить покупку товара
     * boolean buyInApp(String sku)
     * - onGooglePlayBillingBuyInAppSuccess - покупка успешно (или нет) начала покупатся)
     * <p>
     * на оба метода падают все каллбеки - в зависимости от ситуации)
     * - onGooglePlayBillingOnSkuResponse   - прайс товаров
     * - onGooglePlayBillingOnConsume       - покупка совершена
     * - onGooglePlayBillingIsAcknowledge   - покупка реализована + Cb
     * - onGooglePlayBillingAcknowledge     - обратный вызов от Google после успешной реализации товара
     *
     * - onGooglePlayBillingDeveloperError  - какаята ошиюка в ключах или покупках
     * - onGooglePlayBillingUserCanceled    - покупку отменили покупать (в процессе оплаты)
     **/

    private List<String> _skusNames = null;
    private final ArrayList<SkuDetails> _skusDetails = new ArrayList<SkuDetails>();

    private BillingClient _billingClient = null;
    private Boolean _connectionSucces = false;

    final Boolean _mutex = true;
    Boolean _responseEnd = true;

    public interface IBillingResponse {
        void skuResponse(List<SkuDetails> priceOffers);

        void consume(ArrayList<String> skus);

        void acknowledge(ArrayList<String> skus);

        void hasAcknowledge(ArrayList<String> skus, CallbackInterface cb);
    }

    private final IBillingResponse _callbackListener = new IBillingResponse() {
        @Override
        public void skuResponse(List<SkuDetails> priceOffers) {
            String[] stringArray = new String[priceOffers.size()];
            for (int i = 0; i < stringArray.length; i++) {
                stringArray[i] = priceOffers.get(i).getOriginalJson();
            }
            MengineGooglePlayBillingPlugin.this.pythonCall("onGooglePlayBillingOnSkuResponse", stringArray);
        }

        @Override
        public void consume(ArrayList<String> skus) {
            String[] stringArray = skus.toArray(new String[0]);
            MengineGooglePlayBillingPlugin.this.pythonCall("onGooglePlayBillingOnConsume", stringArray);
        }

        @Override
        public void hasAcknowledge(ArrayList<String> skus, CallbackInterface cb) {
            String[] stringArray = skus.toArray(new String[0]);
            MengineGooglePlayBillingPlugin.this.pythonCallCb("onGooglePlayBillingIsAcknowledge", cb, stringArray);
        }

        @Override
        public void acknowledge(ArrayList<String> skus) {
            String[] stringArray = skus.toArray(new String[0]);
            MengineGooglePlayBillingPlugin.this.pythonCall("onGooglePlayBillingAcknowledge", stringArray);
        }
    };


    private final PurchasesUpdatedListener _purchasesUpdatedListener = new PurchasesUpdatedListener() {

        @Override
        public void onPurchasesUpdated(@NonNull BillingResult billingResult, @Nullable List<Purchase> purchases) {

            int responseCode = billingResult.getResponseCode();
            log("onPurchasesUpdated: %i %s", responseCode, billingResult.getDebugMessage());

            switch (responseCode) {
                case BillingClient.BillingResponseCode.OK: {
                    if (purchases != null) {
                        for (Purchase purchase : purchases) {
                            MengineGooglePlayBillingPlugin.this.handlePurchase(purchase);
                        }
                    }
                    break;
                }
                case BillingClient.BillingResponseCode.USER_CANCELED: {
                    log("onPurchasesUpdated: User canceled the purchase");
                    MengineGooglePlayBillingPlugin.this.pythonCall("onGooglePlayBillingUserCanceled");
                    break;
                }
                case BillingClient.BillingResponseCode.ITEM_ALREADY_OWNED: {
                    log("onPurchasesUpdated: The user already owns this item");
                    MengineGooglePlayBillingPlugin.this.queryPurchases();
                    break;
                }
                case BillingClient.BillingResponseCode.DEVELOPER_ERROR: {
                    log(
                            "onPurchasesUpdated: Developer error means that Google Play " +
                                    "does not recognize the configuration. If you are just getting started, " +
                                    "make sure you have configured the application correctly in the " +
                                    "Google Play Console. The SKU product ID must match and the APK you " +
                                    "are using must be signed with release keys."
                    );
                    MengineGooglePlayBillingPlugin.this.pythonCall("onGooglePlayBillingDeveloperError");
                    break;
                }
                case BillingClient.BillingResponseCode.SERVICE_DISCONNECTED: {
                    log("SERVICE_DISCONNECTED");
                    MengineGooglePlayBillingPlugin.this.connectAndQuerySku();
                    break;
                }
            }
        }
    };

    @Override
    public void onPythonEmbedding(MengineActivity activity) {
        this.addPythonPlugin("MengineGooglePlayBillingPlugin");
    }

    public void setSkuList(List<String> skus) {
        _skusNames = new ArrayList<>(skus);
        this.connectAndQuerySku();
    }

    @Override
    public void onCreate(MengineActivity activity, Bundle savedInstanceState) {
        super.onCreate(activity, savedInstanceState);
        _billingClient = BillingClient.newBuilder(activity.getBaseContext())
                .setListener(_purchasesUpdatedListener)
                .enablePendingPurchases()
                .build();

    }

    @Override
    public void onResume(MengineActivity activity) {
        super.onResume(activity);

        this.connectAndQuerySku();
    }

    @Override
    public void onDestroy(MengineActivity activity) {
        super.onDestroy(activity);
        _billingClient.endConnection();
    }

    private void connectAndQuerySku() {
        if (_billingClient == null) return;
        if (_skusNames == null) return;

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
                        MengineGooglePlayBillingPlugin.this.querySkuDetails();
                        MengineGooglePlayBillingPlugin.this.queryPurchases();
                    }
                }

            });
        }
    }

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

                            log("sku responses " + billingResult.getResponseCode());
                            _responseEnd = true;
                            if (billingResult.getResponseCode() == BillingClient.BillingResponseCode.OK) {
                                _skusDetails.clear();
                                for (SkuDetails skuDetails : skuDetailsList) {

                                    log(skuDetails.toString());
                                    _skusDetails.add(skuDetails);
                                }
                                _callbackListener.skuResponse(_skusDetails);
                            }

                        }
                    });
//            }

        }
    }

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
            int responseCode = _billingClient.launchBillingFlow(this.getActivity(), flowParams).getResponseCode();
            if (responseCode == BillingClient.BillingResponseCode.OK) {
                log("buyInApp OK %s", sku);
                MengineGooglePlayBillingPlugin.this.pythonCall("onGooglePlayBillingBuyInAppSuccess", true);
                return true;
            }
            break;
        }
        MengineGooglePlayBillingPlugin.this.pythonCall("onGooglePlayBillingBuyInAppSuccess", false);
        return false;
    }

    @AnyThread
    private void handlePurchase(@Nullable Purchase purchase) {
        log("handlePurchase %s", purchase.toString());
        if (purchase == null) return;
        if (purchase.getPurchaseState() != Purchase.PurchaseState.PURCHASED) return;

        log(purchase.toString());

        if (purchase.isAcknowledged()) {
            _callbackListener.hasAcknowledge(purchase.getSkus(), () -> {
                AcknowledgePurchaseParams.Builder acknowledgePurchaseParams = AcknowledgePurchaseParams.newBuilder()
                        .setPurchaseToken(purchase.getPurchaseToken());
//            scopeIO.launch {
                _billingClient.acknowledgePurchase(acknowledgePurchaseParams.build(), billingResult -> {
                    log("acknowledgePurchase: " + billingResult.getResponseCode() + " " + billingResult.getDebugMessage());
                    if (billingResult.getResponseCode() == BillingClient.BillingResponseCode.OK) {
                        _callbackListener.acknowledge(purchase.getSkus());
                        log("success acknowledge");
                    }
                });
            });
//            }
        } else {
            ConsumeParams consumeParams = ConsumeParams.newBuilder()
                    .setPurchaseToken(purchase.getPurchaseToken())
                    .build();
//            scopeIO.launch {
            _billingClient.consumeAsync(consumeParams, new ConsumeResponseListener() {
                @Override
                public void onConsumeResponse(@NonNull BillingResult billingResult, @NonNull String purchaseToken) {
                    log("consumeAsync: " + billingResult.getResponseCode() + " " + billingResult.getDebugMessage());
                    if (billingResult.getResponseCode() == BillingClient.BillingResponseCode.OK) {
                        _callbackListener.consume(purchase.getSkus());
                    }
                }
            });
//            }
        }
    }

}
