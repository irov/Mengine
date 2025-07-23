package org.Mengine.Plugin.GooglePlayBilling;

import android.os.Bundle;

import androidx.annotation.AnyThread;
import androidx.annotation.NonNull;

import com.android.billingclient.api.AcknowledgePurchaseParams;
import com.android.billingclient.api.BillingClient;
import com.android.billingclient.api.BillingClientStateListener;
import com.android.billingclient.api.BillingFlowParams;
import com.android.billingclient.api.BillingResult;
import com.android.billingclient.api.ConsumeParams;
import com.android.billingclient.api.PendingPurchasesParams;
import com.android.billingclient.api.ProductDetails;
import com.android.billingclient.api.Purchase;
import com.android.billingclient.api.PurchasesUpdatedListener;
import com.android.billingclient.api.QueryProductDetailsParams;
import com.android.billingclient.api.QueryPurchasesParams;
import com.android.billingclient.api.UnfetchedProduct;

import org.Mengine.Base.MengineActivity;
import org.Mengine.Base.MengineApplication;
import org.Mengine.Base.MengineCallback;
import org.Mengine.Base.MengineFragmentInAppPurchase;
import org.Mengine.Base.MengineListenerRemoteConfig;
import org.Mengine.Base.MengineParamInAppProduct;
import org.Mengine.Base.MengineParamInAppPurchase;
import org.Mengine.Base.MengineListenerActivity;
import org.Mengine.Base.MengineListenerApplication;
import org.Mengine.Base.MenginePreferences;
import org.Mengine.Base.MengineService;
import org.Mengine.Base.MengineServiceInvalidInitializeException;
import org.Mengine.Base.MengineUI;
import org.Mengine.Base.MengineUtils;
import org.json.JSONArray;
import org.json.JSONObject;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Objects;
import java.util.Set;

public class MengineGooglePlayBillingPlugin extends MengineService implements MengineListenerApplication, MengineListenerActivity {
    public static final String SERVICE_NAME = "GPlayBilling";
    public static final boolean SERVICE_EMBEDDING = true;

    private static final int ERROR_CODE_UNKNOWN = 0;
    private static final int ERROR_CODE_NOT_FOUND = 1;
    private static final int ERROR_CODE_NOT_SUPPORTED = 2;
    private static final int ERROR_CODE_NOT_INITIALIZED = 3;
    private static final int ERROR_CODE_NOT_READY = 4;

    private List<ProductDetails> m_productsDetails;
    private BillingClient m_billingClient;

