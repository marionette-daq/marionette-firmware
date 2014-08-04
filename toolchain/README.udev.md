
# UDEV suggestions

* Accessing JTAG devices without sudo on Linux
    * Copy the 99-marionette-jtag.rules file to /etc/udev/rules.d/.

* To reload all udev rules:
    * sudo udevadm control --reload-rules
