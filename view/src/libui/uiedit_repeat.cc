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
#include <Xm/Form.h>
#include <Xm/Label.h>
#include <Xm/List.h>
#include <Xm/PushB.h>
#include <Xm/RowColumn.h>
#include <Xm/ScrollBar.h>
#include <Xm/TextF.h>


#include "uiedit_repeat.h"

edit_repeat_p edit_repeat_form = (edit_repeat_p) NULL;



void edit_repeat_form_c::create (Widget parent, char *widget_name)
{
	Widget children[3];      /* Children to manage */
	Arg al[64];                    /* Arg List */
	register int ac = 0;           /* Arg Count */
	Widget button1 = (Widget)NULL;
	Widget form1 = (Widget)NULL;

	if ( !widget_name )
		widget_name = "edit_repeat_form";

	XtSetArg(al[ac], XmNautoUnmanage, FALSE); ac++;
	edit_repeat_form = XmCreateForm ( parent, widget_name, al, ac );
	ac = 0;
	_xd_rootwidget = edit_repeat_form;
	tools_ = XmCreateRowColumn ( edit_repeat_form, "tools_", al, ac );
	button1 = XmCreatePushButton ( tools_, "Apply", al, ac );
	form1 = XmCreateForm ( edit_repeat_form, "form1", al, ac );
	XtSetArg(al[ac], XmNlistSizePolicy, XmRESIZE_IF_POSSIBLE); ac++;
	list_ = XmCreateScrolledList ( form1, "list_", al, ac );
	ac = 0;
	show_list_ = XtParent ( list_ );

	show_text_ = XmCreateForm ( form1, "show_text_", al, ac );
	text_ = XmCreateTextField ( show_text_, "text_", al, ac );
	label_ = XmCreateLabel ( show_text_, "label_", al, ac );

	XtSetArg(al[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
	XtSetArg(al[ac], XmNleftAttachment, XmATTACH_NONE); ac++;
	XtSetArg(al[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
	XtSetValues ( tools_,al, ac );
	ac = 0;

	XtSetArg(al[ac], XmNtopAttachment, XmATTACH_WIDGET); ac++;
	XtSetArg(al[ac], XmNtopWidget, tools_); ac++;
	XtSetArg(al[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
	XtSetArg(al[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
	XtSetArg(al[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
	XtSetValues ( form1,al, ac );
	ac = 0;
	XtAddCallback (button1, XmNactivateCallback,&edit_repeat_form_c:: applyCB, (XtPointer) this);
	children[ac++] = button1;
	XtManageChildren(children, ac);
	ac = 0;

	XtSetArg(al[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
	XtSetArg(al[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
	XtSetArg(al[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
	XtSetArg(al[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
	XtSetValues ( show_list_,al, ac );
	ac = 0;

	XtSetArg(al[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
	XtSetArg(al[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
	XtSetArg(al[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
	XtSetArg(al[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
	XtSetValues ( show_text_,al, ac );
	ac = 0;
	XtAddCallback (list_, XmNbrowseSelectionCallback,&edit_repeat_form_c:: browseCB, (XtPointer) this);
	XtManageChild(list_);

	XtSetArg(al[ac], XmNtopAttachment, XmATTACH_WIDGET); ac++;
	XtSetArg(al[ac], XmNtopWidget, label_); ac++;
	XtSetArg(al[ac], XmNbottomAttachment, XmATTACH_NONE); ac++;
	XtSetArg(al[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
	XtSetArg(al[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
	XtSetValues ( text_,al, ac );
	ac = 0;

	XtSetArg(al[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
	XtSetArg(al[ac], XmNbottomAttachment, XmATTACH_NONE); ac++;
	XtSetArg(al[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
	XtSetArg(al[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
	XtSetValues ( label_,al, ac );
	ac = 0;
	children[ac++] = text_;
	children[ac++] = label_;
	XtManageChildren(children, ac);
	ac = 0;
	children[ac++] = tools_;
	children[ac++] = form1;
	XtManageChildren(children, ac);
}

void edit_repeat_form_c::applyCB( Widget widget, XtPointer client_data, XtPointer call_data )
{
	edit_repeat_form_p instance = (edit_repeat_form_p) client_data;
	instance->applyCB ( widget, call_data );
}

void edit_repeat_form_c::browseCB( Widget widget, XtPointer client_data, XtPointer call_data )
{
	edit_repeat_form_p instance = (edit_repeat_form_p) client_data;
	instance->browseCB ( widget, call_data );
}

