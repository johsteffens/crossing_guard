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

#include "guard.h"

/**********************************************************************************************************************/
// date

static sc_t dmy_s_def = "dmy_s = "
"{"
    "aware_t _;  "
    "sz_t day;   "
    "sz_t month; "
    "sz_t year;  "
"}";

BCORE_DEFINE_FUNCTIONS_SELF_OBJECT_INST( dmy_s, dmy_s_def )

st_s* string_from_date( const dmy_s* o )
{
    return st_s_createf( "%02zu.%02zu.%04zu", o->day, o->month, o->year );
}

dmy_s* date_from_string( const st_s* string )
{
    dmy_s* dt = dmy_s_create();
    st_s_parse_fa( string, 0, -1, "#sz_t.#sz_t.#sz_t", &dt->day, &dt->month, &dt->year );
    dmy_s_check_plausibility( dt );
    return dt;
}

st_s* string_from_cday( sz_t cday )
{
    dmy_s* dt = dmy_s_from_cday( cday );
    st_s* s = string_from_date( dt );
    dmy_s_discard( dt );
    return s;
}

st_s* l_string_from_cday( bcore_life_s* l, sz_t cday )
{
    st_s* s = string_from_cday( cday );
    bcore_life_s_push_aware( l, s );
    return s;
}

dmy_s* date_from_sc( sc_t sc )
{
    st_s* s = st_s_create_sc( sc );
    dmy_s* dt = date_from_string( s );
    st_s_discard( s );
    return dt;
}

void dmy_s_check_plausibility( const dmy_s* o )
{
    ASSERT( ( o->year >= 1900 ) && ( o->year < 2100 ) );
    switch( o->month )
    {
        case  1: ASSERT( o->day <= 31 ); break;
        case  2: ASSERT( o->day <= 29 ); break;
        case  3: ASSERT( o->day <= 31 ); break;
        case  4: ASSERT( o->day <= 30 ); break;
        case  5: ASSERT( o->day <= 31 ); break;
        case  6: ASSERT( o->day <= 30 ); break;
        case  7: ASSERT( o->day <= 31 ); break;
        case  8: ASSERT( o->day <= 31 ); break;
        case  9: ASSERT( o->day <= 30 ); break;
        case 10: ASSERT( o->day <= 31 ); break;
        case 11: ASSERT( o->day <= 30 ); break;
        case 12: ASSERT( o->day <= 31 ); break;
        default: ERR( "invalid month '%lu'", o->month );
    }
}

sz_t cday_from_dmy_s( const dmy_s* o )
{
    dmy_s_check_plausibility( o );

    sz_t yr = o->year - 1900;
    sz_t leap_days = 1 + ( yr / 4 );
    if( ( o->month < 3 ) && ( yr % 4 == 0 ) ) leap_days--;

    sz_t sum = yr * 365 + leap_days;

    switch( o->month - 1 )
    {
        case 11: sum += 30;
        case 10: sum += 31;
        case  9: sum += 30;
        case  8: sum += 31;
        case  7: sum += 31;
        case  6: sum += 30;
        case  5: sum += 31;
        case  4: sum += 30;
        case  3: sum += 31;
        case  2: sum += 28;
        case  1: sum += 31;
        case  0:
        default: break;
    }

    sum += o->day - 1;

    if( sum < 60 ) ERR( "lowest possible cday: '0' is mapped to 01.03.1900" );
    sum -= 60;

    return sum;
}

sz_t cday_from_dmy_sc( sc_t sc )
{
    dmy_s* dt = date_from_sc( sc );
    sz_t cday = cday_from_dmy_s( dt );
    dmy_s_discard( dt );
    return cday;
}

dmy_s dmy_from_cday( sz_t cd )
{
    sz_t lyr = cd / ( 365 * 4 + 1 );
    sz_t ldy = cd - lyr * ( 365 * 4 + 1 );
    sz_t qyr = ldy / 365;
    qyr = ( qyr > 3 ) ? 3 : qyr;
    sz_t yr = ( lyr * 4 ) + qyr;
    sz_t dy = ldy - qyr * 365;
    sz_t mt = 3;

    sz_t dtbl[] = { 31, 30, 31, 30, 31, 31, 30, 31, 30, 31, 31 };

    for( sz_t i = 0; i < 11; i++ )
    {
        if( dtbl[ i ] > dy ) break;
        dy -= dtbl[ i ];
        mt++;
    }

    dmy_s o;
    dmy_s_init( &o );
    o.day   = dy + 1;
    o.month = ( mt <= 12 ) ? mt : mt - 12;
    o.year  = 1900 + yr + ( ( mt <= 12 ) ? 0 : 1 );
    return o;
}

dmy_s* dmy_s_from_cday( sz_t cd )
{
    dmy_s* o  = dmy_s_create();
    *o = dmy_from_cday( cd );
    return o;
}

sz_t wday_from_cday( sz_t cd )
{
    return ( cd + 3 ) % 7;
}

sz_t wday_from_date( const dmy_s* dt )
{
    return wday_from_cday( cday_from_dmy_s( dt ) );
}

