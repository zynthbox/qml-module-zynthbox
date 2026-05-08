/**
 * jzz_webchannel.js
 *
 * Builds a complete Web MIDI API (MIDIAccess) directly over QWebChannel +
 * C++ MidiTransport. Does NOT rely on JZZ.requestMIDIAccess internally —
 * we construct MIDIInput/MIDIOutput objects ourselves so there is no
 * engine-selection re-run that could hit the browser's permission system.
 *
 * Injection order (DocumentCreation, MainWorld):
 *   1. qwebchannel.js    — qt.webChannelTransport
 *   2. midi_prestub      — clears navigator.requestMIDIAccess
 *   3. jzz.js            — JZZ core (optional, for apps that call JZZ() directly)
 *   4. jzz_webchannel.js — this file
 */
(function () {
    'use strict';

    // ── 1. Install queuing stub immediately ───────────────────────────────────
    var _pending = [];

    function _install(fn) {
        var d = { configurable: true, writable: true, value: fn };
        try { Object.defineProperty(navigator, 'requestMIDIAccess', d); }
        catch (e) { navigator.requestMIDIAccess = fn; }
    }

    _install(function stub(opt) {
        console.log('[JZZ-WC] requestMIDIAccess queued (channel not ready)');
        return new Promise(function (res, rej) {
            _pending.push({ opt: opt, res: res, rej: rej });
        });
    });

    console.log('[JZZ-WC] Queuing stub installed');

    // ── 2. Connect QWebChannel ────────────────────────────────────────────────
    function _connect() {
        if (typeof QWebChannel === 'undefined' || !window.qt || !qt.webChannelTransport)
            return setTimeout(_connect, 50);

        console.log('[JZZ-WC] Opening WebChannel...');
        new QWebChannel(qt.webChannelTransport, function (ch) {
            console.log('[JZZ-WC] Channel ready. Objects:', Object.keys(ch.objects).join(', '));
            var transport = ch.objects.midiTransport;
            if (!transport) {
                _fail('midiTransport not found on channel');
                return;
            }
            _buildAccess(transport);
        });
    }

    // ── 3. Build MIDIAccess from transport ────────────────────────────────────
    function _buildAccess(transport) {
        transport.inputPorts(function (ins) {
            transport.outputPorts(function (outs) {
                console.log('[JZZ-WC] Ports — in:', ins.length, ', out:', outs.length);

                // ---- MIDIInput -----------------------------------------------
                var inputMap = new Map();

                ins.forEach(function (p) {
                    var _fns = [];
                    var port = {
                        id: p.id, name: p.name, manufacturer: '',
                        type: 'input', version: '1.0',
                        state: 'connected', connection: 'open',
                        onmidimessage: null, onstatechange: null,
                        addEventListener: function (t, fn) {
                            if (t === 'midimessage') _fns.push(fn);
                        },
                        removeEventListener: function (t, fn) {
                            if (t === 'midimessage') _fns = _fns.filter(function (f) { return f !== fn; });
                        },
                        open:  function () { return Promise.resolve(this); },
                        close: function () { return Promise.resolve(this); },
                        _dispatch: function (bytes) {
                            var ev = { data: new Uint8Array(bytes), target: this, timeStamp: performance.now() };
                            if (typeof this.onmidimessage === 'function') this.onmidimessage(ev);
                            _fns.forEach(function (fn) { fn(ev); });
                        }
                    };
                    inputMap.set(p.id, port);
                    transport.openInput(p.index, p.id);
                });

                // Route incoming bytes to the right MIDIInput
                transport.received.connect(function (portId, bytes) {
                    var port = inputMap.get(portId);
                    if (port) port._dispatch(bytes);
                });

                // ---- MIDIOutput ----------------------------------------------
                var outputMap = new Map();

                outs.forEach(function (p) {
                    var port = {
                        id: p.id, name: p.name, manufacturer: '',
                        type: 'output', version: '1.0',
                        state: 'connected', connection: 'open',
                        onstatechange: null,
                        send:  function (data) { transport.send(this.id, Array.from(data)); },
                        clear: function () {},
                        open:  function () { return Promise.resolve(this); },
                        close: function () { return Promise.resolve(this); }
                    };
                    outputMap.set(p.id, port);
                    transport.openOutput(p.index, p.id);
                });

                // ---- MIDIAccess ---------------------------------------------
                var access = {
                    inputs:  inputMap,
                    outputs: outputMap,
                    sysexEnabled:        true,
                    onstatechange:       null,
                    addEventListener:    function () {},
                    removeEventListener: function () {}
                };

                // ---- Install permanent requestMIDIAccess --------------------
                _install(function () { return Promise.resolve(access); });

                // Drain any calls that arrived while the channel was connecting
                var queued = _pending.splice(0);
                queued.forEach(function (item) { item.res(access); });

                console.log('[JZZ-WC] ✔ MIDI bridge active —',
                    ins.length, 'in /', outs.length, 'out.',
                    queued.length, 'queued call(s) drained.');
            });
        });
    }

    function _fail(msg) {
        console.error('[JZZ-WC]', msg);
        _pending.splice(0).forEach(function (p) {
            p.rej(new DOMException(msg, 'NotAllowedError'));
        });
    }

    _connect();
})();
