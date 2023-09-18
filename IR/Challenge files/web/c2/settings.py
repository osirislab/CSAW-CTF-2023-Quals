import os

from dotenv import load_dotenv

load_dotenv()

UPLOAD_DIR = os.environ.get("UPLOAD_DIR")
STATIC_DIR = os.environ.get("STATIC_DIR")
SECRETS_LOCATION = os.environ.get("SECRETS_LOCATION")
