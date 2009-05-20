#include "WebResponse.h"

WebResponse::WebResponse():
        m_null(true)
{
}

WebResponse::WebResponse(const QString & type, int code, const QByteArray & data, qlonglong length):
        m_type(type), m_code(code), m_data(data), m_length(length), m_null(false)
{
}

void WebResponse::clear()
{
    m_type.clear();
    m_code = 0;
    m_data.clear();
    m_length = 0;
    m_null = true;
}
