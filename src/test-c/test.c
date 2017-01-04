double w;
double h;
double angle;
int angle_dir;
int N;

// Appelé une fois au début depuis sketch.js
void my_setup()
{
    w = 640;
    h = 480;
    angle = 0;
    angle_dir = 0;
    N = 200;

    // Définit la taille de la zone d'affichage
    createCanvas(w, h);
}

// Appelé à chaque frame depuis sketch.js
void my_draw()
{
    int i = 0;
    double r;

    // Fond noir
    background(0.0);

    // Affichage des cercles
    for(i ; i < N; i++)
    {
        // Remplissage des cercles en niveaux de gris
        fill(255.0 * i / N);

        // Contour des cercles en niveaux de gris
        stroke(255.0 * (N - i) / N);

        r = i + angle;

        // Dessin du cercle
        ellipse(w / 2.0 + i * sin(r), h / 2.0 + i * cos(r), i * angle, i * angle);
    }

    // Animation qui boucle :
    if(angle_dir == 0)
    {
        angle += 0.01;
        if(angle > 2)
            angle_dir = 1;
    }
    else if(angle_dir == 1)
    {
        angle -= 0.01;
        if(angle < -2)
            angle_dir = 0;
    }
}