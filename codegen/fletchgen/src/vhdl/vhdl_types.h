// Copyright 2018 Delft University of Technology
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

#pragma once

#include <memory>
#include <string>

#include "../types.h"
#include "../nodes.h"

namespace fletchgen {
namespace vhdl {

// VHDL implementation specific types

std::shared_ptr<Type> valid();
std::shared_ptr<Type> ready();

// VHDL port stuff

std::string ToString(Port::Dir dir);
Port::Dir Reverse(Port::Dir dir);

// VHDL type checking

bool IsCompatible(const std::shared_ptr<Node> &a, const std::shared_ptr<Node> &b);

}  // namespace vhdl
}  // namespace fletchgen