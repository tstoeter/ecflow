//============================================================================
// Copyright 2014 ECMWF.
// This software is licensed under the terms of the Apache Licence version 2.0
// which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
// In applying this licence, ECMWF does not waive the privileges and immunities
// granted to it by virtue of its status as an intergovernmental organisation
// nor does it submit to any jurisdiction.
//============================================================================

#include "VariableModel.hpp"

#include <QColor>
#include <QDebug>

#include "ServerHandler.hpp"
#include "VariableModelData.hpp"

QColor VariableModel::varCol_=QColor(Qt::black);
QColor VariableModel::genVarCol_=QColor(34,51,136);
QColor VariableModel::blockBgCol_=QColor(122,122,122);
QColor VariableModel::blockFgCol_=QColor(255,255,255);

QStringList VariableModel::readOnlyGenVars_;

//=======================================================================
//
// VariabletModel
//
//=======================================================================

VariableModel::VariableModel(VariableModelDataHandler* data,QObject *parent) :
          QAbstractItemModel(parent),
          data_(data)
{
	connect(data_,SIGNAL(reloadBegin()),
			this,SLOT(slotReloadBegin()));

	connect(data_,SIGNAL(reloadEnd()),
					this,SLOT(slotReloadEnd()));

	connect(data_,SIGNAL(addRemoveBegin(int,int)),
				this,SLOT(slotAddRemoveBegin(int,int)));

	connect(data_,SIGNAL(addRemoveEnd(int)),
						this,SLOT(slotAddRemoveEnd(int)));

	connect(data_,SIGNAL(dataChanged(int)),
						this,SLOT(slotDataChanged(int)));

	if(readOnlyGenVars_.isEmpty())
	{
		readOnlyGenVars_ << "ECF_NODE" << "ECF_PORT" << "ECF_PID" << "ECF_VERSION" <<
				"ECF_LISTS";
	}

}

bool VariableModel::hasData() const
{
	return (data_->count()  > 0);
}

int VariableModel::columnCount( const QModelIndex& /*parent */ ) const
{
   	 return 2;
}

int VariableModel::rowCount( const QModelIndex& parent) const
{

	//Parent is the root: the item must be a node or a server
	if(!parent.isValid())
	{
		return data_->count();
	}
	//The parent is a server or a node
	else if(!isVariable(parent))
	{
		int row=parent.row();
		return data_->varNum(row);
	}

	//parent is a variable
	return 0;
}

Qt::ItemFlags VariableModel::flags ( const QModelIndex & index) const
{
	Qt::ItemFlags defaultFlags;

	defaultFlags=Qt::ItemIsEnabled | Qt::ItemIsSelectable;

	return defaultFlags;
}

QVariant VariableModel::data( const QModelIndex& index, int role ) const
{
	if( !index.isValid())
    {
		return QVariant();
	}

	//Data lookup can be costly so we immediately return a default value for all
	//the cases where the default should be used.
	if(role != Qt::DisplayRole && role != Qt::BackgroundRole && role != Qt::ForegroundRole &&
	   role != ReadOnlyRole)
	{
		return QVariant();
	}

	int row=index.row();
	int level=indexToLevel(index);

	//Server or node
	if(level == 1)
	{
		if(role == ReadOnlyRole)
			return QVariant();

		if(role == Qt:: BackgroundRole)
            return blockBgCol_;
        
		else if(role == Qt::ForegroundRole)
            return blockFgCol_;
        

        VariableModelData *d=data_->data(row);
		if(!d)
		{
			return QVariant();
		}

		if(index.column() == 0)
		{
			if(role == Qt::DisplayRole)
			{
				return QString::fromStdString(d->name());
			}
		}

		return QVariant();
	}

	//Variables
	else if (level == 2)
	{
        VariableModelData *d=data_->data(index.parent().row());
		if(!d)
		{
			return QVariant();
		}

		if(role == Qt::ForegroundRole)
		{
			//Generated variable
			if(d->isGenVar(row))
				return genVarCol_;
			else
				return varCol_;
		}
		else if(role == Qt::DisplayRole)
        {    
		    if(index.column() == 0)
		    {
			    return QString::fromStdString(d->name(row));
		    }
		    else if(index.column() == 1)
		    {
			    return QString::fromStdString(d->value(row));
		    }
        }

		else if(role == ReadOnlyRole)
        {
			return (readOnlyGenVars_.contains(QString::fromStdString(d->name(row))))?true:false;
        }

		return QVariant();
	}

	return QVariant();
}

