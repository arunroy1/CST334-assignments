//
// Created by Sam Ogden on 8/8/23.
// filled it by Arun Roy on 7/8/25
//

#include "student_code.h"
#include "stdio.h"

/**
 * Called by run(): record first‐time starts
 */
void mark_start(PROCESS* p, SCHEDULER_STATS* stats, float time_started) {
    stats->num_processes_started++;
    stats->sum_of_response_time += (time_started - p->entry_time);
}

/**
 * Called by stop(): record job completions
 */
void mark_end(PROCESS* p, SCHEDULER_STATS* stats, float time_completed) {
    stats->num_processes_completed++;
    stats->sum_of_turnaround_times += (time_completed - p->entry_time);
}

/**
 * After the simulation loop finishes, compute the averages.
 */
void finalize_stats(SCHEDULER_STATS* stats) {
    if (stats->num_processes_completed > 0) {
        stats->average_turnaround_time =
            stats->sum_of_turnaround_times
            / (float)stats->num_processes_completed;
    } else {
        stats->average_turnaround_time = 0.0F;
    }

    if (stats->num_processes_started > 0) {
        stats->average_response_time =
            stats->sum_of_response_time
            / (float)stats->num_processes_started;
    } else {
        stats->average_response_time = 0.0F;
    }
}

/*
 * Comparison helpers for get_minimum()
 */

int compare_noop(PROCESS p1, PROCESS p2) {
    return 0;
}

int compare_priority(PROCESS p1, PROCESS p2) {
    if (p1.priority < p2.priority) {
        return -1;
    }
    if (p1.priority > p2.priority) {
        return  1;
    }
    return 0;
}

int compare_arrival_time(PROCESS p1, PROCESS p2) {
    if (p1.entry_time < p2.entry_time) {
        return -1;
    }
    if (p1.entry_time > p2.entry_time) {
        return  1;
    }
    return 0;
}

int compare_arrival_time_invert(PROCESS p1, PROCESS p2) {
    // LIFO: newest arrival first
    return compare_arrival_time(p2, p1);
}

int compare_job_duration(PROCESS p1, PROCESS p2) {
    if (p1.duration < p2.duration) {
        return -1;
    }
    if (p1.duration > p2.duration) {
        return  1;
    }
    return 0;
}

int compare_time_remaining(PROCESS p1, PROCESS p2) {
    if (p1.time_remaining < p2.time_remaining) {
        return -1;
    }
    if (p1.time_remaining > p2.time_remaining) {
        return  1;
    }
    return 0;
}

int compare_last_run(PROCESS p1, PROCESS p2) {
    // Round-robin: pick the one that ran longest ago
    if (p1.last_run < p2.last_run) {
        return -1;
    }
    if (p1.last_run > p2.last_run) {
        return  1;
    }
    return 0;
}

/*
 * Six selectors, each just calls get_minimum with the right compare function.
 */

PROCESS* fifo_process_selector(PROCESS_LIST* pl) {
    return get_minimum(pl, compare_arrival_time);
}

PROCESS* rr_process_selector(PROCESS_LIST* pl) {
    return get_minimum(pl, compare_last_run);
}

PROCESS* sjf_process_selector(PROCESS_LIST* pl) {
    return get_minimum(pl, compare_job_duration);
}

PROCESS* priority_process_selector(PROCESS_LIST* pl) {
    return get_minimum(pl, compare_priority);
}

PROCESS* lifo_process_selector(PROCESS_LIST* pl) {
    return get_minimum(pl, compare_arrival_time_invert);
}

PROCESS* stcf_process_selector(PROCESS_LIST* pl) {
    return get_minimum(pl, compare_time_remaining);
}