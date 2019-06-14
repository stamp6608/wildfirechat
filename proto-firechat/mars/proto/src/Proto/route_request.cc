//
//  user.cpp
//  proto
//
//  Created by WF Chat on 2017/12/1.
//  Copyright © 2017年 WildFireChat. All rights reserved.
//

#include "route_request.h"
#include "pbc/pbc.h"

namespace mars{
    namespace stn{

        const char* RouteRequest::getTypeName() {
            return "RouteRequest";
        }
        
        bool RouteRequest::unserializeFromPBMsg(struct pbc_rmessage *msg, bool destroy) {
            app = getString(msg, keyApp, 0);
            platform = getInt(msg, keyPlatform, 0);
            pushType = getInt(msg, keyPushType, 0);
            deviceName = getString(msg, keyDeviceName, 0);
            deviceVersion = getString(msg, keyDeviceVersion, 0);
            phoneName = getString(msg, keyPhoneName, 0);
            language = getString(msg, keyLanguage, 0);
            carrierName = getString(msg, keyCarrierName, 0);
            appversion = getString(msg, keyAppVersion, 0);
            sdkversion = getString(msg, keySdkVersion, 0);
            
            if(destroy)
            finishRead(msg);
            return true;
        }
        
        void RouteRequest::serializeToPBMsg(struct pbc_wmessage* msg) {
            setString(msg, keyApp, app);
            setInt(msg, keyPlatform, platform);
            setInt(msg, keyPushType, pushType);
            setString(msg, keyDeviceName, deviceName);
            setString(msg, keyDeviceVersion, deviceVersion);
            setString(msg, keyPhoneName, phoneName);
            setString(msg, keyLanguage, language);
            setString(msg, keyCarrierName, carrierName);
            setString(msg, keyAppVersion, appversion);
            setString(msg, keySdkVersion, sdkversion);
        }
    }
}

