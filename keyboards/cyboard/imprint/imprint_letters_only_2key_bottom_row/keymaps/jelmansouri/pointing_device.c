#include QMK_KEYBOARD_H

#if defined(POINTING_DEVICE_ENABLE) && defined(SPLIT_POINTING_ENABLE) && defined(POINTING_DEVICE_COMBINED)

#    ifndef IMPRINT_POINTING_LEFT_DEFAULT_CPI
#        define IMPRINT_POINTING_LEFT_DEFAULT_CPI 1000
#    endif

#    ifndef IMPRINT_POINTING_RIGHT_DEFAULT_CPI
#        define IMPRINT_POINTING_RIGHT_DEFAULT_CPI 400
#    endif

#    ifndef IMPRINT_SENSOR_NOISE_COUNTS
#        define IMPRINT_SENSOR_NOISE_COUNTS 1
#    endif

#    ifndef IMPRINT_SENSOR_NOISE_CONFIRM_COUNTS
#        define IMPRINT_SENSOR_NOISE_CONFIRM_COUNTS 2
#    endif

#    ifndef IMPRINT_SENSOR_NOISE_CONFIRM_MS
#        define IMPRINT_SENSOR_NOISE_CONFIRM_MS 15
#    endif

#    ifndef IMPRINT_SENSOR_ACTIVE_MS
#        define IMPRINT_SENSOR_ACTIVE_MS 40
#    endif

#    ifndef IMPRINT_SENSOR_SPIKE_COUNTS
#        define IMPRINT_SENSOR_SPIKE_COUNTS 32U
#    endif

#    ifndef IMPRINT_SENSOR_SPIKE_IDLE_COUNTS
#        define IMPRINT_SENSOR_SPIKE_IDLE_COUNTS 80U
#    endif

#    ifndef IMPRINT_SENSOR_SPIKE_WINDOW_MS
#        define IMPRINT_SENSOR_SPIKE_WINDOW_MS 20U
#    endif

#    ifndef IMPRINT_SENSOR_SPIKE_CONFIRM_MS
#        define IMPRINT_SENSOR_SPIKE_CONFIRM_MS 8U
#    endif

#    ifndef IMPRINT_SENSOR_SPIKE_MAX_MULTIPLIER
#        define IMPRINT_SENSOR_SPIKE_MAX_MULTIPLIER 5U
#    endif

#    ifndef IMPRINT_SENSOR_SPIKE_ACCEL_COUNTS
#        define IMPRINT_SENSOR_SPIKE_ACCEL_COUNTS 16U
#    endif

#    ifndef IMPRINT_RIGHT_ACCEL_TAKEOFF
#        define IMPRINT_RIGHT_ACCEL_TAKEOFF 900U
#    endif

#    ifndef IMPRINT_RIGHT_ACCEL_KNEE
#        define IMPRINT_RIGHT_ACCEL_KNEE 3600U
#    endif

#    ifndef IMPRINT_RIGHT_ACCEL_MAX_GAIN_Q8
#        define IMPRINT_RIGHT_ACCEL_MAX_GAIN_Q8 704U
#    endif

#    ifndef IMPRINT_RIGHT_ACCEL_CARRY_TIMEOUT_MS
#        define IMPRINT_RIGHT_ACCEL_CARRY_TIMEOUT_MS 120U
#    endif

#    ifndef IMPRINT_LEFT_SCROLL_DIVISOR_H
#        define IMPRINT_LEFT_SCROLL_DIVISOR_H 128U
#    endif

#    ifndef IMPRINT_LEFT_SCROLL_DIVISOR_V
#        define IMPRINT_LEFT_SCROLL_DIVISOR_V 128U
#    endif

#    ifndef IMPRINT_LEFT_SCROLL_MIN_DIVISOR_H
#        define IMPRINT_LEFT_SCROLL_MIN_DIVISOR_H 12U
#    endif

#    ifndef IMPRINT_LEFT_SCROLL_MIN_DIVISOR_V
#        define IMPRINT_LEFT_SCROLL_MIN_DIVISOR_V 12U
#    endif

#    ifndef IMPRINT_LEFT_SCROLL_ACCEL_STEP
#        define IMPRINT_LEFT_SCROLL_ACCEL_STEP 700U
#    endif

