#include QMK_KEYBOARD_H

#if defined(POINTING_DEVICE_ENABLE) && defined(SPLIT_POINTING_ENABLE) && defined(POINTING_DEVICE_COMBINED)

/* =========================================================================
 * Sensor CPI (counts per inch). Drives velocity scaling for every threshold
 * below: velocity = (sensor counts * 1000) / elapsed_ms, so doubling CPI
 * doubles the perceived velocity for the same physical motion.
 * If you change CPI, rescale TAKEOFF / KNEE / VELOCITY_MAX / spike counts
 * roughly proportionally.
 * ========================================================================= */

/* Left-hand sensor CPI. Higher values track finer scroll motion but raise
 * the velocity scale (and thus how soon scroll accel kicks in). */
#    ifndef IMPRINT_POINTING_LEFT_DEFAULT_CPI
#        define IMPRINT_POINTING_LEFT_DEFAULT_CPI 1000
#    endif

/* Right-hand sensor CPI for cursor. Lower values give finer cursor control
 * at low speeds; the accel curve compensates for fast motion. */
#    ifndef IMPRINT_POINTING_RIGHT_DEFAULT_CPI
#        define IMPRINT_POINTING_RIGHT_DEFAULT_CPI 400
#    endif

/* =========================================================================
 * Sensor noise filter. Drops sub-threshold jitter unless it forms a
 * consistent direction within a short window. Bypassed during active motion
 * (see IMPRINT_SENSOR_ACTIVE_MS).
 * ========================================================================= */

/* Manhattan-magnitude threshold below which a sample is "noise-sized" and
 * subject to confirmation. Raise to be more aggressive about filtering tiny
 * sensor jitter; lower to let small motions through immediately. */
#    ifndef IMPRINT_SENSOR_NOISE_COUNTS
#        define IMPRINT_SENSOR_NOISE_COUNTS 1
#    endif

/* Cumulative magnitude required to confirm a held noise candidate. With
 * COUNTS=1 and CONFIRM=2, you need at least two same-direction ticks to
 * release motion. Higher = more conservative. */
#    ifndef IMPRINT_SENSOR_NOISE_CONFIRM_COUNTS
#        define IMPRINT_SENSOR_NOISE_CONFIRM_COUNTS 2
#    endif

/* Time window (ms) for confirming the held noise candidate. If a second
 * agreeing tick doesn't arrive within this window, pending state expires.
 * Slow drift (1 tick / >CONFIRM_MS) will be silently filtered. */
#    ifndef IMPRINT_SENSOR_NOISE_CONFIRM_MS
#        define IMPRINT_SENSOR_NOISE_CONFIRM_MS 15
#    endif

/* Active-motion grace period (ms). While motion is being accepted, the noise
 * filter is bypassed for this long after the last released sample. Higher =
 * smoother feel during continuous motion; too high and re-engaging the noise
 * filter at the end of a stroke takes a while. */
#    ifndef IMPRINT_SENSOR_ACTIVE_MS
#        define IMPRINT_SENSOR_ACTIVE_MS 40
#    endif

/* =========================================================================
 * Sensor spike filter. Catches implausibly large jumps that don't match
 * the recent motion envelope and either caps them or holds them one frame
 * for confirmation.
 * ========================================================================= */

/* Magnitudes below this are never considered spikes and pass through
 * unchecked. Set near the top of "normal motion" range. */
#    ifndef IMPRINT_SENSOR_SPIKE_COUNTS
#        define IMPRINT_SENSOR_SPIKE_COUNTS 32U
#    endif

/* Hard cap used when motion arrives after a quiet period (no recent_motion).
 * The first frame coming out of idle is allowed up to this magnitude. */
#    ifndef IMPRINT_SENSOR_SPIKE_IDLE_COUNTS
#        define IMPRINT_SENSOR_SPIKE_IDLE_COUNTS 80U
#    endif

/* How long after the last accepted motion we still consider the user
 * "active" for spike-envelope purposes. Past this, spike checks fall back
 * to the idle path with the IDLE_COUNTS cap. */
#    ifndef IMPRINT_SENSOR_SPIKE_WINDOW_MS
#        define IMPRINT_SENSOR_SPIKE_WINDOW_MS 20U
#    endif

/* A held suspicious sample must be re-asserted (similar direction, similar
 * magnitude) within this window to be accepted (capped). Past the window,
 * the held candidate is dropped and a new one is held. Larger windows give
 * the filter more chances to catch a sustained spike chain (each held frame
 * extends the chain), reducing teleport feel at the cost of a slightly
 * higher held-frame latency on legitimate fast flicks. */
#    ifndef IMPRINT_SENSOR_SPIKE_CONFIRM_MS
#        define IMPRINT_SENSOR_SPIKE_CONFIRM_MS 20U
#    endif

/* During recent motion, samples up to last_movement * MAX_MULTIPLIER pass
 * through. Larger = more tolerant of acceleration; too large lets the
 * envelope run away after a single spike-confirm — once a frame is capped,
 * each subsequent in-direction frame can grow the envelope by this factor,
 * producing multi-frame teleports during fast flicks.
 *   2 = tight: single-frame escape is bounded to ~2x prior frame; very fast
 *       motion will see 1-2 extra held frames before settling. Best teleport
 *       suppression for high-poll-rate sensors. (default)
 *   3 = compromise.
 *   5 = old default; allows multi-frame teleports under right-side accel. */
