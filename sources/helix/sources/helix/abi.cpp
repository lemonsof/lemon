#include <cassert>
#include <helix/abi.h>
#include <helix/alloc.hpp>
#include <helix/runtimes.hpp>
#include <helix/assembly.h>
using namespace helix::impl;

helix_uuid_t HELIX_WIN32_ERROR_CATALOG = {0x3eb0937e, 0xdbe9, 0x4946,
    { 0xa0, 0xb4, 0x44, 0xc4, 0x19, 0xe9, 0x46, 0xf7}};

helix_uuid_t HELIX_COM_ERROR_CATALOG = {0x3eb0937e, 0xdbe9, 0x4946,
    { 0xa0, 0xb4, 0x44, 0xc4, 0x19, 0xe9, 0x46, 0xf2}};

helix_uuid_t HELIX_POSIX_ERROR_CATALOG = {0x11a7987e, 0xa950, 0x434c,
    { 0xa4, 0xbb, 0x76, 0xdf, 0x4d, 0x42, 0xa2, 0xe1}};

helix_uuid_t HELIX_UNITTEST_ERROR_CATALOG = {0x11a7987e, 0xa950, 0x434c,
    { 0xa4, 0xbb, 0x76, 0xd1, 0x4d, 0x42, 0xa2, 0xe2}};

HELIX_API helix_t helix_open(helix_alloc_t * alloc, helix_errcode * errorCode) {
    try {
        if (alloc == nullptr) {
            alloc = default_alloc();
        }
        runtimes *rt = new(alloc) runtimes(alloc);
        return reinterpret_cast<helix_t> (rt->main_actor());
    } catch (const helix_errcode & e) {
        *errorCode = e;

        return nullptr;
    }
}

HELIX_API void helix_close(helix_t helix) {
    basic_actor_t * actor = reinterpret_cast<basic_actor_t*> (helix);

    assert(actor == actor->rt()->main_actor());

    release(actor->rt(), actor->rt()->alloc());
}

HELIX_API uintptr_t helix_go(helix_t helix, void(*f)(helix_t, void*), void* userdata, size_t stacksize) {
    basic_actor_t * actor = reinterpret_cast<basic_actor_t*> (helix);

    try {
        actor->reset_lasterror();

        return actor->rt()->create_go(f, userdata, stacksize);

    } catch (const helix_errcode & e) {
        actor->lasterror(e);
        return uintptr_t(-1);
    }
}

HELIX_API void helix_event_add(helix_t helix, helix_event *event) {
    basic_actor_t * actor = reinterpret_cast<basic_actor_t*> (helix);

    actor->rt()->add_event(actor, event);
}

HELIX_API helix_event* helix_event_remove(helix_t helix, uintptr_t eventid) {
    basic_actor_t * actor = reinterpret_cast<basic_actor_t*> (helix);

    return actor->rt()->remove_event(actor, eventid);
}

HELIX_API void helix_event_clear(helix_t helix) {
    basic_actor_t * actor = reinterpret_cast<basic_actor_t*> (helix);

    actor->rt()->clear_events(actor);
}

HELIX_API helix_errcode * helix_lasterror(helix_t helix) {
    return reinterpret_cast<basic_actor_t*> (helix)->lasterror();
}

HELIX_API helix_event* helix_event_poll(helix_t helix, helix_duration *duration) {
    basic_actor_t * actor = reinterpret_cast<basic_actor_t*> (helix);

    actor->reset_lasterror();

    if (duration != nullptr) {
        helix_duration milliseconds = {helix_milliseconds, 0};

        helix_duration_cast(duration, &milliseconds);

        actor->rt()->timewheel().create_timer(actor->handle(), std::chrono::milliseconds(milliseconds.count));
    }

    helix_context_jump(actor->context(), actor->Q()->context(), 0);

    if (actor->status() == actor_status::stopped) {
        helix_declare_errcode(errorCode);

        helix_user_errno(errorCode, HELIX_KILLED);

        actor->lasterror(errorCode);

        return nullptr;
    }

    if (actor->get_event()->id == timewheel_t::event()) {
        return nullptr;
    }

    return actor->get_event();
}

HELIX_API void helix_notify(helix_t helix, uintptr_t target, uintptr_t eventid) {
    basic_actor_t * actor = reinterpret_cast<basic_actor_t*> (helix);

    actor->reset_lasterror();

    actor->rt()->notify(target, eventid);
}

HELIX_API void helix_notify_one(helix_t helix, uintptr_t eventid) {
    basic_actor_t * actor = reinterpret_cast<basic_actor_t*> (helix);

    actor->reset_lasterror();

    actor->rt()->notify_one(eventid);
}

HELIX_API void helix_notify_all(helix_t helix, uintptr_t eventid) {
    basic_actor_t * actor = reinterpret_cast<basic_actor_t*> (helix);

    actor->reset_lasterror();

    actor->rt()->notify_all(eventid);
}

