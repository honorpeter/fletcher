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

#include <utility>
#include <optional>
#include <string>
#include <memory>
#include <deque>

#include "cerata/types.h"

namespace cerata {

// Forward declarations.
struct Edge;
struct Graph;

/**
 * @brief A node.
 */
class Node : public Named, public std::enable_shared_from_this<Node> {
 public:
  /// Node type IDs with different properties.
  enum ID {
    LITERAL,         ///< No-input     AND multi-output node with storage type and storage value.
    EXPRESSION,      ///< No-input     AND multi-output node that forms a binary tree with operations and nodes.
    SIGNAL,          ///< Single-input AND multi-output node.
    PORT,            ///< Single-input AND multi-output node with direction.
    PARAMETER,       ///< Single-input AND multi-output node with default value.
    ARRAY_SIGNAL,    ///< Multi-input  XOR multi-output node with count node.
    ARRAY_PORT,      ///< Multi-input  XOR multi-output node with count node and direction.
  };

  /// @brief Node constructor.
  Node(std::string name, ID id, std::shared_ptr<Type> type);
  /// @brief Virtual destructor for Node.
  virtual ~Node() = default;

  /// @brief Get a copy of this Node.
  virtual std::shared_ptr<Node> Copy() const = 0;

  /// @brief Return the node Type
  inline std::shared_ptr<Type> type() const { return type_; }
  /// @brief Set the node Type
  inline void SetType(const std::shared_ptr<Type> &type) { type_ = type; }
  /// @brief Return the node type ID
  inline ID id() const { return id_; }
  /// @brief Return whether this node is of a specific node type id.
  inline bool Is(ID node_id) const { return id_ == node_id; }
  /// @brief Return true if this is a PORT node, false otherwise.
  inline bool IsPort() const { return id_ == PORT; }
  /// @brief Return true if this is a SIGNAL node, false otherwise.
  inline bool IsSignal() const { return id_ == SIGNAL; }
  /// @brief Return true if this is a PARAMETER node, false otherwise.
  inline bool IsParameter() const { return id_ == PARAMETER; }
  /// @brief Return true if this is a LITERAL node, false otherwise.
  inline bool IsLiteral() const { return id_ == LITERAL; }
  /// @brief Return true if this is an EXPRESSION node, false otherwise.
  inline bool IsExpression() const { return id_ == EXPRESSION; }
  /// @brief Return true if this is some type of ARRAY node, false otherwise.
  inline bool IsArray() const { return (id_ == ARRAY_PORT) || (id_ == ARRAY_SIGNAL); }
  /// @brief Return true if this is an ARRAY_PORT node, false otherwise.
  inline bool IsArrayPort() const { return id_ == ARRAY_PORT; }
  /// @brief Return true if this is an ARRAY_SIGNAL node, false otherwise.
  inline bool IsArraySignal() const { return id_ == ARRAY_SIGNAL; }

  /// @brief Get the input edges of this Node.
  virtual std::deque<std::shared_ptr<Edge>> inputs() const { return {}; }
  /// @brief Get the output edges of this Node.
  virtual std::deque<std::shared_ptr<Edge>> outputs() const { return {}; }
  /// @brief Add an input to this node.
  virtual void AddInput(const std::shared_ptr<Edge> &input) = 0;
  /// @brief Add an output to this node.
  virtual void AddOutput(const std::shared_ptr<Edge> &output) = 0;
  /// @brief Remove an edge of this node.
  virtual bool RemoveEdge(const std::shared_ptr<Edge> &edge) { return false; }

  /// @brief Set this node's parent
  void SetParent(const Graph *parent);
  inline std::optional<const Graph *> parent() { return parent_; }

  /// @brief Return a human-readable string
  virtual std::string ToString();

