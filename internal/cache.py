"""Cache for Infinity data"""
import threading


class Cache:
    def __init__(self, on_update_func=None):
        self.data = {}
        self.lock = threading.RLock()
        self.on_update_func = on_update_func
    
    def update(self, name, data):
        """Update cache entry"""
        with self.lock:
            if self.data.get(name) != data:
                self.data[name] = data
                if self.on_update_func:
                    self.on_update_func(name, data)
    
    def get(self, name):
        """Get cache entry"""
        with self.lock:
            return self.data.get(name)
    
    def dump(self):
        """Get all cache entries"""
        with self.lock:
            return dict(self.data)
