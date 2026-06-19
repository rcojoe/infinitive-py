"""Flask-based web server for Infinitive API"""
from flask import Flask, request, jsonify, send_from_directory
from flask_cors import CORS
from flask_sock import Sock
import json
import re
import logging
import os

logger = logging.getLogger(__name__)


def launch_webserver(port, api):
    """Launch the web server"""
    app = Flask(__name__, static_folder="static", static_url_path="/static")
    CORS(app)
    sock = Sock(app)

    @app.errorhandler(Exception)
    def handle_error(error):
        logger.error(f"Error: {error}")
        return jsonify({"error": str(error)}), 500

    @app.route("/api/health", methods=["GET"])
    def health():
        return {"message": "Infinitive HVAC Controller"}

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
            for source, data in api.cache.dump().items():
                ws.send(json.dumps({"source": source, "data": data}))
            for message in listener.receive():
                ws.send(json.dumps({"source": message.source, "data": message.data}))
        finally:
            listener.close()

    @app.route("/favicon.ico")
    def favicon():
        favicon_path = os.path.join(app.static_folder, "favicon.ico")
        if os.path.exists(favicon_path):
            return send_from_directory(
                app.static_folder, "favicon.ico",
                mimetype="image/vnd.microsoft.icon"
            )
        return "", 204

    @app.route("/", methods=["GET"])
    def index():
        index_path = os.path.join(app.static_folder, "index.html")
        if os.path.exists(index_path):
            return send_from_directory(app.static_folder, "index.html")
        return (
            "<h1>Infinitive HVAC Controller</h1>"
            "<p>UI not found. Place your built frontend at static/index.html</p>",
            200,
            {"Content-Type": "text/html; charset=utf-8"},
        )

    @app.route("/<path:path>", methods=["GET"])
    def static_proxy(path):
        if path.startswith("api/"):
            return {"error": "Not Found"}, 404

        full_path = os.path.join(app.static_folder, path)
        if os.path.exists(full_path) and os.path.isfile(full_path):
            return send_from_directory(app.static_folder, path)

        index_path = os.path.join(app.static_folder, "index.html")
        if os.path.exists(index_path):
            return send_from_directory(app.static_folder, "index.html")

        return {"error": "Not Found"}, 404

    app.run(host="0.0.0.0", port=port)