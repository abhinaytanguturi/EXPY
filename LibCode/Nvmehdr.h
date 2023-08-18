#include "Storage.h"
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
		   struct nvme_id_ctrl ctrl;
	     int ErrorCode;
		 char **argv;

        virtual void open_dev(const char* dev);             
         int Init(char **); 
	int nvme_identify_ctrl( void *data);
        int nvme_identify(int fd, __u32 nsid, __u32 cdw10, void *data);
        int nvme_submit_admin_passthru(int fd, struct nvme_passthru_cmd *cmd);
      void show_nvme_id_ctrl_cmic(__u8 );
void show_nvme_id_ctrl_oaes(__le32);
void show_nvme_id_ctrl_oacs(__le16);
void show_nvme_id_ctrl_frmw(__u8);
void show_nvme_id_ctrl_lpa(__u8);
void show_nvme_id_ctrl_avscc(__u8);
void show_nvme_id_ctrl_apsta(__u8);
void show_nvme_id_ctrl_rpmbs(__le32);
void show_nvme_id_ctrl_sqes(__u8);
void show_nvme_id_ctrl_cqes(__u8);
void show_nvme_id_ctrl_oncs(__le16);
void show_nvme_id_ctrl_fuses(__le16);
void show_nvme_id_ctrl_fna(__u8);
void show_nvme_id_ctrl_vwc(__u8);
void show_nvme_id_ctrl_nvscc(__u8);
void show_nvme_id_ctrl_sgls(__le32);
void show_nvme_id_ctrl_power(struct nvme_id_ctrl *ctrl);
void print_ps_power_and_scale(__le16 ctr_power, __u8 scale);
void show_nvme_id_ns_nsfeat(__u8 nsfeat);
void show_nvme_id_ns_flbas(__u8 flbas);
void show_nvme_id_ns_mc(__u8 mc);
void show_nvme_id_ns_dpc(__u8 dpc);
void show_nvme_id_ns_dps(__u8 dps);
void show_nvme_id_ns_nmic(__u8 nmic);
void show_nvme_id_ns_rescap(__u8 rescap);
void show_nvme_id_ns_fpi(__u8 fpi);
void show_nvme_id_ns(struct nvme_id_ns *ns, unsigned int mode);
void d(unsigned char *buf, int len, int width, int group);
 long double int128_to_double(__u8 *data);
int show_nvme_id_ctrl_(struct nvme_id_ctrl *ctrl, unsigned int mode);
void get_version(struct list_item* list_item);
char* nvme_char_from_block(char *block);
void get_registers(struct nvme_bar **bar);

void  print_list_item(struct list_item list_item);

void print_list_items(struct list_item *list_items, unsigned len);
const char* suffix_si_get(double *value);
const char* suffix_binary_get(long long *value);
int nvme_list();
int nvme_identify_ns(int fd, __u32 nsid, bool present, void *data);
int nvme_get_nsid(int fd);
int nvme_io(int fd, __u8 opcode, __u64 slba, __u16 nblocks, __u16 control,
	    __u32 dsmgmt, __u32 reftag, __u16 apptag, __u16 appmask, void *data,
	    void *metadata);

int argconfig_parse(int argc, char *argv[], const char *program_desc,
		    const struct argconfig_commandline_options *options,
		    void *config_out, size_t config_size);
int parse_and_open(int argc, char **argv, const char *desc,
	const struct argconfig_commandline_options *clo, void *cfg, size_t size);
int submit_io(int opcode, char *command, const char *desc,
		     int argc, char **argv);

int read_cmd(int argc, char **argv);
int write_cmd(int argc, char **argv);
int compare(int argc, char **argv);
long long suffix_binary_parse(const char *value);
long long elapsed_utime(struct timeval start_time,
					struct timeval end_time);

const char * nvme_status_to_string(__u32 status);
void check_arg_dev(int argc, char **argv);
void get_dev(int argc, char **argv);
int argconfig_parse_subopt_string(char *string, char **options,
				  size_t max_options);


};
