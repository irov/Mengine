import UIKit
import Foundation
import AppTrackingTransparency
import AdSupport

@objc
class AppTracking:NSObject{
    
    @objc
    static func requestAuthorization() {
        DispatchQueue.main.async {
            guard #available(iOS 14, *) else { return }
            ATTrackingManager.requestTrackingAuthorization { status in
                DispatchQueue.main.async {
                    switch status {
                    case .authorized:
                        let idfa = ASIdentifierManager.shared().advertisingIdentifier
                        print("Пользователь разрешил доступ. IDFA: ", idfa)
                    case .denied, .restricted:
                        print("Пользователь запретил доступ.")
                    case .notDetermined:
                        print("Пользователь ещё не получил запрос на авторизацию.")
                    @unknown default:
                        break
                    }
                }
            }
        }
    }
}

