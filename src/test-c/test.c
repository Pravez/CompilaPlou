int x, y;

void my_draw(){
    /*double xx = (double) x;
    double yy = (double) y;*/
    ellipse( ((double) x), ((double) y), 80.0, 80.0);
}

void my_setup(){
    createCanvas(200.0, 200.0);
    x = 100;
    y = 100;
}