/** Copyright 2018 Johannes Bernhard Steffens
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#include "bcore_std.h"

/**********************************************************************************************************************/
// dmy_s

#define TYPEOF_dmy_s typeof( "dmy_s" )
typedef struct dmy_s
{
    aware_t _;
    sz_t day;
    sz_t month;
    sz_t year;
} dmy_s;

BCORE_DECLARE_FUNCTIONS_OBJ( dmy_s )

void dmy_s_check_plausibility( const dmy_s* o );
st_s*   string_from_dmy_s( const dmy_s* o );
st_s*   string_from_cday( sz_t cday );
st_s* l_string_from_cday( bcore_life_s* l, sz_t cday );
dmy_s* dmy_s_from_string( const st_s* string );
dmy_s* dmy_s_from_sc( sc_t sc );
sz_t   cday_from_dmy_s( const dmy_s* o );
sz_t   cday_from_dmy_sc( sc_t sc );  // format "07.09.2023"
dmy_s* dmy_s_from_cday( sz_t cd );
dmy_s  dmy_from_cday( sz_t cd );

/// wday == weekday: 0 = monday, ..., 6 = sunday
sz_t wday_from_cday( sz_t cd );
sz_t wday_from_date( const dmy_s* dt );
sz_t wday_from_dmy_sc( sc_t sc ); // format "07.09.2023"
sc_t sc_from_wday( sz_t wday );
sz_t wday_from_sc( sc_t sc );

/// wcnt == weekcounter (week 1 starts with first monday on cday)
sz_t wcnt_from_cday( sz_t cday );

/// wnum == weeknumber according to ISO 8601
sz_t wnum_from_date( const dmy_s* dt );
sz_t wnum_from_cday( sz_t cday );
sz_t wnum_from_dmy_sc( sc_t sc );

/**********************************************************************************************************************/

#define TYPEOF_date_s typeof( "date_s" )
typedef struct date_s
{
    sz_t cday;
} date_s;

BCORE_DECLARE_OBJECT( date_s )

/**********************************************************************************************************************/

#define TYPEOF_date_arr_s typeof( "date_arr_s" )
typedef struct date_arr_s
{
    aware_t _;
    union
    {
        bcore_array_dyn_solid_static_s arr;
        struct
        {
            date_s* data;
            sz_t size, space;
        };
    };
} date_arr_s;

BCORE_DECLARE_OBJECT( date_arr_s )

/**********************************************************************************************************************/

#define TYPEOF_period_s typeof( "period_s" )
typedef struct period_s
{
    aware_t _;
    date_s first;
    date_s last;
} period_s;

BCORE_DECLARE_OBJECT( period_s )

bl_t period_s_inside( const period_s* o, sz_t cday );

/**********************************************************************************************************************/

#define TYPEOF_period_arr_s typeof( "period_arr_s" )
typedef struct period_arr_s
{
    aware_t _;
    union
    {
        bcore_array_dyn_solid_static_s arr;
        struct
        {
            period_s* data;
            sz_t size, space;
        };
    };
} period_arr_s;

BCORE_DECLARE_OBJECT( period_arr_s )

/**********************************************************************************************************************/

#define TYPEOF_weekday_availability_s typeof( "weekday_availability_s" )
typedef struct weekday_availability_s
{
    aware_t _;
    u0_t weekday_flags;  // mo: &1, tu: &2, we: &4, ...
    sz_t weekly_period;  // 1: each week, 2 every other week, 3 every third week, etc
    sz_t including_week; // this week number is included in period;

} weekday_availability_s;

BCORE_DECLARE_OBJECT( weekday_availability_s )

/**********************************************************************************************************************/

#define TYPEOF_preferences_s typeof( "preferences_s" )
typedef struct preferences_s
{
    aware_t _;
    f3_t weight;
    weekday_availability_s weekday_availability;
    bl_t         always_same_workday;
    date_arr_s   included_dates;
    date_arr_s   excluded_dates;    // exclusions supersede inclusions
    period_arr_s excluded_periods;  // exclusions supersede inclusions
} preferences_s;

BCORE_DECLARE_OBJECT( preferences_s )

/**********************************************************************************************************************/

#define TYPEOF_person_s typeof( "person_s" )
typedef struct person_s
{
    aware_t _;
    st_s name;
    preferences_s  preferences;
    sz_t           assigned_nweekday;
    date_arr_s     assigned_dates;
} person_s;

BCORE_DECLARE_OBJECT( person_s )

/**********************************************************************************************************************/

#define TYPEOF_assignment_s typeof( "assignment_s" )
typedef struct assignment_s
{
    aware_t _;
    union
    {
        bcore_array_dyn_link_static_s arr;
        struct
        {
            person_s** data;
            sz_t size, space;
        };
    };

} assignment_s;

BCORE_DECLARE_OBJECT( assignment_s )

/**********************************************************************************************************************/

#define TYPEOF_assigner_s typeof( "assigner_s" )
typedef struct assigner_s
{
    aware_t _;
    bl_t mo;
    bl_t tu;
    bl_t we;
    bl_t th;
    bl_t fr;
    bl_t sa;
    bl_t su;

    period_arr_s vacation_arr;
    date_arr_s holidays;

    u2_t rseed;
    sz_t cycles;

} assigner_s;

BCORE_DECLARE_OBJECT( assigner_s )

assignment_s* assigner_s_create_assignment( const assigner_s* o, const assignment_s* src, period_s period );

st_s* assigner_s_show_days(       const assigner_s* o, const assignment_s* src, period_s period );
st_s* assigner_s_show_persons(    const assigner_s* o, const assignment_s* src );
st_s* assigner_s_show_calendar(   const assigner_s* o, const assignment_s* src, period_s period );
st_s* assigner_s_show_html_table( const assigner_s* o, const assignment_s* src, period_s period );

/**********************************************************************************************************************/

vd_t guard_signal_handler( const bcore_signal_s* o );

/**********************************************************************************************************************/
