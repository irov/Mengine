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
import com.android.billingclient.api.ProductDetails;
import com.android.billingclient.api.ProductDetailsResponseListener;
import com.android.billingclient.api.Purchase;
import com.android.billingclient.api.PurchasesResponseListener;
import com.android.billingclient.api.PurchasesUpdatedListener;
import com.android.billingclient.api.QueryProductDetailsParams;
import com.android.billingclient.api.QueryPurchasesParams;

import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.MenginePlugin;

import java.util.ArrayList;
import java.util.List;

public class MengineGooglePlayBillingPlugin extends MenginePlugin {
    public static String PLUGIN_NAME = "GooglePlayBilling";
    public static boolean PLUGIN_EMBEDDING = true;

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
     * <p>
     * - onGooglePlayBillingItemAlreadyOwned - покупка уже была куплена
     * - onGooglePlayBillingDeveloperError  - какаята ошиюка в ключах или покупках
     * - onGooglePlayBillingUserCanceled    - покупку отменили покупать (в процессе оплаты)
     **/

    private List<String> m_idsNames = null;
    private final List<ProductDetails> m_productsDetails = new ArrayList<>();

    private BillingClient m_billingClient = null;
    private Boolean m_connectionSucces = false;

    private final Boolean m_mutex = true;
    private Boolean m_responseEnd = true;

    public interface IBillingResponse {
        void skuResponse(List<ProductDetails> priceOffers);

        void consume(List<String> ids);

        void acknowledge(List<String> ids);

        void hasAcknowledge(List<String> ids, CallbackInterface cb);
    }

    private final IBillingResponse m_callbackListener = new IBillingResponse() {
        @Override
        public void skuResponse(List<ProductDetails> priceOffers) {
            List<String> stringArray = new ArrayList<>();

            for (ProductDetails product : priceOffers) {
                stringArray.add(product.toString());
            }

            MengineGooglePlayBillingPlugin.this.pythonCall("onGooglePlayBillingOnSkuResponse", stringArray);
        }

        @Override
        public void consume(List<String> products) {
            MengineGooglePlayBillingPlugin.this.pythonCall("onGooglePlayBillingOnConsume", products);
        }

        @Override
        public void hasAcknowledge(List<String> products, CallbackInterface cb) {
            MengineGooglePlayBillingPlugin.this.pythonCallCb("onGooglePlayBillingIsAcknowledge", cb, products);
        }

        @Override
        public void acknowledge(List<String> products) {
            MengineGooglePlayBillingPlugin.this.pythonCall("onGooglePlayBillingAcknowledge", products);
        }
    };

    public void setSkuList(List<String> ids) {
        this.logInfo("setSkuList: %s", ids);

        m_idsNames = ids;

        this.connectAndQueryProducts();
    }

    @Override
    public void onCreate(MengineActivity activity, Bundle savedInstanceState) {
    }

    @Override
    public void onResume(MengineActivity activity) {
        this.connectAndQueryProducts();
    }

    @Override
    public void onDestroy(MengineActivity activity) {
        if (m_billingClient == null) {
            return;
        }

        m_billingClient.endConnection();
    }