sz_t wday_from_dmy_sc( sc_t sc )
{
    dmy_s* dt = date_from_sc( sc );
    sz_t wday = wday_from_date( dt );
    dmy_s_discard( dt );
    return wday;
}

sc_t sc_from_wday( sz_t wday )
{
    switch( wday )
    {
        case 0: return "mo";
        case 1: return "tu";
        case 2: return "we";
        case 3: return "th";
        case 4: return "fr";
        case 5: return "sa";
        case 6: return "su";
        default: ERR( "invalid weekday '%zu'", wday );
    }
    return "";
}

sz_t wday_from_sc( sc_t sc )
{
    if( bcore_strcmp( sc, "mo" ) == 0 ) return 0;
    if( bcore_strcmp( sc, "tu" ) == 0 ) return 1;
    if( bcore_strcmp( sc, "we" ) == 0 ) return 2;
    if( bcore_strcmp( sc, "th" ) == 0 ) return 3;
    if( bcore_strcmp( sc, "fr" ) == 0 ) return 4;
    if( bcore_strcmp( sc, "sa" ) == 0 ) return 5;
    if( bcore_strcmp( sc, "su" ) == 0 ) return 6;
    ERR( "invalid weekday string '%s'", sc );
    return 0;
}

sz_t wcnt_from_cday( sz_t cday )
{
    return ( cday + 3 ) / 7;
}

sz_t wnum_from_date( const dmy_s* dt )
{
    dmy_s dt_l;
    dmy_s_init( &dt_l );
    dt_l.year  = dt->year;
    dt_l.month = 1;
    dt_l.day   = 1;

    sz_t first_cday = cday_from_dmy_s( &dt_l );
    sz_t first_wday = wday_from_cday( first_cday );

    // monday of week of first_cday
    sz_t mo1_cday = first_cday - first_wday;
    if( first_wday > 3 ) mo1_cday += 7;

    sz_t cur_cday = cday_from_dmy_s( dt );

    if( cur_cday < mo1_cday ) // last week of last year
    {
        if( first_wday == 4 ) return 53;
        if( first_wday == 5 ) return ( ( dt_l.year - 1 ) % 4 ) == 0 ? 53 : 52;
        return 52;
    }
    else
    {
        sz_t wnum = 1 + ( cur_cday - mo1_cday ) / 7;
        if( wnum == 53 )
        {
            if( ( first_wday == 3 ) || ( ( ( dt_l.year % 4 ) == 0 ) && ( first_wday == 2 ) ) )
            {
                return 53;
            }
            else
            {
                return 1;
            }
        }
        return wnum;
    }
}

sz_t wnum_from_cday( sz_t cday )
{
    dmy_s* dt = dmy_s_from_cday( cday );
    sz_t wnum = wnum_from_date( dt );
    dmy_s_discard( dt );
    return wnum;
}

sz_t wnum_from_dmy_sc( sc_t sc )
{
    dmy_s* dt = date_from_sc( sc );
    sz_t wnum = wnum_from_date( dt );
    dmy_s_discard( dt );
    return wnum;
}

/**********************************************************************************************************************/

static sc_t date_s_def = "date_s = "
"{"
    "shell st_s;"
    "hidden sz_t cday;"
    "func bcore_fp_get        get_       = date_s_get_;"
    "func bcore_fp_set        set_       = date_s_set_;"
    "func bcore_fp_copy_typed copy_typed = date_s_copy_typed;"
"}";

BCORE_DEFINE_FUNCTIONS_SELF_OBJECT_INST( date_s, date_s_def )

static void date_s_set_( vd_t obj, sr_s string )
{
    assert( sr_s_type( &string ) == TYPEOF_st_s );
    date_s* o = obj;
    const st_s* s = string.o;
    o->cday = cday_from_dmy_sc( s->sc );
    sr_down( string );
}

static sr_s date_s_get_( vc_t obj )
{
    const date_s* o = obj;
    st_s* s = string_from_cday( o->cday );
    return sr_asd( s );
}

static void date_s_copy_typed( date_s* o, tp_t type, vc_t src )
{
    if( type == TYPEOF_sz_t )
    {
        o->cday = *( const sz_t* )src;
    }
    else
    {
        ERR( "Cannot convert %s to date_s.", ifnameof( type ) );
    }
}

/**********************************************************************************************************************/

static sc_t date_arr_s_def = "date_arr_s = "
"{"
    "aware_t _;      "
    "date_s [] arr;"
"}";

BCORE_DEFINE_FUNCTIONS_SELF_OBJECT_INST( date_arr_s, date_arr_s_def )

bl_t date_arr_s_matches( const date_arr_s* o, sz_t cday )
{
    for( sz_t i = 0; i < o->size; i++ )
    {
        if( o->data[ i ].cday == cday ) return true;
    }
    return false;
}

/**********************************************************************************************************************/

static sc_t period_s_def = "period_s = "
"{"
    "aware_t _;    "
    "date_s first; "
    "date_s last;  "
"}";

BCORE_DEFINE_FUNCTIONS_SELF_OBJECT_INST( period_s, period_s_def )

bl_t period_s_inside( const period_s* o, sz_t cday )
{
    return ( cday >= o->first.cday && cday <= o->last.cday );
}

