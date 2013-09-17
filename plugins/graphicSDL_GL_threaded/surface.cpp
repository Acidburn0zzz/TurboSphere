#include "main.h"
#include "surface.h"
#include "color.h"
#include "primitives.h"
#include "save.h"
#include <functional>
#include <assert.h>

    SDL_mutex *SurfaceQueueMutex            = NULL;
    SDL_mutex *SurfaceQueueNeedMutex        = NULL;
    SDL_mutex *SurfaceQueueIndependantMutex = NULL;

    size_t QueueOperatingPosition = 0;
    size_t QueuePlacingPosition   = 1;

v8Function SurfaceRectangle (V8ARGS);
v8Function SurfaceLine (V8ARGS);
v8Function SurfaceOutlinedCircle (V8ARGS);
v8Function SurfaceFilledCircle (V8ARGS);
v8Function SurfaceGradientRectangle (V8ARGS);

DECLARE_OBJECT_TEMPLATES(Surface);

void SurfaceInit(void){

    SurfaceQueueMutex     = SDL_CreateMutex();
    SurfaceQueueNeedMutex = SDL_CreateMutex();
    SurfaceQueueIndependantMutex = SDL_CreateMutex();

    SurfaceQueue = (TS_GenericSurfaceFunction**)calloc(sizeof(TS_GenericSurfaceFunction*), 0xFFFF);

    QueueOperatingPosition = 0;
    QueuePlacingPosition   = 1;

    INIT_OBJECT_TEMPLATES(Surface);
    SET_CLASS_NAME(Surface, "Surface");

    ADD_TO_PROTO(Surface, "blit",           SurfaceBlit);
    ADD_TO_PROTO(Surface, "blitSurface",    SurfaceBlitSurface);
    ADD_TO_PROTO(Surface, "createImage",    SurfaceToImage);
    ADD_TO_PROTO(Surface, "cloneSection",   SurfaceCloneSection);

    ADD_TO_PROTO(Surface, "save",           SurfaceSave);

    ADD_TO_PROTO(Surface, "setAlpha",       SurfaceSetAlpha);
    ADD_TO_PROTO(Surface, "getClippingRectangle", SurfaceGetClippingRectangle);
    ADD_TO_PROTO(Surface, "setClippingRectangle", SurfaceSetClippingRectangle);

    ADD_TO_PROTO(Surface, "rectangle",          SurfaceRectangle);
    ADD_TO_PROTO(Surface, "line",               SurfaceLine);
    ADD_TO_PROTO(Surface, "outlinedCircle",     SurfaceOutlinedCircle);
    ADD_TO_PROTO(Surface, "filledCircle",       SurfaceFilledCircle);
    ADD_TO_PROTO(Surface, "gradientRectangle",  SurfaceGradientRectangle);

    ADD_TO_PROTO(Surface, "setPixel",  SurfaceSetPixel);
    ADD_TO_PROTO(Surface, "getPixel",  SurfaceGetPixel);

    SET_CLASS_ACCESSOR(Surface, "width",    SurfaceGetWidth,    SurfaceSetWidth);
    SET_CLASS_ACCESSOR(Surface, "height",   SurfaceGetHeight,   SurfaceSetHeight);

    SDL_CreateThread(SurfaceThreadFunction, "TS_SurfaceRenderer", NULL);

}

void SurfaceClose(void){

}

v8Function SurfaceGetWidth(V8GETTERARGS) {
    int value = GET_ACCESSOR_SELF(SDL_Surface*)->w;
    return v8::Integer::New(value);
}

void SurfaceSetWidth(V8SETTERARGS) {
//    GET_SELF(SDL_Surface*)->w = value->Int32Value();
}

v8Function SurfaceGetHeight(V8GETTERARGS) {
    int value = GET_ACCESSOR_SELF(SDL_Surface*)->h;
    return v8::Integer::New(value);
}

void SurfaceSetHeight(V8SETTERARGS) {
//    GET_SELF(SDL_Surface*)->h = value->Int32Value();
}

