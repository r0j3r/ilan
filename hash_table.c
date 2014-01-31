
#define hashsize(n) ((uint32_t)1<<(n))
#define hashmask(n) (hashsize(n)-1)
#define rot(x,k) (((x)<<(k)) | ((x)>>(32-(k))))

#define mix(a,b,c) \
{ \
    a -= c; a ^= rot(c, 4); c += b; \
    b -= a; b ^= rot(a, 6); a += c; \
    c -= b; c ^= rot(b, 8); b += a; \
    a -= c; a ^= rot(c,16); c += b; \
    b -= a; b ^= rot(a,19); a += c; \
    c -= b; c ^= rot(b, 4); b += a; \ 
}

#define final(a,b,c) \
{ \
    c ^= b; c -= rot(b,14); \
    a ^= c; a -= rot(c,11); \
    b ^= a; b -= rot(a,25); \
    c ^= b; c -= rot(b,16); \
    a ^= c; a -= rot(c,4);  \
    b ^= a; b -= rot(a,14); \
    c ^= b; c -= rot(b,24); \
}

//32 bit fast hash  by bob jenkins
uint32_t
hashword(const uint32_t *k, size_t length, uint32_t initval)
{
    uint32_t a,b,c;
    
    a = b = c = 0xdeadbeef + (((uint32_t)length)<<2) + initval;
    while(length > 3)
    {
        a += k[0];
        b += k[1];
        c += k[2];
        mix(a,b,c);
        length -= 3;
        k += 3;
    }
    switch(length)
    {
    case 3: c += k[2]; //fallthru
    case 2: b += k[1]; //fallthru
    case 1: a += k[0]; //fallthru
        final(a,b,c);
    case 0:
        break;
    }
    return c;  
}
