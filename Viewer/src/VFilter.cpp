//============================================================================
// Copyright 2014 ECMWF.
// This software is licensed under the terms of the Apache Licence version 2.0
// which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
// In applying this licence, ECMWF does not waive the privileges and immunities
// granted to it by virtue of its status as an intergovernmental organisation
// nor does it submit to any jurisdiction.
//============================================================================

#include "VFilter.hpp"

#include "VNState.hpp"
#include "VAttribute.hpp"
#include "VIcon.hpp"
#include "VNode.hpp"
#include "VParam.hpp"
#include "VSettings.hpp"

#include "ServerHandler.hpp"

//==============================================
//
// VFilter
//
//==============================================

VParamSet::VParamSet()
{
}

void VParamSet::init(const std::vector<VParam*>& items)
{
	for(std::vector<VParam*>::const_iterator it=items.begin(); it != items.end(); ++it)
	{
		all_.insert((*it));
	}
}

bool VParamSet::isSet(VParam* p) const
{
	if(!p)
		return false;

	return (current_.find(p) != current_.end());
}

bool VParamSet::isSet(const std::string &name) const
{
	for(std::set<VParam*>::const_iterator it=current_.begin(); it != current_.end(); ++it)
	{
		if((*it)->strName() == name)
				return true;
	}
	return false;
}

void VParamSet::current(const std::set<std::string>& names)
{
	current_.clear();
	for(std::set<VParam*>::const_iterator it=all_.begin(); it != all_.end(); ++it)
	{
			if(names.find((*it)->strName()) != names.end())
				current_.insert(*it);
	}

	Q_EMIT changed();
}

void VParamSet::writeSettings(VSettings *vs)
{
	std::vector<std::string> array;

	for(std::set<VParam*>::const_iterator it=current_.begin(); it != current_.end(); ++it)
	{
		array.push_back((*it)->strName());
	}

	vs->put(settingsId_,array);
}

void VParamSet::readSettings(VSettings* vs)
{
	current_.clear();

	std::vector<std::string> array;
	vs->get(settingsId_,array);

	for(std::vector<std::string>::const_iterator it = array.begin(); it != array.end(); ++it)
	{
		std::string name=*it;
		for(std::set<VParam*>::const_iterator itA=all_.begin(); itA != all_.end(); ++itA)
		{
			if((*itA)->strName() == name)
					current_.insert(*itA);
		}
	}
}

//==============================================
//
// StateFilter
//
//==============================================

NodeStateFilter::NodeStateFilter() : VParamSet()
{
	settingsId_="state";
	std::vector<VParam*> v=VNState::filterItems();
	init(v);
	current_=all_;
}


//==============================================
//
// AttributeFilter
//
//==============================================

AttributeFilter::AttributeFilter() : VParamSet()
{
	settingsId_="attribute";
	std::vector<VParam*> v=VAttribute::filterItems();
	init(v);
}

//==============================================
//
// IconFilter
//
//==============================================

IconFilter::IconFilter() : VParamSet()
{
	settingsId_="icon";
	std::vector<VParam*> v=VIcon::filterItems();
	init(v);
	current_=all_;
}


NodeFilterDef::NodeFilterDef(Scope scope) : nodeState_(0)
{
	if(scope == NodeState)
		nodeState_=new NodeStateFilter;

	if(nodeState_)
	{
		connect(nodeState_,SIGNAL(changed()),
					this,SIGNAL(changed()));
	}
}

NodeFilter::NodeFilter(NodeFilterDef* def,ResultMode resultMode) :
		def_(def),
		resultMode_(resultMode),
		beingReset_(false)
{

}


TreeNodeFilter::TreeNodeFilter(NodeFilterDef* def) : NodeFilter(def,StoreNonMatched)
{

}

bool TreeNodeFilter::isNull()
{
	return def_->nodeState_->isComplete();
}