//For use with throwaway textures (normally generated by blitting surfaces).
void TS_BlitTexture(TS_Texture texture, int x[4], int y[4]){




    const GLint   vertexData[]   = {x[0], y[0], x[1], y[1], x[2], y[2], x[3], y[3]};
    const GLint   texcoordData[] = {0, 0, 1, 0, 1, 1, 0, 1};
    const GLuint  colorData[]    = {
        fullmask->toInt(),
        fullmask->toInt(),
        fullmask->toInt(),
        fullmask->toInt()
    };
    glTexCoordPointer(2, GL_INT, 0, texcoordData);
    glVertexPointer(2, GL_INT, 0, vertexData);
    glColorPointer(4, GL_UNSIGNED_BYTE, 0, colorData);
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_TEXTURE_2D);

    glDeleteTextures(1, &texture);

}

TS_Texture TS_SurfaceToTexture(SDL_Surface *surface){

    assert(surface!=NULL);

    TS_Texture texture;

    glGenTextures(1, &texture);

    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    if(SDL_LockMutex(SurfaceQueueIndependantMutex)<0)
        exit(0x110);

    READY_SURFACE(surface);
    SDL_LockSurface(surface);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, surface->w, surface->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, surface->pixels);
    SDL_UnlockSurface(surface);

    if(SDL_UnlockMutex(SurfaceQueueIndependantMutex)<0)
        exit(0x111);

    return texture;
}

void TS_SurfaceFinalizer(V8FINALIZERARGS) {

    SDL_Surface* surf = (SDL_Surface*)parameter;
    SDL_FreeSurface(surf);
    object->Dispose();
}

//Replace Color (threaded)
void TS_SurfaceReplaceColor(void *arg);

//Replace Color Argument structure
typedef struct {SDL_Surface *surface; TS_Color *oldColor; TS_Color *newColor; } ReplaceColorArgs;

//Replace Color queueing instruction
void TS_QueueSurfaceReplaceColor(SDL_Surface *surface, TS_Color *oldColor, TS_Color *newColor){
    if(SDL_LockMutex(SurfaceQueueMutex)<0)
        exit(0x112);
    ReplaceColorArgs *arg = (ReplaceColorArgs*)malloc(sizeof(ReplaceColorArgs));
    arg->surface  = surface;
    arg->oldColor = oldColor;
    arg->newColor = newColor;

    SurfaceQueue[QueuePlacingPosition] = new TS_GenericSurfaceFunction(TS_SurfaceReplaceColor, (void *)arg, surface, NULL);
    QueuePlacingPosition++;
    QueuePlacingPosition%=SurfaceQueueSize;

    if(SDL_LockMutex(SurfaceQueueMutex)<0)
        exit(0x113);
}

//Replace Color payload
void TS_SurfaceReplaceColor(void *arg){
    ReplaceColorArgs* carg = (ReplaceColorArgs*)arg;
    SDL_Surface *surface = carg->surface;
    TS_Color *oldColor   = carg->oldColor;
    TS_Color *newColor   = carg->newColor;

    delete carg;

    SDL_LockSurface(surface);
    intptr_t start = (intptr_t)surface->pixels;
    intptr_t last  = start+((surface->w*surface->h)*sizeof(int));

    const int      oldred      = oldColor->red;
    const uint32_t oldcolorint = oldColor->toInt();
    const uint32_t newcolorint = newColor->toInt();

    while(start<last){
        start = (intptr_t)memchr((void *)start, oldred, last-start);
        if(start&0xFF){
            continue;
        }
        else{
            if(memcmp(&oldcolorint, (void *)start, sizeof(int))!=0){
                continue;
            }
            memcpy((void *)start, &newcolorint, sizeof(int));
        }
    }

    SDL_UnlockSurface(surface);
}

v8Function SurfaceToImage(V8ARGS){

    BEGIN_OBJECT_WRAP_CODE;

	SDL_Surface *surface = GET_SELF(SDL_Surface*);

    TS_Texture texture = TS_SurfaceToTexture(surface);

    const int w = surface->w;
    const int h = surface->h;


    TS_Image *image = new TS_Image(texture, w, h);



    END_OBJECT_WRAP_CODE(Image, image);

}

