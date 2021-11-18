#include "AgendaService.hpp"
#include <list>
#include <string>
#include "Date.hpp"
#include "Meeting.hpp"
#include "User.hpp"
#include <algorithm>
#include <iostream>

/**
 * constructor
 */
AgendaService::AgendaService() { startAgenda(); }

/**
 * destructor
 */
AgendaService::~AgendaService() { quitAgenda(); }

/**
 * start Agenda service and connect to storage
 */
void AgendaService::startAgenda(void) { m_storage = Storage::getInstance(); }

/**
 * quit Agenda service
 */
void AgendaService::quitAgenda(void) {}

/**
 * check if the username match password
 * @param userName the username want to login
 * @param password the password user enter
 * @return if success, true will be returned
 */
bool AgendaService::userLogIn(const std::string &userName,
                              const std::string &password) {
  std::list<User> l;
  User temp(userName, password, "", "");
  l = m_storage->queryUser([&temp](const User &a) {
    if (a.getName() == temp.getName())
      return true;
    else
      return false;
  });
  if (l.empty()) return false;
  if (password == l.begin()->getPassword())
    return true;
  else
    return false;
}

/**
 * regist a user
 * @param userName new user's username
 * @param password new user's password
 * @param email new user's email
 * @param phone new user's phone
 * @return if success, true will be returned
 */
bool AgendaService::userRegister(const std::string &userName,
                                 const std::string &password,
                                 const std::string &email,
                                 const std::string &phone) {
  std::list<User> l;
  User temp(userName, password, email, phone);
  l = m_storage->queryUser([&temp](const User &a) {
    if (a.getName() == temp.getName())
      return true;
    else
      return false;
  });
  if (!l.empty()) return false;
  m_storage->createUser(temp);
  return true;
}

/**
 * delete a user
 * @param userName user's username
 * @param password user's password
 * @return if success, true will be returned
 */
bool AgendaService::deleteUser(const std::string &userName,
                               const std::string &password) {
  std::list<User> l;
  l = m_storage->queryUser([&userName](const User &a) {
    if (a.getName() == userName)
      return true;
    else
      return false;
  });
  if (l.empty()) return false;
  if (password != l.begin()->getPassword()) return false;
  m_storage->deleteUser([&userName](const User &a) {
    if (a.getName() == userName)
      return true;
    else
      return false;
  });
  // 在参与的会议中删除用户
  auto func_findMeeting = [&userName](const Meeting &meeting) -> bool {
    return (meeting.isParticipator(userName));
  };
  auto func_modifyMeeting = [&userName](Meeting &meeting) {
    meeting.removeParticipator(userName);
  };
  m_storage->updateMeeting(func_findMeeting, func_modifyMeeting);
  // 删除用户发起的会议 和 删除由于上一操作导致只剩发起者的会议
  auto func_delMeeting = [&userName](const Meeting &meeting) -> bool {
    return (meeting.getSponsor() == userName ||
            meeting.getParticipator().empty());
  };
  m_storage->deleteMeeting(func_delMeeting);
  return true;
}

/**
 * list all users from storage
 * @return a user list result
 */
