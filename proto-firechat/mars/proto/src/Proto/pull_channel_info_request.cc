//
//  pull_group_member.cpp
//  proto
//
//  Created by WF Chat on 2017/12/2.
//  Copyright © 2017年 WildFireChat. All rights reserved.
//

#include "pull_channel_info_request.h"
#include "pbc/pbc.h"

namespace mars{
    namespace stn{
        
        const char* PullChannelInfoRequest::getTypeName() {
            return "PullChannelInfo";
        }
        
        bool PullChannelInfoRequest::unserializeFromPBMsg(struct pbc_rmessage *msg, bool destroy) {
            channelId = getString(msg, keyChannelId, 0);
            head = getInt64(msg, keyHead, 0);
            
            if(destroy)
            finishRead(msg);
            return true;
        }
        
        void PullChannelInfoRequest::serializeToPBMsg(struct pbc_wmessage* msg) {
            setString(msg, keyChannelId, channelId);
            setInt64(msg, keyHead, head);
        }
    }
}
