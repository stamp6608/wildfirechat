//
//  add_friends_request.hpp
//  proto
//
//  Created by WF Chat on 2017/11/28.
//  Copyright © 2017年 WildFireChat. All rights reserved.
//

#ifndef modify_user_setting_request_hpp
#define modify_user_setting_request_hpp

#include <stdio.h>
#include <string>
#include "pbbase.h"

struct pbc_rmessage;
struct pbc_wmessage;

namespace mars{
    namespace stn{
        class PBBase;
        
        
        class ModifyUserSettingReq : public PBBase{
        protected:
            virtual const char* getTypeName();
        public:
            ModifyUserSettingReq() {}
            virtual ~ModifyUserSettingReq() {}
            virtual bool unserializeFromPBMsg(struct pbc_rmessage *msg, bool destroy);
            virtual void serializeToPBMsg(struct pbc_wmessage* msg);
            
            int scope;
            std::string key;
            std::string value;
        };
    }
}
#endif /* modify_user_setting_request_hpp */