/**********************************************************************************************************************/

static sc_t period_arr_s_def = "period_arr_s = "
"{"
    "aware_t _;      "
    "period_s [] arr;"
"}";

BCORE_DEFINE_FUNCTIONS_SELF_OBJECT_INST( period_arr_s, period_arr_s_def )

bl_t period_arr_s_inside( const period_arr_s* o, sz_t cday )
{
    for( sz_t i = 0; i < o->size; i++ )
    {
        if( period_s_inside( &o->data[ i ], cday ) ) return true;
    }
    return false;
}

/**********************************************************************************************************************/

static sc_t weekday_availability_s_def = "weekday_availability_s = "
"{"
    "aware_t _;                      "
    "hidden u0_t weekday_flags = 31; " // mo: &1, tu: &2, we: &4, ...
    "shell st_s weekdays;            " // "mo, tu, we, .."
    "sz_t weekly_period        = 1;  " // 1: each week, 2 every other week, 3 every third week, etc
    "sz_t including_week       = 1;  " // this week number is included in period;

    "func bcore_fp_get get_weekdays = weekday_availability_s_get_weekdays;"
    "func bcore_fp_set set_weekdays = weekday_availability_s_set_weekdays;"
"}";

BCORE_DEFINE_FUNCTIONS_SELF_OBJECT_INST( weekday_availability_s, weekday_availability_s_def )

static void weekday_availability_s_set_weekdays( vd_t obj, sr_s src )
{
    weekday_availability_s* o = obj;
    st_s* s = src.o;
    sz_t idx = 0;
    sz_t count = 1;
    o->weekday_flags = 0;
    while( idx < s->size )
    {
        if( idx > 0 )
        {
            idx = st_s_parse_fa( s, idx, -1, " , " );
        }
        else
        {
            idx = st_s_parse_fa( s, idx, -1, " " );
        }

        sz_t wday = 7;
        for( sz_t i = 0; i < 7; i++ )
        {
            sc_t wday_sc = sc_from_wday( i );
            if( ( bcore_strcmp( wday_sc, s->sc + idx ) & ~1 ) == 0 ) wday = i;
            if( wday < 7 )
            {
                idx += bcore_strlen( wday_sc );
                break;
            }
        }

        if( wday == 7 ) ERR( "Cannot parse weekday %zu in string \"%s\"", count, s->sc );
        o->weekday_flags = o->weekday_flags | ( 1 << wday );
        count++;
        idx = st_s_parse_fa( s, idx, -1, " " );
    }
    sr_down( src );
}

static sr_s weekday_availability_s_get_weekdays( vc_t obj )
{
    const weekday_availability_s* o = obj;
    st_s* s = st_s_create();
    sz_t count = 0;
    for( sz_t i = 0; i < 7; i++ )
    {
        if( o->weekday_flags & ( 1 << i ) )
        {
            if( count > 0 ) st_s_push_sc( s, ", " );
            st_s_push_sc( s, sc_from_wday( i ) );
            count++;
        }
    }
    return sr_asd( s );
}

static bl_t weekday_availability_s_match( const weekday_availability_s* o, sz_t cday, sz_t wnum )
{
    sz_t wday = wday_from_cday( cday );
    if( ( o->weekday_flags & ( 1 << wday ) ) == 0 ) return false;
    if( o->weekly_period <= 1 ) return true;
    sz_t wdiff = wnum > o->including_week ? wnum - o->including_week : o->including_week - wnum;
    if( wdiff % o->weekly_period == 0 ) return true;
    return false;
}

/**********************************************************************************************************************/

static sc_t preferences_s_def = "preferences_s = "
"{"
    "aware_t _;             "
    "f3_t weight = 1.0;     "
    "weekday_availability_s weekday_availability;"
    "bl_t         always_same_workday = true;"
    "date_arr_s   included_dates;"
    "date_arr_s   excluded_dates;"
    "period_arr_s excluded_periods;"
"}";

BCORE_DEFINE_FUNCTIONS_SELF_OBJECT_INST( preferences_s, preferences_s_def )

static f3_t preferences_s_match( const preferences_s* o, sz_t cday, sz_t wnum )
{
    if( date_arr_s_matches( &o->excluded_dates, cday      ) ) return 0;
    if( period_arr_s_inside( &o->excluded_periods, cday   ) ) return 0;
    if( weekday_availability_s_match( &o->weekday_availability, cday, wnum ) ) return o->weight;
    if( date_arr_s_matches( &o->included_dates, cday      ) ) return o->weight;
    return 0;
}

/**********************************************************************************************************************/

static sc_t person_s_def = "person_s = "
"{"
    "aware_t _;                            "
    "st_s name;                            "
    "preferences_s  preferences;           "
    "hidden sz_t    assigned_nweekday = 7; " // 7 means no specific day assigned
    "shell  st_s    assigned_weekday;      "
    "date_arr_s     assigned_dates;        "

    "func bcore_fp_get get_assigned_weekday = person_s_get_assigned_weekday;"
    "func bcore_fp_set set_assigned_weekday = person_s_set_assigned_weekday;"
"}";

