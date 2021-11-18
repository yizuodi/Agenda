#include <iostream>
#include <string>
#include <iomanip>
using namespace std;
#include "AgendaUI.hpp"

AgendaUI::AgendaUI()
{
    startAgenda();
}

void AgendaUI::OperationLoop(void)
{
    cout << "Welcome to use Agenda 0.1 !" << endl;
    cout << "Email : di@yizuodi.cn" <<endl;
    string opcode;
    do
    {
        cout << "-------------------------------------Agenda-------------------------------------" << endl;
        cout << "Action :" << endl;
        cout << "l  - log in Agenda by user name and password" << endl;
        cout << "r  - register an Agenda account" << endl;
        cout << "q  - quit Agenda" << endl;
        cout << "--------------------------------------------------------------------------------" << endl
             << endl;
        cout << "Agenda :~$ ";
        cin >> opcode;
        switch (opcode[0])
        {
        case 'l':
            userLogIn();
            break;
        case 'r':
            userRegister();
            break;
        case 'q':
            quitAgenda();
            break;
        default:
            cout << "Error code! Please try again." << endl;
            cout << endl;
            break;
        }
    } while (opcode[0] != 'q');
}

/**
     * constructor
     */
void AgendaUI::startAgenda(void)
{
    m_agendaService.startAgenda(); //or do "new"?
}

/**
     * catch user's operation
     * @return the operation
     */
std::string AgendaUI::getOperation()
{
    string opcode;
    cout << "-------------------------------------Agenda-------------------------------------" << endl;
    cout << "Action :" << endl;
    cout << "o    - log out Agenda" << endl;
    cout << "dc   - delete Agenda user" << endl;
    cout << "lu   - list all Agenda user" << endl;
    cout << "cm   - create a meeting" << endl;
    cout << "amp  - add meeting participator" << endl;
    cout << "rmp  - remove meeting participator" << endl;
    cout << "rqm  - request to quit meeting" << endl;
    cout << "la   - list all meetings" << endl;
    cout << "las  - list all sponsor meetings" << endl;
    cout << "lap  - list all participator meetings" << endl;
    cout << "qm   - query meeting by title" << endl;
    cout << "qt   - query meeting by time interval" << endl;
    cout << "dm   - delete meeting by title" << endl;
    cout << "da   - delete all meetings" << endl;
    cout << "--------------------------------------------------------------------------------" << endl;
    cout << endl;
    cout << "Agenda@" << m_userName << " :~# ";
    cin >> opcode;
    return opcode;
}

/**
     * execute the operation
     * @return if the operationloop continue
     */
bool AgendaUI::executeOperation(const std::string &t_operation)
{
    if (t_operation == "o")
    {
        userLogOut();
        return false;
    }
    else if (t_operation == "dc")
    {
        deleteUser();
        return false;
    }
    else if (t_operation == "lu")
    {
        listAllUsers();
        return true;
    }
    else if (t_operation == "cm")
    {
        createMeeting();
        return true;
    }
    else if (t_operation == "amp")
    {
        addMeetingParticipator();
        return true;
    }
    else if (t_operation == "rmp")
    {
        removeMeetingParticipator();
        return true;
    }
    else if (t_operation == "rqm")
    {
        quitMeeting();
        return true;
    }
    else if (t_operation == "la")
    {
        listAllMeetings();
        return true;
    }
    else if (t_operation == "las")
    {
        listAllSponsorMeetings();
        return true;
    }
    else if (t_operation == "lap")
    {
        listAllParticipateMeetings();
        return true;
    }
    else if (t_operation == "qm")
    {
        queryMeetingByTitle();
        return true;
    }
    else if (t_operation == "qt")
    {
        queryMeetingByTimeInterval();
        return true;
    }
    else if (t_operation == "dm")
    {
        deleteMeetingByTitle();
        return true;
    }
    else if (t_operation == "da")
    {
        deleteAllMeetings();
        return true;
    }
    else
    {
        cout << "Error code! Please try again." << endl;
        return true;
    }
}

/**
     * user Login
     */
void AgendaUI::userLogIn(void)
{
    cout << endl;
    cout << "[log in] [user name] [password]" << endl;
    cout << "[log in] ";
    string username, password;
    cin >> username >> password;
    if (m_agendaService.userLogIn(username, password))
    {
        cout << "[log in] succeed!" << endl;
        cout << endl;
        m_userName = username;
        m_userPassword = password;
        while (executeOperation(getOperation()))
            ;
    }
    else
        cout << "[log in] Password error or user doesn't exist" << endl;
}

