from qt import *
from goruntuaktarimi import *
import sys, os

if __name__ == "__main__":
     
    try:	

	psid =  os.getpid()
	rule = "ps aux | grep python | grep mygui | awk '$0 !~ /grep | "+str(psid)+"/"+" {print $2}'"
    	r=os.popen(rule)
    	rtr=r.readlines()
	#print rtr
	#pid=rtr[0].strip()
	#s = "ps aux | grep "+str(pid)
	#os.system(s)
    	if len(rtr) > 0:
        	print "girdi"
		pid=int(rtr[0].strip())
		os.kill(pid,9)
	else:
		app = QApplication(sys.argv)
    		f = Form1()
    		f.show()
    		app.setMainWidget(f)
    		app.exec_loop()
    except Exception, ex:
    	print "Exception:", ex

    
