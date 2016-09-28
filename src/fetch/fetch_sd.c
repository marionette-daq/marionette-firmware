/*! \file fetch_gpio.c
 * Marionette fetch_gpio routines
 * @defgroup fetch_gpio Fetch GPIO
 * @{
 */

#include "ch.h"
#include "hal.h"

#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

#include "util_messages.h"
#include "util_strings.h"
#include "util_general.h"
#include "util_io.h"
#include "util_arg_parse.h"

#include "ff.h"

#include "fetch.h"
#include "fetch_defs.h"
#include "fetch_sd.h"

// list all command function prototypes here 
static bool fetch_sd_connect_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[]);
static bool fetch_sd_disconnect_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[]);
static bool fetch_sd_mount_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[]);
static bool fetch_sd_unmount_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[]);
static bool fetch_sd_format_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[]);
static bool fetch_sd_open_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[]);
static bool fetch_sd_close_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[]);
static bool fetch_sd_unlink_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[]);
static bool fetch_sd_read_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[]);
static bool fetch_sd_write_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[]);
static bool fetch_sd_tell_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[]);
static bool fetch_sd_seek_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[]);
static bool fetch_sd_dir_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[]);
static bool fetch_sd_status_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[]);
static bool fetch_sd_help_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[]);


static fetch_command_t fetch_sd_commands[] = {
    { fetch_sd_connect_cmd,     "connect",    "Connet SD card" },
    { fetch_sd_disconnect_cmd,  "disconnect",    "Disconnect SD card" },
    { fetch_sd_mount_cmd,       "mount",    "Mount file system" },
    { fetch_sd_unmount_cmd,     "unmount",  "Un-mount file system" },
    { fetch_sd_format_cmd,      "format",   "Format FAT32" },
    { fetch_sd_open_cmd,        "open",     "Open File" },
    { fetch_sd_close_cmd,       "close",    "Close file" },
    { fetch_sd_unlink_cmd,      "unlink",   "Delete file" },
    { fetch_sd_read_cmd,        "read",     "Read file" },
    { fetch_sd_write_cmd,       "write",    "Write file" },
    { fetch_sd_tell_cmd,        "tell",     "Tell file" },
    { fetch_sd_seek_cmd,        "seek",     "Seek file" },
    { fetch_sd_dir_cmd,         "dir",      "List directory" },
    { fetch_sd_status_cmd,      "status",   "SD card status" },
    { fetch_sd_help_cmd,        "help",     "SD command help" },
    { NULL, NULL, NULL } // null terminate list
  };

FATFS filesystem;
FIL file_obj;

static bool fatfs_error_check(BaseSequentialStream * chp, FRESULT err)
{
  switch(err)
  {
    case FR_OK:
      return true;
    case FR_DISK_ERR:
      util_message_error(chp, "FR_DISK_ERR");
      return false;
    case FR_INT_ERR:
      util_message_error(chp, "FR_INT_ERR");
    case FR_NOT_READY:
      util_message_error(chp, "FR_NOT_READY");
      return false;
    case FR_NO_FILE:
      util_message_error(chp, "FR_NO_FILE");
      return false;
    case FR_NO_PATH:
      util_message_error(chp, "FR_NO_PATH");
      return false;
    case FR_INVALID_NAME:
      util_message_error(chp, "FR_INVALID_NAME");
      return false;
    case FR_DENIED:
      util_message_error(chp, "FR_DENIED");
      return false;
    case FR_EXIST:
      util_message_error(chp, "FR_EXIST");
      return false;
    case FR_INVALID_OBJECT:
      util_message_error(chp, "FR_INVALID_OBJECT");
      return false;
    case FR_WRITE_PROTECTED:
      util_message_error(chp, "FR_WRITE_PROTECTED");
      return false;
    case FR_INVALID_DRIVE:
      util_message_error(chp, "FR_INVALID_DRIVE");
      return false;
    case FR_NOT_ENABLED:
      util_message_error(chp, "FR_NOT_ENABLED");
      return false;
    case FR_NO_FILESYSTEM:
      util_message_error(chp, "FR_NO_FILESYSTEM");
      return false;
    case FR_MKFS_ABORTED:
      util_message_error(chp, "FR_MKFS_ABORTED");
      return false;
    case FR_TIMEOUT:
      util_message_error(chp, "FR_TIMEOUT");
      return false;
    case FR_LOCKED:
      util_message_error(chp, "FR_LOCKED");
      return false;
    case FR_NOT_ENOUGH_CORE:
      util_message_error(chp, "FR_NOT_ENOUGH_CORE");
      return false;
    case FR_TOO_MANY_OPEN_FILES:
      util_message_error(chp, "FR_TOO_MANY_OPEN_FILES");
      return false;
    case FR_INVALID_PARAMETER:
      util_message_error(chp, "FR_INVALID_PARAMETER");
      return false;
    default:
      util_message_error(chp, "Unknown error, %d", err);
      return false;
  }
}

