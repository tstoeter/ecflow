/*
** Generated by X-Designer
*/
#ifndef _uimail_h
#define _uimail_h

#define XD_MOTIF

#include <xdclass.h>

class mail_shell_c: public xd_XmDialog_c {
public:
	virtual void create (Widget parent, char *widget_name = NULL);
protected:
	Widget mail_shell;
	Widget form_;
	Widget text_;
	Widget input_;
	Widget list_;
public:
	static void closeCB( Widget, XtPointer, XtPointer );
	virtual void closeCB( Widget, XtPointer ) = 0;
	static void sendCB( Widget, XtPointer, XtPointer );
	virtual void sendCB( Widget, XtPointer ) = 0;
};

typedef mail_shell_c *mail_shell_p;


extern mail_shell_p mail_shell;


#endif