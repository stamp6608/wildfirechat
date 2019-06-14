//
//  MessageContent.hpp
//  proto
//
//  Created by WF Chat on 2017/11/30.
//  Copyright © 2017年 WildFireChat. All rights reserved.
//

#ifndef MessageContent_hpp
#define MessageContent_hpp

#include <stdio.h>
#include <string>
#include <list>
#include "pbbase.h"

struct pbc_rmessage;
struct pbc_wmessage;

namespace mars{
    namespace stn{
        class PBBase;

        
        class MessageContent : public PBBase{
        protected:
            virtual const char* getTypeName();
        public:
            MessageContent() {}
            virtual ~MessageContent() {}
            virtual bool unserializeFromPBMsg(struct pbc_rmessage *msg, bool destroy);
            virtual void serializeToPBMsg(struct pbc_wmessage* msg);
            
            int type;
            std::string searchableContent;
            std::string pushContent;
            std::string content;
            std::string data;
            int mediaType;
            std::string remoteMediaUrl;
            int persistFlag;
            int expireDuration;
            int mentionedType;
            std::list<std::string> mentionedTargets;
        };
    }
}

#endif /* MessageContent_hpp */
