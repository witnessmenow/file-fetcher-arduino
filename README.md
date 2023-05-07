# file-fetcher-arduino

A library for getting files & images on arduino based boards (ESP mainly, maybe others)

Trying to make getting files (or images) as easy as possible while keeping it generic so it should hopefully work on most types of boards.

## Work in progress library!

Expect changes, and if you see ways of making it better, please either raise an issue or raise a PR!

## Help support what I do!

I have put a lot of effort into creating Arduino libraries that I hope people can make use of. [If you enjoy my work, please consider becoming a Github sponsor!](https://github.com/sponsors/witnessmenow/)

## How to Use:

1. Pass a Client into the constructor library e.g. `WiFiClientSecure`

```

WiFiClientSecure secured_client;
ImageFetcher fileFetcher(secured_client);

// If HTTPS make sure to handle it
// Either set a cert, fingerprint or set insecure

```

2. If you want to save the file to memory, pass in a pointer and a int (or full [example](/examples/saveImageToMemory))

```
uint8_t *fileFile; // pointer that the library will store the file at (uses malloc)
int fileSize;      // library will update the size of the file
bool gotImage = fileFetcher.getImage(fileUrl, &fileFile, &fileSize);

if(gotImage){
    // fileFile is now a pointer to memory that contains the file file
    // fileSize is the size of the file

    // Use it however you need to!

    free(fileFile); // Make sure to free the memory!
}


```

3. If you want to save the file to File (SPIFFS, SD etc), pass in a file pointer. (or full [example](/examples/saveImageToFlash))

```

fs::File f = SPIFFS.open("/img.png", "w+");
if (!f)
{
    Serial.println("file open failed");
    return -1;
}

bool gotImage = fileFetcher.getImage(fileUrl, &f);

// Make sure to close the file!
f.close();

// your image should now be in the file


```
