package org.Mengine.Plugin.GooglePlayBilling;

import android.content.Context;
import android.os.Bundle;

import androidx.annotation.AnyThread;
import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import com.android.billingclient.api.AcknowledgePurchaseParams;
import com.android.billingclient.api.AcknowledgePurchaseResponseListener;
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
import org.Mengine.Base.MengineFunctorBoolean;
import org.Mengine.Base.MenginePlugin;
import org.Mengine.Base.MenginePluginActivityListener;
import org.Mengine.Base.MenginePluginInvalidInitializeException;
import org.Mengine.Base.MengineUtils;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

public class MengineGooglePlayBillingPlugin extends MenginePlugin implements MenginePluginActivityListener {
    public static final String PLUGIN_NAME = "GooglePlayBilling";
    public static final boolean PLUGIN_EMBEDDING = true;

    /**
     * Метода для библиотеки
     * <p>
     * billingConnect
     * - onGooglePlayBillingConnectServiceDisconnected
     * - onGooglePlayBillingConnectSetupFinishedFailed
     * - onGooglePlayBillingConnectSetupFinishedSuccessful
     * <p>
     * установить список покупок - по которому будут проходить вся логика
     * + запрос на стоимость товаров
     * queryProducts(List<String> products)
     * - onGooglePlayBillingQueryProductFailed
     * - onGooglePlayBillingQueryProductSuccessful
     * <p>
     * queryPurchases
     * - onGooglePlayBillingQueryPurchasesFailed
     * - onGooglePlayBillingQueryPurchasesSuccessful
     * <p>
     * Соверщить покупку товара
     * boolean buyInApp(String productId)
     * - onGooglePlayBillingBuyInAppFailed(String productId)
     * - onGooglePlayBillingBuyInAppSuccess(String productId)
     * <p>
     * на оба метода падают все каллбеки - в зависимости от ситуации)
     * - onGooglePlayBillingPurchaseIsConsumable(List<String> products, cb)
     * - onGooglePlayBillingPurchaseAcknowledged(List<String> products)
     * - onGooglePlayBillingPurchasesAcknowledgeFailed(List<String> products)
     * - onGooglePlayBillingPurchasesAcknowledgeSuccessful(List<String> products)
     * - onGooglePlayBillingPurchasesOnConsumeFailed(List<String> products)
     * - onGooglePlayBillingPurchasesOnConsumeSuccessful(List<String> products)
     * <p>
     * - onGooglePlayBillingPurchasesUpdatedServiceTimeout
     * - onGooglePlayBillingPurchasesUpdatedFeatureNotSupported
     * - onGooglePlayBillingPurchasesUpdatedServiceDisconnected
     * - onGooglePlayBillingPurchasesUpdatedOk
     * - onGooglePlayBillingPurchasesUpdatedUserCanceled
     * - onGooglePlayBillingPurchasesUpdatedServiceUnavailable
     * - onGooglePlayBillingPurchasesUpdatedItemUnavailable
     * - onGooglePlayBillingPurchasesUpdatedDeveloperError
     * - onGooglePlayBillingPurchasesUpdatedError
     * - onGooglePlayBillingPurchasesUpdatedItemAlreadyOwned
     * - onGooglePlayBillingPurchasesUpdatedItemNotOwned
     * - onGooglePlayBillingPurchasesUpdatedUnknown (int responseCode)
     **/

    private ArrayList<ProductDetails> m_productsDetails;
    private BillingClient m_billingClient;

