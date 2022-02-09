import Foundation
import AppTrackingTransparency
import AdSupport

@objc
class AppleAppTrackingNative:NSObject{
    
    @objc
    static func requestAuthorization(cb: ((Int, String?) -> Void)? = nil) {
        DispatchQueue.main.async {
            guard #available(iOS 14, *) else { return }
            ATTrackingManager.requestTrackingAuthorization { status in
                DispatchQueue.main.async {
                    switch status {
                    case .authorized:
                        let idfa = ASIdentifierManager.shared().advertisingIdentifier
                        cb?(0, idfa.uuidString)
                        break
                    case .denied:
                        cb?(1, nil)
                        break
                    case .restricted:
                        cb?(2, nil)
                        break
                    case .notDetermined:
                        cb?(3, nil)
                        break
                    @unknown default:
                        break
                    }
                }
            }
        }
    }
}

