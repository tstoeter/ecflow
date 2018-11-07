//============================================================================
// Copyright 2009-2018 ECMWF.
// This software is licensed under the terms of the Apache Licence version 2.0
// which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
// In applying this licence, ECMWF does not waive the privileges and immunities
// granted to it by virtue of its status as an intergovernmental organisation
// nor does it submit to any jurisdiction.
//============================================================================

#include "TimelineItemWidget.hpp"

#include <QVBoxLayout>

#include "Node.hpp"

#include "InfoProvider.hpp"

#include "MessageLabel.hpp"
#include "ServerHandler.hpp"
#include "UiLog.hpp"
#include "VNode.hpp"
#include "VNState.hpp"
#include "VSettings.hpp"

#include "TimelineData.hpp"
#include "TimelineWidget.hpp"

TimelineItemWidget::TimelineItemWidget(QWidget *parent)
{
    QVBoxLayout* vb=new QVBoxLayout(this);
    vb->setContentsMargins(0,0,0,0);

    w_=new TimelineWidget(this);
    vb->addWidget(w_);

    //This tab is always visible whatever node is selected!!!
    //We keep the data unchanged unless a new server is selected
    keepServerDataOnLoad_=true;

    //We will KEEP the contents when the item (aka tab) becomes unselected
    //unselectedFlags_.clear();
}

TimelineItemWidget::~TimelineItemWidget()
{
    clearContents();
}

QWidget* TimelineItemWidget::realWidget()
{
    return this;
}

void TimelineItemWidget::reload(VInfo_ptr info)
{
    assert(active_);

    if(suspended_)
        return;

    clearContents();

    bool same=hasSameContents(info);

    //set the info. We do not need to observe the node!!!
    info_=info;

    if(!same)
        load();
}

void TimelineItemWidget::load()
{
    if(info_ && info_->server())
    {
        ServerHandler *sh=info_->server();
        Q_ASSERT(sh);
        QString logFile;
        if(VServer* vs=sh->vRoot())
        {
            logFile=QString::fromStdString(vs->findVariable("ECF_LOG",false));
            w_->load(QString::fromStdString(sh->name()),
                         QString::fromStdString(sh->host()),
                         QString::fromStdString(sh->port()),
                         logFile); //last 25 MB are read
        }
    }
}

void TimelineItemWidget::clearContents()
{
    w_->clear();
    InfoPanelItem::clear();
}

//We are independent of the server's state
void TimelineItemWidget::updateState(const FlagSet<ChangeFlag>& flags)
{
}

bool TimelineItemWidget::hasSameContents(VInfo_ptr info)
{
    if(info && info_ && info->server())
    {
        return info->server() == info_->server();
    }
    return false;
}

//We are independent of the server's state
void TimelineItemWidget::serverSyncFinished()
{
}

void TimelineItemWidget::writeSettings(VComboSettings* vs)
{
    vs->beginGroup("timeline");
    w_->writeSettings(vs);
    vs->endGroup();
}

void TimelineItemWidget::readSettings(VComboSettings* vs)
{
    vs->beginGroup("timeline");
    w_->readSettings(vs);
    vs->endGroup();
}

static InfoPanelItemMaker<TimelineItemWidget> maker1("timeline");