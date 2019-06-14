//
//  get_upload_token_result.hpp
//  proto
//
//  Created by WF Chat on 2017/12/1.
//  Copyright © 2017年 WildFireChat. All rights reserved.
//

#ifndef get_upload_token_result_hpp
#define get_upload_token_result_hpp

#include <stdio.h>
#include <string>
#include "pbbase.h"

struct pbc_rmessage;
struct pbc_wmessage;

namespace mars{
    namespace stn{
        class PBBase;
        
        
        class GetUploadTokenResult : public PBBase{
        protected:
            virtual const char* getTypeName();
        public:
            GetUploadTokenResult() {}
            virtual ~GetUploadTokenResult() {}
            virtual bool unserializeFromPBMsg(struct pbc_rmessage *msg, bool destroy);
            virtual void serializeToPBMsg(struct pbc_wmessage* msg);
            
            std::string domain;
            std::string token;
            std::string server;
            int port;
        };
    }
}
#endif /* add_friends_request_hpp */


