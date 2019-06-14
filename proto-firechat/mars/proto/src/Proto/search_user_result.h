//
//  add_friends_request.hpp
//  proto
//
//  Created by WF Chat on 2017/11/28.
//  Copyright © 2017年 WildFireChat. All rights reserved.
//

#ifndef search_user_result_hpp
#define search_user_result_hpp

#include <stdio.h>
#include <string>
#include <list>
#include "pbbase.h"
#include "user.h"

struct pbc_rmessage;
struct pbc_wmessage;

namespace mars{
    namespace stn{
        class PBBase;
      
        class SearchUserResult : public PBBase{
        protected:
            virtual const char* getTypeName();
        public:
            SearchUserResult() {}
            virtual ~SearchUserResult() {}
            virtual bool unserializeFromPBMsg(struct pbc_rmessage *msg, bool destroy);
            virtual void serializeToPBMsg(struct pbc_wmessage* msg);
            
            std::list<User> entrys;
        };
    }
}
#endif /* search_user_result_hpp */