BCORE_DEFINE_FUNCTIONS_SELF_OBJECT_INST( person_s, person_s_def )

static void person_s_set_assigned_weekday( vd_t obj, sr_s wd )
{
    person_s* o = obj;
    st_s* wd_st = wd.o;
    if( st_s_equal_sc( wd_st, "any" ) )
    {
        o->assigned_nweekday = 7;
    }
    else
    {
        o->assigned_nweekday = wday_from_sc( wd_st->sc );
    }
    sr_down( wd );
}

static sr_s person_s_get_assigned_weekday( vc_t obj )
{
    const person_s* o = obj;
    st_s* wd_st;
    if( o->assigned_nweekday < 7 )
    {
        wd_st = st_s_create_sc( sc_from_wday( o->assigned_nweekday ) );
    }
    else
    {
        wd_st = st_s_create_sc( "any" );
    }
    return sr_asd( wd_st );
}

static f3_t person_s_match( const person_s* o, sz_t cday, sz_t wnum )
{
    sz_t wday = wday_from_cday( cday );
    if( o->assigned_nweekday < 7 && o->assigned_nweekday != wday ) return 0;

    f3_t weight = preferences_s_match( &o->preferences, cday, wnum );
    f3_t score = 1.0;
    if( o->assigned_dates.size > 0 )
    {
        sz_t last_assg = o->assigned_dates.data[ o->assigned_dates.size - 1 ].cday;
        // diff: distance from last assignment in weeks
        f3_t diff = ( f3_t )( ( cday > last_assg ) ? ( cday - last_assg ) : 0 );
        diff *= weight;
        f3_t sqr_diff = diff * diff;
        score = sqr_diff / ( sqr_diff + 1 );
    }
    return score;
}

/**********************************************************************************************************************/

static sc_t assignment_s_def = "assignment_s = "
"{"
    "aware_t _;       "
    "person_s * [] arr; "
"}";

BCORE_DEFINE_FUNCTIONS_SELF_OBJECT_INST( assignment_s, assignment_s_def )

static sz_t assignment_s_get_assignment_index( const assignment_s* o, sz_t cday )
{
    for( sz_t i = 0; i < o->size; i++ )
    {
        person_s* p = o->data[ i ];
        date_arr_s* a = &p->assigned_dates;
        for( sz_t j = 0 ;j < a->size; j++ ) if( a->data[ j ].cday == cday ) return i;
    }
    return o->size;
}

/**********************************************************************************************************************/

static sc_t assigner_s_def = "assigner_s = "
"{"
    "aware_t _; "
    "bl_t mo = true;"
    "bl_t tu = true;"
    "bl_t we = true;"
    "bl_t th = true;"
    "bl_t fr = true;"
    "bl_t sa = false;"
    "bl_t su = false;"
    "period_arr_s vacation_arr;"
    "date_arr_s holidays;"
    "u2_t rseed = 12345; "
    "sz_t cycles = 128;  "
"}";

BCORE_DEFINE_FUNCTIONS_SELF_OBJECT_INST( assigner_s, assigner_s_def )

// returns 0 in case of schoolday, 1 in case of vacation, 2 in case of holiday
static s2_t assigner_s_free_day( const assigner_s* o, sz_t cday )
{
    if( period_arr_s_inside( &o->vacation_arr, cday ) ) return 1;
    if( date_arr_s_matches( &o->holidays, cday ) ) return 2;
    return 0;
}

static assignment_s* assigner_s_try_assignment( const assigner_s* o, const assignment_s* src, period_s period, u2_t rseed, f3_t* score )
{
    f3_t score_l = 0;
    sz_t count = 0;
    sz_t fail_count = 0;

    assignment_s* assignment = assignment_s_clone( src );

    {
        bcore_arr_sz_s* permutation = bcore_arr_sz_s_create_random_permutation( bcore_xsg_u2, rseed, assignment->size );
        bcore_array_a_reorder( (bcore_array*)assignment, permutation );
        bcore_arr_sz_s_discard( permutation );
    }

    for( sz_t cday = period.first.cday; cday <= period.last.cday; cday++ )
    {
        if( assigner_s_free_day( o, cday ) ) continue;

        sz_t wday = wday_from_cday( cday );
        sz_t wnum = wnum_from_cday( cday );

        if( !( &o->mo )[ wday ] ) continue;
        f3_t best_match = 0;
        sz_t best_idx = -1;
        for( sz_t i = 0; i < assignment->size; i++ )
        {
            person_s* p = assignment->data[ i ];
            f3_t match = person_s_match( p, cday, wnum );
            if( match > best_match )
            {
                best_match = match;
                best_idx = i;
            }
        }
        if( best_idx < assignment->size )
        {
            person_s* p = assignment->data[ best_idx ];
            bcore_array_a_push( (bcore_array*)&p->assigned_dates, sr_sz( cday ) );
            if( p->assigned_nweekday >= 7 && p->preferences.always_same_workday ) p->assigned_nweekday = wday;
            score_l += best_match;
        }
        else
        {
            fail_count++;
        }
        count++;
    }

    score_l = ( count > 0 ) ? ( score_l / count ) - fail_count : 0;
    if( score ) *score = score_l;
    return assignment;
}

