//
//  modify_group_info_request.cpp
//  proto
//
//  Created by WF Chat on 2017/12/2.
//  Copyright © 2017年 WildFireChat. All rights reserved.
//

#include "modify_channel_info.h"
#include "pbc/pbc.h"

namespace mars{
    namespace stn{
        
        const char* ModifyChannelInfo::getTypeName() {
            return "ModifyChannelInfo";
        }
        
        bool ModifyChannelInfo::unserializeFromPBMsg(struct pbc_rmessage *msg, bool destroy) {
            if(destroy)
                finishRead(msg);
            return true;
        }
        
        void ModifyChannelInfo::serializeToPBMsg(struct pbc_wmessage* msg) {
            setString(msg, keyChannelId, channelId);
            setInt(msg, keyType, type);
            setString(msg, keyValue, value);
        }
    }
}
