// From http://baagoe.com/en/RandomMusings/javascript/
function KISS07() {
  return (function(args) {
    // George Marsaglia, 2007-06-23
    //http://groups.google.com/group/comp.lang.fortran/msg/6edb8ad6ec5421a5
    var x = 123456789;
    var y = 362436069;
    var z =  21288629;
    var w =  14921776;
    var c = 0;

    if (args.length == 0) {
      args = [+new Date];
    }
    var mash = Mash();
    for (var i = 0; i < args.length; i++) {
      x ^= mash(args[i]) * 0x100000000; // 2^32
      y ^= mash(args[i]) * 0x100000000;
      z ^= mash(args[i]) * 0x100000000;
      w ^= mash(args[i]) * 0x100000000;
    }
    if (y === 0) {
      y = 1;
    }
    c ^= z >>> 31;
    z &= 0x7fffffff;
    if ((z % 7559) === 0) {
      z++;
    }
    w &= 0x7fffffff;
    if ((w % 7559) === 0) {
      w++;
    }
    mash = null;

    var uint32 = function() {
      var t;

      x += 545925293;
      x >>>= 0;

      y ^= y << 13;
      y ^= y >>> 17;
      y ^= y << 5;

      t = z + w + c;
      z = w;
      c = t >>> 31;
      w = t & 0x7fffffff;

      return x + y + w >>> 0;
    };

    var random = function() {
      return uint32() * 2.3283064365386963e-10; // 2^-32
    };
    random.uint32 = uint32;
    random.fract53 = function() {
      return random() +
        (uint32() & 0x1fffff) * 1.1102230246251565e-16; // 2^-53
    };
    random.args = args;
    random.version = 'KISS07 0.9';

    return random;
  } (Array.prototype.slice.call(arguments)));
};
