#include <iostream>
#include <locale.h>
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>
//#ifdef LIBUDEV_EXISTS
#include <libudev.h>
//#endif
#include <cassert>
#include <scsi/scsi.h>
#include <scsi/sg.h>

#include "sgio.h"
#include "hdparm.h"

#include "plugin.h"

#include <linux/hdreg.h>
#include "nvme.h"
#include "Storage.h"
#include "Nvmehdr.h"
const char *cfg_str[] =
{	"",	        " HardSect",   " SoftSect",  " NotMFM",
	" HdSw>15uSec", " SpinMotCtl", " Fixed",     " Removeable",
	" DTR<=5Mbs",   " DTR>5Mbs",   " DTR>10Mbs", " RotSpdTol>.5%",
	" dStbOff",     " TrkOff",     " FmtGapReq", " nonMagnetic"
};

#define YN(b)	(((b)==0)?"no":"yes")

static int   read_sector = 0;
 int wcache   = 0,set_wcache   = 0, get_wcache   = 0, glob=0;
static int  write_sector=0;
static __u64 read_sector_addr = 0;


extern int fd;
static __u64 write_sector_addr = 0;

static const unsigned int default_timeout_secs = 15;

 __u16 *id;
static int open_flags = O_RDONLY|O_NONBLOCK;
static int do_IDentity = 0;
static __u8 last_identify_op = 0;    
//static void get_identify_data (int fd);


int timeout_60secs = 60;


int do_drive_cmd (int fd, unsigned char *args, unsigned int timeout_secs);
//inline int needs_lba48 (__u8 ata_op, __u64 lba, unsigned int nsect);
static int is_cfa_dev (__u16 *id);
//void identify (int fd, __u16 *id_supplied);
int prefer_ata12 = 0;
int verbose = 0;
static int do_defaults = 0, do_flush = 0, do_ctimings, do_timings = 0;
char *progname;
static int    argc;
//static char *argv;
//static char  *argp;
static int    num_flags_processed = 0;
static int do_identity = 0, get_geom = 0, noisy = 1, quiet = 0;
const char *pkt_str[] = {
	"Direct-access device",			/* word 0, bits 12-8 = 00 */
	"Sequential-access device",		/* word 0, bits 12-8 = 01 */
	"Printer",				/* word 0, bits 12-8 = 02 */
	"Processor",				/* word 0, bits 12-8 = 03 */
	"Write-once device",			/* word 0, bits 12-8 = 04 */
	"CD-ROM",				/* word 0, bits 12-8 = 05 */
	"Scanner",				/* word 0, bits 12-8 = 06 */
	"Optical memory",			/* word 0, bits 12-8 = 07 */
	"Medium changer",			/* word 0, bits 12-8 = 08 */
	"Communications device",		/* word 0, bits 12-8 = 09 */
	"ACS-IT8 device",			/* word 0, bits 12-8 = 0a */
	"ACS-IT8 device",			/* word 0, bits 12-8 = 0b */
	"Array controller",			/* word 0, bits 12-8 = 0c */
	"Enclosure services",			/* word 0, bits 12-8 = 0d */
	"Reduced block command device",		/* word 0, bits 12-8 = 0e */
	"Optical card reader/writer",		/* word 0, bits 12-8 = 0f */
	"",					/* word 0, bits 12-8 = 10 */
	"",					/* word 0, bits 12-8 = 11 */
	"",					/* word 0, bits 12-8 = 12 */
	"",					/* word 0, bits 12-8 = 13 */
	"",					/* word 0, bits 12-8 = 14 */
	"",					/* word 0, bits 12-8 = 15 */
	"",					/* word 0, bits 12-8 = 16 */
	"",					/* word 0, bits 12-8 = 17 */
	"",					/* word 0, bits 12-8 = 18 */
	"",					/* word 0, bits 12-8 = 19 */
	"",					/* word 0, bits 12-8 = 1a */
	"",					/* word 0, bits 12-8 = 1b */
	"",					/* word 0, bits 12-8 = 1c */
	"",					/* word 0, bits 12-8 = 1d */
	"",					/* word 0, bits 12-8 = 1e */
	"Unknown",				/* word 0, bits 12-8 = 1f */
};
const char *ata1_cfg_str[] = {			/* word 0 in ATA-1 mode */
	"reserved",				/* bit 0 */
	"hard sectored",			/* bit 1 */
	"soft sectored",			/* bit 2 */
	"not MFM encoded ",			/* bit 3 */
	"head switch time > 15us",		/* bit 4 */
	"spindle motor control option",		/* bit 5 */
	"fixed drive",				/* bit 6 */
	"removable drive",			/* bit 7 */
	"disk xfer rate <= 5Mbs",		/* bit 8 */
	"disk xfer rate > 5Mbs, <= 10Mbs",	/* bit 9 */
	"disk xfer rate > 5Mbs",		/* bit 10 */
	"rotational speed tol.",		/* bit 11 */
	"data strobe offset option",		/* bit 12 */
	"track offset option",			/* bit 13 */
	"format speed tolerance gap reqd",	/* bit 14 */
	"ATAPI"					/* bit 14 */
};

/* word 1: number of logical cylinders */
#define LCYLS_MAX		0x3fff /* maximum allowable value */

/* word 2: specific configureation 
 * (a) require SET FEATURES to spin-up
 * (b) require spin-up to fully reply to IDENTIFY DEVICE
 */
#define STBY_NID_VAL		0x37c8  /*     (a) and     (b) */
#define STBY_ID_VAL		0x738c	/*     (a) and not (b) */
#define PWRD_NID_VAL 		0x8c73	/* not (a) and     (b) */
#define PWRD_ID_VAL		0xc837	/* not (a) and not (b) */

/* words 47 & 59: sector_xfer_max & sector_xfer_cur */
#define SECTOR_XFER		0x00ff  /* sectors xfered on r/w multiple cmds*/
#define MULTIPLE_SETTING_VALID  0x0100  /* 1=multiple sector setting is valid */

/* word 49: capabilities 0 */
#define STD_STBY  		0x2000  /* 1=standard values supported (ATA);
					   0=vendor specific values */
#define IORDY_SUP		0x0800  /* 1=support; 0=may be supported */
#define IORDY_OFF		0x0400  /* 1=may be disabled */
#define LBA_SUP			0x0200  /* 1=Logical Block Address support */
#define DMA_SUP			0x0100  /* 1=Direct Memory Access support */
#define DMA_IL_SUP		0x8000  /* 1=interleaved DMA support (ATAPI) */
#define CMD_Q_SUP		0x4000  /* 1=command queuing support (ATAPI) */
#define OVLP_SUP		0x2000  /* 1=overlap operation support (ATAPI) */
#define SWRST_REQ		0x1000  /* 1=ATA SW reset required (ATAPI, obsolete */

/* word 50: capabilities 1 */
#define MIN_STANDBY_TIMER	0x0001  /* 1=device specific standby timer value minimum */

/* words 51 & 52: PIO & DMA cycle times */
#define MODE			0xff00  /* the mode is in the MSBs */

/* word 53: whats_valid */
#define OK_W88     		0x0004	/* the ultra_dma info is valid */
#define OK_W64_70		0x0002  /* see above for word descriptions */
#define OK_W54_58		0x0001  /* current cyl, head, sector, cap. info valid */

/*word 63,88: dma_mode, ultra_dma_mode*/
#define MODE_MAX		7	/* bit definitions force udma <=7 (when
					 * udma >=8 comes out it'll have to be
					 * defined in a new dma_mode word!) */

/* word 64: PIO transfer modes */
#define PIO_SUP			0x00ff  /* only bits 0 & 1 are used so far,  */
#define PIO_MODE_MAX		8       /* but all 8 bits are defined        */

/* word 75: queue_depth */
#define DEPTH_BITS		0x001f  /* bits used for queue depth */

/* words 80-81: version numbers */
/* 0x0000 or  0xffff means device does not report version */

/* word 81: minor version number */
#define MINOR_MAX		0x22
const char *minor_str[MINOR_MAX+2] = {			/* word 81 value: */
	"Unspecified",					/* 0x0000	*/
	"ATA-1 X3T9.2 781D prior to revision 4",	/* 0x0001	*/
	"ATA-1 published, ANSI X3.221-1994",		/* 0x0002	*/
	"ATA-1 X3T9.2 781D revision 4",			/* 0x0003	*/
	"ATA-2 published, ANSI X3.279-1996",		/* 0x0004	*/
	"ATA-2 X3T10 948D prior to revision 2k",	/* 0x0005	*/
	"ATA-3 X3T10 2008D revision 1",			/* 0x0006	*/
	"ATA-2 X3T10 948D revision 2k",			/* 0x0007	*/
	"ATA-3 X3T10 2008D revision 0",			/* 0x0008	*/
	"ATA-2 X3T10 948D revision 3",			/* 0x0009	*/
	"ATA-3 published, ANSI X3.298-199x",		/* 0x000a	*/
	"ATA-3 X3T10 2008D revision 6",			/* 0x000b	*/
	"ATA-3 X3T13 2008D revision 7 and 7a",		/* 0x000c	*/
	"ATA/ATAPI-4 X3T13 1153D revision 6",		/* 0x000d	*/
	"ATA/ATAPI-4 T13 1153D revision 13",		/* 0x000e	*/
	"ATA/ATAPI-4 X3T13 1153D revision 7",		/* 0x000f	*/
	"ATA/ATAPI-4 T13 1153D revision 18",		/* 0x0010	*/
	"ATA/ATAPI-4 T13 1153D revision 15",		/* 0x0011	*/
	"ATA/ATAPI-4 published, ANSI INCITS 317-1998",	/* 0x0012	*/
	"ATA/ATAPI-5 T13 1321D revision 3",
	"ATA/ATAPI-4 T13 1153D revision 14",		/* 0x0014	*/
	"ATA/ATAPI-5 T13 1321D revision 1",		/* 0x0015	*/
	"ATA/ATAPI-5 published, ANSI INCITS 340-2000",	/* 0x0016	*/
	"ATA/ATAPI-4 T13 1153D revision 17",		/* 0x0017	*/
	"ATA/ATAPI-6 T13 1410D revision 0",		/* 0x0018	*/
	"ATA/ATAPI-6 T13 1410D revision 3a",		/* 0x0019	*/
	"ATA/ATAPI-7 T13 1532D revision 1",		/* 0x001a	*/
	"ATA/ATAPI-6 T13 1410D revision 2",		/* 0x001b	*/
	"ATA/ATAPI-6 T13 1410D revision 1",		/* 0x001c	*/
	"ATA/ATAPI-7 published, ANSI INCITS 397-2005",	/* 0x001d	*/
	"ATA/ATAPI-7 T13 1532D revision 0",		/* 0x001e	*/
	"Reserved",					/* 0x001f	*/
	"Reserved",					/* 0x0020	*/
	"ATA/ATAPI-7 T13 1532D revision 4a",		/* 0x0021	*/
	"ATA/ATAPI-6 published, ANSI INCITS 361-2002",	/* 0x0022	*/
	"Reserved",					/* 0x0023-0xfffe*/
};
const char actual_ver[MINOR_MAX+2] = { 
			/* word 81 value: */
	0,		/* 0x0000	WARNING: 	*/
	1,		/* 0x0001	WARNING: 	*/
	1,		/* 0x0002	WARNING: 	*/
	1,		/* 0x0003	WARNING: 	*/
	2,		/* 0x0004	WARNING:   This array 		*/
	2,		/* 0x0005	WARNING:   corresponds 		*/
	3,		/* 0x0006	WARNING:   *exactly*		*/
	2,		/* 0x0007	WARNING:   to the ATA/		*/
	3,		/* 0x0008	WARNING:   ATAPI version	*/
	2,		/* 0x0009	WARNING:   listed in		*/
	3,		/* 0x000a	WARNING:   the 			*/
	3,		/* 0x000b	WARNING:   minor_str 		*/
	3,		/* 0x000c	WARNING:   array		*/
	4,		/* 0x000d	WARNING:   above.		*/
	4,		/* 0x000e	WARNING:  			*/
	4,		/* 0x000f	WARNING:   if you change 	*/
	4,		/* 0x0010	WARNING:   that one,      	*/
	4,		/* 0x0011	WARNING:   change this one	*/
	4,		/* 0x0012	WARNING:   too!!!        	*/
	5,		/* 0x0013	WARNING:	*/
	4,		/* 0x0014	WARNING:	*/
	5,		/* 0x0015	WARNING:	*/
	5,		/* 0x0016	WARNING:	*/
	4,		/* 0x0017	WARNING:	*/
	6,		/* 0x0018	WARNING:	*/
	6,		/* 0x0019	WARNING:	*/
	7,		/* 0x001a	WARNING:	*/
	6,		/* 0x001b	WARNING:	*/
	6,		/* 0x001c	WARNING:	*/
	7,		/* 0x001d	WARNING:	*/
	7,		/* 0x001e	WARNING:	*/
	0,		/* 0x001f	WARNING:	*/
	0,		/* 0x0020	WARNING:	*/
	7,		/* 0x0021	WARNING:	*/
	6,		/* 0x0022	WARNING:	*/
	0		/* 0x0023-0xfffe    		*/
};

/* words 82-84: cmds/feats supported */
#define CMDS_W82		0x77ff  /* word 82: defined command locations*/
#define CMDS_W83		0x3fff  /* word 83: defined command locations*/
#define CMDS_W84		0x27ff  /* word 84: defined command locations*/
#define SUPPORT_48_BIT		0x0400  
#define NUM_CMD_FEAT_STR	48
const char *BuffType[4]		= {"unknown", "1Sect", "DualPort", "DualPortCache"};
static const char unknown[10] = "obsolete";
//static const char unknown[8] = "unknown";
#define unknown "unknown-"

static const char *feat_word69_str[16] = { 
	"CFast specification support",			/* word 69 bit 15 */
	"Deterministic read data after TRIM",		/* word 69 bit 14 */
	"Long physical sector diagnostics",		/* word 69 bit 13 */
	"DEVICE CONFIGURATION SET/IDENTIFY DMA commands", /* word 69 bit 12 */
	"READ BUFFER DMA command",			/* word 69 bit 11 */
	"WRITE BUFFER DMA command",			/* word 69 bit 10 */
	"SET MAX SETPASSWORD/UNLOCK DMA commands",	/* word 69 bit  9 */
	"DOWNLOAD MICROCODE DMA command",		/* word 69 bit  8 */
	"reserved 69[7]",				/* word 69 bit  7 */
	"reserved 69[6]",				/* word 69 bit  6 */
	"Deterministic read ZEROs after TRIM",		/* word 69 bit  5 */
	"reserved 69[4]",				/* word 69 bit  4 */
	"reserved 69[3]",				/* word 69 bit  3 */
	"reserved 69[2]",				/* word 69 bit  2 */
	"reserved 69[1]",				/* word 69 bit  1 */
	"reserved 69[0]",				/* word 69 bit  0 */
};

