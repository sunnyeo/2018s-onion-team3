
#include "common.h"
#include "onionui.h"
#include "message.h"

using namespace newmsg;
int curX = 0, curY = 0;
vector<string> msgList;

namespace oniui{
    OnionUI::OnionUI(){}
    OnionUI::~OnionUI(){}

    static char *onionlogo = (char *)"\
██╗  ██╗███████╗██████╗ \n\
██║  ██║██╔════╝██╔══██╗\n\
███████║█████╗  ██████╔╝\n\
██╔══██║██╔══╝  ██╔══██╗\n\
██║  ██║███████╗██║  ██║\n\
╚═╝  ╚═╝╚══════╝╚═╝  ╚═╝";

    static char *onionmemu = (char *)"\n1. List\n2. Talk\n3. Exit\n> ";

    void OnionUI::MainUI(){
        OnionUI *ui = new OnionUI();
        ui->Init();
        char value = 0;
        while(1){
            ui->ShowMenu();
            cin >> value;
            switch(value){
            case '1':
            {
                g_km->ShowList();
            }
            break;
            case '2':
            {

                WINDOW *listWin, *chatWin;

                /*cout << "=====================================" << endl;
                cout << "Who do you want to talk to?" << endl;
                g_km->ShowList();
                cout << "githubID> ";
                string githubID;
                cin >> githubID; // validation check routine ++++*/

                int maxX = 0, maxY = 0;
                int listX = 0, listY = 0;
                int highlight = 1;
                int choice = 0;
                int c;
                int listLen = nodeMap->size() + 2;
                msgList.clear();
                initscr();
                //raw();
                clear();
                noecho();
                cbreak();
                getmaxyx(stdscr, maxY, maxX);

                listX = (maxX - 30) / 2;
                listY = (maxY - 10) / 2;
                listWin = newwin(10, 30, listY, listX);
                keypad(listWin, true);
                //scrollok(listWin, true);
                refresh();

                while(1) {
                    int x, y, i = 1;
                    x = 2;
                    y = 2;
                    box(listWin, 0, 0);
                    if(highlight == i) {
                        wattron(listWin, A_REVERSE);
                        mvwprintw(listWin, y, x, "%s", "Me");
                        wattroff(listWin, A_REVERSE);
                    } else {
                        mvwprintw(listWin, y, x, "%s", "Me");
                    }
                    ++y;
                    for(nodeIter = nodeMap->begin(); nodeIter != nodeMap->end(); nodeIter++)
                    {
                        i++;
                        if(highlight == i + 1) /* High light the present choice */
                        {	wattron(listWin, A_REVERSE);
                            mvwprintw(listWin, y, x, "%s", nodeIter->first);
                            wattroff(listWin, A_REVERSE);
                        }
                        else
                            mvwprintw(listWin, y, x, "%s", nodeIter->first);
                        ++y;
                    }
                    i++;
                    if(highlight == i) {
                        wattron(listWin, A_REVERSE);
                        mvwprintw(listWin, y, x, "%s", "Exit");
                        wattroff(listWin, A_REVERSE);
                    } else {
                        mvwprintw(listWin, y, x, "%s", "Exit");
                    }
                    ++y;
                    wmove(listWin, maxY, maxX);
                    wrefresh(listWin);
                    if(choice != 0) break;
                    c = wgetch(listWin);
                    switch(c)
                    {
                    case KEY_UP:
                        {
                            if(highlight == 1) {
                                highlight = listLen;
                            } else {
                                highlight--;
                            }
                            break;
                        }
                    case KEY_DOWN:
                        {
                            if(highlight == listLen) {
                                highlight = 1;
                            } else {
                                highlight++;
                            }
                            break;
                        }
                    case 10:
                    {
                        choice = highlight;
                        break;
                    }
                    default:
                    {
                        refresh();
                        break;
                    }
                    }

                }
                clear();
                delwin(listWin);
                if(choice < listLen) {
                    chatWin = newwin(maxY, maxX, 0, 0);
                    keypad(chatWin, true);
                    noecho();
                    wrefresh(chatWin);
                  
                    std::thread t1(OnionUI::UIRecvThread, chatWin, githubID, maxY, maxX);
                    std::thread t2(OnionUI::UISendThread, chatWin, githubID, maxY, maxX);
                    t1.join();
                    if(t2.joinable()) t2.join();
                }

                endwin();
                //ui->ShowMenu();
            }
            break;
            case '3':
            {
                g_km->SendKeyDie();
                cout << "Good Bye!" <<endl;
                sleep(3);
                exit(1);
            }
            break;
            default:
                continue;
            }
        }
    }


