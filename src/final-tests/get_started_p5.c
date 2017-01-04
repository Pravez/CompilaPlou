/** Premier test
 * Simple recopie + "amelioration" du "get started" de la bibliotheque p5
 */


int x, y;

void my_draw(){
    double xx = (double) x;
    double yy = (double) y;
    ellipse( xx,  yy, 80.0, 80.0);
}

void my_setup(){
    createCanvas(200.0, 200.0);
    x = 100;
    y = 100;
}