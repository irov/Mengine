addToLibrary( {
    mengineLocalStorageGetItem__deps: [ '$UTF8ToString', '$stringToUTF8' ],
    mengineLocalStorageGetItem: function( _key, _value, _capacity )
    {
        try
        {
            var key = UTF8ToString( _key );
            var value = window.localStorage.getItem( key );

            if( value === null )
            {
                return false;
            }

            stringToUTF8( value, _value, _capacity );

            return true;
        }
        catch( _error )
        {
            err( 'localStorage: ' + _error );

            return false;
        }
    },

    mengineLocalStorageSetItem__deps: [ '$UTF8ToString' ],
    mengineLocalStorageSetItem: function( _key, _value )
    {
        try
        {
            var key = UTF8ToString( _key );
            var value = UTF8ToString( _value );

            window.localStorage.setItem( key, value );
        }
        catch( _error )
        {
            err( 'localStorage: ' + _error );
        }
    },
} );
