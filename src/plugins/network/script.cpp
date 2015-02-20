#include "script.hpp"
#include "gethostaddress.h"
    
#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
 #include <sys/types.h>
 #include <sys/socket.h>
 #include <ifaddrs.h>
#endif

namespace NetPlug{

void SocketFinalizer(JSFreeOp *fop, JSObject *obj){
    
    struct WSocket *sock = socket_proto.unsafeUnwrap(obj);
    
    if(State_Socket(sock)==0)
        Disconnect_Socket(sock);

   Destroy_Socket(sock);
}

Turbo::JSPrototype<struct WSocket> socket_proto("Socket", nullptr, 0, SocketFinalizer);

bool GetLocalName(JSContext *ctx, unsigned argc, JS::Value *vp){
    char name[2048];
    
    gethostname(name, 2048);
    name[2047] = '\0';
    
    CallArgsFromVp(argc, vp).rval().set(STRING_TO_JSVAL(JS_NewStringCopyZ(ctx, name)));
    return true;
}

bool GetLocalAddress(JSContext *ctx, unsigned argc, JS::Value *vp){
    const char *addr = TS_GetHostAddressIP4();
    if(!addr)
        addr = TS_GetHostAddressIP6();
    if(!addr)
        addr = "";
    
    CallArgsFromVp(argc, vp).rval().set(STRING_TO_JSVAL(JS_NewStringCopyZ(ctx, addr)));
    return true;
}

bool OpenAddress(JSContext *ctx, unsigned argc, JS::Value *vp){
    
    const Turbo::JSType signature[] = {Turbo::String, Turbo::Number};
    
    JS::CallArgs args = CallArgsFromVp(argc, vp);
    
    if(!Turbo::CheckSignature<2>(ctx, args, signature, __func__))
        return false;
    
    struct WSocket *socket = Create_Socket();
    struct Turbo::JSStringHolder<> address(ctx, JS_EncodeString(ctx, args[0].toString()));
    
    enum WSockErr err = Connect_Socket(socket, address.string, args[1].toNumber(), -1);
    if(err!=0){
        Turbo::SetError(ctx, std::string(BRACKNAME " OpenAddress Error ") + ExplainError_Socket(err));
        return false;
    }
    
    args.rval().set(OBJECT_TO_JSVAL(socket_proto.wrap(ctx, socket)));
    
    return true;
}

bool ListenOnPort(JSContext *ctx, unsigned argc, JS::Value *vp){
    JS::CallArgs args = CallArgsFromVp(argc, vp);
    if(!Turbo::CheckForSingleArg(ctx, args, Turbo::Number, __func__))
        return false;
    
    struct WSocket *socket = Create_Socket();
    enum WSockErr err = Listen_Socket(socket, args[0].toNumber());
    
    if(err!=0){
        Turbo::SetError(ctx, std::string(BRACKNAME " ListenOnPort Error ") + ExplainError_Socket(err));
        return false;
    }
    
    args.rval().set(OBJECT_TO_JSVAL(socket_proto.wrap(ctx, socket)));
    
    return true;
}

bool SocketIsConnected(JSContext *ctx, unsigned argc, JS::Value *vp){
    JS::CallArgs args = CallArgsFromVp(argc, vp);
    args.rval().set(BOOLEAN_TO_JSVAL((State_Socket(socket_proto.getSelf(ctx, vp, &args))==0)));
    return true;
}

bool SocketGetPendingReadSize(JSContext *ctx, unsigned argc, JS::Value *vp){
    JS::CallArgs args = CallArgsFromVp(argc, vp);
    args.rval().set(JS_NumberValue((Length_Socket(socket_proto.getSelf(ctx, vp, &args))==0)));
    return true;
}

bool SocketWrite(JSContext *ctx, unsigned argc, JS::Value *vp){
    JS::CallArgs args = CallArgsFromVp(argc, vp);
    if(!Turbo::CheckForSingleArg(ctx, args, Turbo::ArrayBuffer, __func__))
        return false;
        
    JS::RootedObject buffer_root(ctx, args[0].toObjectOrNull());
    
    void *data;
    uint32_t len;
    
    js::GetArrayBufferLengthAndData(buffer_root, &len, (unsigned char **)(&data));
    
    enum WSockErr err = Write_Socket_Len(socket_proto.getSelf(ctx, vp, &args), (const char *)data, len);
    if(err!=0){
        Turbo::SetError(ctx, std::string(BRACKNAME " SocketWrite Error ") + ExplainError_Socket(err));
        return false;
    }
    
    return true;
}

bool SocketRead(JSContext *ctx, unsigned argc, JS::Value *vp){
    JS::CallArgs args = CallArgsFromVp(argc, vp);
    struct WSocket *mSocket = socket_proto.getSelf(ctx, vp, &args);
    
    char *data = NULL;
    enum WSockErr err = Read_Socket(mSocket, &data);
        if(err!=0){
        Turbo::SetError(ctx, std::string(BRACKNAME " SocketRead Error ") + ExplainError_Socket(err));
        return false;
    }
    
    JSObject *buffer = JS_NewArrayBufferWithContents(ctx, strlen(data), data);
    
    args.rval().set(OBJECT_TO_JSVAL(buffer));
    return true;
}

bool SocketClose(JSContext *ctx, unsigned argc, JS::Value *vp){
    JS::CallArgs args = CallArgsFromVp(argc, vp);
    struct WSocket *mSocket = socket_proto.getSelf(ctx, vp, &args);
    if(State_Socket(mSocket)==0)
        Disconnect_Socket(mSocket);
        
    return true;
}

    
}
