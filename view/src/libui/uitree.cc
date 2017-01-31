/*
** Generated by X-Designer
*/
/*
**LIBS: -lXm -lXt -lX11
*/

#include <stdlib.h>
#include <X11/Xatom.h>
#include <X11/Intrinsic.h>
#include <X11/Shell.h>

#include <Xm/Xm.h>
#include <Xm/DialogS.h>
#include <Xm/DrawingA.h>
#include <Xm/Label.h>
#include <Xm/PushB.h>
#include <Xm/RowColumn.h>
#include <Xm/Separator.h>

#include "SimpleTree.h"
#define XmCreateDrawingArea CreateTree
#include "uitree.h"
#include "ecflowview.h"

tree_p tree_ = (tree_p) NULL;

void tree_c::create (Widget parent, char *widget_name)
{
	Widget children[6];      /* Children to manage */
	Arg al[64];                    /* Arg List */
        register int ac = 0;           /* Arg Count */
        Widget button179 = (Widget)NULL;
        Widget button180 = (Widget)NULL;

        if ( !widget_name )
                widget_name = "tree_";

	tree_ = XmCreateDrawingArea ( parent, widget_name, al, ac );
	_xd_rootwidget = tree_;
	see_menu_ = XmCreatePopupMenu ( tree_, "see_menu_", al, ac );
	XtSetArg(al[ac], XmNsensitive, FALSE); ac++;
	XtSetArg(al[ac], XmNmarginTop, 0); ac++;
	XtSetArg(al[ac], XmNmarginBottom, 0); ac++;
	XtSetArg(al[ac], XmNmarginLeft, 0); ac++;
	XtSetArg(al[ac], XmNmarginRight, 0); ac++;
	XtSetArg(al[ac], XmNmarginWidth, 2); ac++;
	XtSetArg(al[ac], XmNmarginHeight, 2); ac++;
	show_current_ = XmCreatePushButton ( see_menu_, "menu_show_current", al, ac );
	ac = 0;
	XtSetArg(al[ac], XmNmarginTop, 0); ac++;
	XtSetArg(al[ac], XmNmarginBottom, 0); ac++;
	XtSetArg(al[ac], XmNmarginLeft, 0); ac++;
	XtSetArg(al[ac], XmNmarginRight, 0); ac++;
	XtSetArg(al[ac], XmNmarginWidth, 2); ac++;
	XtSetArg(al[ac], XmNmarginHeight, 2); ac++;
	button179 = XmCreatePushButton ( see_menu_, "menu_unfold_all", al, ac );
	ac = 0;
	XtSetArg(al[ac], XmNmarginTop, 0); ac++;
	XtSetArg(al[ac], XmNmarginBottom, 0); ac++;
	XtSetArg(al[ac], XmNmarginLeft, 0); ac++;
	XtSetArg(al[ac], XmNmarginRight, 0); ac++;
	XtSetArg(al[ac], XmNmarginWidth, 2); ac++;
	XtSetArg(al[ac], XmNmarginHeight, 2); ac++;
	button180 = XmCreatePushButton ( see_menu_, "menu_fold_all", al, ac );
	ac = 0;
	XtSetArg(al[ac], XmNsensitive, FALSE); ac++;
	fold_around_ = XmCreatePushButton ( see_menu_, "fold_around_", al, ac );
        ac = 0;
        XtSetArg(al[ac], XmNsensitive, FALSE); ac++;
        hide_other_ = XmCreatePushButton ( see_menu_, "hide_other_", al, ac );

        ac = 0;
        XtSetArg(al[ac], XmNsensitive, XECF_SNAP_ENABLED); ac++;
        snapshot_ = XmCreatePushButton ( see_menu_, "snapshot", al, ac );

        ac = 0;
        XtSetArg(al[ac], XmNspacing, 0); ac++;
        XtSetArg(al[ac], XmNentryBorder, 0); ac++;
	XtSetArg(al[ac], XmNmarginWidth, 0); ac++;
	XtSetArg(al[ac], XmNmarginHeight, 0); ac++;
	XtSetArg(al[ac], XmNorientation, XmHORIZONTAL); ac++;
	XtSetArg(al[ac], XmNpacking, XmPACK_TIGHT); ac++;
	XtSetArg(al[ac], XmNentryAlignment, XmALIGNMENT_CENTER); ac++;
	why_menu_ = XmCreatePopupMenu ( tree_, "why_menu", al, ac );
	ac = 0;
	XtSetArg(al[ac], XmNalignment, XmALIGNMENT_BEGINNING); ac++;
	why_label_ = XmCreateLabel ( why_menu_, "why_label_", al, ac );
	ac = 0;
	XtAddCallback (show_current_, XmNactivateCallback,&tree_c:: showCB, (XtPointer) this);
	XtAddCallback (button179, XmNactivateCallback,&tree_c:: unfoldCB, (XtPointer) this);
        XtAddCallback (button180, XmNactivateCallback,&tree_c:: foldCB, (XtPointer) this);
        XtAddCallback (fold_around_, XmNactivateCallback,&tree_c:: aroundCB, (XtPointer) this);
        XtAddCallback (hide_other_, XmNactivateCallback,&tree_c:: hideOtherCB, (XtPointer) this);
        XtAddCallback (snapshot_, XmNactivateCallback,&tree_c:: snapshotCB, (XtPointer) this);
        children[ac++] = show_current_;
        children[ac++] = button179;
        children[ac++] = button180;
        children[ac++] = fold_around_;
        children[ac++] = hide_other_;
        children[ac++] = snapshot_;
        XtManageChildren(children, ac);
        ac = 0;
        children[ac++] = why_label_;
	XtManageChildren(children, ac);
}

void tree_c::hideOtherCB( Widget widget, XtPointer client_data, XtPointer call_data )
{
	tree_p instance = (tree_p) client_data;
        instance->hideOtherCB ( widget, call_data );
}

void tree_c::snapshotCB( Widget widget, XtPointer client_data, XtPointer call_data )
{
  tree_p instance = (tree_p) client_data;
  instance->snapshotCB ( widget, call_data );
}

void tree_c::aroundCB( Widget widget, XtPointer client_data, XtPointer call_data )
{
        tree_p instance = (tree_p) client_data;
	instance->aroundCB ( widget, call_data );
}

void tree_c::foldCB( Widget widget, XtPointer client_data, XtPointer call_data )
{
	tree_p instance = (tree_p) client_data;
	instance->foldCB ( widget, call_data );
}

void tree_c::unfoldCB( Widget widget, XtPointer client_data, XtPointer call_data )
{
	tree_p instance = (tree_p) client_data;
	instance->unfoldCB ( widget, call_data );
}

void tree_c::showCB( Widget widget, XtPointer client_data, XtPointer call_data )
{
	tree_p instance = (tree_p) client_data;
	instance->showCB ( widget, call_data );
}