static const char *feat_word82_str[16] = { 
	"obsolete 82[15]",				/* word 82 bit 15: obsolete  */
	"NOP cmd",					/* word 82 bit 14 */
	"READ_BUFFER command",				/* word 82 bit 13 */
	"WRITE_BUFFER command",				/* word 82 bit 12 */
	"WRITE_VERIFY command",				/* word 82 bit 11: obsolete  */
	"Host Protected Area feature set",		/* word 82 bit 10 */
	"DEVICE_RESET command",				/* word 82 bit  9 */
	"SERVICE interrupt",				/* word 82 bit  8 */
	"Release interrupt",				/* word 82 bit  7 */
	"Look-ahead",					/* word 82 bit  6 */
	"Write cache",					/* word 82 bit  5 */
	"PACKET command feature set",			/* word 82 bit  4 */
	"Power Management feature set",			/* word 82 bit  3 */
	"Removable Media feature set",			/* word 82 bit  2 */
	"Security Mode feature set",			/* word 82 bit  1 */
	"SMART feature set"				/* word 82 bit  0 */
};
static const char *feat_word83_str[16] = { 
	NULL,						/* word 83 bit 15: !valid bit */
	NULL,						/* word 83 bit 14:  valid bit */
	"FLUSH_CACHE_EXT",				/* word 83 bit 13 */
	"Mandatory FLUSH_CACHE",			/* word 83 bit 12 */
	"Device Configuration Overlay feature set",	/* word 83 bit 11 */
	"48-bit Address feature set",			/* word 83 bit 10 */
	"Automatic Acoustic Management feature set",	/* word 83 bit  9 */
	"SET_MAX security extension",			/* word 83 bit  8 */
	"Address Offset Reserved Area Boot",		/* word 83 bit  7 */
	"SET_FEATURES required to spinup after power up",/* word 83 bit 6 */
	"Power-Up In Standby feature set",		/* word 83 bit  5 */
	"Removable Media Status Notification feature set",/* word 83 bit 4 */
	"Advanced Power Management feature set",	/* word 83 bit  3 */
	"CFA feature set",				/* word 83 bit  2 */
	"READ/WRITE_DMA_QUEUED",			/* word 83 bit  1 */
	"DOWNLOAD_MICROCODE"				/* word 83 bit  0 */
};
static const char *feat_word84_str[16] = { 
	NULL,						/* word 84 bit 15: !valid bit */
	NULL,						/* word 84 bit 14:  valid bit */
	"IDLE_IMMEDIATE with UNLOAD",			/* word 84 bit 13 */
	"Command Completion Time Limit (CCTL)",		/* word 84 bit 12 (ref: dt1696) */
	"Time Limited Commands (TLC) feature set",	/* word 84 bit 11 (ref: dt1696) */
	"URG for WRITE_STREAM[_DMA]_EXT",		/* word 84 bit 10 */
	"URG for READ_STREAM[_DMA]_EXT",		/* word 84 bit  9 */
	"64-bit World wide name",			/* word 84 bit  8 */
	"WRITE_DMA_QUEUED_FUA_EXT",			/* word 84 bit  7 */
	"WRITE_{DMA|MULTIPLE}_FUA_EXT",			/* word 84 bit  6 */
	"General Purpose Logging feature set",		/* word 84 bit  5 */
	"Media Card Pass-Through",			/* word 84 bit  4 */
	"Media Card Pass Through Command feature set",	/* word 84 bit  3 */
	"Media serial number",				/* word 84 bit  2 */
	"SMART self-test",				/* word 84 bit  1 */
	"SMART error logging"				/* word 84 bit  0 */
};
static const char *feat_3_str[16] = { 
	NULL,						/* word 119 bit 15: !valid bit */
	NULL,						/* word 119 bit 14:  valid bit */
	"unknown 119[13]",				/* word 119 bit 13 */
	"unknown 119[12]",				/* word 119 bit 12 */
	"unknown 119[11]",				/* word 119 bit 11 */
	"unknown 119[10]",				/* word 119 bit 10 */
	"unknown 119[9]",				/* word 119 bit  9 */
	"unknown 119[8]",				/* word 119 bit  8 */
	"unknown 119[7]",				/* word 119 bit  7 */
	"unknown 119[6]",				/* word 119 bit  6 */
	"Free-fall Control feature set",		/* word 119 bit  5 */
	"Segmented DOWNLOAD_MICROCODE",			/* word 119 bit  4 */
	"{READ,WRITE}_DMA_EXT_GPL commands",		/* word 119 bit  3 */
	"WRITE_UNCORRECTABLE_EXT command",		/* word 119 bit  2 */
	"Write-Read-Verify feature set",		/* word 119 bit  1 */
	"Disable Data Transfer After Error Detection"	/* word 119 bit  0 (ref: 2014DT)*/
};
static const char *cap_sata0_str[16] = { 
	"READ_LOG_DMA_EXT equivalent to READ_LOG_EXT",	/* word 76 bit 15 */
	"Device automatic Partial to Slumber transitions",/* word 76 bit 14 */
	"Host automatic Partial to Slumber transitions",/* word 76 bit 13 */
	"NCQ priority information",			/* word 76 bit 12 */
	"Idle-Unload when NCQ is active",		/* word 76 bit 11 */
	"Phy event counters",				/* word 76 bit 10 */
	"Host-initiated interface power management",	/* word 76 bit  9 */
	"Native Command Queueing (NCQ)",		/* word 76 bit  8 */
	"unknown 76[7]",				/* word 76 bit  7 */
	"unknown 76[6]",				/* word 76 bit  6 */
	"unknown 76[5]",				/* word 76 bit  5 */
	"unknown 76[4]",				/* word 76 bit  4 */
	"Gen3 signaling speed (6.0Gb/s)",		/* word 76 bit  3 */
	"Gen2 signaling speed (3.0Gb/s)",		/* word 76 bit  2 */
	"Gen1 signaling speed (1.5Gb/s)",		/* word 76 bit  1 */
	"unknown 76[0]"					/* word 76 bit  0 */
};
static const char *feat_sata0_str[16] = {
	"unknown 78[15]",				/* word 78 bit 15 */
	"unknown 78[14]",				/* word 78 bit 14 */
	"unknown 78[13]",				/* word 78 bit 13 */
	"unknown 78[12]",				/* word 78 bit 12 */
	"unknown 78[11]",				/* word 78 bit 11 */
	"unknown 78[10]",				/* word 78 bit 10 */
	"unknown 78[9]",				/* word 78 bit  9 */
	"Device Sleep (DEVSLP)",			/* word 78 bit  8 */
	"unknown 78[7]",				/* word 78 bit  7 */
	"Software settings preservation",		/* word 78 bit  6 */
	"Asynchronous notification (eg. media change)",	/* word 78 bit  5 */
	"In-order data delivery",			/* word 78 bit  4 */
	"Device-initiated interface power management",	/* word 78 bit  3 */
	"DMA Setup Auto-Activate optimization",		/* word 78 bit  2 */
	"Non-Zero buffer offsets in DMA Setup FIS",	/* word 78 bit  1 */
	"unknown 78[0]"					/* word 78 bit  0 */
};

/* words 85-87: cmds/feats enabled */
/* use cmd_feat_str[] to display what commands and features have
 * been enabled with words 85-87 
 */
#define WWN_SUP         0x100 /* 1=support; 0=not supported */

/* words 89, 90, SECU ERASE TIME */
#define ERASE_BITS		0x00ff

/* word 92: master password revision */
/* 0x0000 or  0xffff means no support for master password revision */

/* word 93: hw reset result */
#define CBLID			0x2000  /* CBLID status */
#define RST0			0x0001  /* 1=reset to device #0 */
#define DEV_DET			0x0006  /* how device num determined */
#define JUMPER_VAL		0x0002  /* device num determined by jumper */
#define CSEL_VAL		0x0004  /* device num determined by CSEL_VAL */

/* word 127: removable media status notification feature set support */
#define RM_STAT_BITS 		0x0003
#define RM_STAT_SUP		0x0001
	
/* word 128: security */
#define SECU_ENABLED		0x0002
#define SECU_LEVEL		0x0100	/* was 0x0010 */
#define NUM_SECU_STR		6
const char *secu_str[] = {
	"supported",			/* word 128, bit 0 */
	"enabled",			/* word 128, bit 1 */
	"locked",			/* word 128, bit 2 */
	"frozen",			/* word 128, bit 3 */
	"expired: security count",	/* word 128, bit 4 */
	"supported: enhanced erase"	/* word 128, bit 5 */
};

/* word 160: CFA power mode */
#define VALID_W160		0x8000  /* 1=word valid */
#define PWR_MODE_REQ		0x2000  /* 1=CFA power level 1 is NOT supported */
#define PWR_MODE_OFF		0x1000  /* 1=CFA power level 1 commands are DISABLED */
#define MAX_AMPS		0x0fff  /* value = max current in milli-amperes (mA) */

// extern int do_read_sector (int fd, __u64 lba, const char *devname);
 // extern int do_write_sector (int fd, __u64 lba, const char *devname);
/* word 206: SMART command transport (SCT) */
static const char *feat_sct_str[16] = {
	"unknown 206[15] (vendor specific)",		/* word 206 bit 15 */
	"unknown 206[14] (vendor specific)",		/* word 206 bit 14 */
	"unknown 206[13] (vendor specific)",		/* word 206 bit 13 */
	"unknown 206[12] (vendor specific)",		/* word 206 bit 12 */
	"unknown 206[11]",				/* word 206 bit 11 */
	"unknown 206[10]",				/* word 206 bit 10 */
	"unknown 206[9]",				/* word 206 bit  9 */
	"unknown 206[8]",				/* word 206 bit  8 */
	"unknown 206[7]",				/* word 206 bit  7 */
	"unknown 206[6]",				/* word 206 bit  6 */
	"SCT Data Tables (AC5)",			/* word 206 bit  5 */
	"SCT Features Control (AC4)",			/* word 206 bit  4 */
	"SCT Error Recovery Control (AC3)",		/* word 206 bit  3 */
	"SCT Write Same (AC2)",				/* word 206 bit  2 */
	"SCT Read/Write Long (AC1), obsolete",		/* word 206 bit  1: obsolete per T13/e08153r1 */
	"SMART Command Transport (SCT) feature set"	/* word 206 bit  0 */
};

/* word 255: integrity */
#define SIG			0x00ff  /* signature location */
#define SIG_VAL			0x00A5  /* signature value */


class FrameWorkSata:public Storage
{

public:
__u64 MaxLBA;
int countTests, passedTests, failedTests;
int ErrorCode=0;
virtual int Init(char **argv)=0;
virtual int ReadSector(__u64 val)=0;
virtual int WriteSector(__u64 val)=0;
virtual int WriteCache(int enable)=0;
virtual int Identify(void)=0;

};


class SataStorage: public FrameWorkSata
{

public:
	char arg[20];
	int identifier;
	char arg2[20];

     public:
			
			 char *argv;
			 char  *argp;
    
		void open_dev(const char *dev)
		{
			int err;
			assert(dev != NULL);
			devicename = basename(dev);
			::fd = open(dev, open_flags);
			if (::fd < 0)
				goto perror;
		/*
			err = fstat(fd, &nvme_stat);
			if (err < 0)
				goto perror;
			if (!S_ISCHR(nvme_stat.st_mode) && !S_ISBLK(nvme_stat.st_mode)) {
				fprintf(stderr, "%s is not a block or character device\n", dev);
				exit(ENODEV);
			}*/
			return;
		 perror:
		 	ErrorCode=1;
			perror(dev);
			exit(errno);
		}
       
        virtual int Init(char **argv)
        {
//           strcpy(arg, argv[0]);
           assert(argv[0]!= NULL);
           std::cout << fd << std::endl;  

	   open_dev(argv[0]);

	   std::cout << fd << std::endl;  

            return ErrorCode;
        }

        virtual int Identify(void)
        {
	  	 return EntryPoint ("I", 0);
        }
		virtual int ReadSector(__u64 val)
			{
			 assert (val <= MaxLBA);
                    return EntryPoint ("r", val);

			}
		virtual int WriteSector(__u64 val)
			{
			assert ((val <=MaxLBA) && (val > 0));
                    return EntryPoint ("w", val);

			}
		virtual int WriteCache(int enable)
			{
			assert (enable ==1 || enable == 0);
                    return  EntryPoint ("WC", enable);


			}
       int EntryPoint (char const *arg1, __u64 i);
	   int process_dev ();
	   void identify (int fd, __u16 *id_supplied);
	   __u64 tf_to_lba (struct ata_tf *tf);
	   void on_off (unsigned int value);
	   int sg16 (int fd, int rw, int dma, struct ata_tf *tf,
	void *data, unsigned int data_bytes, unsigned int timeout_secs);
	  int print_transport_type(__u16 val[]);
	  void print_features (__u16 supported, __u16 enabled, const char *names[]);
	  void print_ascii(__u16 *p, unsigned int length);
	  int is_dma (__u8 ata_op);
	  __u64 get_lba_capacity (__u16 *idw);
	  void dump_identity (__u16 *idw);
	  int flush_wcache (int fd);
	  void dump_bytes (const char *prefix, unsigned char *p, int len);
	  __u8 mode_loop(__u16 mode_sup, __u16 mode_sel, int cc, __u8 *have_mode);
	  void get_identify_data (int fd);
	 int do_read_sector ( __u64 lba, const char *devname);
	 int do_write_sector ( __u64 lba, const char *devname);
	 int do_drive_cmd (int fd, unsigned char *args, unsigned int timeout_secs);
	 void tf_init (struct ata_tf *tf, __u8 ata_op, __u64 lba, unsigned int nsect);
	 int needs_lba48 (__u8 ata_op, __u64 lba, unsigned int nsect);
	 void dump_sectors (__u16 *w, unsigned int count);
	 
	 void init_hdio_taskfile (struct hdio_taskfile *r, __u8 ata_op, int rw, int force_lba48,
					 __u64 lba, unsigned int nsect, int data_bytes);
	 int do_taskfile_cmd (int fd, struct hdio_taskfile *r, unsigned int timeout_secs);
	 void numeric_parm (char c, const char *name, int *val, int *setparm, int *getparm, int min, int max, int set_only);
};



extern "C" {
    FrameWorkSata* SataStorage_new(){ return new SataStorage(); }
    int SataStorage_Init(FrameWorkSata* FrameWorkSata, char *ptr){ 
	//	char *ptr="/dev/sda";
//           char *ptr1=NULL; 
           return FrameWorkSata->Init(&ptr); }
    int SataStorage_Identify(FrameWorkSata* FrameWorkSata){ 
	//	char *ptr="/dev/sda";
//           char *ptr1=NULL; 
         return  FrameWorkSata->Identify(); }
 int ReadSector(FrameWorkSata* FrameWorkSata, __u64 val){ 
	//	char *ptr="/dev/sda";
//           char *ptr1=NULL; 
          return FrameWorkSata->ReadSector(val); }

int WriteSector(FrameWorkSata* FrameWorkSata, __u64 val){ 
	//	char *ptr="/dev/sda";
//           char *ptr1=NULL; 
          return FrameWorkSata->WriteSector(val); }
int WriteCache(FrameWorkSata* FrameWorkSata, int val){ 
	//	char *ptr="/dev/sda";
//           char *ptr1=NULL; 
           return FrameWorkSata->WriteCache(val); }


}

