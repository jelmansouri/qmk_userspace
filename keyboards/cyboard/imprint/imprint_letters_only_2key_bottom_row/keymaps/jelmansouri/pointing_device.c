#include QMK_KEYBOARD_H

#if defined(POINTING_DEVICE_ENABLE) && defined(SPLIT_POINTING_ENABLE) && defined(POINTING_DEVICE_COMBINED)

/* Left trackball scrolls, right trackball moves the cursor. Both use the same
 * speed-response curve, with speed measured in sensor counts per second:
 *   response = s² / (s² + KNEE²),  s = speed - TAKEOFF  (0 below TAKEOFF)
 * which ramps smoothly from 0 towards 1 as speed rises. */

#    ifndef IMPRINT_LEFT_CPI
#        define IMPRINT_LEFT_CPI 1200
#    endif

#    ifndef IMPRINT_RIGHT_CPI
#        define IMPRINT_RIGHT_CPI 400
#    endif

/* Cursor acceleration: gain goes from 1x up to MAX_GAIN along the curve.
 * Gain is in Q8 (256 = 1x). Set IMPRINT_ACCEL_MAX_GAIN_Q8 to 256 to disable. */
#    ifndef IMPRINT_ACCEL_TAKEOFF
#        define IMPRINT_ACCEL_TAKEOFF 900
#    endif

#    ifndef IMPRINT_ACCEL_KNEE
#        define IMPRINT_ACCEL_KNEE 3600
#    endif

#    ifndef IMPRINT_ACCEL_MAX_GAIN_Q8
#        define IMPRINT_ACCEL_MAX_GAIN_Q8 704 /* 2.75x */
#    endif

/* Scroll acceleration: sensor counts per hi-res wheel unit (with
 * POINTING_DEVICE_HIRES_SCROLL_EXPONENT 1, 10 units = 1 notch) shrinks from
 * DIVISOR at slow speed to MIN_DIVISOR on fast flicks. Larger = slower.
 * Set MIN_DIVISOR equal to DIVISOR to disable. */
#    ifndef IMPRINT_SCROLL_DIVISOR
#        define IMPRINT_SCROLL_DIVISOR 128
#    endif

#    ifndef IMPRINT_SCROLL_MIN_DIVISOR
#        define IMPRINT_SCROLL_MIN_DIVISOR 16
#    endif

#    ifndef IMPRINT_SCROLL_TAKEOFF
#        define IMPRINT_SCROLL_TAKEOFF 600
#    endif

#    ifndef IMPRINT_SCROLL_KNEE
#        define IMPRINT_SCROLL_KNEE 4500
#    endif

/* Flip both scroll axes ("natural" scrolling). */
#    ifndef IMPRINT_SCROLL_REVERSE
#        define IMPRINT_SCROLL_REVERSE 1
#    endif

/* After this long without motion, sub-count carries and speed history reset. */
#    ifndef IMPRINT_IDLE_RESET_MS
#        define IMPRINT_IDLE_RESET_MS 100
#    endif

#    if IMPRINT_SCROLL_MIN_DIVISOR < 1 || IMPRINT_SCROLL_MIN_DIVISOR > IMPRINT_SCROLL_DIVISOR
#        error "IMPRINT_SCROLL_MIN_DIVISOR must be in [1, IMPRINT_SCROLL_DIVISOR]"
#    endif

typedef struct {
    uint32_t last_motion;
    uint32_t speed_avg;
} speed_tracker_t;

static inline int32_t clamp32(int32_t value, int32_t min, int32_t max) {
    return value < min ? min : value > max ? max : value;
}

/* Returns 0..256 (Q8) along the shared response curve. */
static uint32_t curve_q8(uint32_t speed, uint32_t takeoff, uint32_t knee) {
    if (speed <= takeoff) {
        return 0;
    }
    const uint64_t s2 = (uint64_t)(speed - takeoff) * (speed - takeoff);
    const uint64_t k2 = (uint64_t)knee * knee;
    return (uint32_t)((s2 * 256) / (s2 + k2));
}

/* Updates the smoothed speed for a non-zero motion sample. Returns true if the
 * previous motion was long enough ago that callers should reset their carries. */
