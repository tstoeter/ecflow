/*
** Generated by X-Designer
*/
#ifndef _uijobcheck_h
#define _uijobcheck_h

#define XD_MOTIF

#include <xdclass.h>

class jobcheck_form_c: public xd_XmForm_c {
public:
        virtual void create (Widget parent, char *widget_name = NULL);
protected:
        Widget jobcheck_form;
        Widget text_;
        Widget name_;
        Widget tools_;
public:
        static void refreshCB( Widget, XtPointer, XtPointer );
        virtual void refreshCB( Widget, XtPointer ) = 0;
};

typedef jobcheck_form_c *jobcheck_form_p;
class jobcheck;
typedef jobcheck *jobcheck_p;


extern jobcheck_p jobcheck_form;


#endif