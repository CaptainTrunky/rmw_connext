// Copyright 2014-2017 Open Source Robotics Foundation, Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "rcutils/format_string.h"
#include "rcutils/types.h"
#include "rcutils/split.h"

#include "rmw/rmw.h"
#include "rmw/allocators.h"
#include "rmw/error_handling.h"

#include "rmw_connext_shared_cpp/namespace_prefix.hpp"
#include "rmw_connext_shared_cpp/ndds_include.hpp"

bool
_process_topic_name(
  const char * topic_name,
  bool avoid_ros_namespace_conventions,
  char ** topic_str)
{
  bool success = true;
  rcutils_allocator_t allocator = rcutils_get_default_allocator();

  if (!avoid_ros_namespace_conventions) {
    char * concat_str =
      rcutils_format_string(allocator, "%s%s", ros_topic_prefix, topic_name);
    if (!concat_str) {
      RMW_SET_ERROR_MSG("could not allocate memory for topic string")
      success = false;
      goto end;
    }
    *topic_str = DDS_String_dup(concat_str);
    allocator.deallocate(concat_str, allocator.state);
  } else {
    *topic_str = DDS_String_dup(topic_name);
  }

end:
  return success;
}

bool
_process_service_name(
  const char * service_name,
  bool avoid_ros_namespace_conventions,
  char ** request_topic_str,
  char ** response_topic_str)
{
  bool success = true;

  rcutils_allocator_t allocator = rcutils_get_default_allocator();

  std::string _ros_request_prefix = "";
  std::string _ros_response_prefix = "";

  if (!avoid_ros_namespace_conventions) {
    // Set ros specific request and response prefixes
    _ros_request_prefix = ros_service_requester_prefix;
    _ros_response_prefix = ros_service_response_prefix;
  }

  {
    // concat the service_name with ros_service_*_prefix (if required) and Request/Reply suffixes
  char * request_concat_str = rcutils_format_string(
    allocator,
    "%s%s%s", ros_service_requester_prefix, service_name, "Request");
  if (!request_concat_str) {
    RMW_SET_ERROR_MSG("could not allocate memory for request topic string")
    success = false;
    goto end;
  }
  char * response_concat_str = rcutils_format_string(
    allocator,
    "%s%s%s", ros_service_response_prefix, service_name, "Reply");
  if (!response_concat_str) {
    allocator.deallocate(request_concat_str, allocator.state);
    RMW_SET_ERROR_MSG("could not allocate memory for response topic string")
    success = false;
    goto end;
  }
  *request_topic_str = DDS_String_dup(request_concat_str);
  *response_topic_str = DDS_String_dup(response_concat_str);
  allocator.deallocate(request_concat_str, allocator.state);
  allocator.deallocate(response_concat_str, allocator.state);
}

end:
  return success;
}
