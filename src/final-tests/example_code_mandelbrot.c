double w, h;
double bail_out;
double zoom_factor, zoom;

void my_setup()
{
    w = 640;
    h = 480;
    bail_out = 2.0;
    zoom_factor = 1.4;
    zoom = (w * 0.25296875);
    // Définit la taille de la zone d'affichage
    createCanvas((double) w, (double) h);

}

void my_draw()
{
    int x,y,n;
    int maxiter;
    double zr,zi,cr,ci;
    double centerr;
    double centeri;
    double m;
    double a;
    double b;
    double color;

    maxiter = (w/2) * 0.049715909 * log10(zoom);
    centerr = -0.800671;
    centeri = 0.158392;

    for  (y = 0; y < h; y++)  {
        for (x = 0; x < w; x++) {
            m=0;
            /* Get the complex poing on gauss space to be calculate */
            cr=centerr + (x - (w/2))/zoom;
            zr=cr;
            ci=centeri + (y - (h/2))/zoom;
            zi=ci;
            /* Applies the actual mandelbrot formula on that point */
            for (n = 0; n <= maxiter && m < bail_out * bail_out; n ++) {
                a=zr*zr-zi*zi+cr;
                b=2*zr*zi+ci;
                zr=a;
                zi=b;
                m=a*a+b*b;
            }

            /* Paint the pixel calculated depending on the number
           of iterations found */
            if (n<maxiter)
                color= n / maxiter * 255;
            else
                color=0;

            stroke(color);
            point((double) x, (double) y);
        }
    }

    zoom *= zoom_factor;
}
