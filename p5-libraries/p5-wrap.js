mergeInto(LibraryManager.library,
{
    ellipse: function(a,b,c,d) { ellipse(a,b,c,d); },
    createCanvas: function(x, y) { createCanvas(x, y); },
    background: function(c) { background(c); },
    fill: function(c) { fill(c); },
    stroke: function(c) { stroke(c); },
    point: function(x, y) { point(x, y); },
    line: function(a, b, c, d) { line(a, b, c, d); }
});
