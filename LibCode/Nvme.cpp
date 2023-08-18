#include <endian.h>
#include <errno.h>
#include <getopt.h>
#include <fcntl.h>
#include <inttypes.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <ctype.h>
//#ifdef LIBUDEV_EXISTS
#include <libudev.h>
////#endif
//
#include <linux/fs.h>
//
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
//
#include "nvme-ioctl.h"
#include "Nvmehdr.h"
extern int fd;
struct stat nvme_stat;
#define POWER_SCALE(s) ((s) >> 6)

char END_DEFAULT[] = "__end_default__";


int NvmeStorage::nvme_submit_admin_passthru(int fd, struct nvme_passthru_cmd *cmd)
{
	return ioctl(fd, NVME_IOCTL_ADMIN_CMD, cmd);
}


const char *NvmeStorage::nvme_status_to_string(__u32 status)
{
	switch (status & 0x3ff) {
	case NVME_SC_SUCCESS:		return "SUCCESS";
	case NVME_SC_INVALID_OPCODE:	return "INVALID_OPCODE";
	case NVME_SC_INVALID_FIELD:	return "INVALID_FIELD";
	case NVME_SC_CMDID_CONFLICT:	return "CMDID_CONFLICT";
	case NVME_SC_DATA_XFER_ERROR:	return "DATA_XFER_ERROR";
	case NVME_SC_POWER_LOSS:	return "POWER_LOSS";
	case NVME_SC_INTERNAL:		return "INTERNAL";
	case NVME_SC_ABORT_REQ:		return "ABORT_REQ";
	case NVME_SC_ABORT_QUEUE:	return "ABORT_QUEUE";
	case NVME_SC_FUSED_FAIL:	return "FUSED_FAIL";
	case NVME_SC_FUSED_MISSING:	return "FUSED_MISSING";
	case NVME_SC_INVALID_NS:	return "INVALID_NS";
	case NVME_SC_CMD_SEQ_ERROR:	return "CMD_SEQ_ERROR";
	case NVME_SC_LBA_RANGE:		return "LBA_RANGE";
	case NVME_SC_CAP_EXCEEDED:	return "CAP_EXCEEDED";
	case NVME_SC_NS_NOT_READY:	return "NS_NOT_READY";
	case NVME_SC_CQ_INVALID:	return "CQ_INVALID";
	case NVME_SC_QID_INVALID:	return "QID_INVALID";
	case NVME_SC_QUEUE_SIZE:	return "QUEUE_SIZE";
	case NVME_SC_ABORT_LIMIT:	return "ABORT_LIMIT";
	case NVME_SC_ABORT_MISSING:	return "ABORT_MISSING";
	case NVME_SC_ASYNC_LIMIT:	return "ASYNC_LIMIT";
	case NVME_SC_FIRMWARE_SLOT:	return "FIRMWARE_SLOT";
	case NVME_SC_FIRMWARE_IMAGE:	return "FIRMWARE_IMAGE";
	case NVME_SC_INVALID_VECTOR:	return "INVALID_VECTOR";
	case NVME_SC_INVALID_LOG_PAGE:	return "INVALID_LOG_PAGE";
	case NVME_SC_INVALID_FORMAT:	return "INVALID_FORMAT";
	case NVME_SC_BAD_ATTRIBUTES:	return "BAD_ATTRIBUTES";
	case NVME_SC_WRITE_FAULT:	return "WRITE_FAULT";
	case NVME_SC_READ_ERROR:	return "READ_ERROR";
	case NVME_SC_GUARD_CHECK:	return "GUARD_CHECK";
	case NVME_SC_APPTAG_CHECK:	return "APPTAG_CHECK";
	case NVME_SC_REFTAG_CHECK:	return "REFTAG_CHECK";
	case NVME_SC_COMPARE_FAILED:	return "COMPARE_FAILED";
	case NVME_SC_ACCESS_DENIED:	return "ACCESS_DENIED";
	default:			return "Unknown";
	}
}

void NvmeStorage::open_dev(const char *dev)
{
	int err;
	devicename = basename(dev);
	fd = open(dev, O_RDONLY);
	if (fd < 0)
		goto perror;

	err = fstat(fd, &nvme_stat);
	if (err < 0)
		goto perror;
	if (!S_ISCHR(nvme_stat.st_mode) && !S_ISBLK(nvme_stat.st_mode)) {
		fprintf(stderr, "%s is not a block or character device\n", dev);
		exit(ENODEV);
	}
	return;
 perror:
 	ErrorCode=1;
	perror(dev);
	exit(errno);
}


long double NvmeStorage:: int128_to_double(__u8 *data)
{
	int i;
	long double result = 0;

	for (i = 0; i < 16; i++) {
		result *= 256;
		result += data[15 - i];
	}
	return result;
}


int NvmeStorage::nvme_identify_ns(int fd, __u32 nsid, bool present, void *data)
{
	int cns = present ? NVME_ID_CNS_NS_PRESENT : NVME_ID_CNS_NS;

	return nvme_identify(fd, nsid, cns, data);
}

int NvmeStorage::nvme_identify(int fd, __u32 nsid, __u32 cdw10, void *data)
{
	struct nvme_admin_cmd cmd={
        
	//	.opcode =		 nvme_admin_identify,
	//	.nsid	=	 nsid,
	//	.addr=		 (__u64)(uintptr_t) data,
	//	.data_len=	 0x1000,
	//	.cdw10	=	 cdw10

	.opcode = nvme_admin_identify,
		.flags=0,
		.rsvd1=0,
		.nsid=nsid,
		.cdw2=0,
		.cdw3=0,
		.metadata=0,
		.addr=(__u64)(uintptr_t)data,
		.metadata_len=0,
		.data_len=0x1000,
		.cdw10=cdw10,
		.cdw11=0,
		.cdw12=0,
		.cdw13=0,
		.cdw14=0,
		.cdw15=0,
		.timeout_ms=0,
		.result=0
	};

	return nvme_submit_admin_passthru(fd, &cmd);
}

int NvmeStorage::Init(char **argv)
{
     ErrorCode=0;
     strcpy(arg, argv[0]);    
     open_dev(argv[0]);
        //std::cout << "NvmeStorage Init" << std::endl;   
     return ErrorCode; 
}

long long NvmeStorage::elapsed_utime(struct timeval start_time,
					struct timeval end_time)
{
	unsigned long long ret = (end_time.tv_sec - start_time.tv_sec)*1000000 +
		(end_time.tv_usec - start_time.tv_usec);
	return ret;
}
int NvmeStorage::argconfig_parse_subopt_string(char *string, char **options,
				  size_t max_options)
{
	char **o = options;
	char *tmp;

	if (!string || !strlen(string)) {
		*(o++) = NULL;
		*(o++) = NULL;
		return 0;
	}

	tmp = (char*)calloc(strlen(string) + 2, 1);
	strcpy(tmp, string);

	size_t toklen;
	toklen = strcspn(tmp, "=");

	if (!toklen) {
		free(tmp);
		return 1;
	}

	*(o++) = tmp;
	tmp[toklen] = 0;
	tmp += toklen + 1;

	while (1) {
		if (*tmp == '"' || *tmp == '\'' || *tmp == '[' || *tmp == '(' ||
		    *tmp == '{') {

			tmp++;
			toklen = strcspn(tmp, "\"'])}");

			if (!toklen)
				return 1;

			*(o++) = tmp;
			tmp[toklen] = 0;
			tmp += toklen + 1;

			toklen = strcspn(tmp, ";:,");
			tmp[toklen] = 0;
			tmp += toklen + 1;
		} else {
			toklen = strcspn(tmp, ";:,");

			if (!toklen)
				return 1;

			*(o++) = tmp;
			tmp[toklen] = 0;
			tmp += toklen + 1;
		}

		toklen = strcspn(tmp, "=");

		if (!toklen)
			break;

		*(o++) = tmp;
		tmp[toklen] = 0;
		tmp += toklen + 1;

		if ((o - options) > (max_options - 2))
			return 2;
	}

	*(o++) = NULL;
	*(o++) = NULL;

	return 0;
}

