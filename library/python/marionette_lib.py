import serial, threading, logging

# disbales outputting log data to console by default
logging.getLogger(__name__).addHandler(logging.NullHandler())

SERIAL_TIMEOUT = 3.0

class Marionette(object):
  def __init__(self, tty=None ):
    self.tty = tty
    self.serial = None

    if tty:
      self.open(tty)

  def open(self, tty=None):
    if tty:
      self.tty = tty

    self.serial = serial.Serial(port=self.tty, timeout=SERIAL_TIMEOUT)

    # setup for a non interactive shell
    self.shell_prompt(False)
    self.shell_echo(False)
    self.serial.flushInput()

  def close(self):
    if self.serial:
      self.serial.close()
      self.serial = None

  def is_open(self):
    if self.serial:
      return True
    else:
      return False

  def command(self, fmt, *args):
    if not self.serial:
      raise IOError("serial port not open")

    self.serial.flushInput()

    self.serial.write(fmt.strip() % args)
    self.serial.write("\r\n")

    begin_flag = False
    results = {}
    error_list = []
    logger = logging.getLogger(__name__)

    while True:
      line = self.serial.readline()

      if line is None or len(line) == 0:
        raise IOError("read timeout")

      param = line.lower().split(":", 2)
      param[0] = param[0].lower()

      if param[0] == "begin":
        begin_flag = True
      elif not begin_flag:
        # ignore lines before first begin statement
        continue
      elif param[0] == "end" and len(param) == 2:
        if param[1] == "ok" and len(error_list) == 0:
          return results
        elif param[1] == "error":
          raise Exception("\n".join(error_list))
        else:
          raise Exception("invalid end status")
      elif param[0] == "#": # info/comment
        logger.info(":".join(param[1:]))
      elif param[0] == "?": # debug
        logger.debug(":".join(param[1:]))
      elif param[0] == "e": # error
        error_str = ":".join(param[1:])
        error_list.append(error_str)
        logger.error(error_str)
      elif param[0] == "w": # warning
        logger.warning(":".join(param[1:]))
      elif param[0] == "b" and len(param) == 3: # bool
        results[param[1]] = param[2].lower().strip() in ('true', 't', '1')
      elif param[0] == "s" and len(param) == 3: # string
        results[param[1]] = param[2]
      elif param[0] == "sa" and len(param) == 3: # string array
        results[param[1]] = param[2].split(',')
      elif param[0] == "f" and len(param) == 3: # float
        results[param[1]] = map(float, param[2].split(','))
      elif param[0] in ("s8","u8","s16","u16","s32","u32") and len(param) == 3: # integer
        results[param[1]] = map(int, param[2].split(','))
      elif param[0] in ("h8","h16","h32") and len(param) == 3: # hex integer
        results[param[1]] = map(lambda x: int(x,16), param[2].split(','))
      else:
        raise Exception("invalid line format")


  # mshell commands

  def shell_prompt(self, enabled=False):
    if enabled:
      self.command("+prompt")
    else:
      self.command("+noprompt")

  def shell_echo(self, enabled=False):
    if enabled:
      self.command("+echo")
    else:
      self.command("+noecho")

  # fetch commands

  def fetch_version(self):
    return self.command("version")

  def fetch_heartbeat(self, enabled):
    if enabled:
      self.command("heartbeaton")
    else:
      self.command("heartbeatoff")

  def fetch_chip_id(self):
    return self.command("chipid")["chip_id"]

  def fetch_reset_pins(self):
    self.command("resetpins")

  # fetch gpio commands

  def fetch_gpio_read(self, port, pin):
    return self.command("gpio.read(%s,%s)", port, pin)["state"]

  def fetch_gpio_read_port(self, port):
    return self.command("gpio.readport(%s,%s)",port,pin)["state"][0]

  def fetch_gpio_write(self, port, pin, state):
    self.command("gpio.write(%s,%s,%s)", port, pin, state)

  def fetch_gpio_set(self, port, pin):
    self.command("gpio.set(%s,%s)", port, pin)

  def fetch_gpio_clear(self, port, pin):
    self.command("gpio.clear(%s,%s)", port, pin)

  def fetch_gpio_config(self, port, pin, mode):
    self.command("gpio.config(%s,%s,%s)", port, pin, mode)

  def fetch_gpio_info(self, port, pin):
    return self.command("gpio.info(%s,%s)", port, pin)

  def fetch_gpio_reset(self, port, pin):
    self.command("gpio.reset(%s,%s)", port, pin)

  def fetch_gpio_wait(self, port, pin, event, timeout):
    self.command("gpio.wait(%s,%s,%s,%s)", port, pin, event, timeout)

  # fetch adc commands

  def fetch_adc_start(self):
    self.command("adc.start")

  def fetch_adc_stop(self):
    self.command("adc.stop")

  def fetch_adc_wait(self, timeout=10):
    self.command("adc.wait(%s)", timeout)["ready"][0]

  def fetch_adc_status(self):
    self.command("adc.status")["ready"][0]

  def fetch_adc_samples(self):
    return self.command("adc.samples")

  def fetch_adc_config(self, dev, resolution, sample_clk, vref, count, channels):
    self.command("adc.config(%s,%s,%s,%s,%s,%s)", dev, resolution, sample_clk, vref, count, ",".join(channels))

  def fetch_adc_reset(self):
    self.command("adc.reset")

  # fetch dac commands

  def fetch_dac_config(self):
    self.command("dac.config")

  def fetch_dac_reset(self):
    self.command("dac.reset")

  def fetch_dac_write(self, channel_1, channel_2):
    self.command("dac.write(%s,%s)", channel_1, channel_2)

  # fetch spi commands

  def fetch_spi_config(self, dev, cpol, cpha, clk_div, order, port, pin):
    self.command("spi.config(%s,%s,%s,%s,%s,%s,%s)", dev, cpol, cpha, clk_div, order, port, pin)

  def fetch_spi_reset(self):
    self.command("spi.reset")

  def fetch_spi_exchange(self, tx_data):
    if not isinstance(tx_data, bytearray):
      raise TypeError("tx_data must be a bytearray")

    return self.command("spi.exchange(hex,%s", ",".join(map(lambda d: "%x" % d, tx_data)))["rx"]






