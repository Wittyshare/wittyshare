/*
* Copyright (C) 2006-Today Guy Deleeuw
*
* See the LICENSE file for terms of use.
*/

#ifndef WS_OPTION_H__
#define WS_OPTION_H__    1


class WsOption {
public :
  WsOption(const std::string& attribute, const boost::any value);
  const std::string&                 attribute() const;
  void                               setValue(const boost::any& value);
  const boost::any&                  value() const;

private :
  std::string                        m_sAttribute;
  boost::any                         m_value;
};

class WsOptions {
public :
  // CTor
  WsOptions();
  WsOptions(const std::vector<WsOption>& vOptions);
  // Copy constructor
  WsOptions(const WsOptions& src);

  /*! \brief Set an options if previously set, update the value. */
  void                               setOption(const std::string& attribute, boost::any value);
  /*! \brief Set all options. */
  void                               setOptions(const std::vector<WsOption>& vOptions);
  /*! \brief Get an options value. */
  const boost::any&                  option(const std::string& attribute) const;
  /*! \brief Get all options. */
  const std::vector<WsOption>&       options() const;
  /*! \brief Output all options. */
  void                               outOptions(const std::string message);


private :
  std::vector<WsOption>              m_vOptions;
  boost::any                         m_noOptions;
};

#endif
