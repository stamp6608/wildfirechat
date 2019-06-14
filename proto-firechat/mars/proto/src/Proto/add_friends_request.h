//
//  add_friends_request.hpp
//  proto
//
//  Created by WF Chat on 2017/11/28.
//  Copyright © 2017年 WildFireChat. All rights reserved.
//

#ifndef add_friends_request_hpp
#define add_friends_request_hpp

#include <stdio.h>
#include <string>
#include "pbbase.h"

struct pbc_rmessage;
struct pbc_wmessage;

namespace mars{
    namespace stn{
        class PBBase;
        
        
        class AddFriendRequest : public PBBase{
        protected:
            virtual const char* getTypeName();
        public:
            AddFriendRequest() {}
            virtual ~AddFriendRequest() {}
            virtual bool unserializeFromPBMsg(struct pbc_rmessage *msg, bool destroy);
            virtual void serializeToPBMsg(struct pbc_wmessage* msg);
            
            std::string targetUid;
            std::string reason;
        };
    }
}
#endif /* add_friends_request_hpp */
