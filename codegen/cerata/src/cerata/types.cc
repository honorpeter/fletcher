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

#include "cerata/types.h"

#include <utility>
#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>

#include "cerata/nodes.h"
#include "cerata/types.h"
#include "cerata/flattypes.h"

namespace cerata {

bool Type::Is(Type::ID type_id) const {
  return type_id == id_;
}

Type::Type(std::string name, Type::ID id)
    : Named(std::move(name)), id_(id) {}

bool Type::IsAbstract() const {
  return Is(STRING) || Is(BOOLEAN) || Is(RECORD) || Is(STREAM);
}

bool Type::IsSynthPrim() const {
  return Is(CLOCK) || Is(RESET) || Is(BIT) || Is(VECTOR);
}

bool Type::IsNested() const {
  return (id_ == Type::STREAM) || (id_ == Type::RECORD);
}

std::string Type::ToString() const {
  switch (id_) {
    case CLOCK  : return name() + ":Clock";
    case RESET  : return name() + ":Reset";
    case BIT    : return name() + ":Bit";
    case VECTOR : return name() + ":Vector";
    case INTEGER: return name() + ":Natural";
    case STRING : return name() + ":String";
    case BOOLEAN: return name() + ":Boolean";
    case RECORD : return name() + ":Record";
    case STREAM : return name() + ":Stream";
    default :throw std::runtime_error("Cannot return unknown Type ID as string.");
  }
}

std::deque<std::shared_ptr<TypeMapper>> Type::mappers() const {
  return mappers_;
}

void Type::AddMapper(std::shared_ptr<TypeMapper> mapper) {
  Type *other = (Type *) mapper->b();
  if (mapper->a() != this) {
    throw std::runtime_error("Type converter does not convert from " + name());
  }

  // Add the converter to this Type
  mappers_.push_back(mapper);

  // If the other type doesnt already have it, add the inverse map there as well.
  if (!other->GetMapper(this)) {
    other->AddMapper(mapper->Inverse());
  }
}

std::optional<std::shared_ptr<TypeMapper>> Type::GetMapper(const Type *other) const {
  // Search for an explicit type mapper.
  for (const auto &m : mappers_) {
    if (m->CanConvert(this, other)) {
      return m;
    }
  }
  // Implicit type mappers maybe be generated in two cases; if it's exactly the same type or if its an equal type.
  if (other == this) {
    // Generate a type mapper to itself using the TypeMapper constructor.
    return TypeMapper::Make(this);
  }
  if (IsEqual(other)) {
    // Generate an implicit type mapping.
    return TypeMapper::MakeImplicit(this, other);
  }
  // There is no mapper
  return {};
}

bool Type::IsEqual(const Type *other) const {
  return other->id() == id_;
}

Vector::Vector(std::string name, std::shared_ptr<Type> element_type, std::optional<std::shared_ptr<Node>> width)
    : Type(std::move(name), Type::VECTOR), element_type_(std::move(element_type)) {
  // Check if width is parameter or literal node
  if (width) {
    if (!((*width)->IsParameter() || (*width)->IsLiteral() || (*width)->IsExpression())) {
      throw std::runtime_error("Vector width can only be Parameter, Literal or Expression node.");
    }
  }
  width_ = width;
}

std::shared_ptr<Type> Vector::Make(std::string name,
                                   std::shared_ptr<Type> element_type,
                                   std::optional<std::shared_ptr<Node>> width) {
  return std::make_shared<Vector>(name, element_type, width);
}

std::shared_ptr<Type> Vector::Make(std::string name, std::optional<std::shared_ptr<Node>> width) {
  return std::make_shared<Vector>(name, bit(), width);
}

bool Vector::IsEqual(const Type *other) const {
  // Must also be a vector
  if (other->Is(Type::VECTOR)) {
    // Must have the same width
    if (width_ == other->width()) {
      return true;
    }
  }
  return false;
}

std::shared_ptr<Type> Stream::Make(std::string name, std::shared_ptr<Type> element_type, int epc) {
  return std::make_shared<Stream>(name, element_type, "data", epc);
}

std::shared_ptr<Type> Stream::Make(std::string name,
                                   std::shared_ptr<Type> element_type,
                                   std::string element_name,
                                   int epc) {
  return std::make_shared<Stream>(name, element_type, element_name, epc);
}

std::shared_ptr<Type> Stream::Make(std::shared_ptr<Type> element_type, int epc) {
  return std::make_shared<Stream>("stream-" + element_type->name(), element_type, "data", epc);
}

Stream::Stream(const std::string &type_name, std::shared_ptr<Type> element_type, std::string element_name, int epc)
    : Type(type_name, Type::STREAM),
      element_type_(std::move(element_type)),
      element_name_(std::move(element_name)),
      epc_(epc) {}

bool Stream::IsEqual(const Type *other) const {
  if (other->Is(Type::STREAM)) {
    return element_type()->IsEqual((*Cast<Stream>(other))->element_type().get());
  }
  return false;
}

std::shared_ptr<Type> bit() {
  static std::shared_ptr<Type> result = std::make_shared<Bit>("bit");
  return result;
}

std::shared_ptr<Type> string() {
  static std::shared_ptr<Type> result = std::make_shared<String>("string");
  return result;
}

std::shared_ptr<Type> integer() {
  static std::shared_ptr<Type> result = std::make_shared<Integer>("integer");
  return result;
}

std::shared_ptr<Type> boolean() {
  static std::shared_ptr<Type> result = std::make_shared<Boolean>("boolean");
  return result;
}

std::shared_ptr<Type> Integer::Make(std::string name) {
  return std::make_shared<Integer>(name);
}

Boolean::Boolean(std::string name) : Type(std::move(name), Type::BOOLEAN) {}

std::shared_ptr<Type> Boolean::Make(std::string name) {
  return std::make_shared<Boolean>(name);
}

String::String(std::string name) : Type(std::move(name), Type::STRING) {}

std::shared_ptr<Type> String::Make(std::string name) {
  return std::make_shared<String>(name);
}

Clock::Clock(std::string name, std::shared_ptr<ClockDomain> domain)
    : Type(std::move(name), Type::CLOCK), domain(std::move(domain)) {}

std::shared_ptr<Clock> Clock::Make(std::string name, std::shared_ptr<ClockDomain> domain) {
  return std::make_shared<Clock>(name, domain);
}

std::optional<std::shared_ptr<Node>> Clock::width() const {
  return std::dynamic_pointer_cast<Node>(intl<1>());
}

bool Clock::IsEqual(const Type *other) const {
  if (other->id() == Type::CLOCK) {
    auto oc = *Cast<Clock>(other);
    if (oc->domain == this->domain) {
      return true;
    }
  }
  return false;
}

Reset::Reset(std::string name, std::shared_ptr<ClockDomain> domain)
    : Type(std::move(name), Type::RESET), domain(std::move(domain)) {}

std::shared_ptr<Reset> Reset::Make(std::string name, std::shared_ptr<ClockDomain> domain) {
  return std::make_shared<Reset>(name, domain);
}

std::optional<std::shared_ptr<Node>> Reset::width() const {
  return std::dynamic_pointer_cast<Node>(intl<1>());
}

bool Reset::IsEqual(const Type *other) const {
  if (other->id() == Type::RESET) {
    auto oc = *Cast<Reset>(other);
    if (oc->domain == this->domain) {
      return true;
    }
  }
  return false;
}

Bit::Bit(std::string name) : Type(std::move(name), Type::BIT) {}

std::shared_ptr<Bit> Bit::Make(std::string name) {
  return std::make_shared<Bit>(name);
}

std::optional<std::shared_ptr<Node>> Bit::width() const {
  return std::dynamic_pointer_cast<Node>(intl<1>());
}

RecordField::RecordField(std::string name, std::shared_ptr<Type> type)
    : Named(std::move(name)), type_(std::move(type)) {}

std::shared_ptr<RecordField> RecordField::Make(std::string name, std::shared_ptr<Type> type) {
  return std::make_shared<RecordField>(name, type);
}

std::shared_ptr<RecordField> RecordField::Make(std::shared_ptr<Type> type) {
  return std::make_shared<RecordField>(type->name(), type);
}

std::shared_ptr<Type> Record::Make(const std::string &name, std::deque<std::shared_ptr<RecordField>> fields) {
  return std::make_shared<Record>(name, fields);
}

Record &Record::AddField(const std::shared_ptr<RecordField> &field) {
  fields_.push_back(field);
  return *this;
}

Record::Record(std::string name, std::deque<std::shared_ptr<RecordField>> fields)
    : Type(std::move(name), Type::RECORD), fields_(std::move(fields)) {}

bool Record::IsEqual(const Type *other) const {
  if (other == this) {
    return true;
  }
  // Must also be a record
  if (!other->Is(Type::RECORD)) {
    return false;
  }
  // Must have same number of fields
  auto orec = *Cast<Record>(other);
  if (orec->num_fields() != this->num_fields()) {
    return false;
  }
  // Each field must also be of equal type
  for (size_t i = 0; i < this->num_fields(); i++) {
    if (!this->field(i)->type()->IsEqual(orec->field(i)->type().get())) {
      return false;
    }
  }
  // If we didn't return already, the Record Types are the same
  return true;
}

ClockDomain::ClockDomain(std::string name) : Named(std::move(name)) {}

}  // namespace cerata