    @Override
    public void onAppCreate(@NonNull MengineApplication application) throws MengineServiceInvalidInitializeException {
        m_productsDetails = new ArrayList<>();

        PurchasesUpdatedListener purchasesUpdatedListener = (billingResult, purchases) -> {
            int responseCode = billingResult.getResponseCode();
            String debugMessage = billingResult.getDebugMessage();

            switch (responseCode) {
                case BillingClient.BillingResponseCode.FEATURE_NOT_SUPPORTED: {
                    this.logWarning("onPurchasesUpdated [Feature not supported] message: %s"
                        , debugMessage
                    );

                    this.nativeCall("onGooglePlayBillingPurchasesUpdatedFeatureNotSupported");
                }break;
                case BillingClient.BillingResponseCode.SERVICE_DISCONNECTED: {
                    this.logWarning("onPurchasesUpdated [Service disconnected] message: %s"
                        , debugMessage
                    );

                    this.nativeCall("onGooglePlayBillingPurchasesUpdatedServiceDisconnected");
                }break;
                case BillingClient.BillingResponseCode.OK: {
                    this.logInfo("onPurchasesUpdated [User success the purchase]");

                    if (purchases != null) {
                        this.handlePurchases(purchases);
                    }

                    this.nativeCall("onGooglePlayBillingPurchasesUpdatedOk");
                }break;
                case BillingClient.BillingResponseCode.USER_CANCELED: {
                    this.logWarning("onPurchasesUpdated [User canceled the purchase] message: %s"
                        , debugMessage
                    );

                    this.nativeCall("onGooglePlayBillingPurchasesUpdatedUserCanceled");
                }break;
                case BillingClient.BillingResponseCode.SERVICE_UNAVAILABLE: {
                    this.logWarning("onPurchasesUpdated [Service Unavailable] message: %s"
                        , debugMessage
                    );

                    this.nativeCall("onGooglePlayBillingPurchasesUpdatedServiceUnavailable");
                }break;
                case BillingClient.BillingResponseCode.BILLING_UNAVAILABLE: {
                    this.logWarning("onPurchasesUpdated [Billing Unavailable] message: %s"
                        , debugMessage
                    );

                    this.nativeCall("onGooglePlayBillingPurchasesUpdatedBillingUnavailable");
                }break;
                case BillingClient.BillingResponseCode.ITEM_UNAVAILABLE: {
                    this.logWarning("onPurchasesUpdated [The user item unavailable] message: %s"
                        , debugMessage
                    );

                    this.nativeCall("onGooglePlayBillingPurchasesUpdatedItemUnavailable");
                }break;
                case BillingClient.BillingResponseCode.DEVELOPER_ERROR: {
                    this.logError(
                        "onPurchasesUpdated: Developer error means that Google Play " +
                            "does not recognize the configuration. If you are just getting started, " +
                            "make sure you have configured the application correctly in the " +
                            "Google Play Console. The SKU product ID must match and the APK you " +
                            "are using must be signed with release keys."
                    );

                    this.nativeCall("onGooglePlayBillingPurchasesUpdatedDeveloperError");
                }break;
                case BillingClient.BillingResponseCode.ERROR: {
                    this.logWarning("onPurchasesUpdated [Error] message: %s"
                        , debugMessage
                    );

                    this.nativeCall("onGooglePlayBillingPurchasesUpdatedError");
                }break;
                case BillingClient.BillingResponseCode.ITEM_ALREADY_OWNED: {
                    this.logWarning("onPurchasesUpdated [The user already owns this item] message: %s"
                        , debugMessage
                    );

                    this.nativeCall("onGooglePlayBillingPurchasesUpdatedItemAlreadyOwned");
                }break;
                case BillingClient.BillingResponseCode.ITEM_NOT_OWNED: {
                    this.logWarning("onPurchasesUpdated [The user item not owned] message: %s"
                        , debugMessage
                    );

                    this.nativeCall("onGooglePlayBillingPurchasesUpdatedItemNotOwned");
                }break;
                case BillingClient.BillingResponseCode.NETWORK_ERROR: {
                    this.logWarning("onPurchasesUpdated [Network error] message: %s"
                        , debugMessage
                    );

                    this.nativeCall("onGooglePlayBillingPurchasesUpdatedNetworkError");
                }break;
                default: {
                    this.logWarning("onPurchasesUpdated [Unsupported response] code: %d message: %s"
                        , responseCode
                        , debugMessage
                    );

                    this.nativeCall("onGooglePlayBillingPurchasesUpdatedUnknown", responseCode);
                }break;
            }
        };

        PendingPurchasesParams pendingPurchasesParams = PendingPurchasesParams.newBuilder()
                .enableOneTimeProducts()
                .build();

        m_billingClient = BillingClient.newBuilder(application)
            .setListener(purchasesUpdatedListener)
            .enablePendingPurchases(pendingPurchasesParams)
            .enableAutoServiceReconnection()
            .build();
    }

    @Override
    public void onAppTerminate(@NonNull MengineApplication application) {
        m_productsDetails = null;
        m_billingClient = null;
    }

