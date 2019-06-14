//
//  add_friends_request.hpp
//  proto
//
//  Created by WF Chat on 2017/11/28.
//  Copyright © 2017年 WildFireChat. All rights reserved.
//

#ifndef version_hpp
#define version_hpp

#include <stdio.h>
#include <string>
#include <list>
#include "pbbase.h"

struct pbc_rmessage;
struct pbc_wmessage;

namespace mars{
    namespace stn{
        class PBBase;
        
        class Version : public PBBase{
        protected:
            virtual const char* getTypeName();
        public:
            Version() {}
            virtual ~Version() {}
            virtual bool unserializeFromPBMsg(struct pbc_rmessage *msg, bool destroy);
            virtual void serializeToPBMsg(struct pbc_wmessage* msg);
            
            int64_t version;
        };
    }
}
#endif /* version_hpp */
