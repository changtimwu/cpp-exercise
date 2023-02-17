#include <fstream>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <iterator>
#include <stdlib.h>

#pragma pack(push, 1)
typedef struct {
    uint16_t bfType;     /* fixed BM */
    uint32_t bfSize;     /* file size */
    uint16_t bfReserved1;
    uint16_t bfReserved2;
    uint32_t bfOffBits;  /* offset of pixels aka total length of headers*/ 
} BITMAPFILEHEADER;

typedef struct {
    uint32_t biSize;         /*bmp info size;  always 40 bytes */
    int32_t biWidth;
    int32_t biHeight;        
    uint16_t biPlanes;       /* must be 1 */
    uint16_t biBitCount;     /* 8 */
    uint32_t biCompression;  /* 1 for rle8 */
    uint32_t biSizeImage;   /* byte of pixels(after compression)  */
    int32_t biXPelsPerMeter; 
    int32_t biYPelsPerMeter;
    uint32_t biClrUsed;        /* number of colors in palette*/
    uint32_t biClrImportant;  /* ignored: fill save as above */
} BITMAPINFOHEADER;
#pragma pack(pop)


class BmpRasterizer {
  private:
    int width;
    int height;
    std::vector<char> head;
    std::vector<char> data;
    std::string pixfmt;  // 8RLE, 1BPP 

  public:
    BmpRasterizer(int width, int height) : width(width), height(height) {
        int padding = (4 - (width * 3) % 4) % 4;
        int size = width * height * 3 + height * padding;
        data.reserve(size + 54);
        // Append the binary content of myStruct to the data vector
        //data.insert(data.end(), reinterpret_cast<const uint8_t*>(&myStruct), reinterpret_cast<const uint8_t*>(&myStruct + 1));
        // BMP header
        data.push_back('B');
        data.push_back('M');
        data.push_back(size & 0xFF);
        data.push_back((size >> 8) & 0xFF);
        data.push_back((size >> 16) & 0xFF);
        data.push_back((size >> 24) & 0xFF);
        for (int i = 0; i < 4; i++) data.push_back('\0');
        data.push_back(54);
        for (int i = 0; i < 3; i++) data.push_back('\0');
        data.push_back(40);
        for (int i = 0; i < 3; i++) data.push_back('\0');
        data.push_back(width & 0xFF);
        data.push_back((width >> 8) & 0xFF);
        data.push_back((width >> 16) & 0xFF);
        data.push_back((width >> 24) & 0xFF);
        data.push_back(height & 0xFF);
        data.push_back((height >> 8) & 0xFF);
        data.push_back((height >> 16) & 0xFF);
        data.push_back((height >> 24) & 0xFF);
        data.push_back(1);
        for (int i = 0; i < 3; i++) data.push_back('\0');
        data.push_back(24);
        for (int i = 0; i < 25; i++) data.push_back('\0');
    }

       /* 
    int bmpencode_1bpp( std::vector<float> &ind) {
        int32_t dataSize =width * height;
        int32_t palSize;
        BITMAPFILEHEADER bfh = {0};
        bfh.bfType = 0x4d42;
        BITMAPINFOHEADER bih = {0};
        bih.biSize = sizeof(BITMAPINFOHEADER);
        bih.biWidth = width;
        bih.biHeight = height;

        bih.biPlanes = 1;
        bih.biBitCount = 8;
        bih.biCompression = 1;
        bih.biClrUsed = 2;
        bih.biClrImportant = bih.biClrUsed;
        bih.biXPelsPerMeter = bih.biWidth*10; 
        bih.biYPelsPerMeter = bih.biHeight*10;
        palSize = bih.biClrUsed * 4;
        bfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + palSize;

        uint8_t *data = (uint8_t*) malloc(dataSize);
        for (int i = 0; i < height; i++) {
            for (int j = 0; j < width; j++) {
                int index = i * width + j;
                data[index] = i==j ? 0xff:0x00;
            }
        }
        uint8_t *bmpData = (uint8_t *)malloc(dataSize * sizeof(uint8_t));
        uint32_t palData[] = { 0x00000000, 0xffffffff }; 
        // Fill in `data` with some image data
        int bmpSize = OneBppEncode( &bih, data, dataSize, bmpData);
        bfh.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + palSize + bmpSize;
        bih.biSizeImage = bmpSize;

        FILE *f = fopen( dstfn, "wb");
        fwrite(&bfh, sizeof(bfh), 1, f);
        printf("after bmp header offset is %ld\n", ftell(f));
        fwrite(&bih, sizeof(bih), 1, f);
        printf("after bmp info offset is %ld\n", ftell(f));
        fwrite( palData, palSize, 1, f);
        printf("after pal offset is %ld\n", ftell(f));
        fwrite( bmpData, bmpSize, 1, f);
     
        fclose(f);
        return 0;
    }
*/
    void importFrom( int w, int h, const std::vector<uint8_t> &pixels) {
        width = w;
        height = h;
        pixfmt = "8RLE";
    }

