// From http://baagoe.com/en/RandomMusings/javascript/
function Kybos() {
  return (function(args) {
    // Johannes Baagøe <baagoe@baagoe.com>, 2010
    var s0 = 0;
    var s1 = 0;
    var s2 = 0;
    var c = 1;
    var s = [];
    var k = 0;

    var mash = Mash();
    var s0 = mash(' ');
    var s1 = mash(' ');
    var s2 = mash(' ');
    for (var j = 0; j < 8; j++) {
      s[j] = mash(' ');
    }

    if (args.length == 0) {
      args = [+new Date];
    }
    for (var i = 0; i < args.length; i++) {
      s0 -= mash(args[i]);
      if (s0 < 0) {
        s0 += 1;
      }
      s1 -= mash(args[i]);
      if (s1 < 0) {
        s1 += 1;
      }
      s2 -= mash(args[i]);
      if (s2 < 0) {
        s2 += 1;
      }
      for (var j = 0; j < 8; j++) {
        s[j] -= mash(args[i]);
        if (s[j] < 0) {
          s[j] += 1;
        }
      }
    }

    var random = function() {
      var a = 2091639;
      k = s[k] * 8 | 0;
      var r = s[k];
      var t = a * s0 + c * 2.3283064365386963e-10; // 2^-32
      s0 = s1;
      s1 = s2;
      s2 = t - (c = t | 0);
      s[k] -= s2;
      if (s[k] < 0) {
        s[k] += 1;
      }
      return r;
    };
    random.uint32 = function() {
      return random() * 0x100000000; // 2^32
    };
    random.fract53 = function() {
      return random() +
        (random() * 0x200000 | 0) * 1.1102230246251565e-16; // 2^-53
    };
    random.addNoise = function() {
      for (var i = arguments.length - 1; i >= 0; i--) {
        for (j = 0; j < 8; j++) {
          s[j] -= mash(arguments[i]);
          if (s[j] < 0) {
            s[j] += 1;
          }
        }
      }
    };
    random.version = 'Kybos 0.9';
    random.args = args;
    return random;

  } (Array.prototype.slice.call(arguments)));
};
