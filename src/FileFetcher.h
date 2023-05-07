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

#ifndef FileFetcher_h
#define FileFetcher_h

// I find setting these types of flags unreliable from the Arduino IDE
// so uncomment this if its not working for you.

#define FILE_FETCHER_DEBUG 1

// Comment out if you want to disable any serial output from this library (also comment out DEBUG and PRINT_JSON_PARSE)
#define FILE_FETCHER_SERIAL_OUTPUT 1

#include <Arduino.h>
#include <Client.h>

#define FILE_FETCHER_TIMEOUT 2000

class FileFetcher
{
public:
    FileFetcher(Client &client);

    // Generic Request Methods
    int makeGetRequest(int portNumber, const char *command, const char *authorization, const char *accept, const char *host);

    // File methods
    bool getFile(char *fileUrl, Stream *file);
    bool getFile(char *fileUrl, uint8_t **file, int *fileLength);

    int httpsPortNumber = 443;
    int httpPortNumber = 80;
    Client *client;

private:
    int commonGetFile(char *fileUrl);
    int getContentLength();
    int getHttpStatusCode();
    void skipHeaders();
    void closeClient();
};

#endif