 protected:
  /// Node type ID.
  ID id_;
  /// The Type of this Node.
  std::shared_ptr<Type> type_;
  /// An optional parent Graph to which this Node belongs. Initially no value.
  std::optional<const Graph *> parent_ = {};
};

/**
 * @brief A MultiOutputNode is a Node that can drive multiple outputs.
 */
struct MultiOutputNode : public Node {
  /// @brief The outgoing Edges that sink this Node.
  std::deque<std::shared_ptr<Edge>> outputs_;

  /// @brief MultiOutputNode constructor.
  MultiOutputNode(std::string name, Node::ID id, std::shared_ptr<Type> type) : Node(std::move(name),
                                                                                    id,
                                                                                    std::move(type)) {}

  /// @brief Return the incoming edges (in this case just the single input edge).
  std::deque<std::shared_ptr<Edge>> inputs() const override { return {}; }
  /// @brief The outgoing Edges that sink this Node.
  inline std::deque<std::shared_ptr<Edge>> outputs() const override { return outputs_; }

  /// @brief Add an output edge to this node.
  void AddOutput(const std::shared_ptr<Edge> &edge) override;
  /// @brief Remove an edge from this node.
  bool RemoveEdge(const std::shared_ptr<Edge> &edge) override;
  /// @brief Return output edge i of this node.
  inline std::shared_ptr<Edge> output(size_t i) const { return outputs_[i]; }
  /// @brief Return the number of edges of this node.
  inline size_t num_outputs() const { return outputs_.size(); }
};

/**
 * @brief A NormalNode is a single-input, multiple-outputs node
 */
struct NormalNode : public MultiOutputNode {
  /// @brief The incoming Edge that sources this Node.
  std::shared_ptr<Edge> input_;

  /// @brief NormalNode constructor.
  NormalNode(std::string name, Node::ID id, std::shared_ptr<Type> type) : MultiOutputNode(std::move(name),
                                                                                          id,
                                                                                          std::move(type)) {}

  /// @brief Return the incoming edges (in this case just the single input edge).
  std::deque<std::shared_ptr<Edge>> inputs() const override;

  /// @brief Return the single incoming edge.
  std::optional<std::shared_ptr<Edge>> input() const;

  /// @brief Set the input edge of this node.
  void AddInput(const std::shared_ptr<Edge> &edge) override;

  /// @brief Remove an edge from this node.
  bool RemoveEdge(const std::shared_ptr<Edge> &edge) override;
};

/**
 * @brief A Literal Node
 *
 * A literal node can be used to store some literal value. A literal node can, for example, be used for Vector Type
 * widths or it can be connected to a Parameter Node, to give the Parameter its value.
 */
struct Literal : public MultiOutputNode {
  /// The storage type of the literal value.
  enum StorageType { INT, STRING, BOOL } storage_type_;

  /// The string storage.
  std::string str_val_ = "";

  /// The integer storage.
  int int_val_ = 0;

  /// The boolean storage.
  bool bool_val_ = false;

  /// @brief Literal constructor.
  Literal(std::string name,
          const std::shared_ptr<Type> &type,
          StorageType st,
          std::string str_val,
          int int_val,
          bool bool_val)
      : MultiOutputNode(std::move(name), Node::LITERAL, type),
        storage_type_(st),
        str_val_(std::move(str_val)),
        int_val_(int_val) {}

  /// @brief Construct a new Literal with a string storage type.
  Literal(std::string name, const std::shared_ptr<Type> &type, std::string value);
  /// @brief Construct a new Literal with an integer storage type.
  Literal(std::string name, const std::shared_ptr<Type> &type, int value);
  /// @brief Construct a new Literal with a boolean storage type.
  Literal(std::string name, const std::shared_ptr<Type> &type, bool value);