int NvmeStorage::argconfig_parse(int argc, char *argv[], const char *program_desc,
		    const struct argconfig_commandline_options *options,
		    void *config_out, size_t config_size)
{
	char *short_opts;
	char *endptr;
	struct option *long_opts;
	const struct argconfig_commandline_options *s;
	int c, option_index = 0, short_index = 0, options_count = 0;
	void *value_addr;

	errno = 0;
	for (s = options; s->option != 0; s++)
		options_count++;

	long_opts = (option *)malloc(sizeof(struct option) * (options_count + 2));
	short_opts =(char*) malloc(sizeof(*short_opts) * (options_count * 3 + 4));

	for (s = options; (s->option != 0) && (option_index < options_count);
	     s++) {
		if (s->short_option != 0) {
			short_opts[short_index++] = s->short_option;
			if (s->argument_type == required_argument ||
			    s->argument_type == optional_argument)
				short_opts[short_index++] = ':';
			if (s->argument_type == optional_argument)
				short_opts[short_index++] = ':';
		}
		if (s->option && strlen(s->option)) {
			long_opts[option_index].name = s->option;
			long_opts[option_index].has_arg = s->argument_type;

			if (s->argument_type == no_argument
			    && s->default_value != NULL) {
				value_addr = (void *)(char *)s->default_value;

				long_opts[option_index].flag = (int *)value_addr;
				long_opts[option_index].val = 1;
			} else {
				long_opts[option_index].flag = NULL;
				long_opts[option_index].val = 0;
			}
		}
		option_index++;
	}

	long_opts[option_index].name = "help";
	long_opts[option_index].flag = NULL;
	long_opts[option_index].val = 'h';
	option_index++;

	long_opts[option_index].name = NULL;
	long_opts[option_index].flag = NULL;
	long_opts[option_index].val = 0;

	short_opts[short_index++] = '?';
	short_opts[short_index++] = 'h';
	short_opts[short_index] = 0;

	while ((c = getopt_long_only(argc, argv, short_opts, long_opts,
				&option_index)) != -1) {
		if (c != 0) {
			if (c == '?' || c == 'h') {
				; //argconfig_print_help(program_desc, options);
				goto exit;
			}
			for (option_index = 0; option_index < options_count;
			     option_index++) {
				if (c == options[option_index].short_option)
					break;
			}
			if (option_index == options_count)
				continue;
			if (long_opts[option_index].flag) {
				*(uint8_t *)(long_opts[option_index].flag) = 1;
				continue;
			}
		}

		s = &options[option_index];
		value_addr = (void *)(char *)s->default_value;
		if (s->config_type == CFG_STRING) {
			*((char **)value_addr) = optarg;
		} else if (s->config_type == CFG_SIZE) {
			*((size_t *) value_addr) = strtol(optarg, &endptr, 0);
			if (errno || optarg == endptr) {
				fprintf(stderr,
					"Expected integer argument for '%s' but got '%s'!\n",
					long_opts[option_index].name, optarg);
				goto exit;
			}
		} else if (s->config_type == CFG_INT) {
			*((int *)value_addr) = strtol(optarg, &endptr, 0);
			if (errno || optarg == endptr) {
				fprintf(stderr,
					"Expected integer argument for '%s' but got '%s'!\n",
					long_opts[option_index].name, optarg);
				goto exit;
			}
		} else if (s->config_type == CFG_BOOL) {
			int tmp = strtol(optarg, &endptr, 0);
			if (errno || tmp < 0 || tmp > 1 || optarg == endptr) {
				fprintf(stderr,
					"Expected 0 or 1 argument for '%s' but got '%s'!\n",
					long_opts[option_index].name, optarg);
				goto exit;
			}
			*((int *)value_addr) = tmp;
		} else if (s->config_type == CFG_BYTE) {
			uint8_t tmp = strtol(optarg, &endptr, 0);
			if (errno || tmp < 0 || optarg == endptr) {
				fprintf(stderr,
					"Expected positive argument for '%s' but got '%s'!\n",
					long_opts[option_index].name, optarg);
				goto exit;
			}
			*((uint8_t *) value_addr) = tmp;
		} else if (s->config_type == CFG_SHORT) {
			uint16_t tmp = strtol(optarg, &endptr, 0);
			if (errno || tmp < 0 || optarg == endptr) {
				fprintf(stderr,
					"Expected positive argument for '%s' but got '%s'!\n",
					long_opts[option_index].name, optarg);
				goto exit;
			}
			*((uint16_t *) value_addr) = tmp;
		} else if (s->config_type == CFG_POSITIVE) {
			uint32_t tmp = strtol(optarg, &endptr, 0);
			if (errno || tmp < 0 || optarg == endptr) {
				fprintf(stderr,
					"Expected positive argument for '%s' but got '%s'!\n",
					long_opts[option_index].name, optarg);
				goto exit;
			}
			*((uint32_t *) value_addr) = tmp;
		} else if (s->config_type == CFG_INCREMENT) {
			(*((int *)value_addr))++;
		} else if (s->config_type == CFG_LONG) {
			*((long *)value_addr) = strtol(optarg, &endptr, 0);
			if (errno || optarg == endptr) {
				fprintf(stderr,
					"Expected long integer argument for '%s' but got '%s'!\n",
					long_opts[option_index].name, optarg);
				goto exit;
			}
		} else if (s->config_type == CFG_LONG_SUFFIX) {
			*((long *)value_addr) = suffix_binary_parse(optarg);
						printf("CFG_LONG_SUFFIX %d:", *((long *)value_addr));
			if (errno) {
				fprintf(stderr,
					"Expected long suffixed integer argument for '%s' but got '%s'!\n",
					long_opts[option_index].name, optarg);
				goto exit;
			}
		} else if (s->config_type == CFG_DOUBLE) {
			*((double *)value_addr) = strtod(optarg, &endptr);
			if (errno || optarg == endptr) {
				fprintf(stderr,
					"Expected float argument for '%s' but got '%s'!\n",
					long_opts[option_index].name, optarg);
				goto exit;
			}
		} else if (s->config_type == CFG_SUBOPTS) {
			char **opts = ((char **)value_addr);
			int remaining_space = CFG_MAX_SUBOPTS;
			int enddefault = 0;
			while (0 && *opts != NULL) {
				if (*opts == END_DEFAULT)
					enddefault = 1;
				remaining_space--;
				opts++;
			}

			if (!enddefault) {
				*opts = END_DEFAULT;
				remaining_space -= 2;
				opts += 2;
			}

			int r =
			    argconfig_parse_subopt_string(optarg, opts,
							  remaining_space);
			if (r == 2) {
				fprintf(stderr,
					"Error Parsing Sub-Options: Too many options!\n");
				goto exit;
			} else if (r) {
				fprintf(stderr, "Error Parsing Sub-Options\n");
				goto exit;
			}
		} else if (s->config_type == CFG_FILE_A ||
			   s->config_type == CFG_FILE_R ||
			   s->config_type == CFG_FILE_W ||
			   s->config_type == CFG_FILE_AP ||
			   s->config_type == CFG_FILE_RP ||
			   s->config_type == CFG_FILE_WP) {
			const char *fopts = "";
			if (s->config_type == CFG_FILE_A)
				fopts = "a";
			else if (s->config_type == CFG_FILE_R)
				fopts = "r";
			else if (s->config_type == CFG_FILE_W)
				fopts = "w";
			else if (s->config_type == CFG_FILE_AP)
				fopts = "a+";
			else if (s->config_type == CFG_FILE_RP)
				fopts = "r+";
			else if (s->config_type == CFG_FILE_WP)
				fopts = "w+";

			FILE *f = fopen(optarg, fopts);
			if (f == NULL) {
				fprintf(stderr, "Unable to open %s file: %s\n",
					s->option, optarg);
				goto exit;
			}
			*((FILE **) value_addr) = f;
		}
	}
	free(short_opts);
	free(long_opts);

	return 0;
 exit:
	free(short_opts);
	free(long_opts);
	exit(1);
}

