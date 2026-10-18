(() => {
    const maxIdentifierBytes = 128;
    const maxMessageBytes = 1024 * 1024;
    const maxPendingRequests = 128;
    const maxJSONDepth = 256;
    const encoder = new TextEncoder();
    const invalidJSONText = /[\u0000\uD800-\uDFFF]/u;
    const pending = new Map();
    const safeAreaSides = ['top', 'right', 'bottom', 'left'];
    let sequence = 0;
    let safeArea = null;
    let applicationReady = false;

    const createConnection = () => {
        const state = {generation: null, closed: false, persisted: false};
        state.ready = new Promise((resolve, reject) => {
            state.resolve = resolve;
            state.reject = reject;
        });
        state.ready.catch(() => undefined);

        return state;
    };
    let connection = createConnection();

    const closeConnection = persisted => {
        const state = connection;

        if (state.closed === true) {
            return;
        }

        state.closed = true;
        state.persisted = persisted;
        const error = new Error('document_closed');
        state.reject(error);
        state.ready = Promise.reject(error);
        state.ready.catch(() => undefined);

        for (const request of pending.values()) {
            request.reject(error);
        }

        pending.clear();
        safeArea = null;
    };

    const validateJSONText = value => {
        if (invalidJSONText.test(value) === true) {
            throw new TypeError('invalid bridge request');
        }
    };

    const serialize = value => {
        const parents = [];
        const json = JSON.stringify(value, function (key, member) {
            validateJSONText(key);

            if (typeof member === 'string') {
                validateJSONText(member);
            }

            while (parents.length !== 0) {
                if (parents[parents.length - 1] === this) {
                    break;
                }

                parents.pop();
            }

            if (parents.length >= maxJSONDepth) {
                throw new RangeError('bridge message is too deeply nested');
            }

            if (member === null) {
                return member;
            }

            if (typeof member !== 'object') {
                return member;
            }

            parents.push(member);

            return member;
        });

        if (typeof json !== 'string') {
            throw new TypeError('invalid bridge request');
        }

        const bytes = encoder.encode(json);

        if (bytes.length > maxMessageBytes) {
            throw new RangeError('bridge message is too large');
        }

        return json;
    };

    const post = json => {
        if (window.mengine) {
            window.mengine.postMessage(json);

            return;
        }

        if (window.chrome?.webview) {
            window.chrome.webview.postMessage(json);

            return;
        }

        window.webkit.messageHandlers.mengine.postMessage(json);
    };

    const applySafeArea = () => {
        const root = document.documentElement;
        if (root === null) {
            return;
        }

        let scale = 0;

        if (safeArea !== null) {
            const viewportWidth = window.visualViewport?.width || window.innerWidth;

            if (Number.isFinite(viewportWidth) === false) {
                return;
            }

            if (viewportWidth <= 0) {
                return;
            }

            scale = viewportWidth / safeArea.width;
        }

        for (const side of safeAreaSides) {
            const nativeValue = `env(safe-area-inset-${side}, 0px)`;
            const value = safeArea === null
                ? nativeValue
                : `max(${nativeValue}, ${safeArea[side] * scale}px)`;
            const propertyName = `--mengine-safe-area-inset-${side}`;
            root.style.setProperty(propertyName, value);
        }
    };

    const bridge = {
        get ready() {
            return connection.ready;
        },
        connect(value) {
            if (typeof value !== 'string') {
                throw new TypeError('invalid bridge generation');
            }

            if (value.length === 0) {
                throw new TypeError('invalid bridge generation');
            }

            if (connection.closed === true) {
                if (connection.persisted === false) {
                    return;
                }

                connection = createConnection();
            }

            if (connection.generation !== null) {
                if (connection.generation !== value) {
                    closeConnection(false);
                    connection = createConnection();
                }
            }

            const state = connection;
            const json = serialize({kind: 'ready', generation: value});
            post(json);

            if (state.closed === true) {
                return;
            }

            state.generation = value;
            state.resolve();

            if (applicationReady === false) {
                return;
            }

            const appReadyJSON = serialize({kind: 'appReady', generation: value});
            post(appReadyJSON);
        },
        async call(method, args = {}) {
            const state = connection;

            if (state.closed === true) {
                throw new Error('document_closed');
            }

            if (typeof method !== 'string') {
                throw new TypeError('invalid bridge request');
            }

            if (method.length === 0) {
                throw new TypeError('invalid bridge request');
            }

            validateJSONText(method);
            const methodBytes = encoder.encode(method);

            if (methodBytes.length > maxIdentifierBytes) {
                throw new TypeError('invalid bridge request');
            }

            const argumentsJSON = serialize(args);
            const argumentsValue = JSON.parse(argumentsJSON);

            if (argumentsValue === null) {
                throw new TypeError('invalid bridge request');
            }

            if (typeof argumentsValue !== 'object') {
                throw new TypeError('invalid bridge request');
            }

            if (Array.isArray(argumentsValue) === true) {
                throw new TypeError('invalid bridge request');
            }

            await state.ready;

            if (state.closed === true) {
                throw new Error('document_closed');
            }

            if (pending.size >= maxPendingRequests) {
                throw new Error('too many pending bridge requests');
            }

            const requestSequence = ++sequence;
            const id = String(requestSequence);
            const json = serialize({kind: 'request', generation: state.generation, id, method, arguments: argumentsValue});
            const promise = new Promise((resolve, reject) => {
                pending.set(id, {resolve, reject});

                try {
                    post(json);
                } catch (error) {
                    pending.delete(id);
                    reject(error);
                }
            });

            return promise;
        },
        async appReady() {
            const state = connection;
            await state.ready;

            if (state.closed === true) {
                throw new Error('document_closed');
            }

            applicationReady = true;
            const json = serialize({kind: 'appReady', generation: state.generation});
            post(json);
        },
        receive(message) {
            if (connection.closed === true) {
                return;
            }

            if (message === null) {
                return;
            }

            if (typeof message !== 'object') {
                return;
            }

            if (connection.generation === null) {
                return;
            }

            if (message.generation !== connection.generation) {
                return;
            }

            if (message.kind === 'response') {
                const request = pending.get(message.id);

                if (request === undefined) {
                    return;
                }

                pending.delete(message.id);

                if (message.error) {
                    const error = new Error(message.error);
                    request.reject(error);

                    return;
                }

                request.resolve(message.result);

                return;
            }

            if (message.kind === 'safeArea') {
                const values = [message.top, message.right, message.bottom, message.left];

                if (Number.isFinite(message.width) === false) {
                    return;
                }

                if (message.width <= 0) {
                    return;
                }

                const invalidValue = values.some(value => {
                    if (Number.isFinite(value) === false) {
                        return true;
                    }

                    if (value < 0) {
                        return true;
                    }

                    return false;
                });

                if (invalidValue === true) {
                    return;
                }

                safeArea = {
                    width: message.width,
                    top: message.top,
                    right: message.right,
                    bottom: message.bottom,
                    left: message.left
                };
                applySafeArea();

                return;
            }

            const event = new CustomEvent('mengine-message', {detail: message});
            window.dispatchEvent(event);
        }
    };
    const api = Object.freeze(bridge);
    Object.defineProperty(window, 'Mengine', {value: api});
    applySafeArea();
    document.addEventListener('DOMContentLoaded', applySafeArea, {once: true});
    window.addEventListener('resize', applySafeArea);
    window.visualViewport?.addEventListener('resize', applySafeArea);

    if (window.chrome?.webview) {
        window.chrome.webview.addEventListener('message', event => {
            bridge.receive(event.data);
        });
    }

    window.addEventListener('pagehide', event => {
        closeConnection(event.persisted === true);
    });

    window.addEventListener('pageshow', event => {
        if (event.persisted !== true) {
            return;
        }

        if (connection.closed === true) {
            connection = createConnection();
        }

        applySafeArea();
        const json = serialize({kind: 'connect', generation: connection.generation});
        post(json);
    });

    const json = serialize({kind: 'connect', generation: connection.generation});
    post(json);
})();