v8Function SurfaceClone(V8ARGS){
    BEGIN_OBJECT_WRAP_CODE;

    if(SDL_LockMutex(SurfaceQueueIndependantMutex)<0)
        exit(0x116);

	SDL_Surface *srcsurface = GET_SELF(SDL_Surface*);

    READY_SURFACE(srcsurface);

    SDL_Surface *surface = SDL_CreateRGBSurface(0, srcsurface->w, srcsurface->h, DEPTH, CHANNEL_MASKS);

    SDL_BlitSurface(srcsurface, NULL, surface, NULL);

    if(SDL_UnlockMutex(SurfaceQueueIndependantMutex)<0)
        exit(0x117);

    SDL_SetSurfaceRLE(surface, 1);

    END_OBJECT_WRAP_CODE(Surface, surface)

}

v8Function SurfaceCloneSection(V8ARGS){

    if(args.Length()<4){
        THROWERROR("[" PLUGINNAME "] SurfaceCloneSection Error: Called with fewer than 4 arguments.");
    }

    CHECK_ARG_INT(0);
    CHECK_ARG_INT(1);
    CHECK_ARG_INT(2);
    CHECK_ARG_INT(3);

    BEGIN_OBJECT_WRAP_CODE;

    int x = args[0]->Int32Value();
    int y = args[1]->Int32Value();
    int w = args[2]->Int32Value();
    int h = args[3]->Int32Value();

    if(w<0){
        x+=w;
        w=-w;
    }
    if(h<0){
        y+=h;
        h=-h;
    }

	SDL_Surface *srcsurface = GET_SELF(SDL_Surface*);

    if(SDL_LockMutex(SurfaceQueueIndependantMutex)<0)
        exit(0x116);

    READY_SURFACE(srcsurface);

    MINMEMALIGN SDL_Surface *surface = SDL_CreateRGBSurface(0, w, h, DEPTH, CHANNEL_MASKS);

    SDL_Rect rect = {(short int)x, (short int)y, (unsigned short int)w, (unsigned short int)h};

    SDL_BlitSurface(srcsurface, &rect, surface, NULL);

    if(SDL_UnlockMutex(SurfaceQueueIndependantMutex)<0)
        exit(0x117);

    SDL_SetSurfaceRLE(surface, 1);

    END_OBJECT_WRAP_CODE(Surface, surface)

}

v8Function SurfaceGrab(V8ARGS){
    if(args.Length()<4){
        THROWERROR("[" PLUGINNAME "] SurfaceGrab Error: Called with fewer than 4 arguments.");
    }
    CHECK_ARG_INT(0);
    CHECK_ARG_INT(1);
    CHECK_ARG_INT(2);
    CHECK_ARG_INT(3);

    BEGIN_OBJECT_WRAP_CODE;
    int x = args[0]->Int32Value();
    int y = args[1]->Int32Value();
    int w = args[2]->Int32Value();
    int h = args[3]->Int32Value();

    if(w<0){
        x+=w;
        w=-w;
    }
    if(h<0){
        y+=h;
        h=-h;
    }

    TS_Image *image = TS_ImageGrab(x, y, w, h);

    SDL_Surface *surface = image->CreateSurface();
    SDL_SetSurfaceRLE(surface, 1);
    delete image;

    END_OBJECT_WRAP_CODE(Surface, surface);
}

