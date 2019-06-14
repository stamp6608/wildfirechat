//
//  handle_friends_request.hpp
//  proto
//
//  Created by WF Chat on 2017/12/1.
//  Copyright © 2017年 WildFireChat. All rights reserved.
//

#ifndef handle_friends_request_hpp
#define handle_friends_request_hpp

#include <stdio.h>
#include <string>
#include "pbbase.h"

struct pbc_rmessage;
struct pbc_wmessage;

namespace mars{
    namespace stn{
        class PBBase;
        
        
        class HandleFriendRequest : public PBBase{
        protected:
            virtual const char* getTypeName();
        public:
            HandleFriendRequest() {}
            virtual ~HandleFriendRequest() {}
            virtual bool unserializeFromPBMsg(struct pbc_rmessage *msg, bool destroy);
            virtual void serializeToPBMsg(struct pbc_wmessage* msg);
            
            std::string targetUid;
            int status;
        };
    }
}
#endif /* add_friends_request_hpp */



