//
//  modify_group_info_request.hpp
//  proto
//
//  Created by WF Chat on 2017/12/2.
//  Copyright © 2017年 WildFireChat. All rights reserved.
//

#ifndef modify_group_info_alias_hpp
#define modify_group_info_alias_hpp

#include <stdio.h>
#include <string>
#include "pbbase.h"
#include "group.h"
#include <list>
#include "messagecontent.h"

struct pbc_rmessage;
struct pbc_wmessage;

namespace mars{
    namespace stn{
        class PBBase;        
        
        class ModifyGroupMemberAlias : public PBBase{
        protected:
            virtual const char* getTypeName();
        public:
            ModifyGroupMemberAlias() {}
            virtual ~ModifyGroupMemberAlias() {}
            virtual bool unserializeFromPBMsg(struct pbc_rmessage *msg, bool destroy);
            virtual void serializeToPBMsg(struct pbc_wmessage* msg);
            
            std::string groupId;
            std::string alias;
            std::list<int> toLine;
            MessageContent notifyContent;
        };
    }
}
#endif /* modify_group_info_alias_hpp */