assignment_s* assigner_s_create_assignment( const assigner_s* o, const assignment_s* src, period_s period )
{
    assignment_s* best_assignment = assignment_s_clone( src );
    f3_t best_score = -1E20;
    u2_t rval = bcore_xsg3_u2( o->rseed );
    for( sz_t i = 0; i < o->cycles; i++ )
    {
        f3_t score = 0;
        rval = bcore_xsg3_u2( rval );
        assignment_s* try_assignment = assigner_s_try_assignment( o, src, period, rval, &score );
        if( score > best_score )
        {
            best_score = score;
            assignment_s_discard( best_assignment );
            best_assignment = try_assignment;
        }
        else
        {
            assignment_s_discard( try_assignment );
        }
    }
    return best_assignment;
}

st_s* assigner_s_show_days( const assigner_s* o, const assignment_s* src, period_s period )
{
    st_s* log = st_s_create();
    st_s* st_first = string_from_cday( period.first.cday );
    st_s* st_last  = string_from_cday( period.last.cday );
    st_s_pushf( log, "Period %s - %s\n", st_first->sc, st_last->sc );
    st_s_discard( st_first );
    st_s_discard( st_last );

    for( sz_t cday = period.first.cday; cday <= period.last.cday; cday++ )
    {
        sz_t wday = wday_from_cday( cday );
        if( wday == 0 ) st_s_pushf( log, "\n" );
        if( !( &o->mo )[ wday ] ) continue;
        st_s* dmy_string = string_from_cday( cday );
        st_s_pushf( log, "    (W%02zu) %s %s: ", wnum_from_cday( cday ), sc_from_wday( wday ), dmy_string->sc );
        st_s_discard( dmy_string );
        sz_t idx = assignment_s_get_assignment_index( src, cday );
        if( idx < src->size )
        {
            person_s* p = src->data[ idx ];
            st_s_pushf( log, "%s\n", p->name.sc );
        }
        else
        {
            switch( assigner_s_free_day( o, cday ) )
            {
                case 0: st_s_pushf( log, "#### FAILURE ####\n" ); break;
                case 1: st_s_pushf( log, "VACATION\n" ); break;
                case 2: st_s_pushf( log, "HOLIDAY\n" ); break;
                default: break;
            }
        }
    }
    return log;
}

st_s* assigner_s_show_persons( const assigner_s* o, const assignment_s* src )
{
    st_s* log = st_s_create();
    assignment_s* assignment = assignment_s_clone( src );
    bcore_array_a_sort( (bcore_array*)assignment, 0, -1, 1 );

    for( sz_t i = 0; i < assignment->size; i++ )
    {
        const person_s* person = assignment->data[ i ];
        st_s_pushf( log, "%s", person->name.sc );
        if( person->assigned_nweekday < 7 )
        {
            st_s_pushf( log, " (%s)", sc_from_wday( person->assigned_nweekday ) );
        }
        st_s_pushf( log, ":", person->name.sc );
        for( sz_t j = 0; j < person->assigned_dates.size; j++ )
        {
            if( j > 0 ) st_s_pushf( log, ", " );
            if( j % 4 == 0 ) st_s_pushf( log, "\n    " );
            sz_t cday = person->assigned_dates.data[ j ].cday;
            st_s_push_st_d( log, string_from_cday( cday ) );
        }
        st_s_pushf( log, "\n\n" );
    }
    assignment_s_discard( assignment );
    return log;
}

