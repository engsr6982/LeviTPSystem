#pragma once

#include <string>
using string = std::string;

#include <filesystem>
namespace fs = std::filesystem;

#include "ll/api/i18n/I18n.h"
using ll::operator""_tr;

#define DISALLOW_COPY_AND_ASSIGN(TypeName)                                                                             \
    TypeName(TypeName&&)                  = delete;                                                                    \
    TypeName(TypeName const&)             = delete;                                                                    \
    TypeName& operator=(TypeName&&)       = delete;                                                                    \
    TypeName& operator=(TypeName const&)  = delete;                                                                    \
    TypeName& operator=(TypeName const&&) = delete;

#define DISALLOW_CONSTRUCT(TypeName)                                                                                   \
    TypeName()                            = delete;                                                                    \
    TypeName(TypeName&&)                  = delete;                                                                    \
    TypeName(TypeName const&)             = delete;                                                                    \
    TypeName& operator=(TypeName&&)       = delete;                                                                    \
    TypeName& operator=(TypeName const&)  = delete;                                                                    \
    TypeName& operator=(TypeName const&&) = delete;


// LeviLamina Event
#include "ll/api/event/Cancellable.h"
#include "ll/api/event/Event.h"
using Event            = ll::event::Event;
using CancellableEvent = ll::event::Cancellable<ll::event::Event>;

#include "ll/api/event/Emitter.h"
#define IMPLEMENT_EVENT_EMITTER(EventName)                                                                             \
    static std::unique_ptr<ll::event::EmitterBase> emitterFactory##EventName();                                        \
    class EventName##Emitter : public ll::event::Emitter<emitterFactory##EventName, EventName> {};                     \
    static std::unique_ptr<ll::event::EmitterBase> emitterFactory##EventName() {                                       \
        return std::make_unique<EventName##Emitter>();                                                                 \
    }