//
// Created by 王森 on 2019-06-25.
//
#include <functional>
#include <cstdarg>

#ifndef NUFT_LOG_H
#define NUFT_LOG_H

#endif //NUFT_LOG_H


namespace Base{


    struct BaseLogger{

        static auto get_new_fmt(){

            static char new_fmt[1024];

            return new_fmt;
        }

        BaseLogger(){ }

        void Dolog(BaseLogger& context, char const* filename, char const* funcname,int line,
                int level,char const* fmt,va_list va){

            snprintf(BaseLogger::get_new_fmt(),1024,"%s,%d(In function %s):LOG[%d] %s\n",
                    filename,line,funcname,level,fmt);
            vfprintf(stdout,BaseLogger::get_new_fmt(),va);

            fflush(stdout);
        }

        ~BaseLogger(){}
    };

}

template <typename Context>
struct _log_struct{
    typedef std::function<void (Context&, char const*, va_list)>call_type;

    operator call_type()const {
        return [&](Context& context, char const* fmt,va_list va){
            context.Dolog(context,filename,funcname,line,level,fmt,va);
        };
    }

    _log_struct(const char* filename_, const char* funcnanme_,int line_,int level_)
    :filename(filename_),funcname(filename_),line(line_),level(level_){}

    const char * filename;
    const char * funcname;
    int line;
    int level;
};

template <typename Context>
void _log(_log_struct<Context> func,Context context, char const * fmt,...){
    va_list va;
    va_start(va,fmt);
    ((typename _log_struct<Context>::call_type)func)(context, fmt, va);
    va_end(va);
}

#define Nuke_LOG(LOGLEVEL, context, fmt, ...) Base::_log(Base::_log_struct<decltype(context)>{__FILE__, __FUNCTION__, __LINE__, LOGLEVEL}, context, fmt, ##__VA_ARGS__)
