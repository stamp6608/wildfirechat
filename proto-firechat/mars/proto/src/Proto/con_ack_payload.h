//
//  add_friends_request.hpp
//  proto
//
//  Created by WF Chat on 2017/11/28.
//  Copyright © 2017年 WildFireChat. All rights reserved.
//

#ifndef con_ack_payload_hpp
#define con_ack_payload_hpp

#include <stdio.h>
#include <string>
#include <list>
#include "pbbase.h"
#include "user_setting_entry.h"

struct pbc_rmessage;
struct pbc_wmessage;

namespace mars{
    namespace stn{
        class PBBase;
        
        class ConnectAckPayload : public PBBase{
        protected:
            virtual const char* getTypeName();
        public:
            ConnectAckPayload() {} 
            virtual ~ConnectAckPayload() {}
            virtual bool unserializeFromPBMsg(struct pbc_rmessage *msg, bool destroy);
            virtual void serializeToPBMsg(struct pbc_wmessage* msg);
          
            int64_t msg_head;
            int64_t friend_head;
            int64_t friend_rq_head;
            int64_t setting_head;
            std::string node_addr;
            int32_t node_port;
            int64_t server_time;
        };
        

    }
}
#endif /* con_ack_payload_hpp */
