//
//  quit_group_request.hpp
//  proto
//
//  Created by WF Chat on 2017/12/2.
//  Copyright © 2017年 WildFireChat. All rights reserved.
//

#ifndef get_chatroom_info_result_hpp
#define get_chatroom_info_result_hpp

#include <stdio.h>
#include <string>
#include <list>
#include "pbbase.h"

struct pbc_rmessage;
struct pbc_wmessage;

namespace mars{
    namespace stn{
        class PBBase;
        
        typedef enum {
            Chatroom_State_Normal = 0,
            Chatroom_State_NotStart = 1,
            Chatroom_State_End = 2,
        } ChatroomState;

        
        class ChatroomInfo : public PBBase{
        protected:
            virtual const char* getTypeName();
        public:
            ChatroomInfo() {}
            virtual ~ChatroomInfo() {}
            virtual bool unserializeFromPBMsg(struct pbc_rmessage *msg, bool destroy);
            virtual void serializeToPBMsg(struct pbc_wmessage* msg);
            
            std::string title;
            std::string desc;
            std::string portrait;
            int memberCount;
            int64_t createDt;
            int64_t updateDt;
            std::string extra;
            ChatroomState state;
        };
    }
}
#endif /* get_chatroom_info_result_hpp */

