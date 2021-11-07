#include "AgendaService.hpp"
#include <list>
#include <string>
#include <iostream>
using namespace std;
/**
   * constructor
   */
AgendaService::AgendaService()
{
    startAgenda();
}

/**
   * destructor
   */
AgendaService::~AgendaService()
{
    quitAgenda();
}

/**
   * check if the username match password
   * @param userName the username want to login
   * @param password the password user enter
   * @return if success, true will be returned
   */
bool AgendaService::userLogIn(const string &userName, const string &password)
{
    list<User> v = m_storage->queryUser([&](const User &userq)
                                        { return (userq.getName() == userName && userq.getPassword() == password); });
    return (v.size() != 0);
}

/**
   * regist a user
   * @param userName new user's username
   * @param password new user's password
   * @param email new user's email
   * @param phone new user's phone
   * @return if success, true will be returned
   */
bool AgendaService::userRegister(const std::string &userName, const std::string &password,
                                 const std::string &email, const std::string &phone)
{
    User newuser(userName, password, email, phone);
    //只需要验证用户名是否唯一,我们约定，仅用户名不允许相同
    list<User> v = m_storage->queryUser([&](const User &userq)
                                        { return (userq.getName() == userName); });
    if (v.size() != 0)
    {
        return false;
    }
    m_storage->createUser(newuser);
    return m_storage->sync();
}

/**
   * delete a user
   * @param userName user's username
   * @param password user's password
   * @return if success, true will be returned
   */
bool AgendaService::deleteUser(const std::string &userName, const std::string &password)
{
    //删除的用户名和密码与用户输入对应的数量
    int count = m_storage->deleteUser([&](const User &userq)
                                      { return (userq.getName() == userName && userq.getPassword() == password); });
    if (count == 0)
    {
        return false;
    }
    //还得删除该用户创建的会议和从参与的会议中移除该人
    //先直接删除该用户创建的会议
    m_storage->deleteMeeting([&](const Meeting &meetingq)
                             { return (meetingq.getSponsor() == userName); });
    //然后维护一下剩下的会议的数据
    m_storage->updateMeeting([&](const Meeting &meetingq)
                             { return (meetingq.isParticipator(userName)); },
                             [&](Meeting &meetingqq)
                             { meetingqq.removeParticipator(userName); });
    //删除掉已经没人参与的会议
    m_storage->deleteMeeting([&](const Meeting &meetingq)
                             { return (meetingq.getParticipator().size() == 0); });
    return m_storage->sync();
}

/**
   * list all users from storage
   * @return a user list result
   */
list<User> AgendaService::listAllUsers(void) const
{
    list<User> userdata = m_storage->queryUser([&](const User &userq)
                                               { return true; });
    return userdata;
}

/**
   * create a meeting
   * @param userName the sponsor's userName
   * @param title the meeting's title
   * @param participator the meeting's participator
   * @param startData the meeting's start date
   * @param endData the meeting's end date
   * @return if success, true will be returned
   */
bool AgendaService::createMeeting(const std::string &userName, const std::string &title,
                                  const std::string &startDate, const std::string &endDate,
                                  const std::vector<std::string> &participator)
{
    //先验证输入的时间是否合法
    Date startDatetmp(startDate);
    Date endDatetmp(endDate);
    if (!(Date::isValid(startDatetmp) && Date::isValid(endDatetmp)))
    {
        return false;
    }
    if (startDatetmp >= endDatetmp)
    {
        return false;
    }
    //再验证输入的title是否已经存在
    if ((m_storage->queryMeeting([&](const Meeting &meetingq){ return meetingq.getTitle() == title; })).size() != 0)
    {
        return false;//has had
    }
    //再看userName是否有此人
    if ((m_storage->queryUser([&](const User &userq){ return userq.getName() == userName; })).size() == 0)
    {
        return false;
    }
    if (participator.empty())
    {
        return false;
    }
    for (int i = 0; i < participator.size(); i++)
    {
        for (int j = 0; j < participator.size(); j++)
        {
            if (i != j && participator[i] == participator[j])
            {
                return false;
            }
        }
    }
    for(int i=0;i<participator.size();++i){
        if ((m_storage->queryUser([&](const User &userq){ return userq.getName() == participator[i]; })).size() == 0){
            return false;
        }
    }
    //如果创建人参会，说明有问题
    for(int i=0;i<participator.size();++i){
        if (participator[i] == userName)
        {
            return false;
        }
    }
    //再处理participator time
    list<Meeting> stmp = m_storage->queryMeeting([&](const Meeting& meetingq){
			if(meetingq.getSponsor()==userName ||meetingq.isParticipator(userName)){
				if(!(startDatetmp>=meetingq.getEndDate()||endDatetmp<=meetingq.getStartDate())){
					return true;
				}
			}
			return false;
		});
    if(stmp.size()!=0){
            return false;
    }

    for(int i=0;i<participator.size();++i){
        list<Meeting> tmp = m_storage->queryMeeting([&](const Meeting& meetingq){
			if(meetingq.getSponsor()==participator[i]||meetingq.isParticipator(participator[i])){
				if(!(startDatetmp>=meetingq.getEndDate()||endDatetmp<=meetingq.getStartDate())){
					return true;
				}
			}
			return false;
		});
        if(tmp.size()!=0){
            return false;
        }
    }
    Meeting newmeeting(userName, participator,startDatetmp,endDatetmp,title);
    //执行到这里说明没问题，所有参会者都录入了
    m_storage->createMeeting(newmeeting);
    return m_storage->sync();
}

