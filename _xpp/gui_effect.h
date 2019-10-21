#pragma once
#include <set>
#include <cmath>
#include <chrono>
#include <algorithm>
#include "aux_utils.h"
#include "pix.h"
namespace gui
{
    using namespace pix;

    template
        <class type> inline type transition_state (type from, type to, int64_t mul, int64_t div) {
        if (div == 0) return to; return clamp<type>(to*mul/div + from*(div-mul)/div);
    }
    template<> bool inline transition_state<bool> (bool from, bool to, int64_t mul, int64_t div) {
        return div == 0 || mul >= div ? to : from;
    }
    template<> RGBA inline transition_state<RGBA> (RGBA from, RGBA to, int64_t mul, int64_t div) {
        return RGBA (
            transition_state<uint8_t>(from.r, to.r, mul, div),
            transition_state<uint8_t>(from.g, to.g, mul, div),
            transition_state<uint8_t>(from.b, to.b, mul, div),
            transition_state<uint8_t>(from.a, to.a, mul, div));
    }
    template<> XYWH inline transition_state<XYWH> (XYWH from, XYWH to, int64_t mul, int64_t div) {
        return XYWH (
            transition_state<int>(from.x, to.x, mul, div),
            transition_state<int>(from.y, to.y, mul, div),
            transition_state<int>(from.w, to.w, mul, div),
            transition_state<int>(from.h, to.h, mul, div));
    }
    template<> XY   inline transition_state<XY>   (XY   from, XY   to, int64_t mul, int64_t div) {
        return XY   (
            transition_state<int>(from.x, to.x, mul, div),
            transition_state<int>(from.y, to.y, mul, div));
    }

    struct time
    {
        int64_t  ms = 0;
        explicit time() = default;
        explicit time(int64_t ms) : ms(ms) {}
        template<class R, class P> time
        (std::chrono::duration<R, P> duration) {
            ms = std::chrono::duration_cast<
                 std::chrono::milliseconds>
                 (duration).count();
        }

        void operator += (time t) { ms += t.ms; }; friend time operator + (time a, time b) { a += b; return a; }
        void operator -= (time t) { ms -= t.ms; }; friend time operator - (time a, time b) { a -= b; return a; }

        friend bool operator == (time l, time r) { return l.ms == r.ms; }
        friend bool operator != (time l, time r) { return l.ms != r.ms; }
        friend bool operator <= (time l, time r) { return l.ms <= r.ms; }
        friend bool operator <  (time l, time r) { return l.ms <  r.ms; }
        friend bool operator >= (time l, time r) { return l.ms >= r.ms; }
        friend bool operator >  (time l, time r) { return l.ms >  r.ms; }

        static time was;
        static time now;
        static time pause;
        static void reset_after_pause() { pause = time(); }
        static void set_after_pause() { set(); pause = now - was; }
        static void set() { static
            auto t0 = std::chrono::high_resolution_clock::now ();
            auto t1 = std::chrono::high_resolution_clock::now ();
            was = now; now = t1-t0;
        }
    };
    inline time time::was;
    inline time time::now;
    inline time time::pause;
    inline time default_transition_time = std::chrono::milliseconds(0);

    struct base_property : polymorphic { virtual void tick () = 0; };
    inline registry<base_property*> active_properties;

    namespace base { struct widget; }
    extern std::set<base::widget*> widgets;
    extern void change (base::widget*, void*);
    extern base::widget* inholder (void*);

    template<class type> struct property : base_property
    {
        type from, to, was, now;
        time notch, lapse, transition_time = default_transition_time;
        base::widget* widget = nullptr; std::optional<size_t> receipt;

        property (type value = type()) : from(value), to(value), was(value), now(value) {}
       ~property () { if (receipt) active_properties.erase(*receipt); }

        void operator = (type value) { go (value, time(0)); }

        void go (type to_) { go (to_, transition_time); }

        void go (type to_, time lapse_) {
            from = now; to = to_;
            notch = time::now;
            lapse = lapse_;
            tick();
        }

        void tick () override { 
            was = now; notch += time::pause;
            if (now != to)
                now = transition_state (from, to,
                    std::min(time::now - notch, lapse).ms, lapse.ms);
            if (now == to && receipt) { active_properties.erase(*receipt); receipt.reset(); }
            if (now != to && !receipt) receipt = active_properties.append(this);
            if (now != was) {
                if (!widget) widget = inholder(this);
                if (!widget) throw std::runtime_error("property: wrong inholder");
                change(widget, this);
            }
        }
    };
}