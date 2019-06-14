//
//  add_friends_request.hpp
//  proto
//
//  Created by WF Chat on 2017/11/28.
//  Copyright © 2017年 WildFireChat. All rights reserved.
//

#ifndef get_friend_hpp
#define get_friend_hpp

#include <stdio.h>
#include <string>
#include <list>
#include "pbbase.h"
#include "friend.h"

struct pbc_rmessage;
struct pbc_wmessage;

namespace mars{
    namespace stn{
        class PBBase;
      
        class GetFriendsResult : public PBBase{
        protected:
            virtual const char* getTypeName();
        public:
            GetFriendsResult() {}
            virtual ~GetFriendsResult() {}
            virtual bool unserializeFromPBMsg(struct pbc_rmessage *msg, bool destroy);
            virtual void serializeToPBMsg(struct pbc_wmessage* msg);
            
            std::list<Friend> entrys;
        };
    }
}
#endif /* get_friend_hpp */
