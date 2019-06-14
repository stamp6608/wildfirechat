//
//  add_friends_request.cpp
//  proto
//
//  Created by WF Chat on 2017/11/28.
//  Copyright © 2017年 WildFireChat. All rights reserved.
//

#include "route_response.h"
#include "pbc/pbc.h"

namespace mars{
    namespace stn{
        
        const char* RouteResponse::getTypeName() {
            return "RouteResponse";
        }
        
        bool RouteResponse::unserializeFromPBMsg(struct pbc_rmessage *msg, bool destroy) {
            host = getString(msg, keyHost, 0);
            longPort = getInt(msg, keyLongPort, 0);
            shortPort = getInt(msg, keyShortPort, 0);
            
            if (destroy) {
                finishRead(msg);
            }
            return true;
        }
        
        void RouteResponse::serializeToPBMsg(struct pbc_wmessage* msg) {
            setString(msg, keyHost, host);
            setInt(msg, keyLongPort, longPort);
            setInt(msg, keyShortPort, shortPort);
        }
    }
}
