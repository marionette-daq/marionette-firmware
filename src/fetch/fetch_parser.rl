#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#include "ch.h"
#include "hal.h"
#include "util_messages.h"
#include "util_io.h"

#include "fetch_defs.h"
#include "fetch.h"
#include "fetch_commands.h"
#include "fetch_parser.h"

fetch_parser_info_t fetch_parser_info;

static uint8_t hex_value(char c)
{
  if( c >= '0' && c <= '9' )
  {
    return (c-'0');
  }
  else if( c >= 'a' && c <= 'f' )
  {
    return (10+c-'a');
  }
  else if( c >= 'A' && c <= 'F' )
  {
    return (10+c-'A');
  }
  else
  {
    return 0;
  }
}

static void update_error_msg(void)
{
  // don't clobber message if one is already set
  if( fetch_parser_info.error_msg )
  {
    return;
  }
  else if( fetch_parser_info.fsm_state == fetch_parser_info.fsm_state_error )
  {
    fetch_parser_info.error_msg = "Error state, bad input format";
  }
  else if( fetch_parser_info.fsm_state == fetch_parser_info.fsm_state_start )
  {
    fetch_parser_info.error_msg = "Parser never exits start state";
  }
  else if( fetch_parser_info.fsm_state <= fetch_parser_info.fsm_state_first_final )
  {
    fetch_parser_info.error_msg = "End of buffer encountered, max string size";
  }
}


