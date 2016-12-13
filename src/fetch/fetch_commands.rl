#include "fetch_commands.h"
#include "fetch_defs.h"

%%{
  machine fetch_command_parser;

  cmd_delim = '.';

  root_commands = (
                      "help"i       %{ *func=fetch_help_cmd; }
                    | "reset"i      %{ *func=fetch_reset_cmd; }
                    | "version"i    %{ *func=fetch_version_cmd; }
                    | "chip_id"i    %{ *func=fetch_chip_id_cmd; }
                    | "test"i       %{ *func=fetch_test_cmd; }
                    | "test_data"i  %{ *func=fetch_test_data_cmd; }
                    | "clocks"i     %{ *func=fetch_clocks_cmd; }
                  );

  gpio_commands = "gpio"i . cmd_delim . (
                      "reset"i            %{ *func=fetch_gpio_reset_cmd; }
                    | "read"i             %{ *func=fetch_gpio_read_cmd; }
                    | "read_pin"i         %{ *func=fetch_gpio_read_cmd; }
                    | "read_port"i        %{ *func=fetch_gpio_read_port_cmd; }
                    | "read_port_latch"i  %{ *func=fetch_gpio_read_port_latch_cmd; }
                    | "read_latch"i       %{ *func=fetch_gpio_read_latch_cmd; }
                    | "read_all"i         %{ *func=fetch_gpio_read_all_cmd; }
                    | "write"i            %{ *func=fetch_gpio_write_cmd; }
                    | "write_pin"i        %{ *func=fetch_gpio_write_cmd; }
                    | "write_port"i       %{ *func=fetch_gpio_write_port_cmd; }
                    | "write_all"i        %{ *func=fetch_gpio_write_all_cmd; }
                    | "set"i              %{ *func=fetch_gpio_set_cmd; }
                    | "clear"i            %{ *func=fetch_gpio_clear_cmd; }
                    | "config"i           %{ *func=fetch_gpio_config_cmd; }
                    | "info"i             %{ *func=fetch_gpio_info_cmd; }
                    | "shiftout"i         %{ *func=fetch_gpio_shift_out_cmd; }
                    | "help"i             %{ *func=fetch_gpio_help_cmd; }
                  );
  

  spi_commands = "spi"i . cmd_delim . (
                      "reset"i      %{ *func=fetch_spi_reset_cmd; }
                    | "config"i     %{ *func=fetch_spi_config_cmd; }
                    | "help"i       %{ *func=fetch_spi_help_cmd; }
                    | "clock_div"i  %{ *func=fetch_spi_clock_div_cmd; }
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
                    | "read_analog"i  %{ *func=fetch_mbus_read_analog_cmd; }
                    | "read_eeprom"i  %{ *func=fetch_mbus_read_eeprom_cmd; }
                    | "write_eeprom"i %{ *func=fetch_mbus_write_eeprom_cmd; }
                  );

  mcard_commands = "mcard"i . cmd_delim . (
                    "help"i       %{ *func=NULL; /*FIXME*/ }
                  );

  mpipe_commands = "mpipe"i . cmd_delim . (
                    "help"i       %{ *func=NULL; /*FIXME*/ }
                  );

  serial_commands = "serial"i . cmd_delim . (
                      "help"i     %{ *func=fetch_serial_help_cmd; }
                    | "config"    %{ *func=fetch_serial_config_cmd; }
                    | "write"     %{ *func=fetch_serial_write_cmd; }
                    | "read"      %{ *func=fetch_serial_read_cmd; }
                    | "timeout"   %{ *func=fetch_serial_timeout_cmd; }
                    | "status"    %{ *func=fetch_serial_status_cmd; }
                    | "break"     %{ *func=fetch_serial_break_cmd; }
                    | "reset"     %{ *func=fetch_serial_reset_cmd; }
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
                    mpipe_commands  |
                    serial_commands
                  ) @err{ fetch_parser_info.error_msg = "invalid command"; };

}%%

