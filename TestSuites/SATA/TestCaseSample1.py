#import pytest
from loggin import TestCase
b = TestCase()

class Test_demo:
    def setup_method(self, method):
        b.logger.info("TEST CASE STARTED")
        b.logger.info("%s" % (method.__name__))

    def test_001(self):
        b.logger.info("inside testcase 001")
        r = 0
        b.Test_Assert(r,0)

    def test_002(self):
        b.logger.info("inside testcase 002")
        r = 5
        b.Test_Assert(r, 5)

    def test_003(self):
        #b.logger.info("START TEST CASE")
        b.logger.error("inside testcase 003")
        b.Test_Assert(0,0)
