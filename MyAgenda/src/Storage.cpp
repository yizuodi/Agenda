#include <gtest/gtest.h>
#include "utility.h"
#include <iostream>
#include "Storage.hpp"
#include "Path.hpp"
#include <fstream>
#include <sstream>
#include <string>

using namespace std;
shared_ptr<Storage> Storage::m_instance = nullptr;

  /**
  *   default constructor
  */
  Storage::Storage(){
      m_dirty=false;
      readFromFile();
  }

  /**
  *   disallow the copy constructor and assign operator
  */
  /*Storage(const Storage &t_another) = delete;
  void operator=(const Storage &t_another) = delete;
  */

  /**
  *   read file content into memory
  *   @return if success, true will be returned
  */
 bool Storage::readFromFile(void){
      ifstream userfile(Path::userPath);
      if(!(userfile.is_open())){
          return false;
      }
      
      //open ok and user first (doge
      string user_str;
      //data: "name","123","123@123.com","123456"
      while(getline(userfile,user_str)){
          if(user_str==""){
              userfile.close();
              return false;
          }
          vector<string> user_v;
          istringstream iss(user_str);
          string tmp;
          while(getline(iss,tmp,',')){
              string temp;
              for(int i=0;i<tmp.size();++i){
                  if(tmp[i]!='"'){
                      temp.push_back(tmp[i]);
                  }
              }
              user_v.push_back(temp);
          }
          string name=user_v[0];
          string password=user_v[1];
          string email=user_v[2];
          string phone=user_v[3];
          User newuser(name,password,email,phone);
          m_userList.push_back(newuser);
      }
      userfile.close();
      
      ifstream meetingfile(Path::meetingPath);
      if(!(meetingfile.is_open())){
          return false;
      }
      string meeting_str;
      //data: "sponsor","participators","startDate","endDate","title"
      while(getline(meetingfile,meeting_str)){
          if(meeting_str==""){
              meetingfile.close();
              return false;
          }
          vector<string> meeting_v;
          istringstream iss(meeting_str);
          string tmp;
          while(getline(iss,tmp,',')){
              string temp;
              for(int i=0;i<tmp.size();++i){
                  if(tmp[i]!='"'){
                      temp.push_back(tmp[i]);
                  }
              }
              meeting_v.push_back(temp);
          }
          string sponsor=meeting_v[0];
          string participators=meeting_v[1];
          string startDatetmp=meeting_v[2];
          string endDatetmp=meeting_v[3];
          string title=meeting_v[4];
          //deal with participators start/enddate
          vector<string> participatorsv;
          stringstream ss(participators);
          string temp;
          while(getline(ss,temp,'&')){
              participatorsv.push_back(temp);
          }
          Date startDate(startDatetmp),endDate(endDatetmp);
          Meeting newmeeting(sponsor,participatorsv,startDate,endDate,title);
          m_meetingList.push_back(newmeeting);
      }
      meetingfile.close();
      return true;
  }

  /**
  *   write file content from memory
  *   @return if success, true will be returned
  */
  bool Storage::writeToFile(void){
      ofstream userfile(Path::userPath);
      if(!(userfile.is_open())){
          return false;
      }
      
      for(auto it=m_userList.begin();it!=m_userList.end();it++){
           userfile<<'"'<<(*it).getName()<<'"'<<','<<'"'<<(*it).getPassword()<<'"'<<','<<'"'<<(*it).getEmail()<<'"'<<','<<'"'<<(*it).getPhone()<<'"';
          userfile<<'\n';
      }
      userfile.close();
      
      ofstream meetingfile(Path::meetingPath);
      if(!(meetingfile.is_open())){
          return false;
      }
      for(auto it=m_meetingList.begin();it!=m_meetingList.end();it++){
          meetingfile<<'"'<<(*it).getSponsor()<<'"'<<','<<'"';
          vector<string> tmp=(*it).getParticipator();
          for(int i=0;i<tmp.size();i++){
              meetingfile<<tmp[i];
              if(i!=tmp.size()-1){
                  meetingfile<<'&';
              }
          }
          meetingfile<<'"'<<','<<'"'<<Date::dateToString((*it).getStartDate())<<'"'<<','<<'"'<<Date::dateToString((*it).getEndDate())<<'"'<<','<<'"'<<(*it).getTitle()<<'"';
          meetingfile<<'\n';
      }
      meetingfile.close();
      return true;
  }
  /**
  * get Instance of storage
  * @return the pointer of the instance
  */
  std::shared_ptr<Storage> Storage::getInstance(void){
      if(m_instance== nullptr){
          m_instance=shared_ptr<Storage>(new Storage());
      }
      return m_instance;
  }

  /**
  *   destructor
  */
  Storage::~Storage(){
      sync();
  }

  // CRUD for User & Meeting
  // using C++11 Function Template and Lambda Expressions

  /**
  * create a user
  * @param a user object
  */
  void Storage::createUser(const User &t_user){
      m_userList.push_back(t_user);
      m_dirty=true;
  }

  /**
  * query users
  * @param a lambda function as the filter
  * @return a list of fitted users
  */
  std::list<User> Storage::queryUser(std::function<bool(const User &)> filter) const{
      list<User> tmp;
      for(auto i = m_userList.begin();i!=m_userList.end();i++){
          if(filter(*i)){
              tmp.push_back(*i);
          }
      }
      return tmp;
  }

  /**
  * update users
  * @param a lambda function as the filter
  * @param a lambda function as the method to update the user
  * @return the number of updated users
  */
  int Storage::updateUser(std::function<bool(const User &)> filter,
                 std::function<void(User &)> switcher){
                     int count=0;
                     for (auto i = m_userList.begin(); i != m_userList.end(); i++)
                     {
                         if(filter(*i)){
                             switcher(*i);
                             count++;
                         }
                     }
                     if(count!=0){
                         m_dirty=true;
                     }
                     return count;
                 }

  /**
  * delete users
  * @param a lambda function as the filter
  * @return the number of deleted users
  */
  int Storage::deleteUser(std::function<bool(const User &)> filter){
      int count=0;
      for (auto i = m_userList.begin(); i != m_userList.end(); )
      {
          if(filter(*i)){
              i=m_userList.erase(i);
              ++count;
          }
          else i++;
      }
      if(count!=0){
          m_dirty=true;
      }
      return count;
  }

  /**
  * create a meeting
  * @param a meeting object
  */
  void Storage::createMeeting(const Meeting &t_meeting){
      m_meetingList.push_back(t_meeting);
      m_dirty=true;
  }

  /**
  * query meetings
  * @param a lambda function as the filter
  * @return a list of fitted meetings
  */
  std::list<Meeting> Storage::queryMeeting(
      std::function<bool(const Meeting &)> filter) const{
          list<Meeting> tmp;
          for(auto i=m_meetingList.begin();i!=m_meetingList.end();i++){
              if(filter(*i)){
                  tmp.push_back(*i);
              }
          }
          return tmp;
      }

  /**
  * update meetings
  * @param a lambda function as the filter
  * @param a lambda function as the method to update the meeting
  * @return the number of updated meetings
  */
  int Storage::updateMeeting(std::function<bool(const Meeting &)> filter,
                    std::function<void(Meeting &)> switcher){
                        int count=0;
                     for (list<Meeting>::iterator i=m_meetingList.begin(); i != m_meetingList.end(); i++)
                     {
                         if(filter(*i)){
                             switcher(*i);
                             ++count;
                         }
                     }
                     if(count!=0){
                         m_dirty=true;
                     }
                     return count;
                    }

  /**
  * delete meetings
  * @param a lambda function as the filter
  * @return the number of deleted meetings
  */
  int Storage::deleteMeeting(std::function<bool(const Meeting &)> filter){
      int count=0;
      for (auto i = m_meetingList.begin(); i != m_meetingList.end();)
      {
          if(filter(*i)){
              i=m_meetingList.erase(i);
              ++count;
          }
          else i++;
      }
      if(count!=0){
          m_dirty=true;
      }
      return count;
  }

  /**
  * sync with the file 数据同步,
  */
  bool Storage::sync(void){
      if (m_dirty==true){
          m_dirty=false;
          return writeToFile();
      }
      return true;
  }