/****************************************************************************
** ui.h extension file, included from the uic-generated form implementation.
**
** If you want to add, delete, or rename functions or slots, use
** Qt Designer to update this file, preserving your code.
**
** You should not define a constructor or destructor in this file.
** Instead, write your code in functions called init() and destroy().
** These will automatically be called by the form's constructor and
** destructor.
*****************************************************************************/
import os

void Form1::pushButton2_clicked()
{
	
    
}


void Form1::Form1_destroyed( QObject * )
{

	try:
		import os
		os.system("xrandr --output VGA --off")
	except Exception, ex:
		print ex

}


void Form1::VGAChangeButton_clicked()
{
	try:
		import os
		os.system("xrandr --output LVDS --mode 1024x768 --output VGA --mode 1024x768")
	except Exception, ex:
		print ex

}


void Form1::DisableVGAButton_clicked()
{
	try:
		import os
		os.system("xrandr --output VGA --off")
	except Exception, ex:
		print ex

}
