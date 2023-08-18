from ctypes import cdll
from FromJsonFiles import *

libb = cdll.LoadLibrary(JsonFile.read_json('location_path',0,'api_path' ))


class NvmeStorage(object):
    def __init__(self,device_node):
        self.obj = libb.NvmeStorage_new()
        libb.NvmeStorage_Init(self.obj,device_node)

    def IDD(self):
        r = libb.NvmeStorage_Identify(self.obj)
        return r
    def List(self):
	r = libb.NvmeStorage_list(self.obj)
        return r
'''
    def WriteSector(self,val):
        r = libb.WriteSector(self.obj,val)
        return r
    def WriteCache(self,val):
        r = libb.WriteCache(self.obj,val)
        return r
'''
