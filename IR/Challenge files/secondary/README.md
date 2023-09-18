# Secondary Payload


To build the secondary payload, there are a few pieces:

- Dump LSASS
- Second to webserver to parse the dump
- Wait for server to send creds
- Find DC on network
- Once found, connect using credentials found (PTH)


## Initial Ideas

- https://github.com/fortra/nanodump
- https://learn.microsoft.com/en-us/powershell/module/activedirectory/get-addomaincontroller?view=windowsserver2022-ps
- https://github.com/byt3bl33d3r/pth-toolkit


