//
//  add_friends_request.cpp
//  proto
//
//  Created by WF Chat on 2017/11/28.
//  Copyright © 2017年 WildFireChat. All rights reserved.
//

#include "listen_channel_request.h"
#include "pbc/pbc.h"

namespace mars{
    namespace stn{
        
        const char* ListenChannelRequest::getTypeName() {
            return "ListenChannel";
        }
        
        bool ListenChannelRequest::unserializeFromPBMsg(struct pbc_rmessage *msg, bool destroy) {
            channelId = getString(msg, keyChannelId, 0);
            listen = getInt(msg, keyListen, 0);
            if (destroy) {
                finishRead(msg);
            }
            return true;
        }
        
        void ListenChannelRequest::serializeToPBMsg(struct pbc_wmessage* msg) {
            setString(msg, keyChannelId, channelId);
            setInt(msg, keyListen, listen);
        }
    }
}
