#include <gtest/gtest.h>
#include "User.hpp"
using namespace std;
  /**
  * @brief the default constructor
  */

  /**
  * constructor with arguments
  */
  User::User(const std::string &t_userName, const std::string &t_userPassword,
       const std::string &t_userEmail, const std::string &t_userPhone){
           this->m_name=t_userName;
           this->m_email=t_userEmail;
           this->m_phone=t_userPhone;
           this->m_password=t_userPassword;
       }
  User::User(const User &t_user){
           this->m_name=t_user.getName();
           this->m_email=t_user.getEmail();
           this->m_phone=t_user.getPhone();
           this->m_password=t_user.getPassword();

  }

  /**
  * @brief get the name of the user
  * @return   return a string indicate the name of the user
  */
  std::string User::getName() const{
      return this->m_name;
  }

  /**
  * @brief set the name of the user
  * @param   a string indicate the new name of the user
  */
  void User::setName(const std::string &t_name){
      this->m_name=t_name;
  }

  /**
  * @brief get the password of the user
  * @return   return a string indicate the password of the user
  */
  std::string User::getPassword() const{
      return this->m_password;
  }

  /**
  * @brief set the password of the user
  * @param   a string indicate the new password of the user
  */
  void User::setPassword(const std::string &t_password){
      this->m_password=t_password;
  }

  /**
  * @brief get the email of the user
  * @return   return a string indicate the email of the user
  */
  std::string User::getEmail() const{
      return this->m_email;
  }

  /**
  * @brief set the email of the user
  * @param   a string indicate the new email of the user
  */
  void User::setEmail(const std::string &t_email){
      this->m_email=t_email;
  }

  /**
  * @brief get the phone of the user
  * @return   return a string indicate the phone of the user
  */
  std::string User::getPhone() const{
      return this->m_phone;
  }

  /**
  * @brief set the phone of the user
  * @param   a string indicate the new phone of the user
  */
  void User::setPhone(const std::string &t_phone){
      this->m_phone=t_phone;
  }