v8Function SurfaceSave(V8ARGS){

    if(args.Length()<1){
        THROWERROR("[" PLUGINNAME "] SurfaceSave Error: Called with no arguments.");
    }
    CHECK_ARG_STR(0);

    TS_Directories *TS_dirs = GetDirs();

    if(SDL_LockMutex(SurfaceQueueIndependantMutex)<0)
        exit(0x116);

    SDL_Surface *surface = GET_SELF(SDL_Surface*);

    READY_SURFACE(surface);

    v8::String::Utf8Value str(args[0]);
    const char *filename = string(TS_dirs->image).append(*str).c_str();

    size_t len = strlen(filename);
    if(strstr(filename, ".png")==filename+len-4){
        printf("\n\nSaving PNG to file %s.\n\n", filename);
        save_PNG(filename, surface->pixels, surface->w, surface->h, 0);
        if(SDL_UnlockMutex(SurfaceQueueIndependantMutex)<0)
            exit(0x116);
        return v8::Undefined();
    }

    else if(strstr(filename, ".tga")==filename+len-4){
        printf("\n\nSaving TGA to file %s.\n\n", filename);
        save_TGA(filename, surface->pixels, surface->w, surface->h, R8G8B8A8, SDL_GL_SAVETGA_COMPRESS);
        if(SDL_UnlockMutex(SurfaceQueueIndependantMutex)<0)
            exit(0x116);
        return v8::Undefined();
    }
    if(SDL_SaveBMP(surface, filename)!=0){
        printf("[" PLUGINNAME "] SurfaceSave Error: Could not save image %s: %s\n", *str, SDL_GetError());
        SDL_ClearError();

        if(SDL_UnlockMutex(SurfaceQueueIndependantMutex)<0)
            exit(0x116);

        THROWERROR("[" PLUGINNAME "] SurfaceSave Error: Could not save image.");
    }

    if(SDL_UnlockMutex(SurfaceQueueIndependantMutex)<0)
        exit(0x116);
    return v8::Undefined();
}

v8Function NewSurface(V8ARGS){

    BEGIN_OBJECT_WRAP_CODE;
    SDL_Surface *surface = NULL;

    if(args.Length()==1){
        CHECK_ARG_STR(0);

        TS_Directories *TS_dirs = GetDirs();

        v8::String::Utf8Value str(args[0]);
        const char *filename = strdup(string(TS_dirs->image).append(*str).c_str());
        surface = IMG_Load(filename);
        SDL_SetSurfaceBlendMode(surface, SDL_BLENDMODE_NONE);
        if(!surface){
            THROWERROR((std::string("Error: Could not load surface from file ")+std::string(filename)).c_str());
        }
        else{
            SDL_SetSurfaceRLE(surface, 1);
            END_OBJECT_WRAP_CODE(Surface, surface);
        }
    }
    else if(args.Length()==3){
        CHECK_ARG_INT(0);
        CHECK_ARG_INT(1);
        CHECK_ARG_OBJ(2);

        uint32_t width  = args[0]->Uint32Value();
        uint32_t height = args[1]->Uint32Value();

        surface = SDL_CreateRGBSurface(0, width, height, DEPTH, CHANNEL_MASKS);
        SDL_SetSurfaceBlendMode(surface, SDL_BLENDMODE_NONE);
        v8::Local<v8::Object> colorobj = v8::Local<v8::Object>::Cast(args[2]);
        TS_Color *color = (TS_Color*)colorobj->GetAlignedPointerFromInternalField(0);

        SDL_FillRect(surface, NULL, color->toInt());
        SDL_SetSurfaceRLE(surface, 1);

        END_OBJECT_WRAP_CODE(Surface, surface);

    }
    else{
        THROWERROR("[" PLUGINNAME "] NewSurface Error: Requires either 1 or 3 arguments.")
    }
}

v8Function SurfaceSetAlpha(V8ARGS){
    if(args.Length()<1){
        THROWERROR("[" PLUGINNAME "] SurfaceSetAlpha Error: Called with fewer than 2 arguments.");
    }
    CHECK_ARG_INT(0);

    int rawalpha = args[0]->Int32Value();
    if(rawalpha<0)      rawalpha = 0;
    if(rawalpha>255)    rawalpha = 255;
    uint8_t alpha = rawalpha;

    if(SDL_LockMutex(SurfaceQueueIndependantMutex)<0)
        exit(0x116);

    SDL_Surface *surface = GET_SELF(SDL_Surface*);

    READY_SURFACE(surface);

    if(SDL_SetSurfaceAlphaMod(surface, alpha)!=0){
        printf("[" PLUGINNAME "] SurfaceSetAlpha Error: Could not set alpha on surface: %s", SDL_GetError());
        SDL_ClearError();
        if(SDL_UnlockMutex(SurfaceQueueIndependantMutex)<0)
            exit(0x116);
        THROWERROR("[" PLUGINNAME "] SurfaceSetAlpha Error: Could not set alpha.");
    }
    if(SDL_UnlockMutex(SurfaceQueueIndependantMutex)<0)
        exit(0x116);
    return v8::Undefined();

}

