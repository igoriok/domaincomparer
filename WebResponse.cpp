#include "WebResponse.h"

WebResponse::WebResponse():
        m_null(true)
{
}

WebResponse::WebResponse(int code, const QString & type, const QUrl & location, const QByteArray & data, qlonglong length):
        m_code(code), m_type(type), m_location(location), m_data(data), m_length(length), m_null(false)
{
}

void WebResponse::clear()
{
    m_code = 0;
    m_type.clear();
    m_location.clear();
    m_data.clear();
    m_length = 0;
    m_null = true;
}