#    ifndef IMPRINT_LEFT_SCROLL_REVERSE_X
#        define IMPRINT_LEFT_SCROLL_REVERSE_X 1
#    endif

#    ifndef IMPRINT_LEFT_SCROLL_REVERSE_Y
#        define IMPRINT_LEFT_SCROLL_REVERSE_Y 1
#    endif

#    ifndef IMPRINT_LEFT_SCROLL_AXIS_LOCK_RATIO_Y
#        define IMPRINT_LEFT_SCROLL_AXIS_LOCK_RATIO_Y 1U
#    endif

#    ifndef IMPRINT_LEFT_SCROLL_AXIS_LOCK_RATIO_X
#        define IMPRINT_LEFT_SCROLL_AXIS_LOCK_RATIO_X 3U
#    endif

#    ifndef IMPRINT_LEFT_SCROLL_IDLE_RESET_MS
#        define IMPRINT_LEFT_SCROLL_IDLE_RESET_MS 120U
#    endif

#    define IMPRINT_Q8 256L

#    ifdef CONSOLE_ENABLE
#        define IMPRINT_PD_DEBUG(...) uprintf("imprint_pd: " __VA_ARGS__)
#    else
#        define IMPRINT_PD_DEBUG(...)
#    endif

typedef struct {
    mouse_xy_report_t pending_x;
    mouse_xy_report_t pending_y;
    uint32_t          pending_timer;
    uint32_t          activity_timer;
    mouse_xy_report_t last_x;
    mouse_xy_report_t last_y;
    uint32_t          last_movement;
    uint32_t          last_motion_timer;
    mouse_xy_report_t spike_x;
    mouse_xy_report_t spike_y;
    uint32_t          spike_timer;
} sensor_noise_filter_t;

typedef struct {
    int32_t  remainder_h_q8;
    int32_t  remainder_v_q8;
    int8_t   direction_h;
    int8_t   direction_v;
    int8_t   axis_lock;
    uint32_t timer;
} scroll_state_t;

static sensor_noise_filter_t left_noise_filter;
static sensor_noise_filter_t right_noise_filter;
static scroll_state_t        left_scroll_state;
static int32_t               right_accel_carry_x_q8;
static int32_t               right_accel_carry_y_q8;
static uint32_t              right_accel_timer;

static inline int32_t abs32(int32_t value) {
    if (value >= 0) {
        return value;
    }

    if (value == INT32_MIN) {
        return INT32_MAX;
    }

    return -value;
}

static inline int8_t sign32(int32_t value) {
    return value > 0 ? 1 : value < 0 ? -1 : 0;
}

static inline bool opposite_sign(int32_t a, int32_t b) {
    return a != 0 && b != 0 && ((a < 0) != (b < 0));
}

static inline bool vectors_correlate(int32_t x1, int32_t y1, int32_t x2, int32_t y2) {
    const int64_t dot = ((int64_t)x1 * x2) + ((int64_t)y1 * y2);
    return dot > 0;
}

static inline uint32_t manhattan_abs_u32(int32_t x, int32_t y) {
    return (uint32_t)abs32(x) + (uint32_t)abs32(y);
}

static uint32_t isqrt_u64(uint64_t value) {
    if (value == 0) {
        return 0;
    }

    uint64_t x = value;
    uint64_t y = (x + 1U) >> 1;
    while (y < x) {
        x = y;
        y = (x + (value / x)) >> 1;
    }
    return (uint32_t)x;
}

static uint32_t vector_length_u32(int32_t x, int32_t y) {
    const uint64_t xx = (uint64_t)abs32(x);
    const uint64_t yy = (uint64_t)abs32(y);
    return isqrt_u64((xx * xx) + (yy * yy));
}

static inline mouse_xy_report_t clamp_xy(int32_t value);

static report_mouse_t limit_report_manhattan(report_mouse_t report, uint32_t limit) {
    const uint32_t movement = manhattan_abs_u32(report.x, report.y);
    if (limit == 0 || movement <= limit) {
        return report;
    }

    int32_t sx = ((int64_t)report.x * limit) / movement;
    int32_t sy = ((int64_t)report.y * limit) / movement;

    if (sx == 0 && report.x != 0) {
        sx = sign32(report.x);
    }
    if (sy == 0 && report.y != 0) {
        sy = sign32(report.y);
    }

    report.x = clamp_xy(sx);
    report.y = clamp_xy(sy);
    return report;
}