bool VariableModel::variable(const QModelIndex& idx, QString& name,QString& value,bool& genVar) const
{
	int block=-1;
	int row=-1;

	identify(idx,block,row);

	if(row < 0)
		return false;

	if(block >=0 && block < data_->count())
	{
		name=QString::fromStdString(data_->data(block)->name(row));
		value=QString::fromStdString(data_->data(block)->value(row));
		genVar=data_->data(block)->isGenVar(row);
		return true;
	}

	return false;
}

bool VariableModel::setVariable(const QModelIndex& index, QString name,QString value)
{
    int block;
    int row;
    
    identify(index,block,row);
    
    if(block == -1 || row == -1)
        return false;
    
    if(block >=0 && block < data_->count())
    {
        //double check
        if(data_->data(block)->name(row) != name.toStdString())
        {
            assert(0);
            return false;
        }    
                
        //This will call the ServerComThread  so we 
        //do not know if it was succesful. The model will be
        //updated through the observer when the value will actually
        //change.
        data_->data(block)->setValue(row,value.toStdString());
    }
    
    return false;
}

bool VariableModel::removeVariable(const QModelIndex& index, QString name,QString value)
{
    int block;
    int row;

    identify(index,block,row);

    if(block == -1 || row == -1)
        return false;

    if(block >=0 && block < data_->count())
    {
        //double check
        if(data_->data(block)->name(row) != name.toStdString())
        {
            assert(0);
            return false;
        }

        //This will call the ServerComThread  so we
        //do not know if it was succesful. The model will be
        //updated through the observer when the value will actually
        //change.
        data_->data(block)->remove(row,name.toStdString());
    }

    return false;
}

QVariant VariableModel::headerData( const int section, const Qt::Orientation orient , const int role ) const
{
	if ( orient != Qt::Horizontal || role != Qt::DisplayRole )
      		  return QAbstractItemModel::headerData( section, orient, role );

   	switch ( section )
	{
   	case 0: return tr("Name");
   	case 1: return tr("Value");
   	default: return QVariant();
   	}

    return QVariant();
}

QModelIndex VariableModel::index( int row, int column, const QModelIndex & parent ) const
{
	if(!hasData() || row < 0 || column < 0)
	{
		return QModelIndex();
	}

	//When parent is the root this index refers to a node or server
	if(!parent.isValid())
	{
		return createIndex(row,column,0);
	}

	//We are under one of the nodes
	else
	{
		return createIndex(row,column,(parent.row()+1)*1000);
	}

	return QModelIndex();

}

QModelIndex VariableModel::parent(const QModelIndex &child) const
{
	if(!child.isValid())
		return QModelIndex();

	int level=indexToLevel(child);
	if(level == 1)
			return QModelIndex();
	else if(level == 2)
	{
		int id=child.internalId();
		int r=id/1000-1;
		return createIndex(r,child.column(),0);
	}

	return QModelIndex();
}

//----------------------------------------------
//
// Server to index mapping and lookup
//
//----------------------------------------------

int VariableModel::indexToLevel(const QModelIndex& index) const
{
	if(!index.isValid())
		return 0;

	int id=index.internalId();
	if(id >=0 && id < 1000)
	{
			return 1;
	}
	return 2;
}

VariableModelData* VariableModel::indexToData(const QModelIndex& index) const
{
	int block;
	int row;

	identify(index,block,row);

	if(block != -1)
		return data_->data(block);

	return NULL;
}

//----------------------------------------------
//
// Server to index mapping and lookup
//
//----------------------------------------------

bool VariableModel::isVariable(const QModelIndex & index) const
{
	return (indexToLevel(index) == 2);
}

void VariableModel::identify(const QModelIndex& index,int& block,int& row) const
{
    block=-1;
    row=-1;
    
    if(!index.isValid())
    {
       return;
    }
    
    int level=indexToLevel(index);
    
    if(level == 1)
    {
        block=index.row();
        row=-1;
    }
    else if(level == 2)
    {
        block=parent(index).row();
        row=index.row();       
    }        
}    

void VariableModel::slotReloadBegin()
{
	//Reset the whole model
	beginResetModel();
}

void VariableModel::slotReloadEnd()
{
	endResetModel();
}

void VariableModel::slotAddRemoveBegin(int block,int diff)
{
	QModelIndex parent=index(block,0);
	if(!parent.isValid())
		return;

	int num=rowCount(parent);

	//Insertion
	if(diff > 0)
	{
		//We add extra rows to the end
		beginInsertRows(parent,num,num+diff-1);
	}
	//Deletion
	else if(diff <0)
	{
		//We remove rows from the end
		beginRemoveRows(parent,num+diff,num-1);
	}
}