#    ifndef IMPRINT_SENSOR_SPIKE_MAX_MULTIPLIER
#        define IMPRINT_SENSOR_SPIKE_MAX_MULTIPLIER 2U
#    endif

/* Additive grace term on top of last_movement * MULT. Allows a small jump
 * even when last_movement is near zero. */
#    ifndef IMPRINT_SENSOR_SPIKE_ACCEL_COUNTS
#        define IMPRINT_SENSOR_SPIKE_ACCEL_COUNTS 16U
#    endif

/* Minimum sample magnitude required to refresh the spike filter's recent-
 * motion history (last_x/last_y/last_movement/last_motion_timer). Samples
 * at or below this threshold pass through the spike filter unchanged but
 * leave the history alone, so e.g. a noise-confirm release of magnitude
 * NOISE_CONFIRM_COUNTS doesn't shrink the spike envelope for the next real
 * motion. Default = NOISE_CONFIRM_COUNTS (2): only the noise-filter floor
 * is suppressed; any genuine motion (≥ 3) is recorded. Raising this floor
 * delays when slow drifts begin to inform the envelope. */
#    ifndef IMPRINT_SENSOR_SPIKE_HISTORY_MIN_COUNTS
#        define IMPRINT_SENSOR_SPIKE_HISTORY_MIN_COUNTS IMPRINT_SENSOR_NOISE_CONFIRM_COUNTS
#    endif

/* Angular threshold for "this sample is moving in the same direction as the
 * reference". Encoded as cos²(θ) ≥ NUMER / DENOM (cross-multiplied to keep
 * the math in integers). Default 1/2 → angle ≤ 45°.
 *   1/2  = 45°  (default, catches near-perpendicular sensor outliers)
 *   3/4  = 30°  (very strict; may flag legitimate sweeping curves)
 *   1/4  = 60°  (loose; allows broader spread before flagging)
 *   0/1  = 90°  (degenerate: equivalent to old dot>0 check)
 * A smaller numerator widens the accepted cone; a larger one tightens it.
 * Used by the spike filter for both initial alignment vs. last accepted
 * motion and for re-asserting a held spike candidate. */
#    ifndef IMPRINT_SENSOR_SPIKE_ALIGN_COS2_NUMER
#        define IMPRINT_SENSOR_SPIKE_ALIGN_COS2_NUMER 1U
#    endif
#    ifndef IMPRINT_SENSOR_SPIKE_ALIGN_COS2_DENOM
#        define IMPRINT_SENSOR_SPIKE_ALIGN_COS2_DENOM 2U
#    endif
#    if (IMPRINT_SENSOR_SPIKE_ALIGN_COS2_DENOM == 0U) || \
        (IMPRINT_SENSOR_SPIKE_ALIGN_COS2_NUMER > IMPRINT_SENSOR_SPIKE_ALIGN_COS2_DENOM)
#        error "IMPRINT_SENSOR_SPIKE_ALIGN_COS2_NUMER must be in [0, DENOM] and DENOM > 0"
#    endif

/* =========================================================================
 * Right-hand cursor acceleration curve.
 *   gain(v) = 1 + (MAX_GAIN - 1) * v² / (v² + KNEE²)   for v > TAKEOFF
 * (clamped at v <= VELOCITY_MAX). Gain is multiplied into each axis using
 * Q-fixed-point math to preserve sub-pixel precision via a per-axis carry.
 *
 * Velocity is the Euclidean magnitude of (x, y) per ms (not Manhattan), so
 * diagonal motion gets the same gain as cardinal motion of equal physical
 * speed. TAKEOFF/KNEE are calibrated against this Euclidean magnitude.
 * ========================================================================= */

/* Velocity below which gain stays at 1.0 (no acceleration). Raise to widen
 * the precision zone; lower to make accel kick in earlier. */
#    ifndef IMPRINT_RIGHT_ACCEL_TAKEOFF
#        define IMPRINT_RIGHT_ACCEL_TAKEOFF 900U
#    endif

/* Half-response point of the curve (where gain ≈ midway between 1 and MAX).
 * Smaller KNEE = sharper transition; larger = smoother ramp. */
#    ifndef IMPRINT_RIGHT_ACCEL_KNEE
#        define IMPRINT_RIGHT_ACCEL_KNEE 3600U
#    endif

/* Velocity clamp before evaluating the curve. Mostly there to keep
 * intermediate math bounded on extreme flicks; rarely needs tuning. */
#    ifndef IMPRINT_RIGHT_ACCEL_VELOCITY_MAX
#        define IMPRINT_RIGHT_ACCEL_VELOCITY_MAX 30000U
#    endif

/* If the cursor sits idle longer than this, the per-axis carry is dropped
 * so the next motion starts from a clean state. */
#    ifndef IMPRINT_RIGHT_ACCEL_CARRY_TIMEOUT_MS
#        define IMPRINT_RIGHT_ACCEL_CARRY_TIMEOUT_MS 120U
#    endif

/* Diagnostic high-water mark. When CONSOLE_ENABLE is on, any post-acceleration
 * cursor frame whose Manhattan magnitude exceeds this threshold is logged via
 * `R big-out`. Set to 0 to disable. Used for hunting teleport events: anything
 * the OS sees as a single jump larger than this is captured along with input,
 * gain, velocity, and the spike-filter state behind the decision. */