void NvmeStorage:: check_arg_dev(int argc, char **argv)
{
	if (optind >= argc) {
		errno = EINVAL;
		perror(argv[0]);
		exit(errno);
	}
}

void NvmeStorage:: get_dev(int argc, char **argv)
{
	check_arg_dev(argc, argv);
	open_dev((const char *)argv[optind]);
}

int NvmeStorage::parse_and_open(int argc, char **argv, const char *desc,
	const struct argconfig_commandline_options *clo, void *cfg, size_t size)
{
	argconfig_parse(argc, argv, desc, clo, cfg, size);
	//get_dev(argc, argv);
	return fd;
}


int NvmeStorage::nvme_io(int fd, __u8 opcode, __u64 slba, __u16 nblocks, __u16 control,
	    __u32 dsmgmt, __u32 reftag, __u16 apptag, __u16 appmask, void *data,
	    void *metadata)
{

	struct nvme_user_io io = {
		.opcode		= opcode,
		.flags		= 0,
		.control	= control,
		.nblocks	= nblocks,
		.rsvd		= 0,
		.metadata	= (__u64)(uintptr_t) metadata,
		.addr		= (__u64)(uintptr_t) data,
		.slba		= slba,
		.dsmgmt		= dsmgmt,
		.reftag		= reftag,
		.apptag		= appmask,
		.appmask	= apptag
	};
	return ioctl(fd, NVME_IOCTL_SUBMIT_IO, &io);
}

int NvmeStorage::submit_io(int opcode, char *command, const char *desc,
		     int argc, char **argv)
{
	struct timeval start_time, end_time;
	void *buffer, *mbuffer = NULL;
	int err = 0;
	int dfd, mfd;
	int flags = opcode & 1 ? O_RDONLY : O_WRONLY | O_CREAT;
	int mode = S_IRUSR | S_IWUSR |S_IRGRP | S_IWGRP| S_IROTH;
	__u16 control = 0;

	const char *start_block = "64-bit addr of first block to access";
	const char *block_count = "number of blocks (zeroes based) on device to access";
	const char *data_size = "size of data in bytes";
	const char *metadata_size = "size of metadata in bytes";
	const char *ref_tag = "reference tag (for end to end PI)";
	const char *data = "data file";
	const char *metadata = "metadata file";
	const char *prinfo = "PI and check field";
	const char *app_tag_mask = "app tag mask (for end to end PI)";
	const char *app_tag = "app tag (for end to end PI)";
	const char *limited_retry = "limit num. media access attempts";
	const char *latency = "output latency statistics";
	const char *force = "force device to commit data before command completes";
	const char *show = "show command before sending";
	const char *dry = "show command instead of sending";

	struct config {
		__u64 start_block;
		__u16 block_count;
		__u64 data_size;
		__u64 metadata_size;
		__u32 ref_tag;
		char  *data;
		char  *metadata;
		__u8  prinfo;
		__u8  app_tag_mask;
		__u32 app_tag;
		int   limited_retry;
		int   force_unit_access;
		int   show;
		int   dry_run;
		int   latency;
	};

	struct config cfg = {
		.start_block     = 0,
		.block_count     = 0,
		.data_size       = 0,
		.metadata_size   = 0,
		.ref_tag         = 0,
		.data            = "",
		.metadata        = "",
		.prinfo          = 0,
		.app_tag_mask    = 0,
		.app_tag         = 0,
		.limited_retry	 = 0,
		.force_unit_access= 0,
		.show            = 0,
		.dry_run         = 0,
		.latency         = 0
	};

	const struct argconfig_commandline_options command_line_options[] = {
		{"start-block",       's', "NUM",  CFG_LONG_SUFFIX, &cfg.start_block,       required_argument, start_block},
		{"block-count",       'c', "NUM",  CFG_SHORT,       &cfg.block_count,       required_argument, block_count},
		{"data-size",         'z', "NUM",  CFG_LONG_SUFFIX, &cfg.data_size,         required_argument, data_size},
		{"metadata-size",     'y', "NUM",  CFG_LONG_SUFFIX, &cfg.metadata_size,     required_argument, metadata_size},
		{"ref-tag",           'r', "NUM",  CFG_POSITIVE,    &cfg.ref_tag,           required_argument, ref_tag},
		{"data",              'd', "FILE", CFG_STRING,      &cfg.data,              required_argument, data},
		{"metadata",          'M', "FILE", CFG_STRING,      &cfg.metadata,          required_argument, metadata},
		{"prinfo",            'p', "NUM",  CFG_BYTE,        &cfg.prinfo,            required_argument, prinfo},
		{"app-tag-mask",      'm', "NUM",  CFG_BYTE,        &cfg.app_tag_mask,      required_argument, app_tag_mask},
		{"app-tag",           'a', "NUM",  CFG_POSITIVE,    &cfg.app_tag,           required_argument, app_tag},
		{"limited-retry",     'l', "",     CFG_NONE,        &cfg.limited_retry,     no_argument,       limited_retry},
		{"force-unit-access", 'f', "",     CFG_NONE,        &cfg.force_unit_access, no_argument,       force},
		{"show-command",      'v', "",     CFG_NONE,        &cfg.show,              no_argument,       show},
		{"dry-run",           'w', "",     CFG_NONE,        &cfg.dry_run,           no_argument,       dry},
		{"latency",           't', "",     CFG_NONE,        &cfg.latency,           no_argument,       latency},
	};

	parse_and_open(argc, argv, desc, command_line_options, &cfg, sizeof(cfg));

	dfd = mfd = opcode & 1 ? STDIN_FILENO : STDOUT_FILENO;
	if (cfg.prinfo > 0xf)
		return EINVAL;
	control |= (cfg.prinfo << 10);
	if (cfg.limited_retry)
		control |= NVME_RW_LR;
	if (cfg.force_unit_access)
		control |= NVME_RW_FUA;
	if (strlen(cfg.data)){
		dfd = open(cfg.data, flags, mode);
		if (dfd < 0) {
			perror(cfg.data);
			return EINVAL;
		}
		mfd = dfd;
	}
	if (strlen(cfg.metadata)){
		mfd = open(cfg.metadata, flags, mode);
		if (mfd < 0) {
			perror(cfg.data);
			return EINVAL;
		}
	}

	if (!cfg.data_size)	{
		fprintf(stderr, "data size not provided\n");
		return EINVAL;
	}

	if (posix_memalign(&buffer, getpagesize(), cfg.data_size))
		return ENOMEM;
	memset(buffer, 0, cfg.data_size);

	if (cfg.metadata_size) {
		mbuffer = malloc(cfg.metadata_size);
		if (!mbuffer) {
 			free(buffer);
			return ENOMEM;
		}
	}

	if ((opcode & 1) && read(dfd, (void *)buffer, cfg.data_size) < 0) {
		fprintf(stderr, "failed to read data buffer from input file\n");
		err = EINVAL;
		goto free_and_return;
	}

	if ((opcode & 1) && cfg.metadata_size &&
				read(mfd, (void *)mbuffer, cfg.metadata_size) < 0) {
		fprintf(stderr, "failed to read meta-data buffer from input file\n");
		err = EINVAL;
		goto free_and_return;
	}

	if (cfg.show) {
		printf("opcode       : %02x\n", opcode);
		printf("flags        : %02x\n", 0);
		printf("control      : %04x\n", control);
		printf("nblocks      : %04x\n", cfg.block_count);
		printf("rsvd         : %04x\n", 0);
		//printf("metadata     : %"PRIx64"\n", (uint64_t)(uintptr_t)mbuffer);
		//printf("addr         : %"PRIx64"\n", (uint64_t)(uintptr_t)buffer);
		//printf("sbla         : %"PRIx64"\n", (uint64_t)cfg.start_block);
		printf("dsmgmt       : %08x\n", 0);
		printf("reftag       : %08x\n", cfg.ref_tag);
		printf("apptag       : %04x\n", cfg.app_tag);
		printf("appmask      : %04x\n", cfg.app_tag_mask);
		if (cfg.dry_run)
			goto free_and_return;
	}

	gettimeofday(&start_time, NULL);
	err = nvme_io(fd, opcode, cfg.start_block, cfg.block_count, control, 0,
			cfg.ref_tag, cfg.app_tag, cfg.app_tag_mask, buffer, mbuffer);
	gettimeofday(&end_time, NULL);
	if (cfg.latency)
		printf(" latency: %s: %llu us\n",
			command, elapsed_utime(start_time, end_time));
	if (err < 0)
		perror("submit-io");
	else if (err)
		printf("%s:%s(%04x)\n", command, nvme_status_to_string(err), err);
	else {
		if (!(opcode & 1) && write(dfd, (void *)buffer, cfg.data_size) < 0) {
			fprintf(stderr, "failed to write buffer to output file\n");
			err = EINVAL;
			goto free_and_return;
		} else if (!(opcode & 1) && cfg.metadata_size &&
				write(mfd, (void *)mbuffer, cfg.metadata_size) < 0) {
			fprintf(stderr, "failed to write meta-data buffer to output file\n");
			err = EINVAL;
			goto free_and_return;
		} else
			fprintf(stderr, "%s: Success\n", command);
	}
 free_and_return:
	free(buffer);
	if (cfg.metadata_size)
		free(mbuffer);
	return err;
}