void VariableModel::slotAddRemoveEnd(int diff)
{
	//Insertion
	if(diff > 0)
	{
		endInsertRows();
	}
	//Deletion
	else if(diff <0)
	{
		endRemoveRows();
	}
}

void VariableModel::slotDataChanged(int block)
{
	QModelIndex blockIndex0=index(block,0);
	QModelIndex blockIndex1=index(block,1);
	Q_EMIT dataChanged(blockIndex0,blockIndex1);
}

//=======================================================================
//
// VariableSortModel
//
//=======================================================================

VariableSortModel::VariableSortModel(VariableModel *varModel,QObject* parent) :
	QSortFilterProxyModel(parent),
	varModel_(varModel),
	matchMode_(FilterMode),
	ignoreDuplicateNames_(false)
{
	QSortFilterProxyModel::setSourceModel(varModel_);
	setDynamicSortFilter(true);
}


void VariableSortModel::setMatchMode(MatchMode mode)
{
	if(matchMode_ == mode)
		return;

	matchMode_=mode;

	matchLst_.clear();
	matchText_.clear();

	//reload the filter model
	invalidate();
}

void VariableSortModel::setMatchText(QString txt)
{
	matchText_=txt;

	if(matchMode_ == FilterMode)
	{
		//reload the filter model
		invalidate();
	}
}


bool VariableSortModel::lessThan(const QModelIndex &sourceLeft, const QModelIndex &sourceRight) const
{
	//Node or server. Here we want the nodes and server to stay unsorted. That is the order should stay as
	//it is defined in the data handler: the selected node stays on top and its ancestors and the server
	//follow each other downwards. This order is reflected in the row index of these items in
	//the varModel: the selected node's row is 0, its parent's row is 1, etc.
	if(!varModel_->isVariable(sourceLeft))
	{
		if(sortOrder() == Qt::AscendingOrder)
			return (sourceLeft.row() < sourceRight.row());
		else
			return (sourceLeft.row() > sourceRight.row());
	}
	//For variables we simply sort according to the string
	else
	{
		return varModel_->data(sourceLeft,Qt::DisplayRole).toString() < varModel_->data(sourceRight,Qt::DisplayRole).toString();
	}
	return true;
}

bool VariableSortModel::filterAcceptsRow(int sourceRow,const QModelIndex& sourceParent) const
{
	if(matchMode_ != FilterMode || matchText_.simplified().isEmpty())
		return true;

	if(!sourceParent.isValid())
		return true;

	QModelIndex idx=varModel_->index(sourceRow,0,sourceParent);
	QModelIndex idx2=varModel_->index(sourceRow,1,sourceParent);

	QString s=varModel_->data(idx,Qt::DisplayRole).toString();
	QString s2=varModel_->data(idx2,Qt::DisplayRole).toString();

	if(s.contains(matchText_,Qt::CaseInsensitive) || s2.contains(matchText_,Qt::CaseInsensitive))
	{
		return true;
	}

	return false;
}

QVariant VariableSortModel::data(const QModelIndex& idx,int role) const
{
    if(role != Qt::UserRole)
    {
        return QSortFilterProxyModel::data(idx,role);
    }    
    
    //We highlight the matching items (the entire row).
    if(matchMode_ == SearchMode && matchLst_.count() >0)
    {
        int col2=(idx.column()==0)?1:0;
        QModelIndex idx2=index(idx.row(),col2,idx.parent());
            
        //qDebug() << idx << idx2;
        
        if(matchLst_.contains(idx) || matchLst_.contains(idx2))
            return QColor(83,187,109);
    }
    
    return QSortFilterProxyModel::data(idx,role);
}    


QModelIndexList VariableSortModel::match(const QModelIndex& start,int role,const QVariant& value,int hits,Qt::MatchFlags flags) const
{
	if(matchMode_ != SearchMode)
		return QModelIndexList();

	QModelIndex root;
	matchText_=value.toString();

	matchLst_.clear();

	if(matchText_.simplified().isEmpty())
		return matchLst_;

	for(int i=0; i < rowCount(); i++)
	{
		QModelIndex idx=index(i,0);
		for(int row=0; row < rowCount(idx);row++)
		{
			//Name column
			QModelIndex colIdx=index(row,0,idx);
			QString s=data(colIdx,Qt::DisplayRole).toString();

			if(s.contains(matchText_,Qt::CaseInsensitive))
			{
				matchLst_ << colIdx;
				continue;
			}

			//Value columns
			QModelIndex colIdx1=index(row,1,idx);
			s=data(colIdx1,Qt::DisplayRole).toString();
			if(s.contains(matchText_,Qt::CaseInsensitive))
			{
				matchLst_ << colIdx;
			}
		}
	}

	return matchLst_;
}