    protected void billingConnect(@NonNull MengineActivity activity) {
        m_billingClient.startConnection(new BillingClientStateListener() {
            @Override
            public void onBillingServiceDisconnected() {
                MengineGooglePlayBillingPlugin.this.logInfo("Billing disconnected");

                MengineUtils.performOnMainThreadDelayed(() -> {
                    MengineGooglePlayBillingPlugin.this.billingConnect(activity);
                },5000L);
            }

            @Override
            public void onBillingSetupFinished(@NonNull BillingResult billingResult) {
                int responseCode = billingResult.getResponseCode();

                if (responseCode != BillingClient.BillingResponseCode.OK) {
                    MengineGooglePlayBillingPlugin.this.logInfo("billing invalid connection responseCode: %d message: %s"
                        , responseCode
                        , billingResult.getDebugMessage()
                    );

                    return;
                }

                MengineGooglePlayBillingPlugin.this.logInfo("billing setup finished");

                BillingResult supportedProductDetails = m_billingClient.isFeatureSupported(BillingClient.FeatureType.PRODUCT_DETAILS);

                if (supportedProductDetails.getResponseCode() == BillingClient.BillingResponseCode.FEATURE_NOT_SUPPORTED) {
                    MengineGooglePlayBillingPlugin.this.logError("[ERROR] queryProducts billing client feature PRODUCT_DETAILS is not supported");

                    MengineUI.showToastRes(activity, R.string.mengine_googleplaybilling_asks_update_playstore);
                }

                MengineGooglePlayBillingPlugin.this.activateSemaphore("GooglePlayBillingReady");
            }
        });
    }

    @Override
    public void onCreate(@NonNull MengineActivity activity, Bundle savedInstanceState) throws MengineServiceInvalidInitializeException {
        this.billingConnect(activity);
    }

    @Override
    public void onResume(@NonNull MengineActivity activity) {
        this.logInfo("onResume");

        if (m_billingClient == null) {
            return;
        }

        if (m_billingClient.isReady() == false) {
            return;
        }

        QueryPurchasesParams purchasesParams = QueryPurchasesParams.newBuilder()
            .setProductType(BillingClient.ProductType.INAPP)
            .build();

        m_billingClient.queryPurchasesAsync(purchasesParams, (billingResult, purchases) -> {
            int responseCode = billingResult.getResponseCode();

            if (responseCode != BillingClient.BillingResponseCode.OK) {
                return;
            }

            Set<String> owned = new HashSet<>();

            for (Purchase purchase : purchases) {
                if (purchase.isAcknowledged() == false) {
                    continue;
                }

                if (purchase.getPurchaseState() != Purchase.PurchaseState.PURCHASED) {
                    continue;
                }

                List<String> products = purchase.getProducts();

                owned.addAll(products);
            }

            this.logInfo("owned products: %s"
                , owned
            );

            MengineFragmentInAppPurchase.INSTANCE.ownedInAppProducts(owned);
        });
    }

    @Override
    public void onDestroy(@NonNull MengineActivity activity) {
        if (m_billingClient != null) {
            m_billingClient.endConnection();
        }
    }

    public boolean isOwnedInAppProduct(@NonNull String product) {
        boolean owned = MengineFragmentInAppPurchase.INSTANCE.isOwnedInAppProduct(product);

        return owned;
    }