#    ifndef IMPRINT_RIGHT_BIG_OUTPUT_COUNTS
#        define IMPRINT_RIGHT_BIG_OUTPUT_COUNTS 60U
#    endif

/* =========================================================================
 * Left-hand scroll acceleration curve.
 *   divisor(v) = base - (base - min) * v² / (v² + KNEE²)   for v > TAKEOFF
 * Larger divisor = more sensor counts needed per scroll tick; the curve
 * smoothly shrinks the divisor with speed, giving "page-flick" behavior.
 * ========================================================================= */

/* Velocity below which the divisor stays at base (slowest scroll). Lower
 * = scroll accel ramps up sooner. */
#    ifndef IMPRINT_LEFT_SCROLL_ACCEL_TAKEOFF
#        define IMPRINT_LEFT_SCROLL_ACCEL_TAKEOFF 600U
#    endif

/* Half-response point. Lower KNEE = scroll reaches max speed faster. */
#    ifndef IMPRINT_LEFT_SCROLL_ACCEL_KNEE
#        define IMPRINT_LEFT_SCROLL_ACCEL_KNEE 4500U
#    endif

/* Velocity clamp; same role as the right-side counterpart. */
#    ifndef IMPRINT_LEFT_SCROLL_ACCEL_VELOCITY_MAX
#        define IMPRINT_LEFT_SCROLL_ACCEL_VELOCITY_MAX 30000U
#    endif

/* =========================================================================
 * Velocity smoothing (single-pole EMA shared by both sides):
 *   smoothed = (prev * (D-N) + cur * N) / D
 * N == D disables smoothing. Smaller N/D ratios (e.g., 1/4) feel "heavier"
 * (more lag, less jitter); larger ratios (e.g., 3/4) are snappier.
 * ========================================================================= */

/* Numerator of the new-sample weight. */
#    ifndef IMPRINT_VELOCITY_SMOOTH_NUMER
#        define IMPRINT_VELOCITY_SMOOTH_NUMER 1U
#    endif

/* Denominator. Default 1/2 = balanced smoothing. */
#    ifndef IMPRINT_VELOCITY_SMOOTH_DENOM
#        define IMPRINT_VELOCITY_SMOOTH_DENOM 2U
#    endif

#    if (IMPRINT_VELOCITY_SMOOTH_DENOM == 0U) || (IMPRINT_VELOCITY_SMOOTH_NUMER == 0U) || \
        (IMPRINT_VELOCITY_SMOOTH_NUMER > IMPRINT_VELOCITY_SMOOTH_DENOM)
#        error "IMPRINT_VELOCITY_SMOOTH_NUMER must be in [1, IMPRINT_VELOCITY_SMOOTH_DENOM] and DENOM > 0"
#    endif

/* =========================================================================
 * Q fixed-point precision used by accel + scroll accumulators.
 * Q12 (default) = 4096 sub-units per integer; Q16 = 65536. Higher = finer
 * sub-tick resolution at the cost of slightly larger intermediate math.
 * Q12 is the right balance for RP2040 (no FPU); Q16 also fits comfortably.
 * ========================================================================= */
#    ifndef IMPRINT_Q_SHIFT
#        define IMPRINT_Q_SHIFT 12U
#    endif

#    if (IMPRINT_Q_SHIFT < 1U) || (IMPRINT_Q_SHIFT > 30U)
#        error "IMPRINT_Q_SHIFT must be between 1 and 30"
#    endif

#    define IMPRINT_Q_ONE   ((int32_t)1 << IMPRINT_Q_SHIFT)
#    define IMPRINT_Q_ONE_U ((uint32_t)1U << IMPRINT_Q_SHIFT)

/* Maximum gain applied at peak velocity, expressed in current-Q units.
 * Default = 11/4 = 2.75x. Legacy: define IMPRINT_RIGHT_ACCEL_MAX_GAIN_Q8
 * (in Q8 units, e.g., 704 = 2.75x) and it will be auto-converted. */
#    ifndef IMPRINT_RIGHT_ACCEL_MAX_GAIN_Q
#        ifdef IMPRINT_RIGHT_ACCEL_MAX_GAIN_Q8
/* Legacy Q8 fallback. The result must fit in uint32_t; guard against
 * misconfiguration where a large legacy Q8 value combined with a large
 * IMPRINT_Q_SHIFT would overflow the runtime type. */
#            if (((uint64_t)(IMPRINT_RIGHT_ACCEL_MAX_GAIN_Q8) * (1ULL << IMPRINT_Q_SHIFT) / 256ULL) > 0xFFFFFFFFULL
#                error "IMPRINT_RIGHT_ACCEL_MAX_GAIN_Q8 overflows uint32_t at the configured IMPRINT_Q_SHIFT; define IMPRINT_RIGHT_ACCEL_MAX_GAIN_Q directly or lower IMPRINT_Q_SHIFT"
#            endif
#            define IMPRINT_RIGHT_ACCEL_MAX_GAIN_Q ((uint32_t)(((uint64_t)(IMPRINT_RIGHT_ACCEL_MAX_GAIN_Q8) * IMPRINT_Q_ONE_U) / 256U))
#        else
#            define IMPRINT_RIGHT_ACCEL_MAX_GAIN_Q ((uint32_t)(((uint64_t)IMPRINT_Q_ONE_U * 11U) / 4U))
#        endif
#    endif