static bool fetch_sd_connect_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[])
{
  FETCH_PARAM_CHECK(chp, cmd_list, 0, 0);

  if( blkGetDriverState(&SDCD1) != BLK_ACTIVE )
  {
    util_message_error(chp, "state != BLK_ACTIVE");
    util_message_info(chp, "Card already connected?");
  }

  // power on card
  palClearPad(GPIOA, GPIOA_PA10_SDIO_PWR);
  chThdSleepMilliseconds(100);

  // for some reason cards may not connect on first try
  if( sdcConnect(&SDCD1) != MSG_OK )
  {
    if( sdcConnect(&SDCD1) != MSG_OK )
    {
      util_message_error(chp, "sdcConnect FAILED");
      return false;
    }
  }

  util_message_info(chp, "SDCD1->capacity = %d blocks, %d GB\r\n", SDCD1.capacity,
           SDCD1.capacity / (2 * 1024 * 1024));

  return true;
}

static bool fetch_sd_disconnect_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[])
{
  sdcDisconnect(&SDCD1);

  // power off card
  palSetPad(GPIOA, GPIOA_PA10_SDIO_PWR);

  return true;
}

static bool fetch_sd_mount_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[])
{
  FETCH_PARAM_CHECK(chp, cmd_list, 0, 0);

  return fatfs_error_check(chp, f_mount(&filesystem, "", 1) );
}

static bool fetch_sd_unmount_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[])
{
  FETCH_PARAM_CHECK(chp, cmd_list, 0, 0);

  return fatfs_error_check(chp, f_mount(NULL, NULL, 0) );
}

static bool fetch_sd_format_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[])
{
  FETCH_PARAM_CHECK(chp, cmd_list, 0, 0);

  return fatfs_error_check(chp, f_mkfs("", 0, 0) );
}

static bool fetch_sd_open_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[])
{
  FETCH_PARAM_CHECK(chp, cmd_list, 0, 1);

  return fatfs_error_check(chp, f_open(&file_obj, data_list[0], FA_READ+FA_WRITE+FA_OPEN_ALWAYS)); 
}

static bool fetch_sd_close_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[])
{
  FETCH_PARAM_CHECK(chp, cmd_list, 0, 0);

  return fatfs_error_check(chp, f_close(&file_obj));
}

static bool fetch_sd_unlink_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[])
{
  FETCH_PARAM_CHECK(chp, cmd_list, 0, 0);
  
  return fatfs_error_check(chp, f_unlink(data_list[0]));
}

static bool fetch_sd_read_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[])
{
  FETCH_PARAM_CHECK(chp, cmd_list, 0, 0);

  uint8_t buffer[64];
  uint32_t read_count = 0;
  
  if( !fatfs_error_check(chp, f_read(&file_obj, buffer, sizeof(buffer), (UINT*)&read_count)) )
  {
    return false;
  }

  util_message_hex_uint8_array(chp, "data", buffer, read_count);
  util_message_uint32(chp, "count", read_count);

  return true;
}

static bool fetch_sd_write_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[])
{
  FETCH_PARAM_CHECK(chp, cmd_list, 1, 1);

  uint32_t write_count = 0;

  if( !fatfs_error_check(chp, f_write(&file_obj, data_list[0], strlen(data_list[0]), (UINT*)&write_count)) )
  {
    return false;
  }
  
  util_message_uint32(chp, "count", write_count);

  return true;
}

static bool fetch_sd_tell_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[])
{
  FETCH_PARAM_CHECK(chp, cmd_list, 0, 0);
  
  util_message_uint32(chp, "fptr", f_tell(&file_obj));

  return true;
}

static bool fetch_sd_seek_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[])
{
  FETCH_PARAM_CHECK(chp, cmd_list, 1, 1);

  uint32_t offset;

  if( !util_parse_uint32(data_list[0], &offset) )
  {
    util_message_error(chp, "invalid file offset");
    return false;
  }

  return fatfs_error_check(chp, f_lseek(&file_obj, offset));
}

static bool fetch_sd_dir_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[])
{
  DIR dir_obj;
  FILINFO file_info;

  f_opendir(&dir_obj, "/");

  while( fatfs_error_check(chp, f_readdir(&dir_obj, &file_info)) && file_info.fname[0] != 0 )
  {
    util_message_uint32(chp, file_info.fname, file_info.fsize);
  }

  f_closedir(&dir_obj);
  return true;
}

static bool fetch_sd_status_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[])
{
  return true;
}

static bool fetch_sd_help_cmd(BaseSequentialStream * chp, char * cmd_list[], char * data_list[])
{
  FETCH_PARAM_CHECK(chp, cmd_list, 0, 0);

  fetch_display_help_legend(chp);
  fetch_display_help(chp, fetch_sd_commands);
	return true;
}

void fetch_sd_init(BaseSequentialStream * chp)
{
  static bool sd_init_flag = false;

  if( sd_init_flag )
    return;

   // power off card
  palSetPad(GPIOA, GPIOA_PA10_SDIO_PWR);

  // start sdio peripheral
  sdcStart(&SDCD1,0);

  sd_init_flag = true;
}

/*! \brief dispatch a specific spi command
 */
bool fetch_sd_dispatch(BaseSequentialStream * chp, char * cmd_list[], char * data_list[])
{
  return fetch_dispatch(chp, fetch_sd_commands, cmd_list, data_list);
}


bool fetch_sd_reset(BaseSequentialStream * chp)
{
  // TODO
  return true;
}

/*! @} */

