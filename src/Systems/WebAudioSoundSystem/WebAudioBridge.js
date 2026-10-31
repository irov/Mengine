addToLibrary( {
    $MengineAudio: {
        STATE_STOPPED: 0,
        STATE_PLAYING: 1,
        STATE_PAUSED: 2,
        STATE_FINISHED: 3,
        PLAYERS_POOL_SIZE: 8,
        context: null,
        buffers: new Map(),
        streams: new Map(),
        sources: new Map(),
        players: [],
        nextId: 1,
        unlock: null,

        createPlayer: function()
        {
            var element = new Audio();
            element.preload = 'auto';

            var media = MengineAudio.context.createMediaElementSource( element );

            return { element: element, media: media };
        },

        fillPlayers: function()
        {
            while( MengineAudio.players.length < MengineAudio.PLAYERS_POOL_SIZE )
            {
                var player = MengineAudio.createPlayer();
                player.element.load();

                MengineAudio.players.push( player );
            }
        },

        content: function( _source )
        {
            if( _source.stream !== null )
            {
                return _source.stream;
            }

            return _source.buffer;
        },

        position: function( _source )
        {
            if( _source.element !== null )
            {
                if( _source.state !== MengineAudio.STATE_PLAYING )
                {
                    return _source.offset;
                }

                return _source.element.currentTime;
            }

            var offset = _source.offset;

            if( _source.state === MengineAudio.STATE_PLAYING )
            {
                offset += MengineAudio.context.currentTime - _source.started;
            }

            if( _source.buffer === null )
            {
                return 0;
            }

            var duration = _source.buffer.duration;
            var position = _source.loop === true ? offset % duration: Math.min( offset, duration );

            return position;
        },

        detach: function( _source )
        {
            if( _source.element !== null )
            {
                _source.blocked = false;
                _source.element.pause();

                return;
            }

            if( _source.node === null )
            {
                return;
            }

            _source.node.onended = null;
            _source.node.stop();
            _source.node.disconnect();
            _source.gain.disconnect();
            _source.node = null;
            _source.gain = null;
        },

        attachStream: function( _source, _stream )
        {
            var context = MengineAudio.context;
            var player = MengineAudio.players.pop();

            if( player === undefined )
            {
                player = MengineAudio.createPlayer();
            }

            var element = player.element;
            element.src = _stream.url;

            var media = player.media;
            var gain = context.createGain();
            gain.gain.value = _source.volume * _source.volume;
            media.connect( gain );
            gain.connect( context.destination );

            element.onended = function()
            {
                if( _source.element !== element )
                {
                    return;
                }

                _source.state = MengineAudio.STATE_FINISHED;
                _source.offset = _stream.duration;
            };

            element.onerror = function()
            {
                if( _source.element !== element )
                {
                    return;
                }

                err( 'WebAudio stream: ' + element.error.code + ' ' + element.error.message );

                _source.state = MengineAudio.STATE_FINISHED;
                _source.offset = _stream.duration;
            };

            _source.stream = _stream;
            _source.element = element;
            _source.media = media;
            _source.gain = gain;
        },

        releaseStream: function( _source )
        {
            if( _source.element === null )
            {
                return;
            }

            var element = _source.element;
            element.onended = null;
            element.onerror = null;
            element.pause();
            element.removeAttribute( 'src' );
            element.load();

            _source.media.disconnect();
            _source.gain.disconnect();

            if( MengineAudio.players.length < MengineAudio.PLAYERS_POOL_SIZE )
            {
                MengineAudio.players.push( { element: element, media: _source.media } );
            }

            _source.stream = null;
            _source.element = null;
            _source.media = null;
            _source.gain = null;
            _source.blocked = false;
        },

        playStream: function( _source )
        {
            var element = _source.element;

            element.play().catch( function( _error )
            {
                if( _source.element !== element )
                {
                    return;
                }

                if( _error.name === 'AbortError' )
                {
                    return;
                }

                if( _error.name === 'NotAllowedError' )
                {
                    _source.blocked = true;

                    return;
                }

                err( 'WebAudio stream play: ' + _error );
            } );
        },

        start: function( _source )
        {
            if( _source.element !== null )
            {
                _source.element.loop = _source.loop;
                _source.element.currentTime = _source.offset;
                _source.state = MengineAudio.STATE_PLAYING;

                MengineAudio.playStream( _source );

                return true;
            }

            if( _source.buffer === null )
            {
                return false;
            }

            MengineAudio.detach( _source );

            var context = MengineAudio.context;
            var node = context.createBufferSource();
            var gain = context.createGain();
            node.buffer = _source.buffer;
            node.loop = _source.loop;
            gain.gain.value = _source.volume * _source.volume;
            node.connect( gain );
            gain.connect( context.destination );

            _source.node = node;
            _source.gain = gain;
            _source.started = context.currentTime;
            _source.state = MengineAudio.STATE_PLAYING;

            node.onended = function()
            {
                if( _source.node !== node )
                {
                    return;
                }

                _source.state = MengineAudio.STATE_FINISHED;
                _source.offset = _source.buffer.duration;
                node.disconnect();
                gain.disconnect();
                _source.node = null;
                _source.gain = null;
            };

            node.start( 0, _source.offset );

            return true;
        }
    },

    mengineAudioInitialize__deps: [ '$MengineAudio' ],
    mengineAudioInitialize: function()
    {
        try
        {
            var audioContextType = globalThis.AudioContext || globalThis.webkitAudioContext;
            MengineAudio.context = new audioContextType();
            MengineAudio.unlock = function()
            {
                if( MengineAudio.context === null )
                {
                    return;
                }

                MengineAudio.fillPlayers();

                for( var source of MengineAudio.sources.values() )
                {
                    if( source.blocked === false )
                    {
                        continue;
                    }

                    source.blocked = false;

                    if( source.state !== MengineAudio.STATE_PLAYING )
                    {
                        continue;
                    }

                    MengineAudio.playStream( source );
                }

                if( MengineAudio.context.state !== 'suspended' )
                {
                    return;
                }

                MengineAudio.context.resume().catch( function( _error )
                {
                    if( MengineAudio.context === null )
                    {
                        return;
                    }

                    err( _error );
                } );
            };

            for( var type of ['pointerdown', 'keydown', 'touchend'] )
            {
                document.addEventListener( type, MengineAudio.unlock, true );
            }

            return true;
        }
        catch( _error )
        {
            err( 'WebAudio initialization: ' + _error );

            return false;
        }
    },

    mengineAudioFinalize__deps: [ '$MengineAudio' ],
    mengineAudioFinalize: function()
    {
        for( var type of ['pointerdown', 'keydown', 'touchend'] )
        {
            document.removeEventListener( type, MengineAudio.unlock, true );
        }

        for( var source of MengineAudio.sources.values() )
        {
            MengineAudio.detach( source );
            MengineAudio.releaseStream( source );
        }

        for( var stream of MengineAudio.streams.values() )
        {
            URL.revokeObjectURL( stream.url );
        }

        for( var player of MengineAudio.players )
        {
            player.media.disconnect();
        }

        MengineAudio.sources.clear();
        MengineAudio.buffers.clear();
        MengineAudio.streams.clear();
        MengineAudio.players.length = 0;

        if( MengineAudio.context !== null )
        {
            MengineAudio.context.close().catch( function( _error )
            {
                err( _error );
            } );
        }

        MengineAudio.context = null;
        MengineAudio.unlock = null;
    },

    mengineAudioBufferCreate__deps: [ '$MengineAudio' ],
    mengineAudioBufferCreate: function( _pcm, _frames, _channels, _frequency )
    {
        try
        {
            var buffer = MengineAudio.context.createBuffer( _channels, _frames, _frequency );
            var pcmOffset = _pcm >>> 1;
            var sampleScale = 32768;

            for( var channel = 0; channel < _channels; ++channel )
            {
                var data = buffer.getChannelData( channel );

                for( var frame = 0; frame < _frames; ++frame )
                {
                    data[frame] = HEAP16[pcmOffset + frame * _channels + channel] / sampleScale;
                }
            }

            var id = MengineAudio.nextId++;
            MengineAudio.buffers.set( id, buffer );

            return id;
        }
        catch( _error )
        {
            err( 'WebAudio buffer: ' + _error );

            return 0;
        }
    },

    mengineAudioStreamCreate__deps: [ '$MengineAudio' ],
    mengineAudioStreamCreate: function( _data, _size, _duration )
    {
        var data = _data >>> 0;
        var size = _size >>> 0;
        var millisecondsPerSecond = 1000;
        var blob = new Blob( [HEAPU8.slice( data, data + size )] );

        var id = MengineAudio.nextId++;
        MengineAudio.streams.set( id, {
            url: URL.createObjectURL( blob ),
            duration: _duration / millisecondsPerSecond
        } );

        return id;
    },

    mengineAudioBufferRelease__deps: [ '$MengineAudio' ],
    mengineAudioBufferRelease: function( _id )
    {
        MengineAudio.buffers.delete( _id );

        var stream = MengineAudio.streams.get( _id );

        if( stream === undefined )
        {
            return;
        }

        URL.revokeObjectURL( stream.url );
        MengineAudio.streams.delete( _id );
    },

    mengineAudioSourceCreate__deps: [ '$MengineAudio' ],
    mengineAudioSourceCreate: function()
    {
        var id = MengineAudio.nextId++;
        MengineAudio.sources.set( id, {
            buffer: null,
            stream: null,
            node: null,
            element: null,
            media: null,
            gain: null,
            blocked: false,
            state: MengineAudio.STATE_STOPPED,
            offset: 0,
            started: 0,
            volume: 1,
            loop: false
        } );

        return id;
    },

    mengineAudioSourceRelease__deps: [ '$MengineAudio' ],
    mengineAudioSourceRelease: function( _id )
    {
        var source = MengineAudio.sources.get( _id );

        if( source === undefined )
        {
            return;
        }

        MengineAudio.detach( source );
        MengineAudio.releaseStream( source );
        MengineAudio.sources.delete( _id );
    },

    mengineAudioSourceBuffer__deps: [ '$MengineAudio' ],
    mengineAudioSourceBuffer: function( _id, _buffer )
    {
        var source = MengineAudio.sources.get( _id );

        MengineAudio.releaseStream( source );

        source.buffer = MengineAudio.buffers.get( _buffer ) || null;

        var stream = MengineAudio.streams.get( _buffer );

        if( stream === undefined )
        {
            return;
        }

        MengineAudio.attachStream( source, stream );
    },

    mengineAudioPlay__deps: [ '$MengineAudio' ],
    mengineAudioPlay: function( _id )
    {
        var source = MengineAudio.sources.get( _id );

        if( source.state === MengineAudio.STATE_PLAYING )
        {
            return true;
        }

        if( MengineAudio.start( source ) === false )
        {
            return false;
        }

        return true;
    },

    mengineAudioStop__deps: [ '$MengineAudio' ],
    mengineAudioStop: function( _id )
    {
        var source = MengineAudio.sources.get( _id );
        MengineAudio.detach( source );
        source.offset = 0;
        source.state = MengineAudio.STATE_STOPPED;
    },

    mengineAudioPause__deps: [ '$MengineAudio' ],
    mengineAudioPause: function( _id )
    {
        var source = MengineAudio.sources.get( _id );

        if( source.state !== MengineAudio.STATE_PLAYING )
        {
            return;
        }

        source.offset = MengineAudio.position( source );
        MengineAudio.detach( source );
        source.state = MengineAudio.STATE_PAUSED;
    },

    mengineAudioResume__deps: [ '$MengineAudio' ],
    mengineAudioResume: function( _id )
    {
        var source = MengineAudio.sources.get( _id );

        if( source.state !== MengineAudio.STATE_PAUSED )
        {
            return false;
        }

        if( MengineAudio.start( source ) === false )
        {
            return false;
        }

        return true;
    },

    mengineAudioState__deps: [ '$MengineAudio' ],
    mengineAudioState: function( _id )
    {
        var source = MengineAudio.sources.get( _id );

        return source.state;
    },

    mengineAudioVolume__deps: [ '$MengineAudio' ],
    mengineAudioVolume: function( _id, _volume )
    {
        var source = MengineAudio.sources.get( _id );
        source.volume = _volume;

        if( source.gain === null )
        {
            return;
        }

        source.gain.gain.value = _volume * _volume;
    },

    mengineAudioLoop__deps: [ '$MengineAudio' ],
    mengineAudioLoop: function( _id, _loop )
    {
        var source = MengineAudio.sources.get( _id );
        source.offset = MengineAudio.position( source );
        source.started = MengineAudio.context.currentTime;
        source.loop = !!_loop;

        if( source.element !== null )
        {
            source.element.loop = !!_loop;

            return;
        }

        if( source.node === null )
        {
            return;
        }

        source.node.loop = !!_loop;
    },

    mengineAudioSeek__deps: [ '$MengineAudio' ],
    mengineAudioSeek: function( _id, _position )
    {
        var source = MengineAudio.sources.get( _id );
        var content = MengineAudio.content( source );

        if( content === null )
        {
            return false;
        }

        if( Number.isFinite( _position ) === false )
        {
            return false;
        }

        if( _position < 0 )
        {
            return false;
        }

        var millisecondsPerSecond = 1000;
        var positionTolerance = 0.01;
        var maxPosition = content.duration * millisecondsPerSecond + positionTolerance;

        if( _position > maxPosition )
        {
            return false;
        }

        source.offset = _position / millisecondsPerSecond;

        if( source.state !== MengineAudio.STATE_PLAYING )
        {
            return true;
        }

        if( MengineAudio.start( source ) === false )
        {
            return false;
        }

        return true;
    },

    mengineAudioPosition__deps: [ '$MengineAudio' ],
    mengineAudioPosition: function( _id )
    {
        var source = MengineAudio.sources.get( _id );
        var seconds = MengineAudio.position( source );
        var millisecondsPerSecond = 1000;
        var position = seconds * millisecondsPerSecond;

        return position;
    }
} );