/**
     * user regist
     */
void AgendaUI::userRegister(void)
{
    cout << endl;
    cout << "Notice : You can type 'op$q' to stop registering." << endl;
    cout << "So none of the following four terms should equal 'op$q'!!!" << endl;
    cout << endl;
    cout << "[register] [user name] [password] [email] [phone]" << endl;
    cout << "[register] ";
    string username, password, email, phone;
    cin >> username >> password >> email >> phone;
    if ((username == "op$q"||password == "op$q")||(email == "op$q"||phone == "op$q"))
    {
        cout << "The creation process has stopped." << endl;
        return;
    }
    /*if (username == "op$q")
    {
        cout << "This username was banned.Please restart the registration process." << endl;
        return;
    }*/
    if (m_agendaService.userRegister(username, password, email, phone))
        cout << "[register] succeed!" << endl;
    else
        cout << "[register] This username has been registered!" << endl;
}

/**
     * user logout
     */
void AgendaUI::userLogOut(void)
{
    m_userName = "";
    m_userPassword = "";
    cout << "Logout successful." << endl;
    cout << endl;
}

/**
     * quit the Agenda
     */
void AgendaUI::quitAgenda(void)
{
    m_agendaService.quitAgenda();
    cout << "Thanks for using Agenda!" << endl;
    cout << "Quit Agenda successful." << endl;
}

/**
     * delete a user from storage
     */
void AgendaUI::deleteUser(void)
{
    if (m_agendaService.deleteUser(m_userName, m_userPassword))
    {
        cout << endl;
        cout << "[delete agenda account] succeed!" << endl;
    }
    else
    {
        cout << "Unexpected errors. Please contact us." << endl;
    }
    userLogOut();
}

/**
     * list all users from storage
     */
void AgendaUI::listAllUsers(void)
{
    list<User> userlist = m_agendaService.listAllUsers();
    cout << endl;
    cout << "[list all users]" << endl;
    cout << endl;
    cout << left << setw(15) << "name" << left << setw(20) << "email" << left << setw(20) << "phone" << endl;
    for (auto i = userlist.begin(); i != userlist.end(); i++)
    {
        cout << left << setw(15) << (*i).getName() << left << setw(20) << (*i).getEmail() << left << setw(20) << (*i).getPhone() << endl;
    }
}

/**
     * user create a meeting with someone else
     */
void AgendaUI::createMeeting(void)
{
    cout << endl;
    cout << "Notice : You can type 'op$q' to stop creating a meeting." << endl;
    cout << "So none of the following terms should equal 'op$q'!!!" << endl;
    cout << endl;
    cout << "[create meeting]  [the number of participators]" << endl;
    cout << "[create meeting] ";
    string countinput;
    cin >> countinput;
    if (countinput == "op$q")
    {
        cout << "The creation process has stopped." << endl;
        return;
    }
    int count = std::stoi(countinput);
    vector<string> par;
    for (int i = 0; i < count; i++)
    {
        cout << "[create meeting] [please enter the participator " << i + 1 << " ]" << endl;
        string name;
        cin >> name;
        if (name == "op$q")
        {
            cout << "The creation process has stopped." << endl;
            return;
        }
        par.push_back(name);
    }
    cout << "[create meeting] [title] [start time(yyyy-mm-dd/hh:mm)] [end time(yyyy-mm-dd/hh:mm)]" << endl;
    cout << "[create meeting] [title] ";
    string title, start, end;
    cin >> title >> start >> end;
    if ((title == "op$q" || start == "op$q")||(end == "op$q"))
    {
        cout << "The creation process has stopped." << endl;
        return;
    }

    if (m_agendaService.createMeeting(m_userName, title, start, end, par))
        cout << "[create meeting] succeed!" << endl;
    else
        cout << "[create meeting] error!" << endl;
}

void AgendaUI::addMeetingParticipator(void)
{
    cout << endl
         << "[add participator] [meeting title] [participator username]" << endl;
    cout << "[add participator] ";
    string title, participator;
    cin >> title >> participator;
    if (m_agendaService.addMeetingParticipator(m_userName, title, participator))
        cout << "[add participator] succeed!" << endl;
    else
        cout << "[add participator] error!" << endl;
}