/* =========================================================================
 * Left-hand scroll output stage. Sensor delta is divided by `divisor` (per
 * axis) to yield wheel ticks. The accel curve shrinks the divisor with
 * speed; sub-tick remainders are carried in Q-fixed-point.
 * ========================================================================= */

/* Base (slow) divisor for horizontal wheel. Larger = slower scroll at low
 * speed (more counts per tick). */
#    ifndef IMPRINT_LEFT_SCROLL_DIVISOR_H
#        define IMPRINT_LEFT_SCROLL_DIVISOR_H 128U
#    endif

/* Base (slow) divisor for vertical wheel. */
#    ifndef IMPRINT_LEFT_SCROLL_DIVISOR_V
#        define IMPRINT_LEFT_SCROLL_DIVISOR_V 128U
#    endif

/* Minimum divisor at peak velocity (fastest scroll). Smaller = faster
 * "flick" scrolling. The accel curve interpolates between base and min. */
#    ifndef IMPRINT_LEFT_SCROLL_MIN_DIVISOR_H
#        define IMPRINT_LEFT_SCROLL_MIN_DIVISOR_H 16U
#    endif

#    ifndef IMPRINT_LEFT_SCROLL_MIN_DIVISOR_V
#        define IMPRINT_LEFT_SCROLL_MIN_DIVISOR_V 16U
#    endif

/* Set to 1 to flip the corresponding scroll axis. Useful when the trackball
 * is mounted such that "natural" scroll feels inverted. */
#    ifndef IMPRINT_LEFT_SCROLL_REVERSE_X
#        define IMPRINT_LEFT_SCROLL_REVERSE_X 1
#    endif

#    ifndef IMPRINT_LEFT_SCROLL_REVERSE_Y
#        define IMPRINT_LEFT_SCROLL_REVERSE_Y 1
#    endif

/* =========================================================================
 * Left-hand scroll axis lock. Constrains scrolling to a single axis once
 * the user shows clear intent, suppressing diagonal noise. Acquisition is
 * gated by hysteresis so a single diagonal blip doesn't grab the lock.
 * ========================================================================= */

/* Acquisition: dominant axis must be at least this many times the other
 * for HYSTERESIS_SAMPLES consecutive polls before lock commits. Higher =
 * harder to acquire (purer cardinal motion required). */
#    ifndef IMPRINT_LEFT_SCROLL_AXIS_LOCK_ACQUIRE_RATIO
#        define IMPRINT_LEFT_SCROLL_AXIS_LOCK_ACQUIRE_RATIO 2U
#    endif

/* Number of consecutive qualifying samples needed to commit the lock.
 * 1 = lock instantly on first dominant sample (no hysteresis). 2 is a good
 * trade-off between responsiveness and rejecting transients. Max 254. */
#    ifndef IMPRINT_LEFT_SCROLL_AXIS_LOCK_HYSTERESIS_SAMPLES
#        define IMPRINT_LEFT_SCROLL_AXIS_LOCK_HYSTERESIS_SAMPLES 2U
#    endif

#    if IMPRINT_LEFT_SCROLL_AXIS_LOCK_HYSTERESIS_SAMPLES > 254U
#        error "IMPRINT_LEFT_SCROLL_AXIS_LOCK_HYSTERESIS_SAMPLES must be <= 254"
#    endif

/* Stay-locked stickiness for vertical lock: while Y is locked, motion stays
 * locked as long as ay * STAY_RATIO_Y >= ax. Larger = stickier (harder to
 * break the Y lock with incidental X motion). */
#    ifndef IMPRINT_LEFT_SCROLL_AXIS_LOCK_STAY_RATIO_Y
#        define IMPRINT_LEFT_SCROLL_AXIS_LOCK_STAY_RATIO_Y 1U
#    endif

/* Stay-locked stickiness for horizontal lock. Default 3 makes horizontal
 * lock noticeably stickier than vertical because horizontal scrolls are
 * rarer and we want to forgive incidental vertical motion mid-scroll. */
#    ifndef IMPRINT_LEFT_SCROLL_AXIS_LOCK_STAY_RATIO_X
#        define IMPRINT_LEFT_SCROLL_AXIS_LOCK_STAY_RATIO_X 3U
#    endif

/* Idle reset for the entire left-scroll subsystem. After this much quiet
 * time, all derived state (axis lock, candidate, remainders, smoothed
 * velocity, directions) is cleared so the next motion cold-starts. */
#    ifndef IMPRINT_LEFT_SCROLL_IDLE_RESET_MS
#        define IMPRINT_LEFT_SCROLL_IDLE_RESET_MS 60U
#    endif

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
    int32_t  remainder_h_q;
    int32_t  remainder_v_q;
    uint32_t velocity_smooth;
    int8_t   direction_h;
    int8_t   direction_v;
    int8_t   axis_lock;       /* 0 = none, +1 = locked to Y (vertical), -1 = locked to X (horizontal) */
    int8_t   lock_candidate;  /* proposed lock direction awaiting hysteresis confirmation */
    uint8_t  lock_samples;    /* consecutive samples agreeing with lock_candidate */
    uint32_t timer;
} scroll_state_t;

