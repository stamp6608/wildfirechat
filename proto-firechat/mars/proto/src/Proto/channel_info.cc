//
//  group.cpp
//  proto
//
//  Created by WF Chat on 2017/11/29.
//  Copyright © 2017年 WildFireChat. All rights reserved.
//

#include "channel_info.h"
#include "pbc/pbc.h"

namespace mars{
    namespace stn{

        const char* ChannelInfo::getTypeName() {
            return "ChannelInfo";
        }
        
        bool ChannelInfo::unserializeFromPBMsg(struct pbc_rmessage *msg, bool destroy) {
            targetId = getString(msg, keyTargetId, 0);
            name = getString(msg, keyName, 0);
            portrait = getString(msg, keyPortrait, 0);
            owner = getString(msg, keyOwner, 0);
            status = getInt(msg, keyStatus, 0);
            desc = getString(msg, keyDesc, 0);
            extra = getString(msg, keyExtra, 0);
            updateDt = getInt64(msg, keyUpdateDt, 0);
            secret = getString(msg, keySecret, 0);
            callback = getString(msg, keyCallback, 0);
            automatic = getInt(msg, keyAutomatic, 0);
            if(destroy)
            finishRead(msg);
            return true;
        }
        
        void ChannelInfo::serializeToPBMsg(struct pbc_wmessage* msg)  {
            setString(msg, keyTargetId, targetId);
            setString(msg, keyName, name);
            setString(msg, keyPortrait, portrait);
            setString(msg, keyOwner, owner);
            setInt(msg, keyStatus, status);
            setString(msg, keyDesc, desc);
            setString(msg, keyExtra, extra);
            setString(msg, keySecret, secret);
            setString(msg, keyCallback, callback);
            setInt(msg, keyAutomatic, automatic);
            setInt64(msg, keyUpdateDt, updateDt);
        }
    }
}