void AgendaUI::removeMeetingParticipator(void)
{
    cout << endl;
    cout << "[remove participator] [meeting title] [participator username]" << endl;
    cout << "[remove participator] ";
    string title, participator;
    cin >> title >> participator;
    if (m_agendaService.removeMeetingParticipator(m_userName, title, participator))
        cout << "[remove participator] succeed!" << endl;
    else
        cout << "[remove participator] error!" << endl;
}

void AgendaUI::quitMeeting(void)
{
    cout << endl;
    cout << "[quit meeting] [meeting title]" << endl;
    cout << "[quit meeting] ";
    string title;
    cin >> title;
    if (m_agendaService.quitMeeting(m_userName, title))
        cout << "[quitMeeting] succeed!" << endl;
    else
        cout << "[quitMeeting] error!" << endl;
}
/**
     * list all meetings from storage
     */

void AgendaUI::listAllMeetings(void)
{
    cout << endl;
    cout << "[list all meetings]" << endl;
    cout << endl;
    list<Meeting> meetinglist = m_agendaService.listAllMeetings(m_userName);
    printMeetings(meetinglist);
}

/**
     * list all meetings that this user sponsored
     */
void AgendaUI::listAllSponsorMeetings(void)
{
    cout << endl;
    cout << "[list all sponsor meetings]" << endl;
    cout << endl;
    list<Meeting> meetinglist = m_agendaService.listAllSponsorMeetings(m_userName);
    printMeetings(meetinglist);
}

/**
     * list all meetings that this user take part in
     */
void AgendaUI::listAllParticipateMeetings(void)
{
    cout << endl;
    cout << "[list all participator meetings]" << endl;
    cout << endl;
    list<Meeting> meetinglist = m_agendaService.listAllParticipateMeetings(m_userName);
    printMeetings(meetinglist);
}

/**
     * search meetings by title from storage
     */
void AgendaUI::queryMeetingByTitle(void)
{
    cout << endl;
    cout << "[query meeting] [title]:" << endl;
    cout << "[query meeting] ";
    string title;
    cin >> title;
    list<Meeting> meetinglist = m_agendaService.meetingQuery(m_userName, title);
    printMeetings(meetinglist);
}

/**
     * search meetings by timeinterval from storage
     */
void AgendaUI::queryMeetingByTimeInterval(void)
{
    cout << endl;
    cout << "[query meetings] [start time(yyyy-mm-dd/hh:mm)] [end time(yyyy-mm-dd/hh:mm)]" << endl;
    cout << "[query meetings] ";
    string start, end;
    cin >> start >> end;
    cout << "[query meetings]" << endl;
    list<Meeting> meetinglist = m_agendaService.meetingQuery(m_userName, start, end);
    printMeetings(meetinglist);
}

/**
     * delete meetings by title from storage
     */
void AgendaUI::deleteMeetingByTitle(void)
{
    cout << endl;
    cout << "[delete meeting] [title]" << endl;
    cout << "[delete meeting] ";
    string title;
    cin >> title;
    if (m_agendaService.deleteMeeting(m_userName, title))
        cout << "[delete meeting] succeed!" << endl;
    else
        cout << "[error] delete meeting fail!" << endl;
}

/**
     * delete all meetings that this user sponsored
     */
void AgendaUI::deleteAllMeetings(void)
{
    if (m_agendaService.deleteAllMeetings(m_userName))
        cout << "[delete all meeting] succeed!" << endl;
    else
        cout << "[error] delete meeting(s) fail!" << endl;
}

/**
     * show the meetings in the screen
     */
void AgendaUI::printMeetings(const std::list<Meeting> &t_meetings)
{
    cout << left << setw(15) << "title" << left << setw(15) << "sponsor" << left << setw(18) << "start time" << left << setw(18) << "end time" << left << setw(20) << "participators" << endl;
    for (auto i = t_meetings.begin(); i != t_meetings.end(); i++)
    {
        vector<string> tmp = (*i).getParticipator();
        string participators;
        for (int j = 0; j < tmp.size() - 1; j++)
        {
            participators += tmp[j];
            participators += ", ";
        }
        participators += tmp[tmp.size() - 1];
        cout << left << setw(15) << (*i).getTitle() << left << setw(15) << (*i).getSponsor() << left << setw(18) << Date::dateToString((*i).getStartDate()) << left << setw(18) << Date::dateToString((*i).getEndDate()) << left << setw(20) << participators << endl;
    }
}