st_s* assigner_s_show_calendar( const assigner_s* o, const assignment_s* src, period_s period )
{
    st_s* log = st_s_create();
    assignment_s* assignment = assignment_s_clone( src );

   sz_t name_space = 0;

    sc_t week_sc = "MTWTFSS";

    for( sz_t i = 0; i < assignment->size; i++ )
    {
        const person_s* person = assignment->data[ i ];
        name_space = ( person->name.size > name_space ) ? person->name.size : name_space;
    }

    bcore_life_s* l = bcore_life_s_create();
    st_s* st_first = string_from_cday( period.first.cday );
    st_s* st_last  = string_from_cday( period.last.cday );
    st_s_push_fa( log, "Period: #sc_t - #sc_t\n", st_first->sc, st_last->sc );
    st_s_discard( st_first );
    st_s_discard( st_last );

    sz_t cdays     = ( period.last.cday - period.first.cday + 1 );
    sz_t gap_width = 7;
    sz_t cal_start = name_space + gap_width;

    st_s* wdat_row = bcore_life_s_push_aware( l, st_s_create() );
    st_s* wnum_row = bcore_life_s_push_aware( l, st_s_create() );
    st_s* wday_row = bcore_life_s_push_aware( l, st_s_create() );

    st_s_push_char_n( wdat_row, ' ', cal_start );
    st_s_push_char_n( wnum_row, ' ', cal_start );
    st_s_push_char_n( wday_row, ' ', cal_start );

    bcore_arr_st_s* pers_arr = bcore_life_s_push_aware( l, bcore_arr_st_s_create() );
    for( sz_t i = 0; i < assignment->size; i++ )
    {
        st_s* s = bcore_arr_st_s_push_sc( pers_arr, NULL );
        st_s_push_fa( s, "#pn {#sc_t}", name_space, assignment->data[ i ]->name.sc );
        sz_t awday = assignment->data[ i ]->assigned_nweekday;
        st_s_push_fa( s, "#pl5 {(#sc_t)}:#pn {}", awday < 7 ? sc_from_wday( awday ) : "**" );
        st_s_push_fa( s, "#pn {}", cal_start - s->size );
    }

    sz_t fail_count = 0;
    for( sz_t i = 0; i < cdays; i++ )
    {
        sz_t cday = period.first.cday + i;
        sz_t wday = wday_from_cday( cday );
        bl_t regular_wday = ( &o->mo )[ wday ];
        s2_t free_type = assigner_s_free_day( o, cday );
        sz_t assignment_idx = assignment_s_get_assignment_index( assignment, cday );
        bl_t not_assigned = ( assignment_idx < assignment->size ) ? false : true;
        bl_t failure = ( regular_wday && ( free_type == 0 ) && not_assigned );
        fail_count += failure;

        if( wday == 0 )
        {
            st_s* date_st = string_from_cday( cday );
            sz_t wnum = wnum_from_cday( cday );
            st_s_push_sc( wday_row, "|" );
            st_s_push_fa( wnum_row, "|W#sz_t", wnum );
            st_s_push_fa( wdat_row, "|#t6{#<st_s*>}", date_st );
            st_s_discard( date_st );
            for( sz_t i = 0; i < pers_arr->size; i++ ) st_s_push_char( pers_arr->data[ i ], '|' );
        }

        st_s_push_char( wday_row, week_sc[ wday ] );

        for( sz_t i = 0; i < pers_arr->size; i++ )
        {
            bl_t match = date_arr_s_matches( &assignment->data[ i ]->assigned_dates, cday );
            st_s* s = pers_arr->data[ i ];
            char c = match ? '!' : ( free_type > 0 ) ? ' ' : regular_wday ? ( failure ? '#' : '-' ) : ' ';
            st_s_push_char( s, c );
        }

        if( wday_row->size > wnum_row->size ) st_s_push_char( wnum_row, ' ' );
        if( wday_row->size > wdat_row->size ) st_s_push_char( wdat_row, ' ' );
        for( sz_t i = 0; i < pers_arr->size; i++ )
        {
            st_s* s = pers_arr->data[ i ];
            if( wday_row->size > s->size ) st_s_push_char( s, ' ' );
        }
    }

    st_s* sepr_row = bcore_life_s_push_aware( l, st_s_create_fa( "#rn{-}", wdat_row->size + 1 ) );

    st_s_push_fa( log, "#<st_s*>\n", sepr_row );
    st_s_push_fa( log, "#<st_s*>\n", wnum_row );
    st_s_push_fa( log, "#<st_s*>\n", wdat_row );
    st_s_push_fa( log, "#<st_s*>\n", wday_row );
    st_s_push_fa( log, "#<st_s*>\n", sepr_row );
    for( sz_t i = 0; i < pers_arr->size; i++ ) st_s_push_fa( log, "#<st_s*>\n", pers_arr->data[ i ] );
    st_s_push_fa( log, "#<st_s*>\n", sepr_row );
    st_s_push_fa( log, "#sz_t missing assignments\n", fail_count );

    bcore_life_s_discard( l );

    assignment_s_discard( assignment );
    return log;
}

/**********************************************************************************************************************/

