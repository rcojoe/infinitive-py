import unittest
from unittest.mock import patch

from infinity.bus import Bus


def _new_bus_for_open(port_name):
    bus = Bus.__new__(Bus)
    bus.port = None
    bus.port_name = port_name
    return bus


class TestBusOpenSerial(unittest.TestCase):
    @patch("infinity.bus.serial.serial_for_url")
    def test_open_serial_uses_serial_for_url_for_device_paths(self, serial_for_url):
        expected_port = object()
        serial_for_url.return_value = expected_port
        bus = _new_bus_for_open("/dev/ttyUSB0")

        bus.open_serial()

        serial_for_url.assert_called_once_with("/dev/ttyUSB0", baudrate=19200, timeout=1)
        self.assertIs(bus.port, expected_port)

    @patch("infinity.bus.serial.serial_for_url")
    def test_open_serial_uses_serial_for_url_for_socket_urls(self, serial_for_url):
        expected_port = object()
        serial_for_url.return_value = expected_port
        bus = _new_bus_for_open("socket://192.168.1.100:2000")

        bus.open_serial()

        serial_for_url.assert_called_once_with(
            "socket://192.168.1.100:2000", baudrate=19200, timeout=1
        )
        self.assertIs(bus.port, expected_port)
