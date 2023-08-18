import pytest
import globals
from loggin import *
from FromJsonFiles import *
a = JsonFile()
b = LogginClass()
#script_path = "/home/chandrikas/EXPY/TestSuites/SATA/TestCaseSample1.py"
script_path = a.read_json('location_path',0,'script_path')


print "script path = %s" %script_path
pytest.main(["-x" , script_path])

p,f,t = b.casecounter()

b.logger.info("\nTotal Number of test cases : %s \n\t Total PASSED: %s \n\t Total FAILED: %s" %(t,p,f))
