CRIPT  : menu_dialog.sh
# PURPOSE : A menu driven Shell script using dialog utility
#           which Test Suite to run -
#           SATA
#           NVMe
#           Exit
#
##############################################################################
#                 Checking availability of dialog utility                    #
##############################################################################

# dialog is a utility installed by default on all major Linux distributions.
# But it is good to check availability of dialog utility on your Linux box.

which dialog &> /dev/null

[ $? -ne 0 ]  && echo "Dialog utility is not available, Install it" && exit 1

##############################################################################
#                      Define Functions Here                                 #
##############################################################################

###################### InvokeSATA_TestSuite function ##############################

# This function is called when 'Run SATA Tests' option is selected from Main menu

InvokeSATA_TestSuite()
{
	python main.py SATA.json 
	python main.py SATA.json > ../../Logging/SATA/SATACmdLog.txt
	python restart.py
	clear 
	exit 0	
}


######################## InvokeNVMe_TestSuite function #################################

# This function is called when 'Run NVMe Tests' option is selected from Main menu

InvokeNVMe_TestSuite()
{
	python main.py NVME.json 
	python main.py NVME.json > ../../Logging/NVMe/NvmeCmdLog.txt
	python restart.py
	clear
	exit 0
}

##############################################################################
#                           MAIN STARTS HERE                                 #
##############################################################################

while :
do

# Dialog utility to display options list

    dialog --backtitle "MENU DRIVEN PROGRAM" --title "MAIN MENU" \
    --menu "Use [UP/DOWN] key to move" 12 60 6  \
    "SATA" "To invoke SATA based Test Cases" \
    "NVMe" "To invoke NVMe based Test Cases" \
    "EXIT"                "TO EXIT" 2> menuchoices.$$

    retopt=$?
    choice=`cat menuchoices.$$`

    case $retopt in

           0) case $choice in

                  SATA)  InvokeSATA_TestSuite ;;
                  NVMe)  InvokeNVMe_TestSuite ;;
                  EXIT)       clear; rm -f menuchoices.* ; exit 0 ;;  

              esac ;;

          *)clear ; exit ;;
    esac

done 
