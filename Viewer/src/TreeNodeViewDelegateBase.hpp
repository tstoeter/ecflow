//============================================================================
// Copyright 2009-2017 ECMWF.
// This software is licensed under the terms of the Apache Licence version 2.0
// which can be obtained at http://www.apache.org/licenses/LICENSE-2.0.
// In applying this licence, ECMWF does not waive the privileges and immunities
// granted to it by virtue of its status as an intergovernmental organisation
// nor does it submit to any jurisdiction.
//
//============================================================================

#ifndef TREENODEVIEWDELEGATEBASE_HPP
#define TREENODEVIEWDELEGATEBASE_HPP

#include <QBrush>
#include <QMap>
#include <QPen>
#include <QStyledItemDelegate>

#include "NodeViewDelegate.hpp"
#include "VProperty.hpp"

#include <string>

class AnimationHandler;
class PropertyMapper;
class NodeShape;
class NodeText;
class ServerUpdateData;
class TreeNodeModel;

class TreeNodeViewDelegateBase : public NodeViewDelegate
{
Q_OBJECT

public:
    explicit TreeNodeViewDelegateBase(TreeNodeModel* model,QWidget *parent=0);
    ~TreeNodeViewDelegateBase();

    void setIndentation(int o) {indentation_=o;}

Q_SIGNALS:
    void sizeHintChangedGlobal();

protected:
    void updateSettings();

    int renderServer(QPainter *painter,const QModelIndex& index,
                        const QStyleOptionViewItem& option,QString text) const;

    int renderNode(QPainter *painter,const QModelIndex& index,
                    const QStyleOptionViewItem& option,QString text) const;

    void renderServerCell(QPainter *painter,const NodeShape& stateShape,
                                            const NodeText& text,bool selected) const;

    void renderNodeCell(QPainter *painter,const NodeShape& stateShape,const NodeShape &realShape,
                       const NodeText& nodeText,const NodeText& typeText,bool selected) const;

    void renderNodeShape(QPainter* painter,const NodeShape& shape) const;
    void renderTimer(QPainter *painter,QRect target, int remaining, int total) const;
    void renderServerUpdate(QPainter* painter,const ServerUpdateData&) const;

    void widthHintServer(const QModelIndex& index,int& itemWidth,QString text) const;
    int nodeWidth(const QModelIndex& index,QString text) const;

    QString formatTime(int timeInSec) const;
    QColor interpolate(QColor c1,QColor c2,float r) const;

    enum NodeStyle {ClassicNodeStyle,BoxAndTextNodeStyle};

    AnimationHandler* animation_;

    int nodeRectRad_;
    bool drawChildCount_;
    NodeStyle nodeStyle_;
    int indentation_;

    bool drawNodeType_;
    QColor typeBgCol_;

    QFont serverNumFont_;
    QFont suiteNumFont_;
    QFont serverInfoFont_;
    QFont abortedReasonFont_;
    QFont typeFont_;
    QColor bgCol_;

    TreeNodeModel* model_;
};

#endif // TREENODEVIEWDELEGATEBASE_HPP



