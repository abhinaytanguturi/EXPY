
from ctypes import cdll
lib = cdll.LoadLibrary('./StCpp.so')

class Storage(object):
    def __init__(self):
        self.obj = lib.SataStorage_new()

    def Start(self):
        lib.SataStorage_Init(self.obj,'/dev/sda')
	r=lib.SataStorage_Identify(self.obj)
	print r
	assert(r==0)
    def ReadSector(self):
	r=lib.ReadSector(self.obj,100)
	print r
	assert(r==0)
    def WriteSector(self):
	r=lib.WriteSector(self.obj,1222222222)
	print r
	assert(r==0)
    def WriteCache(self):
	r=lib.WriteCache(self.obj,0)
	print r
	assert(r==0)
        r=lib.WriteCache(self.obj,1)
	print r
	assert(r==0)
class Nvme(object):
    def __init__(self):
	self.obj=lib.NvmeStorage_new()
    def Start(self):
	lib.NvmeStorage_Init(self.obj,'/dev/nvme0')
	lib.NvmeStorage_Identify(self.obj)

f=Storage()
f.Start()
f.ReadSector()
f.WriteSector()
f.WriteCache()

N=Nvme()
N.Start()
