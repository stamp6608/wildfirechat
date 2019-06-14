// Tencent is pleased to support the open source community by making Mars available.
// Copyright (C) 2016 THL A29 Limited, a Tencent company. All rights reserved.

// Licensed under the MIT License (the "License"); you may not use this file except in 
// compliance with the License. You may obtain a copy of the License at
// http://opensource.org/licenses/MIT

// Unless required by applicable law or agreed to in writing, software distributed under the License is
// distributed on an "AS IS" basis, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,
// either express or implied. See the License for the specific language governing permissions and
// limitations under the License.

//
//  stn.cpp
//  stn
//
//  Created by yanguoyue on 16/3/3.
//  Copyright © 2016年 Tencent. All rights reserved.
//

#include "mars/stn/stn.h"

#include "mars/comm/thread/atomic_oper.h"
#include <sstream>

namespace mars{
    namespace stn{
        
static uint32_t gs_taskid = 1;
Task::Task():Task(atomic_inc32(&gs_taskid) % 254 + 1) {}
        
Task::Task(uint32_t _taskid) {
    
    taskid = _taskid;
    cmdid = 0;
    channel_id = 0;
    channel_select = 0;
    
    send_only = false;
    need_authed = false;
    limit_flow = true;
    limit_frequency = true;
    
    channel_strategy = kChannelNormalStrategy;
    network_status_sensitive = false;
    priority = kTaskPriorityNormal;
    
    retry_count = -1;
    server_process_cost = -1;
    total_timetout = -1;
    user_context = NULL;
    isRoute = false;
}
const std::string Task::description() const {
    std::stringstream stream;
    stream << "taskid:";
    stream << taskid;
    stream << " cmdid:";
    stream << cmdid;
    stream << " channel_id:";
    stream << channel_id;
    stream << " channel_select:";
    stream << channel_select;
    stream << " cgi:";
    stream << cgi;
    
    stream << " send_only:";
    stream << send_only;
    stream << " need_authed:";
    stream << need_authed;
    stream << " limit_flow:";
    stream << limit_flow;
    
    stream << " limit_frequency:";
    stream << limit_frequency;
    stream << " network_status_sensitive:";
    stream << network_status_sensitive;
    stream << " channel_strategy:";
    stream << channel_strategy;
    stream << " priority:";
    stream << priority;
    
    stream << " retry_count:";
    stream << retry_count;
    stream << " server_process_cost:";
    stream << server_process_cost;
    stream << " total_timetout:";
    stream << total_timetout;
    
    return stream.str();
}
        
    }
}
