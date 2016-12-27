'use strict';

const E = require('./emu');

const timer = function(f) 
{
    const start = new Date().getTime();
    f();
    const end = new Date().getTime();
};

const x = [0, 1, 2, 3, 4, 5];
console.log(x);

E.sort_array(x);

console.log(x);
