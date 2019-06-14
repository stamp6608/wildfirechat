//
//  pull_group_member.hpp
//  proto
//
//  Created by WF Chat on 2017/12/2.
//  Copyright © 2017年 WildFireChat. All rights reserved.
//

#ifndef pull_group_member_hpp
#define pull_group_member_hpp

#include <stdio.h>
#include <string>
#include <list>
#include "pbbase.h"
#include "group.h"

struct pbc_rmessage;
struct pbc_wmessage;

namespace mars{
    namespace stn{
        class PBBase;
        
        
        class PullGroupMemberRequest : public PBBase{
        protected:
            virtual const char* getTypeName();
        public:
            PullGroupMemberRequest() {}
            virtual ~PullGroupMemberRequest() {}
            virtual bool unserializeFromPBMsg(struct pbc_rmessage *msg, bool destroy);
            virtual void serializeToPBMsg(struct pbc_wmessage* msg);
            
            std::string target;
            int64_t head;
        };
        
        class PullGroupMemberResult : public PBBase{
        protected:
            virtual const char* getTypeName();
        public:
            PullGroupMemberResult() {}
            virtual ~PullGroupMemberResult() {}
            virtual bool unserializeFromPBMsg(struct pbc_rmessage *msg, bool destroy);
            virtual void serializeToPBMsg(struct pbc_wmessage* msg);
            
            std::list<GroupMember> members;
        };
    }
}
#endif /* add_friends_request_hpp */