%%{
  machine fetch_command_parser;

  include "fetch_commands.rl";

  action start_data {
      if( *argc >= max_args )
      {
        fetch_parser_info.error_msg = "max arguments";
        fgoto *fetch_command_parser_error;
      }
      else
      {
        argv[*argc] = wp;
        (*argc)++;
      }
    }
  action end_data {
      if( output_count < max_output_len )
      {
        *wp = '\0';
        wp++;
        output_count++;
      }
      else
      {
        fetch_parser_info.error_msg = "max output";
        fgoto *fetch_command_parser_error;
      }
    }
  action copy_data {
      if( output_count < max_output_len )
      {
        *wp = *p;
        wp++;
        output_count++;
      }
      else
      {
        fetch_parser_info.error_msg = "max output";
        fgoto *fetch_command_parser_error;
      }
    }

  ws = [\t ];
  hex_number = '0x' . xdigit+;
  dec_number = digit+;
  oct_number = '0' . [0-7]+;
  not_null = [^\0];
  not_single = [^\'];
  not_double = [^\"];
  not_escape = [^\\];
  escape_str = '\\' . not_null;

  data_string_double = '\"' . ( escape_str | (not_double & not_escape & not_null) )* . '\"';
  data_string_single = '\'' . ( escape_str | (not_single & not_escape & not_null) )* . '\'';
  data_string = (data_string_double | data_string_single);
  
  data_const = ( '_' | alpha ) . ( '-' | '_' | '.' | ':' | alnum )*;
  data_number = [\-+]? . ( hex_number | dec_number | oct_number );

  data_item = (data_string | data_const | data_number) @err{ fetch_parser_info.error_msg = "invalid argument"; } >start_data $copy_data %end_data ;

  data_list = ws* . data_item . ( ws* . ',' . ws* . data_item )* . ws*;

  main := ws* . fetch_command . ws* . ( '(' . data_list? . ')' )? . ws* . 0? @{ fbreak; };

  write data;
}%%

bool fetch_command_parser( const char * input_str, uint32_t max_input_len, char * output_str, uint32_t max_output_len, fetch_func_t * func, uint32_t * argc, char * argv[], uint32_t max_args)
{
  uint32_t cs;
  const char * p = input_str;
  const char * pe = input_str + max_input_len;
  const char * eof = pe;
  char * wp = output_str;
  uint32_t output_count = 0;

  *func = NULL;
  *argc = 0;

  // FIXME add chDbgAssert statements for pointers

  fetch_parser_info.error_msg = NULL;
  fetch_parser_info.fsm_state_first_final = %%{ write first_final; }%%;
  fetch_parser_info.fsm_state_start = %%{ write start; }%%;
  fetch_parser_info.fsm_state_error = %%{ write error; }%%;
  
  %% write init;
  %% write exec;

  fetch_parser_info.fsm_state = cs;
  fetch_parser_info.offset = (p-input_str);
  
  if( cs >= %%{ write first_final; }%% )
  {
    fetch_parser_info.error = false;
    return true;
  }
  else
  {
    fetch_parser_info.error = true;
    update_error_msg();
    return false;
  }
}

%%{
  machine fetch_string_parser;

  action oct_0 { value = *p-'0'; } 
  action oct_n { value = (value << 3) | (*p-'0'); }
  action hex_0 { value = hex_value(*p); }
  action hex_1 { value = (value << 4) | hex_value(*p); }
  action write_value {
      if( len < max_output_len )
      {
        *wp=value;
        wp++;
        len++;
      }
      else
      {
        fetch_parser_info.error_msg = "max output";
        fgoto *fetch_string_parser_error;
      }
    }
  action write_char {
      if( len < max_output_len )
      {
        *wp=*p;
        wp++;
        len++;
      }
      else
      {
        fetch_parser_info.error_msg = "max output";
        fgoto *fetch_string_parser_error;
      }
    }
  
  not_null = [^\0];
  not_single = [^\'];
  not_double = [^\"];
  not_escape = [^\\];

  basic_escape = (  'n' >{ value='\n'; } | 
                    'r' >{ value='\r'; } | 
                    't' >{ value='\t'; } | 
                    'f' >{ value='\f'; } | 
                    'v' >{ value='\v'; } |
                    'a' >{ value='\a'; } |
                    'b' >{ value='\b'; } |
                    '\\' >{ value='\\'; } |
                    '\"' >{ value='\"'; } |
                    '\'' >{ value='\''; } );
  
  hex_escape = [xX] . xdigit >hex_0 . xdigit >hex_1;

  oct_escape = [0-3] >oct_0 . [0-7] >oct_n . [0-7] >oct_n;
  
  str_char_single = ( not_escape & not_single & not_null ) >write_char;
  str_char_double = ( not_escape & not_double & not_null ) >write_char;

  str_escape = '\\' . ( basic_escape | hex_escape | oct_escape ) %write_value;

  single_quote = '\'' . ( str_char_single | str_escape )* . '\'';
  double_quote = '\"' . ( str_char_double | str_escape )* . '\"';

  main := ( single_quote | double_quote ) . 0? @{ fbreak; };

  write data;
}%%


bool fetch_string_parser( const char * input_str, uint32_t max_input_len, char * output_str, uint32_t max_output_len, uint32_t * output_len )
{
  uint32_t cs;
  const char * p = input_str;
  const char * pe = input_str + max_input_len;
  const char * eof = pe;
  char * wp = output_str;

  uint8_t value = 0;
  uint32_t len = 0;
  
  // FIXME add chDbgAssert statements for pointers
  
  fetch_parser_info.error_msg = NULL;
  fetch_parser_info.fsm_state_first_final = %%{ write first_final; }%%;
  fetch_parser_info.fsm_state_start = %%{ write start; }%%;
  fetch_parser_info.fsm_state_error = %%{ write error; }%%;
  
  %% write init;
  %% write exec;

  *output_len = len;

  fetch_parser_info.fsm_state = cs;
  fetch_parser_info.offset = (p-input_str);
  
  if( cs >= %%{ write first_final; }%% )
  {
    fetch_parser_info.error = false;
    return true;
  }
  else
  {
    fetch_parser_info.error = true;
    update_error_msg();
    return false;
  }
}

%%{
  machine fetch_gpio_parser;

  action parse_pin_0 { pp->pin = *p-'0'; }
  action parse_pin_1 { pp->pin = 10 + (*p-'0'); }

  stm_pin             = ('0' . [0-9] >parse_pin_0) | 
                        ('1' . [0-5] >parse_pin_1) | 
                        ([0-9] >parse_pin_0);
  stm_port            = 'A'i >{ pp->port = GPIOA; } |
                        'B'i >{ pp->port = GPIOB; } |
                        'C'i >{ pp->port = GPIOC; } |
                        'D'i >{ pp->port = GPIOD; } |
                        'E'i >{ pp->port = GPIOE; } |
                        'F'i >{ pp->port = GPIOF; } |
                        'G'i >{ pp->port = GPIOG; } |
                        'H'i >{ pp->port = GPIOH; } |
                        'I'i >{ pp->port = GPIOI; };

  stm_port_pin        = 'P'i . stm_port . stm_pin;

  delim = [\-_.:];

  digital_port_pin    = 'D'i . 'IO'i? . delim? .
                        (
                          '0'? . '0' %{ pp->port = GPIOG; pp->pin = 0; } |
                          '0'? . '1' %{ pp->port = GPIOG; pp->pin = 1; } |
                          '0'? . '2' %{ pp->port = GPIOG; pp->pin = 2; } |
                          '0'? . '3' %{ pp->port = GPIOG; pp->pin = 3; } |
                          '0'? . '4' %{ pp->port = GPIOG; pp->pin = 4; } |
                          '0'? . '5' %{ pp->port = GPIOG; pp->pin = 5; } |
                          '0'? . '6' %{ pp->port = GPIOG; pp->pin = 6; } |
                          '0'? . '7' %{ pp->port = GPIOG; pp->pin = 7; } |
                          '0'? . '8' %{ pp->port = GPIOE; pp->pin = 0; } |
                          '0'? . '9' %{ pp->port = GPIOE; pp->pin = 7; } |
                          '10' %{ pp->port = GPIOE; pp->pin =  8; } |
                          '11' %{ pp->port = GPIOF; pp->pin = 11; } |
                          '12' %{ pp->port = GPIOF; pp->pin = 12; } |
                          '13' %{ pp->port = GPIOF; pp->pin = 13; } |
                          '14' %{ pp->port = GPIOF; pp->pin = 14; } |
                          '15' %{ pp->port = GPIOF; pp->pin = 15; } |
                          '16' %{ pp->port = GPIOH; pp->pin =  2; } |
                          '17' %{ pp->port = GPIOH; pp->pin =  3; } |
                          '18' %{ pp->port = GPIOH; pp->pin =  5; } |
                          '19' %{ pp->port = GPIOH; pp->pin =  6; } |
                          '20' %{ pp->port = GPIOH; pp->pin =  9; } |
                          '21' %{ pp->port = GPIOH; pp->pin = 14; } |
                          '22' %{ pp->port = GPIOB; pp->pin =  8; } |
                          '23' %{ pp->port = GPIOB; pp->pin =  9; } |
                          '24' %{ pp->port = GPIOE; pp->pin =  5; } |
                          '25' %{ pp->port = GPIOE; pp->pin =  6; } |
                          '26' %{ pp->port = GPIOE; pp->pin =  9; } |
                          '27' %{ pp->port = GPIOE; pp->pin = 13; } |
                          '28' %{ pp->port = GPIOH; pp->pin = 10; } |
                          '29' %{ pp->port = GPIOH; pp->pin = 11; } |
                          '30' %{ pp->port = GPIOH; pp->pin = 12; } |
                          '31' %{ pp->port = GPIOA; pp->pin = 15; } |
                          '32' %{ pp->port = GPIOI; pp->pin =  1; } |
                          '33' %{ pp->port = GPIOI; pp->pin =  0; } |
                          '34' %{ pp->port = GPIOI; pp->pin =  2; } |
                          '35' %{ pp->port = GPIOI; pp->pin =  3; } |
                          '36' %{ pp->port = GPIOH; pp->pin = 15; } |
                          '37' %{ pp->port = GPIOD; pp->pin =  7; } |
                          '38' %{ pp->port = GPIOG; pp->pin = 13; } |
                          '39' %{ pp->port = GPIOG; pp->pin =  8; } |
                          '40' %{ pp->port = GPIOG; pp->pin = 12; } |
                          '41' %{ pp->port = GPIOG; pp->pin = 14; } |
                          '42' %{ pp->port = GPIOG; pp->pin =  9; } |
                          '43' %{ pp->port = GPIOG; pp->pin = 11; } |
                          '44' %{ pp->port = GPIOF; pp->pin =  0; } |
                          '45' %{ pp->port = GPIOF; pp->pin =  1; } |
                          '46' %{ pp->port = GPIOE; pp->pin =  1; } |
                          '47' %{ pp->port = GPIOG; pp->pin = 15; } |
                          '48' %{ pp->port = GPIOA; pp->pin =  0; } |
                          '49' %{ pp->port = GPIOA; pp->pin =  1; } |
                          '50' %{ pp->port = GPIOE; pp->pin =  2; } |
                          '51' %{ pp->port = GPIOE; pp->pin =  3; } |
                          '52' %{ pp->port = GPIOD; pp->pin =  8; } |
                          '53' %{ pp->port = GPIOD; pp->pin =  9; } |
                          '54' %{ pp->port = GPIOD; pp->pin = 11; } |
                          '55' %{ pp->port = GPIOD; pp->pin = 12; } |
                          '56' %{ pp->port = GPIOD; pp->pin =  5; } |
                          '57' %{ pp->port = GPIOD; pp->pin =  6; } |
                          '58' %{ pp->port = GPIOD; pp->pin =  3; } |
                          '59' %{ pp->port = GPIOD; pp->pin =  4; } |
                          '60' %{ pp->port = GPIOI; pp->pin =  8; } |
                          '61' %{ pp->port = GPIOI; pp->pin = 11; } |
                          '62' %{ pp->port = GPIOD; pp->pin =  1; } |
                          '63' %{ pp->port = GPIOG; pp->pin = 10; } |
                          '64' %{ pp->port = GPIOB; pp->pin = 15; } |
                          '65' %{ pp->port = GPIOI; pp->pin =  4; } |
                          '66' %{ pp->port = GPIOD; pp->pin =  0; } |
                          '67' %{ pp->port = GPIOC; pp->pin = 13; } |
                          '68' %{ pp->port = GPIOI; pp->pin = 10; } |
                          '69' %{ pp->port = GPIOB; pp->pin = 14; }
                        );
  timer_port_pin      = ( 'TIMER'i ) . delim? . 
                        (
                          ('0' . delim . '0') %{ pp->port = GPIOB; pp->pin =  8; } |
                          ('0' . delim . '1') %{ pp->port = GPIOB; pp->pin =  9; } |
                          ('1' . delim . '0') %{ pp->port = GPIOE; pp->pin =  5; } |
                          ('1' . delim . '1') %{ pp->port = GPIOE; pp->pin =  6; } |
                          ('2' . delim . '0') %{ pp->port = GPIOE; pp->pin =  9; } |
                          ('2' . delim . '1') %{ pp->port = GPIOE; pp->pin = 13; } |
                          ('3' . delim . '0') %{ pp->port = GPIOH; pp->pin = 10; } |
                          ('3' . delim . '1') %{ pp->port = GPIOH; pp->pin = 11; } |
                          ('3' . delim . '2') %{ pp->port = GPIOH; pp->pin = 12; } |
                          ('4' . delim . '0') %{ pp->port = GPIOA; pp->pin = 15; } 
                        );

  spi_port_pin        = 'SPI'i . delim? .
                        (
                          '0' . delim? . 'SCK'i  %{ pp->port = GPIOI; pp->pin =  1; } | 
                          '0' . delim? . 'MOSI'i %{ pp->port = GPIOI; pp->pin =  3; } | 
                          '0' . delim? . 'MISO'i %{ pp->port = GPIOI; pp->pin =  2; } | 
                          '0' . delim? . 'NSS'i  %{ pp->port = GPIOI; pp->pin =  0; } |
                          '1' . delim? . 'SCK'i  %{ pp->port = GPIOG; pp->pin = 13; } | 
                          '1' . delim? . 'MOSI'i %{ pp->port = GPIOG; pp->pin = 14; } | 
                          '1' . delim? . 'MISO'i %{ pp->port = GPIOG; pp->pin = 12; } | 
                          '1' . delim? . 'NSS'i  %{ pp->port = GPIOG; pp->pin =  8; }
                        );
  i2c_port_pin        = 'I2C'i . delim? . ( '0' . delim? )? .
                        (
                          'SDA'i %{ pp->port = GPIOF; pp->pin =  0; } |
                          'SCL'i %{ pp->port = GPIOF; pp->pin =  1; }
                        );
  uart_port_pin       = ('UART'i | 'USART'i | 'SERIAL'i ) . delim? .
                        (
                          '0' . delim? . 'TX'i  %{ pp->port = GPIOA; pp->pin =  0; } |
                          '0' . delim? . 'RX'i  %{ pp->port = GPIOA; pp->pin =  1; } |
                          '1' . delim? . 'TX'i  %{ pp->port = GPIOD; pp->pin =  8; } |
                          '1' . delim? . 'RX'i  %{ pp->port = GPIOD; pp->pin =  9; } |
                          '1' . delim? . 'CTS'i %{ pp->port = GPIOD; pp->pin = 11; } |
                          '1' . delim? . 'RTS'i %{ pp->port = GPIOD; pp->pin = 12; } |
                          '2' . delim? . 'TX'i  %{ pp->port = GPIOD; pp->pin =  5; } |
                          '2' . delim? . 'RX'i  %{ pp->port = GPIOD; pp->pin =  6; } |
                          '2' . delim? . 'CTS'i %{ pp->port = GPIOD; pp->pin =  3; } |
                          '2' . delim? . 'RTS'i %{ pp->port = GPIOD; pp->pin =  4; }
                        );

  main := ( stm_port_pin | digital_port_pin | timer_port_pin | spi_port_pin | i2c_port_pin | uart_port_pin ) . 0? @{ fbreak; };

  write data;
}%%

bool fetch_gpio_parser( const char * input_str, uint32_t max_input_len, port_pin_t * pp )
{
  uint32_t cs;
  const char * p = input_str;
  const char * pe = input_str + max_input_len;
  const char * eof = pe;

  // FIXME add chDbgAssert statements for pointers

  pp->port = NULL;
  pp->pin = 0;
  
  fetch_parser_info.error_msg = NULL;
  fetch_parser_info.fsm_state_first_final = %%{ write first_final; }%%;
  fetch_parser_info.fsm_state_start = %%{ write start; }%%;
  fetch_parser_info.fsm_state_error = %%{ write error; }%%;
  
  %% write init;
  %% write exec;

  fetch_parser_info.fsm_state = cs;
  fetch_parser_info.offset = (p-input_str);
  
  if( cs >= %%{ write first_final; }%% )
  {
    fetch_parser_info.error = false;
    return true;
  }
  else
  {
    fetch_parser_info.error = true;
    update_error_msg();
    return false;
  }
}

%%{
  machine fetch_gpio_port_parser;

  stm_port            = 'A'i >{ *port = GPIOA; } |
                        'B'i >{ *port = GPIOB; } |
                        'C'i >{ *port = GPIOC; } |
                        'D'i >{ *port = GPIOD; } |
                        'E'i >{ *port = GPIOE; } |
                        'F'i >{ *port = GPIOF; } |
                        'G'i >{ *port = GPIOG; } |
                        'H'i >{ *port = GPIOH; } |
                        'I'i >{ *port = GPIOI; };
  delim = [\-_.:];

  main := ( ('PORT'i . delim?) | ('GPIO'i . delim?) )? . stm_port . 0? @{ fbreak; };

  write data;
}%%

bool fetch_gpio_port_parser( const char * input_str, uint32_t max_input_len, ioportid_t * port )
{
  uint32_t cs;
  const char * p = input_str;
  const char * pe = input_str + max_input_len;
  const char * eof = pe;

  // FIXME add chDbgAssert statements for pointers

  *port = NULL;
  
  fetch_parser_info.error_msg = NULL;
  fetch_parser_info.fsm_state_first_final = %%{ write first_final; }%%;
  fetch_parser_info.fsm_state_start = %%{ write start; }%%;
  fetch_parser_info.fsm_state_error = %%{ write error; }%%;
  
  %% write init;
  %% write exec;

  fetch_parser_info.fsm_state = cs;
  fetch_parser_info.offset = (p-input_str);
  
  if( cs >= %%{ write first_final; }%% )
  {
    fetch_parser_info.error = false;
    return true;
  }
  else
  {
    fetch_parser_info.error = true;
    update_error_msg();
    return false;
  }
}


%%{
  machine fetch_hex_string_parser;

  action hex_nibble { value = (value << 4) | hex_value(*p); }
  action write_value {
      if( len < max_output_len )
      {
        *wp=value;
        wp++;
        len++;
      }
      else
      {
        fetch_parser_info.error_msg = "max output";
        fgoto *fetch_string_parser_error;
      }
    }

  data_byte = xdigit >hex_nibble . xdigit >hex_nibble >write_value;

  main := 'H'i . data_byte+ . 0? @{ fbreak; };

  write data;
}%%

bool fetch_hex_string_parser( const char * input_str, uint32_t max_input_len, char * output_str, uint32_t max_output_len, uint32_t * output_len )
{
  uint32_t cs;
  const char * p = input_str;
  const char * pe = input_str + max_input_len;
  const char * eof = pe;
  char * wp = output_str;

  // FIXME add chDbgAssert statements for pointers

  uint8_t value = 0;
  uint32_t len = 0;
  
  fetch_parser_info.error_msg = NULL;
  fetch_parser_info.fsm_state_first_final = %%{ write first_final; }%%;
  fetch_parser_info.fsm_state_start = %%{ write start; }%%;
  fetch_parser_info.fsm_state_error = %%{ write error; }%%;
  
  %% write init;
  %% write exec;

  *output_len = len;

  fetch_parser_info.fsm_state = cs;
  fetch_parser_info.offset = (p-input_str);
  
  if( cs >= %%{ write first_final; }%% )
  {
    fetch_parser_info.error = false;
    return true;
  }
  else
  {
    fetch_parser_info.error = true;
    update_error_msg();
    return false;
  }
}

