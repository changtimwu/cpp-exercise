#include <iostream>
#include <vector>

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


static int base64_encode_internal( const std::vector<uint8_t> &srcbuf, std::string &dstbuf)
{
    static const unsigned char base64_table[65] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    size_t olen = 4*(( srcbuf.size() + 2) / 3); /* 3-byte blocks to 4-byte */
    dstbuf.resize(olen);
    int spos=0;
    auto dp = dstbuf.begin();
    for( ; spos <= srcbuf.size()-3; spos+=3) {
        const uint8_t *in = srcbuf.data() + spos;
        //std::vector<uint8_t>::const_iterator in = srcbuf.begin() + spos;
        *dp++ = base64_table[ in[0] >> 2];
        *dp++ = base64_table[((in[0] & 0x03) << 4) | (in[1] >> 4)];
        *dp++ = base64_table[((in[1] & 0x0f) << 2) | (in[2] >> 6)];
        *dp++ = base64_table[in[2] & 0x3f];
    }
    if (spos < srcbuf.size()) {
        const uint8_t *in = srcbuf.data() + spos;
        //std::vector<uint8_t>::const_iterator in = srcbuf.begin() + spos;
    
        *dp++ = base64_table[in[0] >> 2];
        if (spos == (srcbuf.size()- 1)) {
            *dp++ = base64_table[(in[0] & 0x03) << 4];
            *dp++ = '=';
        }
        else {
            *dp++ = base64_table[((in[0] & 0x03) << 4) | (in[1] >> 4)];
            *dp++ = base64_table[(in[1] & 0x0f) << 2];
        }
        *dp++ = '=';
    }
    return 0;
}

int base64_encode( const std::vector<uint8_t> &srcbuf, std::string &dstbuf)
{
     return base64_encode_internal( srcbuf, dstbuf);
}

std::string base64_encode( const std::vector<uint8_t> &srcbuf)
{
    std::string dstr;
    base64_encode_internal( srcbuf, dstr);
    return dstr;
}

std::string& base64_encode1( const std::vector<uint8_t> &srcbuf)
{
    static std::string rstr;
    base64_encode_internal( srcbuf, rstr);
    return rstr;
}

std::string& tenc_bmp8b( int width, int height, const std::vector<uint8_t> &inbuf)
{
    //auto bmpbuf = encode_bmp8b( w, h, inbuf);
    std::vector<uint8_t> bmpbuf{ 'a','b','c','d','e','f','g',25};
    return base64_encode1( bmpbuf);
}

std::string& tenc_bmp1b( int width, int height, const std::vector<float> &inbuf)
{
    std::vector<uint8_t> bmpbuf{ 'a','b','c','d','e','f','g',25};
    
    return base64_encode1( bmpbuf);
}

static void testb64() {
    std::vector<uint8_t> sbuf { 'a','b','c','d','e','f','g',25};
    std::vector<uint8_t> sbuf2 { 'b','c','e','e' };
    
    std::string dstr;
    base64_encode( sbuf, dstr);
    std::cout<<"b64="<<dstr<<std::endl;
    for (int i=0;i<20;i++) {
        std::cout<<"b64="<<  base64_encode1( sbuf) <<std::endl;
        std::cout<<"b64="<<  base64_encode1( sbuf2) <<std::endl;
    }
}


int main() {
  int w=10,h=10;
  std::vector<float> buf(w*h);
  /*
  buf[0] = 14.55;
  auto &r = tenc( w, h, buf);
  std::cout<< r << std::endl;
*/
    
  for (int i=0;i< buf.size(); i++) {
    buf[i] = 0.1*i+0.01;
  }
  auto benc = new BMPEncode( w,h, buf);
  xx(benc->b64str());
  delete benc;

  auto &r = tenc_bmp1b( w,h, buf);
  std::cout<< r << std::endl;
  testb64();
  return 0;
}
 
