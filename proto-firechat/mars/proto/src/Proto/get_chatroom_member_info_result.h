//
//  quit_group_request.hpp
//  proto
//
//  Created by WF Chat on 2017/12/2.
//  Copyright © 2017年 WildFireChat. All rights reserved.
//

#ifndef get_chatroom_member_info_result_hpp
#define get_chatroom_member_info_result_hpp

#include <stdio.h>
#include <string>
#include <list>
#include "pbbase.h"

struct pbc_rmessage;
struct pbc_wmessage;

namespace mars{
    namespace stn{
        class PBBase;
        
        
        class ChatroomMemberInfo : public PBBase{
        protected:
            virtual const char* getTypeName();
        public:
            ChatroomMemberInfo() {}
            virtual ~ChatroomMemberInfo() {}
            virtual bool unserializeFromPBMsg(struct pbc_rmessage *msg, bool destroy);
            virtual void serializeToPBMsg(struct pbc_wmessage* msg);
            
            int memberCount;
            std::list<std::string> members;
        };
    }
}
#endif /* get_chatroom_member_info_result_hpp */