static inline mouse_xy_report_t clamp_xy(int32_t value) {
    if (value < MOUSE_REPORT_XY_MIN) {
        return MOUSE_REPORT_XY_MIN;
    }
    if (value > MOUSE_REPORT_XY_MAX) {
        return MOUSE_REPORT_XY_MAX;
    }
    return (mouse_xy_report_t)value;
}

static inline mouse_hv_report_t clamp_hv(int32_t value) {
    if (value < MOUSE_REPORT_HV_MIN) {
        return MOUSE_REPORT_HV_MIN;
    }
    if (value > MOUSE_REPORT_HV_MAX) {
        return MOUSE_REPORT_HV_MAX;
    }
    return (mouse_hv_report_t)value;
}

static void update_motion_history(sensor_noise_filter_t *state, report_mouse_t report) {
    state->last_x            = report.x;
    state->last_y            = report.y;
    state->last_movement     = manhattan_abs_u32(report.x, report.y);
    state->last_motion_timer = timer_read32();
    state->spike_x           = 0;
    state->spike_y           = 0;
    state->spike_timer       = 0;
}

static report_mouse_t filter_sensor_spike(report_mouse_t report, sensor_noise_filter_t *state, char side) {
    const uint32_t movement = manhattan_abs_u32(report.x, report.y);

    if (movement == 0 || movement < IMPRINT_SENSOR_SPIKE_COUNTS) {
        if (movement != 0) {
            update_motion_history(state, report);
        }
        return report;
    }

    const bool recent_motion =
        state->last_motion_timer && timer_elapsed32(state->last_motion_timer) <= IMPRINT_SENSOR_SPIKE_WINDOW_MS;
    uint32_t allowed    = 0;
    bool     aligned    = true;
    bool     suspicious = false;

    if (!recent_motion) {
        suspicious = movement >= IMPRINT_SENSOR_SPIKE_IDLE_COUNTS;
    } else {
        allowed = (state->last_movement * IMPRINT_SENSOR_SPIKE_MAX_MULTIPLIER) + IMPRINT_SENSOR_SPIKE_ACCEL_COUNTS;
        aligned = vectors_correlate(report.x, report.y, state->last_x, state->last_y);

        suspicious =
            movement > allowed || (!aligned && movement > state->last_movement + IMPRINT_SENSOR_SPIKE_ACCEL_COUNTS);
    }

    if (!suspicious) {
        IMPRINT_PD_DEBUG("%c spike-pass x=%d y=%d movement=%lu last=%lu allowed=%lu aligned=%d recent=%d\n", side,
                         report.x, report.y, (unsigned long)movement, (unsigned long)state->last_movement,
                         (unsigned long)allowed, aligned, recent_motion);
        update_motion_history(state, report);
        return report;
    }

    const bool confirmed_spike = state->spike_timer &&
                                 timer_elapsed32(state->spike_timer) <= IMPRINT_SENSOR_SPIKE_CONFIRM_MS &&
                                 vectors_correlate(report.x, report.y, state->spike_x, state->spike_y);
    if (confirmed_spike) {
        uint32_t cap = recent_motion ? allowed : IMPRINT_SENSOR_SPIKE_IDLE_COUNTS;
        if (cap < IMPRINT_SENSOR_SPIKE_COUNTS) {
            cap = IMPRINT_SENSOR_SPIKE_COUNTS;
        }
        report = limit_report_manhattan(report, cap);
        IMPRINT_PD_DEBUG("%c spike-confirm x=%d y=%d prev_x=%d prev_y=%d movement=%lu cap=%lu\n", side, report.x,
                         report.y, state->spike_x, state->spike_y, (unsigned long)movement, (unsigned long)cap);
        update_motion_history(state, report);
        return report;
    }

    IMPRINT_PD_DEBUG("%c spike-hold x=%d y=%d movement=%lu last=%lu allowed=%lu aligned=%d recent=%d\n", side, report.x,
                     report.y, (unsigned long)movement, (unsigned long)state->last_movement, (unsigned long)allowed,
                     aligned, recent_motion);
    state->spike_x     = report.x;
    state->spike_y     = report.y;
    state->spike_timer = timer_read32();
    report.x           = 0;
    report.y           = 0;
    return report;
}

