#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <pthread.h>
#include <unistd.h>
#include <io.h>

#include <Mlt.h>
using namespace Mlt;
using namespace std;


mlt_producer create_playlist( int argc, char **argv )
{
   // We're creating a playlist here
   mlt_playlist playlist = mlt_playlist_init( );

   // We need the playlist properties to ensure clean up
   mlt_properties properties = mlt_playlist_properties( playlist );

   // Loop through each of the arguments
   int i = 0;
   for ( i = 1; i < argc; i ++ )
   {
       // Create the producer
       mlt_producer producer = mlt_factory_producer( (mlt_profile) NULL, argv[ i ], NULL );

       // Add it to the playlist
       mlt_playlist_append( playlist, producer );

       // Close the producer (see below)
       mlt_producer_close( producer );
   }

   // Return the playlist as a producer
   return mlt_playlist_producer( playlist );
}


void attach_filter_to_playlist( mlt_playlist playlist ) {
    // Create a new filter
    //mlt_filter filter = mlt_factory_filter( "watermark", "+Hello.txt" );
    mlt_filter filter = mlt_factory_filter( (mlt_profile) NULL, "dynamictext", "tests dynamic text" );

    // Get the service object of the playlist
    mlt_service service = mlt_playlist_service( playlist );

    // Attach the filter
    mlt_service_attach( service, filter );

    // Close the filter
    mlt_filter_close( filter );
}

static void transport_action( mlt_producer producer, char *value ) {

    mlt_properties properties = MLT_PRODUCER_PROPERTIES( producer );
	mlt_multitrack multitrack = (mlt_multitrack) mlt_properties_get_data( properties, "multitrack", NULL );
	mlt_consumer consumer = (mlt_consumer) mlt_properties_get_data( properties, "transport_consumer", NULL );
	mlt_properties jack = (mlt_properties) mlt_properties_get_data( MLT_CONSUMER_PROPERTIES( consumer ), "jack_filter", NULL );

    mlt_position position = producer? mlt_producer_position( producer ) : 0;
    //cout << mlt_producer_get_fps( producer ) << endl;

    if (value=="q") {
        if (consumer) {
            mlt_consumer_stop( consumer );
        }
    }

}

int main( int, char **argv )
{
	Factory::init( NULL );
	
    Profile profile;
    
	Producer producer( profile, argv[ 1 ] );
    //*consumer = create_consumer( profile, "multi" );

    //create a MULTI Consumer....instead of just a consumer
	Consumer consumermulti( profile, "multi" );

    char key[1024];
    int k = 0;

    mlt_properties properties = MLT_CONSUMER_PROPERTIES( consumermulti.get_consumer() );
    mlt_properties_set_data( properties, "transport_callback", (void*)transport_action, 0, NULL, NULL );
    mlt_properties_set_int( properties, "buffer", 1 );
/*
    //nil output ??? : creating new service
    snprintf( key, sizeof(key), "%d", k++ );
    mlt_properties new_props = mlt_properties_new();
    mlt_properties_set_data( properties, key, new_props, 0, (mlt_destructor) mlt_properties_close, NULL );
    mlt_properties_set( new_props, "mlt_service", "sdl" );
*/

    //SDL output: creating new service
    snprintf( key, sizeof(key), "%d", k++ );
    mlt_properties new_props = mlt_properties_new();
    mlt_properties_set_data( properties, key, new_props, 0, (mlt_destructor) mlt_properties_close, NULL );
    mlt_properties_set( new_props, "mlt_service", "sdl" );

    //UDP output: creating new service
    snprintf( key, sizeof(key), "%d", k++ );
    mlt_properties new_props2 = mlt_properties_new();
    mlt_properties_set_data( properties, key, new_props2, 0, (mlt_destructor) mlt_properties_close, NULL );
    mlt_properties_set( new_props2, "mlt_service", "avformat" );
    mlt_properties_set( new_props2, "target", "udp://127.0.0.1:1234" );
    //mlt_properties_parse( new_props2, "s=300x150");
    mlt_properties_parse( new_props2, "vcodec=mpeg2video");
    mlt_properties_parse( new_props2, "acodec=mp2");
    mlt_properties_parse( new_props2, "f=mpeg2ts");
    mlt_properties_parse( new_props2, "b=1200k");
    mlt_properties_parse( new_props2, "ab=64k");
    //mlt_properties_parse( new_props2, "vbuffer=128k");
    //mlt_properties_set_int( properties, "buffer", 50 );

    //connecting to a producer...
	consumermulti.connect( producer );	
    consumermulti.start( );

    // Start the consumer
    //mlt_consumer_start( hello );
    term_init( );

    // Wait for the consumer to terminate
    while( !mlt_consumer_is_stopped( consumermulti.get_consumer() ) ) {
        usleep( 1000 );
        //cout << mlt_producer_get_fps( producer.get_producer() ) << endl;
        cout << mlt_producer_position( producer.get_producer() ) << "/" << mlt_producer_get_length( producer.get_producer() ) << endl;

        int value = term_read( );
		if ( value != -1 )
		{
			char string[ 2 ] = { value, 0 };
			transport_action( producer.get_producer(), string );
		}
    }


    //disconnect
	mlt_consumer_connect( consumermulti.get_consumer(), NULL );
    //close
	mlt_consumer_close( consumermulti.get_consumer() );

	return 0;
}
