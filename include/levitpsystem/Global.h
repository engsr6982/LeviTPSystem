#pragma once

#ifdef TPS_EXPORTS
#define TPSAPI __declspec(dllexport)
#else
#define TPSAPI __declspec(dllimport)
#endif

#define TPSNDAPI [[nodiscard]] TPSAPI


#define TPS_DISALLOW_COPY_AND_MOVE(CLASS)                                                                              \
    CLASS(const CLASS&)            = delete;                                                                           \
    CLASS(CLASS&&)                 = delete;                                                                           \
    CLASS& operator=(const CLASS&) = delete;                                                                           \
    CLASS& operator=(CLASS&&)      = delete;
