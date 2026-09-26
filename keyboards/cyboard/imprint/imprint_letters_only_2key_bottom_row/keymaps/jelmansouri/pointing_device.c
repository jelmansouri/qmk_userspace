#include QMK_KEYBOARD_H

#if defined(POINTING_DEVICE_ENABLE) && defined(SPLIT_POINTING_ENABLE) && defined(POINTING_DEVICE_COMBINED)

/* Left trackball scrolls, right trackball moves the cursor. */

#    ifndef IMPRINT_LEFT_CPI
#        define IMPRINT_LEFT_CPI 1200
#    endif

#    ifndef IMPRINT_RIGHT_CPI
#        define IMPRINT_RIGHT_CPI 400
#    endif

/* Sensor counts per hi-res wheel unit (with POINTING_DEVICE_HIRES_SCROLL_EXPONENT 1,
 * 10 units = 1 notch). Larger = slower scrolling. */
#    ifndef IMPRINT_SCROLL_DIVISOR
#        define IMPRINT_SCROLL_DIVISOR 48
#    endif

/* Flip both scroll axes ("natural" scrolling). */
#    ifndef IMPRINT_SCROLL_REVERSE
#        define IMPRINT_SCROLL_REVERSE 1
#    endif

/* Cursor acceleration, speed measured in sensor counts per second:
 *   gain = 1 + (MAX_GAIN - 1) * s² / (s² + KNEE²),  s = speed - TAKEOFF
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

/* After this long without motion, sub-count carries and speed history reset. */
#    ifndef IMPRINT_IDLE_RESET_MS
#        define IMPRINT_IDLE_RESET_MS 100
#    endif

static inline int32_t clamp32(int32_t value, int32_t min, int32_t max) {
    return value < min ? min : value > max ? max : value;
}

/* ---- Right: cursor acceleration ---------------------------------------- */

static uint32_t accel_gain_q8(uint32_t speed) {
    if (speed <= IMPRINT_ACCEL_TAKEOFF) {
        return 256;
    }
    const uint64_t s2 = (uint64_t)(speed - IMPRINT_ACCEL_TAKEOFF) * (speed - IMPRINT_ACCEL_TAKEOFF);
    const uint64_t k2 = (uint64_t)IMPRINT_ACCEL_KNEE * IMPRINT_ACCEL_KNEE;
    return 256 + (uint32_t)(((uint64_t)(IMPRINT_ACCEL_MAX_GAIN_Q8 - 256) * s2) / (s2 + k2));
}

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
    static int32_t  carry_x, carry_y;
    static uint32_t last_motion, speed_avg;

    if (report.x == 0 && report.y == 0) {
        return report;
    }

    uint32_t elapsed = timer_elapsed32(last_motion);
    last_motion      = timer_read32();
    if (elapsed > IMPRINT_IDLE_RESET_MS) {
        carry_x = carry_y = 0;
        speed_avg         = 0;
    }
    if (elapsed == 0) {
        elapsed = 1;
    }

    /* Cheap vector length (max + min/2, within ~12% of Euclidean) so diagonals
     * accelerate like straight lines. */
    const uint32_t ax    = (uint32_t)abs(report.x);
    const uint32_t ay    = (uint32_t)abs(report.y);
    const uint32_t dist  = ax > ay ? ax + ay / 2 : ay + ax / 2;
    const uint32_t speed = dist * 1000 / elapsed;
    speed_avg            = speed_avg ? (speed_avg + speed) / 2 : speed;

    const uint32_t gain = accel_gain_q8(speed_avg);
    report.x            = clamp32(apply_gain(report.x, gain, &carry_x), MOUSE_REPORT_XY_MIN, MOUSE_REPORT_XY_MAX);
    report.y            = clamp32(apply_gain(report.y, gain, &carry_y), MOUSE_REPORT_XY_MIN, MOUSE_REPORT_XY_MAX);
    return report;
}

/* ---- Left: scrolling ---------------------------------------------------- */

static mouse_hv_report_t scroll_axis(int32_t delta, int32_t *acc) {
    if (delta == 0) {
        return 0;
    }
    if ((delta < 0) != (*acc < 0)) {
        *acc = 0; /* direction change: don't unwind the old remainder first */
    }
    *acc += delta;
    const int32_t ticks = *acc / IMPRINT_SCROLL_DIVISOR;
    *acc -= ticks * IMPRINT_SCROLL_DIVISOR;
    return clamp32(ticks, MOUSE_REPORT_HV_MIN, MOUSE_REPORT_HV_MAX);
}

static report_mouse_t scroll_report(report_mouse_t report) {
    static int32_t  acc_h, acc_v;
    static uint32_t last_motion;

    int32_t x = report.x;
    int32_t y = report.y;
    report.x  = 0;
    report.y  = 0;

    if (x == 0 && y == 0) {
        return report;
    }
    if (timer_elapsed32(last_motion) > IMPRINT_IDLE_RESET_MS) {
        acc_h = acc_v = 0;
    }
    last_motion = timer_read32();

#    if IMPRINT_SCROLL_REVERSE
    x = -x;
    y = -y;
#    endif

    report.h = scroll_axis(x, &acc_h);
    report.v = scroll_axis(y, &acc_v);
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
