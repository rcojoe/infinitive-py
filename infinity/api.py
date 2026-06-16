"""Infinity HVAC system API"""
from .bus import Bus
from .conversions import *
from internal.cache import Cache
from internal.dispatcher import Dispatcher
import logging
import struct

logger = logging.getLogger(__name__)


class InfinityApi:
    def __init__(self, serial_port):
        self.bus = Bus(serial_port)
        self.cache = Cache()
        self.dispatcher = Dispatcher()
        
    def get_config(self, zone):
        """Get zone configuration"""
        cfg = self.bus.read_table(0x4000, "TStatZoneParams")  # DevTSTAT
        if not cfg:
            return None
        
        params = self.bus.read_table(0x4000, "TStatCurrentParams")
        if not params:
            return None
        
        hold = bool(cfg.get("zone_hold") & (1 << (zone - 1)))
        
        return {
            "currentTemp": params.get(f"z{zone}_current_temp", 0),
            "currentHumidity": params.get(f"z{zone}_current_humidity", 0),
            "outdoorTemp": params.get("outdoor_air_temp", 0),
            "mode": raw_mode_to_string(params.get("mode", 0) & 0xf),
            "stage": params.get("mode", 0) >> 5,
            "fanMode": raw_fan_mode_to_string(cfg.get(f"z{zone}_fan_mode", 0)),
            "hold": hold,
            "heatSetpoint": cfg.get(f"z{zone}_heat_setpoint", 0),
            "coolSetpoint": cfg.get(f"z{zone}_cool_setpoint", 0),
        }
    
    def get_air_handler(self):
        """Get air handler status"""
        return self.cache.get("air_handler")
    
    def get_heat_pump(self):
        """Get heat pump status"""
        return self.cache.get("heat_pump")
    
    def get_tstat_settings(self):
        """Get thermostat settings"""
        return self.cache.get("tstat")
    
    def get_vacation(self):
        """Get vacation settings"""
        vac = self.bus.read_table(0x4000, "TStatVacationParams")
        if not vac:
            return None
        return self._vacation_to_api(vac)
    
    def update_vacation(self, config):
        """Update vacation settings"""
        vac = {}
        if config.get("days"):
            vac["hours"] = config["days"] * 24
        if config.get("minTemperature"):
            vac["min_temperature"] = config["minTemperature"]
        if config.get("maxTemperature"):
            vac["max_temperature"] = config["maxTemperature"]
        if config.get("minHumidity"):
            vac["min_humidity"] = config["minHumidity"]
        if config.get("maxHumidity"):
            vac["max_humidity"] = config["maxHumidity"]
        if config.get("fanMode"):
            vac["fan_mode"] = string_fan_mode_to_raw(config["fanMode"])
        
        self.bus.update_table(0x4000, "TStatVacationParams", vac)
    
    def update_zone_config(self, zone, config):
        """Update zone configuration"""
        if config.get("fanMode"):
            mode = string_fan_mode_to_raw(config["fanMode"])
            self.bus.write_zone_field(zone, "fan_mode", mode)
        
        if config.get("hold") is not None:
            current = self.bus.read_table(0x4000, "TStatZoneParams")
            if current:
                zone_hold = current.get("zone_hold", 0)
                if config["hold"]:
                    zone_hold |= (1 << (zone - 1))
                else:
                    zone_hold &= ~(1 << (zone - 1))
                self.bus.write_table(0x4000, "TStatZoneParams", {"zone_hold": zone_hold})
        
        if config.get("heatSetpoint", 0) > 0:
            self.bus.write_zone_field(zone, "heat_setpoint", config["heatSetpoint"])
        
        if config.get("coolSetpoint", 0) > 0:
            self.bus.write_zone_field(zone, "cool_setpoint", config["coolSetpoint"])
        
        if config.get("mode"):
            mode = string_mode_to_raw(config["mode"])
            self.bus.write_table(0x4000, "TStatCurrentParams", {"mode": mode})
    
    def get_table_raw(self, device_id, table_addr):
        """Get raw table data"""
        return self.bus.read_table_raw(device_id, table_addr)
    
    def new_listener(self):
        """Create new event listener"""
        return self.dispatcher.new_listener()