extern "C" {
    NvmeStorage* NvmeStorage_new(){ return new NvmeStorage(); }
    void NvmeStorage_Init(NvmeStorage* NvmeStorage1, char *ptr){
		//char *ptr="/dev/nvme0";
		NvmeStorage1->Init(&ptr); }
	int NvmeStorage_Identify(NvmeStorage* NvmeStorage1){
		//char *ptr="/dev/nvme0";
		NvmeStorage1->nvme_identify_ctrl(&NvmeStorage1->ctrl);
		return NvmeStorage1->show_nvme_id_ctrl_(&NvmeStorage1->ctrl,0);
			}
	int NvmeStorage_list(NvmeStorage* NvmeStorage1){
		//char *ptr="/dev/nvme0";
		return NvmeStorage1->nvme_list();
		//NvmeStorage1->show_nvme_id_ctrl_(&NvmeStorage1->ctrl,0);
			}
	void NvmeStorage_Read(NvmeStorage* NvmeStorage1){
		//char *ptr="/dev/nvme0";
			   int count = 0;
	//	while (count < argc)	{
	//	printf("\n arg [%d] = [%s]\n",count+1,argv);
	//	count++;
	//	}
	#if 0	
	strcpy(NvmeStorage1->argv [0],"read");
	
	strcpy(NvmeStorage1->argv [1],"/dev/nvme0n1");
	
	strcpy(NvmeStorage1->argv [2],"-s");
	
	strcpy(NvmeStorage1->argv [3], "3");
	
	strcpy(NvmeStorage1->argv [4], "-c");
	
	strcpy(NvmeStorage1->argv [5],"1");
	
	strcpy(NvmeStorage1->argv [6], "-z");
	
	strcpy(NvmeStorage1->argv [7], "4096");
	#endif
		printf("Before Open_dev %d\r\n",fd);
		NvmeStorage1->open_dev(NvmeStorage1->argv [1]);
	printf("After Open_dev %d\r\n",fd);
		NvmeStorage1->read_cmd(8, NvmeStorage1->argv);
		
		//NvmeStorage1->show_nvme_id_ctrl_(&NvmeStorage1->ctrl,0);
			}
	void NvmeStorage_Write(NvmeStorage* NvmeStorage1,int argc, char **argv){
		//char *ptr="/dev/nvme0";
		NvmeStorage1->write_cmd(argc, argv);
		//NvmeStorage1->show_nvme_id_ctrl_(&NvmeStorage1->ctrl,0);
			}
	void NvmeStorage_Compare(NvmeStorage* NvmeStorage1,int argc, char **argv){
		//char *ptr="/dev/nvme0";
		NvmeStorage1->compare(argc, argv);
		//NvmeStorage1->show_nvme_id_ctrl_(&NvmeStorage1->ctrl,0);
			}
	
}



//extern struct program  nvme;
void SataStorage::numeric_parm (char c, const char *name, int *val, int *setparm, int *getparm, int min, int max, int set_only)
{
	int got_digit = 0;

	//*val = 0;
	*getparm = noisy;
	noisy = 1;
	//if (!*argp && argc && isdigit(*argv))
		//argp = argv++, --argc;
	//while (isdigit(*argp)) {
		*setparm = 1;
	//	*val = (*val * 10) + (*argp++ - '0');
	//	got_digit = 1;
	//}
//	if ((set_only && !got_digit) || *val < min || *val > max) {
	//	fprintf(stderr, "  -%c: bad/missing %s value (%d..%d)\n", c, name, min, max);
	//	exit(EINVAL);
	//}
}

#define NUMERIC_PARM(CH,NAME,VAR,MIN,MAX,GETSET) numeric_parm(CH,NAME,&VAR,&::set_##VAR,&::get_##VAR,MIN,MAX,GETSET)
#define GET_SET_PARM(CH,NAME,VAR,MIN,MAX) CH:NUMERIC_PARM(CH,NAME,VAR,MIN,MAX,0);break
#define     SET_PARM(CH,NAME,VAR,MIN,MAX) CH:NUMERIC_PARM(CH,NAME,VAR,MIN,MAX,1);break
#define     SET_FLAG1(VAR)                get_##VAR=noisy;noisy=1;set_##VAR=1
#define     SET_FLAG(CH,VAR)              CH:SET_FLAG1(VAR);break
#define      DO_FLAG(CH,VAR)              CH:VAR=1;noisy=1;break
#define    INCR_FLAG(CH,VAR)              CH:VAR++;noisy=1;break

#if 0
static void print_devslp_info (int fd, __u16 *id)
{
	/* Print DEVSLP information */
	if (id[78] & 0x0100) {
		__u8 buf[512];
		int deto = 0;
		int mdat = 0;

		memset(buf, 0, 512);
		if (fd != -1 && !get_id_log_page_data(fd, 8, buf) && (buf[0x37] & 0x80)) {
			mdat = buf[0x30] & 0x1f;
			deto = buf[0x31];
			printf("Device Sleep:\n");
			printf("\tDEVSLP Exit Timeout (DETO): %d ms (%s)\n", deto?deto:20, deto?"drive":"default");
			printf("\tMinimum DEVSLP Assertion Time (MDAT): %d ms (%s)\n", mdat?mdat:10, deto?"drive":"default");
		}
	}
}
#endif

#ifdef SG_IO

__u64 SataStorage::tf_to_lba (struct ata_tf *tf)
{
	__u32 lba24, lbah;
	__u64 lba64;

	lba24 = (tf->lob.lbah << 16) | (tf->lob.lbam << 8) | (tf->lob.lbal);
	if (tf->is_lba48)
		lbah = (tf->hob.lbah << 16) | (tf->hob.lbam << 8) | (tf->hob.lbal);
	else
		lbah = (tf->dev & 0x0f);
	lba64 = (((__u64)lbah) << 24) | (__u64)lba24;
	return lba64;
}

enum {
	SG_CDB2_TLEN_NODATA	= 0 << 0,
	SG_CDB2_TLEN_FEAT	= 1 << 0,
	SG_CDB2_TLEN_NSECT	= 2 << 0,

	SG_CDB2_TLEN_BYTES	= 0 << 2,
	SG_CDB2_TLEN_SECTORS	= 1 << 2,

	SG_CDB2_TDIR_TO_DEV	= 0 << 3,
	SG_CDB2_TDIR_FROM_DEV	= 1 << 3,

	SG_CDB2_CHECK_COND	= 1 << 5,
};

 void SataStorage:: on_off (unsigned int value)
{
	printf(value ? " (on)\n" : " (off)\n");
}

 void SataStorage:: dump_bytes (const char *prefix, unsigned char *p, int len)
{
	int i;

	if (prefix)
		fprintf(stderr, "%s: ", prefix);
	for (i = 0; i < len; ++i)
		fprintf(stderr, " %02x", p[i]);
	fprintf(stderr, "\n");
}

int SataStorage::sg16 (int fd, int rw, int dma, struct ata_tf *tf,
	void *data, unsigned int data_bytes, unsigned int timeout_secs)
{
	unsigned char cdb[SG_ATA_16_LEN];
	unsigned char sb[32], *desc;
	struct scsi_sg_io_hdr io_hdr;
	int prefer12 = prefer_ata12, demanded_sense = 0;

	if (tf->command == ATA_OP_PIDENTIFY)
		prefer12 = 0;

	memset(&cdb, 0, sizeof(cdb));
	memset(&sb,     0, sizeof(sb));
	memset(&io_hdr, 0, sizeof(struct scsi_sg_io_hdr));
	if (data && data_bytes && !rw)
		memset(data, 0, data_bytes);

	if (dma) {
		//cdb[1] = data ? (rw ? SG_ATA_PROTO_UDMA_OUT : SG_ATA_PROTO_UDMA_IN) : SG_ATA_PROTO_NON_DATA;
		cdb[1] = data ? SG_ATA_PROTO_DMA : SG_ATA_PROTO_NON_DATA;
	} else {
		cdb[1] = data ? (rw ? SG_ATA_PROTO_PIO_OUT : SG_ATA_PROTO_PIO_IN) : SG_ATA_PROTO_NON_DATA;
	}

	/* libata/AHCI workaround: don't demand sense data for IDENTIFY commands */
	if (data) {
		cdb[2] |= SG_CDB2_TLEN_NSECT | SG_CDB2_TLEN_SECTORS;
		cdb[2] |= rw ? SG_CDB2_TDIR_TO_DEV : SG_CDB2_TDIR_FROM_DEV;
	} else {
		cdb[2] = SG_CDB2_CHECK_COND;
	}

	if (!prefer12 || tf->is_lba48) {
		cdb[ 0] = SG_ATA_16;
		cdb[ 4] = tf->lob.feat;
		cdb[ 6] = tf->lob.nsect;
		cdb[ 8] = tf->lob.lbal;
		cdb[10] = tf->lob.lbam;
		cdb[12] = tf->lob.lbah;
		cdb[13] = tf->dev;
		cdb[14] = tf->command;
		if (tf->is_lba48) {
			cdb[ 1] |= SG_ATA_LBA48;
			cdb[ 3]  = tf->hob.feat;
			cdb[ 5]  = tf->hob.nsect;
			cdb[ 7]  = tf->hob.lbal;
			cdb[ 9]  = tf->hob.lbam;
			cdb[11]  = tf->hob.lbah;
		}
		io_hdr.cmd_len = SG_ATA_16_LEN;
	} else {
		cdb[ 0] = SG_ATA_12;
		cdb[ 3] = tf->lob.feat;
		cdb[ 4] = tf->lob.nsect;
		cdb[ 5] = tf->lob.lbal;
		cdb[ 6] = tf->lob.lbam;
		cdb[ 7] = tf->lob.lbah;
		cdb[ 8] = tf->dev;
		cdb[ 9] = tf->command;
		io_hdr.cmd_len = SG_ATA_12_LEN;
	}

	io_hdr.interface_id	= 'S';
	io_hdr.mx_sb_len	= sizeof(sb);
	io_hdr.dxfer_direction	= data ? (rw ? SG_DXFER_TO_DEV : SG_DXFER_FROM_DEV) : SG_DXFER_NONE;
	io_hdr.dxfer_len	= data ? data_bytes : 0;
	io_hdr.dxferp		= data;
	io_hdr.cmdp		= cdb;
	io_hdr.sbp		= sb;
	io_hdr.pack_id		= tf_to_lba(tf);
	io_hdr.timeout		= (timeout_secs ? timeout_secs : default_timeout_secs) * 1000; /* msecs */

	if (verbose) {
		dump_bytes("outgoing cdb", cdb, sizeof(cdb));
		if (rw && data)
			dump_bytes("outgoing_data",(unsigned char *) data, data_bytes);
	}

	if (ioctl(fd, SG_IO, &io_hdr) == -1) {
		if (verbose)
			perror("ioctl(fd,SG_IO)");
		return -1;	/* SG_IO not supported */
	}

	if (verbose)
		fprintf(stderr, "SG_IO: ATA_%u status=0x%x, host_status=0x%x, driver_status=0x%x\n",
			io_hdr.cmd_len, io_hdr.status, io_hdr.host_status, io_hdr.driver_status);

	if (io_hdr.status && io_hdr.status != SG_CHECK_CONDITION) {
		if (verbose)
			fprintf(stderr, "SG_IO: bad status: 0x%x\n", io_hdr.status);
	  	errno = EBADE;
		return -1;
	}
	if (io_hdr.host_status) {
		if (verbose)
			fprintf(stderr, "SG_IO: bad host status: 0x%x\n", io_hdr.host_status);
	  	errno = EBADE;
		return -1;
	}
	if (verbose) {
		dump_bytes("SG_IO: sb[]", sb, sizeof(sb));
		if (!rw && data)
			dump_bytes("incoming_data", (unsigned char *)data, data_bytes);
	}

	if (io_hdr.driver_status && (io_hdr.driver_status != SG_DRIVER_SENSE)) {
		if (verbose)
			fprintf(stderr, "SG_IO: bad driver status: 0x%x\n", io_hdr.driver_status);
	  	errno = EBADE;
		return -1;
	}

	desc = sb + 8;
	if (io_hdr.driver_status != SG_DRIVER_SENSE) {
		if (sb[0] | sb[1] | sb[2] | sb[3] | sb[4] | sb[5] | sb[6] | sb[7] | sb[8] | sb[9]) {
			static int second_try = 0;
			if (!second_try++)
				fprintf(stderr, "SG_IO: questionable sense data, results may be incorrect\n");
		} else if (demanded_sense) {
			static int second_try = 0;
			if (!second_try++)
				fprintf(stderr, "SG_IO: missing sense data, results may be incorrect\n");
		}
	} else if (sb[0] != 0x72 || sb[7] < 14 || desc[0] != 0x09 || desc[1] < 0x0c) {
		dump_bytes("SG_IO: bad/missing sense data, sb[]", sb, sizeof(sb));
	}

	if (verbose) {
		unsigned int len = desc[1] + 2, maxlen = sizeof(sb) - 8 - 2;
		if (len > maxlen)
			len = maxlen;
		dump_bytes("SG_IO: desc[]", desc, len);
	}

	tf->is_lba48  = desc[ 2] & 1;
	tf->error     = desc[ 3];
	tf->lob.nsect = desc[ 5];
	tf->lob.lbal  = desc[ 7];
	tf->lob.lbam  = desc[ 9];
	tf->lob.lbah  = desc[11];
	tf->dev       = desc[12];
	tf->status    = desc[13];
	tf->hob.feat  = 0;
	if (tf->is_lba48) {
		tf->hob.nsect = desc[ 4];
		tf->hob.lbal  = desc[ 6];
		tf->hob.lbam  = desc[ 8];
		tf->hob.lbah  = desc[10];
	} else {
		tf->hob.nsect = 0;
		tf->hob.lbal  = 0;
		tf->hob.lbam  = 0;
		tf->hob.lbah  = 0;
	}

	if (verbose)
		fprintf(stderr, "      ATA_%u stat=%02x err=%02x nsect=%02x lbal=%02x lbam=%02x lbah=%02x dev=%02x\n",
				io_hdr.cmd_len, tf->status, tf->error, tf->lob.nsect, tf->lob.lbal, tf->lob.lbam, tf->lob.lbah, tf->dev);

	if (tf->status & (ATA_STAT_ERR | ATA_STAT_DRQ)) {
		if (verbose) {
			fprintf(stderr, "I/O error, ata_op=0x%02x ata_status=0x%02x ata_error=0x%02x\n",
				tf->command, tf->status, tf->error);
		}
		errno = EIO;
		return -1;
	}
	return 0;
}

#endif /* SG_IO */
static __u16 min_ata_std (__u16 major)
{
	if (major <= 4)		// up to ata4, no obsolete bits
		return 1;
	if (major == 5)		// ata5 obsoleted the ata1 bit
		return 2;
	if (major <= 7)		// ata6,7 obsoleted the ata2 bit
		return 3;
	return 4;		// ata8 obsoleted the ata3 bit
}

