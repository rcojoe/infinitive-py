"""Flask-based web server for Infinitive API"""
from flask import Flask, request, jsonify
from flask_cors import CORS
from flask_sock import Sock
import json
import re
import logging

logger = logging.getLogger(__name__)


def launch_webserver(port, api):
    """Launch the web server"""
    app = Flask(__name__)
    CORS(app)
    sock = Sock(app)
    
    @app.errorhandler(Exception)
    def handle_error(error):
        logger.error(f"Error: {error}")
        return jsonify({"error": str(error)}), 500
    
    @app.route("/api/tstat/settings", methods=["GET"])
    def get_tstat_settings():
        tss = api.get_tstat_settings()
        if tss:
            return jsonify(tss)
        return {}, 204
    
    @app.route("/api/zone/<zone>/config", methods=["GET"])
    def get_zone_config(zone):
        try:
            zone_num = int(zone)
            if zone_num < 1 or zone_num > 8:
                return {"error": f"Invalid zone: {zone}"}, 400
            cfg = api.get_config(zone_num)
            if cfg:
                return jsonify(cfg)
            return {}, 204
        except ValueError:
            return {"error": f"Invalid zone: {zone}"}, 400
    
    @app.route("/api/airhandler", methods=["GET"])
    def get_air_handler():
        ah = api.get_air_handler()
        if ah:
            return jsonify(ah)
        return {}, 204
    
    @app.route("/api/heatpump", methods=["GET"])
    def get_heat_pump():
        hp = api.get_heat_pump()
        if hp:
            return jsonify(hp)
        return {}, 204
    
    @app.route("/api/zone/1/vacation", methods=["GET"])
    def get_vacation():
        vac = api.get_vacation()
        if vac:
            return jsonify(vac)
        return {}, 204
    
    @app.route("/api/zone/1/vacation", methods=["PUT"])
    def update_vacation():
        data = request.get_json()
        api.update_vacation(data)
        return {}, 200
    
    @app.route("/api/zone/<zone>/config", methods=["PUT"])
    def update_zone_config(zone):
        try:
            zone_num = int(zone)
            if zone_num < 1 or zone_num > 8:
                return {"error": f"Invalid zone: {zone}"}, 400
            data = request.get_json()
            api.update_zone_config(zone_num, data)
            return {}, 200
        except ValueError:
            return {"error": f"Invalid zone: {zone}"}, 400
    
    @app.route("/api/raw/<device>/<table>", methods=["GET"])
    def get_raw(device, table):
        if not re.match(r"^[a-f0-9]{4}$", device):
            return {"error": "Device must be 4 character hex string"}, 400
        if not re.match(r"^[a-f0-9]{6}$", table):
            return {"error": "Table must be 6 character hex string"}, 400
        
        dev_id = int(device, 16)
        table_bytes = bytes.fromhex(table)
        response = api.get_table_raw(dev_id, table_bytes)
        
        if response:
            return {"response": response.hex()}
        return {"error": "Timeout waiting for response"}, 504
    
    @sock.route("/api/ws")
    def websocket(ws):
        listener = api.new_listener()
        try:
            # Send current cache
            for source, data in api.cache.dump().items():
                ws.send(json.dumps({"source": source, "data": data}))
            
            # Listen for updates
            for message in listener.receive():
                ws.send(json.dumps({"source": message.source, "data": message.data}))
        finally:
            listener.close()
    
    @app.route("/", methods=["GET"])
    def index():
        return {"message": "Infinitive HVAC Controller"}
    
    app.run(host="0.0.0.0", port=port, debug=False)