v8Function SurfaceSetClippingRectangle(V8ARGS){
    if(args.Length()<4){
        THROWERROR("[" PLUGINNAME "] SurfaceSetClippingRectangle Error: Called with fewer than 5 arguments.");
    }
    CHECK_ARG_INT(0);
    CHECK_ARG_INT(1);
    CHECK_ARG_INT(2);
    CHECK_ARG_INT(3);

	int x = args[0]->Int32Value();
	int y = args[1]->Int32Value();
	int w = args[2]->Int32Value();
	int h = args[3]->Int32Value();

    if(w<0){
        x+=w;
        w=-w;
    }
    if(h<0){
        y+=h;
        h=-h;
    }

    //TODO: This doesn't NEED to be immediate, make this queueable for the surface drawing thread.

    if(SDL_LockMutex(SurfaceQueueIndependantMutex)<0)
        exit(0x116);

    SDL_Surface *surface = GET_SELF(SDL_Surface*);

    READY_SURFACE(surface);

    SDL_Rect rect = {(short int)x, (short int)y, (unsigned short int)w, (unsigned short int)h};

    SDL_SetClipRect(surface, &rect);

    if(SDL_UnlockMutex(SurfaceQueueIndependantMutex)<0)
        exit(0x116);

	return v8::Undefined();
}

v8Function SurfaceGetClippingRectangle(V8ARGS){

    v8::HandleScope GetClipRectscope;
    v8::Local<v8::Object> cliprect = v8::Object::New();

    SDL_Surface *surface = GET_SELF(SDL_Surface*);

    SDL_Rect rect = {0, 0, 0, 0};
    SDL_GetClipRect(surface, &rect);

    cliprect->Set(v8::String::New("x"), v8::Integer::New(rect.x));
    cliprect->Set(v8::String::New("y"), v8::Integer::New(rect.y));
    cliprect->Set(v8::String::New("w"), v8::Integer::New(rect.w));
    cliprect->Set(v8::String::New("h"), v8::Integer::New(rect.h));

	return GetClipRectscope.Close(cliprect);

}

v8Function SurfaceGetPixel(V8ARGS){
    if(args.Length()<2){
        THROWERROR("[" PLUGINNAME "] SurfaceGetPixel Error: Called with fewer than 2 arguments.");
    }
    CHECK_ARG_INT(0);
    CHECK_ARG_INT(1);

    BEGIN_OBJECT_WRAP_CODE;

    int x = args[0]->Int32Value();
    int y = args[1]->Int32Value();

    if(SDL_LockMutex(SurfaceQueueIndependantMutex)<0)
        exit(0x146);

    SDL_Surface *surface = GET_SELF(SDL_Surface*);

    READY_SURFACE(surface);

    int *pixels = static_cast<int*>(GET_SELF(SDL_Surface*)->pixels);

    SDL_LockSurface(surface);
    int color   = pixels[x+((surface->w)*y)];
    SDL_UnlockSurface(surface);

    if(SDL_UnlockMutex(SurfaceQueueIndependantMutex)<0)
        exit(0x147);

    TS_Color *colorobj = new TS_Color((color&(Frmask)), (color&(Fgmask))>>8, (color&(Fbmask))>>16, (color&(Famask))>>24);

    END_OBJECT_WRAP_CODE(Color, colorobj);
}