__u8 SataStorage::mode_loop(__u16 mode_sup, __u16 mode_sel, int cc, __u8 *have_mode) {
	__u16 ii;
	__u8 err_dma = 0;
	for (ii = 0; ii <= MODE_MAX; ii++) {
		if(mode_sel & 0x0001) {
			printf("*%cdma%u ",cc,ii);
			if(*have_mode) err_dma = 1;
			*have_mode = 1;
		} else if(mode_sup & 0x0001) {
			printf("%cdma%u ",cc,ii);
		}
		mode_sup >>=1;   mode_sel >>=1;
	}
	return err_dma;
}
 int SataStorage::print_transport_type(__u16 val[])
{
	__u16 major = val[TRANSPORT_MAJOR], minor = val[TRANSPORT_MINOR];
	unsigned int ttype, subtype, transport = 0;

	if (major == 0x0000 || major == 0xffff) {
#if 0
		printf("\t%-20snot reported","Transport:");
		if ((val[SATA_CAP_0]  && val[SATA_CAP_0]  != 0xffff)
		 || (val[SATA_SUPP_0] && val[SATA_SUPP_0] != 0xffff)) {
			printf(" (serial)");
		}
		putchar('\n');
#endif
		return transport;
	}
	printf("\t%-20s","Transport:");
	ttype = major >> 12;
	subtype = major & 0xfff;
	transport = ttype;
	switch (ttype) {
		case 0:
			printf("Parallel");
			if (subtype & 1)
				printf(", ATA8-APT");
			break;
		case 1:
			printf("Serial");
			if (subtype & 0x2f) {
				if (subtype & (1<<0))
					printf(", ATA8-AST");
				if (subtype & (1<<1))
					printf(", SATA 1.0a");
				if (subtype & (1<<2))
					printf(", SATA II Extensions");
				if (subtype & (1<<3))
					printf(", SATA Rev 2.5");
				if (subtype & (1<<4))
					printf(", SATA Rev 2.6");
				if (subtype & (1<<5))
					printf(", SATA Rev 3.0");
			}
			break;
		default:
			printf("0x%04x", major);
			break;
	}
	if (minor != 0x0000 && minor != 0xffff) {
		printf("; Revision: ");
		switch (minor) {
			case 0x21:
				printf("ATA8-AST T13 Project D1697 Revision 0b");
				break;
			default:
				printf("0x%04x", minor);
		}
	}
	putchar('\n');
	return transport;
}
 void SataStorage::print_features (__u16 supported, __u16 enabled, const char *names[])
{
	int i;
	for (i = 0; i < 16; ++i) {
		__u16 mask = 1 << i;
		if ((supported & mask) && names[15 - i])
			printf("\t   %c\t%s\n", (enabled & mask) ? '*' : ' ', names[15 - i]);
	}
}
 void SataStorage::print_ascii(__u16 *p, unsigned int length) {
	__u8 ii;
	char cl;

	/* find first non-space & print it */
	for (ii = 0; ii< length; ii++) {
		if(((char) 0x00ff&((*p)>>8)) != ' ') break;
		if((cl = (char) 0x00ff&(*p)) != ' ') {
			if(cl != '\0') printf("%c",cl);
			p++; ii++;
			break;
		}
		p++;
	}
	/* print the rest */
	for (; ii < length; ii++) {
		__u8 c;
		/* some older devices have NULLs */
		c = (*p) >> 8;
		if (c) putchar(c);
		c = (*p);
		if (c) putchar(c);
		p++;
	}
	printf("\n");
}
 inline int SataStorage:: is_dma (__u8 ata_op)
{
	switch (ata_op) {
		case ATA_OP_DSM:
		case ATA_OP_READ_DMA_EXT:
		case ATA_OP_READ_FPDMA:
		case ATA_OP_WRITE_DMA_EXT:
		case ATA_OP_WRITE_FPDMA:
		case ATA_OP_READ_DMA:
		case ATA_OP_WRITE_DMA:
			return SG_DMA;
		default:
			return SG_PIO;
	}
}


 void dmpstr (const char *prefix, unsigned int i, const char *s[], unsigned int maxi)
{
	if (i > maxi)
		printf("%s%u", prefix, i);
	else
		printf("%s%s", prefix, s[i]);
}
 char *strip (char *s)
{
	char *e;

	while (*s == ' ') ++s;
	if (*s)
		for (e = s + strlen(s); *--e == ' '; *e = '\0');
	return s;
}

 __u64 SataStorage::get_lba_capacity (__u16 *idw)
{
	__u64 nsects = ((__u32)idw[58] << 16) | idw[57];

	if (idw[49] & 0x200) {
		nsects = ((__u32)idw[61] << 16) | idw[60];
		if ((idw[83] & 0xc000) == 0x4000 && (idw[86] & 0x0400)) {
			nsects = (__u64)idw[103] << 48 | (__u64)idw[102] << 32 |
			         (__u64)idw[101] << 16 | idw[100];
		}
	}
	return nsects;
}

 void SataStorage::dump_identity (__u16 *idw)
{
	int i;
	char pmodes[64] = {0,}, dmodes[128]={0,}, umodes[128]={0,};
	char *model = strip(strndup((char *)&idw[27], 40));
	char *fwrev = strip(strndup((char *)&idw[23],  8));
	char *serno = strip(strndup((char *)&idw[10], 20));
	__u8 tPIO;

	printf("\n Model=%.40s, FwRev=%.8s, SerialNo=%.20s", model, fwrev, serno);
	printf("\n Config={");
	for (i = 0; i <= 15; i++) {
		if (idw[0] & (1<<i))
			printf("%s", cfg_str[i]);
	}
	printf(" }\n");
	printf(" RawCHS=%u/%u/%u, TrkSize=%u, SectSize=%u, ECCbytes=%u\n",
		idw[1], idw[3], idw[6], idw[4], idw[5], idw[22]);
	dmpstr(" BuffType=", idw[20], BuffType, 3);
	if (idw[21] && idw[21] != 0xffff)
		printf(", BuffSize=%ukB", idw[21] / 2);
	else
		printf(", BuffSize=unknown");
	printf(", MaxMultSect=%u", idw[47] & 0xff);
	if ((idw[47] & 0xff)) {
		printf(", MultSect=");
		if (!(idw[59] & 0x100))
			printf("?%u?", idw[59] & 0xff);
		else if (idw[59] & 0xff)
			printf("%u", idw[59] & 0xff);
		else
			printf("off");
	}
	putchar('\n');
	tPIO = idw[51] >> 8;
	if (tPIO <= 5) {
		strcat(pmodes, "pio0 ");
		if (tPIO >= 1) strcat(pmodes, "pio1 ");
		if (tPIO >= 2) strcat(pmodes, "pio2 ");
	}
	if (!(idw[53] & 1))
		printf(" (maybe):");
	printf(" CurCHS=%u/%u/%u, CurSects=%u", idw[54], idw[55], idw[56], idw[57] | (idw[58] << 16));
	printf(", LBA=%s", YN(idw[49] & 0x200));
	if (idw[49] & 0x200)
 		printf(", LBAsects=%llu", get_lba_capacity(idw));

	if (idw[49] & 0x100) {
		if (idw[62] | idw[63]) {
			if (idw[62] & 0x100)	strcat(dmodes,"*");
			if (idw[62] & 1)	strcat(dmodes,"sdma0 ");
			if (idw[62] & 0x200)	strcat(dmodes,"*");
			if (idw[62] & 2)	strcat(dmodes,"sdma1 ");
			if (idw[62] & 0x400)	strcat(dmodes,"*");
			if (idw[62] & 4)	strcat(dmodes,"sdma2 ");
			if (idw[62] & 0xf800)	strcat(dmodes,"*");
			if (idw[62] & 0xf8)	strcat(dmodes,"sdma? ");
			if (idw[63] & 0x100)	strcat(dmodes,"*");
			if (idw[63] & 1)	strcat(dmodes,"mdma0 ");
			if (idw[63] & 0x200)	strcat(dmodes,"*");
			if (idw[63] & 2)	strcat(dmodes,"mdma1 ");
			if (idw[63] & 0x400)	strcat(dmodes,"*");
			if (idw[63] & 4)	strcat(dmodes,"mdma2 ");
			if (idw[63] & 0xf800)	strcat(dmodes,"*");
			if (idw[63] & 0xf8)	strcat(dmodes,"mdma? ");
		}
	}
	printf("\n IORDY=");
	if (idw[49] & 0x800)
		printf((idw[49] & 0x400) ? "on/off" : "yes");
	else
		printf("no");
	if ((idw[49] & 0x800) || (idw[53] & 2)) {
		if ((idw[53] & 2)) {
			printf(", tPIO={min:%u,w/IORDY:%u}", idw[67], idw[68]);
			if (idw[64] & 1)	strcat(pmodes, "pio3 ");
			if (idw[64] & 2)	strcat(pmodes, "pio4 ");
			if (idw[64] &~3)	strcat(pmodes, "pio? ");
		}
		if (idw[53] & 4) {
			if (idw[88] & 0x100)	strcat(umodes,"*");
			if (idw[88] & 0x001)	strcat(umodes,"udma0 ");
			if (idw[88] & 0x200)	strcat(umodes,"*");
			if (idw[88] & 0x002)	strcat(umodes,"udma1 ");
			if (idw[88] & 0x400)	strcat(umodes,"*");
			if (idw[88] & 0x004)	strcat(umodes,"udma2 ");
			if (idw[88] & 0x800)	strcat(umodes,"*");
			if (idw[88] & 0x008)	strcat(umodes,"udma3 ");
			if (idw[88] & 0x1000)	strcat(umodes,"*");
			if (idw[88] & 0x010)	strcat(umodes,"udma4 ");
			if (idw[88] & 0x2000)	strcat(umodes,"*");
			if (idw[88] & 0x020)	strcat(umodes,"udma5 ");
			if (idw[88] & 0x4000)	strcat(umodes,"*");
			if (idw[88] & 0x040)	strcat(umodes,"udma6 ");
		}
	}
	if ((idw[49] & 0x100) && (idw[53] & 2))
		printf(", tDMA={min:%u,rec:%u}", idw[65], idw[66]);
	printf("\n PIO modes:  %s", pmodes);
	if (*dmodes)
		printf("\n DMA modes:  %s", dmodes);
	if (*umodes)
		printf("\n UDMA modes: %s", umodes);

	printf("\n AdvancedPM=%s",YN(idw[83]&8));
	if (idw[83] & 8) {
		if (!(idw[86]&8))
			printf(": disabled (255)");
		else if ((idw[91]&0xFF00)!=0x4000)
			printf(": unknown setting");
		else
			printf(": mode=0x%02X (%u)",idw[91]&0xFF,idw[91]&0xFF);
	}
	if (idw[82]&0x20)
		printf(" WriteCache=%s",(idw[85]&0x20) ? "enabled" : "disabled");
	if (idw[81] || idw[80]) {
		printf("\n Drive conforms to: ");
		if (idw[81] <= 31)
			printf("%s: ", minor_str[idw[81]]);
		else
			printf("unknown: ");
		if (idw[80] != 0x0000 &&  /* NOVAL_0 */
		    idw[80] != 0xFFFF) {  /* NOVAL_1 */
			int count = 0;
			for (i=0; i <= 7; i++) {
				if (idw[80] & (1<<i))
					printf("%s%u", count++ ? "," : " ATA/ATAPI-", i);
			}
		}
	}
	printf("\n");
	printf("\n * signifies the current active mode\n");
	printf("\n");
}


int SataStorage::flush_wcache (int fd)
{
	__u8 args[4] = {ATA_OP_FLUSHCACHE,0,0,0};
	int err = 0;

	get_identify_data(fd);
	if (id && (id[83] & 0xe000) == 0x6000)
		args[0] = ATA_OP_FLUSHCACHE_EXT;
	if (do_drive_cmd(fd, args, ::timeout_60secs)) {
		err = errno;
		perror (" HDIO_DRIVE_CMD(flushcache) failed");
	}
	return err;
}

int SataStorage::process_dev ()
{
//	int fd;
	int err = 0;
	static long parm, multcount;

	::id = NULL;
//	fd = open(devname, ::open_flags);

	
	
    if (::do_identity) {
		__u16 id2[256];

		if (!ioctl(fd, HDIO_GET_IDENTITY, id2)) {
			if (multcount != -1) {
				id2[59] = multcount | 0x100;
			} else {
				id2[59] &= ~0x100;
			}
			dump_identity(id2);
		} else if (errno == -ENOMSG) {
			printf(" no identification info available\n");
		} else {
			err = errno;
			perror(" HDIO_GET_IDENTITY failed");
		}
    }
    
    if (::do_IDentity) {
		get_identify_data(fd);
		if (id) {
			if (::do_IDentity == 2)
	//			dump_sectors(id, 1);
;
			else
				identify(fd, (__u16 *)::id);
			do_IDentity=0;
		}
	}
    
    if (::read_sector)
	{
		err = do_read_sector( ::read_sector_addr, this->devicename);
		::read_sector=0;

    }
    
    if (::write_sector) {
	//	if (num_flags_processed > 1 || argc)
		//	usage_help(14,EINVAL);
		//confirm_i_know_what_i_am_doing("--write-sector", "You are trying to deliberately overwrite a low-level sector on the media.\nThis is a BAD idea, and can easily result in total data loss.");
		err = do_write_sector( ::write_sector_addr, this->devicename);
		::write_sector=0;	   
    }    
    
    if (::set_wcache ) {
		if (::get_wcache) {
			printf(" setting drive write-caching to %d", ::wcache);
			on_off(::wcache);
		}
		if (!::wcache)
			err = flush_wcache(fd);
		if (ioctl(fd, HDIO_SET_WCACHE, ::wcache)) {
			__u8 setcache[4] = {ATA_OP_SETFEATURES,0,0,0};
			setcache[2] = ::wcache ? 0x02 : 0x82;
			if (do_drive_cmd(fd, setcache, 0)) {
				err = errno;
				perror(" HDIO_DRIVE_CMD(setcache) failed");
			}
		}
		if (!::wcache)
			err = flush_wcache(fd);
		
	}
    
    if (::get_wcache ) {
		get_identify_data(fd);
		if (::id) {
			int supported = ::id[82] & 0x0020;
			if (supported) {
				::wcache = !!(::id[85] & 0x0020);
				printf(" write-caching = %2d", ::wcache);
				on_off(::wcache);
			} else {
				printf(" write-caching = not supported\n");
			}
		}
		
	}
       return err; 
    }
    
 void SataStorage::get_identify_data (int fd)
{
	static __u8 args[4+512];
	int i;

	if (id)
		return;
	memset(args, 0, sizeof(args));
	last_identify_op = ATA_OP_IDENTIFY;
	args[0] = last_identify_op;
	args[3] = 1;	/* sector count */
	if (do_drive_cmd(fd, args, 0)) {
		prefer_ata12 = 0;
		memset(args, 0, sizeof(args));
		last_identify_op = ATA_OP_PIDENTIFY;
		args[0] = last_identify_op;
		args[3] = 1;	/* sector count */
		if (do_drive_cmd(fd, args, 0)) {
			perror(" HDIO_DRIVE_CMD(identify) failed");
			return;
		}
	}
	/* byte-swap the little-endian IDENTIFY data to match byte-order on host CPU */
	id = (__u16 *)(args + 4);
	for (i = 0; i < 0x100; ++i) {
		unsigned char *b = (unsigned char *)&id[i];
		id[i] = b[0] | (b[1] << 8);	/* le16_to_cpu() */
	}
}


