//============================================================================
// Copyright 2009-2017 ECMWF.
// This software is licensed under the terms of the Apache Licence version 2.0
// which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
// In applying this licence, ECMWF does not waive the privileges and immunities
// granted to it by virtue of its status as an intergovernmental organisation
// nor does it submit to any jurisdiction.
//
//============================================================================

#include <QCloseEvent>
#include <QDebug>
#include <QSettings>

#include "CommandOutputDialog.hpp"
#include "SessionHandler.hpp"
#include "VConfig.hpp"
#include "WidgetNameProvider.hpp"

CommandOutputDialog* CommandOutputDialog::dialog_=0;

CommandOutputDialog::CommandOutputDialog(QWidget *parent) :
    QDialog(parent)
{
    setupUi(this);

    setAttribute(Qt::WA_DeleteOnClose);

    QString wt=windowTitle();
    wt+="  -  " + QString::fromStdString(VConfig::instance()->appLongName());
    setWindowTitle(wt);

    //connect(queryWidget_,SIGNAL(closeClicked()),
    //		this,SLOT(accept()));

    //Read the qt settings
    readSettings();

    WidgetNameProvider::nameChildren(this);
}

CommandOutputDialog::~CommandOutputDialog()
{
}

void CommandOutputDialog::closeEvent(QCloseEvent * event)
{
    //queryWidget_->slotStop(); //The search thread might be running!!
    dialog_=0;
    event->accept();
    writeSettings();
}

void CommandOutputDialog::accept()
{
    dialog_=0;
    writeSettings();
    QDialog::accept();
}

void CommandOutputDialog::reject()
{
    dialog_=0;
    writeSettings();
    QDialog::reject();
}

#if 0
void CommandOutputDialog::slotOwnerDelete()
{
    deleteLater();
}
#endif

void CommandOutputDialog::make()
{
    if(!dialog_)
    {
        dialog_=new CommandOutputDialog(0);
        dialog_->show();
    }
}

void CommandOutputDialog::addText(ShellCommand* cmd,QString txt)
{
    make();
    Q_ASSERT(dialog_);
    if(dialog_->widget_->addText(cmd,txt))
       dialog_->raise();
}

void CommandOutputDialog::addErrorText(ShellCommand* cmd,QString txt)
{
    make();
    Q_ASSERT(dialog_);
    if(dialog_->widget_->addErrorText(cmd,txt))
        dialog_->raise();
}

//------------------------------------------
// Settings read/write
//------------------------------------------

void CommandOutputDialog::writeSettings()
{
    SessionItem* cs=SessionHandler::instance()->current();
    Q_ASSERT(cs);
    QSettings settings(QString::fromStdString(cs->qtSettingsFile("CommandOutputDialog")),
                       QSettings::NativeFormat);

    //We have to clear it so that should not remember all the previous values
    settings.clear();

    settings.beginGroup("main");
    settings.setValue("size",size());
   // queryWidget_->writeSettings(settings);
    settings.endGroup();
}

void CommandOutputDialog::readSettings()
{
    SessionItem* cs=SessionHandler::instance()->current();
    Q_ASSERT(cs);
    QSettings settings(QString::fromStdString(cs->qtSettingsFile("CommandOutputDialog")),
                       QSettings::NativeFormat);

    settings.beginGroup("main");
    if(settings.contains("size"))
    {
        resize(settings.value("size").toSize());
    }
    else
    {
        resize(QSize(550,540));
    }

   // queryWidget_->readSettings(settings);

    /*if(settings.contains("current"))
    {
        int current=settings.value("current").toInt();
        if(current >=0)
            list_->setCurrentRow(current);
    }*/
    settings.endGroup();
}