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
#include "guard.h"

void selftest()
{
    bcore_run_signal_selftest( typeof( "all" ), NULL );

    sr_s pl = bcore_inst_typed_create_sr( typeof( "assignment_s" ) );
    bcore_array_q_push( &pl, bcore_inst_typed_create_sr( typeof( "person_s" ) ) );
    bcore_txt_ml_to_stdout( pl );

    bcore_txt_ml_to_stdout( bcore_inst_typed_create_sr( typeof( "assigner_s" ) ) );
}

st_s* process( const st_s* assigner_file, const st_s* assignment_file, period_s period, const st_s* output_format )
{
    st_s* log = st_s_create();
    bcore_life_s* l = bcore_life_s_create();
    sr_s assigner_sr = bcore_life_s_push_sr( l, bcore_txt_ml_from_file( assigner_file->sc ) );
    if( sr_s_type( &assigner_sr ) != typeof( "assigner_s" ) )
    {
        ERR( "Incorrect file '%s'. <assigner_s> expected.", assigner_file );
    }

    sr_s assignment_sr = bcore_life_s_push_sr( l, bcore_txt_ml_from_file( assignment_file->sc ) );
    if( sr_s_type( &assignment_sr ) != typeof( "assignment_s" ) )
    {
        ERR( "Incorrect file '%s'. <assignment_s> expected.", assignment_sr );
    }

    assignment_s* final_assignment = assigner_s_create_assignment( assigner_sr.o, assignment_sr.o, period );
    bcore_life_s_push_aware( l, final_assignment );

    if( st_s_equal_sc( output_format, "dates" ) )
    {
        st_s_push_sc( log, "#### Assignments by date: ####\n\n" );
        st_s_push_st_d( log, assigner_s_show_days( assigner_sr.o, final_assignment, period ) );
    }
    else if( st_s_equal_sc( output_format, "persons" ) )
    {
        st_s_push_sc( log, "\n#### Assignments by person: ####\n\n" );
        st_s_push_st_d( log, assigner_s_show_persons( assigner_sr.o, final_assignment ) );
    }
    else if( st_s_equal_sc( output_format, "assignment" ) )
    {
        bcore_txt_ml_to_stdout( sr_awc( final_assignment ) );
    }
    else if( st_s_equal_sc( output_format, "calendar" ) )
    {
        st_s_push_sc( log, "\n#### Assignment Calendar: ####\n\n" );
        st_s_push_st_d( log, assigner_s_show_calendar( assigner_sr.o, final_assignment, period ) );
    }
    else if( st_s_equal_sc( output_format, "html_table" ) )
    {
        st_s_push_st_d( log, assigner_s_show_html_table( assigner_sr.o, final_assignment, period ) );
    }
    else
    {
        ERR( "Unknown output format '%s' (choose from {dates|persons|assignment|calendar|html_table}", output_format );
    }

    bcore_life_s_discard( l );

    return log;
}

int main( int argc, const char** argv )
{
    bcore_register_signal_handler( guard_signal_handler );
    bcore_msg( "Crossing Guard - Copyright (C) 2017 Johannes B. Steffens\n" );

    if( argc < 5 )
    {
        bcore_msg( "No input.\n" );
        bcore_msg( "Usage:\n" );
        bcore_msg( "   crossing_guard <config_file> <assignment_file> <start_date> <end_date> [-show {dates|persons|assignment|calendar} ]\n" );
        bcore_msg( "Example: crossing_guard config.txt assignment.txt -show persons\n" );
    }
    else
    {
        bcore_life_s* l = bcore_life_s_create();
        st_s* config_file     = bcore_life_s_push_aware( l, st_s_create_sc( argv[ 1 ] ) );
        st_s* assignment_file = bcore_life_s_push_aware( l, st_s_create_sc( argv[ 2 ] ) );
        st_s* first_date      = bcore_life_s_push_aware( l, st_s_create_sc( argv[ 3 ] ) );
        st_s* last_date       = bcore_life_s_push_aware( l, st_s_create_sc( argv[ 4 ] ) );
        period_s period;
        period.first.cday = cday_from_dmy_sc( first_date->sc );
        period.last.cday  = cday_from_dmy_sc( last_date->sc );

        st_s* output_format = NULL;
        st_s* output_file = NULL;
        for( sz_t i = 5; i < argc; i++ )
        {
            if( bcore_strcmp( argv[ i ], "-format" ) == 0 )
            {
                if( ++i == argc ) break;
                output_format = bcore_life_s_push_aware( l, st_s_create_sc( argv[ i ] ) );
            }
            else if( bcore_strcmp( argv[ i ], "-file" ) == 0 )
            {
                if( ++i == argc ) break;
                output_file = bcore_life_s_push_aware( l, st_s_create_sc( argv[ i ] ) );
            }
            else
            {
                ERR( "Invalid Argument '%s'", argv[ i ] );
            }
        }

        st_s* out = bcore_life_s_push_aware( l, process( config_file, assignment_file, period, output_format ) );
        if( output_file )
        {
            vd_t file = bcore_life_s_push_aware( l, bcore_sink_create_file( output_file->sc ) );
            bcore_sink_aware_push_string( file, out );
        }
        else
        {
            st_s_print( out );
        }

        bcore_life_s_discard( l );
    }

    bcore_down( false );
    return 0;
}