  /// @brief Get a smart pointer to a new Literal with string storage, where the Literal name will be the string.
  static std::shared_ptr<Literal> Make(int value);
  /// @brief Get a smart pointer to a new Literal with string storage, where the Literal name will be the string.
  static std::shared_ptr<Literal> Make(const std::shared_ptr<Type> &type, std::string value);
  /// @brief Get a smart pointer to a new Literal with a string storage type.
  static std::shared_ptr<Literal> Make(std::string name, const std::shared_ptr<Type> &type, std::string value);
  /// @brief Get a smart pointer to a new Literal with an integer storage type.
  static std::shared_ptr<Literal> Make(std::string name, const std::shared_ptr<Type> &type, int value);
  /// @brief Get a smart pointer to a new Literal with a boolean storage type.
  static std::shared_ptr<Literal> Make(std::string name, const std::shared_ptr<Type> &type, bool value);

  /// @brief Create a copy of this Literal.
  std::shared_ptr<Node> Copy() const override;
  /// @brief Add an input to this node.
  void AddInput(const std::shared_ptr<Edge> &input) override;

  /// @brief A literal node has no inputs. This function returns an empty list.
  inline std::deque<std::shared_ptr<Edge>> inputs() const override { return {}; }
  /// @brief Get the output edges of this Node.
  inline std::deque<std::shared_ptr<Edge>> outputs() const override { return outputs_; }

  /// @brief Convert the Literal value to a human-readable string.
  std::string ToString() override;
};

/**
 * @brief A node representing a binary tree of other nodes
 */
struct Expression : public MultiOutputNode {
  enum Operation { ADD, SUB, MUL, DIV } operation;
  std::shared_ptr<Node> lhs;
  std::shared_ptr<Node> rhs;
  /**
   * @brief Construct a new expression
   * @param op  The operator between two operands
   * @param lhs The left operand
   * @param rhs The right operand
   */
  Expression(Operation op, const std::shared_ptr<Node> &lhs, const std::shared_ptr<Node> &rhs);

  /// @brief Short-hand to create a smart pointer to an expression.
  static std::shared_ptr<Expression> Make(Operation op,
                                          const std::shared_ptr<Node> &lhs,
                                          const std::shared_ptr<Node> &rhs);

  /// @brief Add an input to this node.
  void AddInput(const std::shared_ptr<Edge> &input) override;

  /// @brief Minimize a node, if it is an expression
  static std::shared_ptr<Node> Minimize(const std::shared_ptr<Node> &node);

  /// @brief Copy this expression.
  std::shared_ptr<Node> Copy() const override;

  /// @brief Minimize the expression and convert it to a human-readable string.
  std::string ToString() override;
};

std::string ToString(Expression::Operation operation);

/// @brief Create an expressions, adding two nodes.
std::shared_ptr<Expression> operator+(const std::shared_ptr<Node> &lhs, const std::shared_ptr<Node> &rhs);
/// @brief Increment a node with an integer. If the lhs is a literal, return a new literal, otherwise and expression.
std::shared_ptr<Node> operator+(const std::shared_ptr<Node> &lhs, int rhs);
/// @brief Decrement a node with an integer. If the lhs is a literal, return a new literal, otherwise and expression.
std::shared_ptr<Node> operator-(const std::shared_ptr<Node> &lhs, int rhs);

std::shared_ptr<Expression> operator-(const std::shared_ptr<Node> &lhs, const std::shared_ptr<Node> &rhs);
std::shared_ptr<Expression> operator*(const std::shared_ptr<Node> &lhs, const std::shared_ptr<Node> &rhs);
std::shared_ptr<Expression> operator/(const std::shared_ptr<Node> &lhs, const std::shared_ptr<Node> &rhs);

/**
 * @brief A Signal Node.
 *
 * A Signal Node can have a single input and multiple outputs.
 */
struct Signal : public NormalNode {
  /// @brief Signal constructor.
  Signal(std::string name, std::shared_ptr<Type> type);
  /// @brief Create a new Signal and return a smart pointer to it.
  static std::shared_ptr<Signal> Make(std::string name, const std::shared_ptr<Type> &type);
  /// @brief Create a new Signal and return a smart pointer to it. The Signal name is derived from the Type name.
  static std::shared_ptr<Signal> Make(const std::shared_ptr<Type> &type);
  /// @brief Create a copy of this Signal.
  std::shared_ptr<Node> Copy() const override;
};

/**
 * @brief A Parameter node.
 *
 * Can be used to define implementation-specific characteristics of a Graph, or can be connected to e.g. Vector widths.
 */
struct Parameter : public NormalNode {
  /// @brief An optional default value.
  std::optional<std::shared_ptr<Literal>> default_value;

