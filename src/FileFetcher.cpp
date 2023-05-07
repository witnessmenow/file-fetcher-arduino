/*
FileFetcher - A library for fetching files & images from the web

Copyright (c) 2023  Brian Lough.

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
*/

#include "FileFetcher.h"

FileFetcher::FileFetcher(Client &client)
{
    this->client = &client;
}

int FileFetcher::makeGetRequest(int portNumber, const char *command, const char *authorization, const char *accept, const char *host)
{
    client->flush();
    client->setTimeout(FILE_FETCHER_TIMEOUT);
    if (!client->connect(host, portNumber))
    {
#ifdef FILE_FETCHER_SERIAL_OUTPUT
        Serial.println(F("Connection failed"));
#endif
        return -1;
    }

    // give the esp a breather
    yield();

    // Send HTTP request
    client->print(F("GET "));
    client->print(command);
    client->println(F(" HTTP/1.0"));

    // Headers
    client->print(F("Host: "));
    client->println(host);

    if (accept != NULL)
    {
        client->print(F("Accept: "));
        client->println(accept);
    }

    if (authorization != NULL)
    {
        client->print(F("Authorization: "));
        client->println(authorization);
    }

    client->println(F("User-Agent: Arduino"));

    client->println(F("Cache-Control: no-cache"));

    if (client->println() == 0)
    {
#ifdef FILE_FETCHER_SERIAL_OUTPUT
        Serial.println(F("Failed to send request"));
#endif
        return -2;
    }

    int statusCode = getHttpStatusCode();

    return statusCode;
}

int FileFetcher::commonGetFile(char *fileUrl)
{
#ifdef FILE_FETCHER_DEBUG
    Serial.print(F("Parsing file URL: "));
    Serial.println(fileUrl);
#endif

    uint8_t lengthOfString = strlen(fileUrl);

    // We are going to just assume https, that's all I've
    // seen and I can't imagine a company will go back
    // to http

    uint8_t protocolLength = 0;
    int portNumber = 80;

    if (strncmp(fileUrl, "https://", 8) == 0)
    {
        // Is HTTPS
        protocolLength = 8;
        portNumber = this->httpsPortNumber;
    }
    else if (strncmp(fileUrl, "http://", 7) == 0)
    {
        // Is HTTP
        protocolLength = 7;
        portNumber = this->httpPortNumber;
    }
    else
    {
#ifdef FILE_FETCHER_SERIAL_OUTPUT
        Serial.print(F("Url not in expected format: "));
        Serial.println(fileUrl);
        Serial.println("(expected it to start with \"https://\" or \"http://\")");
#endif
    }

    char *pathStart = strchr(fileUrl + protocolLength, '/');
    uint8_t pathIndex = pathStart - fileUrl;
    uint8_t pathLength = lengthOfString - pathIndex;
    char path[pathLength + 1];
    strncpy(path, pathStart, pathLength);
    path[pathLength] = '\0';

    uint8_t hostLength = pathIndex - protocolLength;
    char host[hostLength + 1];
    strncpy(host, fileUrl + protocolLength, hostLength);
    host[hostLength] = '\0';

#ifdef FILE_FETCHER_DEBUG

    Serial.print(F("host: "));
    Serial.println(host);

    Serial.print(F("len:host:"));
    Serial.println(hostLength);

    Serial.print(F("path: "));
    Serial.println(path);

    Serial.print(F("len:path: "));
    Serial.println(strlen(path));
#endif

    int statusCode = makeGetRequest(portNumber, path, NULL, "text/html,application/xhtml+xml,application/xml;q=0.9,file/webp,*/*;q=0.8", host);
#ifdef FILE_FETCHER_DEBUG
    Serial.print(F("statusCode: "));
    Serial.println(statusCode);
#endif
    if (statusCode == 200)
    {
        return getContentLength();
    }

    // Failed
    return -1;
}