    @Override
    public void onCreate(MengineActivity activity, Bundle savedInstanceState) throws MenginePluginInvalidInitializeException {
        m_productsDetails = new ArrayList<>();

        PurchasesUpdatedListener purchasesUpdatedListener = new PurchasesUpdatedListener() {
            @Override
            public void onPurchasesUpdated(@NonNull BillingResult billingResult, @Nullable List<Purchase> purchases) {
                int responseCode = billingResult.getResponseCode();
                String debugMessage = billingResult.getDebugMessage();

                switch (responseCode) {
                    case BillingClient.BillingResponseCode.FEATURE_NOT_SUPPORTED: {
                        MengineGooglePlayBillingPlugin.this.logWarning("onPurchasesUpdated [Feature not supported] message: %s"
                            , debugMessage
                        );

                        MengineGooglePlayBillingPlugin.this.pythonCall("onGooglePlayBillingPurchasesUpdatedFeatureNotSupported");
                    }break;
                    case BillingClient.BillingResponseCode.SERVICE_DISCONNECTED: {
                        MengineGooglePlayBillingPlugin.this.logWarning("onPurchasesUpdated [Service disconnected] message: %s"
                            , debugMessage
                        );

                        MengineGooglePlayBillingPlugin.this.pythonCall("onGooglePlayBillingPurchasesUpdatedServiceDisconnected");
                    }break;
                    case BillingClient.BillingResponseCode.OK: {
                        MengineGooglePlayBillingPlugin.this.logMessage("onPurchasesUpdated [User successful the purchase]");

                        if (purchases != null) {
                            for (Purchase purchase : purchases) {
                                MengineGooglePlayBillingPlugin.this.handlePurchase(purchase);
                            }
                        }

                        MengineGooglePlayBillingPlugin.this.pythonCall("onGooglePlayBillingPurchasesUpdatedOk");
                    }break;
                    case BillingClient.BillingResponseCode.USER_CANCELED: {
                        MengineGooglePlayBillingPlugin.this.logWarning("onPurchasesUpdated [User canceled the purchase] message: %s"
                            , debugMessage
                        );

                        MengineGooglePlayBillingPlugin.this.pythonCall("onGooglePlayBillingPurchasesUpdatedUserCanceled");
                    }break;
                    case BillingClient.BillingResponseCode.SERVICE_UNAVAILABLE: {
                        MengineGooglePlayBillingPlugin.this.logWarning("onPurchasesUpdated [Service Unavailable] message: %s"
                            , debugMessage
                        );

                        MengineGooglePlayBillingPlugin.this.pythonCall("onGooglePlayBillingPurchasesUpdatedServiceUnavailable");
                    }break;
                    case BillingClient.BillingResponseCode.BILLING_UNAVAILABLE: {
                        MengineGooglePlayBillingPlugin.this.logWarning("onPurchasesUpdated [Billing Unavailable] message: %s"
                            , debugMessage
                        );

                        MengineGooglePlayBillingPlugin.this.pythonCall("onGooglePlayBillingPurchasesUpdatedBillingUnavailable");
                    }break;
                    case BillingClient.BillingResponseCode.ITEM_UNAVAILABLE: {
                        MengineGooglePlayBillingPlugin.this.logWarning("onPurchasesUpdated [The user item unavailable] message: %s"
                            , debugMessage
                        );

                        MengineGooglePlayBillingPlugin.this.pythonCall("onGooglePlayBillingPurchasesUpdatedItemUnavailable");
                    }break;
                    case BillingClient.BillingResponseCode.DEVELOPER_ERROR: {
                        MengineGooglePlayBillingPlugin.this.logError(
                            "onPurchasesUpdated: Developer error means that Google Play " +
                                "does not recognize the configuration. If you are just getting started, " +
                                "make sure you have configured the application correctly in the " +
                                "Google Play Console. The SKU product ID must match and the APK you " +
                                "are using must be signed with release keys."
                        );

                        MengineGooglePlayBillingPlugin.this.pythonCall("onGooglePlayBillingPurchasesUpdatedDeveloperError");
                    }break;
                    case BillingClient.BillingResponseCode.ERROR: {
                        MengineGooglePlayBillingPlugin.this.logWarning("onPurchasesUpdated [Error] message: %s"
                            , debugMessage
                        );

                        MengineGooglePlayBillingPlugin.this.pythonCall("onGooglePlayBillingPurchasesUpdatedError");
                    }break;
                    case BillingClient.BillingResponseCode.ITEM_ALREADY_OWNED: {
                        MengineGooglePlayBillingPlugin.this.logWarning("onPurchasesUpdated [The user already owns this item] message: %s"
                            , debugMessage
                        );

                        MengineGooglePlayBillingPlugin.this.pythonCall("onGooglePlayBillingPurchasesUpdatedItemAlreadyOwned");
                    }break;
                    case BillingClient.BillingResponseCode.ITEM_NOT_OWNED: {
                        MengineGooglePlayBillingPlugin.this.logWarning("onPurchasesUpdated [The user item not owned] message: %s"
                            , debugMessage
                        );

                        MengineGooglePlayBillingPlugin.this.pythonCall("onGooglePlayBillingPurchasesUpdatedItemNotOwned");
                    }break;
                    default: {
                        MengineGooglePlayBillingPlugin.this.logWarning("onPurchasesUpdated [Unsupported response] code: %d message: %s"
                            , responseCode
                            , debugMessage
                        );

                        MengineGooglePlayBillingPlugin.this.pythonCall("onGooglePlayBillingPurchasesUpdatedUnknown", responseCode);
                    }break;
                }
            }
        };

        Context context = activity.getBaseContext();

        m_billingClient = BillingClient.newBuilder(context)
            .setListener(purchasesUpdatedListener)
            .enablePendingPurchases()
            .build();
    }

