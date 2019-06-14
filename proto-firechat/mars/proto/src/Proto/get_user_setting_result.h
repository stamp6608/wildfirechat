//
//  add_friends_request.hpp
//  proto
//
//  Created by WF Chat on 2017/11/28.
//  Copyright © 2017年 WildFireChat. All rights reserved.
//

#ifndef get_user_setting_result_hpp
#define get_user_setting_result_hpp

#include <stdio.h>
#include <string>
#include <list>
#include "pbbase.h"
#include "user_setting_entry.h"

struct pbc_rmessage;
struct pbc_wmessage;

namespace mars{
    namespace stn{
        class PBBase;
      
        class GetUserSettingResult : public PBBase{
        protected:
            virtual const char* getTypeName();
        public:
            GetUserSettingResult() {}
            virtual ~GetUserSettingResult() {}
            virtual bool unserializeFromPBMsg(struct pbc_rmessage *msg, bool destroy);
            virtual void serializeToPBMsg(struct pbc_wmessage* msg);
            
            std::list<UserSettingEntry> entrys;
        };
    }
}
#endif /* get_user_setting_result_hpp */
