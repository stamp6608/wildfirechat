//
//  add_friends_request.hpp
//  proto
//
//  Created by WF Chat on 2017/11/28.
//  Copyright © 2017年 WildFireChat. All rights reserved.
//

#ifndef friend_hpp
#define friend_hpp

#include <stdio.h>
#include <string>
#include "pbbase.h"

struct pbc_rmessage;
struct pbc_wmessage;

namespace mars{
    namespace stn{
        class PBBase;
        
        
        class Friend : public PBBase{
        protected:
            virtual const char* getTypeName();
        public:
            Friend() {}
            virtual ~Friend() {}
            virtual bool unserializeFromPBMsg(struct pbc_rmessage *msg, bool destroy);
            virtual void serializeToPBMsg(struct pbc_wmessage* msg);
            
            int state;
            std::string uid;
            int64_t updateDt;
            std::string alias;
        };
    }
}
#endif /* friend_hpp */
