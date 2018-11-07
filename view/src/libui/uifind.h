/*
** Generated by X-Designer
*/
#ifndef _uifind_h
#define _uifind_h

#define XD_MOTIF

#include <xdclass.h>

class find_shell_c: public xd_XmDialog_c {
public:
	virtual void create (Widget parent, char *widget_name = NULL);
protected:
	Widget find_shell;
	Widget form_;
	Widget button_find;
	Widget button_close;
	Widget find_text_;
	Widget message_;
	Widget case_;
	Widget back_;
	Widget regexp_;
	Widget wrap_;
	Widget quick_find_;
	Widget quick_menu_;
	Widget edit_;
public:
	static void entryCB( Widget, XtPointer, XtPointer );
	virtual void entryCB( Widget, XtPointer ) = 0;
	static void regexCB( Widget, XtPointer, XtPointer );
	virtual void regexCB( Widget, XtPointer ) = 0;
	static void closeCB( Widget, XtPointer, XtPointer );
	virtual void closeCB( Widget, XtPointer ) = 0;
	static void findCB( Widget, XtPointer, XtPointer );
	virtual void findCB( Widget, XtPointer ) = 0;
};

typedef find_shell_c *find_shell_p;


extern find_shell_p find_shell;


#endif