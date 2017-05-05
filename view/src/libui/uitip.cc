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


#include "uitip.h"

tip_shell_p tip_shell = (tip_shell_p) NULL;



void tip_shell_c::create (Widget parent, char *widget_name)
{
	Widget children[1];      /* Children to manage */
	Display *display = XtDisplay ( parent );
	Arg al[64];                    /* Arg List */
	register int ac = 0;           /* Arg Count */
	XrmValue from_value, to_value; /* For resource conversion */

	if ( !widget_name )
		widget_name = "tip_shell";

	XtSetArg(al[ac], XmNallowShellResize, TRUE); ac++;
	XtSetArg(al[ac], XmNoverrideRedirect, TRUE); ac++;
	XtSetArg(al[ac], XmNsaveUnder, TRUE); ac++;
	XtSetArg(al[ac], XmNtransient, FALSE); ac++;
	tip_shell = XtCreatePopupShell ( widget_name, topLevelShellWidgetClass, parent, al, ac );
	ac = 0;
	_xd_rootwidget = tip_shell;
	XtSetArg(al[ac], XmNautoUnmanage, FALSE); ac++;
	form_ = XmCreateForm ( tip_shell, "form_", al, ac );
	ac = 0;
	XtSetArg(al[ac], XmNborderWidth, 1); ac++;
	if (DefaultDepthOfScreen(DefaultScreenOfDisplay(display)) != 1) {
	from_value.addr = "#f469f468a87c" ;
	from_value.size = strlen( from_value.addr ) + 1;
	to_value.addr = NULL;
	XtConvertAndStore (form_, XmRString, &from_value, XmRPixel, &to_value);
	if ( to_value.addr )
	{
		XtSetArg(al[ac], XmNbackground, *(unsigned int *)to_value.addr); ac++;
	}
	}
	label_ = XmCreateLabel ( form_, "label_", al, ac );
	ac = 0;

	XtSetArg(al[ac], XmNtopAttachment, XmATTACH_FORM); ac++;
	XtSetArg(al[ac], XmNbottomAttachment, XmATTACH_FORM); ac++;
	XtSetArg(al[ac], XmNleftAttachment, XmATTACH_FORM); ac++;
	XtSetArg(al[ac], XmNrightAttachment, XmATTACH_FORM); ac++;
	XtSetValues ( label_,al, ac );
	ac = 0;
	children[ac++] = label_;
	XtManageChildren(children, ac);
	XtManageChild ( form_);
}

