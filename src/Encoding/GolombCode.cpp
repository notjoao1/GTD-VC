//
// Created by Kikom on 01/10/2023.
//

#include "GolombCode.h"
#include <cmath>
#include <iostream>
#include <cmath>

#define LOG2(X) ((unsigned) (8*sizeof (unsigned long long) - __builtin_clzll((X)) - 1))

std::vector<bool> GolombCode::encode(int n, int m) {

    std::vector<bool> ret;

    int q = n / m;
    int r = n-q*m; // r = n % m;

    ret.reserve(q);
    for(int i=0; i < q;i++)
        ret.push_back(true);
    ret.push_back(false);

    auto b = LOG2(m);

    std::vector<bool> temp;
    if ( r < (  (1 << (b+1)) - m ) ){
        temp.reserve(b);
        ret.reserve(b);
        for(; b>0 ; b--){
            if(  r % 2 == 0 )
                temp.push_back(false);
            else
                temp.push_back(true);
            r >>= 1;
        }
        for(auto elem = temp.rbegin() ; elem < temp.rend(); elem++){
            ret.push_back(*elem);
        }
    } else {
        temp.reserve(b+1);
        ret.reserve(b+1);
        r += ( 1 << (b+1) ) - m;
        for(b +=1 ; b>0 ; b--){
            if(  r % 2 == 0 )
                temp.push_back(false);
            else
                temp.push_back(true);
            r >>= 1;
        }
        for(auto elem = temp.rbegin() ; elem < temp.rend(); elem++){
            ret.push_back(*elem);
        }
    }

    return ret;
}

int GolombCode::decode_one(int m,std::vector<bool> i) {
    int q= 0,r_ = 0;
    auto b = LOG2(m);
    auto bit =i.begin();
    for( ; *bit; ++bit )q++;
    for(int k=0 ; k < b ; k++ ) r_ = r_*2 + *(++bit);
    return q*m +( (r_ < (1<<(b+1)) - m) ? r_: r_*2 + *(++bit) - (1<<(b+1)) + m );
}

std::vector<int> GolombCode::decode(int m, const std::vector<bool>& stream, int n) {
    std::vector<int> res;
    res.reserve(n);
    for(int i = 0; i < n ; i++)
        res.push_back(GolombCode::decode_one(m,stream));
    return res;
}


int GolombCode::decodeFrom64bits(unsigned long long i, int m) {
    // if encoded number isn't bigger than 64 bits, and it's contained in i
    // this is very fast , but limited
    auto q =  (unsigned) __builtin_clzll( (~i) );
    auto b = LOG2(m);
    i<<= q+1;
    auto r = (i >> (64-b));
    i <<= b;
    return q*m +( int(r < (1<<(b+1)) - m) ? r: (r<<1) + (i>>63) - (1<<(b+1)) + m );
}

void GolombCode::encode(unsigned int n, int m, BitStreamWrite &stream) {

    unsigned int q = n / m;
    unsigned int r = n-q*m;

    while(q >= 64){
        stream.write(64,0xFFFFFFFFFFFFFFFF);
        q-=64;
    }

    stream.write((int) q+1,0xFFFFFFFFFFFFFFFE);

    int b = LOG2(m);

    //std::cout << " [m]" << " - " << m << " [n]" << " - " << n << " [q]" << " - " << n / m << " [r]" << " - " << r << " [b]" << " - " << b << std::endl;

    if ( int(r - (1 << (b+1)) + m) < 0  ){
        stream.write(b,r);
    }
    else {
        stream.write(b+1,r + (1 << (b+1)) - m);
    }
}

void GolombCode::encode(std::vector<unsigned int> v, int m, BitStreamWrite &b) {
    for(auto i = v.begin() ; i < v.end() ; i++ )
        GolombCode::encode(*i,m,b);
}

unsigned int GolombCode::decode_one(int m, BitStreamRead &stream) {
    int q = 0;
    auto i = stream.read(64);
    while(i == 0xFFFFFFFFFFFFFFFF){
        q+=64;
        i = stream.read(64);
    }
    int temp =  __builtin_clzll( (~i) );
    q += temp;

    int b = LOG2(m);

    bool push = true;
    if(62-temp-b<0) {
        i = (i << (temp + b - 62)) | stream.read(temp + b - 62);
        i<<= 63-b;
        push = false;
    } else {
        i<<= q+1;
    }

    auto r = (i >> (64-b));
    i <<= b;
    if(int(r < (1<<(b+1)) - m)){
        if(push)
            stream.back_front(63-temp-b);
        else
            stream.back_front(1);
        return q*m+r;
    } else {
        if(push)
            stream.back_front(62-temp-b);
        return q*m+(r<<1) + (i>>63) - (1<<(b+1)) + m;
    }
}

std::vector<unsigned int> GolombCode::decode(int m,BitStreamRead &b, int n) {
    std::vector<unsigned int> res;
    res.reserve(n);
    for(int i = 0; i < n ; i++)
        res.push_back(GolombCode::decode_one(m,b));
    return res;
}

int GolombCode::estimate(const cv::Mat &m) {
    int c = 0;
    for(int i = 0; i < m.rows; i++)
        for (int j = 0; j < m.cols; j++){
            if( m.at<unsigned int>(i, j) == 0 )
                c++;
        }

    float d = float(c) / float( m.rows * m.cols );
    int m_param = std::ceil( -1 / log2f(d) );
    return m_param;
}

int GolombCode::estimate(const unsigned int* m,int siz) {
    //int hist[512];
    //for(int & i : hist)
    //    i =0;
    int c = 0;
    for(int i = 0; i < siz; i++){
        if( m[i]== 0 )
            c++;
        //hist[m[i]]++;
    }

    //for(int i = 0 ; i < 512 ; i++){
    //    std::cout << "[" << i << "]" << " - " << hist[i] << std::endl;
    //}

    float d = float(c) / float( siz );
    //std::cout << "[d]" << " - " << d << std::endl;
    int m_param = std::ceil( -1 / log2f(1-d) );
    //std::cout << "[m]" << " - " << m_param << std::endl;
    return m_param;
}

unsigned int GolombCode::mapIntToUInt(int n) {
    if ( n < 0)
        return -n*2-1;
    return 2 * n;
}

int GolombCode::mapUIntToInt(unsigned int n) {
    if( n % 2 == 1)
        return -(n+1)/2;
    return n/2 ;
}
































