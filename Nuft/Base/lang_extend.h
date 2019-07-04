//
// Created by 王森 on 2019-06-27.
//

#include <map>
#include <vector>
#include <algorithm>
#include <string>

#ifndef NUFT_LANG_EXTEND_H
#define NUFT_LANG_EXTEND_H

#endif //NUFT_LANG_EXTEND_H

namespace Base{

    template <typename type1,typename type2>
    bool contains(const type1& tmp1, const type2& tmp2){

        return std::find(tmp1.begin(),tmp1.end(),tmp2)!=tmp1.end();
    }

    template <typename K_type,typename V_type>
    bool contains(const std::map<K_type,V_type>& M, const K_type& key ){

        typename std::map<K_type,V_type>::const_iterator iter = M.find(key);

        return iter!=M.end();
    }

    template <typename type>
    bool in(std::vector<type> V,const type& val){

        return std::find(V.begin(),V.end(),val)!=V.end();
    }

    inline void trim(std::string& line){
        auto val = line.find_first_not_of(" \n\r\t")+1;

        if(val==line.size()||val==std::string::npos){
            val = line.find_last_not_of(" \n\r\t");
            line = line.substr(val);
        }else{
            line.erase(val);
        }
    }

    struct defer{

    public:
                ~defer(){

                    while (!fstack.empty()){
                        fstack.back()();
                        fstack.pop_back();
                    }
                }

                void push(std::function<void ()>func){
                    fstack.push_back(func);
                }

    protected:
                std::vector<std::function<void ()>>fstack;
            };

}