//============================================================================
// Copyright 2014 ECMWF.
// This software is licensed under the terms of the Apache Licence version 2.0
// which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
// In applying this licence, ECMWF does not waive the privileges and immunities
// granted to it by virtue of its status as an intergovernmental organisation
// nor does it submit to any jurisdiction.
//============================================================================

#include "OutputFileProvider.hpp"

#include "LogServer.hpp"
#include "OutputFileClient.hpp"
#include "VNode.hpp"
#include "VReply.hpp"
#include "ServerHandler.hpp"
#include "UserMessage.hpp"

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/algorithm/string/predicate.hpp>

OutputFileProvider::OutputFileProvider(InfoPresenter* owner) :
	InfoProvider(owner,VTask::OutputTask),
	outClient_(NULL)
{
}

void OutputFileProvider::clear()
{
	if(outClient_)
	{
		delete outClient_;
		outClient_=NULL;
	}
	InfoProvider::clear();
}

//Node
void OutputFileProvider::visit(VInfoNode* info)
{
	//Reset the reply
	reply_->reset();

	if(!info)
 	{
       	owner_->infoFailed(reply_);
   	}

	ServerHandler* server=info_->server();
	VNode *n=info->node();

    if(!n || !n->node())
   	{
       	owner_->infoFailed(reply_);
   	}

    //Get the filename
    std::string fileName=n->findVariable("ECF_JOBOUT",true);

    fetchFile(server,n,fileName,true);
}

//Get a file
void OutputFileProvider::file(const std::string& fileName)
{
	//Check if the task is already running
	if(task_)
	{
		task_->status(VTask::CANCELLED);
		task_.reset();
	}

	//Reset the reply
	reply_->reset();

	if(!info_->isNode() || !info_->node() || !info_->node()->node())
	{
	    owner_->infoFailed(reply_);
	}

	ServerHandler* server=info_->server();
	VNode *n=info_->node();

	//Get the filename
	std::string jobout=n->findVariable("ECF_JOBOUT",true);

	fetchFile(server,n,fileName,(fileName==jobout));
}

void OutputFileProvider::fetchFile(ServerHandler *server,VNode *n,const std::string& fileName,bool isJobout)
{
	if(!n || !n->node() || !server)
    {
    	owner_->infoFailed(reply_);
    	return;
    }

	//Set the filename in reply
	reply_->fileName(fileName);

	//No filename is available
	if(fileName.empty())
	{
		//Joubout variable is not defined or empty
		if(isJobout)
		{
			reply_->setErrorText("Variable ECF_JOBOUT is not defined!");
			owner_->infoFailed(reply_);
		}
		else
		{
			reply_->setErrorText("Output file is not defined!");
			owner_->infoFailed(reply_);
		}

		return;
	}

    //Check if it is tryno 0
    if(boost::algorithm::ends_with(fileName,".0"))
    {
    	reply_->setInfoText("Job output does not exist yet (TRYNO is 0!)");
    	owner_->infoReady(reply_);
    	return;
    }

    //----------------------------------
    // The host is the localhost
    //----------------------------------

    if(server->isLocalHost())
    {
    	//We try to read the file directly from the disk
    	if(!isJobout || server->readFromDisk())
    	{
    		if(fetchLocalFile(fileName))
    			return;
    	}
    }
    //----------------------------------------------------
    // Not the loacalhost or we could not read the file
    //----------------------------------------------------

    //We try the output client, its asynchronous!
    if(fetchFileViaOutputClient(n,fileName))
    {
    	//If we are here we created a output client and asked to the fetch the
    	//file asynchronously. The ouput client will call slotOutputClientFinished() or
    	//slotOutputClientError eventually!!
    	return;
    }

    //If there is no output client and it is not the localhost we try
    //to read it again from the disk!!!
    if(!server->isLocalHost())
    {
    	if(!isJobout || server->readFromDisk())
    	{
    		//Get the fileName
    		if(fetchLocalFile(fileName))
    	    	return;
    	}
    }

    //If we are here no output client is defined and we could not read the file from
    //the local disk.
    //We try the server if it is the jobout file
    if(isJobout)
    {
    	fetchJoboutViaServer(server,n,fileName);
        return;
    }

    //If we are we coud not get the file
    owner_->infoFailed(reply_);
}

