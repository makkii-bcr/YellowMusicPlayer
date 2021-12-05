#ifndef YMP_MSG_H
#define YMP_MSG_H

#include <windows.h>

struct PostMsgData {
    PostMsgData(DWORD _id) : id(_id) {};
    DWORD id;
};

struct PostMsgData2 : public PostMsgData {
    PostMsgData2(DWORD _id, TCHAR* _text) : PostMsgData(_id) {
        text2 = _text;
    };
    TCHAR text2[1000];
};

#endif