/**
   * add a participator to a meeting
   * @param userName the sponsor's userName
   * @param title the meeting's title
   * @param participator the meeting's participator
   * @return if success, true will be returned
   */
bool AgendaService::addMeetingParticipator(const std::string &userName,
                                           const std::string &title,
                                           const std::string &participator)
{
    //先看userName和participator是否有此人
    if ((m_storage->queryUser([&](const User &userq)
                              { return userq.getName() == userName; }))
            .size() == 0)
    {
        return false;
    }
    if ((m_storage->queryUser([&](const User &userq)
                              { return userq.getName() == participator; }))
            .size() == 0)
    {
        return false;
    }
    //检查userName-title的组合是否存在对应的会议，且添加的成员是否尚未参与该会议
    list<Meeting> meetingdatatmp = m_storage->queryMeeting([&](const Meeting &meetingq)
                                                           { return (meetingq.getTitle() == title && meetingq.getSponsor() == userName) && (!meetingq.isParticipator(participator)); });
    if (meetingdatatmp.size() != 1)
    {
        return false;
    }
    //检查添加的会议参与者是否是会议发起人
    if (userName == participator)
    {
        return false;
    }
    //下面检查待添加的成员是否有时间
    Meeting tmp = meetingdatatmp.front();
    Date startdate = tmp.getStartDate();
    Date enddate = tmp.getEndDate();
    list<Meeting> meetingtmp = m_storage->queryMeeting([&](const Meeting &meetingq)
                                                       {
                                                           if (meetingq.getSponsor() == participator || meetingq.isParticipator(participator))
                                                           {
                                                               if (meetingq.getStartDate() >= enddate || meetingq.getEndDate() <= startdate)
                                                               {
                                                                   return false;
                                                               }
                                                               else
                                                                   return true;
                                                           }
                                                           else
                                                               return false;
                                                       });
    if (meetingtmp.size() != 0)
    {
        return false;
    }
    //终于没问题了，添加该用户
    int count = m_storage->updateMeeting([&](const Meeting &meetingq)
                                         { return meetingq.getTitle() == title; },
                                         [&](Meeting &meetingqq)
                                         { meetingqq.addParticipator(participator); });
    //万一添加不成功呢
    if (count != 0)
    {
        return m_storage->sync();
    }
    return false;
}

/**
   * remove a participator from a meeting
   * @param userName the sponsor's userName
   * @param title the meeting's title
   * @param participator the meeting's participator
   * @return if success, true will be returned
   */
bool AgendaService::removeMeetingParticipator(const std::string &userName,
                                              const std::string &title,
                                              const std::string &participator)
{
    int count = m_storage->updateMeeting([&](const Meeting &meetingq)
                                         { return (meetingq.isParticipator(participator) && meetingq.getSponsor() == userName) && meetingq.getTitle() == title; },
                                         [&](Meeting &meetingqq)
                                         { meetingqq.removeParticipator(participator); });
    if (count != 0)
    {
        //如果删除成功，检查是否有因此没有参与者的会议(若因此造成会议 参与者 人数为 0，则会议也将被删除)
        m_storage->deleteMeeting([&](const Meeting &meetingq)
                                 { return meetingq.getParticipator().size() == 0; });
        return m_storage->sync();
    }
    return false;
}

/**
   * quit from a meeting 发起人不能退出他或她的会议
   * @param userName the current userName. need to be the participator (a sponsor can not quit his/her meeting)
   * @param title the meeting's title
   * @return if success, true will be returned
   */