v8Function SurfaceSetPixel(V8ARGS){
    if(args.Length()<3){
        THROWERROR("[" PLUGINNAME "] SurfaceSetPixel Error: Called with fewer than 3 arguments.");
    }
    CHECK_ARG_INT(0);
    CHECK_ARG_INT(1);
    CHECK_ARG_OBJ(2);

    int x = args[0]->Int32Value();
    int y = args[1]->Int32Value();

    //TODO: This doesn't NEED to be immediate, make this queueable for the surface drawing thread.

    if(SDL_LockMutex(SurfaceQueueIndependantMutex)<0)
        exit(0x176);

    SDL_Surface *surface = GET_SELF(SDL_Surface*);

    READY_SURFACE(surface);

    int *pixels = static_cast<int*>(GET_SELF(SDL_Surface*)->pixels);

    v8::Local<v8::Object> colorobj = v8::Local<v8::Object>::Cast(args[2]);
    TS_Color *color = (TS_Color*)colorobj->GetAlignedPointerFromInternalField(0);

    SDL_LockSurface(surface);
    pixels[x+((surface->w)*y)] = color->toInt();
    SDL_UnlockSurface(surface);

    if(SDL_UnlockMutex(SurfaceQueueIndependantMutex)<0)
        exit(0x177);

    return v8::Undefined();

}

v8Function SurfaceRectangle(V8ARGS){
    if(args.Length()<5){
        THROWERROR("[" PLUGINNAME "] SurfaceRectangle Error: Called with fewer than 5 arguments.");
    }
    CHECK_ARG_INT(0);
    CHECK_ARG_INT(1);
    CHECK_ARG_INT(2);
    CHECK_ARG_INT(3);
    CHECK_ARG_OBJ(4);

	int x = args[0]->Int32Value();
	int y = args[1]->Int32Value();
	int w = args[2]->Int32Value();
	int h = args[3]->Int32Value();

    if((w==0)||(h==0)){
        return v8::Undefined();
    }

    if(w<0){
        x+=w;
        w=-w;
    }
    if(h<0){
        y+=h;
        h=-h;
    }

    v8::Local<v8::Object> colorobj = v8::Local<v8::Object>::Cast(args[4]);
    TS_Color *color = (TS_Color*)colorobj->GetAlignedPointerFromInternalField(0);

    SDL_Rect rect = {(short int)x, (short int)y, (unsigned short int)w, (unsigned short int)h};

    //TODO: This doesn't NEED to be immediate, make this queueable for the surface drawing thread.

    if(SDL_LockMutex(SurfaceQueueIndependantMutex)<0)
        exit(0x178);

    SDL_Surface *surface = GET_SELF(SDL_Surface*);

    READY_SURFACE(surface);

    if(SDL_FillRect(surface, &rect, color->toInt())!=0){

        if(SDL_UnlockMutex(SurfaceQueueIndependantMutex)<0)
            exit(0x179);

        printf("[" PLUGINNAME "] SurfaceRectangle Error: Could not set alpha on surface: %s", SDL_GetError());
        SDL_ClearError();
        THROWERROR("[" PLUGINNAME "] SurfaceRectangle Error: Could not draw rectangle.");
    }

    if(SDL_UnlockMutex(SurfaceQueueIndependantMutex)<0)
        exit(0x179);
	return v8::Undefined();
}


void TS_QueueSoftLine(int, int, int, int, TS_Color*, SDL_Surface*);

v8Function SurfaceLine(V8ARGS){
    if(args.Length()<5){
        THROWERROR("[" PLUGINNAME "] SurfaceLine Error: Called with fewer than 5 arguments.");
    }
    CHECK_ARG_INT(0);
    CHECK_ARG_INT(1);
    CHECK_ARG_INT(2);
    CHECK_ARG_INT(3);
    CHECK_ARG_OBJ(4);

	int x1 = args[0]->Int32Value();
	int y1 = args[1]->Int32Value();
	int x2 = args[2]->Int32Value();
	int y2 = args[3]->Int32Value();

    v8::Local<v8::Object> colorobj = v8::Local<v8::Object>::Cast(args[4]);
    TS_Color *color = (TS_Color*)colorobj->GetAlignedPointerFromInternalField(0);

    SDL_Surface *surface = GET_SELF(SDL_Surface*);

    TS_QueueSoftLine(x1, y1, x2, y2, color, surface);

	return v8::Undefined();
}