bool FileFetcher::getFile(char *fileUrl, Stream *file)
{
    int totalLength = commonGetFile(fileUrl);

#ifdef FILE_FETCHER_DEBUG
    Serial.print(F("file length: "));
    Serial.println(totalLength);
#endif
    if (totalLength > 0)
    {
        skipHeaders();
        int remaining = totalLength;
        // This section of code is inspired but the "Web_Jpg"
        // example of TJpg_Decoder
        // https://github.com/Bodmer/TJpg_Decoder
        // -----------
        uint8_t buff[128] = {0};
        while (client->connected() && (remaining > 0 || remaining == -1))
        {
            // Get available data size
            size_t size = client->available();

            if (size)
            {
                // Read up to 128 bytes
                int c = client->readBytes(buff, ((size > sizeof(buff)) ? sizeof(buff) : size));

                // Write it to file
                file->write(buff, c);

                // Calculate remaining bytes
                if (remaining > 0)
                {
                    remaining -= c;
                }
            }
            yield();
        }
// ---------
#ifdef FILE_FETCHER_DEBUG
        Serial.println(F("Finished getting file"));
#endif
    }

    closeClient();

    return (totalLength > 0); // Probably could be improved!
}

bool FileFetcher::getFile(char *fileUrl, uint8_t **file, int *fileLength)
{
    int totalLength = commonGetFile(fileUrl);

#ifdef FILE_FETCHER_DEBUG
    Serial.print(F("file length: "));
    Serial.println(totalLength);
#endif
    if (totalLength > 0)
    {
        skipHeaders();
        uint8_t *imgPtr = (uint8_t *)malloc(totalLength);
        *file = imgPtr;
        *fileLength = totalLength;
        int remaining = totalLength;
        int amountRead = 0;

#ifdef FILE_FETCHER_DEBUG
        Serial.println(F("Fetching File"));
#endif

        // This section of code is inspired but the "Web_Jpg"
        // example of TJpg_Decoder
        // https://github.com/Bodmer/TJpg_Decoder
        // -----------
        uint8_t buff[128] = {0};
        while (client->connected() && (remaining > 0 || remaining == -1))
        {
            // Get available data size
            size_t size = client->available();

            if (size)
            {
                // Read up to 128 bytes
                int c = client->readBytes(buff, ((size > sizeof(buff)) ? sizeof(buff) : size));

                // Write it to file
                memcpy((uint8_t *)imgPtr + amountRead, (uint8_t *)buff, c);

                // Calculate remaining bytes
                if (remaining > 0)
                {
                    amountRead += c;
                    remaining -= c;
                }
            }
            yield();
        }
// ---------
#ifdef FILE_FETCHER_DEBUG
        Serial.println(F("Finished getting file"));
#endif
    }

    closeClient();

    return (totalLength > 0); // Probably could be improved!
}

int FileFetcher::getContentLength()
{

    if (client->find("Content-Length:"))
    {
        int contentLength = client->parseInt();
#ifdef FILE_FETCHER_DEBUG
        Serial.print(F("Content-Length: "));
        Serial.println(contentLength);
#endif
        return contentLength;
    }

    return -1;
}

void FileFetcher::skipHeaders()
{

    if (!client->find("\r\n\r\n"))
    {
#ifdef FILE_FETCHER_SERIAL_OUTPUT
        Serial.println(F("Invalid response"));
#endif
        return;
    }
}

int FileFetcher::getHttpStatusCode()
{
    char status[32] = {0};
    client->readBytesUntil('\r', status, sizeof(status));
#ifdef FILE_FETCHER_DEBUG
    Serial.print(F("Status: "));
    Serial.println(status);
#endif

    char *token;
    token = strtok(status, " "); // https://www.tutorialspoint.com/c_standard_library/c_function_strtok.htm

#ifdef FILE_FETCHER_DEBUG
    Serial.print(F("HTTP Version: "));
    Serial.println(token);
#endif

    if (token != NULL && (strcmp(token, "HTTP/1.0") == 0 || strcmp(token, "HTTP/1.1") == 0))
    {
        token = strtok(NULL, " ");
        if (token != NULL)
        {
#ifdef FILE_FETCHER_DEBUG
            Serial.print(F("Status Code: "));
            Serial.println(token);
#endif
            return atoi(token);
        }
    }

    return -1;
}

void FileFetcher::closeClient()
{
    if (client->connected())
    {
#ifdef FILE_FETCHER_DEBUG
        Serial.println(F("Closing client"));
#endif
        client->stop();
    }
}