  /// @brief Construct a new Parameter, optionally defining a default value Literal.
  Parameter(std::string name,
            const std::shared_ptr<Type> &type,
            std::optional<std::shared_ptr<Literal>> default_value = {});

  /// @brief Get a smart pointer to a new Parameter, optionally defining a default value Literal.
  static std::shared_ptr<Parameter> Make(std::string name,
                                         std::shared_ptr<Type> type,
                                         std::optional<std::shared_ptr<Literal>> default_value = {});

  /// @brief Create a copy of this Parameter.
  std::shared_ptr<Node> Copy() const override;

  /// @brief Short hand to get value node.
  std::optional<std::shared_ptr<Node>> value() const;
};

/**
 * @brief A terminator structure to enable terminator sanity checks.
 */
class Term {
 public:
  /// Terminator direction.
  enum Dir { NONE, IN, OUT };

  /// @brief Return the direction of this terminator.
  inline Dir dir() const { return dir_; };

  /// @brief Construct a new Term.
  explicit Term(Dir dir) : dir_(dir) {}

  /// @brief Return true if this Term is an input, false otherwise.
  inline bool IsInput() { return dir_ == IN; }
  /// @brief Return true if this Term is an output, false otherwise.
  inline bool IsOutput() { return dir_ == OUT; }

 private:
  Dir dir_;
};

/**
 * @brief A port is a terminator node on a graph
 */
struct Port : public NormalNode, public Term {
  Port(std::string name, std::shared_ptr<Type> type, Term::Dir dir);
  static std::shared_ptr<Port> Make(std::string name, std::shared_ptr<Type> type, Term::Dir dir = Term::IN);
  static std::shared_ptr<Port> Make(std::shared_ptr<Type> type, Term::Dir dir = Term::IN);
  std::shared_ptr<Node> Copy() const override;
};

/**
 * @brief A node where either inputs or outputs form an array of a specific type.
 *
 * The flattened type of all connected nodes must be strongly equal, i.e. the widths of the flattened subtypes are all
 * the same.
 *
 * Example: If there is a connection from ArrayNode "a" to both node "b" and "c", then "b" and "c" are said to be
 *          concatenated on the output of ArrayNode "a". The concatenated side of "a" is its output.
 *
 *          Here, a, b and c must all have exactly the same flattened type list.
 *
 * The number of edges that is concatenated onto an ArrayNode is related through another node via a special edge size()
 * If size_ is an integer literal, it will automatically be incremented when nodes are added.
 * Otherwise, if the Node is a Parameter node being sourced by an integer literal, it can also be automatically be
 * incremented.
 */
class ArrayNode : public Node {
 public:
  /// Which side of the node the "array" is on.
  enum ArraySide {
    ARRAY_OUT,  ///< The array is on the side of the outgoing edges.
    ARRAY_IN    ///< The array is on the side of the incoming edges.
  };

  /// @brief ArrayNode constructor.
  ArrayNode(std::string name, Node::ID id, std::shared_ptr<Type> type, ArraySide array_side);
  /// @brief Add an input edge to this node.
  void AddInput(const std::shared_ptr<Edge> &edge) override;
  /// @brief Add an output edge to this node.
  void AddOutput(const std::shared_ptr<Edge> &edge) override;

