import json
import sys,os
import globals

class JsonFile:

    '''
    globals.file1 = sys.argv[1]
    '''
    

    @staticmethod
    def read_json(a,b,c):
        '''
        This method is used to read from the json config file, which takes the user input.

        '''

        if globals.SATA == 0:
            json_file = os.path.join('../ConfigSuite', 'tsuite2.json')
        else:
            json_file = os.path.join('../ConfigSuite', 'tsuite3.json')
        with  open(json_file) as config_json:
            config_data = json.load(config_json)

        script_detail = config_data[a][b][c]
        return script_detail