    @Override
    public void onDestroy(MengineActivity activity) {
        m_productsDetails = null;

        if (m_billingClient != null) {
            m_billingClient.endConnection();
            m_billingClient = null;
        }
    }

    public void billingConnect() {
        if (m_billingClient == null) {
            this.logError("[ERROR] billingConnect billing client not created");

            return;
        }

        this.logMessage("billingConnect");

        m_billingClient.startConnection(new BillingClientStateListener() {
            @Override
            public void onBillingServiceDisconnected() {
                MengineGooglePlayBillingPlugin.this.logMessage("Billing disconnected");

                MengineGooglePlayBillingPlugin.this.pythonCall("onGooglePlayBillingConnectServiceDisconnected");
            }

            @Override
            public void onBillingSetupFinished(@NonNull BillingResult billingResult) {
                int responseCode = billingResult.getResponseCode();

                if (responseCode != BillingClient.BillingResponseCode.OK) {
                    MengineGooglePlayBillingPlugin.this.logWarning("billing invalid connection responseCode: %d message: %s"
                        , responseCode
                        , billingResult.getDebugMessage()
                    );

                    MengineGooglePlayBillingPlugin.this.pythonCall("onGooglePlayBillingConnectSetupFinishedFailed");

                    return;
                }

                MengineGooglePlayBillingPlugin.this.logMessage("billing setup finished message: %s"
                    , billingResult.getDebugMessage()
                );

                MengineGooglePlayBillingPlugin.this.pythonCall("onGooglePlayBillingConnectSetupFinishedSuccessful");
            }
        });
    }

