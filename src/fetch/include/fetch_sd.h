/*! \file fetch_gpio.h
 * @addtogroup fetch_gpio
 * @{
 */

#ifndef __FETCH_SD_H_
#define __FETCH_SD_H_

#ifdef __cplusplus
extern "C" {
#endif

void fetch_sd_init(void);
bool fetch_sd_reset(BaseSequentialStream * chp);

bool fetch_sd_connect_cmd(BaseSequentialStream * chp, uint32_t argc, char * argv[]);
bool fetch_sd_disconnect_cmd(BaseSequentialStream * chp, uint32_t argc, char * argv[]);
bool fetch_sd_mount_cmd(BaseSequentialStream * chp, uint32_t argc, char * argv[]);
bool fetch_sd_unmount_cmd(BaseSequentialStream * chp, uint32_t argc, char * argv[]);
bool fetch_sd_format_cmd(BaseSequentialStream * chp, uint32_t argc, char * argv[]);
bool fetch_sd_open_cmd(BaseSequentialStream * chp, uint32_t argc, char * argv[]);
bool fetch_sd_close_cmd(BaseSequentialStream * chp, uint32_t argc, char * argv[]);
bool fetch_sd_unlink_cmd(BaseSequentialStream * chp, uint32_t argc, char * argv[]);
bool fetch_sd_read_cmd(BaseSequentialStream * chp, uint32_t argc, char * argv[]);
bool fetch_sd_write_cmd(BaseSequentialStream * chp, uint32_t argc, char * argv[]);
bool fetch_sd_tell_cmd(BaseSequentialStream * chp, uint32_t argc, char * argv[]);
bool fetch_sd_seek_cmd(BaseSequentialStream * chp, uint32_t argc, char * argv[]);
bool fetch_sd_dir_cmd(BaseSequentialStream * chp, uint32_t argc, char * argv[]);
bool fetch_sd_status_cmd(BaseSequentialStream * chp, uint32_t argc, char * argv[]);
bool fetch_sd_help_cmd(BaseSequentialStream * chp, uint32_t argc, char * argv[]);


#ifdef __cplusplus
}
#endif


#endif
/*! @} */