bool TreeNodeFilter::isFiltered(VNode* node)
{
	if(resultMode_==StoreNonMatched)
	{
		if(result_.empty())
			return true;

		return (std::find(result_.begin(), result_.end(), node) == result_.end());
	}
	else if(resultMode_==StoreMatched)
	{
		if(result_.empty())
			return false;

		return (std::find(result_.begin(), result_.end(), node) == result_.end());
	}

	return false;
}

bool TreeNodeFilter::update(const VNode *node)
{
	beginReset(node->server());
	endReset();
	return true;
}


void TreeNodeFilter::beginReset(ServerHandler* server)
{
	beingReset_=true;

	result_.clear();
	resultMode_=StoreNonMatched;

	//If all states are visible
	if(def_->nodeState_->isComplete())
	{
		return;
	}

	else if(def_->nodeState_->isEmpty())
	{
		resultMode_=StoreMatched;
		return;
	}

	VServer* root=server->vRoot();
	for(unsigned int j=0; j < root->numOfChildren();j++)
	{
		filterState(root->childAt(j),def_->nodeState_);
	}
}

void TreeNodeFilter::endReset()
{
	beingReset_=false;
}

bool TreeNodeFilter::filterState(VNode* node,VParamSet* stateFilter)
{
	bool ok=false;
	if(stateFilter->isSet(VNState::toState(node)))
	{
		ok=true;
	}

	for(unsigned int i=0; i < node->numOfChildren(); i++)
	{
		if(filterState(node->childAt(i),stateFilter) == true && ok == false)
		{
			ok=true;
		}
	}


	if(!ok)
		result_.insert(node);

	return ok;
}

int TreeNodeFilter::matchCount()
{
	if(beingReset_)
		return 0;

	if(resultMode_==StoreMatched)
	{
		return static_cast<int>(result_.size());
	}
	return 0;
};

int TreeNodeFilter::nonMatchCount()
{
	if(beingReset_)
			return 0;

	if(resultMode_==StoreNonMatched)
	{
		return static_cast<int>(result_.size());
	}
	return 0;
}

int TreeNodeFilter::realMatchCount()
{
	return 0;
}

VNode* TreeNodeFilter::realMatchAt(int)
{
	return NULL;
}



//===========================================================
// TableNodeFilter
//===========================================================

TableNodeFilter::TableNodeFilter(NodeFilterDef* def) : NodeFilter(def,StoreMatched)
{
	//type_.insert("suite");
}

bool TableNodeFilter::isNull()
{
	return def_->nodeState_->isComplete();
}

bool TableNodeFilter::isFiltered(VNode* node)
{
	if(beingReset_)
		return false;

	return (std::find(match_.begin(), match_.end(), node) != match_.end());
}

void TableNodeFilter::clear()
{
	match_.clear();
}

bool TableNodeFilter::update(const VNode *node)
{
	beginReset(node->server());
	endReset();
	return true;
}

void TableNodeFilter::beginReset(ServerHandler* server)
{
	beingReset_=true;

	match_.clear();

	VServer* s=server->vRoot();

	//TODO: implement the filter!!!
	s->collect(match_);
}

void TableNodeFilter::endReset()
{
	beingReset_=false;
}

VNode* TableNodeFilter::matchAt(int i)
{
	if(beingReset_)
		return NULL;

	if(match_.empty())
		return NULL;

	assert(i>=0 && i < match_.size());
	return match_.at(i);
}

int TableNodeFilter::matchPos(const VNode* node)
{
	if(beingReset_)
		return -1;

	std::vector<VNode*>::const_iterator it=std::find(match_.begin(), match_.end(), node);
	if(it != match_.end())
		return it-match_.begin();

	return -1;
}

int TableNodeFilter:: matchCount()
{
	if(beingReset_)
		return 0;

	return static_cast<int>(match_.size());
}

int TableNodeFilter::realMatchCount()
{
	return static_cast<int>(match_.size());
}

VNode* TableNodeFilter::realMatchAt(int i)
{
	if(match_.empty())
		return NULL;

	assert(i>=0 && i < match_.size());
	return match_.at(i);
}

