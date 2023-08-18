import ctypes
import pytest
import sys,os
import string

from time import time, sleep
sys.path.insert(0, '../../TestEngine/TestInterface/NVMe')
from NVME_wrapper import * 
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
		globals.SATA=0
	
	def test_Nvme_Ctrl_001(self):
		globals.NvmeIdCtrl = NvmeStorage(device_node.value).IDD()
		b.Test_Assert(globals.NvmeIdCtrl,0)
	
	def test_Nvme_List_002(self):
		globals.NvmeIdList = NvmeStorage(device_node.value).List()
		b.Test_Assert(globals.NvmeIdList,0)

'''
	def test_identify_003(self):
       		r = Storage(device_node.value).IDD()
        	b.Test_Assert(r,0)
	
	def test_read_sector_004(self):
		r = Storage(device_node.value).ReadSector(100)
		b.Test_Assert(r,0)
'''	
'''
 	def test_read_sector_005(self,10000000):
		r = libb.ReadSector(self.obj,val)
 		b.Test_Assert(r,0)

	def test_write_sector_006(self,1000000000):
		r = libb.WriteSector(self.obj,val)
		b.Test_Assert(r,0)
'''