int NvmeStorage::read_cmd(int argc, char **argv)
{
	const char *desc = "Copy specified logical blocks on the given "\
		"device to specified data buffer (default buffer is stdout).";
	return submit_io(nvme_cmd_read, "read", desc, argc, argv);
}

int NvmeStorage::write_cmd(int argc, char **argv)
{
	const char *desc = "Copy from provided data buffer (default "\
		"buffer is stdin) to specified logical blocks on the given "\
		"device.";
	return submit_io(nvme_cmd_write, "write", desc, argc, argv);
}

int NvmeStorage::compare(int argc, char **argv)
{
	const char *desc = "Compare specified logical blocks on "\
		"device with specified data buffer; return failure if buffer "\
		"and block(s) are dissimilar";
	return submit_io(nvme_cmd_compare, "compare", desc, argc, argv);
}

int NvmeStorage::nvme_identify_ctrl(void *data)
{
	return nvme_identify(fd, 0, 1, data);
}


void NvmeStorage::d(unsigned char *buf, int len, int width, int group)
{
	int i, offset = 0, line_done = 0;
	char ascii[width + 1];

	printf("     ");
	for (i = 0; i <= 15; i++)
		printf("%3x", i);
	for (i = 0; i < len; i++) {
		line_done = 0;
		if (i % width == 0)
			printf( "\n%04x:", offset);
		if (i % group == 0)
			printf( " %02x", buf[i]);
		else
			printf( "%02x", buf[i]);
		ascii[i % width] = (buf[i] >= '!' && buf[i] <= '~') ? buf[i] : '.';
		if (((i + 1) % width) == 0) {
			ascii[i % width + 1] = '\0';
			printf( " \"%.*s\"", width, ascii);
			offset += width;
			line_done = 1;
		}
	}
	if (!line_done) {
		unsigned b = width - (i % width);
		ascii[i % width + 1] = '\0';
		printf( " %*s \"%.*s\"",
				2 * b + b / group + (b % group ? 1 : 0), "",
				width, ascii);
	}
	printf( "\n");
}
void NvmeStorage::show_nvme_id_ctrl_oaes(__le32 ctrl_oaes)
{
	__u32 oaes = le32toh(ctrl_oaes);
	__u32 rsvd0 = (oaes & 0xFFFFFE00) >> 9;
	__u32 nace = (oaes & 0x100) >> 8;
	__u32 rsvd1 = oaes & 0xFF;

	if (rsvd0)
		printf(" [31:9] : %#x\tReserved\n", rsvd0);
	printf("  [8:8] : %#x\tNamespace Attribute Changed Event %sSupported\n",
		nace, nace ? "" : "Not ");
	if (rsvd1)
		printf("  [7:0] : %#x\tReserved\n", rsvd1);
	printf("\n");
}

void NvmeStorage::show_nvme_id_ctrl_oacs(__le16 ctrl_oacs)
{
	__u16 oacs = le16toh(ctrl_oacs);
	__u16 rsvd = (oacs & 0xFFF0) >> 4;
	__u16 nsm = (oacs & 0x8) >> 3;
	__u16 fwc = (oacs & 0x4) >> 2;
	__u16 fmt = (oacs & 0x2) >> 1;
	__u16 sec = oacs & 0x1;

	if (rsvd)
		printf(" [15:4] : %#x\tReserved\n", rsvd);
	printf("  [3:3] : %#x\tNS Management and Attachment %sSupported\n",
		nsm, nsm ? "" : "Not ");
	printf("  [2:2] : %#x\tFW Commit and Download %sSupported\n",
		fwc, fwc ? "" : "Not ");
	printf("  [1:1] : %#x\tFormat NVM %sSupported\n",
		fmt, fmt ? "" : "Not ");
	printf("  [0:0] : %#x\tSec. Send and Receive %sSupported\n",
		sec, sec ? "" : "Not ");
	printf("\n");
}

void NvmeStorage::show_nvme_id_ctrl_frmw(__u8 frmw)
{
	__u8 rsvd = (frmw & 0xE0) >> 5;
	__u8 fawr = (frmw & 0x10) >> 4;
	__u8 nfws = (frmw & 0xE) >> 1;
	__u8 s1ro = frmw & 0x1;
	if (rsvd)
		printf("  [7:5] : %#x\tReserved\n", rsvd);
	printf("  [4:4] : %#x\tFirmware Activate Without Reset %sSupported\n",
		fawr, fawr ? "" : "Not ");
	printf("  [3:1] : %#x\tNumber of Firmware Slots\n", nfws);
	printf("  [0:0] : %#x\tFirmware Slot 1 Read%s\n",
		s1ro, s1ro ? "-Only" : "/Write");
	printf("\n");
}

void NvmeStorage::show_nvme_id_ctrl_lpa(__u8 lpa)
{
	__u8 rsvd = (lpa & 0xFC) >> 2;
	__u8 celp = (lpa & 0x2) >> 1;
	__u8 smlp = lpa & 0x1;
	if (rsvd)
		printf("  [7:2] : %#x\tReserved\n", rsvd);
	printf("  [1:1] : %#x\tCommand Effects Log Page %sSupported\n",
		celp, celp ? "" : "Not ");
	printf("  [0:0] : %#x\tSMART/Health Log Page per NS %sSupported\n",
		smlp, smlp ? "" : "Not ");
	printf("\n");
}

void NvmeStorage::show_nvme_id_ctrl_avscc(__u8 avscc)
{
	__u8 rsvd = (avscc & 0xFE) >> 1;
	__u8 fmt = avscc & 0x1;
	if (rsvd)
		printf("  [7:1] : %#x\tReserved\n", rsvd);
	printf("  [0:0] : %#x\tAdmin Vendor Specific Commands uses %s Format\n",
		fmt, fmt ? "NVMe" : "Vendor Specific");
	printf("\n");
}

void NvmeStorage::show_nvme_id_ctrl_apsta(__u8 apsta)
{
	__u8 rsvd = (apsta & 0xFE) >> 1;
	__u8 apst = apsta & 0x1;
	if (rsvd)
		printf("  [7:1] : %#x\tReserved\n", rsvd);
	printf("  [0:0] : %#x\tAutonomous Power State Transitions %sSupported\n",
		apst, apst ? "" : "Not ");
	printf("\n");
}

