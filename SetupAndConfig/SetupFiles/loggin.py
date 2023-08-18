import logging
import os
import time
import os, datetime
from FromJsonFiles import *

num_testcases_passed = 0
num_testcases_failed = 0
tol_num_testcases = 0

mydir = JsonFile.read_json('location_path',0,'log_path' )
 

# <chandu> I have commented this part which creates a new folder on timestamp and logs each time without overwritting
'''timestr = time.strftime("%Y%m%d-%H%M%S")

mydir = os.path.join(os.getcwd(), timestr)
try:
    os.makedirs(mydir)
except OSError, e:
    if e.errno != 17:
        raise  # This was not a "directory exist" error..
'''
#<chandu>  Logging should be upgraded to have a flag set in json which moved the log file to database

class LogginClass:

    mydir = JsonFile.read_json('location_path',0,'log_path' )
    @property
    def logger(self):
        self.logger = logging.getLogger('demo')
        if not getattr(self.logger, 'handler_set', None):
            self.logger.setLevel(logging.INFO)
            self.fh = logging.FileHandler(os.path.join(mydir, "runLog.log"),"w",encoding=None, delay="true")
            self.fh.setLevel(logging.INFO)
            self.ch = logging.StreamHandler()
            self.ch.setLevel(logging.INFO)
            formatter = logging.Formatter('[%(asctime)s] - %(levelname)s - %(message)s')
            self.fh.setFormatter(formatter)
            self.ch.setFormatter(formatter)
            self.logger.addHandler(self.fh)
            self.logger.addHandler(self.ch)
            self.logger.handler_set = True
        return self.logger

    def Test_Assert(self,r,e):
        if r == e:
            global num_testcases_passed
            num_testcases_passed += 1
            global tol_num_testcases
            tol_num_testcases += 1
            self.logger.info("[PASSED]")
            self.logger.info("TEST CASE ENDED\n")
            assert True
        else:
            global num_testcases_failed
            num_testcases_failed += 1
            global tol_num_testcases
            tol_num_testcases += 1
            self.logger.error("[FAILED]")
            self.logger.info("TEST CASE ENDED\n")
            assert False

    def casecounter(self):
        return (num_testcases_passed, num_testcases_failed, tol_num_testcases)

