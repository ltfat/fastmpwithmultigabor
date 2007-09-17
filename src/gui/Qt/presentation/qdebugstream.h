#ifndef QDEBUGSTREAM_H_
#define QDEBUGSTREAM_H_

#include <iostream>
#include <streambuf>
#include <string>
#include "main_window.h"

class QDebugStream : public std::basic_streambuf<char>
{
public:
 QDebugStream(std::ostream &stream, MainWindow* main) : m_stream(stream)
 {
 log_window = main;	
  m_old_buf = stream.rdbuf();
  stream.rdbuf(this);
 }
 ~QDebugStream()
 {
  // output anything that is left
  if (!m_string.empty())
   log_window->readFromStdout(QString(m_string.c_str()));

  m_stream.rdbuf(m_old_buf);
 }

protected:
 virtual int_type overflow(int_type v)
 {
  if (v == '\n')
  {
   log_window->readFromStdout(QString(m_string.c_str()));
   m_string.erase(m_string.begin(), m_string.end());
  }
  else
   m_string += v;

  return v;
 }

 virtual std::streamsize xsputn(const char *p, std::streamsize n)
 {
  m_string.append(p, p + n);

  unsigned int pos = 0;
  while (pos != std::string::npos)
  {
   pos = m_string.find('\n');
   if (pos != std::string::npos)
   {
    std::string tmp(m_string.begin(), m_string.begin() + pos);
    log_window->readFromStdout(QString(tmp.c_str()));
    m_string.erase(m_string.begin(), m_string.begin() + pos + 1);
   }
  }

  return n;
 }

private:
 std::ostream &m_stream;
 std::streambuf *m_old_buf;
 std::string m_string;
 MainWindow* log_window;
};

#endif /*QDEBUGSTREAM_H_*/
