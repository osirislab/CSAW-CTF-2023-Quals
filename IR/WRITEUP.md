# What is going on?
Find the ransom note to get the flag. It is located in the `C:\` folder

# Initial Access
The attackers got in by phishing. They sent a malicious attachment and got in that way. 

The email that contained the malicious document had the subject line of `Monthly Finance Report - July`. 

The SHA1 hash of the attachment is `7b0b63e902504bfd7976e3911e054148f335a2e9`. 

This document uses the Follina exploit which has the CVE ID of `CVE-2022-30190` to get in.

The document dropped a file called `coms.exe` on the machine.

# Communicator
The file coms.exe does communication between the infected host and the attacker's Command and Control (C2C) server. This communicator will do the following steps:
- Dump LSASS
- Find the Domain Controller (DC) server on the infected host's network and get its IP
- Send the dump and the DC's IP to the C2C server to parse the dump
- Get the command to perform the Pass The Hash (PtH) attack using the credentials extracted from the dump by the C2C server. The credentials are the username and the NTLM hash of the password and the command is base64 encoded and RC4 encrypted.
- Connect to the DC using the command given in step 4
- Download ransomware on the DC, execute it and securely delete the ransomware
- Download ransomware on the infected host, execute it and securely delete the ransomware

The lateral movement uses the SMBExec tool which executes the powershell command to download the ransomware by starting a service on the remote host by modifying certain registry keys (similar to PSExec).

Answer to the questions:
- The `lsass.exe` is the process whose memory is being dumped to `C:\\Windows\\Temp\\FXSTIFFDebugLogFileAdmin.txt`
- The command being sent to the server starts with `TgBlAHcALQBNAG8AZAB1AGwAZQAgAC0A` base64 (considering utf16) or `TmV3LU1vZHVsZSAtTmFtZSBMYVRtMHYg` base64 (considering utf8)
- The communicator pulls `lat.ps1` to do lateral movement and `runs0mewhere.exe` which is the ransomware
- The algorithm used for encryption at step 4 is `RC4`
- The key is base85 encoded and XORed with 0x32 in the binary and it is `!!!!!w0wURG0od@r3v__:O:O`

# Ransomware
The file `runs0mewhere.exe` contains the C# ransomware. It can be easily pulled apart using dnSpy. The ransomware only goes after files in the `C:\Users\` folder.

In the ransomware, you will see that files with extension of `.exe,.dll,.lnk,.sys,.msi,.exten,.ost,.pst` are not encrypted and files in the folders with the names of `tmp,winnt,temp,thumb,$recycle.bin,system volume information,boot,windows,downloads` are skipped as well.

You can find through some research that the `Conti` ransomware source code was leaked last year and it available on GitHub. They also avoid most of these extensions and folders.

You can also see that `RSA` and `AES-CBC` is used for the encryption of each file. But, since we use a 4096 bit key for RSA, that makes it pretty much impossible to retrieve the private key from the public key.

# C2C Server
The C2C server has an API that is used by the `coms.exe` to download files. The file downloading endpoint (aka `/static`) is vulnerable to Local File Inclusion (LFI). Using `/static?file=../flag.txt`, you can get the flag and also get a hint on the file name of the private key, which is at `/static?file=../private_key.txt`.

# Recover Host files
With the private key, we can recover the flag in the file `Hudson_Hustles_new_flyer.EXTEN`.

# Recover Domain Controller files
With the private key, we can recover the flag in the file `Tiger_Woods_Hustlin.EXTEN`.