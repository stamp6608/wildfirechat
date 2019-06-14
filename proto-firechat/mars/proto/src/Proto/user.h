//
//  user.hpp
//  proto
//
//  Created by WF Chat on 2017/12/1.
//  Copyright © 2017年 WildFireChat. All rights reserved.
//

#ifndef user_hpp
#define user_hpp

#include <stdio.h>
#include <string>
#include "pbbase.h"

struct pbc_rmessage;
struct pbc_wmessage;

namespace mars{
    namespace stn{
        class PBBase;
        
        
        class User : public PBBase{
        protected:
            virtual const char* getTypeName();
        public:
            User() {}
            virtual ~User() {}
            virtual bool unserializeFromPBMsg(struct pbc_rmessage *msg, bool destroy);
            virtual void serializeToPBMsg(struct pbc_wmessage* msg);
            
            std::string uid;
            std::string name;
            std::string displayName;
            std::string portrait;
            std::string mobile;
            std::string email;
            std::string address;
            std::string company;
            std::string extra;
            int64_t updateDt;
            int type;
            int gender;
            std::string social;
        };
    }
}
#endif /* add_friends_request_hpp */
