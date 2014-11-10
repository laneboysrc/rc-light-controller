'use strict';
/*jslint browser: true, vars: true, indent: 2 */
/*global define, CodeMirror */
(function (mod) {
  if (typeof exports === "object" && typeof module === "object") {// CommonJS
    mod(require("../../lib/codemirror"));
  } else if (typeof define === "function" && define.amd) { // AMD
    define(["../../lib/codemirror"], mod);
  } else { // Plain browser env
    mod(CodeMirror);
  }
})(function (CodeMirror) {

  CodeMirror.defineMode("light-program", function (configuration, parserConfig) {

    var directives = {
      "end": "keyword",
      "run": "keyword",
      "when": "keyword",
      "or": "keyword",
      "goto": "keyword",
      "sleep": "keyword",
      "fade": "keyword",
      "stepsize": "keyword",
      "all": "qualifier",
      "leds": "qualifier",
      "led": "def",
      "abs": "operator",
      "global": "keyword",
      "var": "def",
      "skip": "keyword",
      "if": "keyword",
      "master": "qualifier",
      "slave": "qualifier",
      "random": "built-in",
      "steering": "built-in",
      "throttle": "built-in",
      "gear": "built-in",
      "clicks": "built-in",
    };

    var run_state_directives = {
      "when": "keyword",
      "always": "attribute",
    };

    var run_condition_directives = {
      "or": "keyword",
      "light-switch-position-0": "attribute",
      "light-switch-position-1": "attribute",
      "light-switch-position-2": "attribute",
      "light-switch-position-3": "attribute",
      "light-switch-position-4": "attribute",
      "light-switch-position-5": "attribute",
      "light-switch-position-6": "attribute",
      "light-switch-position-7": "attribute",
      "light-switch-position-8": "attribute",
      "neutral": "attribute",
      "forward": "attribute",
      "reversing": "attribute",
      "braking": "attribute",
      "indicator-left": "attribute",
      "indicator-right": "attribute",
      "hazard": "attribute",
      "blink-flag": "attribute",
      "blink-left": "attribute",
      "blink-right": "attribute",
      "winch-disabled": "attribute",
      "winch-idle": "attribute",
      "winch-in": "attribute",
      "winch-out": "attribute",
      "no-signal": "attribute",
      "initializing": "attribute",
      "servo-output-setup-centre": "attribute",
      "servo-output-setup-left": "attribute",
      "servo-output-setup-right": "attribute",
      "reversing-setup-steering": "attribute",
      "reversing-setup-throttle": "attribute",
      "gear-changed": "attribute"
    };

    var skip_if_directives_multiple = {
      "any": "keyword",
      "all": "keyword",
      "none": "keyword",
    };

    var skip_if_directives_single = {
      "is": "keyword",
      "not": "keyword",
    };

    var car_state_directives = {
      "light-switch-position-0": "attribute",
      "light-switch-position-1": "attribute",
      "light-switch-position-2": "attribute",
      "light-switch-position-3": "attribute",
      "light-switch-position-4": "attribute",
      "light-switch-position-5": "attribute",
      "light-switch-position-6": "attribute",
      "light-switch-position-7": "attribute",
      "light-switch-position-8": "attribute",
      "neutral": "attribute",
      "forward": "attribute",
      "reversing": "attribute",
      "braking": "attribute",
      "indicator-left": "attribute",
      "indicator-right": "attribute",
      "hazard": "attribute",
      "blink-flag": "attribute",
      "blink-left": "attribute",
      "blink-right": "attribute",
      "winch-disabled": "attribute",
      "winch-idle": "attribute",
      "winch-in": "attribute",
      "winch-out": "attribute",
      "no-signal": "attribute",
      "initializing": "attribute",
      "servo-output-setup-centre": "attribute",
      "servo-output-setup-left": "attribute",
      "servo-output-setup-right": "attribute",
      "reversing-setup-steering": "attribute",
      "reversing-setup-throttle": "attribute",
    };


    // ***************************************************************************
    function nextUntilUnescaped(stream, end) {
      var escaped = false;
      var next;

      next = stream.next();
      while (next !== null) {
        if (next === end && !escaped) {
          return false;
        }
        escaped = !escaped && next === "\\";
        next = stream.next();
      }
      return escaped;
    }


    // ***************************************************************************
    function car_states(stream) {
      var style, cur, ch;

      ch = stream.next();

      if (/[\w\-_]/.test(ch)) {
        stream.eatWhile(/[\w\-_]/);
        cur = stream.current();
        style = car_state_directives[cur];
        if (style) {
          return style;
        }
      }

      return 'error';
    }


    // ***************************************************************************
    function anything_is_error(stream) {
      stream.next();
      return 'error';
    }


    // ***************************************************************************
    function run_condition(stream, state) {
      var style, cur, ch;

      ch = stream.next();

      if (/[\w\-_]/.test(ch)) {
        stream.eatWhile(/[\w\-_]/);
        cur = stream.current();
        style = run_condition_directives[cur];
        if (style) {
          return style;
        }
      }

      state.tokenize = anything_is_error;
      return 'error';
    }


    // ***************************************************************************
    function run_state(stream, state) {
      var style, cur, ch;

      ch = stream.next();

      if (/[\w\-_]/.test(ch)) {
        stream.eatWhile(/[\w\-_]/);
        cur = stream.current();
        style = run_state_directives[cur];
        if (cur === "when") {
          state.tokenize = run_condition;
        } else {
          state.tokenize = anything_is_error;
        }
        if (style) {
          return style;
        }
      }

      state.tokenize = anything_is_error;
      return 'error';
    }


    // ***************************************************************************
    function goto_state(stream, state) {
      var ch;

      ch = stream.next();

      if (!(/\d/.test(ch)) && /[\w\-_]/.test(ch)) {
        stream.eatWhile(/[\w\-_]/);
        state.tokenize = anything_is_error;
        return 'atom';
      }

      stream.skipToEnd();
      state.tokenize = default_state;
      return 'error';
    }


    // ***************************************************************************
    function car_state(stream, state) {
      var style, cur, ch;

      ch = stream.next();

      if (/[\w\-_]/.test(ch)) {
        stream.eatWhile(/[\w\-_]/);
        cur = stream.current();
        style = car_state_directives[cur];
        if (style) {
          state.tokenize = anything_is_error;
          return style;
        }
      }

      state.tokenize = anything_is_error;
      return 'error';
    }


    // ***************************************************************************
    function skip_if(stream, state) {
      var style, cur, ch;

      ch = stream.next();


      if (!(/\d/.test(ch)) && /[\w\-_]/.test(ch)) {
        stream.eatWhile(/[\w\-_]/);
        cur = stream.current();

        style = skip_if_directives_multiple[cur];
        if (style) {
          state.tokenize = car_states;
          return style;
        }

        style = skip_if_directives_single[cur];
        if (style) {
          state.tokenize = car_state;
          return style;
        }

        state.tokenize = default_state;
        style = directives[cur];
        return style || "variable";
      }

      state.tokenize = anything_is_error;
      return 'error';
    }


    // ***************************************************************************
    function skip(stream, state) {
      var cur, ch;

      ch = stream.next();
      if (/[\w\-_]/.test(ch)) {
        stream.eatWhile(/[\w\-_]/);
        cur = stream.current();
        if (cur === "if") {
          state.tokenize = skip_if;
          return directives[cur];
        }
      }

      state.tokenize = anything_is_error;
      return 'error';
    }

    // ***************************************************************************
    function default_state(stream, state) {
      var style, cur, ch;

      ch = stream.next();

      if (/\d/.test(ch)) {
        if (ch === "0" && stream.eat("x")) {
          stream.eatWhile(/[0-9a-fA-F]/);
          return "number";
        }
        stream.eatWhile(/\d/);
        return "number";
      }

      if (/[\w\-_]/.test(ch)) {
        stream.eatWhile(/[\w\-_]/);
        if (stream.eat(":")) {
          return 'atom';
        }
        cur = stream.current();
        if (cur === "run") {
          state.tokenize = run_state;
        }
        if (cur === "skip") {
          state.tokenize = skip;
        }
        if (cur === "goto") {
          state.tokenize = goto_state;
        }
        style = directives[cur];
        return style || "variable";
      }
    }


    // ***************************************************************************
    return {

      // ----------------------------------
      startState: function () {
        return {
          tokenize: default_state
        };
      },

      // ----------------------------------
      token: function (stream, state) {
        var ch;

        if (stream.sol()) {
          state.tokenize = default_state;
        }

        if (stream.eatSpace()) {
          return null;
        }

        ch = stream.next();

        if (ch === '/') {
          if (stream.eat("/")) {
            stream.skipToEnd();
            return "comment";
          }
        }

        if (ch === ';') {
          stream.skipToEnd();
          return "comment";
        }

        if (ch === '"') {
          nextUntilUnescaped(stream, '"');
          return "string";
        }

        stream.backUp(1);
        return state.tokenize(stream, state);
      },

      // ----------------------------------
      lineComment: "//",
    };
  });

});
