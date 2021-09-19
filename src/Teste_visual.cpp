#include <vector>
#include <ncurses.h>
#include "CamadaFisica.hpp"
#include <math.h>

#ifdef _WIN32
    #include <windows.h>
#else
    #include <unistd.h>
#endif // _WIN32
const char* names[]= {"LOW","NEUTRAL","HIGH"};
int main()
{
    WINDOW *screen,*signal_w,*sender_w,*receiver_w;
    screen = initscr();
    noecho();
    keypad(screen,1);
    timeout(0);

    int w,h;
    getmaxyx(screen,h,w);
    signal_w = subwin(screen,h/3,w,2*h/3,0);
    sender_w = subwin(screen,2*h/3,w/2,0,0);
    receiver_w = subwin(screen,2*h/3,w/2,0,w/2);
    box(signal_w,0,0);

    mvwaddstr(signal_w,1,1,"Banana");
    refresh();

    std::vector<double> transmission(w-2,0);

    Modulador* envia = new Modulador(0.6,3);
    Demodulador* recebe = new Demodulador(M_PI,.6,3,4,3,.9);

    char key;
    int i;
    char adjusting = 0;
    double diff = 0;
    while((key=getch())!='q')
    {
        switch (key)
        {
            case 'a':
                envia->S=ALTO;
            break;
            case 'c':
                envia->S=NEUTRO;
            break;
            case 'b':
                envia->S=BAIXO;
            break;
            case 'p':
                adjusting=!adjusting;
            break;
            default:
            break;
        }
        envia->passatempo();
        if(adjusting)
        {
            diff = recebe->ajusta_fase(transmission.at(transmission.size()-1));
            if(diff<0.01)
                adjusting=0;
        }
        else
            recebe->recebe_amostra(transmission.at(transmission.size()-1));
        for(i=transmission.size()-1;i>0;i--)
            transmission.at(i)=transmission.at(i-1);
        transmission.at(0)=envia->saida;
        erase();
        box(receiver_w,0,0);
        mvwaddstr(receiver_w,1,1,"Current State");
        mvwaddstr(receiver_w,2,1,names[recebe->S]);
        mvwprintw(receiver_w,5,1,"V: %.3lf",recebe->media_movel);
        mvwprintw(receiver_w,6,1,"Phase: %.3lf",recebe->fase);
        mvwprintw(receiver_w,7,1,"Error at HIGH state: %.3lf",diff);
        mvwprintw(receiver_w,10,1,"%s",adjusting?"ADJUSTING":"READING");
        box(sender_w,0,0);
        mvwaddstr(sender_w,1,1,"Current State");
        mvwaddstr(sender_w,2,1,names[envia->S]);
        mvwprintw(sender_w,5,1,"V: %.3lf",envia->saida);
        box(signal_w,0,0);
        for(i=1;i<w-1;i++)
        {
            int height = (h/3)*((transmission.at(i-1)+4)/7);
            mvwaddch(signal_w,height,i,'*');
        }
        refresh();
        #ifdef _WIN32
            Sleep(1000/30);
        #else
            usleep(1000/30 * 1000);
        #endif // _WIN32
    }
    delwin(signal_w);
    echo();
    keypad(screen,0);
    delwin(screen);
    endwin();
    return 0;
}