  /// @brief Increment the size of the ArrayNode.
  void increment();
  /// @brief Set the array size of this ArrayNode. The size Node must appear as a node on the parent Graph.
  std::shared_ptr<Edge> SetSize(std::shared_ptr<Node> size);
  /// @brief Return the Node representing the number of edges on the array side of this ArrayNode.
  std::shared_ptr<Node> size() const;
  /// @brief Return the inputs to this node.
  std::deque<std::shared_ptr<Edge>> inputs() const override;
  /// @brief Return the outputs of this node.
  std::deque<std::shared_ptr<Edge>> outputs() const override;
  /// @brief Return the index of an edge on this node.
  size_t IndexOf(const std::shared_ptr<Edge> &edge);

 private:
  /// @brief Append another Node to this ArrayNode, returns an Edge between this ArrayNode and the appended Node.
  std::shared_ptr<Edge> Append(const std::shared_ptr<Edge> &n);
  /// Which side is the "array" side
  ArraySide array_side_;
  /// A node representing the number of concatenated edges.
  std::shared_ptr<Edge> size_;
  /// The concatenated side.
  std::shared_ptr<Edge> single_edge_;
  /// The arrayed side.
  std::deque<std::shared_ptr<Edge>> array_edges_;
};

/**
 * @brief A port node that has an array of multiple edges to other nodes.
 */
struct ArrayPort : public ArrayNode, public Term {
  /// @brief Construct a new ArrayPort.
  ArrayPort(std::string name, std::shared_ptr<Type> type, Port::Dir dir);

  /// @brief Get a smart pointer to a new ArrayPort.
  static std::shared_ptr<ArrayPort> Make(std::string name,
                                         std::shared_ptr<Type> type,
                                         std::shared_ptr<Node> size,
                                         Port::Dir dir = Port::Dir::IN);
  /// @brief Get a smart pointer to a new ArrayPort. The ArrayPort name is derived from the Type name.
  static std::shared_ptr<ArrayPort> Make(std::shared_ptr<Type> type,
                                         std::shared_ptr<Node> size,
                                         Port::Dir dir = Port::Dir::IN);

  /// @brief Create a copy of this ArrayPort.
  std::shared_ptr<Node> Copy() const override;
};

/**
 * @brief Cast a Node to some (typically) less generic Node type T.
 * @tparam T    The new Node type.
 * @param obj   The Node to cast.
 * @return      Optionally, the Node casted to T, if successful.
 */
template<typename T>
std::optional<std::shared_ptr<T>> Cast(const std::shared_ptr<Node> &obj) {
  auto result = std::dynamic_pointer_cast<T>(obj);
  if (result != nullptr) {
    return result;
  } else {
    return {};
  }
}

/**
 * @brief Cast a Node to some (typically) less generic Node type T.
 * @tparam T    The new Node type.
 * @param obj   The Node to cast.
 * @return      Optionally, the Node casted to T, if successful.
 */
template<typename T>
std::optional<T *> Cast(Node *obj) {
  auto result = std::dynamic_pointer_cast<T>(obj);
  if (result != nullptr) {
    return result;
  } else {
    return {};
  }
}

/// @brief Convert a Node ID to a human-readable string.
std::string ToString(Node::ID id);

// Some often used literals for convenience:
/**
 * @brief Create an integer Literal.
 * @tparam V    The integer value.
 * @return      A smart pointer to a literal node representing the value.
 */
template<int V>
std::shared_ptr<Literal> intl() {
  static std::shared_ptr<Literal> result = std::make_shared<Literal>("int" + std::to_string(V), integer(), V);
  return result;
}

/**
 * @brief Create a string literal.
 * @param str   The string value.
 * @return      A smart pointer to a literal node representing the string.
 */
std::shared_ptr<Literal> strl(std::string str);

/// @brief Return a literal node representing a Boolean true.
std::shared_ptr<Literal> bool_true();

/// @brief Return a literal node representing a Boolean false.
std::shared_ptr<Literal> bool_false();

}  // namespace cerata