int SataStorage::do_drive_cmd (int fd, unsigned char *args, unsigned int timeout_secs)
{
#ifdef SG_IO

	struct ata_tf tf;
	void *data = NULL;
	unsigned int data_bytes = 0;
	int rc;

	if (args == NULL)
		goto use_legacy_ioctl;
	/*
	 * Reformat and try to issue via SG_IO:
	 * args[0]: command in; status out.
	 * args[1]: lbal for SMART, nsect for all others; error out
	 * args[2]: feat in; nsect out.
	 * args[3]: data-count (512 multiple) for all cmds.
	 */
	tf_init(&tf, args[0], 0, 0);
	tf.lob.nsect = args[1];
	tf.lob.feat  = args[2];
	if (args[3]) {
		data_bytes   = args[3] * 512;
		data         = args + 4;
		if (!tf.lob.nsect)
			tf.lob.nsect = args[3];
	}
	if (tf.command == ATA_OP_SMART) {
		tf.lob.nsect = args[3];
		tf.lob.lbal  = args[1];
		tf.lob.lbam  = 0x4f;
		tf.lob.lbah  = 0xc2;
	}

	rc = sg16(fd, SG_READ, is_dma(tf.command), &tf, data, data_bytes, timeout_secs);
	if (rc == -1) {
		if (errno == EINVAL || errno == ENODEV || errno == EBADE)
			goto use_legacy_ioctl;
	}

	if (rc == 0 || errno == EIO) {
		args[0] = tf.status;
		args[1] = tf.error;
		args[2] = tf.lob.nsect;
	}
	return rc;

use_legacy_ioctl:
#endif /* SG_IO */
	if (verbose) {
		if (args)
			fprintf(stderr, "Trying legacy HDIO_DRIVE_CMD\n");
	}
	return ioctl(fd, HDIO_DRIVE_CMD, args);
}

void SataStorage::tf_init (struct ata_tf *tf, __u8 ata_op, __u64 lba, unsigned int nsect)
{
	memset(tf, 0, sizeof(*tf));
	tf->command  = ata_op;
	tf->dev      = ATA_USING_LBA;
	tf->lob.lbal = lba;
	tf->lob.lbam = lba >>  8;
	tf->lob.lbah = lba >> 16;
	tf->lob.nsect = nsect;
	if (needs_lba48(ata_op, lba, nsect)) {
		tf->is_lba48 = 1;
		tf->hob.nsect = nsect >> 8;
		tf->hob.lbal = lba >> 24;
		tf->hob.lbam = lba >> 32;
		tf->hob.lbah = lba >> 40;
	} else {
		tf->dev |= (lba >> 24) & 0x0f;
	}
}


 inline int SataStorage::needs_lba48 (__u8 ata_op, __u64 lba, unsigned int nsect)
{
	switch (ata_op) {
		case ATA_OP_DSM:
		case ATA_OP_READ_PIO_EXT:
		case ATA_OP_READ_DMA_EXT:
		case ATA_OP_WRITE_PIO_EXT:
		case ATA_OP_WRITE_DMA_EXT:
		case ATA_OP_READ_VERIFY_EXT:
		case ATA_OP_WRITE_UNC_EXT:
		case ATA_OP_READ_NATIVE_MAX_EXT:
		case ATA_OP_SET_MAX_EXT:
		case ATA_OP_FLUSHCACHE_EXT:
			return 1;
		case ATA_OP_SECURITY_ERASE_PREPARE:
		case ATA_OP_SECURITY_ERASE_UNIT:
		case ATA_OP_VENDOR_SPECIFIC_0x80:
		case ATA_OP_SMART:
			return 0;
	}
	if (lba >= lba28_limit)
		return 1;
	if (nsect) {
		if (nsect > 0xff)
			return 1;
		if ((lba + nsect - 1) >= lba28_limit)
			return 1;
	}
	return 0;
}
 

//extern int nvme_identify_ctrl(int fd, void *data);


 char *nvme_char_from_block(char *block)
{
	char slen[16];
	unsigned len;
	if (strncmp("nvme", block, 4)) {
		fprintf(stderr,"Device %s is not a nvme device.", block);
		exit(-1);
	}
	sscanf(block,"nvme%d", &len);
	sprintf(slen,"%d", len);
	block[4+strlen(slen)] = 0;
	return block;
}
#if 0
 void get_registers(struct nvme_bar **bar)
{
	int pci_fd;
	char *base, path[512];
	void *membase;

	base = nvme_char_from_block((char *)devicename);
	sprintf(path, "/sys/class/nvme/%s/device/resource0", base);
	pci_fd = open(path, O_RDONLY);
	if (pci_fd < 0) {
		sprintf(path, "/sys/class/misc/%s/device/resource0", base);
		pci_fd = open(path, O_RDONLY);
	}
	if (pci_fd < 0) {
		fprintf(stderr, "%s did not find a pci resource\n", base);
		exit(ENODEV);
	}

	membase = mmap(0, getpagesize(), PROT_READ, MAP_SHARED, pci_fd, 0);
	if (membase == MAP_FAILED) {
		fprintf(stderr, "%s failed to map\n", base);
		exit(ENODEV);
	}
	*bar = membase;
}
#endif


static struct si_suffix {
	double magnitude;
	const char *suffix;
} si_suffixes[] = {
	{1e15, "P"},
	{1e12, "T"},
	{1e9, "G"},
	{1e6, "M"},
	{1e3, "k"},
	{1e0, ""},
	{1e-3, "m"},
	{1e-6, "u"},
	{1e-9, "n"},
	{1e-12, "p"},
	{1e-15, "f"},
	{0}
};

const char *suffix_si_get(double *value)
{
	struct si_suffix *s;

	for (s = si_suffixes; s->magnitude != 0; s++) {
		if (*value >= s->magnitude) {
			*value /= s->magnitude;
			return s->suffix;
		}
	}
	return "";
}

static struct binary_suffix {
	int shift;
	const char *suffix;
} binary_suffixes[] = {
	{50, "Pi"},
	{40, "Ti"},
	{30, "Gi"},
	{20, "Mi"},
	{10, "Ki"},
	{0, ""}
};

const char *suffix_binary_get(long long *value)
{
	struct binary_suffix *s;

	for (s = binary_suffixes; s->shift != 0; s++) {
		if (llabs(*value) >= (1LL << s->shift)) {
			*value =
			    (*value + (1 << (s->shift - 1))) / (1 << s->shift);
			return s->suffix;
		}
	}

	return "";
}


int fd;

int main (int argc, char **argv)
{
   int ret;
//   struct nvme_id_ctrl ctrl;

   unsigned int flags = 0;
  struct nvme_id_ctrl ctrl;
   FrameWorkSata* SataStorage_instance;
NvmeStorage* NvmeStorage_instance;
//if (argc < 2) {
	//	//general_help(&builtin);
		//return 0;
	//}
	__u8 vs;

	setlocale(LC_ALL, "");


	SataStorage_instance= SataStorage_new();
	//SataStorage_Init(SataStorage_instance);

//NvmeStorage_instance=NvmeStorage_new();
//NvmeStorage_Init(NvmeStorage_instance);

	SataStorage_instance->Identify();
	SataStorage_instance->ReadSector(195352516);
	SataStorage_instance->WriteSector(1000000001);
	SataStorage_instance->WriteCache(0);

NvmeStorage_instance=NvmeStorage_new();
//NvmeStorage_Init(NvmeStorage_instance);

NvmeStorage_instance->nvme_identify_ctrl( &ctrl);
NvmeStorage_instance->show_nvme_id_ctrl_(&ctrl, flags);
	return ret;
}


int SataStorage:: EntryPoint (char const *arg1, __u64 i )
{
     
    int no_more_flags = 0, disallow_flags = 0;
	char c=i;
	char name[32];
	//argc = _argc;
	argv = arg2;
	argp = NULL;
	
    if (0 == strcasecmp(arg1, "WC")) {
		c = 'W';
        if(i == 0)
            ::wcache=0;
        else
            ::wcache=1;
	//	--num_flags_processed;	/* doesn't count as an action flag */
	}
	else 
	 if (0 == strcasecmp(arg1, "I")) {
		c = 'I';
	//	--num_flags_processed;	/* doesn't count as an action flag */
	}
	else 
	 if (0 == strcasecmp(arg1, "i")) {
		c = 'i';
	//	--num_flags_processed;	/* doesn't count as an action flag */
	}
    else
     if (0 == strcasecmp(arg1, "r")) {
		c = 'r';
        ::read_sector_addr=i;
        ::read_sector=1;
	//	--num_flags_processed;	/* doesn't count as an action flag */
	}
    else
     if (0 == strcasecmp(arg1, "w")) {
		c = 'w';
        ::write_sector_addr=i;
        ::write_sector=1;
	//	--num_flags_processed;	/* doesn't count as an action flag */
	}
#if 0
return (0);
	if  ((progname = (char *) strrchr(argv, '/')) == NULL)
		progname = argv;
	else
		progname++;
	++argv;

	if (!--argc)
		;//usage_help(6,EINVAL);
//	while (argc--) {
		argp = argv++;
		if (no_more_flags || argp[0] != '-') {
			if (!num_flags_processed)
				do_defaults = 1;
			process_dev(argp);
//			continue;
		}
		if (0 == strcmp(argp, "--")) {
			no_more_flags = 1;
//			continue;
		}
		if (disallow_flags)
		;//	usage_help(7,EINVAL);
		if (!*++argp)
		;//	usage_help(8,EINVAL);
    #endif
		while (c) {
			switch (c) {
                case      DO_FLAG('i',::do_identity);
				case      DO_FLAG('I',::do_IDentity);
                case GET_SET_PARM('W',"write-cache",wcache,0,1);
                break;
                #if 0
				case GET_SET_PARM('a',"filesystem-read-ahead",fsreadahead,0,2048);
				case GET_SET_PARM('A',"look-ahead",lookahead,0,1);
				case GET_SET_PARM('b',"bus-state",busstate,0,2);
				case GET_SET_PARM('B',"power-management-mode",apmmode,0,255);
				case GET_SET_PARM('c',"32-bit-IO",io32bit,0,3);
				case     SET_FLAG('C',powermode);
				case GET_SET_PARM('d',"dma-enable",dma,0,1);
				case     SET_PARM('D',"defects-management",defects,0,1);
				case     SET_PARM('E',"CDROM/DVD-speed",cdromspeed,0,255);
				case      DO_FLAG('f',do_flush);
				case      DO_FLAG('F',do_flush_wcache);
				case      DO_FLAG('g',get_geom);
				case              'h': usage_help(9,0); break;
				case     SET_FLAG('H',hitachi_temp);
			
				case GET_SET_PARM('J',"WDC-idle3-timeout",wdidle3,0,300);
				case GET_SET_PARM('k',"kernel-keep-settings",keep,0,1);
				case     SET_PARM('K',"drive-keep-settings",dkeep,0,1);
				case     SET_PARM('L',"door-lock",doorlock,0,1);
				case GET_SET_PARM('m',"multmode-count",mult,0,64);
				case GET_SET_PARM('M',"acoustic-management",acoustic,0,255);
				case GET_SET_PARM('n',"ignore-write-errors",nowerr,0,1);
				case              'N': get_set_max_sectors_parms(); break;
				case     SET_PARM('P',"prefetch",prefetch,0,255);
				case              'q': quiet = 1; noisy = 0; break;
				case GET_SET_PARM('Q',"queue-depth",dma_q,0,1024);
				case     SET_PARM('s',"powerup-in-standby",powerup_in_standby,0,1);
				case     SET_PARM('S',"standby-interval",standby,0,255);
				case GET_SET_PARM('r',"read-only",readonly,0,1);
				case GET_SET_PARM('R',"write-read-verify",write_read_verify,0,3);
				case      DO_FLAG('t',do_timings);
				case      DO_FLAG('T',do_ctimings);
				case GET_SET_PARM('u',"unmask-irq",unmask,0,1);
				case      DO_FLAG('v',do_defaults);
				case              'V': fprintf(stdout, "%s %s\n", progname, HDPARM_VERSION); exit(0);
				case     SET_FLAG('w',doreset);
				case GET_SET_PARM('W',"write-cache",wcache,0,1);
				case     SET_FLAG('y',standbynow);
				case     SET_FLAG('Y',sleepnow);
				case     SET_FLAG('z',reread_partn);
				case     SET_FLAG('Z',seagate);

				case '-':
					if (get_longarg())
						disallow_flags = 1;
					break;

				case 'p':
					get_piomode = noisy;
					noisy = 1;
				;//	GET_XFERMODE(set_piomode,piomode);
					break;

				case 'X':
					get_xfermode = noisy;
					noisy = 1;
				;//	GET_XFERMODE(set_xfermode,xfermode_requested);
					if (!set_xfermode)
						fprintf(stderr, "-X: missing value\n");
					break;
#endif

				default:
				;//	usage_help(10,EINVAL);
			}
            break;
			::num_flags_processed++;
		}
       return process_dev();
		if (!argc)
		;//	usage_help(11,EINVAL);
	// return (0);
}

