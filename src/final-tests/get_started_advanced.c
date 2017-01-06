/** Deuxième test
 * Petits calculs en plus avec la bibliotheque p5
 */

int pos_x;
int pos_y;
int to_add_x;
int to_add_y;

void my_setup() {
  createCanvas(400.0,400.0);

  pos_x = 180;
  pos_y = 180;

  to_add_x = 1;
  to_add_y = -1;
}


void my_draw() {
  background(0.0);
  stroke(255.0);


  
  if(pos_x > 350){
    to_add_x = -1;
  }
  if(pos_x < 50){
    to_add_x = 1;
  }

  if(pos_y > 350){
    to_add_y = -1;
  }
  if(pos_y < 50){
    to_add_y = 1;
  }

  ellipse( (double) pos_x, (double) pos_y, 10.0, 10.0);
  ellipse( (double) pos_y, (double) pos_x, 10.0, 10.0);

  pos_x = pos_x + to_add_x;
  pos_y = pos_y + to_add_y;
}
