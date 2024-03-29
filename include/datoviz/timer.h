/*************************************************************************************************/
/*  Timer                                                                                        */
/*************************************************************************************************/

#ifndef DVZ_HEADER_TIMER
#define DVZ_HEADER_TIMER



/*************************************************************************************************/
/*  Includes                                                                                     */
/*************************************************************************************************/

#include "_list.h"
#include "common.h"



/*************************************************************************************************/
/*  Constants                                                                                    */
/*************************************************************************************************/

#define DVZ_TIMER_MAX_FIRING 16



/*************************************************************************************************/
/*  Enums                                                                                        */
/*************************************************************************************************/



/*************************************************************************************************/
/*  Typedefs                                                                                     */
/*************************************************************************************************/

typedef struct DvzTimer DvzTimer;
typedef struct DvzTimerItem DvzTimerItem;
typedef struct DvzTimerEvent DvzTimerEvent;
typedef struct DvzTimerPayload DvzTimerPayload;

typedef void (*DvzTimerCallback)(DvzTimer* timer, DvzTimerEvent ev);



/*************************************************************************************************/
/*  Structs                                                                                      */
/*************************************************************************************************/

struct DvzTimerItem
{
    DvzTimer* timer;
    uint32_t timer_idx;
    double delay, period;
    uint64_t count, max_count;
    double start_time, last_fire;
    bool is_running;
};



struct DvzTimer
{
    double time;
    DvzList* items;
    uint32_t firing_count;
    DvzTimerItem* firing[DVZ_TIMER_MAX_FIRING];

    DvzList* callbacks;
};



struct DvzTimerEvent
{
    DvzTimerItem* item;
    double time;
    void* user_data;
};



struct DvzTimerPayload
{
    DvzTimerItem* item;
    DvzTimerCallback callback;
    void* user_data;
};



EXTERN_C_ON

/*************************************************************************************************/
/*  Timer functions                                                                              */
/*************************************************************************************************/

DVZ_EXPORT DvzTimer* dvz_timer(void);



DVZ_EXPORT uint32_t dvz_timer_count(DvzTimer* timer);



DVZ_EXPORT DvzTimerItem*
dvz_timer_new(DvzTimer* timer, double delay, double period, uint64_t max_count);



DVZ_EXPORT void dvz_timer_start(DvzTimerItem* item);



DVZ_EXPORT void dvz_timer_pause(DvzTimerItem* item);



DVZ_EXPORT void dvz_timer_remove(DvzTimerItem* item);



DVZ_EXPORT bool dvz_timer_running(DvzTimerItem* item);



DVZ_EXPORT void dvz_timer_tick(DvzTimer* timer, double time);



DVZ_EXPORT DvzTimerItem** dvz_timer_firing(DvzTimer* timer, uint32_t* count);



DVZ_EXPORT void dvz_timer_callback(
    DvzTimer* timer, DvzTimerItem* item, DvzTimerCallback callback, void* user_data);



DVZ_EXPORT void dvz_timer_destroy(DvzTimer* timer);



EXTERN_C_OFF

#endif