void SataStorage::identify (int fd, __u16 *id_supplied)
{
	unsigned int sector_bytes = 512;
	__u16 val[256], ii, jj, kk;
	__u16 like_std = 1, std = 0, min_std = 0xffff;
	__u16 dev = NO_DEV, eqpt = NO_DEV;
	__u8  have_mode = 0, err_dma = 0;
	__u8  chksum = 0;
	__u32 ll, mm, nn;
	__u64 bb, bbbig; /* (:) */
	int transport, is_cfa = 0, atapi_has_dmadir = 0, sdma_ok;

	memcpy(val, id_supplied, sizeof(val));

	/* calculate checksum over all bytes */
	for (ii = GEN_CONFIG; ii<=INTEGRITY; ii++) {
		chksum += val[ii] + (val[ii] >> 8);
	}

	/* check if we recognise the device type */
	printf("\n");

	//if(val[GEN_CONFIG] == 0x848a || val[GEN_CONFIG] == 0x844a) {
	if (is_cfa_dev(val)) {
		is_cfa = 1;
		dev = ATA_DEV;
		like_std = 4;
		printf("CompactFlash ATA device\n");
	} else if(!(val[GEN_CONFIG] & NOT_ATA)) {
		dev = ATA_DEV;
		printf("ATA device, with ");
	} else if(!(val[GEN_CONFIG] & NOT_ATAPI)) {
		dev = ATAPI_DEV;
		eqpt = (val[GEN_CONFIG] & EQPT_TYPE) >> SHIFT_EQPT;
		printf("ATAPI %s, with ", pkt_str[eqpt]);
		like_std = 3;
	} else {
		printf("Unknown device type:\n\tbits 15&14 of general configuration word 0 both set to 1.\n");
		exit(EINVAL);
	}
	if (!is_cfa) {
		if(!(val[GEN_CONFIG] & MEDIA_REMOVABLE))
			printf("non-");
		printf("removable media\n");
	}

	/* Info from the specific configuration word says whether or not the
	 * ID command completed correctly.  It is only defined, however in
	 * ATA/ATAPI-5 & 6; it is reserved (value theoretically 0) in prior 
	 * standards.  Since the values allowed for this word are extremely
	 * specific, it should be safe to check it now, even though we don't
	 * know yet what standard this device is using.
	 */
	if((val[CONFIG]==STBY_NID_VAL) || (val[CONFIG]==STBY_ID_VAL) ||
	   (val[CONFIG]==PWRD_NID_VAL) || (val[CONFIG]==PWRD_ID_VAL) ) {
		like_std = 5;
		if((val[CONFIG]==STBY_NID_VAL) || (val[CONFIG]==STBY_ID_VAL))
			printf("powers-up in standby; SET FEATURES subcmd spins-up.\n");
		if(((val[CONFIG]==STBY_NID_VAL) || (val[CONFIG]==PWRD_NID_VAL)) &&
		   (val[GEN_CONFIG] & INCOMPLETE)) 
			printf("\n\tWARNING: ID response incomplete.\n\tWARNING: Following data may be incorrect.\n\n");
	}

	/* output the model and serial numbers and the fw revision */
	if(val[START_MODEL]) {
		printf("\t%-20s","Model Number:");
		print_ascii(&val[START_MODEL], LENGTH_MODEL);
	}
	if(val[START_SERIAL]) {
		printf("\t%-20s","Serial Number:");
		print_ascii( &val[START_SERIAL], LENGTH_SERIAL);
	}
	if(val[START_FW_REV]) {
		printf("\t%-20s","Firmware Revision:");
		print_ascii(&val[START_FW_REV], LENGTH_FW_REV);
	}
	if(val[START_MEDIA]) {
		printf("\t%-20s","Media Serial Num:");
		print_ascii(&val[START_MEDIA], LENGTH_MEDIA);
	}
	if(val[START_MANUF]) {
		printf("\t%-20s","Media Manufacturer:");
		print_ascii(&val[START_MANUF], LENGTH_MANUF);
	}

	transport = print_transport_type(val);

	/* major & minor standards version number (Note: these words were not
	 * defined until ATA-3 & the CDROM std uses different words.) */
	printf("Standards:");
	if(eqpt != CDROM) {
		//printf("major=%04x minor=%04x\n", val[MAJOR], val[MINOR]);
		const char * used = 0;
		if(val[MINOR] && (val[MINOR] <= MINOR_MAX)) {
			if(like_std < 3)
				like_std = 3;
			std = actual_ver[val[MINOR]];
			if (std)
				used = minor_str[val[MINOR]];
		} else {
			/* check for recent ATA-8 revision codes (not added to
			 * actual_ver/minor_str to avoid large sparse tables) */
			switch (val[MINOR]) {
			  case 0x0027: used = "ATA-8-ACS revision 3c"; break;
			  case 0x0033: used = "ATA-8-ACS revision 3e"; break;
			  case 0x0042: used = "ATA-8-ACS revision 3f"; break;
			  case 0x0052: used = "ATA-8-ACS revision 3b"; break;
			  case 0x0107: used = "ATA-8-ACS revision 2d"; break;
			}
			if (used)
				std = 8;
		}
		if (used)
			printf("\n\tUsed: %s ", used);
		else if (val[MINOR] >= 0x001f) /* first "reserved" value possibly later used by ATA-8 */
			printf("\n\tUsed: unknown (minor revision code 0x%04x) ", val[MINOR]);

		/* looks like when they up-issue the std, they obsolete one;
		 * thus, only the newest 4 issues need be supported.
		 * (That's what "kk" and "min_std" are all about) */
		if(val[MAJOR] && (val[MAJOR] != 0xffff)) {
			printf("\n\tSupported: ");
			jj = val[MAJOR] << 1;
			kk = min_ata_std(like_std);
			for (ii = 14; ii > kk; ii--) {
				if(jj & 0x8000) {
					printf("%u ", ii);
					if (ii > like_std) {
						like_std = ii;
						kk = min_ata_std(like_std);
					}
					if (min_std > ii)
						min_std = ii;
				}
				jj <<= 1;
			}
			if(like_std < 3)
				like_std = 3;
		}
		/* Figure out what standard the device is using if it hasn't told
		 * us.  If we know the std, check if the device is using any of
		 * the words from the next level up.  It happens.
		 */
		if(like_std < std) like_std = std;
		if(((std == 7) || (!std && (like_std < 8))) &&
		   (val[SCT_SUPP] & 0x1)) {
			like_std = 8;
		} else if(((std == 5) || (!std && (like_std < 6))) &&
		   ( (((val[CMDS_SUPP_1] & VALID) == VALID_VAL) &&
		     ((val[CMDS_SUPP_1] & CMDS_W83) > 0x00ff)) ||
		    (((val[CMDS_SUPP_2] & VALID) == VALID_VAL) &&
		     (val[CMDS_SUPP_2] & CMDS_W84) ) ) ) {
			like_std = 6;
		}  else	if(((std == 4) || (!std && (like_std < 5))) &&
		   ((((val[INTEGRITY] & SIG) == SIG_VAL) && !chksum) ||
		    ((val[HWRST_RSLT] & VALID) == VALID_VAL) ||
		    (((val[CMDS_SUPP_1] & VALID) == VALID_VAL) &&
		     ((val[CMDS_SUPP_1] & CMDS_W83) > 0x001f)) ) ) {
			like_std = 5;
		}  else if(((std == 3) || (!std && (like_std < 4))) &&
			   ((((val[CMDS_SUPP_1] & VALID) == VALID_VAL) &&
			     (((val[CMDS_SUPP_1] & CMDS_W83) > 0x0000) ||  
			      ((val[CMDS_SUPP_0] & CMDS_W82) > 0x000f))) ||
			    ((val[CAPAB_1] & VALID) == VALID_VAL) ||
			    ((val[WHATS_VALID] & OK_W88) && val[ULTRA_DMA]) ||
			    ((val[RM_STAT] & RM_STAT_BITS) == RM_STAT_SUP) ) ) {
			like_std = 4;
		}  else if(((std == 2) || (!std && (like_std < 3))) &&
			   ((val[CMDS_SUPP_1] & VALID) == VALID_VAL) ) {
			like_std = 3;
		}  else if(((std == 1) || (!std && (like_std < 2))) &&
		   	   ((val[CAPAB_0] & (IORDY_SUP | IORDY_OFF)) ||
		   	    (val[WHATS_VALID] & OK_W64_70)) ) {
			like_std = 2;
		}
		if(!std) {
			printf("\n\tLikely used: %u\n",like_std);
		} else if(like_std > std) {
			printf("& some of %u\n",like_std);
		} else  printf("\n");
	} else {
		/* TBD: do CDROM stuff more thoroughly.  For now... */
		kk = 0;
		if(val[CDR_MINOR] == 9) {
			kk = 1;
			printf("\n\tUsed: ATAPI for CD-ROMs, SFF-8020i, r2.5");
		}
		if(val[CDR_MAJOR] && (val[CDR_MAJOR] != 0xffff)) {
			kk = 1;
			printf("\n\tSupported: CD-ROM ATAPI");
			jj = val[CDR_MAJOR] >> 1;
			for (ii = 1; ii <15; ii++) {
				if(jj & 0x0001) {
					printf("-%u ", ii);
				}
				jj >>= 1;
			}
		}
		if(!kk) printf("\n\tLikely used CD-ROM ATAPI-1\n");
		else	printf("\n");
		/* the cdrom stuff is more like ATA-2 than anything else, so: */
		like_std = 2;
	}

	if(min_std == 0xffff)
		min_std = like_std > 4 ? like_std - 3 : 1;

	printf("Configuration:\n");
	/* more info from the general configuration word */
	if((eqpt != CDROM) && (like_std == 1)) {
		jj = val[GEN_CONFIG] >> 1;
		for (ii = 1; ii < 15; ii++) {
			if(jj & 0x0001) printf("\t%s\n",ata1_cfg_str[ii]);
			jj >>=1;
		}
	}
	if(dev == ATAPI_DEV) {
		printf("\tDRQ response: "); /* Data Request (DRQ) */
		switch(val[GEN_CONFIG] & DRQ_RESPONSE_TIME) {
		case DRQ_3MS_VAL : printf("3ms.\n"); break;
		case DRQ_INTR_VAL : printf("<=10ms with INTRQ\n"); break;
		case DRQ_50US_VAL : printf("50us.\n"); break;
		default : printf("unknown.\n"); break;
		}
		printf("\tPacket size: ");
		switch(val[GEN_CONFIG] & PKT_SIZE_SUPPORTED) {
		case PKT_SIZE_12_VAL : printf("12 bytes\n"); break;
		case PKT_SIZE_16_VAL : printf("16 bytes\n"); break;
		default : printf("Unknown\n"); break;
		}
	} else {
		/* addressing...CHS? See section 6.2 of ATA specs 4 or 5 */
		ll = 0; mm = 0; bb = 0; bbbig = 0;
		if (val[CAPAB_0] & LBA_SUP)
			ll = (__u32)val[LBA_SECTS_MSB] << 16 | val[LBA_SECTS_LSB];
		if ( (ll > 0x00FBFC10) && (!val[LCYLS])) {
			printf("\tCHS addressing not supported\n");
		} else {
			jj = val[WHATS_VALID] & OK_W54_58;
			printf("\tLogical\t\tmax\tcurrent\n");
			printf("\tcylinders\t%u\t%u\n",val[LCYLS],jj?val[LCYLS_CUR]:0);
			printf("\theads\t\t%u\t%u\n",val[LHEADS],jj?val[LHEADS_CUR]:0);
			printf("\tsectors/track\t%u\t%u\n",val[LSECTS],jj?val[LSECTS_CUR]:0);
			if(jj)
				bb = (__u64)val[LCYLS_CUR] * val[LHEADS_CUR] * val[LSECTS_CUR];
			else
				bb = (__u64)val[LCYLS] * val[LHEADS] * val[LSECTS];
			printf("\t--\n");
			if((min_std == 1) && (val[TRACK_BYTES] || val[SECT_BYTES])) {
				printf("\tbytes/track: %u",val[TRACK_BYTES]);
				printf("\tbytes/sector: %u\n",val[SECT_BYTES]);
			}
			if(jj) {
				mm = (__u32)val[CAPACITY_MSB] << 16 | val[CAPACITY_LSB];
				/* ATA-1 is ambiguous on ordering of words 57 & 58 */
				if(like_std < 3) {
					nn = (__u32)val[CAPACITY_LSB] << 16 | val[CAPACITY_MSB];
					/* check Endian of capacity bytes */
					if(llabs((long long)(mm - bb)) > llabs((long long)(nn - bb)))
						mm = nn;
				}
				printf("\tCHS current addressable sectors:%11u\n",mm);
			} 
		}
		if (val[CAPAB_0] & LBA_SUP) {
		/* LBA addressing */
			printf("\tLBA    user addressable sectors:%11u\n",ll);
			if( ((val[CMDS_SUPP_1] & VALID) == VALID_VAL) &&
			     (val[CMDS_SUPP_1] & SUPPORT_48_BIT) ) {
				bbbig = (__u64)val[LBA_64_MSB] << 48 | 
				        (__u64)val[LBA_48_MSB] << 32 |
				        (__u64)val[LBA_MID] << 16 | 
					val[LBA_LSB] ;
                                MaxLBA=bbbig;
				printf("\tLBA48  user addressable sectors:%11llu\n", (unsigned long long)bbbig);
			}
		}
		if((val[106] & 0xc000) != 0x4000) {
			printf("\t%-31s %11u bytes\n","Logical/Physical Sector size:", sector_bytes);
		} else {
			unsigned int lsize = 256, pfactor = 1;
			if (val[106] & (1<<13))
				pfactor = (1 << (val[106] & 0xf));
			if (val[106] & (1<<12))
				lsize = (val[118] << 16) | val[117];
			sector_bytes = 2 * lsize;
			printf("\t%-31s %11u bytes\n","Logical  Sector size:", sector_bytes);
			printf("\t%-31s %11u bytes\n","Physical Sector size:", sector_bytes * pfactor);
			if ((val[209] & 0xc000) == 0x4000) {
				unsigned int offset = val[209] & 0x1fff;
				printf("\t%-31s %11u bytes\n", "Logical Sector-0 offset:", offset * sector_bytes);
			}
		}
		if (!bbbig) bbbig = (__u64)(ll>mm ? ll : mm); /* # 512 byte blocks */
		if (!bbbig) bbbig = bb;
		bbbig *= (sector_bytes / 512);
		printf("\tdevice size with M = 1024*1024: %11llu MBytes\n", (unsigned long long)(bbbig>>11));
		bbbig = (bbbig<<9)/1000000;
		printf("\tdevice size with M = 1000*1000: %11llu MBytes ", (unsigned long long)bbbig);
		if(bbbig > 1000) printf("(%llu GB)\n", (unsigned long long)(bbbig/1000));
		else printf("\n");
	}

	/* device cache/buffer size, if reported (obsolete field, but usually valid regardless) */
	printf("\tcache/buffer size  = ");
	if (val[20] <= 3 && val[BUF_SIZE] && val[BUF_SIZE] != 0xffff) {
		printf("%u KBytes", val[BUF_SIZE] / 2);
		if (val[20])
			printf(" (type=%s)", BuffType[val[20]]);
	} else {
		printf("unknown");
	}
	putchar('\n');

	/* Form factor */
	if(val[168] && (val[168] & 0xfff8) == 0) {
		printf("\tForm Factor: ");
		switch(val[168]) {
		case 1:
			printf("5.25 inch");
			break;
		case 2:
			printf("3.5 inch");
			break;
		case 3:
			printf("2.5 inch");
			break;
		case 4:
			printf("1.8 inch");
			break;
		case 5:
			printf("less than 1.8 inch");
			break;
		default:
			printf("unknown (0x%04x]", val[168]);
			break;
		}
		printf("\n");
	}

	/* Spinning disk or solid state? */
	if(val[NMRR] == 1)
		printf("\tNominal Media Rotation Rate: Solid State Device\n");
	else if(val[NMRR] > 0x401)
		printf("\tNominal Media Rotation Rate: %u\n", val[NMRR]);

	/* hw support of commands (capabilities) */
	printf("Capabilities:\n");
	printf("\t");
	if(dev == ATAPI_DEV) {
		if(eqpt != CDROM) {
			if(val[CAPAB_0] & CMD_Q_SUP) printf("Cmd queuing, ");
		}
		if(val[CAPAB_0] & OVLP_SUP) printf("Cmd overlap, ");
	}
	if(val[CAPAB_0] & LBA_SUP) printf("LBA, ");
	if(like_std != 1) {
		printf("IORDY");
		if(!(val[CAPAB_0] & IORDY_SUP)) printf("(may be)");
		if(val[CAPAB_0] & IORDY_OFF) printf("(can");
		else			     printf("(cannot");
		printf(" be disabled)");
	} else {
		printf("IORDY not likely"); 
	}
	printf("\n");
	if((like_std == 1) && val[BUF_TYPE]) {
		kk = val[BUF_TYPE];
		printf("\tBuffer type: %04x: ",kk);
		if (kk < 2) 	printf("single port, single-sector");
		else		printf("dual port, multi-sector");
		if (kk > 2)	printf(" with read caching ability");
		printf("\n");
	}
	jj = 0;
	if((min_std == 1) && (val[BUF_SIZE] && (val[BUF_SIZE] != 0xffff))) {
		printf("\tBuffer size: %.1fkB",(float)val[BUF_SIZE]/2);
		jj = 1;
	}
	if((min_std < 4) && (val[RW_LONG])) {
		printf("\tbytes avail on r/w long: %u",val[RW_LONG]);
		jj = 1;
	}
	if((eqpt != CDROM) && (like_std > 3)) {
		int has_queuing = 0;
		if (transport == 1 || (val[SATA_CAP_0] && val[SATA_CAP_0] != 0xffff)) {
			if (val[SATA_CAP_0] & 0x0100)
				has_queuing = 1;	// SATA NCQ
		}
		if ((val[CMDS_SUPP_1] & VALID) == VALID_VAL && val[CMDS_SUPP_1] & 2) {
			has_queuing = 1;		// TCQ
		}
		if (has_queuing) {
			printf("\tQueue depth: %u",(val[QUEUE_DEPTH] & DEPTH_BITS)+1);
			jj = 1;
		}
	}
	if(jj) printf("\n");
	if(dev == ATA_DEV) {
		if(like_std == 1) {
			printf("\tCan");
			if(!val[DWORD_IO]) printf("not");
			printf(" perform double-word IO\n");
		} else {
			printf("\tStandby timer values: spec'd by ");
			if(val[CAPAB_0] & STD_STBY) printf("Standard");
			else 			    printf("Vendor");
			if((like_std > 3) && ((val[CAPAB_1] & VALID) == VALID_VAL)) {
				if(val[CAPAB_1] & MIN_STANDBY_TIMER) printf(", with ");
				else 				     printf(", no ");
				printf("device specific minimum\n");
			} else  printf("\n");
		}
		printf("\tR/W multiple sector transfer: ");
		if((like_std < 3) && !(val[SECTOR_XFER_MAX] & SECTOR_XFER)) {
			printf("not supported\n");
		} else {
			printf("Max = %u\t",val[SECTOR_XFER_MAX] & SECTOR_XFER);
			printf("Current = ");
			if(val[SECTOR_XFER_CUR] & MULTIPLE_SETTING_VALID)
				printf("%u\n",val[SECTOR_XFER_CUR] & SECTOR_XFER);
			else	printf("?\n");
		}
		if((like_std > 3) && (val[CMDS_SUPP_1] & 0xc008) == 0x4008) {
			printf("\tAdvanced power management level: ");
			if (val[CMDS_EN_1] & 0x0008)
				printf("%u\n", val[ADV_PWR] & 0xff);
			else
				printf("disabled\n");
		}
		if(like_std > 5) {
			if(val[ACOUSTIC]) {
				printf("\tRecommended acoustic management value: %u, current value: %u\n", (val[ACOUSTIC] >> 8) & 0x00ff, val[ACOUSTIC] & 0x00ff);
			}
		}
	} else { /* ATAPI */
		if(eqpt != CDROM) {
			if(val[CAPAB_0] & SWRST_REQ) printf("\tATA sw reset required\n");
		}
		if(val[PKT_REL] || val[SVC_NBSY]) {
			printf("\tOverlap support:");
			if(val[PKT_REL]) printf(" %uus to release bus.",val[PKT_REL]);
			if(val[SVC_NBSY]) printf(" %uus to clear BSY after SERVICE cmd.",val[SVC_NBSY]);
			printf("\n");
		}
	}

	/* Some SATA-ATAPI devices use a different interpretation of IDENTIFY words for DMA modes */
	if (dev == ATAPI_DEV && val[62] & 0x8000) {
		atapi_has_dmadir = 1;
		sdma_ok = 0;  /* word 62 has been re-purposed for non-sdma mode reporting */
		printf("\tDMADIR bit required in PACKET commands\n");
	} else {
		__u16 w62 = val[62];
		__u8 hi = w62 >> 8, lo = w62;
		if (!w62 || (lo & 0xf8))
			sdma_ok = 0;
		else if (hi && hi != 1 && hi != 2 && hi != 4)
			sdma_ok = 0;
		else
			sdma_ok = 1;
	}

	printf("\tDMA: ");
	/* DMA stuff. Check that only one DMA mode is selected. */
	if(!atapi_has_dmadir && !(val[CAPAB_0] & DMA_SUP)) {
		printf("not supported\n");
	} else {
		if(val[DMA_MODE] && !val[62] && !val[MULTI_DMA]) {
			printf("sdma%u",(val[DMA_MODE] & MODE) >> 8);
		} else {
			if(sdma_ok) {
				kk = val[62] >> 8;
				jj = val[62];
				err_dma += mode_loop(jj,kk,'s',&have_mode);
			}
			if(val[MULTI_DMA]) {
				kk = val[MULTI_DMA] >> 8;
				jj = atapi_has_dmadir ? (val[62] >> 7) & 7 : val[MULTI_DMA];
				err_dma += mode_loop(jj,kk,'m',&have_mode);
			}
			if((val[WHATS_VALID] & OK_W88) && val[ULTRA_DMA]) {
				kk = val[ULTRA_DMA] >> 8;
				jj = atapi_has_dmadir ? val[62] & 0x7f : val[ULTRA_DMA];
				err_dma += mode_loop(jj,kk,'u',&have_mode);
			}
			if(err_dma || !have_mode)
				printf("(?)");
		}
		printf("\n");

		if((dev == ATAPI_DEV) && (eqpt != CDROM) && (val[CAPAB_0] & DMA_IL_SUP)) 
			printf("\t     Interleaved DMA support\n");

		if((val[WHATS_VALID] & OK_W64_70) && 
		   (val[DMA_TIME_MIN] || val[DMA_TIME_NORM])) {
			printf("\t     Cycle time:");
			if(val[DMA_TIME_MIN])
				printf(" min=%uns",val[DMA_TIME_MIN]);
			if(val[DMA_TIME_NORM])
				printf(" recommended=%uns",val[DMA_TIME_NORM]);
			printf("\n");
		}
	}

	/* Programmed IO stuff */
	printf("\tPIO: ");
        /* If a drive supports mode n (e.g. 3), it also supports all modes less
	 * than n (e.g. 3, 2, 1 and 0).  Print all the modes. */
	if((val[WHATS_VALID] & OK_W64_70) && (val[ADV_PIO_MODES] & PIO_SUP)) {
		jj = ((val[ADV_PIO_MODES] & PIO_SUP) << 3) | 0x0007;
		for (ii = 0; ii <= PIO_MODE_MAX ; ii++) {
			if(jj & 0x0001)
				printf("pio%d ",ii);
			jj >>=1;
		}
		printf("\n");
	} else if(((min_std < 5) || (eqpt == CDROM)) && ((val[PIO_MODE]>>8) <= 2)) {
		for (ii = 0; ii <= val[PIO_MODE]>>8; ii++) {
			printf("pio%d ",ii);
		}
		printf("\n");
	} else  printf("unknown\n");
	if(val[WHATS_VALID] & OK_W64_70) {
		if(val[PIO_NO_FLOW] || val[PIO_FLOW]) {
			printf("\t     Cycle time:");
			if(val[PIO_NO_FLOW])
				printf(" no flow control=%uns", val[PIO_NO_FLOW]);
			if(val[PIO_FLOW])
				printf("  IORDY flow control=%uns", val[PIO_FLOW]);
			printf("\n");
		}
	}

	if((val[CMDS_SUPP_1] & VALID) == VALID_VAL){
		printf("Commands/features:\n\tEnabled\tSupported:\n");
		print_features(val[CMDS_SUPP_0] & 0x7fff, val[CMDS_EN_0], feat_word82_str);
		if( (val[CMDS_SUPP_1] &  VALID) == VALID_VAL)
			print_features(val[CMDS_SUPP_1] & 0x3fff, val[CMDS_EN_1], feat_word83_str);
		if( (val[CMDS_SUPP_2] &  VALID) == VALID_VAL
		 && (val[CMDS_EN_2]  &   VALID) == VALID_VAL) {
			print_features(val[CMDS_SUPP_2] & 0x3fff, val[CMDS_EN_2], feat_word84_str);
			if ((val[CMDS_SUPP_2] & 0x1800) == 0x1800 && val[116] && val[116] != 0xffff)
				printf("                (%u msec for TLC completion timer)\n", 10 * (unsigned int)(val[116]));
		}
		if( (val[CMDS_SUPP_1] &  VALID) == VALID_VAL
		 && (val[CMDS_EN_1]   & 0x8000) == 0x8000
		 && (val[CMDS_SUPP_3] &  VALID) == VALID_VAL
		 && (val[CMDS_EN_3]   &  VALID) == VALID_VAL)
			print_features(val[CMDS_SUPP_3] & 0x3fff, val[CMDS_EN_3], feat_3_str);
		if (transport == 1 || (val[SATA_CAP_0] && val[SATA_CAP_0] != 0xffff))
			print_features(val[SATA_CAP_0],  val[SATA_CAP_0], cap_sata0_str);
		if (transport == 1 || (val[SATA_SUPP_0] && val[SATA_SUPP_0] != 0xffff))
			print_features(val[SATA_SUPP_0], val[SATA_EN_0], feat_sata0_str);
		if (val[SCT_SUPP] & 0x1)
			print_features(val[SCT_SUPP], val[SCT_SUPP] & 0x3f, feat_sct_str);
	}
	if (like_std > 6) {
		const __u16 trimd = 1<<14;	/* deterministic read data after TRIM */
		const __u16 trimz = 1<<5;	/* deterministic read ZEROs after TRIM */
		__u16 word69 = val[69] & ~(trimz | trimd); /* TRIM bits require special interpretation */
		print_features(word69, word69, feat_word69_str);
		if (val[169] & 1 && val[169] != 0xffff) { /* supports TRIM ? */
			printf("\t   *\tData Set Management TRIM supported");
			if (val[105] && val[105] != 0xffff)
				printf(" (limit %u block%s)\n", val[105], val[105] > 1 ? "s" : "");
			else
				printf(" (limit unknown)\n");
			if (val[69] & trimd) { /* Deterministic TRIM support */
				if (val[69] & trimz)
					print_features(trimz, trimz, feat_word69_str);
				else
					print_features(trimd, trimd, feat_word69_str);
			}
		}
		
	}
	if (is_cfa) {
		unsigned int mode, max, selected;
		char modes[256];
		modes[0] = '\0';

		// CFA pio5-6:
		max = val[163] & 7;
		if (max == 1 || max == 2) {
			selected = (val[163] >> 6) & 7;
			for (mode = 1; mode <= max; ++mode) {
				if (mode == selected)
					strcat(modes, "*");
				sprintf(modes + strlen(modes), "pio%u ", mode + 4);
			}
		}
		// CFA mdma3-4:
		max = (val[163] >> 3) & 7;
		if (max == 1 || max == 2) {
			selected = (val[163] >> 9) & 7;
			for (mode = 1; mode <= max; ++mode) {
				if (mode == selected)
					strcat(modes, "*");
				sprintf(modes + strlen(modes), "mdma%u ", mode + 2);
			}
		}
		if (val[164] & 0x8000)
		{
			static const unsigned char io_times [4] = {255,120,100,80};
			static const unsigned char mem_times[4] = {250,120,100,80};
			max = val[164] & 7;
			if (max <= 3)
				printf("\t\tCFA max advanced io_udma cycle time: %uns\n", io_times[max]);
			max = (val[164] >> 3) & 7;
			if (max <= 3)
				printf("\t\tCFA max advanced mem_udma cycle time: %uns\n", mem_times[max]);
			// CFA ioport dma0-6:
			max = (val[164] >> 6) & 7;
			if (max <= 6) {
				selected = (val[164] >> 12) & 7;
				for (mode = 0; mode <= max; ++mode) {
					if (mode == selected)
						strcat(modes, "*");
					sprintf(modes + strlen(modes), "io_udma%u ", mode + 4);
				}
			}
			// CFA memory udma0-6:
			max = (val[164] >> 9) & 7;
			if (max <= 6) {
				selected = (val[164] >> 12) & 7;
				for (mode = 0; mode <= max; ++mode) {
					if (mode == selected)
						strcat(modes, "*");
					sprintf(modes + strlen(modes), "mem_udma%u ", mode + 4);
				}
			}
		}
		if (modes[0])
			printf("\t   *\tCFA advanced modes: %s\n", modes);

		if(val[CFA_PWR_MODE] & VALID_W160) {
			putchar('\t');
			if((val[CFA_PWR_MODE] & PWR_MODE_REQ) == 0)
				printf("   *");
			printf("\tCFA Power Level 1 ");
			if(val[CFA_PWR_MODE] & PWR_MODE_REQ)
				printf(" not supported");
			if(val[CFA_PWR_MODE] & MAX_AMPS)
				printf(" (max %umA)", val[CFA_PWR_MODE] & MAX_AMPS);
			printf("\n");
		}
		//else printf("\t\tCFA Power modes not reported\n");
		if (val[162] && val[162] != 0xffff) {
			if (val[162] & 1)
				printf("\t\tKey Management (CPRM) feature set\n");
		}
	}

	if((val[RM_STAT] & RM_STAT_BITS) == RM_STAT_SUP) 
		printf("\t\tRemovable Media Status Notification feature set supported\n");

	/* security */
	if((val[CMDS_SUPP_2] & 2) && (eqpt != CDROM) && (like_std > 3) && (val[SECU_STATUS] || val[ERASE_TIME] || val[ENH_ERASE_TIME]))
	{
		printf("Security: \n");
		if(val[PSWD_CODE] && (val[PSWD_CODE] != 0xffff))
			printf("\tMaster password revision code = %u\n",val[PSWD_CODE]);
		jj = val[SECU_STATUS];
		if(jj) {
			for (ii = 0; ii < NUM_SECU_STR; ii++) {
				if(!(jj & 0x0001)) printf("%s", ii ? "\tnot\t" : "\t(?)\t");
				else		   printf("\t\t");
				printf("%s\n",secu_str[ii]);
				jj >>=1;
			}
			if(val[SECU_STATUS] & SECU_ENABLED) {
				printf("\tSecurity level ");
				if(val[SECU_STATUS] & SECU_LEVEL) printf("maximum\n");
				else				  printf("high\n");
			}
		}
		jj =  val[ERASE_TIME];
		kk =  val[ENH_ERASE_TIME];
		if((jj && jj <= 0x00ff) || (kk && kk <= 0x00ff)) {
			printf("\t");
			if (jj) {
				if (jj == 0xff)
					printf("more than 508");
				else
					printf("%u", jj * 2);
				printf("min for SECURITY ERASE UNIT. ");
			}
			if (kk) {
				if (kk == 0xff)
					printf("more than 508");
				else
					printf("%u", kk * 2);
				printf("min for ENHANCED SECURITY ERASE UNIT. ");
			}
			printf("\n");
		}
	}
	//printf("w84=0x%04x w87=0x%04x like_std=%d\n", val[84], val[87], like_std);
	if((eqpt != CDROM) && (like_std > 3) && (val[CMDS_SUPP_2] & WWN_SUP)) {
		printf("Logical Unit WWN Device Identifier: %04x%04x%04x%04x\n", val[108], val[109], val[110], val[111]);
		printf("\tNAA\t\t: %x\n", (val[108] & 0xf000) >> 12);
		printf("\tIEEE OUI\t: %06x\n", (((val[108] & 0x0fff) << 12) | ((val[109] & 0xfff0) >> 4)));
		printf("\tUnique ID\t: %x%08x\n", (val[109] & 0x000f), ((val[110] << 16) | val[111]));
	}

	/* reset result */
	if((val[HWRST_RSLT] & VALID) == VALID_VAL) {
		printf("HW reset results:\n");
		if(val[HWRST_RSLT] & CBLID) printf("\tCBLID- above Vih\n");
		else			    printf("\tCBLID- below Vih\n");
		if(val[HWRST_RSLT] & RST0)  {
			printf("\tDevice num = 0");
			jj = val[HWRST_RSLT];
		} else {
			printf("\tDevice num = 1");
			jj = val[HWRST_RSLT] >> 8;
		}
		if((jj & DEV_DET) == JUMPER_VAL) 
			printf(" determined by the jumper");
		else if((jj & DEV_DET) == CSEL_VAL)
			printf(" determined by CSEL");
		printf("\n");
	}
	//print_devslp_info(fd, val);

	/* more stuff from std 5 */
	if ((like_std > 4) && (eqpt != CDROM)) {
		if ((val[INTEGRITY] & SIG) == SIG_VAL) {
			printf("Checksum: %scorrect", chksum ? "in" : "");
			if (chksum)
				printf(" (0x%02x), expected 0x%02x\n", chksum, 0x100 - chksum);
			putchar('\n');
		} else {
			printf("Integrity word not set (found 0x%04x, expected 0x%02x%02x)\n",
				val[INTEGRITY], 0x100 - chksum, SIG_VAL);
		}
	}
}


