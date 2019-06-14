//
//  quit_group_request.hpp
//  proto
//
//  Created by WF Chat on 2017/12/2.
//  Copyright © 2017年 WildFireChat. All rights reserved.
//

#ifndef get_chatroom_member_info_request_hpp
#define get_chatroom_member_info_request_hpp

#include <stdio.h>
#include <string>
#include <list>
#include "pbbase.h"

struct pbc_rmessage;
struct pbc_wmessage;

namespace mars{
    namespace stn{
        class PBBase;
        
        
        class GetChatroomMemberInfoRequest : public PBBase{
        protected:
            virtual const char* getTypeName();
        public:
            GetChatroomMemberInfoRequest() {}
            virtual ~GetChatroomMemberInfoRequest() {}
            virtual bool unserializeFromPBMsg(struct pbc_rmessage *msg, bool destroy);
            virtual void serializeToPBMsg(struct pbc_wmessage* msg);
            
            std::string chatroomId;
            int maxCount;
        };
    }
}
#endif /* get_chatroom_member_info_request_hpp */