void NvmeStorage::show_nvme_id_ctrl_rpmbs(__le32 ctrl_rpmbs)
{
	__u32 rpmbs = le32toh(ctrl_rpmbs);
	__u32 asz = (rpmbs & 0xFF000000) >> 24;
	__u32 tsz = (rpmbs & 0xFF0000) >> 16;
	__u32 rsvd = (rpmbs & 0xFFC0) >> 6;
	__u32 auth = (rpmbs & 0x38) >> 3;
	__u32 rpmb = rpmbs & 0x3;

	printf(" [31:24]: %#x\tAccess Size\n", asz);
	printf(" [23:16]: %#x\tTotal Size\n", tsz);
	if (rsvd)
		printf(" [15:6] : %#x\tReserved\n", rsvd);
	printf("  [5:3] : %#x\tAuthentication Method\n", auth);
	printf("  [2:0] : %#x\tNumber of RPMB Units\n", rpmb);
	printf("\n");
}

void NvmeStorage::show_nvme_id_ctrl_sqes(__u8 sqes)
{
	__u8 msqes = (sqes & 0xF0) >> 4;
	__u8 rsqes = sqes & 0xF;
	printf("  [7:4] : %#x\tMax SQ Entry Size (%d)\n", msqes, 1 << msqes);
	printf("  [3:0] : %#x\tMin SQ Entry Size (%d)\n", rsqes, 1 << rsqes);
	printf("\n");
}

void NvmeStorage::show_nvme_id_ctrl_cqes(__u8 cqes)
{
	__u8 mcqes = (cqes & 0xF0) >> 4;
	__u8 rcqes = cqes & 0xF;
	printf("  [7:4] : %#x\tMax CQ Entry Size (%d)\n", mcqes, 1 << mcqes);
	printf("  [3:0] : %#x\tMin CQ Entry Size (%d)\n", rcqes, 1 << rcqes);
	printf("\n");
}

void NvmeStorage::show_nvme_id_ctrl_oncs(__le16 ctrl_oncs)
{
	__u16 oncs = le16toh(ctrl_oncs);
	__u16 rsvd = (oncs & 0xFFC0) >> 6;
	__u16 resv = (oncs & 0x20) >> 5;
	__u16 save = (oncs & 0x10) >> 4;
	__u16 wzro = (oncs & 0x8) >> 3;
	__u16 dsms = (oncs & 0x4) >> 2;
	__u16 wunc = (oncs & 0x2) >> 1;
	__u16 cmp = oncs & 0x1;

	if (rsvd)
		printf(" [15:6] : %#x\tReserved\n", rsvd);
	printf("  [5:5] : %#x\tReservations %sSupported\n",
		resv, resv ? "" : "Not ");
	printf("  [4:4] : %#x\tSave and Select %sSupported\n",
		save, save ? "" : "Not ");
	printf("  [3:3] : %#x\tWrite Zeroes %sSupported\n",
		wzro, wzro ? "" : "Not ");
	printf("  [2:2] : %#x\tData Set Management %sSupported\n",
		dsms, dsms ? "" : "Not ");
	printf("  [1:1] : %#x\tWrite Uncorrectable %sSupported\n",
		wunc, wunc ? "" : "Not ");
	printf("  [0:0] : %#x\tCompare %sSupported\n",
		cmp, cmp ? "" : "Not ");
	printf("\n");
}

void NvmeStorage::show_nvme_id_ctrl_fuses(__le16 ctrl_fuses)
{
	__u16 fuses = le16toh(ctrl_fuses);
	__u16 rsvd = (fuses & 0xFE) >> 1;
	__u16 cmpw = fuses & 0x1;

	if (rsvd)
		printf(" [15:1] : %#x\tReserved\n", rsvd);
	printf("  [0:0] : %#x\tFused Compare and Write %sSupported\n",
		cmpw, cmpw ? "" : "Not ");
	printf("\n");
}

void NvmeStorage::show_nvme_id_ctrl_fna(__u8 fna)
{
	__u8 rsvd = (fna & 0xF8) >> 3;
	__u8 cese = (fna & 0x4) >> 2;
	__u8 cens = (fna & 0x2) >> 1;
	__u8 fmns = fna & 0x1;
	if (rsvd)
		printf("  [7:3] : %#x\tReserved\n", rsvd);
	printf("  [2:2] : %#x\tCrypto Erase %sSupported as part of Secure Erase\n",
		cese, cese ? "" : "Not ");
	printf("  [1:1] : %#x\tCrypto Erase Applies to %s Namespace(s)\n",
		cens, cens ? "All" : "Single");
	printf("  [0:0] : %#x\tFormat Applies to %s Namespace(s)\n",
		fmns, fmns ? "All" : "Single");
	printf("\n");
}

void NvmeStorage::show_nvme_id_ctrl_vwc(__u8 vwc)
{
	__u8 rsvd = (vwc & 0xFE) >> 1;
	__u8 vwcp = vwc & 0x1;
	if (rsvd)
		printf("  [7:3] : %#x\tReserved\n", rsvd);
	printf("  [0:0] : %#x\tVolatile Write Cache %sPresent\n",
		vwcp, vwcp ? "" : "Not ");
	printf("\n");
}

void NvmeStorage::show_nvme_id_ctrl_nvscc(__u8 nvscc)
{
	__u8 rsvd = (nvscc & 0xFE) >> 1;
	__u8 fmt = nvscc & 0x1;
	if (rsvd)
		printf("  [7:1] : %#x\tReserved\n", rsvd);
	printf("  [0:0] : %#x\tNVM Vendor Specific Commands uses %s Format\n",
		fmt, fmt ? "NVMe" : "Vendor Specific");
	printf("\n");
}

void NvmeStorage::show_nvme_id_ctrl_sgls(__le32 ctrl_sgls)
{
	__u32 sgls = le32toh(ctrl_sgls);
	__u32 rsvd0 = (sgls & 0xFFF80000) >> 19;
	__u32 sglltb = (sgls & 0x40000) >> 18;
	__u32 bacmdb = (sgls & 0x20000) >> 17;
	__u32 bbs = (sgls & 0x10000) >> 16;
	__u32 rsvd1 = (sgls & 0xFFFE) >> 1;
	__u32 sglsp = sgls & 0x1;

	if (rsvd0)
		printf(" [31:19]: %#x\tReserved\n", rsvd0);
	if (sglsp || (!sglsp && sglltb))
		printf(" [18:18]: %#x\tSGL Length Larger than Buffer %sSupported\n",
			sglltb, sglltb ? "" : "Not ");
	if (sglsp || (!sglsp && bacmdb))
		printf(" [17:17]: %#x\tByte-Aligned Contig. MD Buffer %sSupported\n",
			bacmdb, bacmdb ? "" : "Not ");
	if (sglsp || (!sglsp && bbs))
		printf(" [16:16]: %#x\tSGL Bit-Bucket %sSupported\n",
			bbs, bbs ? "" : "Not ");
	if (rsvd1)
		printf(" [15:1] : %#x\tReserved\n", rsvd1);
	printf("  [0:0] : %#x\tScatter-Gather Lists %sSupported\n",
		sglsp, sglsp ? "" : "Not ");
	printf("\n");
}

void NvmeStorage::show_nvme_id_ns_nsfeat(__u8 nsfeat)
{
	__u8 rsvd = (nsfeat & 0xF8) >> 3;
	__u8 dulbe = (nsfeat & 0x4) >> 2;
	__u8 na = (nsfeat & 0x2) >> 1;
	__u8 thin = nsfeat & 0x1;
	if (rsvd)
		printf("  [7:3] : %#x\tReserved\n", rsvd);
	printf("  [2:2] : %#x\tDeallocated or Unwritten Logical Block error %sSupported\n",
		dulbe, dulbe ? "" : "Not ");
	printf("  [1:1] : %#x\tNamespace uses %s\n",
		na, na ? "NAWUN, NAWUPF, and NACWU" : "AWUN, AWUPF, and ACWU");
	printf("  [0:0] : %#x\tThin Provisioning %sSupported\n",
		thin, thin ? "" : "Not ");
	printf("\n");
}

