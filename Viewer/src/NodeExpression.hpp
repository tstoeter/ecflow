#ifndef NODEEXPRESSION_HPP_
#define NODEEXPRESSION_HPP_

//============================================================================
// Copyright 2015 ECMWF. 
// This software is licensed under the terms of the Apache Licence version 2.0 
// which can be obtained at http://www.apache.org/licenses/LICENSE-2.0. 
// In applying this licence, ECMWF does not waive the privileges and immunities 
// granted to it by virtue of its status as an intergovernmental organisation 
// nor does it submit to any jurisdiction. 
//============================================================================

#include "DState.hpp"

#include "VInfo.hpp"
#include "VItem.hpp"
#include "VNState.hpp"
#include "VSState.hpp"
#include "StringMatchMode.hpp"

class VItem;

// ----------------------
// Node condition classes
// ----------------------

class BaseNodeCondition;  // forward declaration

class NodeExpressionParser
{
public:
    enum NodeType {SERVER, SUITE, FAMILY, TASK, ALIAS, NODE, BAD};

    enum AttributeType {ATTRIBUTE, METER, EVENT, REPEAT, TRIGGER, LABEL, TIME, DATE,
                   LATE, LIMIT, LIMITER, VAR, GENVAR, BADATTRIBUTE};

    static NodeExpressionParser* instance();

    BaseNodeCondition *parseWholeExpression(std::string, bool caseSensitiveStringMatch=true);

    NodeType  nodeType(const std::string &name) const;
    const std::string& typeName(const NodeType&) const;
    AttributeType toAttrType(const std::string &name) const;
    const std::string& toAttrName(const AttributeType&) const;

protected:
    NodeExpressionParser();

    bool isUserLevel(const std::string &str) const;
    bool isNodeHasAttribute(const std::string &str) const;
    bool isNodeFlag(const std::string &str) const;
    bool isWhatToSearchIn(const std::string &str, bool &isAttribute) const;
    bool isAttribute(const std::string &str) const;

    BaseNodeCondition *parseExpression(bool caseSensitiveStringMatch);
    void setTokens(std::vector<std::string> &tokens) {tokens_ = tokens; i_ = tokens_.begin();}
    std::vector<BaseNodeCondition *> popLastNOperands(std::vector<BaseNodeCondition *> &inOperands, int n);

    static NodeExpressionParser* instance_;
    std::vector<std::string> tokens_;
    std::vector<std::string>::const_iterator i_;
    std::map<std::string,NodeType> nameToNodeType_;
    std::map<NodeType,std::string> nodeTypeToName_;
    std::map<std::string,AttributeType> nameToAttrType_;
    std::map<AttributeType,std::string> attrTypeToName_;
    std::string badTypeStr_;
    std::string badAttributeStr_;
};



// -----------------------------------------------------------------
// BaseNodeCondition
// The parent class for all node conditions.
// delayUnwinding: choose whether to unwind the function stack
// immediately after parsing this condition, or delay until we've
// reached the end of the current sub-expression. Set to true for
// loosely-coupled operators such as 'and', and set to false for
// others which need to consume their arguments immediately.
// -----------------------------------------------------------------

class BaseNodeCondition
{
public:
    BaseNodeCondition() {delayUnwinding_ = false;}
    virtual ~BaseNodeCondition() {}

    bool execute(VInfo_ptr nodeInfo);
    virtual bool execute(VItem* item)=0;

    virtual int  numOperands() {return 0;}
    virtual std::string print() = 0;
    virtual bool operand2IsArbitraryString() {return false;}

    void setOperands(std::vector<BaseNodeCondition *> ops) {operands_ = ops;}
    bool containsAttributeSearch();
    bool delayUnwinding() {return delayUnwinding_;}

protected:
    virtual bool searchInAttributes() {return false;}

    std::vector<BaseNodeCondition *> operands_;
    bool delayUnwinding_;
};

// -----------------------------------------------------------------

class AndNodeCondition : public BaseNodeCondition
{
public:
    AndNodeCondition() {delayUnwinding_ = true;}
    ~AndNodeCondition() {}

    bool execute(VItem* node);
    int  numOperands() {return 2;}
    std::string print() {return std::string("and") + "(" + operands_[0]->print() + "," + operands_[1]->print() + ")";}
};

// -----------------------------------------------------------------

class OrNodeCondition : public BaseNodeCondition
{
public:
    OrNodeCondition()  {delayUnwinding_ = true;}
    ~OrNodeCondition() {}

    bool execute(VItem* node);
    int  numOperands() {return 2;}
    std::string print() {return std::string("or") + "(" + operands_[0]->print() + "," + operands_[1]->print() + ")";}
};

// -----------------------------------------------------------------

class NotNodeCondition : public BaseNodeCondition
{
public:
    NotNodeCondition()  {}
    ~NotNodeCondition() {}

    bool execute(VItem* node);
    int  numOperands() {return 1;}
    std::string print() {return std::string("not") + "(" + operands_[0]->print() + ")";}
};



// --------------------------------
// String matching utitlity classes
// --------------------------------

// note that it would be ideal for the match() function to take references to strings
// for efficiency, but this is not always possible.

class StringMatchBase
{
public:
    StringMatchBase(bool caseSensitive)  {caseSensitive_ = caseSensitive;}
    virtual ~StringMatchBase() {}

    virtual bool match(std::string searchFor, std::string searchIn) = 0;

protected:
    bool caseSensitive_;
};

class StringMatchExact : public StringMatchBase
{
public:
    StringMatchExact(bool caseSensitive) : StringMatchBase(caseSensitive) {}
    ~StringMatchExact() {}

