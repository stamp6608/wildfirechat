//
//  remove_group_member_request.hpp
//  proto
//
//  Created by WF Chat on 2017/12/2.
//  Copyright © 2017年 WildFireChat. All rights reserved.
//

#ifndef remove_group_member_request_hpp
#define remove_group_member_request_hpp

#include <stdio.h>
#include <string>
#include <list>
#include "pbbase.h"
#include "messagecontent.h"


struct pbc_rmessage;
struct pbc_wmessage;

namespace mars{
    namespace stn{
        class PBBase;
        
        class RemoveGroupMemberRequest : public PBBase{
        protected:
            virtual const char* getTypeName();
        public:
            RemoveGroupMemberRequest() {}
            virtual ~RemoveGroupMemberRequest() {}
            virtual bool unserializeFromPBMsg(struct pbc_rmessage *msg, bool destroy);
            virtual void serializeToPBMsg(struct pbc_wmessage* msg);
            
            std::string groupId;
            std::list<std::string> removedMembers;
            std::list<int> toLines;
            MessageContent notifyContent;
        };
    }
}
#endif /* add_friends_request_hpp */

