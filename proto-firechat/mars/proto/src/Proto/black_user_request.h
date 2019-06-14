//
//  add_friends_request.hpp
//  proto
//
//  Created by WF Chat on 2017/11/28.
//  Copyright © 2017年 WildFireChat. All rights reserved.
//

#ifndef black_user_request_hpp
#define black_user_request_hpp

#include <stdio.h>
#include <string>
#include "pbbase.h"

struct pbc_rmessage;
struct pbc_wmessage;

namespace mars{
    namespace stn{
        class PBBase;
        
        
        class BlackUserRequest : public PBBase{
        protected:
            virtual const char* getTypeName();
        public:
            BlackUserRequest() {}
            virtual ~BlackUserRequest() {}
            virtual bool unserializeFromPBMsg(struct pbc_rmessage *msg, bool destroy);
            virtual void serializeToPBMsg(struct pbc_wmessage* msg);
            
            std::string userId;
            int status;
        };
    }
}
#endif /* black_user_request_hpp */
