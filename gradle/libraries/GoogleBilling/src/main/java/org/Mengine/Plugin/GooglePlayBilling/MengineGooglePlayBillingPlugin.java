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

import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.MenginePlugin;

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

    private List<String> m_skusNames = null;
    private final ArrayList<SkuDetails> m_skusDetails = new ArrayList<SkuDetails>();

    private BillingClient m_billingClient = null;
    private Boolean m_connectionSucces = false;

    private final Boolean m_mutex = true;
    private Boolean m_responseEnd = true;

    @Override
    public void onPythonEmbedding(MengineActivity activity) {
        this.addPythonPlugin("GooglePlayBilling");
    }

    public interface IBillingResponse {
        void skuResponse(List<SkuDetails> priceOffers);

        void consume(ArrayList<String> skus);

        void acknowledge(ArrayList<String> skus);

        void hasAcknowledge(ArrayList<String> skus, CallbackInterface cb);
    }

    private final IBillingResponse m_callbackListener = new IBillingResponse() {
        @Override
        public void skuResponse(List<SkuDetails> priceOffers) {
            List<String> stringArray = new ArrayList<>();

            for (SkuDetails sku : priceOffers) {
                String json = sku.getOriginalJson();

                stringArray.add(json);
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

    public void setSkuList(List<String> skus) {
        this.info("setSkuList: %s", skus);

        m_skusNames = new ArrayList<>(skus);
        this.connectAndQuerySku();
    }

    @Override
    public void onCreate(MengineActivity activity, Bundle savedInstanceState) {
        final PurchasesUpdatedListener purchasesUpdatedListener = new PurchasesUpdatedListener() {
            @Override
            public void onPurchasesUpdated(@NonNull BillingResult billingResult, @Nullable List<Purchase> purchases) {
                int responseCode = billingResult.getResponseCode();

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
                        MengineGooglePlayBillingPlugin.this.log("onPurchasesUpdated: User canceled the purchase");

                        MengineGooglePlayBillingPlugin.this.pythonCall("onGooglePlayBillingUserCanceled");
                        break;
                    }
                    case BillingClient.BillingResponseCode.ITEM_ALREADY_OWNED: {
                        MengineGooglePlayBillingPlugin.this.log("onPurchasesUpdated: The user already owns this item");

                        MengineGooglePlayBillingPlugin.this.queryPurchases();
                        break;
                    }
                    case BillingClient.BillingResponseCode.DEVELOPER_ERROR: {
                        MengineGooglePlayBillingPlugin.this.log(
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
                        MengineGooglePlayBillingPlugin.this.log("onPurchasesUpdated: service disconnected");

                        MengineGooglePlayBillingPlugin.this.connectAndQuerySku();
                        break;
                    }
                    default: {
                        MengineGooglePlayBillingPlugin.this.error("onPurchasesUpdated: [%d] %s"
                                , responseCode
                                , billingResult.getDebugMessage()
                        );

                        break;
                    }
                }
            }
        };

        m_billingClient = BillingClient.newBuilder(activity.getBaseContext())
                .setListener(purchasesUpdatedListener)
                .enablePendingPurchases()
                .build();
    }

    @Override
    public void onResume(MengineActivity activity) {
        this.connectAndQuerySku();
    }

    @Override
    public void onDestroy(MengineActivity activity) {
        m_billingClient.endConnection();
    }

    private void connectAndQuerySku() {
        this.info("connect and query sku");

        if (m_billingClient == null) {
            return;
        }

        if (m_skusNames == null) {
            return;
        }

        if (m_connectionSucces == true) {
            this.querySkuDetails();
        } else {
            m_billingClient.startConnection(new BillingClientStateListener() {
                @Override
                public void onBillingServiceDisconnected() {
                    m_connectionSucces = false;
                }

                @Override
                public void onBillingSetupFinished(@NonNull BillingResult billingResult) {
                    m_connectionSucces = true;

                    int responseCode = billingResult.getResponseCode();

                    if (responseCode != BillingClient.BillingResponseCode.OK) {
                        MengineGooglePlayBillingPlugin.this.error("Billing invalid connection [%u] %s"
                            , responseCode
                            , billingResult.getDebugMessage()
                        );

                        return;
                    }

                    MengineGooglePlayBillingPlugin.this.querySkuDetails();
                    MengineGooglePlayBillingPlugin.this.queryPurchases();
                }
            });
        }
    }

    @AnyThread
    private void querySkuDetails() {
        this.info("query sku details");

        if (m_billingClient == null) {
            return;
        }

        synchronized (m_mutex) {
            if (m_responseEnd == false) {
                return;
            }

            m_responseEnd = false;

            SkuDetailsParams.Builder params = SkuDetailsParams.newBuilder();
            params.setSkusList(m_skusNames).setType(BillingClient.SkuType.INAPP);

            m_billingClient.querySkuDetailsAsync( params.build(), new SkuDetailsResponseListener() {
                @Override
                public void onSkuDetailsResponse(@NonNull BillingResult billingResult, @Nullable List<SkuDetails> skuDetailsList) {
                    m_responseEnd = true;

                    int responseCode = billingResult.getResponseCode();

                    if (responseCode != BillingClient.BillingResponseCode.OK) {
                        MengineGooglePlayBillingPlugin.this.error("Billing invalid query Sku details [%d] %s"
                            , responseCode
                            , billingResult.getDebugMessage()
                        );

                        return;
                    }

                    m_skusDetails.clear();

                    for (SkuDetails skuDetails : skuDetailsList) {
                        m_skusDetails.add(skuDetails);
                    }

                    MengineGooglePlayBillingPlugin.this.info("sku responses details: %s"
                        , m_skusDetails.toString()
                    );

                    m_callbackListener.skuResponse(m_skusDetails);
                }
            });
        }
    }

    private void queryPurchases() {
        this.info("query purchases");

        if (m_billingClient == null) {
            return;
        }

        m_billingClient.queryPurchasesAsync(BillingClient.SkuType.INAPP, (@NonNull BillingResult billingResult, @NonNull List<Purchase> purchasesList) -> {
            int responseCode = billingResult.getResponseCode();

            if (responseCode != BillingClient.BillingResponseCode.OK) {
                MengineGooglePlayBillingPlugin.this.error("Billing invalid query purchases [%d] %s"
                    , responseCode
                    , billingResult.getDebugMessage()
                );

                return;
            }

            for (Purchase purchase : purchasesList) {
                handlePurchase(purchase);
            }
        });
    }

    boolean buyInApp(String sku) {
        this.info("buy InApp sku %s"
            , sku.toString()
        );

        if (m_billingClient == null) {
            return false;
        }

        MengineActivity activity = this.getActivity();

        for (SkuDetails skuDetail : m_skusDetails) {
            if (skuDetail.getSku().equals(sku) == false) {
                continue;
            }

            BillingFlowParams flowParams = BillingFlowParams.newBuilder()
                .setSkuDetails(skuDetail)
                .build();

            BillingResult billingResult = m_billingClient.launchBillingFlow(activity, flowParams);

            int responseCode = billingResult.getResponseCode();

            if (responseCode != BillingClient.BillingResponseCode.OK) {
                this.error("Billing invalid launch billing flow [%d] %s"
                    , responseCode
                    , billingResult.getDebugMessage()
                );

                return false;
            }

            this.info( "buy InApp OK %s"
                , sku
            );

            this.pythonCall("onGooglePlayBillingBuyInAppSuccess", sku, true);

            return true;
        }

        MengineGooglePlayBillingPlugin.this.pythonCall("onGooglePlayBillingBuyInAppSuccess", sku, false);

        return false;
    }

    @AnyThread
    private void handlePurchase(@Nullable Purchase purchase) {
        this.info("handlePurchase state %d %s"
            , purchase.getPurchaseState()
            , purchase.toString()
        );

        if (purchase == null) {
            return;
        }

        int state = purchase.getPurchaseState();

        if (state != Purchase.PurchaseState.PURCHASED) {
            return;
        }

        if (purchase.isAcknowledged() == true) {
            ArrayList<String> skus = purchase.getSkus();

            m_callbackListener.hasAcknowledge(skus, (Object result) -> {
                String token = purchase.getPurchaseToken();

                AcknowledgePurchaseParams.Builder acknowledgePurchaseParams = AcknowledgePurchaseParams.newBuilder()
                    .setPurchaseToken(token);

                m_billingClient.acknowledgePurchase(acknowledgePurchaseParams.build(), billingResult -> {
                    MengineGooglePlayBillingPlugin.this.info("acknowledgePurchase: " + billingResult.getResponseCode() + " " + billingResult.getDebugMessage());

                    int responseCode = billingResult.getResponseCode();

                    if (responseCode != BillingClient.BillingResponseCode.OK) {
                        MengineGooglePlayBillingPlugin.this.error("Billing invalid acknowledge purchase [%d] %s"
                            , responseCode
                            , billingResult.getDebugMessage()
                        );

                        return;
                    }

                    MengineGooglePlayBillingPlugin.this.log("success acknowledge");

                    MengineGooglePlayBillingPlugin.this.m_callbackListener.acknowledge(skus);
                });
            });
        } else {
            String token = purchase.getPurchaseToken();

            ConsumeParams consumeParams = ConsumeParams.newBuilder()
                    .setPurchaseToken(token)
                    .build();

            m_billingClient.consumeAsync(consumeParams, new ConsumeResponseListener() {
                @Override
                public void onConsumeResponse(@NonNull BillingResult billingResult, @NonNull String purchaseToken) {
                    MengineGooglePlayBillingPlugin.this.info("consumeAsync: %d message %s"
                            , billingResult.getResponseCode()
                            , billingResult.getDebugMessage()
                    );

                    int responseCode = billingResult.getResponseCode();

                    if (responseCode != BillingClient.BillingResponseCode.OK) {
                        MengineGooglePlayBillingPlugin.this.error("Billing invalid consume [%d] %s"
                            , responseCode
                            , billingResult.getDebugMessage()
                        );

                        return;
                    }

                    ArrayList<String> skus = purchase.getSkus();

                    MengineGooglePlayBillingPlugin.this.m_callbackListener.consume(skus);
                }
            });
        }
    }
}