    public void queryProducts(List<String> products) {
        this.logInfo("queryProducts products: %s"
            , products
        );

        if (m_billingClient == null) {
            this.logError("[ERROR] queryProducts billing client not created");

            this.nativeCall("onGooglePlayBillingQueryProductError", ERROR_CODE_NOT_INITIALIZED, new RuntimeException("Billing client not initialized"));

            return;
        }

        if (m_billingClient.isReady() == false) {
            this.logError("[ERROR] queryProducts billing client not ready");

            this.nativeCall("onGooglePlayBillingQueryProductError", ERROR_CODE_NOT_READY, new RuntimeException("Billing client not ready"));

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

        m_billingClient.queryProductDetailsAsync(params.build(), (billingResult, productDetailsResult) -> {
            int responseCode = billingResult.getResponseCode();

            if (responseCode != BillingClient.BillingResponseCode.OK) {
                this.logError("[ERROR] billing invalid query product details code: %d message: %s"
                    , responseCode
                    , billingResult.getDebugMessage()
                );

                this.nativeCall("onGooglePlayBillingQueryProductFailed");

                return;
            }

            List<ProductDetails> productsDetails = productDetailsResult.getProductDetailsList();

            this.logInfo("billing query products details: %s"
                , productsDetails
            );

            List<UnfetchedProduct> unfetchedProducts = productDetailsResult.getUnfetchedProductList();

            this.logInfo("billing query products unfetched: %s"
                , unfetchedProducts
            );

            m_productsDetails.clear();
            m_productsDetails.addAll(productsDetails);

            ArrayList<MengineParamInAppProduct> products1 = new ArrayList<>();

            for(ProductDetails details : productsDetails) {
                MengineParamInAppProduct product = new MengineParamInAppProduct();

                String productId = details.getProductId();
                String productType = details.getProductType();
                String name = details.getName();
                String title = details.getTitle();
                String description = details.getDescription();

                product.INAPPPRODUCT_ID = productId;
                product.INAPPPRODUCT_TYPE = productType;
                product.INAPPPRODUCT_NAME = name;
                product.INAPPPRODUCT_TITLE = title;
                product.INAPPPRODUCT_DESCRIPTION = description;

                ProductDetails.OneTimePurchaseOfferDetails oneTimePurchaseOfferDetails = details.getOneTimePurchaseOfferDetails();

                if (oneTimePurchaseOfferDetails != null) {
                    long priceAmountMicros = oneTimePurchaseOfferDetails.getPriceAmountMicros();
                    String formattedPrice = oneTimePurchaseOfferDetails.getFormattedPrice();
                    String priceCurrencyCode = oneTimePurchaseOfferDetails.getPriceCurrencyCode();

                    product.INAPPPRODUCT_PRICE_AMOUNT_MICROS = priceAmountMicros;
                    product.INAPPPRODUCT_PRICE_FORMATTED = formattedPrice;
                    product.INAPPPRODUCT_PRICE_CURRENCY_CODE = priceCurrencyCode;
                } else {
                    product.INAPPPRODUCT_PRICE_AMOUNT_MICROS = 0L;
                    product.INAPPPRODUCT_PRICE_FORMATTED = "";
                    product.INAPPPRODUCT_PRICE_CURRENCY_CODE = "";
                }

                products1.add(product);
            }

            MengineFragmentInAppPurchase.INSTANCE.setInAppProducts(products1);

            List<Map<String, Object>> desc_products = new ArrayList<>();

            for (MengineParamInAppProduct product : products1) {
                Map<String, Object> desc_product = new HashMap<>();

                Map<String, Object> desc_offer = new HashMap<>();

                long priceAmountMicros = product.INAPPPRODUCT_PRICE_AMOUNT_MICROS;
                String formattedPrice = product.INAPPPRODUCT_PRICE_FORMATTED;
                String priceCurrencyCode = product.INAPPPRODUCT_PRICE_CURRENCY_CODE;

                desc_offer.put("priceAmountMicros", priceAmountMicros);
                desc_offer.put("formattedPrice", formattedPrice);
                desc_offer.put("priceCurrencyCode", priceCurrencyCode);

                desc_product.put("oneTimePurchaseOfferDetails", desc_offer);

                String description = product.INAPPPRODUCT_DESCRIPTION;
                String name = product.INAPPPRODUCT_NAME;
                String productId = product.INAPPPRODUCT_ID;
                String productType = product.INAPPPRODUCT_TYPE;
                String title = product.INAPPPRODUCT_TITLE;

                desc_product.put("description", description);
                desc_product.put("name", name);
                desc_product.put("productId", productId);
                desc_product.put("productType", productType);
                desc_product.put("title", title);

                desc_products.add(desc_product);
            }

            this.logInfo("billing response skuResponse: %s"
                , desc_products
            );

            this.nativeCall("onGooglePlayBillingQueryProductSuccess", desc_products);
        });
    }

    public void restorePurchases() {
        this.logInfo("restorePurchases");

        if (m_billingClient == null) {
            this.logError("[ERROR] restorePurchases billing client not created");

            this.nativeCall("onGooglePlayBillingRestorePurchasesError", ERROR_CODE_NOT_INITIALIZED, new RuntimeException("Billing client not initialized"));

            return;
        }

        if (m_billingClient.isReady() == false) {
            this.logError("[ERROR] restorePurchases billing client not ready");

            this.nativeCall("onGooglePlayBillingRestorePurchasesError", ERROR_CODE_NOT_READY, new RuntimeException("Billing client not ready"));

            return;
        }

        this.buildEvent("mng_billing_restore_purchases")
            .log();

        QueryPurchasesParams purchasesParams = QueryPurchasesParams.newBuilder()
            .setProductType(BillingClient.ProductType.INAPP)
            .build();

        m_billingClient.queryPurchasesAsync(purchasesParams, (billingResult, purchases) -> {
            int responseCode = billingResult.getResponseCode();

            if (responseCode != BillingClient.BillingResponseCode.OK) {
                this.logError("[ERROR] billing invalid restore purchases responseCode: %d message: %s"
                    , responseCode
                    , billingResult.getDebugMessage()
                );

                this.buildEvent("mng_billing_restore_purchases_failed")
                    .addParameterLong("error_code", responseCode)
                    .log();

                this.nativeCall("onGooglePlayBillingRestorePurchasesFailed");

                MengineUI.showToastRes(MengineActivity.INSTANCE, R.string.mengine_googleplaybilling_restore_purchases_failed);

                return;
            }

            this.logInfo("billing success restore purchases: %s"
                , purchases
            );

            this.buildEvent("mng_billing_restore_purchases_success")
                .log();

            this.handlePurchases(purchases);

            Set<String> owned = new HashSet<>();

            for (Purchase purchase : purchases) {
                if (purchase.isAcknowledged() == false) {
                    continue;
                }

                if (purchase.getPurchaseState() != Purchase.PurchaseState.PURCHASED) {
                    continue;
                }

                List<String> products = purchase.getProducts();

                owned.addAll(products);
            }

            MengineFragmentInAppPurchase.INSTANCE.ownedInAppProducts(owned);

            this.nativeCall("onGooglePlayBillingRestorePurchasesSuccess", owned);

            MengineUI.showToastRes(MengineActivity.INSTANCE, R.string.mengine_googleplaybilling_restore_purchases_success);
        });
    }

    protected ProductDetails getProductDetails(String productId) {
        for (ProductDetails product : m_productsDetails) {
            String currentProductId = product.getProductId();

            if (Objects.equals(currentProductId, productId) == false) {
                continue;
            }

            return product;
        }

        return null;
    }

    public void buyInApp(String productId) {
        this.logInfo("buyInApp productId: %s"
            , productId
        );

        if (m_billingClient == null) {
            this.logError("[ERROR] buyInApp billing client not created");

            this.nativeCall("onGooglePlayBillingBuyInAppError", productId, ERROR_CODE_NOT_INITIALIZED, new RuntimeException("Billing client not initialized"));

            return;
        }

        if (m_billingClient.isReady() == false) {
            this.logError("[ERROR] buyInApp billing client not ready");

            this.nativeCall("onGooglePlayBillingBuyInAppLaunchFlowError", productId, ERROR_CODE_NOT_READY, new RuntimeException("Billing client not ready"));

            return;
        }

        MengineActivity activity = this.getMengineActivity();

        if (activity == null) {
            this.logError("[ERROR] buyInApp invalid activity");

            this.nativeCall("onGooglePlayBillingBuyInAppLaunchFlowError", productId, ERROR_CODE_NOT_READY, new RuntimeException("Activity not ready"));

            return;
        }

        ProductDetails product = this.getProductDetails(productId);

        if (product == null) {
            this.logError("[ERROR] buyInApp invalid product id: %s"
                , productId
            );

            this.nativeCall("onGooglePlayBillingBuyInAppLaunchFlowError", productId, ERROR_CODE_NOT_FOUND, new RuntimeException("Product not found"));

            return;
        }

        this.buildEvent("mng_billing_buy_launch_flow")
            .addParameterString("product_id", productId)
            .log();

        List<BillingFlowParams.ProductDetailsParams> productDetailsParamsList = new ArrayList<>();

        BillingFlowParams.ProductDetailsParams productParams = BillingFlowParams.ProductDetailsParams.newBuilder()
            .setProductDetails(product)
            .build();

        productDetailsParamsList.add(productParams);

        BillingFlowParams flowParams = BillingFlowParams.newBuilder()
            .setProductDetailsParamsList(productDetailsParamsList)
            .build();

        BillingResult billingResult = m_billingClient.launchBillingFlow(activity, flowParams);

        int responseCode = billingResult.getResponseCode();

        if (responseCode != BillingClient.BillingResponseCode.OK) {
            int subResponseCode = billingResult.getOnPurchasesUpdatedSubResponseCode();

            this.logError("[ERROR] billing invalid launch billing flow code: %d sub: %d message: %s"
                , responseCode
                , subResponseCode
                , billingResult.getDebugMessage()
            );

            this.buildEvent("mng_billing_buy_launch_flow_failed")
                .addParameterString("product_id", productId)
                .addParameterLong("error_code", responseCode)
                .addParameterLong("sub_code", subResponseCode)
                .log();

            this.nativeCall("onGooglePlayBillingBuyInAppLaunchFlowFailed", productId, responseCode, subResponseCode);

            return;
        }

        this.logInfo("buy InApp success productId: %s"
            , productId
        );

        this.buildEvent("mng_billing_buy_launch_flow_success")
            .addParameterString("product_id", productId)
            .log();

        this.nativeCall("onGooglePlayBillingBuyInAppLaunchFlowSuccess", productId);
    }

    protected void purchaseInAppProduct(@NonNull Purchase purchase, boolean isConsumable) {
        String orderId = purchase.getOrderId();
        List<String> products = purchase.getProducts();
        int quantity = purchase.getQuantity();
        boolean isAcknowledged = purchase.isAcknowledged();
        String token = purchase.getPurchaseToken();
        String originalJson = purchase.getOriginalJson();

        MengineParamInAppPurchase purchase1 = new MengineParamInAppPurchase();
        purchase1.INAPPPURCHASE_TRANSACTION = orderId;
        purchase1.INAPPPURCHASE_PRODUCTS = products;
        purchase1.INAPPPURCHASE_QUANTITY = quantity;
        purchase1.INAPPPURCHASE_ACKNOWLEDGED = isAcknowledged;
        purchase1.INAPPPURCHASE_CONSUMABLE = isConsumable;
        purchase1.INAPPPURCHASE_TOKEN = token;
        purchase1.INAPPPURCHASE_DATA = originalJson;

        MengineFragmentInAppPurchase.INSTANCE.purchaseInAppProduct(purchase1);
    }

    @AnyThread
    private void handleNonConsumablePurchase(@NonNull Purchase purchase) {
        this.logInfo("handleNonConsumablePurchase purchase: %s"
            , purchase
        );

        List<String> products = purchase.getProducts();
        String token = purchase.getPurchaseToken();

        AcknowledgePurchaseParams.Builder acknowledgePurchaseParams = AcknowledgePurchaseParams.newBuilder()
            .setPurchaseToken(token);

        m_billingClient.acknowledgePurchase(acknowledgePurchaseParams.build(), billingResult -> {
            int responseCode = billingResult.getResponseCode();

            if (responseCode != BillingClient.BillingResponseCode.OK) {
                this.logError("[ERROR] billing invalid acknowledge purchase code: %d message: %s"
                    , responseCode
                    , billingResult.getDebugMessage()
                );

                this.nativeCall("onGooglePlayBillingPurchaseAcknowledgeFailed", products);

                return;
            }

            this.logInfo("billing success acknowledge purchase: %s products: %s"
                , billingResult.getDebugMessage()
                , products
            );

            MenginePreferences.addPreferenceStrings("mengine.billing.purchase.owned.products", products);

            this.purchaseInAppProduct(purchase, false);

            this.nativeCall("onGooglePlayBillingPurchaseAcknowledgeSuccess", products);
        });
    }

    @AnyThread
    private void handleConsumablePurchase(@NonNull Purchase purchase) {
        this.logInfo("handleConsumablePurchase purchase: %s"
            , purchase
        );

        List<String> products = purchase.getProducts();
        String token = purchase.getPurchaseToken();

        ConsumeParams consumeParams = ConsumeParams.newBuilder()
            .setPurchaseToken(token)
            .build();

        m_billingClient.consumeAsync(consumeParams, (billingResult, purchaseToken) -> {
            int responseCode = billingResult.getResponseCode();

            if (responseCode != BillingClient.BillingResponseCode.OK) {
                this.logError("[ERROR] billing invalid consume code: %d message: %s"
                    , responseCode
                    , billingResult.getDebugMessage()
                );

                this.nativeCall("onGooglePlayBillingPurchasesOnConsumeFailed", products);

                return;
            }

            this.logInfo("billing success consume: %s products: %s"
                , billingResult.getDebugMessage()
                , products
            );

            this.purchaseInAppProduct(purchase, true);

            this.nativeCall("onGooglePlayBillingPurchasesOnConsumeSuccess", products);
        });
    }

    @AnyThread
    private void handlePurchases(List<Purchase> purchases) {
        for (Purchase purchase : purchases) {
            this.handlePurchase(purchase);
        }
    }

    @AnyThread
    private void handlePurchase(@NonNull Purchase purchase) {
        this.logInfo("handlePurchase purchase: %s"
            , purchase
        );

        List<String> products = purchase.getProducts();

        int state = purchase.getPurchaseState();

        switch (state) {
            case Purchase.PurchaseState.UNSPECIFIED_STATE: {
                this.logError("[ERROR] handlePurchase invalid purchase state: UNSPECIFIED_STATE purchase: %s"
                    , purchase
                );

                this.nativeCall("onGooglePlayBillingPurchaseUnspecifiedState", products);
            } break;
            case Purchase.PurchaseState.PURCHASED: {
                MengineCallback cb = (boolean successful, Map<String, Object> result) -> {
                    if (successful == false) {
                        this.logError("[ERROR] handlePurchase invalid isConsumable");

                        return;
                    }

                    boolean isConsumable = (boolean)result.get("isConsumable");

                    boolean acknowledged = purchase.isAcknowledged();

                    this.logInfo("handlePurchase Acknowledged: %s product: %s consumable: %s"
                        , acknowledged
                        , products
                        , isConsumable
                    );

                    if (acknowledged == true) {
                        this.nativeCall("onGooglePlayBillingPurchaseAcknowledged", products);

                        return;
                    }

                    if (isConsumable == true) {
                        this.handleConsumablePurchase(purchase);
                    } else {
                        this.handleNonConsumablePurchase(purchase);
                    }
                };

                this.nativeCall("onGooglePlayBillingPurchaseIsConsumable", products, cb);
            } break;
            case Purchase.PurchaseState.PENDING: {
                this.logInfo("handlePurchase [Pending] purchase: %s"
                    , purchase
                );

                this.nativeCall("onGooglePlayBillingPurchasePending", products);
            } break;
        }
    }
}