static int is_cfa_dev (__u16 *id)
{
	/*
	 * id[0] == 0x848a means "CFA compliant, not ATA-4 compliant".
	 * id[0] == 0x044a is also allowed, but ISTR that some HDs use it too.
	 * Also, bit 0x0004 of id[83] means "supports CFA feature set".
	 */
	return id[0] == 0x848a || id[0] == 0x844a || (id[83] & 0xc004) == 0x4004;
}
#if 0
 void flush_buffer_cache (int fd)
{
	sync();
	fsync(fd);				/* flush buffers */
	fdatasync(fd);				/* flush buffers */
	sync();
	if (ioctl(fd, BLKFLSBUF, NULL))		/* do it again, big time */
		perror("BLKFLSBUF failed");
	else
		do_drive_cmd(fd, NULL, 0);	/* IDE: await completion */
	sync();
}
#endif

 void SataStorage::dump_sectors (__u16 *w, unsigned int count)
{
	unsigned int i;

	for (i = 0; i < (count*256/8); ++i) {
#if 0
		printf("%04x %04x %04x %04x %04x %04x %04x %04x\n",
			w[0], w[1], w[2], w[3], w[4], w[5], w[6], w[7]);
		w += 8;
#else
		int word;
		for (word = 0; word < 8; ++word) {
			unsigned char *b = (unsigned char *)w++;
			printf("%02x%02x", b[0], b[1]);
			putchar(word == 7 ? '\n' : ' ');
		}
#endif
	}
}