std::list<User> AgendaService::listAllUsers(void) const {
  std::list<User> l;
  l = m_storage->queryUser([](const User &a) { return true; });
  return l;
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
bool AgendaService::createMeeting(
    const std::string &userName, const std::string &title,
    const std::string &startDate, const std::string &endDate,
    const std::vector<std::string> &participator) {
  for (auto it = participator.begin(); it != participator.end(); it++) {
    for (auto itj = participator.begin(); itj != it; itj++) {
      if ((*itj) == (*it)) return false;
    }
  }
  Date sTime;
  Date eTime;
  sTime = Date::stringToDate(startDate);
  eTime = Date::stringToDate(endDate);
  if (!Date::isValid(sTime)) return false;
  if (!Date::isValid(eTime)) return false;
  if (sTime >= eTime) return false;
  std::list<User> lu;
  std::list<Meeting> l;
  for (auto it = participator.begin(); it != participator.end(); it++) {
    if (*it == userName) return false;
    std::string puserName = *it;
    lu = m_storage->queryUser([&puserName](const User &a) {
      if (a.getName() == puserName) {
        return true;
      } else {
        return false;
      }
    });
    if (lu.empty()) return false;
    l = m_storage->queryMeeting([&puserName, &sTime, &eTime](const Meeting &a) {
      if (a.getSponsor() == puserName || a.isParticipator(puserName)) {
        if (a.getStartDate() <= sTime && a.getEndDate() > sTime) return true;
        if (a.getStartDate() < eTime && a.getEndDate() >= eTime) return true;
        if (a.getStartDate() >= sTime && a.getEndDate() <= eTime) return true;
        return false;
      } else {
        return false;
      }
    });
    if (!l.empty()) return false;
  }
  lu = m_storage->queryUser([&userName](const User &a) {
    if (a.getName() == userName) {
      return true;
    } else {
      return false;
    }
  });
  if (lu.empty()) return false;
  l = m_storage->queryMeeting([&userName, &sTime, &eTime](const Meeting &a) {
    if (a.getSponsor() == userName || a.isParticipator(userName)) {
      if (a.getStartDate() <= sTime && a.getEndDate() > sTime) return true;
      if (a.getStartDate() < eTime && a.getEndDate() >= eTime) return true;
      if (a.getStartDate() >= sTime && a.getEndDate() <= eTime) return true;
      return false;
    } else {
      return false;
    }
  });
  if (!l.empty()) return false;
  l = m_storage->queryMeeting([&title](const Meeting &a) {
    if (a.getTitle() == title)
      return true;
    else
      return false;
  });
  if (!l.empty()) return false;
  Meeting temp(userName, participator, sTime, eTime, title);
  m_storage->createMeeting(temp);
  return true;
}

bool AgendaService::addMeetingParticipator(const std::string &userName,
                                           const std::string &title,
                                           const std::string &participator) {
  // 查是否存在会议
  auto checkMeetingExist = [&userName, &title](const Meeting &meeting) {
    if (userName == meeting.getSponsor() && title == meeting.getTitle())
      return true;
    return false;
  };
  auto meetings = m_storage->queryMeeting(checkMeetingExist);
  if (meetings.empty()) return false;

  // 查是否存在参与者
  auto lu = m_storage->queryUser([&participator](const User& u) {
    return u.getName() == participator;
  });
  if (lu.empty()) return false;

  auto meeting = meetings.front();

  // 查时间冲突
  auto l = m_storage->queryMeeting([&participator, &meeting](const Meeting &a) {
    if (a.getSponsor() == participator || a.isParticipator(participator)) {
      if (a.getStartDate() <= meeting.getStartDate() && a.getEndDate() > meeting.getStartDate()) return true;
      if (a.getStartDate() < meeting.getEndDate() && a.getEndDate() >= meeting.getEndDate()) return true;
      if (a.getStartDate() >= meeting.getStartDate() && a.getEndDate() <= meeting.getEndDate()) return true;
      return false;
    } else {
      return false;
    }
  });
  if (!l.empty()) return false;

  // 加参与者
  m_storage->updateMeeting(
      [&meeting](const Meeting &t_meeting) {
        if (t_meeting.getTitle() == meeting.getTitle()) return true;
        return false;
      },
      [&participator](Meeting &t_meeting) {
        t_meeting.addParticipator(participator);
      });
  return true;
}

bool AgendaService::removeMeetingParticipator(const std::string &userName,
                                              const std::string &title,
                                              const std::string &participator) {
  // 查是否存在
  auto checkMeetingExist = [&userName, &title](const Meeting &meeting) {
    if (userName == meeting.getSponsor() && title == meeting.getTitle())
      return true;
    return false;
  };
  auto meetings = m_storage->queryMeeting(checkMeetingExist);
  if (meetings.empty()) return false;

  auto meeting = meetings.front();
  // 查是否有该参与者
  auto participators = meeting.getParticipator();
  if (std::find(participators.begin(), participators.end(), participator) == participators.end()) return false;
  // 删参与者
  m_storage->updateMeeting(
      [&meeting](const Meeting &t_meeting) {
        if (t_meeting.getTitle() == meeting.getTitle()) return true;
        return false;
      },
      [&participator](Meeting &t_meeting) {
        t_meeting.removeParticipator(participator);
      });
  m_storage->deleteMeeting(
      [&meeting](const Meeting &t_meeting) {
        return t_meeting.getParticipator().empty() && t_meeting.getTitle() == meeting.getTitle();
      });
  return true;
}

bool AgendaService::quitMeeting(const std::string &userName,
                                const std::string &title) {
  // 查是否存在
  auto checkMeetingExist = [&title, &userName](const Meeting &meeting) {
    if (title == meeting.getTitle() && meeting.isParticipator(userName)) return true;
    return false;
  };
  auto meetings = m_storage->queryMeeting(checkMeetingExist);
  if (meetings.empty()) return false;

  // 删参与者
  Meeting meeting = meetings.front();
  m_storage->updateMeeting(
      [&meeting](const Meeting &t_meeting) {
        if (t_meeting.getTitle() == meeting.getTitle()) return true;
        return false;
      },
      [&userName](Meeting &t_meeting) {
        t_meeting.removeParticipator(userName);
      });
  m_storage->deleteMeeting([&meeting](const Meeting &m) {
    return m.getParticipator().empty() && m.getTitle() == meeting.getTitle();
  });
  return true;
}

/**
 * search a meeting by username and title
 * @param userName as a sponsor OR a participator
 * @param title the meeting's title
 * @return a meeting list result
 */
std::list<Meeting> AgendaService::meetingQuery(const std::string &userName,
                                               const std::string &title) const {
  std::list<Meeting> l;
  l = m_storage->queryMeeting([&userName, &title](const Meeting &a) {
    if (a.getSponsor() == userName || a.isParticipator(userName)) {
      if (a.getTitle() == title) {
        return true;
      } else {
        return false;
      }
    } else {
      return false;
    }
  });
  return l;
}

/**
 * search a meeting by username, time interval
 * @param userName as a sponsor OR a participator
 * @param startDate time interval's start date
 * @param endDate time interval's end date
 * @return a meeting list result
 */
std::list<Meeting> AgendaService::meetingQuery(
    const std::string &userName, const std::string &startDate,
    const std::string &endDate) const {
  std::list<Meeting> l;
  Date sTime, eTime;
  sTime = Date::stringToDate(startDate);
  eTime = Date::stringToDate(endDate);
  if (!Date::isValid(sTime)) return l;
  if (!Date::isValid(eTime)) return l;
  if (sTime > eTime) return l;
  l = m_storage->queryMeeting([&userName, &sTime, &eTime](const Meeting &a) {
    if (a.getSponsor() == userName || a.isParticipator(userName)) {
      if (a.getStartDate() <= sTime && a.getEndDate() >= sTime) return true;
      if (a.getStartDate() <= eTime && a.getEndDate() >= eTime) return true;
      if (a.getStartDate() >= sTime && a.getEndDate() <= eTime) return true;
      return false;
    } else {
      return false;
    }
  });
  return l;
}

/**
 * list all meetings the user take part in
 * @param userName user's username
 * @return a meeting list result
 */

std::list<Meeting> AgendaService::listAllMeetings(
    const std::string &userName) const {
  std::list<Meeting> l;
  l = m_storage->queryMeeting([&userName](const Meeting &a) {
    if (a.getSponsor() == userName || a.isParticipator(userName)) {
      return true;
    } else {
      return false;
    }
  });
  return l;
}

/**
 * list all meetings the user sponsor
 * @param userName user's username
 * @return a meeting list result
 */
std::list<Meeting> AgendaService::listAllSponsorMeetings(
    const std::string &userName) const {
  std::list<Meeting> l;
  l = m_storage->queryMeeting([&userName](const Meeting &a) {
    if (a.getSponsor() == userName) {
      return true;
    } else {
      return false;
    }
  });
  return l;
}

/**
 * list all meetings the user take part in and sponsor by other
 * @param userName user's username
 * @return a meeting list result
 */
std::list<Meeting> AgendaService::listAllParticipateMeetings(
    const std::string &userName) const {
  std::list<Meeting> l;
  l = m_storage->queryMeeting([&userName](const Meeting &a) {
    if (a.isParticipator(userName)) {
      return true;
    } else {
      return false;
    }
  });
  return l;
}

/**
 * delete a meeting by title and its sponsor
 * @param userName sponsor's username
 * @param title meeting's title
 * @return if delete this meeting success
 */
bool AgendaService::deleteMeeting(const std::string &userName,
                                  const std::string &title) {
  int ans = 0;
  ans = m_storage->deleteMeeting([&userName, &title](const Meeting &a) {
    if (a.getSponsor() == userName && a.getTitle() == title) {
      return true;
    } else {
      return false;
    }
  });
  if (ans > 0)
    return true;
  else
    return false;
}

/**
 * delete all meetings by sponsor
 * @param userName sponsor's username
 * @return if delete all meetings success
 */
bool AgendaService::deleteAllMeetings(const std::string &userName) {
  int ans = 0;
  ans = m_storage->deleteMeeting([&userName](const Meeting &a) {
    if (a.getSponsor() == userName) {
      return true;
    } else {
      return false;
    }
  });
  if (ans > 0)
    return true;
  else
    return false;
}