//
//  transfer_group_request.cpp
//  proto
//
//  Created by WF Chat on 2017/12/2.
//  Copyright © 2017年 WildFireChat. All rights reserved.
//

#include "transfer_channel_request.h"
#include "pbc/pbc.h"

namespace mars{
    namespace stn{

        const char* TransferChannelRequest::getTypeName() {
            return "TransferChannel";
        }
        
        bool TransferChannelRequest::unserializeFromPBMsg(struct pbc_rmessage *msg, bool destroy) {
            channelId = getString(msg, keyChannelId, 0);
            newOwner = getString(msg, keyNewOwner, 0);
            
            if(destroy)
            finishRead(msg);
            return true;
        }
        
        void TransferChannelRequest::serializeToPBMsg(struct pbc_wmessage* msg) {
            setString(msg, keyChannelId, channelId);
            setString(msg, keyNewOwner, newOwner);
        }
    }
}
