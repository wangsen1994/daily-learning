//
// Created by 王森 on 2019-06-27.
//

#include <cstddef>
#include <cstring>
#include <cstdarg>
#include <cstdio>
#include <string>
#include <vector>
#include <algorithm>

#ifndef NUFT_STIRING_UTILS_H
#define NUFT_STIRING_UTILS_H

#endif //NUFT_STIRING_UTILS_H


namespace Base{

    inline int asprintf(char** ptr, const char* fmt,...){

        size_t fmt_len = std::strlen(fmt);
        int size = fmt_len*2+50;
        *ptr = new char[size];

        va_list ap;

        while (1){
            va_start(ap,fmt);
            int n = vsnprintf(*ptr,size,fmt,ap);
            va_end(ap);

            if(n>-1&&n<size){
                return n;
            }else{
                delete []*ptr;
                size*=2;
                *ptr = new char[size];
            }
        }

        return -1;

    }

    inline std::vector<std::string> split(const std::string& s,const std::string& splitter){

        std::vector<std::string> res;
        size_t start = 0;
        size_t p;

        while ((p=s.find(splitter,start))!=std::string::npos) { /* npos 用来表示不存在的位置 */
            res.push_back(std::string(s.begin()+start,s.begin()+p));
            start = p+splitter.size();
        }

        res.push_back(std::string(s.begin()+start,s.end()));

        if(res.size()==1&&res[0]==""){return {};}

        return res;
    }

    template <typename Iter>
    inline std::string join(Iter b,Iter e, const std::string& combine){

        return accumulate(b,e,std::string(""),[&](const auto& x, const auto& y){return x==""?y:x+combine+y;})
    }

    template <typename Iter,typename I>
    inline std::string join(Iter b,Iter e, const std::string& combine,I i){
        std::string s;

        for(auto i=b;i!=e;i++){

            if(i!=b){s+=combine;}

            s+=I(*i);
        }

        return s;
    }
}