static report_mouse_t accept_sensor_motion(report_mouse_t report, sensor_noise_filter_t *state, char side) {
    report = filter_sensor_spike(report, state, side);
    if (report.x || report.y) {
        state->activity_timer = timer_read32();
    }
    return report;
}

static report_mouse_t filter_sensor_noise(report_mouse_t report, sensor_noise_filter_t *state, char side) {
    const uint32_t movement = manhattan_abs_u32(report.x, report.y);

    if (movement == 0) {
        if (state->pending_timer && timer_elapsed32(state->pending_timer) > IMPRINT_SENSOR_NOISE_CONFIRM_MS) {
            IMPRINT_PD_DEBUG("%c noise-expire pending_x=%d pending_y=%d\n", side, state->pending_x, state->pending_y);
            state->pending_x     = 0;
            state->pending_y     = 0;
            state->pending_timer = 0;
        }
        return report;
    }

    const bool is_noise_sized = movement <= IMPRINT_SENSOR_NOISE_COUNTS;
    const bool is_active = state->activity_timer && timer_elapsed32(state->activity_timer) <= IMPRINT_SENSOR_ACTIVE_MS;

    if (!is_noise_sized || is_active) {
        if (state->pending_x || state->pending_y) {
            IMPRINT_PD_DEBUG("%c noise-flush x=%d y=%d pending_x=%d pending_y=%d active=%d\n", side, report.x, report.y,
                             state->pending_x, state->pending_y, is_active);
            report.x = clamp_xy((int32_t)report.x + state->pending_x);
            report.y = clamp_xy((int32_t)report.y + state->pending_y);
        }
        state->pending_x     = 0;
        state->pending_y     = 0;
        state->pending_timer = 0;
        return accept_sensor_motion(report, state, side);
    }

    if (!state->pending_timer || timer_elapsed32(state->pending_timer) > IMPRINT_SENSOR_NOISE_CONFIRM_MS ||
        opposite_sign(report.x, state->pending_x) || opposite_sign(report.y, state->pending_y)) {
        IMPRINT_PD_DEBUG("%c noise-hold x=%d y=%d movement=%lu previous_x=%d previous_y=%d\n", side, report.x, report.y,
                         (unsigned long)movement, state->pending_x, state->pending_y);
        state->pending_x     = report.x;
        state->pending_y     = report.y;
        state->pending_timer = timer_read32();
        report.x             = 0;
        report.y             = 0;
        return report;
    }

    state->pending_x = clamp_xy((int32_t)state->pending_x + report.x);
    state->pending_y = clamp_xy((int32_t)state->pending_y + report.y);

    if (manhattan_abs_u32(state->pending_x, state->pending_y) >= IMPRINT_SENSOR_NOISE_CONFIRM_COUNTS) {
        IMPRINT_PD_DEBUG("%c noise-confirm x=%d y=%d\n", side, state->pending_x, state->pending_y);
        report.x             = state->pending_x;
        report.y             = state->pending_y;
        state->pending_x     = 0;
        state->pending_y     = 0;
        state->pending_timer = 0;
        return accept_sensor_motion(report, state, side);
    } else {
        IMPRINT_PD_DEBUG("%c noise-accumulate pending_x=%d pending_y=%d\n", side, state->pending_x, state->pending_y);
        report.x = 0;
        report.y = 0;
    }

    return report;
}

static uint32_t elapsed_or_one(uint32_t *timer) {
    const uint32_t now     = timer_read32();
    uint32_t       elapsed = *timer ? timer_elapsed32(*timer) : 1;
    *timer                 = now;
    return elapsed == 0 ? 1 : elapsed;
}