    void importFrom( int w, int h, const std::vector<float> &srcpixels) {
        width = w;
        height = h;
        pixfmt = "1BPP";

        int32_t dataSize = width * height;
        int32_t palSize;
        BITMAPFILEHEADER bfh = {0};
        bfh.bfType = 0x4d42;
        BITMAPINFOHEADER bih = {0};
        bih.biSize = sizeof(BITMAPINFOHEADER);
        bih.biWidth = width;
        bih.biHeight = height;

        bih.biPlanes = 1;
        bih.biBitCount = 8;
        bih.biCompression = 1;
        bih.biClrUsed = 2;
        bih.biClrImportant = bih.biClrUsed;
        bih.biXPelsPerMeter = bih.biWidth*10; 
        bih.biYPelsPerMeter = bih.biHeight*10;
        palSize = bih.biClrUsed * 4;
        bfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + palSize;

        uint32_t palData[] = { 0x00000000, 0xffffffff };
        auto bmpdata = OneBppEncode( &bih, srcpixels);
        bih.biSizeImage = bmpdata.size();
        bfh.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + palSize + bih.biSizeImage;
        // serialize all
        data.clear();
        data.append(&bfh);
        data.append(&bih);
        data.append(&palData);
        data.append(bmpdata);
    }

    void setPixel(int x, int y, char r, char g, char b) {
        int padding = (4 - (width * 3) % 4) % 4;
        int rowSize = width * 3 + padding;
        int offset = rowSize * (height - y - 1) + x * 3;
        data[offset] = b;
        data[offset + 1] = g;
        data[offset + 2] = r;
    }

    void setRaw( std::vector<char> aa ) {
    }

    void save( const char* filename) {
        std::ofstream file(filename, std::ios::binary);
        file.write(&data[0], data.size());
        file.close();
    }

    std::string toBase64() {
        return base64_encode( data);
    }

    private:
    inline std::string base64_encode(const std::vector<char>& data) {
        static const std::string base64_chars =
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz"
            "0123456789+/";

        std::string encoded;
        encoded.reserve(((data.size() + 2) / 3) * 4);

        for (size_t i = 0; i < data.size(); i += 3) {
            uint32_t value = (data[i] << 16) | ((i + 1 < data.size()) ? (data[i + 1] << 8) : 0) | ((i + 2 < data.size()) ? data[i + 2] : 0);
            encoded.push_back(base64_chars[(value >> 18) & 0x3f]);
            encoded.push_back(base64_chars[(value >> 12) & 0x3f]);
            if (i + 1 < data.size()) {
                encoded.push_back(base64_chars[(value >> 6) & 0x3f]);
            }
            if (i + 2 < data.size()) {
                encoded.push_back(base64_chars[value & 0x3f]);
            }
        }

        while (encoded.size() % 4 != 0) {
            encoded.push_back('=');
        }

        return encoded;
    }

    inline std::string base64_encode(const std::string &in) {
        static const std::string base64_chars =
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz"
            "0123456789+/";

        std::string out;

        int val = 0, valb = -6;
        for (unsigned char c : in) {
            val = (val << 8) + c;
            valb += 8;
            while (valb >= 0) {
                out.push_back(base64_chars[(val >> valb) & 0x3F]);
                valb -= 6;
            }
        }
        if (valb > -6) {
            out.push_back(base64_chars[((val << 8) >> (valb + 8)) & 0x3F]);
        }
        while (out.size() % 4 != 0) {
            out.push_back('=');
        }
        return out;
    }
    void pushback_bmpheader(const BITMAPFILEHEADER *pbmph) {
        const uint8_t* dbegin = reinterpret_cast<const uint8_t*>(pbmph);
        const uint8_t* dend = dbegin + sizeof(BITMAPFILEHEADER);
        //append multiple bytes
        data.insert(data.end(), dbegin, dend);
    }
    void pushback_bmpheader(const BITMAPINFOHEADER *pbmpi) {
        const uint8_t* dbegin = reinterpret_cast<const uint8_t*>(pbmpi);
        const uint8_t* dend = dbegin + sizeof(BITMAPINFOHEADER);
        //append multiple bytes
        data.insert(data.end(), dbegin, dend);
    }

    static std::vector<char> &OneBppEncode( BITMAPINFOHEADER *bih, const std::vector<float> &srcpix) {
        static std::vector<char> encbuf;
        const float *data = srcpix.data();
        int dataSize = srcpix.size();

        int headlen =  sizeof(BITMAPINFOHEADER) + sizeof(BITMAPFILEHEADER) + 4*256;
        int iw = ( bih->biWidth +7) / 8;
        int padding = (iw % 4) ?  (4-(iw % 4)):0;
        int padw = iw + padding;
        int imageSize = padw * bih->biHeight;
        //int fileSize = 54 + 2 + imageSize;*/
        encbuf.resize( imageSize);
        char *encpix = encbuf.data();
        bih->biBitCount=1;
        bih->biCompression=0;
        for (int y = bih->biHeight-1; y >=0; y--) {
            const float *rowsrc = data + y*bih->biWidth;
            char *rowdst = encpix + padw * (bih->biHeight-1-y);
            for (uint32_t x=0; x < bih->biWidth; x++) {
                if ( rowsrc[x] > 0.9) {
                    rowdst[x / 8 ] |= 1 << (7-(x % 8));
                }
            }
            for ( int i; i< padding; i++) 
                rowdst[ iw+ i ] = 0;
        }
        return encbuf;
    }
};


int main(){
    std::vector<char> sv= { 'a','b','c','d','e','f','g',25};
 
    BmpRasterizer a(100,100);
    a.setRaw(sv);    
    std::cout<<a.toBase64()<<"\n";
    return 0;
}
