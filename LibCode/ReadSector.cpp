#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <scsi/scsi.h>
#include <scsi/sg.h>

#include "sgio.h"
#include "hdparm.h"

#include <linux/hdreg.h>
extern __u16 *id;
extern int verbose;

//extern inline int needs_lba48 (__u8 ata_op, __u64 lba, unsigned int nsect);
int timeout_60secs = 60;

// extern void flush_buffer_cache (int fd);

class SataStorage;

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
#if 0
static inline int needs_lba48 (__u8 ata_op, __u64 lba, unsigned int nsect)
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
#endif
void SataStorage::init_hdio_taskfile (struct hdio_taskfile *r, __u8 ata_op, int rw, int force_lba48,
				__u64 lba, unsigned int nsect, int data_bytes)
{
	memset(r, 1, sizeof(struct hdio_taskfile) + data_bytes);
    
    
    
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
int SataStorage::do_read_sector (int fd, __u64 lba, const char *devname)
{
	int err = 0;
	__u8 ata_op;
	struct hdio_taskfile *r;

	//abort_if_not_full_device(fd, lba, devname, NULL);
	r = malloc(sizeof(struct hdio_taskfile) + 512);
	if (!r) {
		err = errno;
		perror("malloc()");
		return err;
	}
	ata_op = (lba >= lba28_limit) ? ATA_OP_READ_PIO_EXT : ATA_OP_READ_PIO;
	init_hdio_taskfile(r, ata_op, RW_READ, LBA28_OK, lba, 1, 512);

	printf("reading sector %llu: ", lba);
	fflush(stdout);

	if (do_taskfile_cmd(fd, r, timeout_60secs)) {
		err = errno;
		perror("FAILED");
	} else {
		printf("succeeded\n");
		dump_sectors(r->data, 1);
	}
	free(r);
	return err;
}

 int SataStorage::do_write_sector (int fd, __u64 lba, const char *devname)
{
	int err = 0,i;
	__u8 ata_op;
	struct hdio_taskfile *r;

//	abort_if_not_full_device(fd, lba, devname, NULL);
	r = malloc(sizeof(struct hdio_taskfile) + 512);
	if (!r) {
		err = errno;
		perror("malloc()");
		return err;
	}
    //for (i=0;i<512;i++)
       // r->data[i]=1;
    
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
