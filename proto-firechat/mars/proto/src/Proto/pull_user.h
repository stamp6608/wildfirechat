//
//  pull_user.hpp
//  proto
//
//  Created by WF Chat on 2017/12/2.
//  Copyright © 2017年 WildFireChat. All rights reserved.
//

#ifndef pull_user_hpp
#define pull_user_hpp

#include <stdio.h>
#include <string>
#include <list>
#include "user.h"
#include "pbbase.h"

struct pbc_rmessage;
struct pbc_wmessage;

namespace mars{
    namespace stn{
        class PBBase;
        
        typedef enum {
            Success = 0,
            NotFound = 1,
            NotModified = 2
        } PullUserResultCode;
        
        class UserRequest : public PBBase{
        protected:
            virtual const char* getTypeName();
        public:
            UserRequest() {}
            virtual ~UserRequest() {}
            virtual bool unserializeFromPBMsg(struct pbc_rmessage *msg, bool destroy);
            virtual void serializeToPBMsg(struct pbc_wmessage* msg);
            
            std::string uid;
            int64_t updateDt;
        };
        
        class PullUserRequest : public PBBase{
        protected:
            virtual const char* getTypeName();
        public:
            PullUserRequest() {}
            virtual ~PullUserRequest() {}
            virtual bool unserializeFromPBMsg(struct pbc_rmessage *msg, bool destroy);
            virtual void serializeToPBMsg(struct pbc_wmessage* msg);
            
            std::list<UserRequest> requests;
        };

        
        class UserResult : public PBBase{
        protected:
            virtual const char* getTypeName();
        public:
            UserResult() {}
            virtual ~UserResult() {}
            virtual bool unserializeFromPBMsg(struct pbc_rmessage *msg, bool destroy);
            virtual void serializeToPBMsg(struct pbc_wmessage* msg);
            
            User user;
            PullUserResultCode code;
        };
        
        class PullUserResult : public PBBase{
        protected:
            virtual const char* getTypeName();
        public:
            PullUserResult() {}
            virtual ~PullUserResult() {}
            virtual bool unserializeFromPBMsg(struct pbc_rmessage *msg, bool destroy);
            virtual void serializeToPBMsg(struct pbc_wmessage* msg);
            
            std::list<UserResult> results;
        };

    }
}
#endif /* add_friends_request_hpp */

