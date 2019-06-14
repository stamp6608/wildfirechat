//
//  add_friends_request.cpp
//  proto
//
//  Created by WF Chat on 2017/11/28.
//  Copyright © 2017年 WildFireChat. All rights reserved.
//

#include "con_ack_payload.h"
#include "pbc/pbc.h"

namespace mars{
    namespace stn{
        
        const char* ConnectAckPayload::getTypeName() {
            return "ConnectAckPayload";
        }
        
        bool ConnectAckPayload::unserializeFromPBMsg(struct pbc_rmessage *msg, bool destroy) {
            msg_head = getInt64(msg, keyMsgHead, 0);
            friend_head = getInt64(msg, keyFriendHead, 0);
            friend_rq_head = getInt64(msg, keyFriendRqHead, 0);
            setting_head = getInt64(msg, keySettingHead, 0);
            node_addr = getString(msg, keyNodeAddr, 0);
            node_port = getInt(msg, keyNodePort, 0);
            server_time = getInt64(msg, keyServerTime, 0);
            if (destroy) {
                finishRead(msg);
            }
            return true;
        }
        
        void ConnectAckPayload::serializeToPBMsg(struct pbc_wmessage* msg) {
            setInt64(msg, keyMsgHead, msg_head);
            setInt64(msg, keyFriendHead, friend_head);
            setInt64(msg, keyFriendRqHead, friend_rq_head);
            setInt64(msg, keySettingHead, setting_head);
            setString(msg, keyNodeAddr, node_addr);
            setInt(msg, keyNodePort, node_port);
            setInt64(msg, keyServerTime, server_time);
        }
    }
}
