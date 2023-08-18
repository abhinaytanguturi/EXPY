#!/bin/bash

echo  "Select one of the following Storage TestSuites> "
echo  "1.SATA"
echo  "2.Nvme"
echo  "3.SCSI"
read character
case $character in
    1 ) python main.py tsuite2.json 
	python main.py tsuite2.json > ../../Logging/SATA/SATACmdLog.txt
        ;;
    2 ) python main.py tsuite3.json 
	python main.py tsuite3.json > nvme.txt
        ;;
    3 ) echo "You entered three."
        ;;
    * ) echo "You did not enter a number"
        echo "between 1 and 3."
esac