    bool match(std::string searchFor, std::string searchIn);
};

class StringMatchContains : public StringMatchBase
{
public:
    StringMatchContains(bool caseSensitive)  : StringMatchBase(caseSensitive) {}
    ~StringMatchContains() {}

    bool match(std::string searchFor, std::string searchIn);
};

class StringMatchWildcard : public StringMatchBase
{
public:
    StringMatchWildcard(bool caseSensitive)  : StringMatchBase(caseSensitive) {}
    ~StringMatchWildcard() {}

    bool match(std::string searchFor, std::string searchIn);
};

class StringMatchRegexp : public StringMatchBase
{
public:
    StringMatchRegexp(bool caseSensitive)  : StringMatchBase(caseSensitive) {}
    ~StringMatchRegexp() {}

    bool match(std::string searchFor, std::string searchIn);
};

// -----------------------------------------------------------------

// -------------------------
// String matching condition
// -------------------------

class StringMatchCondition : public BaseNodeCondition
{
public:
    StringMatchCondition(StringMatchMode::Mode matchMode, bool caseSensitive);
    ~StringMatchCondition() {if (matcher_) delete matcher_;}

    bool execute(VItem *node);
    int  numOperands() {return 2;}
    std::string print() {return operands_[0]->print() + " = " + operands_[1]->print();}
    bool operand2IsArbitraryString() {return true;}
private:
    StringMatchBase *matcher_;
};

// -----------------------------------------------------------------

// ---------------------------
// Basic true/false conditions
// ---------------------------

class TrueNodeCondition : public BaseNodeCondition
{
public:
    TrueNodeCondition()  {}
    ~TrueNodeCondition() {}

    bool execute(VItem*) {return true;}
    std::string print() {return std::string("true");}
};

class FalseNodeCondition : public BaseNodeCondition
{
public:
    FalseNodeCondition()  {}
    ~FalseNodeCondition() {}

    bool execute(VItem*) {return false;}
    std::string print() {return std::string("false");}
};

// -----------------------------------------------------------------

// -------------------
// Node type condition
// -------------------

class TypeNodeCondition : public BaseNodeCondition
{
public:
    explicit TypeNodeCondition(NodeExpressionParser::NodeType type) {type_ = type;}
    ~TypeNodeCondition() {}

    bool execute(VItem* node);
    std::string print() {return NodeExpressionParser::instance()->typeName(type_);}

private:
    NodeExpressionParser::NodeType type_;
};

// -----------------------------------------------------------------

// --------------------
// Node state condition
// --------------------

class StateNodeCondition : public BaseNodeCondition
{
public:
    explicit StateNodeCondition(QString stateName) {stateName_ = stateName;}
    ~StateNodeCondition() {}

    bool execute(VItem* node);
    std::string print() {return stateName_.toStdString();}

private:
    QString stateName_;
};

// -----------------------------------------------------------------

// --------------------
// User level condition
// --------------------

class UserLevelCondition : public BaseNodeCondition
{
public:
    explicit UserLevelCondition(QString userLevelName) {userLevelName_ = userLevelName;}
    ~UserLevelCondition() {}

    bool execute(VItem*);
    std::string print() {return userLevelName_.toStdString();}

private:
    QString userLevelName_;
};

// -----------------------------------------------------------------

// ------------------------
// Node attribute condition
// ------------------------

class NodeAttributeCondition : public BaseNodeCondition
{
public:
    explicit NodeAttributeCondition(QString nodeAttrName) {nodeAttrName_ = nodeAttrName;}
    ~NodeAttributeCondition() {}

    bool execute(VItem*);
    std::string print() {return nodeAttrName_.toStdString();}

private:
    QString nodeAttrName_;
};

// -----------------------------------------------------------------

// ------------------------
// Node attribute condition
// ------------------------

class NodeFlagCondition : public BaseNodeCondition
{
public:
    explicit NodeFlagCondition(QString nodeFlagName) {nodeFlagName_ = nodeFlagName;}
    ~NodeFlagCondition() {}

    bool execute(VItem*);
    std::string print() {return nodeFlagName_.toStdString();}

private:
    QString nodeFlagName_;
};
// -----------------------------------------------------------------

// -----------------
// Search conditions
// -----------------

class WhatToSearchInOperand : public BaseNodeCondition
{
public:
    explicit WhatToSearchInOperand(std::string what, bool &attr);
    ~WhatToSearchInOperand();

    std::string name() {return what_;}
    bool execute(VItem* node) {return false;} // not called
    std::string print() {return what_;}
    std::string what() {return what_;}

private:
    std::string what_;  // TODO XXX: optimise - we should store an enum here
    bool searchInAttributes_;

    void searchInAttributes(bool attr) {searchInAttributes_ = attr;}
    bool searchInAttributes() {return searchInAttributes_;}
};

// -----------------------------------------------------------------

class WhatToSearchForOperand : public BaseNodeCondition
{
public:
    explicit WhatToSearchForOperand(std::string what) {what_ = what;}
    ~WhatToSearchForOperand();

    std::string name() {return what_;}
    bool execute(VItem* node) {return false;} // not called
    std::string print() {return what_;}
    std::string what() {return what_;}

private:
    std::string what_;
};

// ------------------------
// Attribute condition
// ------------------------

class AttributeCondition : public BaseNodeCondition
{
public:
    explicit AttributeCondition(QString attrName) {attrName_ = attrName;}
    ~AttributeCondition() {}

    bool execute(VItem*);
    std::string print() {return attrName_.toStdString();}

private:
    QString attrName_;
};




#endif