    public void queryProducts(List<String> products) {
        if (m_billingClient == null) {
            this.logError("[ERROR] queryProducts billing client not created");

            return;
        }

        this.logMessage("queryProducts products: %s"
            , products
        );

        BillingResult billingResult = m_billingClient.isFeatureSupported(BillingClient.FeatureType.PRODUCT_DETAILS);

        if (billingResult.getResponseCode() == BillingClient.BillingResponseCode.FEATURE_NOT_SUPPORTED) {
            this.makeToastLong("GooglePlay billing asks you to update the PlayStore app", 10000L);

            return;
        }

        List<QueryProductDetailsParams.Product> productList = new ArrayList<>();

        for (String productId : products) {
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
            public void onProductDetailsResponse(@NonNull BillingResult billingResult, @NonNull List<ProductDetails> productsDetails) {
                int responseCode = billingResult.getResponseCode();

                if (responseCode != BillingClient.BillingResponseCode.OK) {
                    MengineGooglePlayBillingPlugin.this.logError("[ERROR] billing invalid query product details code: %d message: %s"
                        , responseCode
                        , billingResult.getDebugMessage()
                    );

                    MengineGooglePlayBillingPlugin.this.pythonCall("onGooglePlayBillingQueryProductFailed");

                    return;
                }

                m_productsDetails.clear();
                m_productsDetails.addAll(productsDetails);

                MengineGooglePlayBillingPlugin.this.logMessage("Billing query products details: %s"
                    , m_productsDetails
                );

                List<Map<String, Object>> desc_products = new ArrayList<>();

                for (ProductDetails product : m_productsDetails) {
                    Map<String, Object> desc_product = new HashMap<>();

                    Map<String, Object> desc_offer = new HashMap<>();

                    ProductDetails.OneTimePurchaseOfferDetails oneTimePurchaseOfferDetails = product.getOneTimePurchaseOfferDetails();
                    long priceAmountMicros = oneTimePurchaseOfferDetails.getPriceAmountMicros();
                    String formattedPrice = oneTimePurchaseOfferDetails.getFormattedPrice();
                    String priceCurrencyCode = oneTimePurchaseOfferDetails.getPriceCurrencyCode();

                    desc_offer.put("priceAmountMicros", priceAmountMicros);
                    desc_offer.put("formattedPrice", formattedPrice);
                    desc_offer.put("priceCurrencyCode", priceCurrencyCode);

                    desc_product.put("oneTimePurchaseOfferDetails", desc_offer);

                    String description = product.getDescription();
                    String name = product.getName();
                    String productId = product.getProductId();
                    String productType = product.getProductType();
                    String title = product.getTitle();

                    desc_product.put("description", description);
                    desc_product.put("name", name);
                    desc_product.put("productId", productId);
                    desc_product.put("productType", productType);
                    desc_product.put("title", title);

                    desc_products.add(desc_product);
                }

                MengineGooglePlayBillingPlugin.this.logMessage("Billing response skuResponse: %s"
                    , desc_products
                );

                MengineGooglePlayBillingPlugin.this.pythonCall("onGooglePlayBillingQueryProductSuccessful", desc_products);
            }
        });
    }

    public void queryPurchases() {
        if (m_billingClient == null) {
            this.logError("[ERROR] queryPurchases billing client not created");

            return;
        }

        this.logMessage("queryPurchases");

        QueryPurchasesParams purchasesParams = QueryPurchasesParams.newBuilder()
            .setProductType(BillingClient.ProductType.INAPP)
            .build();

        m_billingClient.queryPurchasesAsync(purchasesParams, new PurchasesResponseListener() {
            @Override
            public void onQueryPurchasesResponse(@NonNull BillingResult billingResult, @NonNull List<Purchase> purchases) {
                int responseCode = billingResult.getResponseCode();

                if (responseCode != BillingClient.BillingResponseCode.OK) {
                    MengineGooglePlayBillingPlugin.this.logError("[ERROR] billing invalid query purchases responseCode: %d message: %s"
                        , responseCode
                        , billingResult.getDebugMessage()
                    );

                    MengineGooglePlayBillingPlugin.this.pythonCall("onGooglePlayBillingQueryPurchasesFailed");

                    return;
                }

                MengineGooglePlayBillingPlugin.this.logMessage("billing successful query message: %s purchases: %s"
                    , billingResult.getDebugMessage()
                    , purchases
                );

                for (Purchase purchase : purchases) {
                    MengineGooglePlayBillingPlugin.this.handlePurchase(purchase);
                }

                MengineGooglePlayBillingPlugin.this.pythonCall("onGooglePlayBillingQueryPurchasesSuccessful");
            }
        });
    }

    protected ProductDetails getProductDetails(String productId) {
        for (ProductDetails product : m_productsDetails) {
            String currentProductId = product.getProductId();

            if (currentProductId.equals(productId) == false) {
                continue;
            }

            return product;
        }

        return null;
    }

    public boolean buyInApp(String productId) {
        if (m_billingClient == null) {
            this.logError("[ERROR] buyInApp billing client not created");

            return false;
        }

        this.logMessage("buyInApp productId: %s"
            , productId
        );

        ProductDetails product = this.getProductDetails(productId);

        if (product == null) {
            this.logError("[ERROR] buyInApp invalid product id: %s"
                , productId
            );

            return false;
        }

        List<BillingFlowParams.ProductDetailsParams> productDetailsParamsList = new ArrayList<>();

        BillingFlowParams.ProductDetailsParams productParams = BillingFlowParams.ProductDetailsParams.newBuilder()
            .setProductDetails(product)
            .build();

        productDetailsParamsList.add(productParams);

        BillingFlowParams flowParams = BillingFlowParams.newBuilder()
            .setProductDetailsParamsList(productDetailsParamsList)
            .build();

        MengineActivity activity = this.getMengineActivity();

        BillingResult billingResult = m_billingClient.launchBillingFlow(activity, flowParams);

        int responseCode = billingResult.getResponseCode();

        if (responseCode != BillingClient.BillingResponseCode.OK) {
            this.logError("[ERROR] billing invalid launch billing flow code: %d message: %s"
                , responseCode
                , billingResult.getDebugMessage()
            );

            this.pythonCall("onGooglePlayBillingBuyInAppFailed", productId);

            return true;
        }

        this.logMessage("buy InApp successful productId: %s message: %s"
            , productId
            , billingResult.getDebugMessage()
        );

        this.pythonCall("onGooglePlayBillingBuyInAppSuccessful", productId);

        return true;
    }

    @AnyThread
    private void handleNonConsumablePurchase(@Nullable Purchase purchase) {
        this.logMessage("handleNonConsumablePurchase purchase: %s"
            , purchase
        );

        List<String> products = purchase.getProducts();
        String token = purchase.getPurchaseToken();

        AcknowledgePurchaseParams.Builder acknowledgePurchaseParams = AcknowledgePurchaseParams.newBuilder()
            .setPurchaseToken(token);

        m_billingClient.acknowledgePurchase(acknowledgePurchaseParams.build(), new AcknowledgePurchaseResponseListener() {
            @Override
            public void onAcknowledgePurchaseResponse(BillingResult billingResult) {
                MengineGooglePlayBillingPlugin.this.logMessage("acknowledgePurchase responseCode: %d debugMessage: %s"
                        , billingResult.getResponseCode()
                        , billingResult.getDebugMessage()
                );

                int responseCode = billingResult.getResponseCode();

                if (responseCode != BillingClient.BillingResponseCode.OK) {
                    MengineGooglePlayBillingPlugin.this.logError("[ERROR] billing invalid acknowledge purchase code: %d message: %s"
                            , responseCode
                            , billingResult.getDebugMessage()
                    );

                    MengineGooglePlayBillingPlugin.this.pythonCall("onGooglePlayBillingPurchasesAcknowledgeFailed", products);

                    return;
                }

                MengineGooglePlayBillingPlugin.this.logMessage("billing success acknowledge purchase: %s"
                        , billingResult.getDebugMessage()
                );

                MengineGooglePlayBillingPlugin.this.pythonCall("onGooglePlayBillingPurchasesAcknowledgeSuccessful", products);
            }
        });
    }

    @AnyThread
    private void handleConsumablePurchase(@Nullable Purchase purchase) {
        this.logMessage("handleConsumablePurchase purchase: %s"
            , purchase
        );

        List<String> products = purchase.getProducts();
        String token = purchase.getPurchaseToken();

        ConsumeParams consumeParams = ConsumeParams.newBuilder()
            .setPurchaseToken(token)
            .build();

        m_billingClient.consumeAsync(consumeParams, new ConsumeResponseListener() {
            @Override
            public void onConsumeResponse(@NonNull BillingResult billingResult, @NonNull String purchaseToken) {
                int responseCode = billingResult.getResponseCode();

                if (responseCode != BillingClient.BillingResponseCode.OK) {
                    MengineGooglePlayBillingPlugin.this.logError("[ERROR] billing invalid consume code: %d message: %s"
                        , responseCode
                        , billingResult.getDebugMessage()
                    );

                    MengineGooglePlayBillingPlugin.this.pythonCall("onGooglePlayBillingPurchasesOnConsumeFailed", products);

                    return;
                }

                MengineGooglePlayBillingPlugin.this.logMessage("billing successful consume: %s"
                    , billingResult.getDebugMessage()
                );

                MengineGooglePlayBillingPlugin.this.pythonCall("onGooglePlayBillingPurchasesOnConsumeSuccessful", products);
            }
        });
    }

    @AnyThread
    private void handlePurchase(@Nullable Purchase purchase) {
        this.logMessage("handlePurchase purchase: %s"
            , purchase
        );

        if (purchase == null) {
            return;
        }

        int state = purchase.getPurchaseState();

        if (state != Purchase.PurchaseState.PURCHASED) {
            return;
        }

        List<String> products = purchase.getProducts();

        MengineFunctorBoolean cb = (boolean isConsumable) -> {
            boolean acknowledged = purchase.isAcknowledged();

            MengineGooglePlayBillingPlugin.this.logMessage("handlePurchase Acknowledged: %s product: %s consumable: %s"
                , acknowledged
                , products
                , isConsumable
            );

            if (acknowledged == true) {
                MengineGooglePlayBillingPlugin.this.pythonCall("onGooglePlayBillingPurchaseAcknowledged", products);

                return;
            }

            if (isConsumable == true) {
                MengineGooglePlayBillingPlugin.this.handleConsumablePurchase(purchase);
            } else {
                MengineGooglePlayBillingPlugin.this.handleNonConsumablePurchase(purchase);
            }
        };

        this.pythonCall("onGooglePlayBillingPurchaseIsConsumable", products, cb);
    }
}
