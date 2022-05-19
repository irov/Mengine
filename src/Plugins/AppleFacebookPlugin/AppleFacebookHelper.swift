import Foundation
import FBSDKCoreKit
import FBSDKShareKit




typealias cb_SwiftToObjC  = (Int, String?) -> Void;

class FBDelegate : FBSDKShareKit.SharingDelegate{
    
    var m_cb:cb_SwiftToObjC;
    
    public init(cb: cb_SwiftToObjC!){
        m_cb = cb;
    }
    
    func sharer(_ sharer: Sharing, didCompleteWithResults results: [String : Any]) {
        m_cb(0, sharer.shareContent?.placeID);
    }
    
    func sharer(_ sharer: Sharing, didFailWithError error: Error) {
        m_cb(1,error.localizedDescription)
    }
    
    func sharerDidCancel(_ sharer: Sharing) {
        m_cb(2,nil)
    }
    
    
}
@objc
class AppleFacebookHelper:NSObject{
    
    @objc
    static func ShareImageLink(link:String, image:String,cb:@escaping cb_SwiftToObjC) {
        DispatchQueue.main.async {
            
            if image.isEmpty {
                
                let content = ShareLinkContent()
                content.contentURL = URL(string: link)
                
                let dialog = ShareDialog(
                    viewController: UIApplication.shared.delegate!.window!!.rootViewController,
                    content: content,
                    delegate: FBDelegate.init(cb: cb)
                )
                dialog.show()
                
            }
            else
            
            //непонятно картинка в каком формате будет передаватся
            //дата или путь  ??
            if let data = try? Data(contentsOf: URL(string: image)!) {
                if let image = UIImage(data: data) {
                    
                    DispatchQueue.main.async{
                        let photo = SharePhoto(image: image, isUserGenerated:true)
                        
                        let content = ShareMediaContent()
                        content.contentURL = URL(string: link)
                        content.media = [photo]
                        
                        let dialog = ShareDialog(
                            viewController: UIApplication.shared.delegate!.window!!.rootViewController,
                            content: content,
                            delegate: FBDelegate.init(cb: cb)
                        )
                        dialog.show()
                    }
                    
                }
            }
            
            else
            {

                cb(1, "empty data");
            }
        }
        
    }
}

