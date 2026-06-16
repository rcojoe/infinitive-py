"""Main entry point for Infinitive HVAC controller"""
import argparse
import logging
from infinity.api import InfinityApi
from webserver import launch_webserver

logging.basicConfig(level=logging.DEBUG)
logger = logging.getLogger(__name__)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--httpport", type=int, default=8080, help="HTTP port to listen on")
    parser.add_argument("--serial", type=str, required=True, help="Path to serial port")
    
    args = parser.parse_args()
    
    try:
        api = InfinityApi(args.serial)
        launch_webserver(args.httpport, api)
    except Exception as e:
        logger.error(f"Error opening serial port: {e}")
        raise


if __name__ == "__main__":
    main()