int SataStorage::do_taskfile_cmd (int fd, struct hdio_taskfile *r, unsigned int timeout_secs)
{
	int rc;
#ifdef SG_IO
	struct ata_tf tf;
	void *data = NULL;
	unsigned int data_bytes = 0;
	int rw = SG_READ;
	/*
	 * Reformat and try to issue via SG_IO:
	 */
	tf_init(&tf, 0, 0, 0);
#if 1 /* debugging */
	if (verbose) {
		printf("oflags.bits.lob_all=0x%02x, flags={", r->oflags.bits.lob_all);
		if (r->oflags.bits.lob.feat)	printf(" feat");
		if (r->oflags.bits.lob.nsect)	printf(" nsect");
		if (r->oflags.bits.lob.lbal)	printf(" lbal");
		if (r->oflags.bits.lob.lbam)	printf(" lbam");
		if (r->oflags.bits.lob.lbah)	printf(" lbah");
		if (r->oflags.bits.lob.dev)	printf(" dev");
		if (r->oflags.bits.lob.command) printf(" command");
		printf(" }\n");
		printf("oflags.bits.hob_all=0x%02x, flags={", r->oflags.bits.hob_all);
		if (r->oflags.bits.hob.feat)	printf(" feat");
		if (r->oflags.bits.hob.nsect)	printf(" nsect");
		if (r->oflags.bits.hob.lbal)	printf(" lbal");
		if (r->oflags.bits.hob.lbam)	printf(" lbam");
		if (r->oflags.bits.hob.lbah)	printf(" lbah");
		printf(" }\n");
	}
#endif
	if (r->oflags.bits.lob.feat)		tf.lob.feat  = r->lob.feat;
	if (r->oflags.bits.lob.lbal)		tf.lob.lbal  = r->lob.lbal;
	if (r->oflags.bits.lob.nsect)		tf.lob.nsect = r->lob.nsect;
	if (r->oflags.bits.lob.lbam)		tf.lob.lbam  = r->lob.lbam;
	if (r->oflags.bits.lob.lbah)		tf.lob.lbah  = r->lob.lbah;
	if (r->oflags.bits.lob.dev)		tf.dev       = r->lob.dev;
	if (r->oflags.bits.lob.command)	tf.command   = r->lob.command;
	if (needs_lba48(tf.command,0,0) || r->oflags.bits.hob_all || r->iflags.bits.hob_all) {
		tf.is_lba48 = 1;
		if (r->oflags.bits.hob.feat)	tf.hob.feat  = r->hob.feat;
		if (r->oflags.bits.hob.lbal)	tf.hob.lbal  = r->hob.lbal;
		if (r->oflags.bits.hob.nsect)	tf.hob.nsect = r->hob.nsect;
		if (r->oflags.bits.hob.lbam)	tf.hob.lbam  = r->hob.lbam;
		if (r->oflags.bits.hob.lbah)	tf.hob.lbah  = r->hob.lbah;
		if (verbose)
			fprintf(stderr, "using LBA48 taskfile\n");
	}
	switch (r->cmd_req) {
		case TASKFILE_CMD_REQ_OUT:
		case TASKFILE_CMD_REQ_RAW_OUT:
			data_bytes = r->obytes;
			data       = r->data;
			rw         = SG_WRITE;
			break;
		case TASKFILE_CMD_REQ_IN:
			data_bytes = r->ibytes;
			data       = r->data;
			break;
	}

	rc = sg16(fd, rw, is_dma(tf.command), &tf, data, data_bytes, timeout_secs);
	if (rc == -1) {
		if (errno == EINVAL || errno == ENODEV || errno == EBADE)
			goto use_legacy_ioctl;
	}

	if (rc == 0 || errno == EIO) {
		if (r->iflags.bits.lob.feat)	r->lob.feat  = tf.error;
		if (r->iflags.bits.lob.lbal)	r->lob.lbal  = tf.lob.lbal;
		if (r->iflags.bits.lob.nsect)	r->lob.nsect = tf.lob.nsect;
		if (r->iflags.bits.lob.lbam)	r->lob.lbam  = tf.lob.lbam;
		if (r->iflags.bits.lob.lbah)	r->lob.lbah  = tf.lob.lbah;
		if (r->iflags.bits.lob.dev)	r->lob.dev   = tf.dev;
		if (r->iflags.bits.lob.command)	r->lob.command = tf.status;
		if (r->iflags.bits.hob.feat)	r->hob.feat  = tf.hob.feat;
		if (r->iflags.bits.hob.lbal)	r->hob.lbal  = tf.hob.lbal;
		if (r->iflags.bits.hob.nsect)	r->hob.nsect = tf.hob.nsect;
		if (r->iflags.bits.hob.lbam)	r->hob.lbam  = tf.hob.lbam;
		if (r->iflags.bits.hob.lbah)	r->hob.lbah  = tf.hob.lbah;
	}
	return rc;

use_legacy_ioctl:
#else
	timeout_secs = 0;	/* keep compiler happy */
#endif /* SG_IO */
	if (verbose)
		fprintf(stderr, "trying legacy HDIO_DRIVE_TASKFILE\n");
	errno = 0;

	rc = ioctl(fd, HDIO_DRIVE_TASKFILE, r);
	if (verbose) {
		int err = errno;
		fprintf(stderr, "rc=%d, errno=%d, returned ATA registers: ", rc, err);
		if (r->iflags.bits.lob.feat)	fprintf(stderr, " er=%02x", r->lob.feat);
		if (r->iflags.bits.lob.nsect)	fprintf(stderr, " ns=%02x", r->lob.nsect);
		if (r->iflags.bits.lob.lbal)	fprintf(stderr, " ll=%02x", r->lob.lbal);
		if (r->iflags.bits.lob.lbam)	fprintf(stderr, " lm=%02x", r->lob.lbam);
		if (r->iflags.bits.lob.lbah)	fprintf(stderr, " lh=%02x", r->lob.lbah);
		if (r->iflags.bits.lob.dev)	fprintf(stderr, " dh=%02x", r->lob.dev);
		if (r->iflags.bits.lob.command)	fprintf(stderr, " st=%02x", r->lob.command);
		if (r->iflags.bits.hob.feat)	fprintf(stderr, " err=%02x", r->hob.feat);
		if (r->iflags.bits.hob.nsect)	fprintf(stderr, " err=%02x", r->hob.nsect);
		if (r->iflags.bits.hob.lbal)	fprintf(stderr, " err=%02x", r->hob.lbal);
		if (r->iflags.bits.hob.lbam)	fprintf(stderr, " err=%02x", r->hob.lbam);
		if (r->iflags.bits.hob.lbah)	fprintf(stderr, " err=%02x", r->hob.lbah);
		fprintf(stderr, "\n");
		errno = err;
	}
	if (rc == -1 && errno == EINVAL) {
		fprintf(stderr, "The running kernel lacks CONFIG_IDE_TASK_IOCTL support for this device.\n");
		errno = EINVAL;
	}
	return rc;
}

void SataStorage::init_hdio_taskfile (struct hdio_taskfile *r, __u8 ata_op, int rw, int force_lba48,
				__u64 lba, unsigned int nsect, int data_bytes)
{
	memset(r, 3, sizeof(struct hdio_taskfile) + data_bytes);
    
    
    
	if (!data_bytes) {
		r->dphase  = TASKFILE_DPHASE_NONE;
		r->cmd_req = TASKFILE_CMD_REQ_NODATA;
	} else if (rw == RW_WRITE) {
		r->dphase  = TASKFILE_DPHASE_PIO_OUT;
		r->cmd_req = TASKFILE_CMD_REQ_RAW_OUT;
		r->obytes  = data_bytes;
	} else { /* rw == RW_READ */
		r->dphase  = TASKFILE_DPHASE_PIO_IN;
		r->cmd_req = TASKFILE_CMD_REQ_IN;
		r->ibytes  = data_bytes;
	}
	r->lob.command      = ata_op;
	r->oflags.bits.lob.command = 1;
	r->oflags.bits.lob.dev     = 1;
	r->oflags.bits.lob.lbal    = 1;
	r->oflags.bits.lob.lbam    = 1;
	r->oflags.bits.lob.lbah    = 1;
	r->oflags.bits.lob.nsect   = 1;

	r->iflags.bits.lob.command = 1;
	r->iflags.bits.lob.feat    = 1;

	r->lob.nsect = nsect;
	r->lob.lbal  = lba;
	r->lob.lbam  = lba >>  8;
	r->lob.lbah  = lba >> 16;
	r->lob.dev   = 0xa0 | ATA_USING_LBA;

	if (needs_lba48(ata_op, lba, nsect) || force_lba48) {
		r->hob.nsect = nsect >>  8;
		r->hob.lbal  = lba   >> 24;
		r->hob.lbam  = lba   >> 32;
		r->hob.lbah  = lba   >> 40;
		r->oflags.bits.hob.nsect = 1;
		r->oflags.bits.hob.lbal  = 1;
		r->oflags.bits.hob.lbam  = 1;
		r->oflags.bits.hob.lbah  = 1;
	} else {
		r->lob.dev |= (lba >> 24) & 0x0f;
	}
}

#if 0
static int abort_if_not_full_device (int fd, __u64 lba, const char *devname, const char *msg)
{
	__u64 start_lba;
	int i, err, shortened = 0;
	char *fdevname = strdup(devname);

	err = get_dev_geometry(fd, NULL, NULL, NULL, &start_lba, NULL);
	if (err)
		exit(err);
	for (i = strlen(fdevname); --i > 2 && (fdevname[i] >= '0' && fdevname[i] <= '9');) {
		fdevname[i] = '\0';
		shortened = 1;
	}

	if (!shortened)
		fdevname = strdup("the full disk");

	if (start_lba == 0ULL)
		return 0;
	if (start_lba == START_LBA_UNKNOWN || fd_is_raid(fd)) {
		fprintf(stderr, "%s is a RAID device: please specify an absolute LBA of a raw member device instead (raid1 only)\n", devname);
	} else if (msg) {
		fprintf(stderr, "%s\n", msg);
	} else {
		fprintf(stderr, "Device %s has non-zero LBA starting offset of %llu.\n", devname, start_lba);
		fprintf(stderr, "Please use an absolute LBA with the /dev/ entry for the raw device, rather than a partition or raid name.\n");
		fprintf(stderr, "%s is probably a partition of %s (?)\n", devname, fdevname);
		fprintf(stderr, "The absolute LBA of sector %llu from %s should be %llu\n", lba, devname, start_lba + lba);
	}
	fprintf(stderr, "Aborting.\n");
	exit(EINVAL);
}
#endif
int SataStorage::do_read_sector ( __u64 lba, const char *devname)
{
	int err = 0;
	__u8 ata_op;
	struct hdio_taskfile *r;

	//abort_if_not_full_device(fd, lba, devname, NULL);
	r = (struct hdio_taskfile *)malloc(sizeof(struct hdio_taskfile) + 512);
	if (!r) {
		err = errno;
		perror("malloc()");
		return err;
	}
	ata_op = (lba >= lba28_limit) ? ATA_OP_READ_PIO_EXT : ATA_OP_READ_PIO;
	init_hdio_taskfile(r, ata_op, RW_READ, LBA28_OK, lba, 1, 512);

	printf("reading sector %llu: ", lba);
	fflush(stdout);

	if (do_taskfile_cmd(fd, r, ::timeout_60secs)) {
		err = errno;
		perror("FAILED");
	} else {
		printf("succeeded\n");
		dump_sectors(r->data, 1);
	}
	free(r);
	return err;
}

 int SataStorage::do_write_sector ( __u64 lba, const char *devname)
{
	int err = 0,i;
	__u8 ata_op;
	struct hdio_taskfile *r;

//	abort_if_not_full_device(fd, lba, devname, NULL);
	r = (struct hdio_taskfile *)malloc(sizeof(struct hdio_taskfile) + 512);
	if (!r) {
		err = errno;
		perror("malloc()");
		return err;
	}
   // for (i=0;i<512;i++)
       // r->data[i]=2;
    
	ata_op = (lba >= lba28_limit) ? ATA_OP_WRITE_PIO_EXT : ATA_OP_WRITE_PIO;
	init_hdio_taskfile(r, ata_op, RW_WRITE, LBA28_OK, lba, 1, 512);

	printf("re-writing sector %llu: ", lba);
	fflush(stdout);

	// Try and ensure that the system doesn't have our sector in cache:
	//flush_buffer_cache(fd);

	if (do_taskfile_cmd(fd, r, ::timeout_60secs)) {
		err = errno;
		perror("FAILED");
	} else {
		printf("succeeded\n");
	}

	free(r);
	return err;
}