st_s* assigner_s_show_html_table( const assigner_s* o, const assignment_s* src, period_s period )
{
    st_s* log = st_s_create();

    sc_t month_list[] = { "Jan", "Feb", "M&auml;r", "Apr", "Mai", "Jun", "Jul", "Aug", "Sep", "Okt", "Nov", "Dez" };

    st_s_push_fa( log, "<!doctype html>\n" );
    st_s_push_fa( log, "<html lang=\"de\">\n" );
    st_s_push_fa( log, "<head>\n" );
    st_s_push_fa( log, "<meta charset=\"utf-8\">\n" );
    st_s_push_fa( log, "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n" );
    st_s_push_fa( log, "<title>Crossing Guard Table</title>\n" );

    st_s_push_fa( log, "<style>\n" );

    st_s_push_fa( log, "table { border-collapse: separate; border-spacing: 0.2em; background-color: ##c0c0c0; }" );
    st_s_push_fa( log, "td {  text-align: center; background-color: ##ffffff; padding: 0.2em; }" );
    st_s_push_fa( log, "th {  text-align: center; background-color: ##f0f0f0; padding: 0.5em; }" );

    st_s_push_fa( log, "</style>\n" );

    st_s_push_fa( log, "</head>\n" );


    st_s_push_fa( log, "<body>\n" );

    assignment_s* assignment = assignment_s_clone( src );

    bcore_life_s* l = bcore_life_s_create();
    st_s_push_fa( log, "<h2>#<st_s*> - #<st_s*></h2>\n",
                  l_string_from_cday( l, period.first.cday ),
                  l_string_from_cday( l, period.last.cday ) );

    sz_t start_cday = period.first.cday - wday_from_cday( period.first.cday ); // mo of first week
    sz_t end_cday = period.last.cday - wday_from_cday( period.last.cday ) + 6; // su of last week

    st_s_push_fa( log, "<table>\n" );
    st_s_push_fa( log, "<thead>\n" );
    st_s_push_fa( log, "<tr>" );
    st_s_push_fa( log, "<th> Woche </th>" );
    if( o->mo ) st_s_push_fa( log, "<th> Montag </th>" );
    if( o->tu ) st_s_push_fa( log, "<th> Dienstag </th>" );
    if( o->we ) st_s_push_fa( log, "<th> Mittwoch </th>" );
    if( o->th ) st_s_push_fa( log, "<th> Donnerstag </th>" );
    if( o->fr ) st_s_push_fa( log, "<th> Freitag </th>" );
    if( o->sa ) st_s_push_fa( log, "<th> Samstag </th>" );
    if( o->su ) st_s_push_fa( log, "<th> Sonntag </th>" );
    st_s_push_fa( log, "<th></th>" );

    st_s_push_fa( log, "</tr>\n" );
    st_s_push_fa( log, "</thead>\n" );

    st_s_push_fa( log, "<tbody>\n" );
    sz_t mnum1 = 0;
    sz_t mnum2 = 0;
    for( sz_t cday = start_cday; cday <= end_cday; cday++ )
    {
        sz_t wday = wday_from_cday( cday );
        dmy_s dmy = dmy_from_cday( cday );
        sz_t wnum = wnum_from_cday( cday );

        if( wday == 0 )
        {
            st_s_push_fa( log, "<tr style=\"background-color: #sc_t;\">", ( wnum & 1 ) ? "#ffffff" : "#f8f8f8" );
            st_s_push_fa( log, "<td style=\"background-color: #sc_t;\">", "#f0f0f0" );
            st_s_push_fa( log, "#sz_t", wnum );
            st_s_push_fa( log, "</td>" );
            mnum1 = 0;
            mnum2 = 0;
        }

        if( ( &o->mo )[ wday ] )
        {
            if( !mnum1 )
            {
                mnum1 = dmy.month;
            }
            else if( mnum1 != dmy.month )
            {
                mnum2 = dmy.month;
            }

            sz_t idx = assignment_s_get_assignment_index( src, cday );
            s2_t free_type = ( idx == src->size ) ? assigner_s_free_day( o, cday ) : 0;

            bl_t regular = idx < src->size;
            bl_t vacation = free_type == 1;
            bl_t holiday = free_type == 2;
            bl_t outside = ( cday < period.first.cday || cday > period.last.cday );
            bl_t failure = ( !outside ) && ( !regular ) && ( free_type == 0 );

            if( outside )
            {
                st_s_push_fa( log, "<td style=\"background-color: #sc_t;\">", "#f0f080" );
            }
            else if( regular )
            {
                st_s_push_fa( log, "<td>" );
            }
            else if( vacation )
            {
                st_s_push_fa( log, "<td style=\"background-color: #sc_t;\">", "#f0f0ff" );
            }
            else if( holiday )
            {
                st_s_push_fa( log, "<td style=\"background-color: #sc_t;\">", "#f0f0ff" );
            }
            else if( failure )
            {
                st_s_push_fa( log, "<td style=\"background-color: #sc_t;\">", "#f08080" );
            }

            st_s_push_fa( log, "#sz_t", dmy.day );

            if( idx < src->size )
            {
                person_s* p = src->data[ idx ];
                st_s_push_fa( log, "<div><b>#sc_t</b></div>", p->name.sc );
            }
            else
            {
                if( cday >= period.first.cday && cday <= period.last.cday )
                {
                    switch( free_type )
                    {
                        case 0: st_s_push_fa( log, "<div style=\"color: ##00ffff; background-color: ##ff0000;\"><b>Fehlt</b></div>" ); break;
                        case 1: st_s_push_fa( log, "<div>Ferien</div>" ); break;
                        case 2: st_s_push_fa( log, "<div>Schulfrei</div>" ); break;
                    }
                }
            }
            st_s_push_fa( log, "</td>" );
        }

        if( wday == 6 )
        {
            st_s_push_fa( log, "<td style=\"background-color: #sc_t;\">", "#f0f0f0" );
            if( mnum1 && !mnum2 )
            {
                st_s_push_fa( log, "<div>#sc_t</div><div>#sz_t</div>", month_list[ mnum1 - 1 ], dmy.year );
            }
            else
            {
                if( mnum2 == 1 )
                {
                    st_s_push_fa( log, "<div>#sc_t #sz_t/</div><div>#sc_t #sz_t</div>", month_list[ mnum1 - 1 ], dmy.year - 1, month_list[ mnum2 - 1 ], dmy.year );
                }
                else
                {
                    st_s_push_fa( log, "<div>#sc_t/#sc_t</div><div>#sz_t</div>", month_list[ mnum1 - 1 ], month_list[ mnum2 - 1 ], dmy.year );
                }
            }

            st_s_push_fa( log, "</td>" );
        }
    }
    st_s_push_fa( log, "</tbody>\n" );

    st_s_push_fa( log, "</table>\n" );

    bcore_life_s_discard( l );

    assignment_s_discard( assignment );


    st_s_push_fa( log, "<body>\n" );
    st_s_push_fa( log, "</body>\n" );
    st_s_push_fa( log, "</html>\n" );

    return log;
}

