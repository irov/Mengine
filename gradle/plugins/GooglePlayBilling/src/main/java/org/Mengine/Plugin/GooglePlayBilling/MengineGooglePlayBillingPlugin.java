package org.Mengine.Plugin.GooglePlayBilling;

import android.os.Bundle;
import android.os.Handler;

import androidx.annotation.AnyThread;
import androidx.annotation.NonNull;
import androidx.annotation.Nullable;

import com.android.billingclient.api.AccountIdentifiers;
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
import org.Mengine.Base.MengineParamInAppProduct;
import org.Mengine.Base.MengineParamInAppPurchase;
import org.Mengine.Base.MengineListenerActivity;
import org.Mengine.Base.MengineListenerApplication;
import org.Mengine.Base.MenginePreferences;
import org.Mengine.Base.MengineService;
import org.Mengine.Base.MengineServiceInvalidInitializeException;
import org.Mengine.Base.MengineUI;
import org.Mengine.Base.MengineUtils;

import java.util.ArrayList;
import java.util.Collections;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.concurrent.atomic.AtomicBoolean;
import java.util.concurrent.atomic.AtomicReference;
import java.util.function.Consumer;

public class MengineGooglePlayBillingPlugin extends MengineService implements MengineListenerApplication, MengineListenerActivity {
    public static final String SERVICE_NAME = "GPlayBilling";
    public static final boolean SERVICE_EMBEDDING = true;

    private static final int ERROR_CODE_UNKNOWN = 0;
    private static final int ERROR_CODE_NOT_FOUND = 1;
    private static final int ERROR_CODE_NOT_SUPPORTED = 2;
    private static final int ERROR_CODE_NOT_INITIALIZED = 3;
    private static final int ERROR_CODE_NOT_READY = 4;
    private static final int RESTORE_QUERY_MAX_RETRIES = 2;
    private static final long RESTORE_QUERY_RETRY_DELAY_MS = 1000;
    private static final String PREFERENCE_PURCHASE_DELIVERED_TOKENS = "mengine.billing.purchase.delivered.tokens";

    private volatile BillingClient m_billingClient;
    private boolean m_billingConnecting;
    private volatile boolean m_productsQueried;
    private final AtomicBoolean m_queryProductsInProgress = new AtomicBoolean(false);
    private final AtomicReference<String> m_pendingPurchaseProductId = new AtomicReference<>(null);
    private final AtomicBoolean m_restorePurchasesInProgress = new AtomicBoolean(false);
    private final Map<String, List<Consumer<Boolean>>> m_purchaseProcessingCompletions = new HashMap<>();

