Module.preRun = Module.preRun || [];
Module.preRun.push( function()
{
    FS.mkdirTree( '/user' );
    FS.mount( IDBFS, { autoPersist: true }, '/user' );

    addRunDependency( 'mengine-user' );

    FS.syncfs( true, function( _error )
    {
        if( _error )
        {
            err( 'user storage: ' + _error );
        }

        removeRunDependency( 'mengine-user' );
    } );
} );
