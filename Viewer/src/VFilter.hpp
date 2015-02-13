//============================================================================
// Copyright 2014 ECMWF.
// This software is licensed under the terms of the Apache Licence version 2.0
// which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
// In applying this licence, ECMWF does not waive the privileges and immunities
// granted to it by virtue of its status as an intergovernmental organisation
// nor does it submit to any jurisdiction.
//============================================================================

#ifndef VIEWFILTER_HPP_
#define VIEWFILTER_HPP_

#include <set>

#include <QObject>

#include "VParam.hpp"

#include "Node.hpp"

class ServerHandler;
class VSettings;

#include <boost/property_tree/ptree.hpp>

class VParamSet : public QObject
{
Q_OBJECT

public:
	VParamSet();
	virtual ~VParamSet() {};

	const std::set<VParam*>& current() const {return current_;}
	void current(const std::set<std::string>&);
	const std::set<VParam*>& all() const {return all_;}

	bool isEmpty() const {return current_.size() ==0 ;}
	bool isComplete() const { return all_.size() == current_.size();}
	bool isSet(const std::string&) const;
	bool isSet(VParam*) const;

	void writeSettings(VSettings* vs);
	void readSettings(VSettings* vs);

Q_SIGNALS:
	void changed();

protected:
	void init(const std::vector<VParam*>& items);

	std::set<VParam*> all_;
	std::set<VParam*> current_;
	std::string settingsId_;
};

class NodeStateFilter : public VParamSet
{
public:
	NodeStateFilter();
};

class AttributeFilter : public VParamSet
{
public:
	AttributeFilter();
};

class IconFilter : public VParamSet
{
public:
	IconFilter();
};

class TreeNodeFilter;
class  TableNodeFilter;

class NodeFilterDef : public QObject
{
Q_OBJECT

friend class  TreeNodeFilter;
friend class  TableNodeFilter;

public:
	enum Scope {NodeState};
	NodeFilterDef(Scope);
	NodeStateFilter* nodeState() const {return nodeState_;}

Q_SIGNALS:
	void changed();

protected:
	//NodeStateFilter *serverState_;
	NodeStateFilter *nodeState_;
	//AttributeFilter *attribute_;
	//std::string nodeType_;
	//std::string nodeName_;
};


class NodeFilter
{
public:
	NodeFilter(NodeFilterDef* def);
	virtual ~NodeFilter() {};

	enum ChangeAspect {AllChanged,StateChanged,AttributeChanged};

	virtual void reset(ServerHandler* server)=0;
    virtual bool isFiltered(Node* node)=0;
    virtual int  matchCount()=0;
    virtual int  nonMatchCount()=0;
    virtual Node* match(int i)=0;

protected:
    NodeFilterDef* def_;
    std::set<std::string> type_;

};

class TreeNodeFilter : public NodeFilter
{
public:
	TreeNodeFilter(NodeFilterDef* def);
	void reset(ServerHandler* server);
	bool isFiltered(Node* node);
	int  matchCount() {return -1;};
	int  nonMatchCount() {return static_cast<int>(nonMatch_.size());};
	Node* match(int i) {return NULL;}

private:
	bool filterState(node_ptr node,VParamSet* stateFilter);
	std::set<Node*> nonMatch_;
};

class TableNodeFilter : public NodeFilter
{
public:
	TableNodeFilter(NodeFilterDef* def);
	void reset(ServerHandler* server);
	bool isFiltered(Node* node);
	int  matchCount() {return static_cast<int>(match_.size());};
	int  nonMatchCount() {return -1;}
	Node* match(int i);

private:
	std::vector<Node*> match_;
};

#endif