    @Override
    public void onAppCreate(@NonNull MengineApplication application) throws MengineServiceInvalidInitializeException {
        m_billingConnecting = false;
        m_productsQueried = false;

        PurchasesUpdatedListener purchasesUpdatedListener = (billingResult, purchases) -> {
            if (m_billingClient == null) {
                return;
            }

            int responseCode = billingResult.getResponseCode();
            String debugMessage = billingResult.getDebugMessage();
            String productId = m_pendingPurchaseProductId.getAndSet(null);

            switch (responseCode) {
                case BillingClient.BillingResponseCode.FEATURE_NOT_SUPPORTED: {
                    this.logWarning("onPurchasesUpdated [Feature not supported] message: %s"
                        , debugMessage
                    );

                    this.nativeCall("onGooglePlayBillingPurchasesUpdatedFeatureNotSupported", productId);
                }break;
                case BillingClient.BillingResponseCode.SERVICE_DISCONNECTED: {
                    this.logWarning("onPurchasesUpdated [Service disconnected] message: %s"
                        , debugMessage
                    );

                    this.nativeCall("onGooglePlayBillingPurchasesUpdatedServiceDisconnected", productId);
                }break;
                case BillingClient.BillingResponseCode.OK: {
                    this.logInfo("onPurchasesUpdated [User success the purchase]");

                    if (purchases != null) {
                        this.handlePurchases(purchases);
                    }

                    this.nativeCall("onGooglePlayBillingPurchasesUpdatedOk", productId);
                }break;
                case BillingClient.BillingResponseCode.USER_CANCELED: {
                    this.logInfo("onPurchasesUpdated [User canceled the purchase]");

                    this.nativeCall("onGooglePlayBillingPurchasesUpdatedUserCanceled", productId);
                }break;
                case BillingClient.BillingResponseCode.SERVICE_UNAVAILABLE: {
                    this.logWarning("onPurchasesUpdated [Service Unavailable] message: %s"
                        , debugMessage
                    );

                    this.nativeCall("onGooglePlayBillingPurchasesUpdatedServiceUnavailable", productId);
                }break;
                case BillingClient.BillingResponseCode.BILLING_UNAVAILABLE: {
                    this.logWarning("onPurchasesUpdated [Billing Unavailable] message: %s"
                        , debugMessage
                    );

                    this.nativeCall("onGooglePlayBillingPurchasesUpdatedBillingUnavailable", productId);
                }break;
                case BillingClient.BillingResponseCode.ITEM_UNAVAILABLE: {
                    this.logWarning("onPurchasesUpdated [The user item unavailable] message: %s"
                        , debugMessage
                    );

                    this.nativeCall("onGooglePlayBillingPurchasesUpdatedItemUnavailable", productId);
                }break;
                case BillingClient.BillingResponseCode.DEVELOPER_ERROR: {
                    this.logError(
                        "onPurchasesUpdated: Developer error means that Google Play " +
                            "does not recognize the configuration. If you are just getting started, " +
                            "make sure you have configured the application correctly in the " +
                            "Google Play Console. The SKU product ID must match and the APK you " +
                            "are using must be signed with release keys."
                    );

                    this.nativeCall("onGooglePlayBillingPurchasesUpdatedDeveloperError", productId);
                }break;
                case BillingClient.BillingResponseCode.ERROR: {
                    this.logWarning("onPurchasesUpdated [Error] message: %s"
                        , debugMessage
                    );

                    this.nativeCall("onGooglePlayBillingPurchasesUpdatedError", productId);
                }break;
                case BillingClient.BillingResponseCode.ITEM_ALREADY_OWNED: {
                    this.logWarning("onPurchasesUpdated [The user already owns this item] message: %s"
                        , debugMessage
                    );

                    this.nativeCall("onGooglePlayBillingPurchasesUpdatedItemAlreadyOwned", productId);
                }break;
                case BillingClient.BillingResponseCode.ITEM_NOT_OWNED: {
                    this.logWarning("onPurchasesUpdated [The user item not owned] message: %s"
                        , debugMessage
                    );

                    this.nativeCall("onGooglePlayBillingPurchasesUpdatedItemNotOwned", productId);
                }break;
                case BillingClient.BillingResponseCode.NETWORK_ERROR: {
                    this.logWarning("onPurchasesUpdated [Network error] message: %s"
                        , debugMessage
                    );

                    this.nativeCall("onGooglePlayBillingPurchasesUpdatedNetworkError", productId);
                }break;
                default: {
                    this.logWarning("onPurchasesUpdated [Unsupported response] code: %d message: %s"
                        , responseCode
                        , debugMessage
                    );

                    this.nativeCall("onGooglePlayBillingPurchasesUpdatedUnknown", productId, responseCode);
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
        BillingClient billingClient = m_billingClient;
        m_billingClient = null;

        if (billingClient != null) {
            billingClient.endConnection();
        }

        m_billingConnecting = false;
        m_productsQueried = false;
        m_queryProductsInProgress.set(false);
        m_pendingPurchaseProductId.set(null);
        m_restorePurchasesInProgress.set(false);

        synchronized (m_purchaseProcessingCompletions) {
            m_purchaseProcessingCompletions.clear();
        }
    }

    protected void billingConnect() {
        BillingClient billingClient = m_billingClient;

        if (billingClient == null) {
            return;
        }

        if (billingClient.isReady() == true) {
            return;
        }

        if (m_billingConnecting == true) {
            return;
        }

        m_billingConnecting = true;

        billingClient.startConnection(new BillingClientStateListener() {
            @Override
            public void onBillingServiceDisconnected() {
                if (m_billingClient != billingClient) {
                    return;
                }

                m_billingConnecting = false;

                MengineGooglePlayBillingPlugin.this.logInfo("Billing disconnected");
            }

            @Override
            public void onBillingSetupFinished(@NonNull BillingResult billingResult) {
                if (m_billingClient != billingClient) {
                    return;
                }

                m_billingConnecting = false;

                int responseCode = billingResult.getResponseCode();

                if (responseCode != BillingClient.BillingResponseCode.OK) {
                    MengineGooglePlayBillingPlugin.this.logInfo("billing invalid connection responseCode: %d message: %s"
                        , responseCode
                        , billingResult.getDebugMessage()
                    );

                    return;
                }

                MengineGooglePlayBillingPlugin.this.logInfo("billing setup finished");

                BillingResult supportedProductDetails = billingClient.isFeatureSupported(BillingClient.FeatureType.PRODUCT_DETAILS);

                if (supportedProductDetails.getResponseCode() == BillingClient.BillingResponseCode.FEATURE_NOT_SUPPORTED) {
                    MengineGooglePlayBillingPlugin.this.logError("[ERROR] queryProducts billing client feature PRODUCT_DETAILS is not supported");
                    MengineUI.showToastRes(MengineGooglePlayBillingPlugin.this.getMengineActivity(), R.string.mengine_googleplaybilling_asks_update_playstore);
                }

                MengineGooglePlayBillingPlugin.this.activateSemaphore("GooglePlayBillingReady");

                if (m_productsQueried == true) {
                    MengineGooglePlayBillingPlugin.this.queryOwnedPurchases();
                }
            }
        });
    }

    @Override
    public void onCreate(@NonNull MengineActivity activity, Bundle savedInstanceState) throws MengineServiceInvalidInitializeException {
        this.billingConnect();
    }

    @Override
    public void onResume(@NonNull MengineActivity activity) {
        BillingClient billingClient = m_billingClient;

        if (billingClient == null) {
            return;
        }

        if (billingClient.isReady() == false) {
            this.billingConnect();

            return;
        }

        if (m_productsQueried == false) {
            return;
        }

        this.queryOwnedPurchases();
    }

    private void queryOwnedPurchases() {
        BillingClient billingClient = m_billingClient;

        if (billingClient == null) {
            return;
        }

        if (billingClient.isReady() == false) {
            return;
        }

        QueryPurchasesParams purchasesParams = QueryPurchasesParams.newBuilder()
            .setProductType(BillingClient.ProductType.INAPP)
            .build();

        billingClient.queryPurchasesAsync(purchasesParams, (billingResult, purchases) -> {
            if (m_billingClient != billingClient) {
                return;
            }

            int responseCode = billingResult.getResponseCode();

            if (responseCode != BillingClient.BillingResponseCode.OK) {
                this.logWarning("query owned purchases failed code: %d message: %s"
                    , responseCode
                    , billingResult.getDebugMessage()
                );

                return;
            }

            this.handlePurchases(purchases);

            Set<String> owned = new HashSet<>();

            for (Purchase purchase : purchases) {
                if (purchase.isAcknowledged() == false) {
                    continue;
                }

                int state = purchase.getPurchaseState();

                if (state != Purchase.PurchaseState.PURCHASED) {
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

    public boolean isOwnedInAppProduct(@NonNull String product) {
        boolean owned = MengineFragmentInAppPurchase.INSTANCE.isOwnedInAppProduct(product);

        return owned;
    }

    public void queryProducts(@NonNull List<String> consumableProducts, @NonNull List<String> nonConsumableProducts) {
        if (consumableProducts.isEmpty() == true) {
            if (nonConsumableProducts.isEmpty() == true) {
                this.logError("[Error] queryProducts empty products list");

                this.nativeCall("onGooglePlayBillingQueryProductError", ERROR_CODE_NOT_FOUND, new IllegalArgumentException("Products list is empty"));

                return;
            }
        }

        this.logInfo("queryProducts consumableProducts: %s nonConsumableProducts: %s"
            , consumableProducts
            , nonConsumableProducts
        );

        List<String> products = new ArrayList<>();

        products.addAll(consumableProducts);
        products.addAll(nonConsumableProducts);

        BillingClient billingClient = m_billingClient;

        if (billingClient == null) {
            this.logError("[ERROR] queryProducts billing client not created");

            this.nativeCall("onGooglePlayBillingQueryProductError", ERROR_CODE_NOT_INITIALIZED, new RuntimeException("Billing client not initialized"));

            return;
        }

        if (billingClient.isReady() == false) {
            this.logInfo("queryProducts billing client not ready");
            MengineUtils.performOnMainThread(this::billingConnect);

            this.nativeCall("onGooglePlayBillingQueryProductFailed");

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

        if (m_queryProductsInProgress.compareAndSet(false, true) == false) {
            return;
        }

        billingClient.queryProductDetailsAsync(params.build(), (billingResult, productDetailsResult) -> {
            if (m_billingClient != billingClient) {
                return;
            }

            m_queryProductsInProgress.set(false);

            int responseCode = billingResult.getResponseCode();

            if (responseCode != BillingClient.BillingResponseCode.OK) {
                String debugMessage = billingResult.getDebugMessage();

                if (MengineGooglePlayBillingPlugin.this.isBillingResponseRetryable(responseCode) == true) {
                    this.logWarning("billing query product details temporary failure code: %d message: %s"
                        , responseCode
                        , debugMessage
                    );

                    this.nativeCall("onGooglePlayBillingQueryProductFailed");

                    return;
                }

                this.logError("[ERROR] billing invalid query product details code: %d message: %s"
                    , responseCode
                    , debugMessage
                );

                this.nativeCall("onGooglePlayBillingQueryProductError", responseCode, new RuntimeException(debugMessage));

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

            m_productsQueried = true;
            this.nativeCall("onGooglePlayBillingQueryProductSuccess", desc_products);
            this.queryOwnedPurchases();
        });
    }

    private boolean isBillingResponseRetryable(int responseCode) {
        switch (responseCode) {
            case BillingClient.BillingResponseCode.SERVICE_DISCONNECTED:
            case BillingClient.BillingResponseCode.SERVICE_UNAVAILABLE:
            case BillingClient.BillingResponseCode.ERROR:
            case BillingClient.BillingResponseCode.NETWORK_ERROR:
                return true;
            default:
                return false;
        }
    }

    public void restorePurchases() {
        this.logInfo("restorePurchases");

        BillingClient billingClient = m_billingClient;

        if (billingClient == null) {
            this.logError("[ERROR] restorePurchases billing client not created");

            this.nativeCall("onGooglePlayBillingRestorePurchasesError", ERROR_CODE_NOT_INITIALIZED, new RuntimeException("Billing client not initialized"));

            return;
        }

        if (m_restorePurchasesInProgress.compareAndSet(false, true) == false) {
            this.logWarning("restorePurchases already in progress");

            return;
        }

        this.buildEvent("mng_billing_restore_purchases")
            .log();

        this.queryRestorePurchases(billingClient, 0, purchases -> {
            Consumer<Boolean> completion = MengineUtils.createCountDownConsumer(purchases, successful -> {
                if (m_billingClient != billingClient) {
                    return;
                }

                if (successful == false) {
                    this.failRestorePurchases();

                    return;
                }

                this.queryRestorePurchases(billingClient, 0, this::finishRestorePurchases);
            });

            for (Purchase purchase : purchases) {
                this.handlePurchase(purchase, completion);
            }
        });
    }

    private void queryRestorePurchases(@NonNull BillingClient billingClient, int attempt, @NonNull Consumer<List<Purchase>> onSuccess) {
        if (m_billingClient != billingClient) {
            return;
        }

        QueryPurchasesParams purchasesParams = QueryPurchasesParams.newBuilder()
            .setProductType(BillingClient.ProductType.INAPP)
            .build();

        billingClient.queryPurchasesAsync(purchasesParams, (billingResult, purchases) -> {
            if (m_billingClient != billingClient) {
                return;
            }

            int responseCode = billingResult.getResponseCode();

            if (responseCode == BillingClient.BillingResponseCode.OK) {
                onSuccess.accept(purchases);

                return;
            }

            if (this.scheduleRestorePurchasesRetry(billingClient, attempt, responseCode, onSuccess) == true) {
                return;
            }

            this.logError("[ERROR] billing invalid restore purchases responseCode: %d message: %s"
                , responseCode
                , billingResult.getDebugMessage()
            );

            this.buildEvent("mng_billing_restore_purchases_failed")
                .addParameterLong("error_code", responseCode)
                .log();

            this.failRestorePurchases();
        });
    }

    private boolean scheduleRestorePurchasesRetry(@NonNull BillingClient billingClient, int attempt, int responseCode, @NonNull Consumer<List<Purchase>> onSuccess) {
        if (attempt >= RESTORE_QUERY_MAX_RETRIES) {
            return false;
        }

        if (this.isBillingResponseRetryable(responseCode) == false) {
            return false;
        }

        int nextAttempt = attempt + 1;
        long delay = RESTORE_QUERY_RETRY_DELAY_MS * nextAttempt;

        this.logWarning("billing restore purchases retry: %d responseCode: %d"
            , nextAttempt
            , responseCode
        );

        Handler handler = MengineUtils.performOnMainThreadDelayed(() -> this.queryRestorePurchases(billingClient, nextAttempt, onSuccess), delay);

        if (handler == null) {
            return false;
        }

        return true;
    }

    private void finishRestorePurchases(@NonNull List<Purchase> purchases) {
        Set<String> owned = new HashSet<>();

        for (Purchase purchase : purchases) {
            if (purchase.isAcknowledged() == false) {
                continue;
            }

            if (purchase.getPurchaseState() != Purchase.PurchaseState.PURCHASED) {
                continue;
            }

            owned.addAll(purchase.getProducts());
        }

        MengineFragmentInAppPurchase.INSTANCE.ownedInAppProducts(owned);
        m_restorePurchasesInProgress.set(false);

        this.buildEvent("mng_billing_restore_purchases_success")
            .log();

        this.nativeCall("onGooglePlayBillingRestorePurchasesSuccess", owned);
        MengineUI.showToastRes(this.getMengineActivity(), R.string.mengine_googleplaybilling_restore_purchases_success);
    }

    private void failRestorePurchases() {
        m_restorePurchasesInProgress.set(false);

        this.nativeCall("onGooglePlayBillingRestorePurchasesFailed");
        MengineUI.showToastRes(this.getMengineActivity(), R.string.mengine_googleplaybilling_restore_purchases_failed);
    }

    public void buyInApp(String productId) {
        this.logInfo("buyInApp productId: %s"
            , productId
        );

        BillingClient billingClient = m_billingClient;

        if (billingClient == null) {
            this.logError("[ERROR] buyInApp billing client not created");

            this.nativeCall("onGooglePlayBillingBuyInAppLaunchFlowError", productId, ERROR_CODE_NOT_INITIALIZED, new RuntimeException("Billing client not initialized"));

            return;
        }

        if (billingClient.isReady() == false) {
            this.logError("[ERROR] buyInApp billing client not ready");
            this.nativeCall("onGooglePlayBillingBuyInAppLaunchFlowError", productId, ERROR_CODE_NOT_READY, new RuntimeException("Billing client not ready"));

            return;
        }

        if (m_pendingPurchaseProductId.compareAndSet(null, productId) == false) {
            String pendingProductId = m_pendingPurchaseProductId.get();

            this.logError("[ERROR] buyInApp another purchase is in progress productId: %s", pendingProductId);
            this.nativeCall("onGooglePlayBillingBuyInAppLaunchFlowError", productId, ERROR_CODE_NOT_READY, new RuntimeException("Another purchase is in progress"));

            return;
        }

        QueryProductDetailsParams.Product queryProduct = QueryProductDetailsParams.Product.newBuilder()
            .setProductId(productId)
            .setProductType(BillingClient.ProductType.INAPP)
            .build();

        QueryProductDetailsParams queryProductDetailsParams = QueryProductDetailsParams.newBuilder()
            .setProductList(Collections.singletonList(queryProduct))
            .build();

        billingClient.queryProductDetailsAsync(queryProductDetailsParams, (billingResult, productDetailsResult) -> {
            if (m_billingClient != billingClient) {
                return;
            }

            int responseCode = billingResult.getResponseCode();

            if (responseCode != BillingClient.BillingResponseCode.OK) {
                String debugMessage = billingResult.getDebugMessage();

                this.logError("[ERROR] buyInApp query product failed code: %d message: %s"
                    , responseCode
                    , debugMessage
                );

                m_pendingPurchaseProductId.compareAndSet(productId, null);
                this.nativeCall("onGooglePlayBillingBuyInAppLaunchFlowError", productId, ERROR_CODE_UNKNOWN, new RuntimeException(debugMessage));

                return;
            }

            List<ProductDetails> productDetailsList = productDetailsResult.getProductDetailsList();

            if (productDetailsList.isEmpty() == true) {
                this.logError("[ERROR] buyInApp invalid product id: %s"
                    , productId
                );

                m_pendingPurchaseProductId.compareAndSet(productId, null);
                this.nativeCall("onGooglePlayBillingBuyInAppLaunchFlowError", productId, ERROR_CODE_NOT_FOUND, new RuntimeException("Product not found"));

                return;
            }

            ProductDetails productDetails = productDetailsList.get(0);

            MengineUtils.performOnMainThread(() -> {
                if (m_billingClient != billingClient) {
                    return;
                }

                this.launchBillingFlow(productId, productDetails);
            });
        });
    }

    protected void launchBillingFlow(@NonNull String productId, @NonNull ProductDetails product) {
        BillingClient billingClient = m_billingClient;

        if (billingClient == null) {
            return;
        }

        MengineActivity activity = this.getMengineActivity();

        if (MengineUtils.isActivityAvailable(activity) == false) {
            this.logError("[ERROR] buyInApp invalid activity");

            m_pendingPurchaseProductId.compareAndSet(productId, null);
            this.nativeCall("onGooglePlayBillingBuyInAppLaunchFlowError", productId, ERROR_CODE_NOT_READY, new RuntimeException("Activity not ready"));

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

        BillingResult billingResult = billingClient.launchBillingFlow(activity, flowParams);

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

            m_pendingPurchaseProductId.compareAndSet(productId, null);
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

    private boolean isPurchaseDelivered(@NonNull String token) {
        Set<String> deliveredTokens = MenginePreferences.getPreferenceStrings(PREFERENCE_PURCHASE_DELIVERED_TOKENS, null);

        if (deliveredTokens == null) {
            return false;
        }

        boolean delivered = deliveredTokens.contains(token);

        return delivered;
    }

    private void markPurchaseDelivered(@NonNull String token) {
        MenginePreferences.addPreferenceStrings(PREFERENCE_PURCHASE_DELIVERED_TOKENS, Collections.singleton(token));
    }

    private void clearPurchaseDelivered(@NonNull String token) {
        MenginePreferences.removePreferenceStrings(PREFERENCE_PURCHASE_DELIVERED_TOKENS, Collections.singleton(token));
    }

    private boolean beginPurchaseProcessing(@NonNull String token, @NonNull Consumer<Boolean> completion) {
        synchronized (m_purchaseProcessingCompletions) {
            List<Consumer<Boolean>> completions = m_purchaseProcessingCompletions.get(token);

            if (completions != null) {
                completions.add(completion);

                return false;
            }

            List<Consumer<Boolean>> newCompletions = new ArrayList<>();
            newCompletions.add(completion);
            m_purchaseProcessingCompletions.put(token, newCompletions);

            return true;
        }
    }

    private void completePurchaseProcessing(@NonNull String token, boolean successful) {
        List<Consumer<Boolean>> completions;

        synchronized (m_purchaseProcessingCompletions) {
            completions = m_purchaseProcessingCompletions.remove(token);
        }

        if (completions == null) {
            return;
        }

        for (Consumer<Boolean> completion : completions) {
            completion.accept(successful);
        }
    }

    private void deliverPurchase(@NonNull Purchase purchase, boolean isConsumable, @NonNull Consumer<Boolean> completion) {
        List<String> products = purchase.getProducts();
        String token = purchase.getPurchaseToken();

        if (this.isPurchaseDelivered(token) == true) {
            if (isConsumable == true) {
                this.handleConsumablePurchase(purchase, completion);
            } else {
                this.handleNonConsumablePurchase(purchase, completion);
            }

            return;
        }

        MengineCallback cb = MengineCallback.onMainThread((boolean successful, Map<String, Object> result) -> {
            if (successful == false) {
                this.logError("[ERROR] purchase delivery failed products: %s", products);
                this.nativeCall("onGooglePlayBillingPurchaseDeliveryFailed", products);
                completion.accept(false);

                return;
            }

            this.markPurchaseDelivered(token);

            if (isConsumable == true) {
                this.handleConsumablePurchase(purchase, completion);
            } else {
                this.handleNonConsumablePurchase(purchase, completion);
            }
        });

        String accountToken = MengineGooglePlayBillingPlugin.getPurchaseAccountToken(purchase);

        this.nativeCall("onGooglePlayBillingPurchaseDeliver", products, token, accountToken, cb);
    }

    @Nullable
    private static String getPurchaseAccountToken(@NonNull Purchase purchase) {
        AccountIdentifiers accountIdentifiers = purchase.getAccountIdentifiers();

        if (accountIdentifiers == null) {
            return null;
        }

        String obfuscatedAccountId = accountIdentifiers.getObfuscatedAccountId();

        if ("".equals(obfuscatedAccountId) == true) {
            return null;
        }

        return obfuscatedAccountId;
    }

    @AnyThread
    private void handleNonConsumablePurchase(@NonNull Purchase purchase, @NonNull Consumer<Boolean> completion) {
        BillingClient billingClient = m_billingClient;

        if (billingClient == null) {
            completion.accept(false);

            return;
        }

        this.logInfo("handleNonConsumablePurchase purchase: %s"
            , purchase
        );

        List<String> products = purchase.getProducts();
        String token = purchase.getPurchaseToken();

        AcknowledgePurchaseParams.Builder acknowledgePurchaseParams = AcknowledgePurchaseParams.newBuilder()
            .setPurchaseToken(token);

        billingClient.acknowledgePurchase(acknowledgePurchaseParams.build(), billingResult -> {
            if (m_billingClient != billingClient) {
                return;
            }

            int responseCode = billingResult.getResponseCode();

            if (responseCode != BillingClient.BillingResponseCode.OK) {
                this.logError("[ERROR] billing invalid acknowledge purchase code: %d message: %s"
                    , responseCode
                    , billingResult.getDebugMessage()
                );

                this.nativeCall("onGooglePlayBillingPurchaseAcknowledgeFailed", products);
                completion.accept(false);

                return;
            }

            this.logInfo("billing success acknowledge purchase: %s products: %s"
                , billingResult.getDebugMessage()
                , products
            );

            MenginePreferences.addPreferenceStrings("mengine.billing.purchase.owned.products", products);

            this.purchaseInAppProduct(purchase, false);
            this.clearPurchaseDelivered(token);

            this.nativeCall("onGooglePlayBillingPurchaseAcknowledgeSuccess", products, token);
            completion.accept(true);
        });
    }

    @AnyThread
    private void handleConsumablePurchase(@NonNull Purchase purchase, @NonNull Consumer<Boolean> completion) {
        BillingClient billingClient = m_billingClient;

        if (billingClient == null) {
            completion.accept(false);

            return;
        }

        this.logInfo("handleConsumablePurchase purchase: %s"
            , purchase
        );

        List<String> products = purchase.getProducts();
        String token = purchase.getPurchaseToken();

        ConsumeParams consumeParams = ConsumeParams.newBuilder()
            .setPurchaseToken(token)
            .build();

        billingClient.consumeAsync(consumeParams, (billingResult, purchaseToken) -> {
            if (m_billingClient != billingClient) {
                return;
            }

            int responseCode = billingResult.getResponseCode();

            if (responseCode != BillingClient.BillingResponseCode.OK) {
                this.logError("[ERROR] billing invalid consume code: %d message: %s"
                    , responseCode
                    , billingResult.getDebugMessage()
                );

                this.nativeCall("onGooglePlayBillingPurchasesOnConsumeFailed", products);
                completion.accept(false);

                return;
            }

            this.logInfo("billing success consume: %s products: %s"
                , billingResult.getDebugMessage()
                , products
            );

            this.purchaseInAppProduct(purchase, true);
            this.clearPurchaseDelivered(token);

            this.nativeCall("onGooglePlayBillingPurchasesOnConsumeSuccess", products, token);
            completion.accept(true);
        });
    }

    @AnyThread
    private void handlePurchases(List<? extends Purchase> purchases) {
        Consumer<Boolean> completion = successful -> {};

        for (Purchase purchase : purchases) {
            this.handlePurchase(purchase, completion);
        }
    }

    @AnyThread
    private void handlePurchase(@NonNull Purchase purchase, @NonNull Consumer<Boolean> completion) {
        this.logInfo("handlePurchase purchase: %s"
            , purchase
        );

        List<String> products = purchase.getProducts();
        String token = purchase.getPurchaseToken();

        if (this.beginPurchaseProcessing(token, completion) == false) {
            return;
        }

        Consumer<Boolean> processingCompletion = successful -> this.completePurchaseProcessing(token, successful);

        int state = purchase.getPurchaseState();

        switch (state) {
            case Purchase.PurchaseState.UNSPECIFIED_STATE: {
                this.logError("[ERROR] handlePurchase invalid purchase state: UNSPECIFIED_STATE purchase: %s"
                    , purchase
                );

                this.nativeCall("onGooglePlayBillingPurchaseUnspecifiedState", products);
                processingCompletion.accept(false);
            } break;
            case Purchase.PurchaseState.PURCHASED: {
                MengineCallback cb = MengineCallback.onMainThread((boolean successful, Map<String, Object> result) -> {
                    if (successful == false) {
                        this.logError("[ERROR] handlePurchase invalid isConsumable");
                        processingCompletion.accept(false);

                        return;
                    }

                    Boolean isConsumable = (Boolean)result.get("isConsumable");

                    if (isConsumable == null) {
                        this.logError("[ERROR] handlePurchase isConsumable is null");
                        processingCompletion.accept(false);

                        return;
                    }

                    boolean acknowledged = purchase.isAcknowledged();

                    this.logInfo("handlePurchase Acknowledged: %s product: %s consumable: %s"
                        , acknowledged
                        , products
                        , isConsumable
                    );

                    if (acknowledged == true) {
                        MengineCallback acknowledgedCb = MengineCallback.onMainThread((boolean acknowledgedSuccessful, Map<String, Object> result2) -> {
                            if (acknowledgedSuccessful == false) {
                                processingCompletion.accept(false);

                                return;
                            }

                            this.clearPurchaseDelivered(token);
                            processingCompletion.accept(true);
                        });

                        this.nativeCall("onGooglePlayBillingPurchaseAcknowledged", products, token, acknowledgedCb);

                        return;
                    }

                    if (isConsumable == true) {
                        this.deliverPurchase(purchase, true, processingCompletion);
                    } else {
                        this.deliverPurchase(purchase, false, processingCompletion);
                    }
                });

                this.nativeCall("onGooglePlayBillingPurchaseIsConsumable", products, cb);
            } break;
            case Purchase.PurchaseState.PENDING: {
                this.logInfo("handlePurchase [Pending] purchase: %s"
                    , purchase
                );

                this.nativeCall("onGooglePlayBillingPurchasePending", products);
                processingCompletion.accept(true);
            } break;
        }
    }
}
