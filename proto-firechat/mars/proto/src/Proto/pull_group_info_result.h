//
//  pull_group_info_result.hpp
//  proto
//
//  Created by WF Chat on 2017/12/2.
//  Copyright © 2017年 WildFireChat. All rights reserved.
//

#ifndef pull_group_info_result_hpp
#define pull_group_info_result_hpp

#include <stdio.h>
#include <string>
#include <list>
#include "pbbase.h"
#include "group.h"

struct pbc_rmessage;
struct pbc_wmessage;

namespace mars{
    namespace stn{
        class PBBase;
        
        class PullGroupInfoResult : public PBBase{
        protected:
            virtual const char* getTypeName();
        public:
            PullGroupInfoResult() {}
            virtual ~PullGroupInfoResult() {}
            virtual bool unserializeFromPBMsg(struct pbc_rmessage *msg, bool destroy);
            virtual void serializeToPBMsg(struct pbc_wmessage* msg);
            
            std::list<GroupInfo> infos;
        };
    }
}
#endif /* add_friends_request_hpp */

