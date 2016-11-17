#include "fetch_commands.h"
#include "fetch_defs.h"

%%{
  machine fetch_command_parser;

  cmd_delim = '.';

  root_commands = (
                      "help"i     %{ *func=fetch_help_cmd; }
                    | "reset"i    %{ *func=fetch_reset_cmd; }
                    | "version"i  %{ *func=fetch_version_cmd; }
                    | "chipid"i   %{ *func=fetch_chip_id_cmd; }
                    | "test"i     %{ *func=fetch_test_cmd; }
                    | "clocks"i   %{ *func=fetch_clocks_cmd; }
                  );

  gpio_read_commands = "read"i . cmd_delim . (
                      "pins"i       %{ *func=fetch_gpio_read_cmd; }
                    | "port"i       %{ *func=fetch_gpio_read_port_cmd; }
                    | "latch"i      %{ *func=fetch_gpio_read_port_latch_cmd; }
                    | "all"i        %{ *func=fetch_gpio_read_all_cmd; }
                  );

  gpio_write_commands = "write"i . cmd_delim . (
                      "pins"i       %{ *func=fetch_gpio_write_cmd; }
                    | "port"i       %{ *func=fetch_gpio_write_port_cmd; }
                    | "all"i        %{ *func=NULL; /*FIXME (fetch_func_t)fetch_gpio_write_all_cmd*/ }
                  );

  gpio_commands = "gpio"i . cmd_delim . (
                      "reset"i      %{ *func=fetch_gpio_reset_cmd; }
                    | "read"i       %{ *func=fetch_gpio_read_cmd; }
                    | gpio_read_commands
                    | "write"i      %{ *func=fetch_gpio_write_cmd; }
                    | gpio_write_commands
                    | "set"i        %{ *func=fetch_gpio_set_cmd; }
                    | "clear"i      %{ *func=fetch_gpio_clear_cmd; }
                    | "config"i     %{ *func=fetch_gpio_config_cmd; }
                    | "info"i       %{ *func=fetch_gpio_info_cmd; }
                    | "shiftout"i   %{ *func=NULL; /*FIXME*/ }
                    | "help"i       %{ *func=fetch_gpio_help_cmd; }
                  );
  

  spi_commands = "spi"i . cmd_delim . (
                      "reset"i      %{ *func=fetch_spi_reset_cmd; }
                    | "config"i     %{ *func=fetch_spi_config_cmd; }
                    | "help"i       %{ *func=fetch_spi_help_cmd; }
                    | "clocks"i     %{ *func=fetch_spi_clocks_cmd; }
                    | "exchange"i   %{ *func=fetch_spi_exchange_cmd; } 
                  );

  dac_commands = "dac"i . cmd_delim . (
                      "help"i       %{ *func=fetch_dac_help_cmd; }
                    | "write"i      %{ *func=fetch_dac_write_cmd; }
                    | "reset"i      %{ *func=fetch_dac_reset_cmd; }
                  );

  i2c_commands = "i2c"i . cmd_delim . (
                      "write"i      %{ *func=fetch_i2c_write_cmd; }
                    | "read"i       %{ *func=fetch_i2c_read_cmd; }
                    | "config"i     %{ *func=fetch_i2c_config_cmd; }
                    | "reset"i      %{ *func=fetch_i2c_reset_cmd; }
                    | "help"i       %{ *func=fetch_i2c_help_cmd; }
                  );

  adc_commands = "adc"i . cmd_delim . (
                      "help"i       %{ *func=fetch_adc_help_cmd; }
                    | "single"i     %{ *func=fetch_adc_single_cmd; }
                    | "start"i      %{ *func=fetch_adc_stream_start_cmd; }
                    | "stop"i       %{ *func=fetch_adc_stream_stop_cmd; }
                    | "status"i     %{ *func=fetch_adc_status_cmd; }
                    | "config"i     %{ *func=fetch_adc_config_cmd; }
                    | "reset"i      %{ *func=fetch_adc_reset_cmd; }
                  );

  can_commands = "can"i . cmd_delim . (
                      "tx"i         %{ *func=NULL; /*FIXME*/ }
                    | "rx"i         %{ *func=NULL; /*FIXME*/ }
                    | "status"i     %{ *func=NULL; /*FIXME*/ }
                    | "config"i     %{ *func=NULL; /*FIXME*/ }
                    | "help"i       %{ *func=NULL; /*FIXME*/ }
                    | "reset"i      %{ *func=NULL; /*FIXME*/ }
                  );

  timer_commands = "timer"i . cmd_delim . (
                    "help"i       %{ *func=NULL; /*FIXME*/ }
                  );

  mbus_commands = "mbus"i . cmd_delim . (
                      "help"i         %{ *func=fetch_mbus_help_cmd; }
                    | "select"i       %{ *func=fetch_mbus_select_cmd; }
                    | "detect"i       %{ *func=fetch_mbus_detect_cmd; }
                    | "readanalog"i   %{ *func=fetch_mbus_read_analog_cmd; }
                    | "readeeprom"i   %{ *func=fetch_mbus_read_eeprom_cmd; }
                    | "writeeeprom"i  %{ *func=fetch_mbus_write_eeprom_cmd; }
                  );

  mcard_commands = "mcard"i . cmd_delim . (
                    "help"i       %{ *func=NULL; /*FIXME*/ }
                  );

  mpipe_commands = "mpipe"i . cmd_delim . (
                    "help"i       %{ *func=NULL; /*FIXME*/ }
                  );

  fetch_command = ( root_commands   | 
                    gpio_commands   | 
                    spi_commands    | 
                    dac_commands    |
                    i2c_commands    |
                    adc_commands    |
                    can_commands    |
                    timer_commands  |
                    mbus_commands   |
                    mcard_commands  |
                    mpipe_commands
                  ) @err{ fetch_parser_info.error_msg = "invalid command"; };

}%%

