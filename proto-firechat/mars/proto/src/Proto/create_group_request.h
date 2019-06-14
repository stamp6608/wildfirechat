//
//  create_group_request.hpp
//  proto
//
//  Created by WF Chat on 2017/11/30.
//  Copyright © 2017年 WildFireChat. All rights reserved.
//

#ifndef create_group_request_hpp
#define create_group_request_hpp

#include <stdio.h>
#include <string>
#include "group.h"
#include "messagecontent.h"
#include "pbbase.h"

struct pbc_rmessage;
struct pbc_wmessage;

namespace mars{
    namespace stn{
        class PBBase;
        
        
        class CreateGroupRequest : public PBBase{
        protected:
            virtual const char* getTypeName();
        public:
            CreateGroupRequest() {}
            virtual ~CreateGroupRequest() {}
            virtual bool unserializeFromPBMsg(struct pbc_rmessage *msg, bool destroy);
            virtual void serializeToPBMsg(struct pbc_wmessage* msg);
            
            Group group;
            std::vector<int> toLines;
            MessageContent notifyContent;
        };
    }
}
#endif /* create_group_request_hpp */
