//
//  add_friends_request.cpp
//  proto
//
//  Created by WF Chat on 2017/11/28.
//  Copyright © 2017年 WildFireChat. All rights reserved.
//

#include "search_channel_result.h"
#include "pbc/pbc.h"

namespace mars{
    namespace stn{
        
      
        const char* SearchChannelResult::getTypeName() {
            return "SearchChannelResult";
        }
        
        bool SearchChannelResult::unserializeFromPBMsg(struct pbc_rmessage *msg, bool destroy) {
            for (int i = 0; i < getSize(msg, keyChannel); i++) {
                struct pbc_rmessage *tmpMsg = getSubMessage(msg, keyChannel, i);
                ChannelInfo result;
                result.unserializeFromPBMsg(tmpMsg, false);
                channels.push_back(result);
            }
            keyword = getString(msg, keyKeyword, 0);
            if (destroy) {
                finishRead(msg);
            }
            return true;
        }
        
        void SearchChannelResult::serializeToPBMsg(struct pbc_wmessage* msg) {
            
        }
    }
}