static bool track_speed(speed_tracker_t *t, int32_t x, int32_t y) {
    uint32_t elapsed = timer_elapsed32(t->last_motion);
    t->last_motion   = timer_read32();

    const bool idle = elapsed > IMPRINT_IDLE_RESET_MS;
    if (idle) {
        t->speed_avg = 0;
    }
    if (elapsed == 0) {
        elapsed = 1;
    }

    /* Cheap vector length (max + min/2, within ~12% of Euclidean) so diagonals
     * behave like straight lines. */
    const uint32_t ax    = (uint32_t)abs(x);
    const uint32_t ay    = (uint32_t)abs(y);
    const uint32_t dist  = ax > ay ? ax + ay / 2 : ay + ax / 2;
    const uint32_t speed = dist * 1000 / elapsed;
    t->speed_avg         = t->speed_avg ? (t->speed_avg + speed) / 2 : speed;
    return idle;
}

/* ---- Right: cursor acceleration ---------------------------------------- */

static int32_t apply_gain(int32_t delta, uint32_t gain_q8, int32_t *carry) {
    if ((delta < 0) != (*carry < 0)) {
        *carry = 0; /* direction change: drop the stale fraction */
    }
    const int32_t scaled = delta * (int32_t)gain_q8 + *carry;
    const int32_t out    = scaled / 256;
    *carry               = scaled - out * 256;
    return out;
}

static report_mouse_t accelerate_cursor(report_mouse_t report) {
    static speed_tracker_t tracker;
    static int32_t         carry_x, carry_y;

    if (report.x == 0 && report.y == 0) {
        return report;
    }
    if (track_speed(&tracker, report.x, report.y)) {
        carry_x = carry_y = 0;
    }

    const uint32_t response = curve_q8(tracker.speed_avg, IMPRINT_ACCEL_TAKEOFF, IMPRINT_ACCEL_KNEE);
    const uint32_t gain     = 256 + ((IMPRINT_ACCEL_MAX_GAIN_Q8 - 256) * response) / 256;

    report.x = clamp32(apply_gain(report.x, gain, &carry_x), MOUSE_REPORT_XY_MIN, MOUSE_REPORT_XY_MAX);
    report.y = clamp32(apply_gain(report.y, gain, &carry_y), MOUSE_REPORT_XY_MIN, MOUSE_REPORT_XY_MAX);
    return report;
}

/* ---- Left: scrolling ---------------------------------------------------- */

/* Accumulates in Q8 wheel units so the divisor can change between samples. */
static mouse_hv_report_t scroll_axis(int32_t delta, uint32_t divisor, int32_t *acc_q8) {
    if (delta == 0) {
        return 0;
    }
    if ((delta < 0) != (*acc_q8 < 0)) {
        *acc_q8 = 0; /* direction change: don't unwind the old remainder first */
    }
    *acc_q8 += delta * 256 / (int32_t)divisor;
    const int32_t ticks = *acc_q8 / 256;
    *acc_q8 -= ticks * 256;
    return clamp32(ticks, MOUSE_REPORT_HV_MIN, MOUSE_REPORT_HV_MAX);
}

static report_mouse_t scroll_report(report_mouse_t report) {
    static speed_tracker_t tracker;
    static int32_t         acc_h, acc_v;

    int32_t x = report.x;
    int32_t y = report.y;
    report.x  = 0;
    report.y  = 0;

    if (x == 0 && y == 0) {
        return report;
    }
    if (track_speed(&tracker, x, y)) {
        acc_h = acc_v = 0;
    }

#    if IMPRINT_SCROLL_REVERSE
    x = -x;
    y = -y;
#    endif

    const uint32_t response = curve_q8(tracker.speed_avg, IMPRINT_SCROLL_TAKEOFF, IMPRINT_SCROLL_KNEE);
    const uint32_t divisor =
        IMPRINT_SCROLL_DIVISOR - ((IMPRINT_SCROLL_DIVISOR - IMPRINT_SCROLL_MIN_DIVISOR) * response) / 256;

    report.h = scroll_axis(x, divisor, &acc_h);
    report.v = scroll_axis(y, divisor, &acc_v);
    return report;
}

/* ---- QMK hooks ---------------------------------------------------------- */

void pointing_device_init_user(void) {
    pointing_device_set_cpi_on_side(true, IMPRINT_LEFT_CPI);
    pointing_device_set_cpi_on_side(false, IMPRINT_RIGHT_CPI);
}

report_mouse_t pointing_device_task_combined_user(report_mouse_t left_report, report_mouse_t right_report) {
    return pointing_device_combine_reports(scroll_report(left_report), accelerate_cursor(right_report));
}

#endif