/**********************************************************************************************************************/

st_s* guard_selftest( void )
{
    st_s* log = st_s_create();
    bcore_life_s* l = bcore_life_s_create();

    for( sz_t cday = 0; cday < 60000; cday++ )
    {
        dmy_s* dt = dmy_s_from_cday( cday );
        sz_t cday2 = cday_from_dmy_s( dt );
        ASSERT( cday == cday2 );
        dmy_s_discard( dt );
    }

    {
        st_s* string = bcore_life_s_push_aware
        (
            l,
            string_from_date
            (
                bcore_life_s_push_aware( l, date_from_sc( "24.05.2018" ) )
            )
        );
        ASSERT( st_s_cmp_sc( string, "24.05.2018" ) == 0 );
    }

    {
        st_s* string = bcore_life_s_push_aware
        (
            l,
            string_from_date
            (
                bcore_life_s_push_aware( l, date_from_sc( "29.02.2012" ) )
            )
        );
        ASSERT( st_s_cmp_sc( string, "29.02.2012" ) == 0 );
    }

    {
        sz_t cday = 0;
        st_s* s = string_from_cday( cday );
        st_s_pushf( log, "cday %zu is %s, %s\n", cday, sc_from_wday( wday_from_cday( cday ) ), s->sc );
        st_s_discard( s );
    }
    {
        sc_t sc = "11.06.1969";
        st_s_pushf( log, "%s is %s in week %zu\n", sc, sc_from_wday( wday_from_dmy_sc( sc ) ), wnum_from_dmy_sc( sc ) );
    }
    {
        sc_t sc = "11.06.2017";
        st_s_pushf( log, "%s is %s in week %zu\n", sc, sc_from_wday( wday_from_dmy_sc( sc ) ), wnum_from_dmy_sc( sc ) );
    }
    {
        sc_t sc = "11.06.2018";
        st_s_pushf( log, "%s is %s in week %zu\n", sc, sc_from_wday( wday_from_dmy_sc( sc ) ), wnum_from_dmy_sc( sc ) );
    }
    {
        sc_t sc = "01.01.2012";
        st_s_pushf( log, "%s is %s in week %zu\n", sc, sc_from_wday( wday_from_dmy_sc( sc ) ), wnum_from_dmy_sc( sc ) );
    }
    {
        sc_t sc = "31.12.2012";
        st_s_pushf( log, "%s is %s in week %zu\n", sc, sc_from_wday( wday_from_dmy_sc( sc ) ), wnum_from_dmy_sc( sc ) );
    }
    {
        sc_t sc = "31.12.2020";
        st_s_pushf( log, "%s is %s in week %zu\n", sc, sc_from_wday( wday_from_dmy_sc( sc ) ), wnum_from_dmy_sc( sc ) );
    }
    {
        sc_t sc = "01.01.2027";
        st_s_pushf( log, "%s is %s in week %zu\n", sc, sc_from_wday( wday_from_dmy_sc( sc ) ), wnum_from_dmy_sc( sc ) );
    }
    {
        sc_t sc = "09.01.2027";
        st_s_pushf( log, "%s is %s in week %zu\n", sc, sc_from_wday( wday_from_dmy_sc( sc ) ), wnum_from_dmy_sc( sc ) );
    }

    bcore_life_s_discard( l );

    return log;
}

/**********************************************************************************************************************/

vd_t guard_signal_handler( const bcore_signal_s* o )
{
    switch( bcore_signal_s_handle_type( o, typeof( "guard" ) ) )
    {
        case TYPEOF_init1:
        {
            BCORE_REGISTER_OBJECT( dmy_s );
            BCORE_REGISTER_OBJECT( date_s );
            BCORE_REGISTER_FUNC(  date_s_set_ );
            BCORE_REGISTER_FUNC(  date_s_get_ );
            BCORE_REGISTER_FUNC(  date_s_copy_typed );
            BCORE_REGISTER_OBJECT( date_arr_s );
            BCORE_REGISTER_OBJECT( period_s );
            BCORE_REGISTER_OBJECT( period_arr_s );
            BCORE_REGISTER_OBJECT( weekday_availability_s );
            BCORE_REGISTER_FUNC(  weekday_availability_s_get_weekdays );
            BCORE_REGISTER_FUNC(  weekday_availability_s_set_weekdays );
            BCORE_REGISTER_OBJECT( preferences_s );
            BCORE_REGISTER_OBJECT( person_s );
            BCORE_REGISTER_FUNC(  person_s_get_assigned_weekday );
            BCORE_REGISTER_FUNC(  person_s_set_assigned_weekday );
            BCORE_REGISTER_OBJECT( assignment_s );
            BCORE_REGISTER_OBJECT( assigner_s );
        }
        break;

        case TYPEOF_selftest:
        {
            st_s_print_d( guard_selftest() );
        }
        break;

        default: break;
    }
    return NULL;
}
