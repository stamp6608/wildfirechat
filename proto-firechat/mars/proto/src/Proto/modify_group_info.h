//
//  modify_group_info_request.hpp
//  proto
//
//  Created by WF Chat on 2017/12/2.
//  Copyright © 2017年 WildFireChat. All rights reserved.
//

#ifndef modify_group_info_request_hpp
#define modify_group_info_request_hpp

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
        
        class ModifyGroupInfoRequest : public PBBase{
        protected:
            virtual const char* getTypeName();
        public:
            ModifyGroupInfoRequest() {}
            virtual ~ModifyGroupInfoRequest() {}
            virtual bool unserializeFromPBMsg(struct pbc_rmessage *msg, bool destroy);
            virtual void serializeToPBMsg(struct pbc_wmessage* msg);
            
            std::string groupId;
            int type;
            std::string value;
            std::list<int> toLine;
            MessageContent notifyContent;
        };
    }
}
#endif /* add_friends_request_hpp */

