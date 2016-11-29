#ifndef __FETCH_PARSER_H
#define __FETCH_PRRSER_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
  bool error;
  uint32_t offset;
  uint32_t fsm_state;
  uint32_t fsm_state_first_final;
  uint32_t fsm_state_error;
  uint32_t fsm_state_start;
  const char * error_msg;
} fetch_parser_info_t;

extern fetch_parser_info_t fetch_parser_info;

bool fetch_command_parser( const char * input_str, uint32_t max_input_len, char * output_str, uint32_t max_output_len, fetch_func_t * func, uint32_t * argc, char * argv[], uint32_t max_args);

bool fetch_string_parser( const char * input_str, uint32_t max_input_len, char * output_str, uint32_t max_output_len, uint32_t * output_len );

bool fetch_gpio_parser( const char * input_str, uint32_t max_input_len, port_pin_t * pp );

bool fetch_gpio_port_parser( const char * input_str, uint32_t max_input_len, ioportid_t * port );

bool fetch_hex_string_parser( const char * input_str, uint32_t max_input_len, char * output_str, uint32_t max_output_len, uint32_t * output_len );

#ifdef __cplusplus
}
#endif

#endif
