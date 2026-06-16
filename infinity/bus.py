"""Serial bus communication for Infinity protocol"""
import serial
import struct
import logging
import threading
import time
import queue

logger = logging.getLogger(__name__)

# Constants
DEV_SAM = 0x4000
DEV_TSTAT = 0x4000

READ_TABLE_BLOCK = 0x60
WRITE_TABLE_BLOCK = 0x61
ACK_06 = 0x06
ACK_02 = 0x02


class Bus:
    def __init__(self, port_name):
        self.port = None
        self.port_name = port_name
        self.response_timeout = 3.0
        self.response_retries = 3
        self.action_ch = queue.Queue()
        self.response_ch = queue.Queue()
        self.open_serial()
        
        # Start reader thread
        self.reader_thread = threading.Thread(target=self._reader, daemon=True)
        self.reader_thread.start()
        
        # Start broker thread
        self.broker_thread = threading.Thread(target=self._broker, daemon=True)
        self.broker_thread.start()
    
    def open_serial(self):
        """Open serial port"""
        try:
            self.port = serial.Serial(self.port_name, baudrate=19200, timeout=1)
            logger.info(f"Opened serial port {self.port_name}")
        except Exception as e:
            logger.error(f"Failed to open serial port: {e}")
            raise
    
    def _reader(self):
        """Read from serial port"""
        msg = b""
        while True:
            try:
                if not self.port:
                    self.open_serial()
                    time.sleep(0.5)
                    continue
                
                data = self.port.read(256)
                if not data:
                    time.sleep(0.1)
                    continue
                
                msg += data
                
                while len(msg) >= 10:
                    frame_len = msg[4] + 10
                    if len(msg) < frame_len:
                        break
                    
                    buf = msg[:frame_len]
                    frame = Frame()
                    if frame.decode(buf):
                        # Handle frame
                        self.response_ch.put(frame)
                    
                    # Remove processed frame from message buffer
                    msg = msg[frame_len:]
            except Exception as e:
                logger.error(f"Serial read error: {e}")
                if self.port:
                    try:
                        self.port.close()
                    except:
                        pass
                self.port = None
                time.sleep(1)
    
    def _broker(self):
        """Process actions from action queue"""
        try:
            while True:
                action = self.action_ch.get(timeout=1)
                self._perform_action(action)
        except queue.Empty:
            pass
        except Exception as e:
            logger.error(f"Broker error: {e}")
    
    def _perform_action(self, action):
        """Perform a bus action"""
        logger.info(f"Encoded frame: {action['frame']}")
        encoded = action['frame'].encode()
        self._send_frame(encoded)
        
        start_time = time.time()
        timeout = self.response_timeout
        tries = 0
        
        while tries < self.response_retries:
            try:
                res = self.response_ch.get(timeout=timeout)
                
                if res.src != action['frame'].dst:
                    continue
                
                req_table = action['frame'].data[0:3]
                res_table = res.data[0:3]
                
                if action['frame'].op == READ_TABLE_BLOCK and req_table != res_table:
                    logger.warning(f"Got response for incorrect table, is: {res_table.hex()} expected: {req_table.hex()}")
                    continue
                
                action['response'] = res
                action['ok'] = True
                action['ch'].put(True)
                return
            except queue.Empty:
                logger.debug("Timeout waiting for response, retransmitting frame")
                self._send_frame(encoded)
                tries += 1
        
        logger.warning("Action timed out")
        action['ch'].put(False)
    
    def _send_frame(self, data):
        """Send frame to serial port"""
        if self.port:
            try:
                self.port.write(data)
            except Exception as e:
                logger.error(f"Error sending frame: {e}")
    
    def read_table(self, device_id, table_name):
        """Read table from device"""
        # Implementation depends on table definitions
        logger.warning(f"read_table not fully implemented for {table_name}")
        return None
    
    def write_table(self, device_id, table_name, data):
        """Write table to device"""
        logger.warning(f"write_table not fully implemented for {table_name}")
    
    def write_zone_field(self, zone, field_name, value):
        """Write a zone field"""
        logger.warning(f"write_zone_field not fully implemented")
    
    def read_table_raw(self, device_id, table_addr):
        """Read raw table data"""
        logger.warning("read_table_raw not fully implemented")
        return None


class Frame:
    def __init__(self, src=None, dst=None, op=None, data=None):
        self.src = src
        self.dst = dst
        self.op = op
        self.data = data or b""
    
    def encode(self):
        """Encode frame to bytes"""
        if len(self.data) > 255:
            raise ValueError("Frame data too large")
        
        buf = b""
        buf += struct.pack(">H", self.dst)
        buf += struct.pack(">H", self.src)
        buf += struct.pack("B", len(self.data))
        buf += b"\x00\x00"
        buf += struct.pack("B", self.op)
        buf += self.data
        
        cksum = self._checksum(buf)
        return buf + cksum
    
    def decode(self, buf):
        """Decode frame from bytes"""
        if len(buf) < 10:
            return False
        
        # Check if all zeros
        if all(b == 0 for b in buf):
            return False
        
        # Verify checksum
        cksum = self._checksum(buf[:-2])
        if cksum != buf[-2:]:
            return False
        
        self.dst = struct.unpack(">H", buf[0:2])[0]
        self.src = struct.unpack(">H", buf[2:4])[0]
        self.data_len = buf[4]
        self.op = buf[7]
        self.data = buf[8:-2]
        return True
    
    def __str__(self):
        return f"{self.src:04x} -> {self.dst:04x}: op={self.op:02x} data={self.data.hex()}"
    
    @staticmethod
    def _checksum(data):
        """Calculate CRC16 checksum"""
        # CRC16 CCITT implementation
        crc = 0x0000
        for byte in data:
            crc ^= byte << 8
            for _ in range(8):
                crc <<= 1
                if crc & 0x10000:
                    crc ^= 0x1021
                crc &= 0xFFFF
        return struct.pack(">H", crc)