void NvmeStorage::show_nvme_id_ns_flbas(__u8 flbas)
{
	__u8 rsvd = (flbas & 0xE0) >> 5;
	__u8 mdedata = (flbas & 0x10) >> 4;
	__u8 lbaf = flbas & 0xF;
	if (rsvd)
		printf("  [7:5] : %#x\tReserved\n", rsvd);
	printf("  [4:4] : %#x\tMetadata Transferred %s\n",
		mdedata, mdedata ? "at End of Data LBA" : "in Separate Contiguous Buffer");
	printf("  [3:0] : %#x\tCurrent LBA Format Selected\n", lbaf);
	printf("\n");
}

void NvmeStorage::show_nvme_id_ns_mc(__u8 mc)
{
	__u8 rsvd = (mc & 0xFC) >> 2;
	__u8 mdp = (mc & 0x2) >> 1;
	__u8 extdlba = mc & 0x1;
	if (rsvd)
		printf("  [7:2] : %#x\tReserved\n", rsvd);
	printf("  [1:1] : %#x\tMetadata Pointer %sSupported\n",
		mdp, mdp ? "" : "Not ");
	printf("  [0:0] : %#x\tMetadata as Part of Extended Data LBA %sSupported\n",
		extdlba, extdlba ? "" : "Not ");
	printf("\n");
}

void NvmeStorage::show_nvme_id_ns_dpc(__u8 dpc)
{
	__u8 rsvd = (dpc & 0xE0) >> 5;
	__u8 pil8 = (dpc & 0x10) >> 4;
	__u8 pif8 = (dpc & 0x8) >> 3;
	__u8 pit3 = (dpc & 0x4) >> 2;
	__u8 pit2 = (dpc & 0x2) >> 1;
	__u8 pit1 = dpc & 0x1;
	if (rsvd)
		printf("  [7:5] : %#x\tReserved\n", rsvd);
	printf("  [4:4] : %#x\tProtection Information Transferred as Last 8 Bytes of Metadata %sSupported\n",
		pil8, pil8 ? "" : "Not ");
	printf("  [3:3] : %#x\tProtection Information Transferred as First 8 Bytes of Metadata %sSupported\n",
		pif8, pif8 ? "" : "Not ");
	printf("  [2:2] : %#x\tProtection Information Type 3 %sSupported\n",
		pit3, pit3 ? "" : "Not ");
	printf("  [1:1] : %#x\tProtection Information Type 2 %sSupported\n",
		pit2, pit2 ? "" : "Not ");
	printf("  [0:0] : %#x\tProtection Information Type 1 %sSupported\n",
		pit1, pit1 ? "" : "Not ");
	printf("\n");
}

void NvmeStorage::show_nvme_id_ns_dps(__u8 dps)
{
	__u8 rsvd = (dps & 0xF0) >> 4;
	__u8 pif8 = (dps & 0x8) >> 3;
	__u8 pit = dps & 0x7;
	if (rsvd)
		printf("  [7:4] : %#x\tReserved\n", rsvd);
	printf("  [3:3] : %#x\tProtection Information is Transferred as %s 8 Bytes of Metadata\n",
		pif8, pif8 ? "First" : "Last");
	printf("  [2:0] : %#x\tProtection Information %s\n", pit,
		pit == 3 ? "Type 3 Enabled" :
		pit == 2 ? "Type 2 Enabled" :
		pit == 1 ? "Type 1 Enabled" :
		pit == 0 ? "Disabled" : "Reserved Enabled");
	printf("\n");
}

void NvmeStorage::show_nvme_id_ns_nmic(__u8 nmic)
{
	__u8 rsvd = (nmic & 0xFE) >> 1;
	__u8 mp = nmic & 0x1;
	if (rsvd)
		printf("  [7:1] : %#x\tReserved\n", rsvd);
	printf("  [0:0] : %#x\tNamespace Multipath %sCapable\n",
		mp, mp ? "" : "Not ");
	printf("\n");
}

void NvmeStorage::show_nvme_id_ns_rescap(__u8 rescap)
{
	__u8 rsvd = (rescap & 0x80) >> 7;
	__u8 eaar = (rescap & 0x40) >> 6;
	__u8 wear = (rescap & 0x20) >> 5;
	__u8 earo = (rescap & 0x10) >> 4;
	__u8 wero = (rescap & 0x8) >> 3;
	__u8 ea = (rescap & 0x4) >> 2;
	__u8 we = (rescap & 0x2) >> 1;
	__u8 ptpl = rescap & 0x1;
	if (rsvd)
		printf("  [7:7] : %#x\tReserved\n", rsvd);
	printf("  [6:6] : %#x\tExclusive Access - All Registrants %sSupported\n",
		eaar, eaar ? "" : "Not ");
	printf("  [5:5] : %#x\tWrite Exclusive - All Registrants %sSupported\n",
		wear, wear ? "" : "Not ");
	printf("  [4:4] : %#x\tExclusive Access - Registrants Only %sSupported\n",
		earo, earo ? "" : "Not ");
	printf("  [3:3] : %#x\tWrite Exclusive - Registrants Only %sSupported\n",
		wero, wero ? "" : "Not ");
	printf("  [2:2] : %#x\tExclusive Access %sSupported\n",
		ea, ea ? "" : "Not ");
	printf("  [1:1] : %#x\tWrite Exclusive %sSupported\n",
		we, we ? "" : "Not ");
	printf("  [0:0] : %#x\tPersist Through Power Loss %sSupported\n",
		ptpl, ptpl ? "" : "Not ");
	printf("\n");
}


enum {
	TERSE = 0x1u,	// only show a few useful fields
	HUMAN = 0x2u,	// interpret some values for humans
	VS    = 0x4u,	// print vendor specific data area
	RAW   = 0x8u,	// just dump raw bytes
};

void NvmeStorage::show_nvme_id_ns_fpi(__u8 fpi)
{
	__u8 fpis = (fpi & 0x80) >> 7;
	__u8 fpii = fpi & 0x7F;
	printf("  [7:7] : %#x\tFormat Progress Indicator %sSupported\n",
		fpis, fpis ? "" : "Not ");
	if (fpis || (!fpis && fpii))
	printf("  [6:0] : %#x\tFormat Progress Indicator (Remaining %d%%)\n",
		fpii, 100 - fpii);
	printf("\n");
}

void NvmeStorage::show_nvme_id_ns(struct nvme_id_ns *ns, unsigned int mode)
{
	int i;
	int human = mode&HUMAN,
		vs = mode&VS;

//	printf("nsze    : %#"PRIx64"\n", (uint64_t)le64toh(ns->nsze));
//	printf("ncap    : %#"PRIx64"\n", (uint64_t)le64toh(ns->ncap));
//	printf("nuse    : %#"PRIx64"\n", (uint64_t)le64toh(ns->nuse));
	printf("nsfeat  : %#x\n", ns->nsfeat);
	if (human)
		show_nvme_id_ns_nsfeat(ns->nsfeat);
	printf("nlbaf   : %d\n", ns->nlbaf);
	printf("flbas   : %#x\n", ns->flbas);
	if (human)
		show_nvme_id_ns_flbas(ns->flbas);
	printf("mc      : %#x\n", ns->mc);
	if (human)
		show_nvme_id_ns_mc(ns->mc);
	printf("dpc     : %#x\n", ns->dpc);
	if (human)
		show_nvme_id_ns_dpc(ns->dpc);
	printf("dps     : %#x\n", ns->dps);
	if (human)
		show_nvme_id_ns_dps(ns->dps);
	printf("nmic    : %#x\n", ns->nmic);
	if (human)
		show_nvme_id_ns_nmic(ns->nmic);
	printf("rescap  : %#x\n", ns->rescap);
	if (human)
		show_nvme_id_ns_rescap(ns->rescap);
	printf("fpi     : %#x\n", ns->fpi);
	if (human)
		show_nvme_id_ns_fpi(ns->fpi);
	printf("nawun   : %d\n", le16toh(ns->nawun));
	printf("nawupf  : %d\n", le16toh(ns->nawupf));
	printf("nacwu   : %d\n", le16toh(ns->nacwu));
	printf("nabsn   : %d\n", le16toh(ns->nabsn));
	printf("nabo    : %d\n", le16toh(ns->nabo));
	printf("nabspf  : %d\n", le16toh(ns->nabspf));
	printf("nvmcap  : %.0Lf\n", int128_to_double(ns->nvmcap));

	printf("nguid   : ");
	for (i = 0; i < 16; i++)
		printf("%02x", ns->nguid[i]);
	printf("\n");

	printf("eui64   : ");
	for (i = 0; i < 8; i++)
		printf("%02x", ns->eui64[i]);
	printf("\n");

	for (i = 0; i <= ns->nlbaf; i++) {
		if (human)
			printf("LBA Format %2d : Metadata Size: %-3d bytes - "
				"Data Size: %-2d bytes - Relative Performance: %#x %s %s\n", i,
				le16toh(ns->lbaf[i].ms), 1 << ns->lbaf[i].ds, ns->lbaf[i].rp,
				ns->lbaf[i].rp == 3 ? "Degraded" :
				ns->lbaf[i].rp == 2 ? "Good" :
				ns->lbaf[i].rp == 1 ? "Better" : "Best",
				i == (ns->flbas & 0xf) ? "(in use)" : "");
		else
			printf("lbaf %2d : ms:%-3d ds:%-2d rp:%#x %s\n", i,
				le16toh(ns->lbaf[i].ms), ns->lbaf[i].ds, ns->lbaf[i].rp,
				i == (ns->flbas & 0xf) ? "(in use)" : "");
	}
	if (vs) {
		printf("vs[]:");
		d(ns->vs, sizeof(ns->vs), 16, 1);
	}
}

