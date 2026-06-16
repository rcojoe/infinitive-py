"""Conversion functions for Infinity protocol values"""


def raw_mode_to_string(mode):
    """Convert raw mode byte to string"""
    modes = {
        0: "heat",
        1: "cool",
        2: "auto",
        3: "electric",
        4: "heatpump",
        5: "off",
    }
    return modes.get(mode, "unknown")


def string_mode_to_raw(mode):
    """Convert mode string to raw byte"""
    modes = {
        "heat": 0,
        "cool": 1,
        "auto": 2,
        "electric": 3,
        "heatpump": 4,
        "off": 5,
    }
    return modes.get(mode, 5)


def raw_fan_mode_to_string(mode):
    """Convert raw fan mode byte to string"""
    modes = {
        0: "auto",
        1: "low",
        2: "med",
        3: "high",
    }
    return modes.get(mode, "unknown")


def string_fan_mode_to_raw(mode):
    """Convert fan mode string to raw byte"""
    modes = {
        "auto": 0,
        "low": 1,
        "med": 2,
        "high": 3,
    }
    return modes.get(mode, 0)