bool OutputFileProvider::fetchFileViaOutputClient(VNode *n,const std::string& fileName)
{
	std::string host, port;
	if(n->logServer(host,port))
	{
		//host=host + "baaad";

		reply_->setInfoText("Getting file through log server: " + host + "@" + port);
		owner_->infoProgress(reply_);

		if(!outClient_)
		{
			outClient_=new OutputFileClient(host,port,this);

			connect(outClient_,SIGNAL(error(QString)),
				this,SLOT(slotOutputClientError(QString)));

			connect(outClient_,SIGNAL(progress(QString)),
				this,SLOT(slotOutputClientProgress(QString)));

			connect(outClient_,SIGNAL(finished()),
				this,SLOT(slotOutputClientFinished()));
		}

		outClient_->getFile(fileName);

		return true;
	}

	return false;
}

void OutputFileProvider::slotOutputClientFinished()
{
	VFile_ptr tmp = outClient_->result();

	if(tmp && tmp.get() && tmp->exists())
	{
		reply_->setInfoText("");
		reply_->fileReadMode(VReply::LogServerReadMode);

		std::string method="served by " + outClient_->host() + "@" + outClient_->portStr();
		reply_->fileReadMethod(method);

		reply_->tmpFile(tmp);
		owner_->infoReady(reply_);
	}
}

void OutputFileProvider::slotOutputClientProgress(QString msg)
{
	//reply_->setInfoText(msg.toStdString());
	//owner_->infoProgress(reply_);
	//reply_->setInfoText("");
	qDebug() << "prog: " << msg;
}

void OutputFileProvider::slotOutputClientError(QString msg)
{
	if(info_ && info_.get())
	{
		ServerHandler* server=info_->server();
		VNode *n=info_->node();

		if(server && n)
		{
			std::string jobout=n->findVariable("ECF_JOBOUT",true);
			if(outClient_->remoteFile() == jobout)
			{
				fetchJoboutViaServer(server,n,jobout);
				return;
			}
		}
	}

	reply_->setErrorText(msg.toStdString());
	owner_->infoFailed(reply_);
}

void OutputFileProvider::fetchJoboutViaServer(ServerHandler *server,VNode *n,const std::string& fileName)
{
    //Define a task for getting the info from the server.
    task_=VTask::create(taskType_,n,this);

    task_->reply()->fileReadMode(VReply::ServerReadMode);
    task_->reply()->fileName(fileName);

    //Run the task in the server. When it finish taskFinished() is called. The text returned
    //in the reply will be prepended to the string we generated above.
    server->run(task_);
}

bool OutputFileProvider::fetchLocalFile(const std::string& fileName)
{
	//we do not want to delete the file once the VFile object is destroyed!!
	VFile_ptr f(VFile::create(fileName,false));
	if(f->exists())
	{
		reply_->fileReadMode(VReply::LocalReadMode);
		reply_->tmpFile(f);
		owner_->infoReady(reply_);
		return true;
	}

	return false;
}

/*
VDir_ptr OutputProvider::fetchDirViaLogServer(VNode *n,const std::string& fileName)
{
	VDir_ptr res;

	//Create a logserver
	LogServer_ptr logServer=n->logServer();

	if(logServer && logServer->ok())
	{
		res=logServer->getDir(fileName.c_str());
	}

	return res;
}
*/

/*
VDir_ptr OutputProvider::fetchLocalDir(const std::string& path)
{
	VDir_ptr res;

	boost::filesystem::path p(path);

	try {
		//Is it a directory?
		if(boost::filesystem::is_directory(p))
		{
			return res;
		}
		//It must be a file
		if(boost::filesystem::exists(p) &&
		   boost::filesystem::exists(p.parent_path()))
		{
			std::string dirName=p.parent_path().string();
			std::string fileName=p.leaf().string();

			std::string::size_type pos=fileName.find_last_of(".");
			if(pos != std::string::npos)
			{
				std::string pattern=fileName.substr(0,pos);
				res=VDir_ptr(new VDir(dirName,pattern));
				return res;
			}
		}

	}
	catch (const boost::filesystem::filesystem_error& e)
	{
		UserMessage::message(UserMessage::WARN,false,"fetchLocalDir failed:" + std::string(e.what()));
		return res;
	}


	return res;
}

*/
std::string OutputFileProvider::joboutFileName() const
{
	if(info_ && info_->isNode() && info_->node() && info_->node()->node())
	{
		return info_->node()->findVariable("ECF_JOBOUT",true);
	}

	return std::string();
}