void NvmeStorage::print_ps_power_and_scale(__le16 ctr_power, __u8 scale)
{
	__u16 power = le16toh(ctr_power);

	switch (scale & 0x3) {
	case 0:
		/* Not reported for this power state */
		printf("-");
		break;

	case 1:
		/* Units of 0.0001W */
		printf("%01u.%04uW", power / 10000, power % 10000);
		break;

	case 2:
		/* Units of 0.01W */
		printf("%01u.%02uW", power / 100, scale % 100);
		break;

	default:
		printf("reserved");
	}
}

 void NvmeStorage:: show_nvme_id_ctrl_power(struct nvme_id_ctrl *ctrl)
{
	int i;


	for (i = 0; i <= ctrl->npss; i++) {
		__u16 max_power = le16toh(ctrl->psd[i].max_power);

		printf("ps %4d : mp:", i);

		if (ctrl->psd[i].flags & NVME_PS_FLAGS_MAX_POWER_SCALE)
			printf("%01u.%04uW ", max_power / 10000, max_power % 10000);
		else
			printf("%01u.%02uW ", max_power / 100, max_power % 100);

		if (ctrl->psd[i].flags & NVME_PS_FLAGS_NON_OP_STATE)
			printf("non-");

		printf("operational enlat:%d exlat:%d rrt:%d rrl:%d\n"
			"          rwt:%d rwl:%d idle_power:",
			le32toh(ctrl->psd[i].entry_lat), le32toh(ctrl->psd[i].exit_lat),
			ctrl->psd[i].read_tput, ctrl->psd[i].read_lat,
			ctrl->psd[i].write_tput, ctrl->psd[i].write_lat);
		print_ps_power_and_scale(ctrl->psd[i].idle_power,
					 POWER_SCALE(ctrl->psd[i].idle_scale));
		printf(" active_power:");
		print_ps_power_and_scale(ctrl->psd[i].active_power,
					 POWER_SCALE(ctrl->psd[i].active_work_scale));
		printf("\n");

	}
}
void NvmeStorage::show_nvme_id_ctrl_cmic(__u8 cmic)
{
	__u8 rsvd = (cmic & 0xF8) >> 3;
	__u8 sriov = (cmic & 0x4) >> 2;
	__u8 mctl = (cmic & 0x2) >> 1;
	__u8 mp = cmic & 0x1;
	if (rsvd)
		printf("  [7:3] : %#x\tReserved\n", rsvd);
	printf("  [2:2] : %#x\t%s\n", sriov, sriov ? "SR-IOV" : "PCI");
	printf("  [1:1] : %#x\t%s Controller\n",
		mctl, mctl ? "Multi" : "Single");
	printf("  [0:0] : %#x\t%s Port\n", mp, mp ? "Multi" : "Single");
	printf("\n");
}

int NvmeStorage::show_nvme_id_ctrl_(struct nvme_id_ctrl *ctrl, unsigned int mode)
{
	int human = mode & HUMAN, vs = mode & VS;

	printf("vid     : %#x\n", le16toh(ctrl->vid));
	printf("ssvid   : %#x\n", le16toh(ctrl->ssvid));
	printf("sn      : %-.*s\n", (int)sizeof(ctrl->sn), ctrl->sn);
	printf("mn      : %-.*s\n", (int)sizeof(ctrl->mn), ctrl->mn);
	printf("fr      : %-.*s\n", (int)sizeof(ctrl->fr), ctrl->fr);
	printf("rab     : %d\n", ctrl->rab);
	printf("ieee    : %02x%02x%02x\n",
		ctrl->ieee[2], ctrl->ieee[1], ctrl->ieee[0]);
	printf("cmic    : %#x\n", ctrl->cmic);
	if (human)
		show_nvme_id_ctrl_cmic(ctrl->cmic);
	printf("mdts    : %d\n", ctrl->mdts);
	printf("cntlid  : %x\n", le16toh(ctrl->cntlid));
	printf("ver     : %x\n", le32toh(ctrl->ver));
	printf("rtd3r   : %x\n", le32toh(ctrl->rtd3r));
	printf("rtd3e   : %x\n", le32toh(ctrl->rtd3e));
	printf("oaes    : %#x\n", le32toh(ctrl->oaes));
	if (human)
		show_nvme_id_ctrl_oaes(ctrl->oaes);
	printf("oacs    : %#x\n", le16toh(ctrl->oacs));
	if (human)
		show_nvme_id_ctrl_oacs(ctrl->oacs);
	printf("acl     : %d\n", ctrl->acl);
	printf("aerl    : %d\n", ctrl->aerl);
	printf("frmw    : %#x\n", ctrl->frmw);
	if (human)
		show_nvme_id_ctrl_frmw(ctrl->frmw);
	printf("lpa     : %#x\n", ctrl->lpa);
	if (human)
		show_nvme_id_ctrl_lpa(ctrl->lpa);
	printf("elpe    : %d\n", ctrl->elpe);
	printf("npss    : %d\n", ctrl->npss);
	printf("avscc   : %#x\n", ctrl->avscc);
	if (human)
		show_nvme_id_ctrl_avscc(ctrl->avscc);
	printf("apsta   : %#x\n", ctrl->apsta);
	if (human)
		show_nvme_id_ctrl_apsta(ctrl->apsta);
	printf("wctemp  : %d\n", le16toh(ctrl->wctemp));
	printf("cctemp  : %d\n", le16toh(ctrl->cctemp));
	printf("mtfa    : %d\n", le16toh(ctrl->mtfa));
	printf("hmpre   : %d\n", le32toh(ctrl->hmpre));
	printf("hmmin   : %d\n", le32toh(ctrl->hmmin));
	printf("tnvmcap : %.0Lf\n", int128_to_double(ctrl->tnvmcap));
	printf("unvmcap : %.0Lf\n", int128_to_double(ctrl->unvmcap));
	printf("rpmbs   : %#x\n", le32toh(ctrl->rpmbs));
	if (human)
		show_nvme_id_ctrl_rpmbs(ctrl->rpmbs);
	printf("sqes    : %#x\n", ctrl->sqes);
	if (human)
		show_nvme_id_ctrl_sqes(ctrl->sqes);
	printf("cqes    : %#x\n", ctrl->cqes);
	if (human)
		show_nvme_id_ctrl_cqes(ctrl->cqes);
	printf("nn      : %d\n", le32toh(ctrl->nn));
	printf("oncs    : %#x\n", le16toh(ctrl->oncs));
	if (human)
		show_nvme_id_ctrl_oncs(ctrl->oncs);
	printf("fuses   : %#x\n", le16toh(ctrl->fuses));
	if (human)
		show_nvme_id_ctrl_fuses(ctrl->fuses);
	printf("fna     : %#x\n", ctrl->fna);
	if (human)
		show_nvme_id_ctrl_fna(ctrl->fna);
	printf("vwc     : %#x\n", ctrl->vwc);
	if (human)
		show_nvme_id_ctrl_vwc(ctrl->vwc);
	printf("awun    : %d\n", le16toh(ctrl->awun));
	printf("awupf   : %d\n", le16toh(ctrl->awupf));
	printf("nvscc   : %d\n", ctrl->nvscc);
	if (human)
		show_nvme_id_ctrl_nvscc(ctrl->nvscc);
	printf("acwu    : %d\n", le16toh(ctrl->acwu));
	printf("sgls    : %x\n", le32toh(ctrl->sgls));
	if (human)
		show_nvme_id_ctrl_sgls(ctrl->sgls);

	show_nvme_id_ctrl_power(ctrl);
	//if (vendor_show)
	//	vendor_show(ctrl->vs);
	//else if (vs) {
		//printf("vs[]:\n");
		//d(ctrl->vs, sizeof(ctrl->vs), 16, 1);
	//}
	return ErrorCode;
}

