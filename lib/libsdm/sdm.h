#ifndef SDM_H
#define SDM_H

#include <sys/types.h>
#include <stdint.h>
#include <stdio.h> /* FILE* */ 

#include <utils.h>

#define BUFSIZE  (1024 * 4)

#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define SDM_PKG_MAGIC 0x00000000ff7f0080UL
#else
#define SDM_PKG_MAGIC 0x80007fff00000000UL
#endif

enum {
    SDM_CMD_STOP    = 0,
    SDM_CMD_TX      = 1,
    SDM_CMD_RX      = 2,
    SDM_CMD_REF     = 3,
    SDM_CMD_CONFIG  = 4
};

enum {
    SDM_REPLAY_STOP   = 0,
    SDM_REPLAY_RX     = 2,
    SDM_REPLAY_BUSY   = 254,
    SDM_REPLAY_REPORT = 255,
};

enum {
    SDM_REPLAY_REPORT_NO_SDM_MODE = 0,
    SDM_REPLAY_REPORT_TX_STOP     = 1,
    SDM_REPLAY_REPORT_RX_STOP     = 2,
    SDM_REPLAY_REPORT_REF         = 3,
    SDM_REPLAY_REPORT_CONFIG      = 4,
    SDM_REPLAY_REPORT_DROP        = 254,
    SDM_REPLAY_REPORT_UNKNOWN     = 255,
};


enum {
    SDM_FILE_TYPE_FLOAT = 1
   ,SDM_FILE_TYPE_INT
};

typedef struct sdm_pkt_t {
    uint64_t magic;
    uint8_t  cmd;
    union {
        struct {
            uint16_t threshold;
            uint8_t  gain_and_srclvl;
        };
        struct {
            uint16_t param;
            uint8_t  dummy;
        };
        char rx_len[3];
    };
    uint32_t data_len; /* in 16bit words */
    uint16_t data[0];
} sdm_pkt_t;

enum {
    SDM_STATE_INIT = 1,
    SDM_STATE_IDLE,
    SDM_STATE_WAIT_REPLY,
    SDM_STATE_RX
};

typedef struct {
    int  sockfd;
    char *rx_data;
    int  rx_data_len;

    int state;

    char* filename;
    int data_len;

    sdm_pkt_t cmd; /* last received command */
} sdm_session_t;

sdm_session_t* sdm_connect(char *ip, int port);
void  sdm_close(sdm_session_t *ss);

int   sdm_cmd(sdm_session_t *sd, int cmd_code, ...);
int   sdm_extract_replay(char *buf, size_t len, sdm_pkt_t **cmd);

int   sdm_rx(sdm_session_t *ss, int cmd, ...);
int   sdm_handle_rx_data(sdm_session_t *ss, char *buf, int len);

void  sdm_set_idle_state(sdm_session_t *ss);

int   sdm_show(sdm_pkt_t *cmd);

int       sdm_save_samples(char *filename, char *buf, size_t len);
uint16_t *sdm_load_samples(char *filename, size_t *len);

int   sdm_autodetect_samples_file_type(FILE *fp);
const char* strrpbrk(const char *s, const char *accept_only);

char* sdm_cmd_to_str(uint8_t cmd);
char* sdm_reply_to_str(uint8_t cmd);
char* sdm_reply_report_to_str(uint8_t cmd);
char* sdm_samples_file_type_to_str(uint8_t type);

#endif

