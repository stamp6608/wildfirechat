//
//  upload_device_token_request.cpp
//  proto
//
//  Created by WF Chat on 2017/12/2.
//  Copyright © 2017年 WildFireChat. All rights reserved.
//

#include "upload_device_token_request.h"
#include "pbc/pbc.h"

namespace mars{
    namespace stn{
        
        const char* UploadDeviceTokenRequest::getTypeName() {
            return "UploadDeviceTokenRequest";
        }
        
        bool UploadDeviceTokenRequest::unserializeFromPBMsg(struct pbc_rmessage *msg, bool destroy) {
            platform = (Platform)getInt(msg, keyPlatform, 0);
            appName = getString(msg, keyAppName, 0);
            deviceToken = getString(msg, keyDeviceToken, 0);
            pushType = getInt(msg, keyPushType, 0);
          
            if(destroy)
            finishRead(msg);
            return true;
        }
        
        void UploadDeviceTokenRequest::serializeToPBMsg(struct pbc_wmessage* msg) {
            setInt(msg, keyPlatform, platform);
            setString(msg, keyAppName, appName);
            setString(msg, keyDeviceToken, deviceToken);
            setInt(msg, keyPushType, pushType);
        }
    }
}
