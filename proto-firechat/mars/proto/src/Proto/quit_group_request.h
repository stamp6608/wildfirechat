//
//  quit_group_request.hpp
//  proto
//
//  Created by WF Chat on 2017/12/2.
//  Copyright © 2017年 WildFireChat. All rights reserved.
//

#ifndef quit_group_request_hpp
#define quit_group_request_hpp

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
        
        
        class QuitGroupRequest : public PBBase{
        protected:
            virtual const char* getTypeName();
        public:
            QuitGroupRequest() {}
            virtual ~QuitGroupRequest() {}
            virtual bool unserializeFromPBMsg(struct pbc_rmessage *msg, bool destroy);
            virtual void serializeToPBMsg(struct pbc_wmessage* msg);
            
            std::string groupId;
            std::list<int> toLines;
            MessageContent notifyContent;
        };
    }
}
#endif /* add_friends_request_hpp */