void TS_QueueSoftLine(int x1, int y1, int x2, int y2, TS_Color *color, SDL_Surface *surface){
    if(SDL_LockMutex(SurfaceQueueMutex)<0)
        exit(0x112);
    SoftLineArgs *arg = (SoftLineArgs*)malloc(sizeof(SoftLineArgs));
    arg->surface  = surface;
    arg->color    = color;
    arg->x1 = x1;
    arg->x2 = x2;
    arg->y1 = y1;
    arg->y2 = y2;

    SurfaceQueue[QueuePlacingPosition] = new TS_GenericSurfaceFunction(TS_SoftLine, (void *)arg, surface, NULL);
    QueuePlacingPosition++;
    QueuePlacingPosition%=SurfaceQueueSize;


    if(SDL_UnlockMutex(SurfaceQueueMutex)<0)
        exit(0x113);
}


v8Function SurfaceGradientRectangle(V8ARGS){
    if(args.Length()<8){
        THROWERROR("[" PLUGINNAME "] SurfaceGradientRectangle Error: Called with fewer than 8 arguments.");
    }
    CHECK_ARG_INT(0);
    CHECK_ARG_INT(1);
    CHECK_ARG_INT(2);
    CHECK_ARG_INT(3);
    CHECK_ARG_OBJ(4);

	int x = args[0]->Int32Value();
	int y = args[1]->Int32Value();
	int w = args[2]->Int32Value();
	int h = args[3]->Int32Value();

    if((w==0)||(h==0)){
        return v8::Undefined();
    }

    if(w<0){
        x+=w;
        w=-w;
    }
    if(h<0){
        y+=h;
        h=-h;
    }

    v8::Local<v8::Object> colorobj = v8::Local<v8::Object>::Cast(args[4]);
    TS_Color *color1 = (TS_Color*)colorobj->GetAlignedPointerFromInternalField(0);

    colorobj = v8::Local<v8::Object>::Cast(args[5]);
    TS_Color *color2 = (TS_Color*)colorobj->GetAlignedPointerFromInternalField(0);

    colorobj = v8::Local<v8::Object>::Cast(args[6]);
    TS_Color *color3 = (TS_Color*)colorobj->GetAlignedPointerFromInternalField(0);

    colorobj = v8::Local<v8::Object>::Cast(args[7]);
    TS_Color *color4 = (TS_Color*)colorobj->GetAlignedPointerFromInternalField(0);

    SDL_Surface *surface = GET_SELF(SDL_Surface*);

    if(((color1==color2)&&(color1==color3)&&(color1==color4))||((color1->toInt()==color2->toInt())&&(color1->toInt()==color3->toInt())&&(color1->toInt()==color4->toInt()))){
        TS_SoftRectangle(x, y, w, h, color1, surface);
    }

    TS_SoftGradientRectangle(x, y, w, h, color1, color2, color3, color4, surface);

	return v8::Undefined();
}

v8Function SurfaceOutlinedCircle(V8ARGS){
    if(args.Length()<4){
        THROWERROR("[" PLUGINNAME "] SurfaceOutlinedCircle Error: Called with fewer than 4 arguments.");
    }
    CHECK_ARG_INT(0);
    CHECK_ARG_INT(1);
    CHECK_ARG_INT(2);
    CHECK_ARG_OBJ(3);

	int x = args[0]->Int32Value();
	int y = args[1]->Int32Value();
	int r = args[2]->Int32Value();

    if(r<0){
        r*=-1;
    }
    else if(r==0){
        return v8::Undefined();
    }

    v8::Local<v8::Object> colorobj = v8::Local<v8::Object>::Cast(args[3]);
    TS_Color *color = (TS_Color*)colorobj->GetAlignedPointerFromInternalField(0);

    SDL_Surface *surface = GET_SELF(SDL_Surface*);

    TS_SoftOutlinedCircle(x, y, r, color, surface);

	return v8::Undefined();
}

