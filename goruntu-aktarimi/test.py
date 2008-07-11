# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file 'test.ui'
#
# Created: Mon Jun 30 14:17:13 2008
#      by: The PyQt User Interface Compiler (pyuic) 3.17.4
#
# WARNING! All changes made in this file will be lost!


from qt import *


class Form1(QDialog):
    def __init__(self,parent = None,name = None,modal = 0,fl = 0):
        QDialog.__init__(self,parent,name,modal,fl)

        if not name:
            self.setName("Form1")

        self.setEnabled(1)
        self.setSizePolicy(QSizePolicy(QSizePolicy.Preferred,QSizePolicy.Preferred,0,0,self.sizePolicy().hasHeightForWidth()))
        self.setMaximumSize(QSize(367,310))
        self.setBaseSize(QSize(0,0))
        self.setBackgroundOrigin(QDialog.WindowOrigin)
        self.setModal(0)


        self.tabWidget = QTabWidget(self,"tabWidget")
        self.tabWidget.setGeometry(QRect(10,10,340,290))
        self.tabWidget.setBackgroundOrigin(QTabWidget.WindowOrigin)
        self.tabWidget.setFocusPolicy(QTabWidget.ClickFocus)
        self.tabWidget.setAcceptDrops(0)
        self.tabWidget.setTabShape(QTabWidget.Rounded)

        self.tab = QWidget(self.tabWidget,"tab")

        self.DisableVGAButton = QPushButton(self.tab,"DisableVGAButton")
        self.DisableVGAButton.setGeometry(QRect(247,208,74,34))
        self.DisableVGAButton.setSizePolicy(QSizePolicy(QSizePolicy.Preferred,QSizePolicy.Preferred,0,0,self.DisableVGAButton.sizePolicy().hasHeightForWidth()))
        self.DisableVGAButton.setBackgroundOrigin(QPushButton.WindowOrigin)

        self.VGAChangeButton = QPushButton(self.tab,"VGAChangeButton")
        self.VGAChangeButton.setGeometry(QRect(121,208,77,34))
        self.VGAChangeButton.setSizePolicy(QSizePolicy(QSizePolicy.Preferred,QSizePolicy.Preferred,0,0,self.VGAChangeButton.sizePolicy().hasHeightForWidth()))
        self.VGAChangeButton.setBackgroundOrigin(QPushButton.WindowOrigin)

        self.HelpButton = QPushButton(self.tab,"HelpButton")
        self.HelpButton.setGeometry(QRect(13,208,58,34))
        self.HelpButton.setSizePolicy(QSizePolicy(QSizePolicy.Preferred,QSizePolicy.Preferred,0,0,self.HelpButton.sizePolicy().hasHeightForWidth()))

        self.warninButton = QLabel(self.tab,"warninButton")
        self.warninButton.setGeometry(QRect(120,70,80,40))
        self.warninButton.setBackgroundOrigin(QLabel.WindowOrigin)
        self.warninButton.setAlignment(QLabel.WordBreak | QLabel.AlignCenter)
        self.tabWidget.insertTab(self.tab,QString.fromLatin1(""))

        self.tab_2 = QWidget(self.tabWidget,"tab_2")
        self.tabWidget.insertTab(self.tab_2,QString.fromLatin1(""))

        self.TabPage = QWidget(self.tabWidget,"TabPage")
        self.tabWidget.insertTab(self.TabPage,QString.fromLatin1(""))

        self.TabPage_2 = QWidget(self.tabWidget,"TabPage_2")
        self.tabWidget.insertTab(self.TabPage_2,QString.fromLatin1(""))

        self.TabPage_3 = QWidget(self.tabWidget,"TabPage_3")
        self.tabWidget.insertTab(self.TabPage_3,QString.fromLatin1(""))

        self.languageChange()

        self.resize(QSize(367,310).expandedTo(self.minimumSizeHint()))
        self.clearWState(Qt.WState_Polished)

        self.connect(self,SIGNAL("destroyed(QObject*)"),self.Form1_destroyed)
        self.connect(self.VGAChangeButton,SIGNAL("clicked()"),self.VGAChangeButton_clicked)
        self.connect(self.DisableVGAButton,SIGNAL("clicked()"),self.DisableVGAButton_clicked)


    def languageChange(self):
        self.setCaption(self.__tr("Display Switch"))
        self.DisableVGAButton.setText(self.__tr("Disable"))
        self.VGAChangeButton.setText(self.__tr("Change"))
        self.HelpButton.setText(self.__tr("Help"))
        self.warninButton.setText(self.__tr("<font color=\"#ff0000\"></font>"))
        self.tabWidget.changeTab(self.tab,self.__tr("Clone to VGA"))
        self.tabWidget.changeTab(self.tab_2,self.__tr("Clone to HDMI"))
        self.tabWidget.changeTab(self.TabPage,self.__tr("Clone to SVIDEO"))
        self.tabWidget.changeTab(self.TabPage_2,self.__tr("Tab"))
        self.tabWidget.changeTab(self.TabPage_3,self.__tr("Tab"))


    def pushButton2_clicked(self):
        print "Form1.pushButton2_clicked(): Not implemented yet"

    def Form1_destroyed(self,a0):
        
        	try:
        		import os
        		os.system("xrandr --output VGA --off")
        	except Exception, ex:
        		print ex
        
        

    def VGAChangeButton_clicked(self):
        	try:
        		import os
        		os.system("xrandr --output LVDS --mode 1024x768 --output VGA --mode 1024x768")
        	except Exception, ex:
        		print ex
        
        

    def DisableVGAButton_clicked(self):
        	try:
        		import os
        		os.system("xrandr --output VGA --off")
        	except Exception, ex:
        		print ex
        
        

    def __tr(self,s,c = None):
        return qApp.translate("Form1",s,c)