#define MAX_LIST_ITEMS 256

struct list_item {
	char				node[1024];
	struct nvme_id_ctrl ctrl;
	int 				nsid;
	struct nvme_id_ns	ns;
	unsigned			block;
	__le32				ver;
};

 void NvmeStorage::get_version(struct list_item* list_item)
{
	struct nvme_bar *bar;

	list_item->ver = list_item->ctrl.ver;
	if (list_item->ctrl.ver)
		return;
	get_registers(&bar);
	list_item->ver = bar->vs;
}

 
  char *NvmeStorage::nvme_char_from_block(char *block)
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
 
  void NvmeStorage::get_registers(struct nvme_bar **bar)
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
	 *bar = (struct nvme_bar *)membase;
 }

 struct si_suffix {
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
 
 const char *NvmeStorage::suffix_si_get(double *value)
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
 
 const char *NvmeStorage::suffix_binary_get(long long *value)
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
 
 long long NvmeStorage:: suffix_binary_parse(const char *value)
 {
	 char *suffix;
	 errno = 0;
	 long long ret = strtol(value, &suffix, 0);
	 if (errno)
		 return 0;
 
	 struct binary_suffix *s;
	 for (s = binary_suffixes; s->shift != 0; s++) {
		 if (tolower(suffix[0]) == tolower(s->suffix[0])) {
			 ret <<= s->shift;
			 return ret;
		 }
	 }
 	 if (suffix[0] != '\0')
		 errno = EINVAL;
 
	 return ret;
 }

 void NvmeStorage::print_list_item(struct list_item list_item)
{

	double nsze       = list_item.ns.nsze;
	double nuse       = list_item.ns.nuse;
	long long int lba = list_item.ns.lbaf[(list_item.ns.flbas & 0x0f)].ds;

	lba  = (1 << lba);
	nsze *= lba;
	nuse *= lba;

	const char *s_suffix = suffix_si_get(&nsze);
	const char *u_suffix = suffix_si_get(&nuse);
	const char *l_suffix = suffix_binary_get(&lba);

	char usage[128];
	sprintf(usage,"%6.2f %2sB / %6.2f %2sB", nuse, u_suffix,
		nsze, s_suffix);
	char format[128];
	sprintf(format,"%3.0f %2sB + %2d B", (double)lba, l_suffix,
		list_item.ns.lbaf[(list_item.ns.flbas & 0x0f)].ms);
	char version[128];
	sprintf(version,"%d.%d", (list_item.ver >> 16),
		(list_item.ver >> 8) & 0xff);

	printf("%-16s %-*.*s %-*.*s %-8s %-9d %-26s %-16s %-.*s\n", list_item.node,
            (int)sizeof(list_item.ctrl.sn), (int)sizeof(list_item.ctrl.sn), list_item.ctrl.sn,
            (int)sizeof(list_item.ctrl.mn), (int)sizeof(list_item.ctrl.mn), list_item.ctrl.mn,
            version, list_item.nsid, usage, format, (int)sizeof(list_item.ctrl.fr), list_item.ctrl.fr);
}

 void NvmeStorage::print_list_items(struct list_item *list_items, unsigned len)
{
	unsigned i;

	printf("%-16s %-20s %-40s %-8s %-9s %-26s %-16s %-8s\n",
	    "Node", "SN", "Model", "Version", "Namespace", "Usage", "Format", "FW Rev");
	printf("%-16s %-20s %-40s %-8s %-9s %-26s %-16s %-8s\n",
            "----------------", "--------------------", "----------------------------------------",
            "--------", "---------", "--------------------------", "----------------", "--------");
	for (i = 0 ; i < len ; i++)
		print_list_item(list_items[i]);

}
 int NvmeStorage::nvme_get_nsid(int fd)
 {
	 static struct stat nvme_stat;
	 int err = fstat(fd, &nvme_stat);
 
	 if (err < 0)
		 return err;
 
	 if (!S_ISBLK(nvme_stat.st_mode)) {
		 fprintf(stderr,
			 "Error: requesting namespace-id from non-block device\n");
		 exit(ENOTBLK);
	 }
	 return ioctl(fd, NVME_IOCTL_ID);
 }


int NvmeStorage::nvme_list()
{

	   unsigned int count=0;
	   struct udev *udev;
	   struct udev_enumerate *enumerate;
	   struct udev_list_entry *devices, *dev_list_entry;
	   struct udev_device *dev;

	 struct list_item list_items[MAX_LIST_ITEMS];
	
	 


	udev = udev_new();
		if (!udev) {
				perror("nvme-list: Cannot create udev context.");
				return errno;
		}

		enumerate = udev_enumerate_new(udev);
		udev_enumerate_add_match_subsystem(enumerate, "block");
		udev_enumerate_add_match_property(enumerate, "DEVTYPE", "disk");
		udev_enumerate_scan_devices(enumerate);
		devices = udev_enumerate_get_list_entry(enumerate);
		udev_list_entry_foreach(dev_list_entry, devices) {
			printf("dev_list_entry count%d\n",count);

			
		int err;
		const char *path, *node;
		path = udev_list_entry_get_name(dev_list_entry);
		dev  = udev_device_new_from_syspath(udev, path);
		node = udev_device_get_devnode(dev);
		if (strstr(node,"nvme")!=NULL){
           
			open_dev(node);
          

		err = nvme_identify_ctrl(&list_items[count].ctrl);
		if (err > 0)
			return err;
		list_items[count].nsid = nvme_get_nsid(fd);
			err = nvme_identify_ns(fd, list_items[count].nsid,
			0, &list_items[count].ns);
		if (err > 0)
			return err;
		strcpy(list_items[count].node, node);
		list_items[count].block = S_ISBLK(nvme_stat.st_mode);
		get_version(&list_items[count]);
		count++;
		}
	 }

	udev_enumerate_unref(enumerate);
	udev_unref(udev);
//	#endif
    if (count)
		print_list_items(list_items, count);
	return ErrorCode;


}



#if 0
class NvmeStorage: public Storage
{
private:
char arg[20];
int identifier;
char arg2[20];

        public:
       /* NvmeStorage()
 *         {
 *                     
 *                                 
 *                                         }*/
        void Init(char **argv)
        {
         
          StrCpy(arg, argv[0]);    
          ::open_dev(argv[0]);
          std::cout << "NvmeStorage Init" << std::endl;   
            
        }
        
		int nvme_identify_ctrl(int fd, void *data);
        int nvme_identify(int fd, __u32 nsid, __u32 cdw10, void *data);
        int nvme_submit_admin_passthru(int fd, struct nvme_passthru_cmd *cmd);
       
};
#endif