static uint32_t right_accel_gain_q8(uint32_t velocity) {
    const uint32_t max_gain_q8 =
        IMPRINT_RIGHT_ACCEL_MAX_GAIN_Q8 < IMPRINT_Q8 ? IMPRINT_Q8 : IMPRINT_RIGHT_ACCEL_MAX_GAIN_Q8;

    if (velocity <= IMPRINT_RIGHT_ACCEL_TAKEOFF || max_gain_q8 == IMPRINT_Q8) {
        return IMPRINT_Q8;
    }

    velocity -= IMPRINT_RIGHT_ACCEL_TAKEOFF;
    if (velocity > 30000U) {
        velocity = 30000U;
    }

    const uint64_t v2       = (uint64_t)velocity * velocity;
    const uint64_t knee2    = (uint64_t)IMPRINT_RIGHT_ACCEL_KNEE * IMPRINT_RIGHT_ACCEL_KNEE;
    const uint32_t curve_q8 = (uint32_t)((v2 * IMPRINT_Q8) / (v2 + knee2));

    return IMPRINT_Q8 + (((max_gain_q8 - IMPRINT_Q8) * curve_q8) / IMPRINT_Q8);
}

static report_mouse_t accelerate_right_report(report_mouse_t report) {
    const int32_t x = report.x;
    const int32_t y = report.y;

    if (x == 0 && y == 0) {
        if (right_accel_timer && timer_elapsed32(right_accel_timer) > IMPRINT_RIGHT_ACCEL_CARRY_TIMEOUT_MS) {
            right_accel_carry_x_q8 = 0;
            right_accel_carry_y_q8 = 0;
        }
        return report;
    }

    const uint32_t elapsed = elapsed_or_one(&right_accel_timer);
    if (elapsed > IMPRINT_RIGHT_ACCEL_CARRY_TIMEOUT_MS || opposite_sign(x, right_accel_carry_x_q8) ||
        opposite_sign(y, right_accel_carry_y_q8)) {
        right_accel_carry_x_q8 = 0;
        right_accel_carry_y_q8 = 0;
    }

    const uint32_t distance = manhattan_abs_u32(x, y);
    const uint32_t velocity = (distance * 1000U) / elapsed;
    const uint32_t gain_q8  = right_accel_gain_q8(velocity);

    const int32_t scaled_x_q8 = right_accel_carry_x_q8 + (x * (int32_t)gain_q8);
    const int32_t scaled_y_q8 = right_accel_carry_y_q8 + (y * (int32_t)gain_q8);
    const int32_t out_x       = scaled_x_q8 / IMPRINT_Q8;
    const int32_t out_y       = scaled_y_q8 / IMPRINT_Q8;

    right_accel_carry_x_q8 = scaled_x_q8 - (out_x * IMPRINT_Q8);
    right_accel_carry_y_q8 = scaled_y_q8 - (out_y * IMPRINT_Q8);
    report.x               = clamp_xy(out_x);
    report.y               = clamp_xy(out_y);
    return report;
}

static uint16_t scroll_divisor(uint16_t base, uint16_t minimum, uint32_t velocity) {
    if (base < minimum) {
        base = minimum;
    }

    uint16_t reduction = (uint16_t)(velocity / IMPRINT_LEFT_SCROLL_ACCEL_STEP);
    if (reduction > (base - minimum)) {
        reduction = base - minimum;
    }
    return base - reduction;
}

static void apply_scroll_axis_lock(int32_t *x, int32_t *y) {
    int32_t ax = abs32(*x);
    int32_t ay = abs32(*y);

    if (ax == 0 && ay == 0) {
        return;
    }

    if (left_scroll_state.timer && timer_elapsed32(left_scroll_state.timer) > IMPRINT_LEFT_SCROLL_IDLE_RESET_MS) {
        left_scroll_state.axis_lock = 0;
    }

    if (left_scroll_state.axis_lock > 0) {
        if ((int64_t)ay * IMPRINT_LEFT_SCROLL_AXIS_LOCK_RATIO_Y >= ax) {
            const int32_t locked = (int32_t)vector_length_u32(*x, *y);
            *y                   = sign32(*y) * locked;
            *x                   = 0;
            return;
        }
        left_scroll_state.axis_lock = 0;
    } else if (left_scroll_state.axis_lock < 0) {
        if ((int64_t)ax * IMPRINT_LEFT_SCROLL_AXIS_LOCK_RATIO_X >= ay) {
            const int32_t locked = (int32_t)vector_length_u32(*x, *y);
            *x                   = sign32(*x) * locked;
            *y                   = 0;
            return;
        }
        left_scroll_state.axis_lock = 0;
    }

    if ((int64_t)ay * IMPRINT_LEFT_SCROLL_AXIS_LOCK_RATIO_Y >= ax) {
        const int32_t locked        = (int32_t)vector_length_u32(*x, *y);
        left_scroll_state.axis_lock = 1;
        *y                          = sign32(*y) * locked;
        *x                          = 0;
    } else if ((int64_t)ax * IMPRINT_LEFT_SCROLL_AXIS_LOCK_RATIO_X >= ay) {
        const int32_t locked        = (int32_t)vector_length_u32(*x, *y);
        left_scroll_state.axis_lock = -1;
        *x                          = sign32(*x) * locked;
        *y                          = 0;
    }
}