static sensor_noise_filter_t left_noise_filter;
static sensor_noise_filter_t right_noise_filter;
static scroll_state_t        left_scroll_state;
static int32_t               right_accel_carry_x_q;
static int32_t               right_accel_carry_y_q;
static uint32_t              right_accel_timer;
static uint32_t              right_velocity_smooth;

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

/* Returns true when (x1,y1) and (x2,y2) point in roughly the same direction,
 * i.e. cos²(θ) ≥ ALIGN_COS2_NUMER / ALIGN_COS2_DENOM. The comparison is
 * cross-multiplied to keep everything in integer math:
 *     dot² · DENOM ≥ |a|² · |b|² · NUMER
 * Returns false when either vector is zero (no defined direction) or when
 * the dot product is non-positive (angle > 90°).
 *
 * Overflow note: intermediates fit in uint64 with comfortable margin for
 * realistic per-poll deltas (sensors emit small counts per ms even with
 * MOUSE_EXTENDED_REPORT enabled). Worst-case head-room shrinks if DENOM is
 * raised above the defaults; e.g. with int16 reports near ±32767 and
 * DENOM=4, lhs approaches the uint64 ceiling. Practical sensor traffic
 * never reaches that range, so the bound is informational. */
static inline bool vectors_aligned(int32_t x1, int32_t y1, int32_t x2, int32_t y2) {
    const int64_t dot = ((int64_t)x1 * x2) + ((int64_t)y1 * y2);
    if (dot <= 0) {
        return false;
    }
    const uint64_t cur_sq  = (uint64_t)((int64_t)x1 * x1 + (int64_t)y1 * y1);
    const uint64_t last_sq = (uint64_t)((int64_t)x2 * x2 + (int64_t)y2 * y2);
    if (cur_sq == 0U || last_sq == 0U) {
        return false;
    }
    const uint64_t lhs = (uint64_t)dot * (uint64_t)dot * (uint64_t)IMPRINT_SENSOR_SPIKE_ALIGN_COS2_DENOM;
    const uint64_t rhs = cur_sq * last_sq * (uint64_t)IMPRINT_SENSOR_SPIKE_ALIGN_COS2_NUMER;
    return lhs >= rhs;
}

static inline uint32_t manhattan_abs_u32(int32_t x, int32_t y) {
    return (uint32_t)abs32(x) + (uint32_t)abs32(y);
}