    void OnionUI::UIRecvThread(WINDOW* win, string githubID, int maxY, int maxX) {
        Message *msg = new Message();
        while(1){
            if(qRecvMsg.empty() == 0) {
                string str = qRecvMsg.front();
                json tmp;
                tmp = json::parse(str);
                string tmp_content = tmp.at("content").get<std::string>();
                string tmp_ip = tmp.at("sendip").get<std::string>();
                string tmp_githubID = g_km->FindgithubID(tmp_ip);
                if(tmp_githubID.compare(githubID) == 0){
                    msgList.push_back("You: " + tmp_content + "\n");
                    clear();
                    mvprintw(0, 0, onionlogo);
                    mvprintw(maxY - 1, 0, ">");
                    refresh();
                    qRecvMsg.pop();
                }
                else{
                    continue;
                }
            }
        }
    }


    void OnionUI::UISendThread(WINDOW *win, string githubID, int maxY, int maxX) {
        Message *msg = new Message();
        string str = "";
        //keypad(win, true);
        mvwprintw(win, 0, 0, "Her");
        k_mutex.lock();
        curY = 1;
        k_mutex.unlock();
        while(1) {
            //string str;
            mvwprintw(win, maxY - 1, 0, ">");
            wprintw(win, str.c_str());
            //getstr(&str[0]);
            wrefresh(win);
            int input = wgetch(win);
            //mvwprintw(win, 0, 5, "pressed : %d", input);
            //wmove(win, maxY -1, str.length() + 1);
            if(input >= 0x20 && input <= 0x7e) {
                str.push_back(input);
            } else if(input == KEY_UP) {
                k_mutex.lock();
                if(msgList.size() > 22 && curY > 23) {
                    curY--;
                }
                k_mutex.unlock();
            } else if(input == KEY_DOWN) {
                k_mutex.lock();
                if(msgList.size() > 22 && curY < msgList.size() + 1) {
                    curY++;
                }
                k_mutex.unlock();
            } else if(input == KEY_EXIT) {
                break; //return;
            } else if(input == 10) {
                k_mutex.lock();
                msgList.push_back("Me: " + str + "\n");
                curY = msgList.size() + 1;
                k_mutex.unlock();
                string tmp_ip = g_km->Findip(githubID);
                msg->SetMessage(githubID, tmp_ip, str);
                msg->SendMessage();
                str = "";
            }
            wclear(win);
            wrefresh(win);
            mvwprintw(win, 0, 0, "Her");
            wmove(win, 1, 0);
            if(curY >= maxY - 1) {
                for( int i = curY - maxY + 1; i < curY - 1 ; i++ ) {
                    wprintw(win, msgList.at(i).c_str());
                    wrefresh(win);
                }
            } else {
                for( int i = 0 ; i < msgList.size(); i++ ) {
                    wprintw(win, msgList.at(i).c_str());
                }
            }


        }
    }

    void OnionUI::Init(){
        cout << onionlogo;
    }

    void OnionUI::ShowLogo(){
        cout << onionlogo;
    }

    void OnionUI::ShowMenu(){
        cout << onionmemu;
    }

}
