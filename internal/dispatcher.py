"""Event dispatcher for Infinity updates"""
import threading
import queue


class Event:
    def __init__(self, source, data):
        self.source = source
        self.data = data


class Listener:
    def __init__(self, ch, close_func):
        self.ch = ch
        self.close_func = close_func
    
    def receive(self):
        """Receive events"""
        while True:
            try:
                event = self.ch.get(timeout=1)
                if event is None:
                    break
                yield event
            except queue.Empty:
                continue
    
    def close(self):
        """Close listener"""
        self.close_func()


class Dispatcher:
    def __init__(self):
        self.listeners = set()
        self.lock = threading.RLock()
    
    def new_listener(self):
        """Create new listener"""
        ch = queue.Queue(maxsize=32)
        
        def close_func():
            with self.lock:
                if ch in self.listeners:
                    self.listeners.discard(ch)
        
        with self.lock:
            self.listeners.add(ch)
        
        return Listener(ch, close_func)
    
    def broadcast_event(self, source, data):
        """Broadcast event to all listeners"""
        event = Event(source, data)
        with self.lock:
            for ch in list(self.listeners):
                try:
                    ch.put_nowait(event)
                except queue.Full:
                    # Queue full, remove listener
                    self.listeners.discard(ch)