static uint32_t isqrt_u64(uint64_t value) {
    if (value == 0) {
        return 0;
    }

    const uint8_t bit_length = (uint8_t)(64U - (uint8_t)__builtin_clzll(value));
    uint64_t      x          = 1ULL << ((bit_length + 1U) >> 1);

    x = (x + (value / x)) >> 1;
    x = (x + (value / x)) >> 1;
    x = (x + (value / x)) >> 1;

    while (x > (value / x)) {
        --x;
    }

    while ((x + 1U) <= (value / (x + 1U))) {
        ++x;
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

    /* Bumping zeroed axes to ±1 can push the manhattan sum past `limit`
       (the truncated quotients can each round up by 1). Zero the
       smaller-magnitude axis to honor the contract. */
    if ((uint32_t)abs32(sx) + (uint32_t)abs32(sy) > limit) {
        if ((uint32_t)abs32(sx) <= (uint32_t)abs32(sy)) {
            sx = 0;
        } else {
            sy = 0;
        }
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
    const uint32_t mv = manhattan_abs_u32(report.x, report.y);
    /* Only refresh recent-motion history for samples above the configured
       floor (HISTORY_MIN_COUNTS, default NOISE_CONFIRM_COUNTS). Suppresses
       any tiny passing sample — most importantly the noise-confirm release
       of magnitude NOISE_CONFIRM_COUNTS — from shrinking the spike envelope
       (allowed = last_movement * MULT + ACCEL) for the next real motion
       within SPIKE_WINDOW_MS. The spike candidate state is always cleared
       below regardless of magnitude so a tiny accepted sample still
       invalidates a stale held candidate. */
    if (mv > IMPRINT_SENSOR_SPIKE_HISTORY_MIN_COUNTS) {
        state->last_x            = report.x;
        state->last_y            = report.y;
        state->last_movement     = mv;
        state->last_motion_timer = timer_read32();
    }
    state->spike_x     = 0;
    state->spike_y     = 0;
    state->spike_timer = 0;
}

static report_mouse_t filter_sensor_spike(report_mouse_t report, sensor_noise_filter_t *state, char side) {
    /* Drop a stale held-spike candidate so it doesn't linger past its confirm
       window. Harmless functionally (the confirmed_spike check already inspects
       the timer) but keeps state tidy. */
    if (state->spike_timer && timer_elapsed32(state->spike_timer) > IMPRINT_SENSOR_SPIKE_CONFIRM_MS) {
        state->spike_x     = 0;
        state->spike_y     = 0;
        state->spike_timer = 0;
    }

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
        aligned = vectors_aligned(report.x, report.y, state->last_x, state->last_y);

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
                                 vectors_aligned(report.x, report.y, state->spike_x, state->spike_y);
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
    /* Deliberately leave `last_motion_timer`, `last_movement`, and `last_x/y`
       untouched: the held sample is suspicious, not a fact. As a side effect,
       a sustained burst of held spikes lets `last_motion_timer` age out, so
       once SPIKE_WINDOW_MS has elapsed since the last *accepted* sample the
       filter falls into the tighter idle path (cap = IDLE_COUNTS) on the
       very next evaluation. */
    report.x = 0;
    report.y = 0;
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

/*
 * Smooth velocity-response curve: returns 0..IMPRINT_Q_ONE_U.
 * Below `takeoff` the response is 0; above takeoff it ramps as v² / (v² + knee²).
 * `velocity_max` clamps the input to keep intermediate math bounded.
 */
static uint32_t accel_curve_response_q(uint32_t velocity, uint32_t takeoff, uint32_t knee, uint32_t velocity_max) {
    if (velocity <= takeoff || knee == 0U) {
        return 0U;
    }
    velocity -= takeoff;
    if (velocity_max != 0U && velocity > velocity_max) {
        velocity = velocity_max;
    }
    const uint64_t v2    = (uint64_t)velocity * velocity;
    const uint64_t knee2 = (uint64_t)knee * knee;
    return (uint32_t)((v2 * IMPRINT_Q_ONE_U) / (v2 + knee2));
}

static inline uint32_t velocity_smooth_u32(uint32_t prev, uint32_t current) {
#    if IMPRINT_VELOCITY_SMOOTH_NUMER == IMPRINT_VELOCITY_SMOOTH_DENOM
    (void)prev;
    return current;
#    else
    return (uint32_t)(((uint64_t)prev * (IMPRINT_VELOCITY_SMOOTH_DENOM - IMPRINT_VELOCITY_SMOOTH_NUMER) +
                       (uint64_t)current * IMPRINT_VELOCITY_SMOOTH_NUMER) /
                      IMPRINT_VELOCITY_SMOOTH_DENOM);
#    endif
}

static uint32_t right_accel_gain_q(uint32_t velocity) {
    const uint32_t max_gain_q =
        IMPRINT_RIGHT_ACCEL_MAX_GAIN_Q < IMPRINT_Q_ONE_U ? IMPRINT_Q_ONE_U : IMPRINT_RIGHT_ACCEL_MAX_GAIN_Q;

    if (max_gain_q == IMPRINT_Q_ONE_U) {
        return IMPRINT_Q_ONE_U;
    }

    const uint32_t curve_q = accel_curve_response_q(velocity, IMPRINT_RIGHT_ACCEL_TAKEOFF, IMPRINT_RIGHT_ACCEL_KNEE,
                                                    IMPRINT_RIGHT_ACCEL_VELOCITY_MAX);
    if (curve_q == 0U) {
        return IMPRINT_Q_ONE_U;
    }

    const uint64_t boost = ((uint64_t)(max_gain_q - IMPRINT_Q_ONE_U) * curve_q) >> IMPRINT_Q_SHIFT;
    return IMPRINT_Q_ONE_U + (uint32_t)boost;
}

static report_mouse_t accelerate_right_report(report_mouse_t report) {
    const int32_t x = report.x;
    const int32_t y = report.y;

    if (x == 0 && y == 0) {
        if (right_accel_timer && timer_elapsed32(right_accel_timer) > IMPRINT_RIGHT_ACCEL_CARRY_TIMEOUT_MS) {
            right_accel_carry_x_q = 0;
            right_accel_carry_y_q = 0;
            right_velocity_smooth = 0;
        }
        return report;
    }

    const uint32_t elapsed = elapsed_or_one(&right_accel_timer);
    if (elapsed > IMPRINT_RIGHT_ACCEL_CARRY_TIMEOUT_MS || opposite_sign(x, right_accel_carry_x_q) ||
        opposite_sign(y, right_accel_carry_y_q)) {
        right_accel_carry_x_q = 0;
        right_accel_carry_y_q = 0;
        right_velocity_smooth = 0;
    }

    const uint32_t distance = vector_length_u32(x, y);
    const uint32_t velocity = (distance * 1000U) / elapsed;
    /* Cold-start: skip EMA on the very first responsive sample so accel
       isn't artificially halved coming out of idle. */
    right_velocity_smooth =
        right_velocity_smooth ? velocity_smooth_u32(right_velocity_smooth, velocity) : velocity;
    /* Asymmetric gain: use whichever of instant/smoothed velocity is lower.
       During acceleration the EMA lags below instant, so min == smoothed and
       jitter is filtered out. During deceleration the EMA lags above instant,
       so min == instant and gain drops immediately with the slowing motion.
       Without this, post-flick deceleration tremor on the trackball gets
       amplified by the still-high smoothed velocity, producing oscillating
       cursor output that can trigger macOS' "shake to find pointer" feature. */
    const uint32_t gain_velocity = (velocity < right_velocity_smooth) ? velocity : right_velocity_smooth;
    const uint32_t gain_q        = right_accel_gain_q(gain_velocity);

    const int64_t scaled_x_q = (int64_t)right_accel_carry_x_q + ((int64_t)x * (int64_t)gain_q);
    const int64_t scaled_y_q = (int64_t)right_accel_carry_y_q + ((int64_t)y * (int64_t)gain_q);
    const int32_t out_x      = (int32_t)(scaled_x_q / IMPRINT_Q_ONE);
    const int32_t out_y      = (int32_t)(scaled_y_q / IMPRINT_Q_ONE);

    right_accel_carry_x_q = (int32_t)(scaled_x_q - ((int64_t)out_x * IMPRINT_Q_ONE));
    right_accel_carry_y_q = (int32_t)(scaled_y_q - ((int64_t)out_y * IMPRINT_Q_ONE));
    report.x              = clamp_xy(out_x);
    report.y              = clamp_xy(out_y);

#    if defined(CONSOLE_ENABLE) && IMPRINT_RIGHT_BIG_OUTPUT_COUNTS > 0
    {
        const uint32_t out_mag = (uint32_t)abs32(report.x) + (uint32_t)abs32(report.y);
        if (out_mag > IMPRINT_RIGHT_BIG_OUTPUT_COUNTS) {
            const sensor_noise_filter_t *st = &right_noise_filter;
            IMPRINT_PD_DEBUG(
                "R big-out in_x=%d in_y=%d in_mag=%lu out_x=%d out_y=%d out_mag=%lu "
                "vel=%lu gain_q=%lu elapsed=%lu last_x=%d last_y=%d last_mv=%lu\n",
                (int)x, (int)y, (unsigned long)distance, (int)report.x, (int)report.y,
                (unsigned long)out_mag, (unsigned long)right_velocity_smooth, (unsigned long)gain_q,
                (unsigned long)elapsed, (int)st->last_x, (int)st->last_y, (unsigned long)st->last_movement);
        }
    }
#    endif

    return report;
}

/*
 * Smooth scroll divisor: shrinks from `base` down to `minimum` along the
 * shared accel curve. Fast motion -> small divisor -> bigger scroll steps.
 */
static uint16_t scroll_divisor(uint16_t base, uint16_t minimum, uint32_t velocity) {
    if (base <= minimum) {
        return minimum;
    }
    const uint16_t span    = (uint16_t)(base - minimum);
    const uint32_t curve_q = accel_curve_response_q(velocity, IMPRINT_LEFT_SCROLL_ACCEL_TAKEOFF,
                                                    IMPRINT_LEFT_SCROLL_ACCEL_KNEE,
                                                    IMPRINT_LEFT_SCROLL_ACCEL_VELOCITY_MAX);
    if (curve_q == 0U) {
        return base;
    }
    uint32_t reduction = (uint32_t)(((uint64_t)span * curve_q) >> IMPRINT_Q_SHIFT);
    if (reduction > span) {
        reduction = span;
    }
    return (uint16_t)(base - reduction);
}

/*
 * Update axis lock state and project (x,y) onto the locked axis.
 *
 *   - Acquisition requires the dominant axis to exceed the other by a
 *     configurable factor for several consecutive samples (hysteresis),
 *     preventing single-tick diagonal blips from grabbing the lock.
 *   - Release uses a stickier threshold ("stay" ratios) so incidental
 *     off-axis motion does not break an active lock.
 *   - When projecting we keep the dominant component AS-IS (no vector-length
 *     boost). The orthogonal axis is zeroed and its accumulator state is
 *     cleared when the lock direction changes.
 */
static void scroll_clear_axis_state(bool clear_h, bool clear_v) {
    if (clear_h) {
        left_scroll_state.remainder_h_q = 0;
        left_scroll_state.direction_h   = 0;
    }
    if (clear_v) {
        left_scroll_state.remainder_v_q = 0;
        left_scroll_state.direction_v   = 0;
    }
}

static void scroll_commit_lock(int32_t *x, int32_t *y, int8_t direction) {
    if (direction > 0) {
        /* lock to Y: zero X, clear stale H state */
        if (left_scroll_state.axis_lock != 1) {
            scroll_clear_axis_state(true, false);
        }
        left_scroll_state.axis_lock = 1;
        *x                          = 0;
    } else if (direction < 0) {
        /* lock to X: zero Y, clear stale V state */
        if (left_scroll_state.axis_lock != -1) {
            scroll_clear_axis_state(false, true);
        }
        left_scroll_state.axis_lock = -1;
        *y                          = 0;
    }
    left_scroll_state.lock_candidate = 0;
    left_scroll_state.lock_samples   = 0;
}

static void apply_scroll_axis_lock(int32_t *x, int32_t *y) {
    const int32_t ax = abs32(*x);
    const int32_t ay = abs32(*y);

    if (ax == 0 && ay == 0) {
        return;
    }

    /* Stay-locked check first: stickier release thresholds. */
    if (left_scroll_state.axis_lock > 0) {
        if ((int64_t)ay * IMPRINT_LEFT_SCROLL_AXIS_LOCK_STAY_RATIO_Y >= ax) {
            *x = 0;
            return;
        }
        left_scroll_state.axis_lock = 0;
    } else if (left_scroll_state.axis_lock < 0) {
        if ((int64_t)ax * IMPRINT_LEFT_SCROLL_AXIS_LOCK_STAY_RATIO_X >= ay) {
            *y = 0;
            return;
        }
        left_scroll_state.axis_lock = 0;
    }

    /* Acquisition: dominant axis must beat the other by ACQUIRE_RATIO. */
    int8_t proposed = 0;
    if ((int64_t)ay >= (int64_t)ax * IMPRINT_LEFT_SCROLL_AXIS_LOCK_ACQUIRE_RATIO) {
        proposed = 1; /* Y dominant */
    } else if ((int64_t)ax >= (int64_t)ay * IMPRINT_LEFT_SCROLL_AXIS_LOCK_ACQUIRE_RATIO) {
        proposed = -1; /* X dominant */
    }

    if (proposed == 0) {
        left_scroll_state.lock_candidate = 0;
        left_scroll_state.lock_samples   = 0;
        return; /* free motion (both axes pass through) */
    }

    if (left_scroll_state.lock_candidate == proposed) {
        if (left_scroll_state.lock_samples < UINT8_MAX) {
            left_scroll_state.lock_samples++;
        }
    } else {
        left_scroll_state.lock_candidate = proposed;
        left_scroll_state.lock_samples   = 1;
    }

    /* Commit the lock once the candidate has been observed enough times.
       Until then both axes pass through unchanged so the user can still
       resolve ambiguity (free diagonal scroll for ~HYSTERESIS_SAMPLES polls). */
    if (left_scroll_state.lock_samples >= IMPRINT_LEFT_SCROLL_AXIS_LOCK_HYSTERESIS_SAMPLES) {
        scroll_commit_lock(x, y, proposed);
    }
}

static mouse_hv_report_t scroll_axis(int32_t delta, int32_t *remainder_q, int8_t *direction, uint16_t base_divisor,
                                     uint16_t min_divisor, uint32_t velocity) {
    const int8_t new_direction = sign32(delta);
    if (new_direction == 0) {
        return 0;
    }

    if (*direction != 0 && *direction != new_direction) {
        *remainder_q = 0;
    }
    *direction = new_direction;

    const uint16_t divisor = scroll_divisor(base_divisor, min_divisor, velocity);
    if (divisor == 0U) {
        return 0;
    }

    const int64_t next_q     = (int64_t)*remainder_q + (((int64_t)delta * IMPRINT_Q_ONE) / divisor);
    const int32_t raw_output = (int32_t)(next_q / IMPRINT_Q_ONE);

    if (raw_output < MOUSE_REPORT_HV_MIN) {
        *remainder_q = 0;
        return MOUSE_REPORT_HV_MIN;
    }
    if (raw_output > MOUSE_REPORT_HV_MAX) {
        *remainder_q = 0;
        return MOUSE_REPORT_HV_MAX;
    }

    *remainder_q = (int32_t)(next_q - ((int64_t)raw_output * IMPRINT_Q_ONE));
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

    if (x == 0 && y == 0) {
        if (left_scroll_state.timer && timer_elapsed32(left_scroll_state.timer) > IMPRINT_LEFT_SCROLL_IDLE_RESET_MS) {
            left_scroll_state.remainder_h_q   = 0;
            left_scroll_state.remainder_v_q   = 0;
            left_scroll_state.direction_h     = 0;
            left_scroll_state.direction_v     = 0;
            left_scroll_state.axis_lock       = 0;
            left_scroll_state.lock_candidate  = 0;
            left_scroll_state.lock_samples    = 0;
            left_scroll_state.velocity_smooth = 0;
        }
        report.h = 0;
        report.v = 0;
        report.x = 0;
        report.y = 0;
        return report;
    }

    const uint32_t elapsed = elapsed_or_one(&left_scroll_state.timer);
    if (elapsed > IMPRINT_LEFT_SCROLL_IDLE_RESET_MS) {
        left_scroll_state.remainder_h_q   = 0;
        left_scroll_state.remainder_v_q   = 0;
        left_scroll_state.direction_h     = 0;
        left_scroll_state.direction_v     = 0;
        left_scroll_state.axis_lock       = 0;
        left_scroll_state.lock_candidate  = 0;
        left_scroll_state.lock_samples    = 0;
        left_scroll_state.velocity_smooth = 0;
    }

    /* Compute velocity from the full pre-lock motion magnitude (Euclidean)
       so that diagonal flicks get the same accel response as cardinal ones.
       Reaching here implies x|y nonzero (zero-motion path early-returned
       above). After an idle reset, velocity_smooth was cleared, so the very
       first post-idle sample seeds directly from instant_velocity (no EMA
       lag); since `elapsed` is large in that case, instant_velocity will be
       small and the first scroll tick deliberately cold-starts at the slow
       base divisor. Sustained scrolling then ramps up via EMA. */
    const uint32_t pre_lock_distance = vector_length_u32(x, y);
    const uint32_t instant_velocity  = (pre_lock_distance * 1000U) / elapsed;
    /* Cold-start: skip EMA on the very first responsive sample. */
    left_scroll_state.velocity_smooth =
        left_scroll_state.velocity_smooth
            ? velocity_smooth_u32(left_scroll_state.velocity_smooth, instant_velocity)
            : instant_velocity;

    apply_scroll_axis_lock(&x, &y);

    report.h = scroll_axis(x, &left_scroll_state.remainder_h_q, &left_scroll_state.direction_h,
                           IMPRINT_LEFT_SCROLL_DIVISOR_H, IMPRINT_LEFT_SCROLL_MIN_DIVISOR_H,
                           left_scroll_state.velocity_smooth);
    report.v = scroll_axis(y, &left_scroll_state.remainder_v_q, &left_scroll_state.direction_v,
                           IMPRINT_LEFT_SCROLL_DIVISOR_V, IMPRINT_LEFT_SCROLL_MIN_DIVISOR_V,
                           left_scroll_state.velocity_smooth);
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
