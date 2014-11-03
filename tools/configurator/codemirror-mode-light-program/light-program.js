(function(mod) {
  if (typeof exports == "object" && typeof module == "object") // CommonJS
    mod(require("../../lib/codemirror"));
  else if (typeof define == "function" && define.amd) // AMD
    define(["../../lib/codemirror"], mod);
  else // Plain browser env
    mod(CodeMirror);
})(function(CodeMirror) {
"use strict";

CodeMirror.defineMode("light-program", function(_config, parserConfig) {
  'use strict';

  var directives = {
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

  var run_condition_directives = {
    "or": "keyword",
    "always": "attribute",
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

  var skip_if_directives = {
    "any": "keyword",
    "all": "keyword",
    "is": "keyword",
    "none": "keyword",
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


  function nextUntilUnescaped(stream, end) {
    var escaped = false, next;
    while ((next = stream.next()) != null) {
      if (next === end && !escaped) {
        return false;
      }
      escaped = !escaped && next === "\\";
    }
    return escaped;
  }


  function car_state(ch, stream, state) {
    var style, cur;

    if (/[\w\-_]/.test(ch)) {
      stream.eatWhile(/[\w\-_]/);
      cur = stream.current().toLowerCase();
      style = car_state_directives[cur];
      if (style) {
        return style;
      }
    }
    stream.skipToEnd();
    state.tokenize = null;
    return 'error';
  }


  function skip_if(ch, stream, state) {
    var style, cur;

    if (/[\w\-_]/.test(ch)) {
      stream.eatWhile(/[\w\-_]/);
      cur = stream.current().toLowerCase();
      style = skip_if_directives[cur];
      if (style) {
        state.tokenize = car_state;
        return style;
      }
      else{
        state.tokenize = null;
        style = directives[cur];
        return style || "variable";
      }
    }
    stream.skipToEnd();
    state.tokenize = null;
    return 'error';
  }


  function skip(ch, stream, state) {
    var style, cur;

    if (/[\w\-_]/.test(ch)) {
      stream.eatWhile(/[\w\-_]/);
      cur = stream.current().toLowerCase();
      if (cur === "if") {
        state.tokenize = skip_if;
        return directives[cur];
      }
    }
    stream.skipToEnd();
    state.tokenize = null;
    return 'error';
  }


  function run_condition(ch, stream, state) {
    var style, cur;

    if (/[\w\-_]/.test(ch)) {
      stream.eatWhile(/[\w\-_]/);
      cur = stream.current().toLowerCase();
      style = run_condition_directives[cur];
      if (style) return style;
    }
    stream.skipToEnd();
    state.tokenize = null;
    return 'error';
  }

  return {
    startState: function() {
      return {
        tokenize: null
      };
    },

    token: function(stream, state) {
      if (stream.sol()) {
        state.tokenize = null;
      }

      if (stream.eatSpace()) {
        return null;
      }

      var style, cur, ch = stream.next();

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

      if (state.tokenize) {
        return state.tokenize(ch, stream, state);
      }

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
        cur = stream.current().toLowerCase();
        if (cur === "when") {
          state.tokenize = run_condition;
        }
        if (cur === "skip") {
          state.tokenize = skip;
        }
        style = directives[cur];
        return style || "variable";
      }

    },

    lineComment: "//",
  };
});

});