static mouse_hv_report_t scroll_axis(int32_t delta, int32_t *remainder_q8, int8_t *direction, uint16_t base_divisor,
                                     uint16_t min_divisor, uint32_t elapsed) {
    const int8_t new_direction = sign32(delta);
    if (new_direction == 0) {
        return 0;
    }

    if (*direction != 0 && *direction != new_direction) {
        *remainder_q8 = 0;
    }
    *direction = new_direction;

    const uint32_t velocity = ((uint32_t)abs32(delta) * 1000U) / elapsed;
    const uint16_t divisor  = scroll_divisor(base_divisor, min_divisor, velocity);

    const int64_t next_q8    = (int64_t)*remainder_q8 + (((int64_t)delta * IMPRINT_Q8) / divisor);
    const int32_t raw_output = (int32_t)(next_q8 / IMPRINT_Q8);

    if (raw_output < MOUSE_REPORT_HV_MIN) {
        *remainder_q8 = 0;
        return MOUSE_REPORT_HV_MIN;
    }
    if (raw_output > MOUSE_REPORT_HV_MAX) {
        *remainder_q8 = 0;
        return MOUSE_REPORT_HV_MAX;
    }

    *remainder_q8 = (int32_t)(next_q8 - ((int64_t)raw_output * IMPRINT_Q8));
    return (mouse_hv_report_t)raw_output;
}

static report_mouse_t scroll_left_report(report_mouse_t report) {
    int32_t x = report.x;
    int32_t y = report.y;

#    if IMPRINT_LEFT_SCROLL_REVERSE_X
    x = -x;
#    endif
#    if IMPRINT_LEFT_SCROLL_REVERSE_Y
    y = -y;
#    endif

    uint32_t elapsed = elapsed_or_one(&left_scroll_state.timer);
    if (elapsed > IMPRINT_LEFT_SCROLL_IDLE_RESET_MS) {
        left_scroll_state.remainder_h_q8 = 0;
        left_scroll_state.remainder_v_q8 = 0;
        left_scroll_state.direction_h    = 0;
        left_scroll_state.direction_v    = 0;
        left_scroll_state.axis_lock      = 0;
        elapsed                          = 1;
    }

    apply_scroll_axis_lock(&x, &y);

    report.h = scroll_axis(x, &left_scroll_state.remainder_h_q8, &left_scroll_state.direction_h,
                           IMPRINT_LEFT_SCROLL_DIVISOR_H, IMPRINT_LEFT_SCROLL_MIN_DIVISOR_H, elapsed);
    report.v = scroll_axis(y, &left_scroll_state.remainder_v_q8, &left_scroll_state.direction_v,
                           IMPRINT_LEFT_SCROLL_DIVISOR_V, IMPRINT_LEFT_SCROLL_MIN_DIVISOR_V, elapsed);
    report.x = 0;
    report.y = 0;
    return report;
}

void pointing_device_init_user(void) {
    pointing_device_set_cpi_on_side(true, IMPRINT_POINTING_LEFT_DEFAULT_CPI);
    pointing_device_set_cpi_on_side(false, IMPRINT_POINTING_RIGHT_DEFAULT_CPI);

    right_accel_timer       = timer_read32();
    left_scroll_state.timer = timer_read32();
}

report_mouse_t pointing_device_task_combined_user(report_mouse_t left_report, report_mouse_t right_report) {
    left_report  = filter_sensor_noise(left_report, &left_noise_filter, 'L');
    right_report = filter_sensor_noise(right_report, &right_noise_filter, 'R');

    left_report  = scroll_left_report(left_report);
    right_report = accelerate_right_report(right_report);

    return pointing_device_combine_reports(left_report, right_report);
}

#endif
