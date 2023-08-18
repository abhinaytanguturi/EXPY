import ctypes
import pytest
import sys,os
import string

from time import time, sleep
sys.path.insert(0, '../../TestEngine/TestInterface/SATA')
from SATA_wrapper import * 
from loggin import LogginClass
from FromJsonFiles import *
import globals

#<chandu> all the reference to other files should be moved under one class so that, creating multiple objects is reduced


b = LogginClass()
device_node = JsonFile.read_json('details',0,'drive_name' )
device_node = ctypes.c_char_p(device_node)



class Test_EntryPoint:

	def setup_method(self, method):
		b.logger.info("TEST CASE STARTED")
		b.logger.info("%s" % (method.__name__))
		globals.SATA=1


	def test_write_cache_enable(self):
		r = Storage(device_node.value).WriteCache(1)
		globals.retvalCacheE = r
		b.Test_Assert(r,0)


	def test_write_cache_disable(self):
		r = Storage(device_node.value).WriteCache(0)
		globals.retvalCache = r
		b.Test_Assert(r,0)

	def test_identify(self):
		globals.retvalIDD = Storage(device_node.value).IDD()
        b.Test_Assert(globals.retval,0)


	def test_readsector(self):
		r = Storage(device_node.value).ReadSector(20000)
		b.Test_Assert(r,0)


	def test_writesector(self):
		r = Storage(device_node.value).WriteSector(199999999)
		b.Test_Assert(r, 0)


	def test_readsector(self):
		r = Storage(device_node.value).ReadSector(199999999)
		b.Test_Assert(r, 0)