v8Function SurfaceFilledCircle(V8ARGS){
    if(args.Length()<4){
        THROWERROR("[" PLUGINNAME "] SurfaceFilledCircle Error: Called with fewer than 4 arguments.");
    }
    CHECK_ARG_INT(0);
    CHECK_ARG_INT(1);
    CHECK_ARG_INT(2);
    CHECK_ARG_OBJ(3);

	int x = args[0]->Int32Value();
	int y = args[1]->Int32Value();
	int r = args[2]->Int32Value();

    if(r<0){
        r*=-1;
    }
    else if(r==0){
        return v8::Undefined();
    }

    v8::Local<v8::Object> colorobj = v8::Local<v8::Object>::Cast(args[3]);
    TS_Color *color = (TS_Color*)colorobj->GetAlignedPointerFromInternalField(0);

    SDL_Surface *surface = GET_SELF(SDL_Surface*);

    TS_SoftFilledCircle(x, y, r, color, surface);

	return v8::Undefined();
}

struct TS_SurfaceBlitSurfaceArg{SDL_Surface *src; SDL_Rect *srcrect; SDL_Surface *dst; SDL_Rect *dstrect; };

void TS_SurfaceBlitSurface(void *arg){
    TS_SurfaceBlitSurfaceArg *carg = (TS_SurfaceBlitSurfaceArg*)arg;

	SDL_BlitSurface(carg->src, carg->srcrect, carg->dst, carg->dstrect);

    free(carg);
}

v8Function SurfaceBlitSurface(V8ARGS){
    if(args.Length()<2){
        THROWERROR(" Error: Called with fewer than 3 arguments.");
    }
    CHECK_ARG_OBJ(0);
    CHECK_ARG_INT(1);
    CHECK_ARG_INT(2);

	int x = args[1]->Int32Value();
	int y = args[2]->Int32Value();

    v8::Local<v8::Object> surfobj = v8::Local<v8::Object>::Cast(args[0]);
    SDL_Surface *surf = (SDL_Surface*)surfobj->GetAlignedPointerFromInternalField(0);

	SDL_Surface *surface = GET_SELF(SDL_Surface*);

    if(SDL_LockMutex(SurfaceQueueMutex)<0)
        exit(0x112);
    TS_SurfaceBlitSurfaceArg *arg = (TS_SurfaceBlitSurfaceArg*)malloc(sizeof(TS_SurfaceBlitSurfaceArg));

    SDL_Rect *rect = (SDL_Rect *)malloc(sizeof(SDL_Rect));

    rect->x = x;
    rect->y = y;
    rect->w = surf->w;
    rect->h = surf->h;

    arg->src = surf;
    arg->srcrect = NULL;
    arg->dst = surface;
    arg->dstrect = rect;

    SurfaceQueue[QueuePlacingPosition] = new TS_GenericSurfaceFunction(TS_SurfaceBlitSurface, (void *)arg, surface, surf);
    QueuePlacingPosition++;
    QueuePlacingPosition%=SurfaceQueueSize;


    if(SDL_UnlockMutex(SurfaceQueueMutex)<0)
        exit(0x113);


    return v8::Undefined();
}

v8Function SurfaceBlit(V8ARGS){
    if(args.Length()<2){
        THROWERROR("[" PLUGINNAME "] SurfaceBlit Error: Called with fewer than 2 arguments.");
    }
    CHECK_ARG_INT(0);
    CHECK_ARG_INT(1);

	int x = args[0]->Int32Value();
	int y = args[1]->Int32Value();

	SDL_Surface *surface = GET_SELF(SDL_Surface*);

    TS_Texture texture = TS_SurfaceToTexture(surface);

    int xs[4] = {x, x+surface->w, x+surface->w, x};
    int ys[4] = {y, y, y+surface->h, y+surface->h};

    TS_BlitTexture(texture, xs, ys);

    glDeleteTextures(1, &texture);

    return v8::Undefined();
}

