addToLibrary( {
    mengineDocumentHasFocus: function()
    {
        return document.hasFocus();
    },

    mengineIsCoarsePointer: function()
    {
        return window.matchMedia( '(pointer: coarse)' ).matches;
    },

    mengineHasTouchPoints: function()
    {
        return navigator.maxTouchPoints > 0;
    },

    mengineIsOnline: function()
    {
        return navigator.onLine;
    },

    mengineOpenUrl__deps: [ '$UTF8ToString' ],
    mengineOpenUrl: function( _url )
    {
        var url = UTF8ToString( _url );
        var opened = window.open( url, '_blank' );

        if( opened === null )
        {
            return false;
        }

        opened.opener = null;

        return true;
    },

    mengineWriteClipboard__deps: [ '$UTF8ToString' ],
    mengineWriteClipboard: function( _text )
    {
        if( navigator.clipboard === undefined )
        {
            return false;
        }

        var text = UTF8ToString( _text );

        navigator.clipboard.writeText( text ).catch( function( _error )
        {
            err( 'clipboard: ' + _error );
        } );

        return true;
    },

    mengineSetupCanvas: function()
    {
        var canvas = document.querySelector( '#canvas' );

        if( canvas.hasAttribute( 'tabindex' ) === true )
        {
            return;
        }

        canvas.tabIndex = 0;
    },

    mengineFocusCanvas: function()
    {
        document.querySelector( '#canvas' ).focus();
    },

    mengineGetCanvasOrigin: function( _left, _top )
    {
        var rect = document.querySelector( '#canvas' ).getBoundingClientRect();

        HEAPF64[_left >> 3] = rect.left;
        HEAPF64[_top >> 3] = rect.top;
    },
} );
