#ifndef MINT_UI_API_H_
#define MINT_UI_API_H_

#if defined(_WIN32) || defined(_WIN64)
    #define MINT_DLLEXPORT __declspec(dllexport)
    #define MINT_DLLIMPORT __declspec(dllimport)
#else
    #define MINT_DLLEXPORT __attribute__((visibility("default")))
    #define MINT_DLLIMPORT __attribute__((visibility("default")))
#endif

#ifdef MINT_UI_EXPORTS
    #define MINT_UI_API MINT_DLLEXPORT
#else
    #define MINT_UI_API MINT_DLLIMPORT
#endif

#endif // MINT_UI_API_H_