    public void initialize() {
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
                        MengineGooglePlayBillingPlugin.this.logWarning("onPurchasesUpdated: User canceled the purchase");

                        MengineGooglePlayBillingPlugin.this.pythonCall("onGooglePlayBillingUserCanceled");
                        break;
                    }
                    case BillingClient.BillingResponseCode.ITEM_ALREADY_OWNED: {
                        MengineGooglePlayBillingPlugin.this.logWarning("onPurchasesUpdated: The user already owns this item");

                        MengineGooglePlayBillingPlugin.this.queryPurchases();
                        break;
                    }
                    case BillingClient.BillingResponseCode.DEVELOPER_ERROR: {
                        MengineGooglePlayBillingPlugin.this.logError(
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
                        MengineGooglePlayBillingPlugin.this.logWarning("onPurchasesUpdated: service disconnected");

                        MengineGooglePlayBillingPlugin.this.connectAndQueryProducts();
                        break;
                    }
                    default: {
                        MengineGooglePlayBillingPlugin.this.logError("onPurchasesUpdated: unsupported response [%d] %s"
                                , responseCode
                                , billingResult.getDebugMessage()
                        );

                        break;
                    }
                }
            }
        };

        MengineActivity activity = this.getActivity();

        m_billingClient = BillingClient.newBuilder(activity.getBaseContext())
                .setListener(purchasesUpdatedListener)
                .enablePendingPurchases()
                .build();
    }

    private void connectAndQueryProducts() {
        this.logInfo("connect and query sku");

        if (m_billingClient == null) {
            return;
        }

        if (m_idsNames == null) {
            return;
        }

        if (m_connectionSucces == true) {
            this.queryProducts();
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
                        MengineGooglePlayBillingPlugin.this.logError("Billing invalid connection [%u] %s"
                                , responseCode
                                , billingResult.getDebugMessage()
                        );

                        return;
                    }

                    MengineGooglePlayBillingPlugin.this.queryProducts();
                    MengineGooglePlayBillingPlugin.this.queryPurchases();
                }
            });
        }
    }

    @AnyThread
    private void queryProducts() {
        this.logInfo("query sku details");

        if (m_billingClient == null) {
            return;
        }

        synchronized (m_mutex) {
            if (m_responseEnd == false) {
                return;
            }

            m_responseEnd = false;

            List<QueryProductDetailsParams.Product> productList = new ArrayList<>();
            for (String productId : m_idsNames) {
                QueryProductDetailsParams.Product product = QueryProductDetailsParams.Product.newBuilder()
                        .setProductId(productId)
                        .setProductType(BillingClient.ProductType.INAPP)
                        .build();
                productList.add(product);
            }

            QueryProductDetailsParams.Builder params = QueryProductDetailsParams.newBuilder();
            params.setProductList(productList);

            m_billingClient.queryProductDetailsAsync(params.build(), new ProductDetailsResponseListener() {
                @Override
                public void onProductDetailsResponse(@NonNull BillingResult billingResult, @NonNull List<ProductDetails> productDetailList) {
                    m_responseEnd = true;

                    int responseCode = billingResult.getResponseCode();

                    if (responseCode != BillingClient.BillingResponseCode.OK) {
                        MengineGooglePlayBillingPlugin.this.logError("Billing invalid query Sku details [%d] %s"
                                , responseCode
                                , billingResult.getDebugMessage()
                        );

                        return;
                    }

                    m_productsDetails.clear();
                    m_productsDetails.addAll(productDetailList);

                    MengineGooglePlayBillingPlugin.this.logInfo("sku responses details: %s"
                            , m_productsDetails.toString()
                    );

                    m_callbackListener.skuResponse(m_productsDetails);
                }
            });
        }
    }

    private void queryPurchases() {
        this.logInfo("query purchases");

        if (m_billingClient == null) {
            return;
        }

        QueryPurchasesParams purchasesParams = QueryPurchasesParams.newBuilder()
                .setProductType(BillingClient.ProductType.INAPP)
                .build();

        m_billingClient.queryPurchasesAsync(purchasesParams, new PurchasesResponseListener() {
                    @Override
                    public void onQueryPurchasesResponse(@NonNull BillingResult billingResult, @NonNull List<Purchase> purchasesList) {
                        int responseCode = billingResult.getResponseCode();

                        if (responseCode != BillingClient.BillingResponseCode.OK) {
                            MengineGooglePlayBillingPlugin.this.logError("Billing invalid query purchases [%d] %s"
                                    , responseCode
                                    , billingResult.getDebugMessage()
                            );
                            return;
                        }
                        for (Purchase purchase : purchasesList) {
                            handlePurchase(purchase);
                        }
                    }
                });
    }

    boolean buyInApp(String productId) {
        this.logInfo("buy InApp sku %s"
                , productId.toString()
        );

        if (m_billingClient == null) {
            return false;
        }

        MengineActivity activity = this.getActivity();

        for (ProductDetails product : m_productsDetails) {
            if (product.getProductId().equals(productId) == false) {
                continue;
            }

            List<BillingFlowParams.ProductDetailsParams> productDetailsParamsList = new ArrayList<>();

            productDetailsParamsList.add(
                    BillingFlowParams.ProductDetailsParams.newBuilder()
                            .setProductDetails(product)
                            .build());

            BillingFlowParams flowParams = BillingFlowParams.newBuilder()
                    .setProductDetailsParamsList(productDetailsParamsList)
                    .build();

            BillingResult billingResult = m_billingClient.launchBillingFlow(activity, flowParams);

            int responseCode = billingResult.getResponseCode();

            if (responseCode != BillingClient.BillingResponseCode.OK) {
                this.logError("Billing invalid launch billing flow [%d] %s"
                        , responseCode
                        , billingResult.getDebugMessage()
                );

                return false;
            }

            this.logInfo("buy InApp OK %s"
                    , productId
            );

            this.pythonCall("onGooglePlayBillingBuyInAppSuccess", productId, true);

            return true;
        }

        this.pythonCall("onGooglePlayBillingBuyInAppSuccess", productId, false);

        return false;
    }

    @AnyThread
    private void handlePurchase(@Nullable Purchase purchase) {
        if (purchase == null) {
            this.logInfo("handlePurchase state null null");
            return;
        }

        this.logInfo("handlePurchase state %d %s"
                , purchase.getPurchaseState()
                , purchase.toString()
        );


        int state = purchase.getPurchaseState();

        if (state != Purchase.PurchaseState.PURCHASED) {
            return;
        }

        if (purchase.isAcknowledged() == true) {
            List<String> products = purchase.getProducts();

            m_callbackListener.hasAcknowledge(products, (Object result) -> {
                String token = purchase.getPurchaseToken();

                AcknowledgePurchaseParams.Builder acknowledgePurchaseParams = AcknowledgePurchaseParams.newBuilder()
                        .setPurchaseToken(token);

                m_billingClient.acknowledgePurchase(acknowledgePurchaseParams.build(), billingResult -> {
                    MengineGooglePlayBillingPlugin.this.logInfo("acknowledgePurchase: " + billingResult.getResponseCode() + " " + billingResult.getDebugMessage());

                    int responseCode = billingResult.getResponseCode();

                    if (responseCode != BillingClient.BillingResponseCode.OK) {
                        MengineGooglePlayBillingPlugin.this.logError("Billing invalid acknowledge purchase [%d] %s"
                                , responseCode
                                , billingResult.getDebugMessage()
                        );

                        return;
                    }

                    MengineGooglePlayBillingPlugin.this.logInfo("success acknowledge");

                    MengineGooglePlayBillingPlugin.this.m_callbackListener.acknowledge(products);
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
                    MengineGooglePlayBillingPlugin.this.logInfo("consumeAsync: %d message %s"
                            , billingResult.getResponseCode()
                            , billingResult.getDebugMessage()
                    );

                    int responseCode = billingResult.getResponseCode();

                    if (responseCode != BillingClient.BillingResponseCode.OK) {
                        MengineGooglePlayBillingPlugin.this.logError("Billing invalid consume [%d] %s"
                                , responseCode
                                , billingResult.getDebugMessage()
                        );

                        return;
                    }

                    List<String> products = purchase.getProducts();
                    MengineGooglePlayBillingPlugin.this.m_callbackListener.consume(products);
                }
            });
        }
    }
}