bool AgendaService::quitMeeting(const std::string &userName, const std::string &title)
{
    int count = m_storage->updateMeeting([&](const Meeting &meetingq)
                                         { return (meetingq.isParticipator(userName) && meetingq.getSponsor() != userName) && meetingq.getTitle() == title; },
                                         [&](Meeting &meetingqq)
                                         { meetingqq.removeParticipator(userName); });
    if (count != 0)
    {
        //如果删除成功，检查是否有因此没有参与者的会议(若因此造成会议 参与者 人数为 0，则会议也将被删除)
        m_storage->deleteMeeting([&](const Meeting &meetingq)
                                 { return meetingq.getParticipator().size() == 0; });
        return m_storage->sync();
    }
    return false;
}

/**
   * search a meeting by username and title 
   * @param userName as a sponsor OR a participator
   * @param title the meeting's title
   * @return a meeting list result
   */
list<Meeting> AgendaService::meetingQuery(const std::string &userName,
                                          const std::string &title) const
{
    list<Meeting> meetingdata = m_storage->queryMeeting([&](const Meeting &meetingq)
                                                        { return (meetingq.isParticipator(userName) || meetingq.getSponsor() == userName) && meetingq.getTitle() == title; });
    return meetingdata;
}
/**
   * search a meeting by username, time interval
   * @param userName as a sponsor OR a participator
   * @param startDate time interval's start date
   * @param endDate time interval's end date
   * @return a meeting list result
   */
list<Meeting> AgendaService::meetingQuery(const std::string &userName,
                                          const std::string &startDate,
                                          const std::string &endDate) const
{
    Date startDatetmp(startDate);
    Date endDatetmp(endDate);
    list<Meeting> meetingdata;
    if (!(Date::isValid(startDatetmp) && Date::isValid(endDatetmp)))
    {
        return meetingdata;
    }
    meetingdata = m_storage->queryMeeting([&](const Meeting &meetingq)
                                          {
                                              if (meetingq.isParticipator(userName) || meetingq.getSponsor() == userName)
                                              {
                                                  if (!((meetingq.getStartDate() > endDatetmp) || (meetingq.getEndDate() < startDatetmp)))
                                                  {
                                                      return true;
                                                  }
                                              }
                                              return false;
                                          });
    return meetingdata;
}

/**
   * list all meetings the user take part in
   * @param userName user's username
   * @return a meeting list result
   */
list<Meeting> AgendaService::listAllMeetings(const std::string &userName) const
{
    list<Meeting> meetingdata = m_storage->queryMeeting([&](const Meeting &meetingq)
                                                        { return meetingq.isParticipator(userName) || meetingq.getSponsor() == userName; });
    return meetingdata;
}

/**
   * list all meetings the user sponsor
   * @param userName user's username
   * @return a meeting list result
   */
list<Meeting> AgendaService::listAllSponsorMeetings(const std::string &userName) const
{
    list<Meeting> meetingdata = m_storage->queryMeeting([&](const Meeting &meetingq)
                                                        { return meetingq.getSponsor() == userName; });
    return meetingdata;
}

/**
   * list all meetings the user take part in and sponsor by other
   * @param userName user's username
   * @return a meeting list result
   */
list<Meeting> AgendaService::listAllParticipateMeetings(
    const std::string &userName) const
{
    list<Meeting> meetingdata = m_storage->queryMeeting([&](const Meeting &meetingq)
                                                        { return meetingq.isParticipator(userName); });
    return meetingdata;
}

/**
   * delete a meeting by title and its sponsor
   * @param userName sponsor's username
   * @param title meeting's title
   * @return if success, true will be returned
   */
bool AgendaService::deleteMeeting(const std::string &userName, const std::string &title)
{
    int count = m_storage->deleteMeeting([&](const Meeting &meetingq)
                                         { return meetingq.getSponsor() == userName && meetingq.getTitle() == title; });
    if (count != 0)
    {
        return m_storage->sync();
    }
    return false;
}

/**
   * delete all meetings by sponsor
   * @param userName sponsor's username
   * @return if success, true will be returned
   */
bool AgendaService::deleteAllMeetings(const std::string &userName)
{
    int count = m_storage->deleteMeeting([&](const Meeting &meetingq)
                                         { return meetingq.getSponsor() == userName; });
    if (count != 0)
    {
        return m_storage->sync();
    }
    return false;
}

/**
   * start Agenda service and connect to storage
   */
void AgendaService::startAgenda(void)
{
    m_storage = Storage::getInstance();
}

/**
   * quit Agenda service
   */
void AgendaService::quitAgenda(void)
{
    m_storage->sync();
}
