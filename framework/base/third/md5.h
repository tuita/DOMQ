#ifndef _BASE_MD5_H
#define _BASE_MD5_H
#ifdef __cplusplus
extern "C"
{
#endif
#ifndef uint8
#define uint8  unsigned char
#endif

#ifndef uint32
#define uint32 unsigned long int
#endif
namespace base
{
    typedef struct
    {
        uint32 total[2];
        uint32 state[4];
        uint8 buffer[64];
    }
    md5_context;

    void md5_starts( md5_context *ctx );
    void md5_update( md5_context *ctx, uint8 *input, uint32 length );
    void md5_finish( md5_context *ctx, uint8 digest[16] );
    char *md5_string(const unsigned char *in, char *out, uint32 in_len);
}

#ifdef __cplusplus
}
#endif

namespace base
{

// encapsulate md5 for cpp programmer using easily
class MD5
{
public:
	void Init();	
	void Update(const char * pBuffer, int iLen);	
	const char * GetCheckSum();
 
private:
	md5_context context_;
	char md5str_[32+1];
	bool finished_;
};

}

#endif

