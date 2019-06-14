//
//  add_group_member_request.hpp
//  proto
//
//  Created by WF Chat on 2017/11/30.
//  Copyright © 2017年 WildFireChat. All rights reserved.
//

#ifndef add_group_member_request_hpp
#define add_group_member_request_hpp

#include <stdio.h>
#include <string>
#include <list>
#include "group.h"
#include "messagecontent.h"
#include "pbbase.h"

struct pbc_rmessage;
struct pbc_wmessage;

namespace mars{
    namespace stn{
        class PBBase;
        
        
        class AddGroupMemberRequest : public PBBase{
        protected:
            virtual const char* getTypeName();
        public:
            AddGroupMemberRequest() {}
            virtual ~AddGroupMemberRequest() {}
            virtual bool unserializeFromPBMsg(struct pbc_rmessage *msg, bool destroy);
            virtual void serializeToPBMsg(struct pbc_wmessage* msg);
            
            std::string groupId;
            std::list<GroupMember> addedMembers;
            std::vector<int> toLines;
            MessageContent notifyContent;
        };
    }
}
#endif /* add_group_member_request_hpp */
