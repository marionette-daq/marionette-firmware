#ifndef __FETCH_MBUS_H_
#define __FETCH_MBUS_H_

#ifdef __cplusplus
extern "C" {
#endif

bool fetch_mbus_reset(BaseSequentialStream * chp);
void fetch_mbus_init(void);

bool fetch_mbus_help_cmd(BaseSequentialStream * chp, uint32_t argc, char * argv[]);
bool fetch_mbus_select_cmd(BaseSequentialStream * chp, uint32_t argc, char * argv[]);
bool fetch_mbus_detect_cmd(BaseSequentialStream * chp, uint32_t argc, char * argv[]);
bool fetch_mbus_read_analog_cmd(BaseSequentialStream * chp, uint32_t argc, char * argv[]);
bool fetch_mbus_read_eeprom_cmd(BaseSequentialStream * chp, uint32_t argc, char * argv[]);
bool fetch_mbus_write_eeprom_cmd(BaseSequentialStream * chp, uint32_t argc, char * argv[]);

#ifdef __cplusplus
}
#endif


#endif
/*! @} */
